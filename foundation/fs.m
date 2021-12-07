/* fs.m  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#if FOUNDATION_PLATFORM_MACOS

#include <foundation/apple.h>

#include <dispatch/queue.h>
#include <sys/stat.h>

extern void*
fs_event_stream_create(const char* path, size_t length);

extern void
fs_event_stream_destroy(void* stream);

// This implementation is not optimal in any way, but will do for now
// Memory allocation mania should really be cleaned up

typedef __attribute__((__aligned__(8))) struct file_node_t file_node_t;

struct file_node_t {
	string_t name;
	file_node_t** subdirs;
	string_t* files;
	tick_t* last_modified;
};

static void
fs_node_deallocate(file_node_t* node) {
	string_deallocate(node->name.str);
	string_array_deallocate(node->files);
	for (size_t isub = 0, subsize = array_size(node->subdirs); isub < subsize; ++isub)
		fs_node_deallocate(node->subdirs[isub]);
	array_deallocate(node->subdirs);
	array_deallocate(node->last_modified);
	memory_deallocate(node);
}

static void
fs_node_populate(file_node_t* node, const char* fullpath, size_t length) {
	string_t* subdirs = fs_subdirs(fullpath, length);
	for (size_t isub = 0, subsize = array_size(subdirs); isub < subsize; ++isub) {
		file_node_t* child = memory_allocate(0, sizeof(file_node_t), 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		child->name = subdirs[isub];
		array_push(node->subdirs, child);
	}
	array_deallocate(subdirs);  // Only array, strings are kept in nodes

	node->files = fs_files(fullpath, length);
	for (size_t isub = 0, subsize = array_size(node->files); isub < subsize; ++isub) {
		string_t filepath = path_allocate_concat(fullpath, length, STRING_ARGS(node->files[isub]));
		tick_t last_modified = fs_last_modified(STRING_ARGS(filepath));
		array_push(node->last_modified, last_modified);
		string_deallocate(filepath.str);
	}

	for (size_t isub = 0, subsize = array_size(node->subdirs); isub < subsize; ++isub) {
		string_t subpath = path_allocate_concat(fullpath, length, STRING_ARGS(node->subdirs[isub]->name));
		fs_node_populate(node->subdirs[isub], STRING_ARGS(subpath));
		string_deallocate(subpath.str);
	}
}

static file_node_t*
fs_node_find(file_node_t* root, const char* path, size_t pathlen) {
	if (!pathlen || string_equal(path, pathlen, STRING_CONST("/")))
		return root;

	file_node_t* node = root;
	do {
		file_node_t* next = 0;
		size_t separator = string_find(path, pathlen, '/', 0);
		for (size_t isub = 0, subsize = array_size(node->subdirs); isub < subsize; ++isub) {
			if (string_equal(STRING_ARGS(node->subdirs[isub]->name), path,
			                 separator != STRING_NPOS ? separator : pathlen)) {
				next = node->subdirs[isub];
				path = path + separator + 1;
				break;
			}
		}
		node = next;
		if (!*path)
			return node;
	} while (node);

	return 0;
}

static void
fs_node_send_deletions(file_node_t* node, const char* path, size_t pathlen) {
	char pathbuf[BUILD_MAX_PATHLEN];

	for (size_t ifile = 0, fsize = array_size(node->files); ifile < fsize; ++ifile) {
		string_t pathstr = path_concat(pathbuf, sizeof(pathbuf), path, pathlen, STRING_ARGS(node->files[ifile]));
		fs_event_post(FOUNDATIONEVENT_FILE_DELETED, STRING_ARGS(pathstr));
	}

	for (size_t isub = 0, subsize = array_size(node->subdirs); isub < subsize; ++isub) {
		string_t pathstr = path_concat(pathbuf, sizeof(pathbuf), path, pathlen, STRING_ARGS(node->subdirs[isub]->name));
		fs_node_send_deletions(node->subdirs[isub], STRING_ARGS(pathstr));
	}
}

static void
fs_node_send_creations(file_node_t* node, const char* path, size_t pathlen) {
	char pathbuf[BUILD_MAX_PATHLEN];

	for (size_t ifile = 0, fsize = array_size(node->files); ifile < fsize; ++ifile) {
		string_t pathstr = path_concat(pathbuf, sizeof(pathbuf), path, pathlen, STRING_ARGS(node->files[ifile]));
		fs_event_post(FOUNDATIONEVENT_FILE_CREATED, STRING_ARGS(pathstr));
	}

	for (size_t isub = 0, subsize = array_size(node->subdirs); isub < subsize; ++isub) {
		string_t pathstr = path_concat(pathbuf, sizeof(pathbuf), path, pathlen, STRING_ARGS(node->subdirs[isub]->name));
		fs_node_send_creations(node->subdirs[isub], STRING_ARGS(pathstr));
	}
}

static void
fs_event_stream_callback(ConstFSEventStreamRef stream_ref, void* user_data, size_t events_count,
                         const char* const event_paths[], const FSEventStreamEventFlags event_flags[],
                         const FSEventStreamEventId event_ids[]) {
	file_node_t* root_node = user_data;
	char pathbuf[BUILD_MAX_PATHLEN];
	FOUNDATION_UNUSED(stream_ref);

	bool bootstrap_thread = !thread_is_entered();
	if (bootstrap_thread)
		thread_enter();

	@autoreleasepool {
		for (size_t i = 0; i < events_count; ++i) {
			const char* rawpath = event_paths[i];
			size_t rawpath_len = string_length(rawpath);

			if ((rawpath_len > 8) && (strncmp(rawpath, "/private", 8) == 0)) {
				rawpath += 8;
				rawpath_len -= 8;
			}

			FSEventStreamEventFlags flags = event_flags[i];
			FSEventStreamEventId identifier = event_ids[i];

			/* Store path and recurse flag in paths-to-process,
			   then keep state and rescan for changes in fs monitor thread*/
			if ((flags & kFSEventStreamEventFlagMustScanSubDirs) != 0) {
				// TODO: Implement
				log_warnf(0, WARNING_UNSUPPORTED,
				          STRING_CONST("Got kFSEventStreamEventFlagMustScanSubDirs: %.*s (0x%x 0x%x)"),
				          (int)rawpath_len, rawpath, (unsigned int)flags, (unsigned int)identifier);
			} else {
				FOUNDATION_UNUSED(identifier);

				size_t root_ofs = string_find_string(rawpath, rawpath_len, STRING_ARGS(root_node->name), 0);
				if (root_ofs == STRING_NPOS)
					continue;

				const char* path = rawpath + root_ofs;
				size_t path_len = rawpath_len - root_ofs;

				const char* subpath = path + root_node->name.length + 1;
				size_t subpath_len = path_len - root_node->name.length - 1;

				file_node_t* node = fs_node_find(root_node, subpath, subpath_len);
				if (!node)
					continue;

				string_t* files = fs_files(rawpath, rawpath_len);

				// Check if file have been added, removed or modified
				for (size_t isub = 0, subsize = array_size(node->files); isub < subsize;) {
					ssize_t ifile;

					string_t pathstr =
					    path_concat(pathbuf, sizeof(pathbuf), path, path_len, STRING_ARGS(node->files[isub]));

					ifile = string_array_find((const string_const_t*)files, array_size(files),
					                          STRING_ARGS(node->files[isub]));
					if (ifile < 0) {
						string_deallocate(node->files[isub].str);
						array_erase_memcpy(node->files, isub);
						array_erase_memcpy(node->last_modified, isub);
						--subsize;
						fs_event_post(FOUNDATIONEVENT_FILE_DELETED, STRING_ARGS(pathstr));
					} else {
						tick_t last_modified = fs_last_modified(STRING_ARGS(pathstr));
						if (last_modified > node->last_modified[isub]) {
							node->last_modified[isub] = last_modified;
							fs_event_post(FOUNDATIONEVENT_FILE_MODIFIED, STRING_ARGS(pathstr));
						}
						++isub;
					}
				}
				for (size_t isub = 0, subsize = array_size(files); isub < subsize; ++isub) {
					if (string_array_find((const string_const_t*)node->files, array_size(node->files),
					                      STRING_ARGS(files[isub])) == -1) {
						string_t pathstr =
						    path_concat(pathbuf, sizeof(pathbuf), path, path_len, STRING_ARGS(files[isub]));
						tick_t last_mod = fs_last_modified(STRING_ARGS(pathstr));

						array_push(node->last_modified, last_mod);
						array_push(node->files, files[isub]);
						files[isub] = (string_t){0, 0};
						fs_event_post(FOUNDATIONEVENT_FILE_CREATED, STRING_ARGS(pathstr));
					}
				}

				string_array_deallocate(files);

				// Check for subdir additions/removals
				string_t* subdirs = fs_subdirs(rawpath, rawpath_len);
				for (size_t iexist = 0, existsize = array_size(node->subdirs); iexist < existsize;) {
					bool found = false;
					for (size_t isub = 0, subsize = array_size(subdirs); isub < subsize; ++isub) {
						if (string_equal(STRING_ARGS(node->subdirs[iexist]->name), STRING_ARGS(subdirs[isub]))) {
							found = true;
							break;
						}
					}

					if (!found) {
						// Recurse and send out file deletion events
						string_t pathstr = path_concat(pathbuf, sizeof(pathbuf), rawpath, rawpath_len,
						                               STRING_ARGS(node->subdirs[iexist]->name));
						fs_node_send_deletions(node->subdirs[iexist], STRING_ARGS(pathstr));
						fs_node_deallocate(node->subdirs[iexist]);
						array_erase_memcpy(node->subdirs, iexist);
						--existsize;
					} else {
						++iexist;
					}
				}

				for (size_t isub = 0, subsize = array_size(subdirs); isub < subsize; ++isub) {
					bool found = false;
					for (size_t iexist = 0, existsize = array_size(node->subdirs); iexist < existsize; ++iexist) {
						if (string_equal(STRING_ARGS(node->subdirs[iexist]->name), STRING_ARGS(subdirs[isub]))) {
							found = true;
							break;
						}
					}

					if (!found) {
						file_node_t* child =
						    memory_allocate(0, sizeof(file_node_t), 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

						child->name = subdirs[isub];
						subdirs[isub] = (string_t){0, 0};

						array_push(node->subdirs, child);

						string_t pathstr =
						    path_concat(pathbuf, sizeof(pathbuf), rawpath, rawpath_len, STRING_ARGS(child->name));
						fs_node_populate(child, STRING_ARGS(pathstr));
						fs_node_send_creations(child, pathstr.str + root_ofs, pathstr.length - root_ofs);
					}
				}

				string_array_deallocate(subdirs);
			}
		}
	}

	// This is run in a dispatch thread by the OS, need to clean up
	if (bootstrap_thread)
		thread_exit();
}

static const void*
fs_event_stream_retain(const void* info) {
	return info;
}

static void
fs_event_stream_release(const void* info) {
#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"
#endif
	if (!info)
		return;
	bool bootstrap_thread = !thread_is_entered();
	if (bootstrap_thread)
		thread_enter();
	fs_node_deallocate((file_node_t*)info);
	if (bootstrap_thread)
		thread_exit();
#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic pop
#endif
}

void*
fs_event_stream_create(const char* path, size_t length) {
	@autoreleasepool {
		file_node_t* node = memory_allocate(0, sizeof(file_node_t), 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		node->name = string_clone(path, length);

		fs_node_populate(node, STRING_ARGS(node->name));

		NSString* nspath = [[NSString alloc] initWithBytes:path length:length encoding:NSUTF8StringEncoding];
		NSArray* patharr = [NSArray arrayWithObject:nspath];
		NSTimeInterval latency = 1.0;
		FSEventStreamContext context;
		memset(&context, 0, sizeof(context));
		context.info = node;
		context.retain = fs_event_stream_retain;
		context.release = fs_event_stream_release;

		// TODO: Implement allocator based on foundation memory allocation subsystem
		void* stream = FSEventStreamCreate(0, (FSEventStreamCallback)&fs_event_stream_callback, &context,
		                                   (__bridge CFArrayRef)patharr, kFSEventStreamEventIdSinceNow,
		                                   (CFAbsoluteTime)latency, kFSEventStreamCreateFlagNone);
		if (stream) {
			FSEventStreamSetDispatchQueue(stream, dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0));
			if (NO == FSEventStreamStart(stream)) {
				log_error(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to start FS event stream"));
			}
		} else {
			log_error(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to create FS event stream"));
		}

		log_debugf(0, STRING_CONST("Started FS event stream for: %.*s"), (int)length, path);

		return stream;
	}
}

void
fs_event_stream_destroy(void* stream) {
	if (!stream)
		return;

	@autoreleasepool {
		FSEventStreamStop(stream);
		FSEventStreamInvalidate(stream);
		FSEventStreamRelease(stream);
	}

	thread_yield();
}

#endif
