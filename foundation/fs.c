/* fs.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#include <foundation/foundation.h>
#include <foundation/internal.h>

#if FOUNDATION_PLATFORM_WINDOWS
#include <foundation/windows.h>
#include <sys/utime.h>
#include <io.h>
#include <fcntl.h>
#elif FOUNDATION_PLATFORM_POSIX
#include <foundation/posix.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <utime.h>
#include <fcntl.h>
#include <dirent.h>
#endif

#if FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
#if FOUNDATION_COMPILER_GCC
#pragma GCC diagnostic push
#if FOUNDATION_GCC_VERSION > 40700
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#elif FOUNDATION_COMPILER_CLANG
#if __has_warning("-Wpedantic")
#pragma clang diagnostic ignored "-Wpedantic"
#endif
#endif
#include <sys/inotify.h>
#if FOUNDATION_COMPILER_GCC
#pragma GCC diagnostic pop
#endif
#endif

#include <stdio.h>

#if FOUNDATION_PLATFORM_WINDOWS
typedef HANDLE fs_file_descriptor;
#define FD_VALID(x) ((x) != INVALID_HANDLE_VALUE)
#else
typedef FILE* fs_file_descriptor;
#define FD_VALID(x) ((x) != 0)
#endif

struct fs_monitor_t {
	string_t path;
	bool inuse;
#if FOUNDATION_PLATFORM_MACOS
	void* stream;
#else
	thread_t thread;
#endif
};

struct stream_file_t {
	/*lint -e830 -e754 It is used, through stream type */
	FOUNDATION_DECLARE_STREAM;
	fs_file_descriptor fd;
};

typedef FOUNDATION_ALIGN(16) struct fs_monitor_t fs_monitor_t;
typedef FOUNDATION_ALIGN(8) struct stream_file_t stream_file_t;

#define GET_FILE(s) ((stream_file_t*)(s))
#define GET_FILE_CONST(s) ((const stream_file_t*)(s))
#define GET_STREAM(f) ((stream_t*)(f))

static stream_vtable_t fs_file_vtable;

static mutex_t* fs_monitor_lock;
static fs_monitor_t* fs_monitors;
static event_stream_t* fs_event_stream_current;

#if FOUNDATION_PLATFORM_WINDOWS || FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID || FOUNDATION_PLATFORM_MACOS
#define FOUNDATION_HAVE_FS_MONITOR 1
static void*
fs_monitor_thread(void*);
#else
#define FOUNDATION_HAVE_FS_MONITOR 0
#endif

#if FOUNDATION_PLATFORM_MACOS

extern void*
fs_event_stream_create(const char* path, size_t length);

extern void
fs_event_stream_destroy(void* stream);

#endif

static string_const_t
fs_strip_protocol(const char* path, size_t length) {
	string_const_t stripped = path_strip_protocol(path, length);
	ptrdiff_t diff = pointer_diff(stripped.str, path);
	if (!diff)
		return stripped;
	if (((diff == 6) || (diff == 7)) && string_equal(path, 6, STRING_CONST("file:/")))
		return stripped;
	return string_empty();
}

bool
fs_monitor(const char* path, size_t length) {
	bool ret = false;
#if FOUNDATION_HAVE_FS_MONITOR
	size_t mi;
	char buf[BUILD_MAX_PATHLEN];
	string_t path_clone;

	mutex_lock(fs_monitor_lock);

	for (mi = 0; mi < foundation_config().fs_monitor_max; ++mi) {
		if (fs_monitors[mi].inuse && string_equal(STRING_ARGS(fs_monitors[mi].path), path, length)) {
			mutex_unlock(fs_monitor_lock);
			return true;
		}
	}

	memory_context_push(HASH_STREAM);

	path_clone = string_copy(buf, BUILD_MAX_PATHLEN, path, length);
	path_clone = path_clean(STRING_ARGS(path_clone), BUILD_MAX_PATHLEN);
	path_clone = path_absolute(STRING_ARGS(path_clone), BUILD_MAX_PATHLEN);
	path_clone = string_clone(STRING_ARGS(path_clone));

	for (mi = 0; mi < foundation_config().fs_monitor_max; ++mi) {
		if (!fs_monitors[mi].inuse) {
			fs_monitors[mi].inuse = true;
			fs_monitors[mi].path = path_clone;
#if FOUNDATION_PLATFORM_MACOS
			// On macOS file system monitors are run in system dispatch
			fs_monitor_thread(fs_monitors + mi);
#else
			thread_initialize(&fs_monitors[mi].thread, fs_monitor_thread, fs_monitors + mi, STRING_CONST("fs_monitor"),
			                  THREAD_PRIORITY_BELOWNORMAL, 0);
			thread_start(&fs_monitors[mi].thread);
#endif
			ret = true;
			break;
		}
	}

	if (mi == foundation_config().fs_monitor_max) {
		string_deallocate(path_clone.str);
		log_errorf(0, ERROR_OUT_OF_MEMORY, STRING_CONST("Unable to monitor file system, no free monitor slots: %.*s"),
		           (int)length, path);
	}

	memory_context_pop();
	mutex_unlock(fs_monitor_lock);

#else
	FOUNDATION_UNUSED(path);
	FOUNDATION_UNUSED(length);
#endif
	return ret;
}

static void
fs_stop_monitor(fs_monitor_t* monitor) {
	if (!monitor->inuse)
		return;

#if FOUNDATION_PLATFORM_MACOS
	if (monitor->stream)
		fs_event_stream_destroy(monitor->stream);
#else
	thread_signal(&monitor->thread);
	thread_finalize(&monitor->thread);
#endif
	string_deallocate(monitor->path.str);
	monitor->inuse = false;
}

void
fs_unmonitor(const char* path, size_t length) {
	mutex_lock(fs_monitor_lock);

	if (fs_monitors) {
		for (size_t mi = 0; mi < foundation_config().fs_monitor_max; ++mi) {
			if (fs_monitors[mi].inuse && string_equal(STRING_ARGS(fs_monitors[mi].path), path, length))
				fs_stop_monitor(fs_monitors + mi);
		}
	}

	mutex_unlock(fs_monitor_lock);
}

bool
fs_is_file(const char* path, size_t length) {
	return fs_stat(path, length).is_file;
}

bool
fs_is_directory(const char* path, size_t length) {
	return fs_stat(path, length).is_directory;
}

string_t*
fs_subdirs(const char* path, size_t length) {
	string_t* arr = 0;
#if FOUNDATION_PLATFORM_WINDOWS

	// Windows specific implementation of directory listing
	char buffer[BUILD_MAX_PATHLEN];
	string_t cleanpath = string_copy(buffer, sizeof(buffer) - 2, path, length);

	if (cleanpath.length && (cleanpath.str[cleanpath.length - 1] != '/'))
		cleanpath.str[cleanpath.length++] = '/';
	cleanpath.str[cleanpath.length++] = '*';
	cleanpath.str[cleanpath.length] = 0;

	WIN32_FIND_DATAA data;
	HANDLE find = FindFirstFileA(cleanpath.str, &data);
	if (find != INVALID_HANDLE_VALUE) {
		do {
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				string_t filename;
				if (data.cFileName[0] == '.') {
					if (!data.cFileName[1] || (data.cFileName[1] == '.'))
						continue;  // Don't include . and .. directories
				}
				filename = string_clone(data.cFileName, string_length(data.cFileName));
				array_push(arr, filename);
			}
		} while (FindNextFileA(find, &data));
	}
	FindClose(find);

#elif FOUNDATION_PLATFORM_POSIX

	// POSIX specific implementation of directory listing
	char foundpath_buffer[BUILD_MAX_PATHLEN];
	string_t cleanpath = string_copy(foundpath_buffer, sizeof(foundpath_buffer), path, length);
	DIR* dir = opendir(cleanpath.str);
	if (dir) {
		struct dirent* entry = 0;
		struct stat st;

		memory_context_push(HASH_STREAM);

		while ((entry = readdir(dir)) != 0) {
			if (entry->d_name[0] == '.') {
				if (!entry->d_name[1] || (entry->d_name[1] == '.'))
					continue;  // Don't include . and .. directories
			}
			size_t entrylen = string_length(entry->d_name);
			string_t thispath =
			    path_append(foundpath_buffer, length, sizeof(foundpath_buffer), entry->d_name, entrylen);
			if (!stat(thispath.str, &st) && S_ISDIR(st.st_mode))
				array_push(arr, string_clone(entry->d_name, entrylen));
		}

		closedir(dir);

		memory_context_pop();
	}

#else
#error Not implemented
#endif

	return arr;
}

string_t*
fs_files(const char* path, size_t length) {
	string_t* arr = 0;
#if FOUNDATION_PLATFORM_WINDOWS

	// Windows specific implementation of directory listing
	char buffer[BUILD_MAX_PATHLEN];
	string_t cleanpath = string_copy(buffer, sizeof(buffer) - 2, path, length);

	if (cleanpath.length && (cleanpath.str[cleanpath.length - 1] != '/'))
		cleanpath.str[cleanpath.length++] = '/';
	cleanpath.str[cleanpath.length++] = '*';
	cleanpath.str[cleanpath.length] = 0;

	WIN32_FIND_DATAA data;
	HANDLE find = FindFirstFileA(cleanpath.str, &data);
	if (find != INVALID_HANDLE_VALUE) {
		do {
			if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				string_t filename;
				if (data.cFileName[0] == '.') {
					if (!data.cFileName[1] || (data.cFileName[1] == '.'))
						continue;  // Don't include . and .. directories
				}
				filename = string_clone(data.cFileName, string_length(data.cFileName));
				array_push(arr, filename);
			}
		} while (FindNextFileA(find, &data));
	}
	FindClose(find);

#elif FOUNDATION_PLATFORM_POSIX

	// POSIX specific implementation of directory listing
	char localpath[BUILD_MAX_PATHLEN];
	string_copy(localpath, sizeof(localpath), path, length);
	DIR* dir = opendir(localpath);
	if (dir) {
		// We have a directory, parse and create virtual file system
		struct dirent* entry = 0;
		struct stat st;

		memory_context_push(HASH_STREAM);

		while ((entry = readdir(dir)) != 0) {
			size_t entrylen = string_length(entry->d_name);
			string_t thispath = path_append(localpath, length, sizeof(localpath), entry->d_name, entrylen);
			if (!stat(thispath.str, &st) && S_ISREG(st.st_mode))
				array_push(arr, string_clone(entry->d_name, entrylen));
		}

		closedir(dir);

		memory_context_pop();
	}

#else
#error Not implemented
#endif

	return arr;
}

bool
fs_move_file(const char* path_source, size_t length_source, const char* path_dest, size_t length_dest) {
	bool result;
	string_const_t fspath_source, fspath_dest;

	fspath_source = fs_strip_protocol(path_source, length_source);
	if (!fspath_source.length)
		return false;
	fspath_dest = fs_strip_protocol(path_dest, length_dest);
	if (!fspath_dest.length)
		return false;

#if FOUNDATION_PLATFORM_WINDOWS

	wchar_t* wpath_source = wstring_allocate_from_string(fspath_source.str, fspath_source.length);
	wchar_t* wpath_dest = wstring_allocate_from_string(fspath_dest.str, fspath_dest.length);
	result = MoveFileW(wpath_source, wpath_dest);
	wstring_deallocate(wpath_source);
	wstring_deallocate(wpath_dest);

#elif FOUNDATION_PLATFORM_POSIX

	char buffer_source[BUILD_MAX_PATHLEN];
	char buffer_dest[BUILD_MAX_PATHLEN];
	string_t finalpath_source = string_copy(buffer_source, sizeof(buffer_source), STRING_ARGS(fspath_source));
	string_t finalpath_dest = string_copy(buffer_dest, sizeof(buffer_dest), STRING_ARGS(fspath_dest));
	result = (rename(finalpath_source.str, finalpath_dest.str) == 0);

#else
#error Not implemented
#endif

	return result;
}

void
fs_set_mode(const char* path, size_t length, uint mode) {
	string_const_t fspath = fs_strip_protocol(path, length);
	if (!fspath.length)
		return;

#if FOUNDATION_PLATFORM_WINDOWS

	wchar_t* wpath = wstring_allocate_from_string(fspath.str, fspath.length);
	if (mode & 0200)
		SetFileAttributesW(wpath, FILE_ATTRIBUTE_ARCHIVE);
	else
		SetFileAttributesW(wpath, FILE_ATTRIBUTE_READONLY);
	wstring_deallocate(wpath);

#elif FOUNDATION_PLATFORM_POSIX

	char buffer[BUILD_MAX_PATHLEN];
	string_t finalpath = string_copy(buffer, sizeof(buffer), STRING_ARGS(fspath));
	chmod(finalpath.str, (mode_t)mode);

#else
#error Not implemented
#endif
}

bool
fs_remove_file(const char* path, size_t length) {
	bool result;
	string_const_t fspath;
#if FOUNDATION_PLATFORM_WINDOWS
	wchar_t* wpath;
#endif

	fspath = fs_strip_protocol(path, length);
	if (!fspath.length)
		return false;

	fs_stat_t stat = fs_stat(STRING_ARGS(fspath));
	if (!stat.is_file)
		return false;

#if FOUNDATION_PLATFORM_WINDOWS

	wpath = wstring_allocate_from_string(fspath.str, fspath.length);
	SetFileAttributesW(wpath, FILE_ATTRIBUTE_NORMAL);
	result = DeleteFileW(wpath);
	wstring_deallocate(wpath);

#elif FOUNDATION_PLATFORM_POSIX

	char buffer[BUILD_MAX_PATHLEN];
	string_t finalpath = string_copy(buffer, sizeof(buffer), STRING_ARGS(fspath));

	chmod(finalpath.str, 0664);
	result = (unlink(finalpath.str) == 0);

#else
#error Not implemented
#endif

	return result;
}

bool
fs_remove_directory(const char* path, size_t length) {
	/*lint --e{438,613,838} Lint gets seriously confused about the arrays here */
	bool result = false;
	string_t* subpaths;
	string_t* subfiles;
	string_const_t abspath, fspath;
	string_t localpath = {0, 0};
	char abspath_buffer[BUILD_MAX_PATHLEN];
	size_t fspathofs;
	size_t remain;
#if FOUNDATION_PLATFORM_WINDOWS
	wchar_t* wfpath = 0;
#endif
	size_t i, num;

	localpath = string_copy(abspath_buffer, BUILD_MAX_PATHLEN, path, length);
	abspath = string_to_const(localpath);

	fspath = fs_strip_protocol(STRING_ARGS(abspath));
	if (!fs_is_directory(STRING_ARGS(fspath)))
		goto end;

	fspathofs = (size_t)pointer_diff(fspath.str, abspath_buffer);
	remain = BUILD_MAX_PATHLEN - fspathofs;
	subpaths = fs_subdirs(STRING_ARGS(fspath));
	for (i = 0, num = array_size(subpaths); i < num; ++i) {
		localpath = path_append(abspath_buffer + fspathofs, fspath.length, remain, STRING_ARGS(subpaths[i]));
		fs_remove_directory(STRING_ARGS(localpath));
	}
	string_array_deallocate(subpaths);

	subfiles = fs_files(STRING_ARGS(fspath));
	for (i = 0, num = array_size(subfiles); i < num; ++i) {
		localpath = path_append(abspath_buffer + fspathofs, fspath.length, remain, STRING_ARGS(subfiles[i]));
		fs_remove_file(STRING_ARGS(localpath));
	}
	string_array_deallocate(subfiles);

#if FOUNDATION_PLATFORM_WINDOWS

	wfpath = wstring_allocate_from_string(fspath.str, fspath.length);
	result = RemoveDirectoryW(wfpath);
	wstring_deallocate(wfpath);

#elif FOUNDATION_PLATFORM_POSIX

	// Re-terminate string at base path
	abspath_buffer[fspathofs + fspath.length] = 0;
	result = (rmdir(abspath_buffer) == 0);

#else
#error Not implemented
#endif

end:

	return result;
}

bool
fs_make_directory(const char* path, size_t length) {
	bool result = false;
	char abspath_buffer[BUILD_MAX_PATHLEN];
	size_t offset;
	string_t localpath;
	string_const_t fspath;

	localpath = string_copy(abspath_buffer, sizeof(abspath_buffer), path, length);
	fspath = fs_strip_protocol(STRING_ARGS(localpath));
	if (!fspath.length)
		return true;
	localpath = (string_t){localpath.str + pointer_diff(fspath.str, localpath.str), fspath.length};
	offset = 1;

#if FOUNDATION_PLATFORM_WINDOWS
	if ((localpath.length > 2) && (localpath.str[1] == ':'))
		offset += 2;  // Drive letter
#endif

	do {
		offset = string_find(STRING_ARGS(localpath), '/', offset);
		if (offset != STRING_NPOS) {
			localpath.str[offset] = 0;
			localpath.length = offset;
		}
		if (!fs_is_directory(STRING_ARGS(localpath))) {
#if FOUNDATION_PLATFORM_WINDOWS
			wchar_t* wpath = wstring_allocate_from_string(STRING_ARGS(localpath));
			result = CreateDirectoryW(wpath, 0);
			wstring_deallocate(wpath);
#elif FOUNDATION_PLATFORM_POSIX
			mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH;
			result = (mkdir(localpath.str, mode) == 0);
#else
#error Not implemented
#endif
			if (!result) {
				if (!fs_is_directory(STRING_ARGS(localpath))) {
					int err = system_error();
					string_const_t errmsg = system_error_message(err);
					log_warnf(0, WARNING_SUSPICIOUS, STRING_CONST("Failed to create directory '%.*s': %.*s (%d)"),
					          STRING_FORMAT(localpath), STRING_FORMAT(errmsg), err);
					goto end;
				}
			}
		} else {
			result = true;
		}

		if (offset != STRING_NPOS) {
			localpath.str[offset] = '/';
			localpath.length = fspath.length;
			++offset;
		}
	} while (offset < localpath.length);

end:

	return result;
}

bool
fs_copy_file(const char* source, size_t srclen, const char* dest, size_t destlen) {
	stream_t* infile;
	stream_t* outfile;
	void* buffer;
	string_const_t destpath;

	infile = fs_open_file(source, srclen, STREAM_IN | STREAM_BINARY);
	if (!infile)
		return false;

	destpath = path_directory_name(dest, destlen);
	if (destpath.length)
		fs_make_directory(STRING_ARGS(destpath));

	outfile = fs_open_file(dest, destlen, STREAM_OUT | STREAM_BINARY | STREAM_CREATE | STREAM_TRUNCATE);
	if (!outfile) {
		stream_deallocate(infile);
		return false;
	}

	buffer = memory_allocate(0, 64 * 1024, 0, MEMORY_TEMPORARY);

	while (!stream_eos(infile)) {
		size_t numread = stream_read(infile, buffer, 64 * 1024);
		if (numread > 0)
			stream_write(outfile, buffer, numread);
	}

	memory_deallocate(buffer);
	stream_deallocate(infile);
	stream_deallocate(outfile);

	return true;
}

tick_t
fs_last_modified(const char* path, size_t length) {
	return (tick_t)fs_stat(path, length).last_modified;
}

size_t
fs_size(const char* path, size_t length) {
	return fs_stat(path, length).size;
}

fs_stat_t
fs_stat(const char* path, size_t length) {
	fs_stat_t fsstat = {0};
#if FOUNDATION_PLATFORM_WINDOWS
	WIN32_FILE_ATTRIBUTE_DATA attrib;
	memset(&attrib, 0, sizeof(attrib));

	BOOL success = 0;
	string_const_t cpath = fs_strip_protocol(path, length);
	if (cpath.length) {
		/*
		wchar_t* wpath = wstring_allocate_from_string(STRING_ARGS(cpath));
		success = GetFileAttributesExW(wpath, GetFileExInfoStandard, &attrib);
		wstring_deallocate(wpath);
		*/
		char buffer[BUILD_MAX_PATHLEN];
		string_t cleanpath = string_copy(buffer, sizeof(buffer), path, length);
		success = GetFileAttributesExA(cleanpath.str, GetFileExInfoStandard, &attrib);
	}
	if (success) {
		fsstat.size = ((uint64_t)attrib.nFileSizeHigh << 32ULL) + attrib.nFileSizeLow;
		fsstat.is_valid = true;
		fsstat.is_directory = (attrib.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		fsstat.is_file = !fsstat.is_directory;
		// Set some reasonable defaults for file mode since Windows does not have it
		if (fsstat.is_directory)
			fsstat.mode = (attrib.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? 0555 : 0755;
		else
			fsstat.mode = (attrib.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? 0444 : 0644;

		// This is retarded beyond belief, Microsoft decided that "100-nanosecond intervals since 1 Jan
		// 1601" was a nice basis for a timestamp... wtf? Anyway, number of such intervals to base date
		// for unix time, 1 Jan 1970, is 116444736000000000
		const uint64_t ms_offset_time = 116444736000000000ULL;
		uint64_t high_time = (uint64_t)attrib.ftLastWriteTime.dwHighDateTime;
		uint64_t low_time = (uint64_t)attrib.ftLastWriteTime.dwLowDateTime;
		uint64_t last_write_time = ((high_time << 32ULL) + low_time);
		fsstat.last_modified = (last_write_time > ms_offset_time) ? ((last_write_time - ms_offset_time) / 10000LL) : 0;
	}
#else
	string_const_t fspath = fs_strip_protocol(path, length);
	if (fspath.length) {
		char buffer[BUILD_MAX_PATHLEN];
		string_t finalpath = string_copy(buffer, sizeof(buffer), STRING_ARGS(fspath));
#if FOUNDATION_PLATFORM_MACOS
		struct stat st;
		if (stat(finalpath.str, &st) == 0) {
#else
		struct stat64 st;
		if (stat64(finalpath.str, &st) == 0) {
#endif
			fsstat.size = (uint64_t)st.st_size;
			fsstat.last_modified = (uint64_t)st.st_mtime * 1000LL;
			fsstat.mode = st.st_mode & 0777;
			fsstat.is_valid = true;
			fsstat.is_file = S_ISREG(st.st_mode);
			fsstat.is_directory = S_ISDIR(st.st_mode);
		}
	}
#endif
	return fsstat;
}

uint128_t
fs_md5(const char* path, size_t length) {
	uint128_t digest = uint128_null();
	stream_t* file = fs_open_file(path, length, STREAM_IN | STREAM_BINARY);
	if (file) {
		digest = stream_md5(file);
		stream_deallocate(file);
	}
	return digest;
}

void
fs_touch(const char* path, size_t length) {
#if FOUNDATION_PLATFORM_WINDOWS
	string_const_t cpath = fs_strip_protocol(path, length);
	if (cpath.length) {
		wchar_t* wpath = wstring_allocate_from_string(STRING_ARGS(cpath));
		_wutime(wpath, 0);
		wstring_deallocate(wpath);
	}
#elif FOUNDATION_PLATFORM_POSIX
	string_const_t fspath = fs_strip_protocol(path, length);
	if (fspath.length) {
		char buffer[BUILD_MAX_PATHLEN];
		string_t finalpath = string_copy(buffer, sizeof(buffer), STRING_ARGS(fspath));
		utime(finalpath.str, 0);
	}
#else
#error Not implemented
#endif
}

stream_t*
fs_temporary_file(void) {
	char buf[BUILD_MAX_PATHLEN];
	string_t filename = path_make_temporary(buf, BUILD_MAX_PATHLEN);
	string_const_t directory = path_directory_name(filename.str, filename.length);

	fs_make_directory(directory.str, directory.length);
	return fs_open_file(filename.str, filename.length,
	                    STREAM_IN | STREAM_OUT | STREAM_BINARY | STREAM_CREATE | STREAM_TRUNCATE);
}

static string_t*
fs_matching_name_regex(const char* path, size_t length, regex_t* pattern, bool recurse, bool isfile) {
	string_t* names = 0;
	string_t* subdirs;
	string_t localpath;
	size_t id, dsize, in, nsize, capacity;

#if FOUNDATION_PLATFORM_WINDOWS

	// Windows specific implementation of directory listing
	WIN32_FIND_DATAW data;
	char filename[BUILD_MAX_PATHLEN];
	wchar_t* wpattern;
	size_t wsize = length;
	capacity = length + 4;

	memory_context_push(HASH_STREAM);

	wpattern = memory_allocate(0, sizeof(wchar_t) * capacity, 0, MEMORY_TEMPORARY);
	wstring_from_string(wpattern, capacity, path, length);
	if (length && (path[length - 1] != '/'))
		wpattern[wsize++] = L'/';
	wpattern[wsize++] = L'*';
	wpattern[wsize] = 0;

	HANDLE find = FindFirstFileW(wpattern, &data);

	if (find != INVALID_HANDLE_VALUE)
		do {
			bool fileattr = ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
			if (isfile == fileattr) {
				string_t filestr = string_convert_utf16(filename, BUILD_MAX_PATHLEN, (const uint16_t*)data.cFileName,
				                                        wstring_length(data.cFileName));
				if (regex_match(pattern, STRING_ARGS(filestr), 0, 0)) {
					filestr = string_clone(STRING_ARGS(filestr));
					array_push(names, path_clean(STRING_ARGS(filestr), false));
				}
			}
		} while (FindNextFileW(find, &data));

	memory_deallocate(wpattern);
	memory_context_pop();

	FindClose(find);

#else

	string_t* fnames = isfile ? fs_files(path, length) : fs_subdirs(path, length);

	memory_context_push(HASH_STREAM);

	for (in = 0, nsize = array_size(fnames); in < nsize; ++in) {
		if (regex_match(pattern, STRING_ARGS(fnames[in]), 0, 0)) {
			array_push(names, fnames[in]);
			fnames[in] = (string_t){0, 0};
		}
	}

	memory_context_pop();

	string_array_deallocate(fnames);

#endif

	if (!recurse)
		return names;

	subdirs = fs_subdirs(path, length);

	memory_context_push(HASH_STREAM);

	capacity = BUILD_MAX_PATHLEN;
	localpath = string_allocate(0, capacity);
	localpath = string_copy(localpath.str, BUILD_MAX_PATHLEN, path, length);

	/*lint --e{438,613,838} Lint gets seriously confused about the arrays here */
	for (id = 0, dsize = array_size(subdirs); id < dsize; ++id) {
		string_t* subnames;
		localpath = path_append(localpath.str, length, capacity, STRING_ARGS(subdirs[id]));
		subnames = fs_matching_files_regex(STRING_ARGS(localpath), pattern, true);

		for (in = 0, nsize = array_size(subnames); in < nsize; ++in)
			array_push(names, path_allocate_concat(STRING_ARGS(subdirs[id]), STRING_ARGS(subnames[in])));

		string_array_deallocate(subnames);
	}

	string_deallocate(localpath.str);
	string_array_deallocate(subdirs);

	memory_context_pop();

	return names;
}

string_t*
fs_matching_files_regex(const char* path, size_t length, regex_t* pattern, bool recurse) {
	return fs_matching_name_regex(path, length, pattern, recurse, true);
}

string_t*
fs_matching_files(const char* path, size_t length, const char* pattern, size_t pattern_length, bool recurse) {
	regex_t* regex = regex_compile(pattern, pattern_length);
	string_t* names = fs_matching_files_regex(path, length, regex, recurse);
	regex_deallocate(regex);
	return names;
}

string_t*
fs_matching_subdirs_regex(const char* path, size_t length, regex_t* pattern, bool recurse) {
	return fs_matching_name_regex(path, length, pattern, recurse, false);
}

string_t*
fs_matching_subdirs(const char* path, size_t length, const char* pattern, size_t pattern_length, bool recurse) {
	regex_t* regex = regex_compile(pattern, pattern_length);
	string_t* names = fs_matching_subdirs_regex(path, length, regex, recurse);
	regex_deallocate(regex);
	return names;
}

void
fs_event_post(foundation_event_id id, const char* path, size_t pathlen) {
	event_post_varg(fs_event_stream(), (int)id, 0, 0, &pathlen, sizeof(pathlen), path, pathlen, nullptr);
}

string_const_t
fs_event_path(const event_t* event) {
	return string_const(pointer_offset_const(event->payload, sizeof(size_t)), event->payload[0]);
}

event_stream_t*
fs_event_stream(void) {
	return fs_event_stream_current;
}

#if FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID

struct fs_watch_t {
	int fd;
	string_t path;
};

typedef struct fs_watch_t fs_watch_t;

static void
fs_send_creations(char* path, size_t length, size_t capacity) {
	size_t ifile, isub, fsize, subsize;

	string_t* files = fs_files(path, length);
	for (ifile = 0, fsize = array_size(files); ifile < fsize; ++ifile) {
		string_t filepath = path_append(path, length, capacity, STRING_ARGS(files[ifile]));
		fs_event_post(FOUNDATIONEVENT_FILE_CREATED, STRING_ARGS(filepath));
	}
	string_array_deallocate(files);

	string_t* subdirs = fs_subdirs(path, length);
	for (isub = 0, subsize = array_size(subdirs); isub < subsize; ++isub) {
		string_t subpath = path_append(path, length, capacity, STRING_ARGS(subdirs[isub]));
		fs_send_creations(STRING_ARGS(subpath), capacity);
	}
	string_array_deallocate(subdirs);
}

static void
fs_add_notify_subdir(int notify_fd, char* path, size_t length, size_t capacity, fs_watch_t** watch_arr,
                     string_t** path_arr, bool send_create) {
	string_t* subdirs = 0;
	string_t local_path;
	string_t stored_path;
	int fd = inotify_add_watch(notify_fd, path, IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVE);
	if (fd < 0) {
		log_warnf(0, WARNING_SYSTEM_CALL_FAIL, STRING_CONST("Failed watching subdir: %.*s (%d)"), (int)length, path,
		          fd);
		return;
	}

	if (send_create)
		fs_send_creations(path, length, capacity);

	// Include terminating / in paths stored in path_arr/watch_arr
	local_path = string_append(path, length, capacity, STRING_CONST("/"));
	stored_path = string_clone(STRING_ARGS(local_path));
	array_push((*path_arr), stored_path);

	fs_watch_t watch;
	watch.fd = fd;
	watch.path = stored_path;
	array_push((*watch_arr), watch);

	// Recurse
	subdirs = fs_subdirs(STRING_ARGS(local_path));
	for (size_t i = 0, size = array_size(subdirs); i < size; ++i) {
		string_t subpath = string_append(STRING_ARGS(local_path), capacity, STRING_ARGS(subdirs[i]));
		fs_add_notify_subdir(notify_fd, STRING_ARGS(subpath), capacity, watch_arr, path_arr, send_create);
	}
	string_array_deallocate(subdirs);
}

static fs_watch_t*
fs_lookup_watch(fs_watch_t* watch_arr, int fd) {
	// TODO: If array is kept sorted on fd, this could be made faster
	for (size_t i = 0, size = array_size(watch_arr); i < size; ++i) {
		if (watch_arr[i].fd == fd)
			return watch_arr + i;
	}
	return 0;
}

#endif

#if FOUNDATION_HAVE_FS_MONITOR

static void*
fs_monitor_thread(void* monitorptr) {
	fs_monitor_t* monitor = monitorptr;
	bool keep_running = true;

#if FOUNDATION_PLATFORM_WINDOWS

	DWORD buffer_size = 63 * 1024;
	DWORD out_size = 0;
	OVERLAPPED overlap;
	BOOL success = FALSE;
	HANDLE dir = 0;
	wchar_t* wfpath = 0;
	void* buffer;
	HANDLE handle;
	int event;
	int wait_status;
	beacon_t* beacon = &thread_self()->beacon;

	memory_context_push(HASH_STREAM);

	buffer = memory_allocate(0, buffer_size, 8, MEMORY_PERSISTENT);
	handle = CreateEvent(0, FALSE, FALSE, 0);
	if (handle == INVALID_HANDLE_VALUE) {
		string_const_t errstr = system_error_message(0);
		log_warnf(0, WARNING_SUSPICIOUS, STRING_CONST("Unable to create event to monitor path: %.*s : %.*s"),
		          STRING_FORMAT(monitor->path), STRING_FORMAT(errstr));
		goto exit_thread;
	}

	event = beacon_add_handle(beacon, handle);

#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID

	char pathbuffer[BUILD_MAX_PATHLEN];
	string_t local_path;
	int notify_fd = inotify_init();
	fs_watch_t* watch = 0;
	string_t* paths = 0;
	beacon_t* beacon = &thread_self()->beacon;

	memory_context_push(HASH_STREAM);

	array_reserve(watch, 1024);

	// Recurse and add all subdirs
	local_path = string_copy(pathbuffer, sizeof(pathbuffer), STRING_ARGS(monitor->path));
	fs_add_notify_subdir(notify_fd, STRING_ARGS(local_path), sizeof(pathbuffer), &watch, &paths, false);

	beacon_add_fd(beacon, notify_fd);

#elif FOUNDATION_PLATFORM_MACOS

	memory_context_push(HASH_STREAM);

	monitor->stream = fs_event_stream_create(STRING_ARGS(monitor->path));

#else

	memory_context_push(HASH_STREAM);

#endif

	// log_debugf(0, STRING_CONST("Monitoring file system: %.*s"),
	//           STRING_FORMAT(monitor->path));

#if FOUNDATION_PLATFORM_WINDOWS
	{
		wfpath = wstring_allocate_from_string(STRING_ARGS(monitor->path));
		dir = CreateFileW(wfpath, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, 0,
		                  OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, 0);
		wstring_deallocate(wfpath);
	}
	if (dir == INVALID_HANDLE_VALUE) {
		string_const_t errstr = system_error_message(0);
		log_warnf(0, WARNING_SUSPICIOUS, STRING_CONST("Unable to open handle for path: %.*s : %.*s"),
		          STRING_FORMAT(monitor->path), STRING_FORMAT(errstr));
		goto exit_thread;
	}
#endif

	while (keep_running) {
#if FOUNDATION_PLATFORM_WINDOWS
		DWORD transferred;

		memset(&overlap, 0, sizeof(overlap));
		overlap.hEvent = handle;

		out_size = 0;
		success = ReadDirectoryChangesW(
		    dir, buffer, buffer_size, TRUE,
		    FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE, &out_size, &overlap,
		    0);
		if (!success) {
			string_const_t errstr = system_error_message(0);
			log_warnf(0, WARNING_SUSPICIOUS, STRING_CONST("Unable to read directory changes for path: %.*s : %.*s"),
			          STRING_FORMAT(monitor->path), STRING_FORMAT(errstr));
			goto exit_thread;
		}

		wait_status = beacon_wait(beacon);

		if (wait_status <= 0) {
			// Thread signalled or error
			keep_running = false;
		} else if (wait_status == event) {
			// File system change
			transferred = 0;
			success = GetOverlappedResult(dir, &overlap, &transferred, FALSE);
			if (!success) {
				string_const_t errstr = system_error_message(0);
				log_warnf(0, WARNING_SUSPICIOUS, STRING_CONST("Unable to read directory changes for path: %.*s : %.*s"),
				          STRING_FORMAT(monitor->path), STRING_FORMAT(errstr));
			} else {
				PFILE_NOTIFY_INFORMATION info = buffer;
				do {
					size_t numchars = info->FileNameLength / sizeof(wchar_t);
					wchar_t term = info->FileName[numchars];
					string_t utfstr;
					string_t fullpath;

					info->FileName[numchars] = 0;
					utfstr = string_allocate_from_wstring(info->FileName, wstring_length(info->FileName));
					utfstr = path_clean(STRING_ARGS_CAPACITY(utfstr));
					fullpath = path_allocate_concat(STRING_ARGS(monitor->path), STRING_ARGS(utfstr));

					if (fs_is_directory(STRING_ARGS(fullpath))) {
						// Ignore directory changes
					} else {
						foundation_event_id fsevent = FOUNDATIONEVENT_NOEVENT;
						switch (info->Action) {
							case FILE_ACTION_ADDED:
								fsevent = FOUNDATIONEVENT_FILE_CREATED;
								break;
							case FILE_ACTION_REMOVED:
								fsevent = FOUNDATIONEVENT_FILE_DELETED;
								break;
							case FILE_ACTION_MODIFIED:
								if (fs_is_file(STRING_ARGS(fullpath)))
									fsevent = FOUNDATIONEVENT_FILE_MODIFIED;
								break;

							// Treat rename as delete/add pair
							case FILE_ACTION_RENAMED_OLD_NAME:
								fsevent = FOUNDATIONEVENT_FILE_DELETED;
								break;
							case FILE_ACTION_RENAMED_NEW_NAME:
								fsevent = FOUNDATIONEVENT_FILE_CREATED;
								break;

							default:
								break;
						}

						if (fsevent)
							fs_event_post(fsevent, STRING_ARGS(fullpath));
					}
					string_deallocate(utfstr.str);
					string_deallocate(fullpath.str);

					info->FileName[numchars] = term;

					info = info->NextEntryOffset ?
					           (PFILE_NOTIFY_INFORMATION)(pointer_offset(info, info->NextEntryOffset)) :
                               nullptr;
				} while (info);
			}
		}

#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID

		int avail = 0;
		if (beacon_wait(beacon) == 0)
			keep_running = false;
		else
			ioctl(notify_fd, FIONREAD, &avail);
		if (avail > 0) {
			void* buffer =
			    memory_allocate(HASH_STREAM, (size_t)avail + 4, 8, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
			int offset = 0;
			ssize_t avail_read = read(notify_fd, buffer, (size_t)avail);
			struct inotify_event* event = (struct inotify_event*)buffer;
			while (offset < avail_read) {
				fs_watch_t* curwatch = fs_lookup_watch(watch, event->wd);
				if (!curwatch) {
					log_warnf(0, WARNING_SUSPICIOUS,
					          STRING_CONST("inotify watch not found: %d %x %x %" PRIsize " bytes: %.*s"), event->wd,
					          event->mask, event->cookie, (size_t)event->len, (int)event->len,
					          (const char*)event->name);
					goto skipwatch;
				}

				string_t curpath = string_copy(pathbuffer, sizeof(pathbuffer), STRING_ARGS(curwatch->path));
				curpath =
				    string_append(STRING_ARGS(curpath), sizeof(pathbuffer), event->name, string_length(event->name));

				bool is_dir = ((event->mask & IN_ISDIR) != 0);

				if ((event->mask & IN_CREATE) || (event->mask & IN_MOVED_TO)) {
					if (is_dir)
						fs_add_notify_subdir(notify_fd, STRING_ARGS(curpath), sizeof(pathbuffer), &watch, &paths, true);
					else
						fs_event_post(FOUNDATIONEVENT_FILE_CREATED, STRING_ARGS(curpath));
				}
				if ((event->mask & IN_DELETE) || (event->mask & IN_MOVED_FROM)) {
					if (!is_dir)
						fs_event_post(FOUNDATIONEVENT_FILE_DELETED, STRING_ARGS(curpath));
				}
				if (event->mask & IN_MODIFY) {
					if (!is_dir)
						fs_event_post(FOUNDATIONEVENT_FILE_MODIFIED, STRING_ARGS(curpath));
				}
				/* Moved events are also notified as CREATE/DELETE with cookies, so ignore for now
				if (event->mask & IN_MOVED_FROM)
				if (event->mask & IN_MOVED_TO)*/

			skipwatch:
				offset += event->len + sizeof(struct inotify_event);
				event = (struct inotify_event*)pointer_offset(buffer, offset);
			}
			memory_deallocate(buffer);
		}

#elif FOUNDATION_PLATFORM_MACOS

		keep_running = false;

		//#elif FOUNDATION_PLATFORM_BSD
		//  TODO: Implement using kqueue and directory watching using open with O_EVTONLY
		//        https://github.com/emcrisostomo/fswatch/blob/master/libfswatch/src/libfswatch/c%2B%2B/kqueue_monitor.cpp

#else
		// log_debug(0, STRING_CONST("Filesystem watcher not implemented on this platform"));
		// Not implemented yet, just wait for signal to simulate thread
		thread_wait();
		keep_running = false;
#endif
	}

	// log_debugf(0, STRING_CONST("Stopped monitoring file system: %.*s"),
	//           STRING_FORMAT(monitor->path));

#if FOUNDATION_PLATFORM_WINDOWS

exit_thread:

	CloseHandle(dir);

	if (handle != INVALID_HANDLE_VALUE)
		CloseHandle(handle);

	memory_deallocate(buffer);

#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID

	close(notify_fd);
	string_array_deallocate(paths);
	array_deallocate(watch);

#endif

	memory_context_pop();

	return 0;
}

#endif

static fs_file_descriptor
fs_file_fopen(const char* path, size_t length, unsigned int mode, bool* dotrunc) {
	fs_file_descriptor fd = 0;
	int retry = 0;

#if FOUNDATION_PLATFORM_WINDOWS
#define MODESTRING(x) L##x
	wchar_t* wpath;

	DWORD read_write = 0;
	DWORD share = FILE_SHARE_READ;
	DWORD create = OPEN_EXISTING;
	if (mode & STREAM_IN) {
		if (mode & STREAM_OUT) {
			if (mode & STREAM_CREATE) {
				if (mode & STREAM_TRUNCATE)
					create = CREATE_ALWAYS;
				else {
					create = OPEN_EXISTING;
					retry = 1;
				}
			} else {
				create = OPEN_EXISTING;
				if ((mode & STREAM_TRUNCATE) && dotrunc)
					*dotrunc = true;
			}
			read_write = GENERIC_READ | GENERIC_WRITE;
		} else {
			// truncate is ignored for read-only files
			create = OPEN_EXISTING;
			if (mode & STREAM_CREATE) {
				retry = 1;
			}
			read_write = GENERIC_READ;
			share |= FILE_SHARE_WRITE;
		}
	} else if (mode & STREAM_OUT) {
		if (mode & STREAM_TRUNCATE) {
			if (mode & STREAM_CREATE)
				create = CREATE_ALWAYS;
			else {
				create = OPEN_EXISTING;
				if (dotrunc)
					*dotrunc = true;
			}
		} else {
			create = OPEN_EXISTING;
			if (mode & STREAM_CREATE)
				retry = 1;
		}
		read_write = GENERIC_WRITE;
	} else {
		return INVALID_HANDLE_VALUE;
	}
	if ((mode & STREAM_CREATE_EXCLUSIVE) == STREAM_CREATE_EXCLUSIVE) {
		create = CREATE_NEW;
		share = FILE_SHARE_READ;
	}
#else
#if FOUNDATION_PLATFORM_LINUX
#define MODESTRING(x) x "e"
	const char* modestr;
#else
#define MODESTRING(x) x
	const char* modestr;
#endif
	if (mode & STREAM_IN) {
		if (mode & STREAM_OUT) {
			if ((mode & STREAM_CREATE_EXCLUSIVE) == STREAM_CREATE_EXCLUSIVE) {
				modestr = MODESTRING("w+bx");
			} else if (mode & STREAM_CREATE) {
				if (mode & STREAM_TRUNCATE)
					modestr = MODESTRING("w+b");
				else {
					modestr = MODESTRING("r+b");
					retry = 1;
				}
			} else {
				modestr = MODESTRING("r+b");
				if ((mode & STREAM_TRUNCATE) && dotrunc)
					*dotrunc = true;
			}
		} else {
			// truncate is ignored for read-only files
			if ((mode & STREAM_CREATE_EXCLUSIVE) == STREAM_CREATE_EXCLUSIVE) {
				modestr = MODESTRING("r+bx");
				retry = 1;
			} else if (mode & STREAM_CREATE) {
				modestr = MODESTRING("r+b");
				retry = 1;
			} else
				modestr = MODESTRING("rb");
		}
	} else if (mode & STREAM_OUT) {
		if (mode & STREAM_TRUNCATE) {
			if ((mode & STREAM_CREATE_EXCLUSIVE) == STREAM_CREATE_EXCLUSIVE) {
				modestr = MODESTRING("w+bx");
			} else if (mode & STREAM_CREATE) {
				modestr = MODESTRING("w+b");
			} else {
				modestr = MODESTRING("r+b");
				if (dotrunc)
					*dotrunc = true;
			}
		} else {
			if ((mode & STREAM_CREATE_EXCLUSIVE) == STREAM_CREATE_EXCLUSIVE) {
				modestr = MODESTRING("w+bx");
			} else {
				modestr = MODESTRING("r+b");
			}
			if (mode & STREAM_CREATE)
				retry = 1;
		}
	} else {
		return 0;
	}
#endif

	do {
#if FOUNDATION_PLATFORM_WINDOWS
		wpath = wstring_allocate_from_string(path, length);
		fd = CreateFileW(wpath, read_write, share, nullptr, create, FILE_ATTRIBUTE_NORMAL, nullptr);
		wstring_deallocate(wpath);
#elif FOUNDATION_PLATFORM_POSIX
		FOUNDATION_UNUSED(length);
		fd = fopen(path, modestr);
#else
#error Not implemented
#endif
		// In case retry is set, it's because we want to create a file if it does not exist,
		// but not truncate existing file, while still not using append mode since that fixes
		// writing to end of file. Try first with r+b to avoid truncation, then if it fails
		// i.e file does not exist, create it with w+b
#if FOUNDATION_PLATFORM_WINDOWS
		if ((mode & STREAM_CREATE_EXCLUSIVE) == STREAM_CREATE_EXCLUSIVE)
			create = CREATE_NEW;
		else
			create = CREATE_ALWAYS;
#else
		if ((mode & STREAM_CREATE_EXCLUSIVE) == STREAM_CREATE_EXCLUSIVE)
			modestr = MODESTRING("w+bx");
		else
			modestr = MODESTRING("w+b");
#endif
	} while (!FD_VALID(fd) && (retry-- > 0));

	if (FD_VALID(fd) && (mode & STREAM_ATEND)) {
#if FOUNDATION_PLATFORM_WINDOWS
		if (SetFilePointer(fd, 0, 0, FILE_END) == INVALID_SET_FILE_POINTER)
			log_warnf(0, WARNING_SYSTEM_CALL_FAIL, STRING_CONST("Unable to seek to end of stream '%.*s'"), (int)length,
			          path);
#else
		if (fseek(fd, 0, SEEK_END))
			log_warnf(0, WARNING_SYSTEM_CALL_FAIL, STRING_CONST("Unable to seek to end of stream '%.*s'"), (int)length,
			          path);
#endif
	}

	return fd;
}

static size_t
fs_file_tell(stream_t* stream) {
	if (!FD_VALID(GET_FILE(stream)->fd))
		return 0;
#if FOUNDATION_PLATFORM_WINDOWS
	LARGE_INTEGER large_offset = {0};
	LONG low_word = SetFilePointerEx(GET_FILE(stream)->fd, large_offset, &large_offset, FILE_CURRENT);
	if (low_word == INVALID_SET_FILE_POINTER)
		return 0;
	return (size_t)large_offset.QuadPart;
#else
	off_t pos = ftello(GET_FILE(stream)->fd);
	return (size_t)(pos < 0 ? 0 : pos);
#endif
}

static void
fs_file_seek(stream_t* stream, ssize_t offset, stream_seek_mode_t direction) {
#if FOUNDATION_PLATFORM_WINDOWS
	LARGE_INTEGER large_offset;
	large_offset.QuadPart = (LONGLONG)offset;
	if (SetFilePointerEx(GET_FILE(stream)->fd, large_offset, 0,
	                     (direction == STREAM_SEEK_BEGIN) ?
	                         FILE_BEGIN :
                             ((direction == STREAM_SEEK_END) ? FILE_END : FILE_CURRENT)) == INVALID_SET_FILE_POINTER) {
		log_warnf(0, WARNING_SYSTEM_CALL_FAIL, STRING_CONST("Unable to seek to %" PRId64 ":%d in stream '%.*s'"),
		          (int64_t)offset, (int)direction, STRING_FORMAT(stream->path));
	}
#else
	/*lint -esym(970,long) */
	if (fseek(GET_FILE(stream)->fd, (long)offset,
	          (direction == STREAM_SEEK_BEGIN) ? SEEK_SET : ((direction == STREAM_SEEK_END) ? SEEK_END : SEEK_CUR))) {
		log_warnf(0, WARNING_SYSTEM_CALL_FAIL, STRING_CONST("Unable to seek to %" PRId64 ":%d in stream '%.*s'"),
		          (int64_t)offset, (int)direction, STRING_FORMAT(stream->path));
	}
#endif
}

static size_t
fs_file_size(stream_t* stream) {
	if (!FD_VALID(GET_FILE(stream)->fd))
		return 0;
#if FOUNDATION_PLATFORM_WINDOWS
	LARGE_INTEGER file_size = {0};
	if (GetFileSizeEx(GET_FILE(stream)->fd, &file_size) == 0)
		return 0;
	return (size_t)file_size.QuadPart;
#else
	size_t cur, size;

	cur = fs_file_tell(stream);
	fs_file_seek(stream, 0, STREAM_SEEK_END);
	size = fs_file_tell(stream);
	fs_file_seek(stream, (ssize_t)cur, STREAM_SEEK_BEGIN);

	return size;
#endif
}

static bool
fs_file_eos(stream_t* stream) {
	if (!FD_VALID(GET_FILE(stream)->fd))
		return true;
#if FOUNDATION_PLATFORM_WINDOWS
	size_t current = fs_file_tell(stream);
	size_t size = fs_file_size(stream);
	return (current == size);
#else
	return feof(GET_FILE(stream)->fd) != 0;
#endif
}

static void
fs_file_truncate(stream_t* stream, size_t length) {
	if (!(stream->mode & STREAM_OUT) || !FD_VALID(GET_FILE(stream)->fd))
		return;

	if (length >= fs_file_size(stream))
		return;

#if FOUNDATION_PLATFORM_WINDOWS
	LARGE_INTEGER large_offset;
	large_offset.QuadPart = (LONGLONG)length;
	bool success = (SetFilePointerEx(GET_FILE(stream)->fd, large_offset, 0, FILE_BEGIN) != INVALID_SET_FILE_POINTER);
	if (success)
		success = (SetEndOfFile(GET_FILE(stream)->fd) != 0);
	if (!success) {
		string_const_t errstr = system_error_message(0);
		string_const_t fspath = fs_strip_protocol(STRING_ARGS(stream->path));
		log_warnf(0, WARNING_SUSPICIOUS, STRING_CONST("Unable to truncate real file %.*s (%" PRIsize " bytes): %.*s"),
		          STRING_FORMAT(fspath), length, STRING_FORMAT(errstr));
	}

#elif FOUNDATION_PLATFORM_POSIX

	stream_file_t* file;
	string_const_t fspath;
	size_t cur;

	cur = fs_file_tell(stream);
	if (cur > length)
		cur = length;

	file = GET_FILE(stream);
	fspath = fs_strip_protocol(STRING_ARGS(file->path));
	if (!fspath.length)
		return;

	fclose(file->fd);
	file->fd = 0;

	int fd = open(fspath.str, O_RDWR);
	if (ftruncate(fd, (ssize_t)length) < 0) {
		int err = system_error();
		string_const_t errmsg = system_error_message(err);
		log_warnf(0, WARNING_SUSPICIOUS,
		          STRING_CONST("Unable to truncate real file %.*s (%" PRIsize " bytes): %.*s (%d)"),
		          STRING_FORMAT(fspath), length, STRING_FORMAT(errmsg), err);
	}
	close(fd);

	file->fd = fs_file_fopen(fspath.str, fspath.length, stream->mode, 0);
	fs_file_seek(stream, (ssize_t)cur, STREAM_SEEK_BEGIN);

#else
#error Not implemented
#endif
}

static void
fs_file_flush(stream_t* stream) {
	if (!FD_VALID(GET_FILE(stream)->fd))
		return;
#if FOUNDATION_PLATFORM_WINDOWS
	FlushFileBuffers(GET_FILE(stream)->fd);
#else
	fflush(GET_FILE(stream)->fd);
#endif
}

static size_t
fs_file_read(stream_t* stream, void* buffer, size_t size) {
	if (!(stream->mode & STREAM_IN) || !FD_VALID(GET_FILE(stream)))
		return 0;

	stream_file_t* file = GET_FILE(stream);
#if FOUNDATION_PLATFORM_WINDOWS
	DWORD bytes_read = 0;
	if (ReadFile(file->fd, buffer, (DWORD)size, &bytes_read, 0) == 0)
		return 0;
	return bytes_read;
#else
	return fread(buffer, 1, size, file->fd);
#endif
}

static size_t
fs_file_write(stream_t* stream, const void* buffer, size_t size) {
	if (!(stream->mode & STREAM_OUT) || !FD_VALID(GET_FILE(stream)->fd))
		return 0;

	stream_file_t* file = GET_FILE(stream);
#if FOUNDATION_PLATFORM_WINDOWS
	DWORD bytes_written = 0;
	if (WriteFile(file->fd, buffer, (DWORD)size, &bytes_written, 0) == 0)
		return 0;
	return bytes_written;
#else
	return fwrite(buffer, 1, size, file->fd);
#endif
}

static tick_t
fs_file_last_modified(const stream_t* stream) {
	const stream_file_t* fstream = GET_FILE_CONST(stream);
	return fs_last_modified(fstream->path.str, fstream->path.length);
}

static size_t
fs_file_available_read(stream_t* stream) {
	size_t size = fs_file_size(stream);
	size_t cur = fs_file_tell(stream);

	if (size > cur)
		return size - cur;

	return 0;
}

static stream_t*
fs_file_clone(stream_t* stream) {
	stream_file_t* file = GET_FILE(stream);
	return fs_open_file(file->path.str, file->path.length, file->mode);
}

static void
fs_file_finalize(stream_t* stream) {
	stream_file_t* file = GET_FILE(stream);
	if (!FD_VALID(file->fd))
		return;

	if (file->mode & STREAM_SYNC) {
		fs_file_flush(stream);
		if (file->fd) {
#if FOUNDATION_PLATFORM_WINDOWS
			//_commit(_fileno(file->fd));
#elif FOUNDATION_PLATFORM_MACOS
			fcntl(fileno(file->fd), F_FULLFSYNC, 0);
#elif FOUNDATION_PLATFORM_POSIX
			fsync(fileno(file->fd));
#else
#error Not implemented
#endif
		}
	}

#if FOUNDATION_PLATFORM_WINDOWS
	CloseHandle(file->fd);
	file->fd = INVALID_HANDLE_VALUE;
#else
	fclose(file->fd);
	file->fd = 0;
#endif
}

stream_t*
fs_open_file(const char* path, size_t length, unsigned int mode) {
	stream_file_t* file;
	fs_file_descriptor fd;
	stream_t* stream;
	string_const_t fspath;
	string_t localpath;
	string_t finalpath;
	size_t capacity;
	bool dotrunc;
	char buffer[BUILD_MAX_PATHLEN];

	capacity = sizeof(buffer);
	localpath = string_copy(buffer, capacity, path, length);
	localpath = path_clean(STRING_ARGS(localpath), capacity);
	if (!path_is_absolute(STRING_ARGS(localpath)))
		localpath = path_absolute(STRING_ARGS(localpath), capacity);

	capacity = localpath.length + 8;
	finalpath = string_allocate(0, capacity);
	finalpath = string_copy(finalpath.str, capacity, STRING_ARGS(localpath));
	if (string_find_string(STRING_ARGS(finalpath), STRING_CONST("://"), 0) == STRING_NPOS) {
		if (finalpath.str[0] == '/')
			finalpath = string_prepend(STRING_ARGS(finalpath), capacity, STRING_CONST("file:/"));
		else
			finalpath = string_prepend(STRING_ARGS(finalpath), capacity, STRING_CONST("file://"));
	}

	dotrunc = false;

	fspath = fs_strip_protocol(STRING_ARGS(finalpath));
	fd = fs_file_fopen(STRING_ARGS(fspath), mode, &dotrunc);
	if (!FD_VALID(fd)) {
		string_deallocate(finalpath.str);
		return 0;
	}

	file = memory_allocate(HASH_STREAM, sizeof(stream_file_t), 8, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	stream = GET_STREAM(file);
	stream_initialize(stream, BUILD_DEFAULT_STREAM_BYTEORDER);

	file->fd = fd;
	file->type = STREAMTYPE_FILE;
	file->persistent = 1;
	file->mode = mode & (STREAM_OUT | STREAM_IN | STREAM_BINARY | STREAM_SYNC);
	file->path = finalpath;
	file->vtable = &fs_file_vtable;

	if (dotrunc)
		fs_file_truncate(stream, 0);
	else if (mode & STREAM_ATEND)
		stream_seek(stream, 0, STREAM_SEEK_END);

	return stream;
}

int
internal_fs_initialize(void) {
#if FOUNDATION_HAVE_FS_MONITOR
	fs_monitors = memory_allocate(HASH_STREAM, sizeof(fs_monitor_t) * foundation_config().fs_monitor_max, 0,
	                              MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	fs_monitor_lock = mutex_allocate(STRING_CONST("fs_monitors"));
#else
	fs_monitors = 0;
	fs_monitor_lock = 0;
#endif

	fs_event_stream_current = event_stream_allocate(512);

	fs_file_vtable.read = fs_file_read;
	fs_file_vtable.write = fs_file_write;
	fs_file_vtable.eos = fs_file_eos;
	fs_file_vtable.flush = fs_file_flush;
	fs_file_vtable.truncate = fs_file_truncate;
	fs_file_vtable.size = fs_file_size;
	fs_file_vtable.seek = fs_file_seek;
	fs_file_vtable.tell = fs_file_tell;
	fs_file_vtable.lastmod = fs_file_last_modified;
	fs_file_vtable.buffer_read = 0;
	fs_file_vtable.available_read = fs_file_available_read;
	fs_file_vtable.finalize = fs_file_finalize;
	fs_file_vtable.clone = fs_file_clone;

	internal_ringbuffer_stream_initialize();
	internal_buffer_stream_initialize();
#if FOUNDATION_PLATFORM_ANDROID
	internal_asset_stream_initialize();
#endif
	internal_pipe_stream_initialize();

	return 0;
}

void
internal_fs_finalize(void) {
	if (fs_monitors) {
		for (size_t mi = 0; mi < foundation_config().fs_monitor_max; ++mi)
			fs_stop_monitor(fs_monitors + mi);
	}
	mutex_deallocate(fs_monitor_lock);

	event_stream_deallocate(fs_event_stream_current);
	fs_event_stream_current = 0;

	memory_deallocate(fs_monitors);
	fs_monitors = 0;
}
