/* path.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#include <string.h>

string_t
path_clean(char* path, size_t length, size_t capacity) {
	size_t ofs;
	size_t ahead, num, back, reduce;
	size_t reduce_limit = 0;
	size_t protocol = 0;
	size_t inlength = length;
	bool firstsep = true;
	bool clearsep;

	FOUNDATION_UNUSED(capacity);

	for (ofs = 0; ofs < length;) {
		if (path[ofs] == ':') {
			if (firstsep) {
				if (!ofs) {
					length--;
					memmove(path, path + 1, length);
					path[length] = 0;
				} else {
					firstsep = false;
					protocol = ofs++;
					reduce_limit = ofs;
					if ((ofs < length) && ((path[ofs] == '/') || (path[ofs] == '\\'))) {
						path[ofs] = '/';
						// If protocol length is 1 it's a drive letter
						if (protocol == 1) {
							if ((path[0] >= 'a') && (path[0] <= 'z'))
								path[0] = path[0] + (char)('A' - 'a');
						}
						if ((protocol > 1) && ((ofs + 1) < length) &&
						    ((path[ofs + 1] == '/') || (path[ofs + 1] == '\\'))) {
							++ofs;
							path[ofs] = '/';
						}
						reduce_limit = ofs + 1;
					}
				}
				continue;
			} else if (protocol && (ofs > reduce_limit)) {
				// Corner case of "protocol://C:/" style paths to prevent drive
				// letter from being reduced
				if ((path[ofs - 2] == ':') ||
				    ((path[ofs - 2] == '/') && ((path[ofs - 3] == ':') || (path[ofs - 3] == '/')))) {
					reduce_limit = ++ofs;
					if ((ofs < length) && ((path[ofs] == '/') || (path[ofs] == '\\'))) {
						path[ofs] = '/';
						reduce_limit = ofs + 1;
					}
					continue;
				}
			}
		}
		// Change backslash to forward slash and skip ahead
		if (path[ofs] == '\\')
			path[ofs++] = '/';
		else if (path[ofs++] != '/')
			continue;
		firstsep = false;

		// Reduce multiple slashes "//./" unless directly after protocol separator
		/*lint -e{850} */
		for (ahead = ofs; ahead < length; ++ahead) {
			if (path[ahead] == '.') {  // Catch "/./" path segments
				if ((ahead + 1 < length) && ((path[ahead + 1] == '/') || (path[ahead + 1] == '\\'))) {
					++ahead;
					continue;
				}
				break;
			} else if ((path[ahead] != '/') && (path[ahead] != '\\'))
				break;
			// Continue as long as we keep getting separators
		}
		if (ahead > ofs) {
			memmove(path + ofs, path + ahead, length - ahead);
			length -= ahead - ofs;
			path[length] = 0;
		}

		// Catch and reduce "/../" path segments
		ahead = ofs;
		num = 0;
		while ((ahead + 1 < length) && (path[ahead] == '.') && (path[ahead + 1] == '.')) {
			if (ahead + 2 == length) {
				++num;
				ahead = length;
				break;
			} else if ((path[ahead + 2] == '/') || (path[ahead + 2] == '\\')) {
				++num;
				ahead += 3;
			} else {
				break;
			}
		}
		if (num > 0) {
			reduce = 0;
			back = (ofs > reduce_limit) ? ofs - 1 : ofs;
			while ((back > reduce_limit) && (reduce < num)) {
				if (path[back - 1] == '.') {
					// Only nuke starting "./" or "../" if we have an absolute protocol path
					if ((back == (reduce_limit + 1)) || ((back == (reduce_limit + 2)) && (path[back - 2] == '.'))) {
						if ((reduce_limit > 2) && (path[reduce_limit - 1] == '/'))
							back = reduce_limit;
						break;
					}
					if ((path[back - 2] == '.') && (path[back - 3] == '/'))
						break;
					if (path[back - 2] == '/')
						break;
				}
				--back;
				++reduce;
				while ((back > reduce_limit) && (path[back] != '/'))
					--back;
			}
			if ((back == reduce_limit) && ((!back && (path[back] == '/')) || (back && (path[back - 1] == '/')))) {
				reduce = num;  // Path starts absolute, drop all "../"
			}
			if (reduce > 0) {
				clearsep = false;
				ofs = (path[back] == '/') ? back + 1 : back;
				if (reduce < num)
					ahead -= 3 * (num - reduce);
				if (length == ahead)
					clearsep = (path[length - 1] != '/');
				memmove(path + ofs, path + ahead, length - ahead);
				length -= ahead - ofs;
				path[length] = 0;
				if ((length > 1) && clearsep && (length > reduce_limit) && (path[length - 1] == '/'))
					path[--length] = 0;
				ofs = (ofs > (reduce_limit + 1)) ? ofs - 1 : (reduce_limit ? reduce_limit - 1 : 0);
				if (path[ofs] == ':')
					++ofs;
			}
		}
	}

	ofs = reduce_limit;

	// Clean starting "./"
	if ((length > ofs + 1) && (path[ofs] == '.') && (path[ofs + 1] == '/')) {
		memmove(path + ofs, path + ofs + 2, length - (ofs + 2));
		length -= 2;
	}

	// Clean ending "/."
	if ((length > 1) && (path[length - 2] == '/') && (path[length - 1] == '.'))
		--length;
	if ((length == 1) && (path[0] == '.'))
		--length;

	if (length < inlength)
		path[length] = 0;

	return (string_t){path, length};
}

string_const_t
path_base_file_name(const char* path, size_t length) {
	size_t start, end;
	if (!path || !length)
		return string_const(0, 0);
	start = string_find_last_of(path, length, STRING_CONST("/\\"), STRING_NPOS);
	end = string_rfind(path, length, '.', STRING_NPOS);
	// For "dot" files, i.e files with names like "/path/to/.file", return the dot name ".file"
	if (!end || (end == (start + 1)) || ((start != STRING_NPOS) && (end < start)))
		end = STRING_NPOS;
	if (start != STRING_NPOS)
		return string_substr(path, length, start + 1, (end != STRING_NPOS) ? (end - (start + 1)) : STRING_NPOS);
	return string_substr(path, length, 0, end);
}

string_const_t
path_base_file_name_with_directory(const char* path, size_t length) {
	size_t start, end;
	if (!path || !length)
		return string_const(0, 0);
	start = string_find_last_of(path, length, STRING_CONST("/\\"), STRING_NPOS);
	end = string_rfind(path, length, '.', STRING_NPOS);
	// For "dot" files, i.e files with names like "/path/to/.file", return the dot name ".file"
	if (!end || (end == (start + 1)) || ((start != STRING_NPOS) && (end < start)))
		end = STRING_NPOS;
	return string_substr(path, length, 0, (end != STRING_NPOS) ? end : STRING_NPOS);
}

string_const_t
path_file_extension(const char* path, size_t length) {
	size_t start = string_find_last_of(path, length, STRING_CONST("/\\"), STRING_NPOS);
	size_t end = string_rfind(path, length, '.', STRING_NPOS);
	if ((end != STRING_NPOS) && ((start == STRING_NPOS) || (end > start)))
		return string_substr(path, length, end + 1, STRING_NPOS);
	return string_const(0, 0);
}

string_const_t
path_file_name(const char* path, size_t length) {
	size_t end = string_find_last_of(path, length, STRING_CONST("/\\"), STRING_NPOS);
	if (end == STRING_NPOS)
		return string_const(path, length);
	return string_substr(path, length, end + 1, STRING_NPOS);
}

string_const_t
path_directory_name(const char* path, size_t length) {
	string_const_t result, protocol;
	size_t end = string_find_last_of(path, length, STRING_CONST("/\\"), STRING_NPOS);
	if (end == 0)
		return string_const("/", 1);
	if (end == STRING_NPOS) {
		if ((length >= 2) && (path[1] == ':'))
			return string_const(path, 2);
		return string_const(0, 0);
	}
	result = string_substr(path, length, 0, end);
	protocol = path_protocol(result.str, result.length + 1);
	if (protocol.length)
		protocol.length += 3;  // Include separator
	// Check if only a protocol
	if (result.length <= protocol.length)
		return protocol;
	// Check if only a drive letter (plus optional protocol)
	if (length >= protocol.length + 2) {
		if (path[protocol.length + 1] == ':') {
			if ((length >= protocol.length + 3) && (result.length <= protocol.length + 3) &&
			    ((path[protocol.length + 2] == '/') || (path[protocol.length + 2] == '\\')))
				return string_const(path, protocol.length + 3);
			if (result.length <= protocol.length + 2)
				return string_const(path, protocol.length + 2);
		}
	}
	return result;
}

string_const_t
path_subpath(const char* path, size_t length, const char* root, size_t root_length) {
	string_const_t testroot, testpath;
	string_const_t testrootstrip, testpathstrip;

	if (!root_length)
		return string_const(path, length);

	testpath = string_const(path, length);
	testpathstrip = path_strip_protocol(STRING_ARGS(testpath));

	testroot = string_const(root, root_length);
	testrootstrip = path_strip_protocol(STRING_ARGS(testroot));

	if ((testpath.str == testpathstrip.str) || (testroot.str == testrootstrip.str)) {
		// Either argument has no protocol, ignore it (otherwise match)
		testpath = testpathstrip;
		testroot = testrootstrip;
	}

	if (testpath.length <= testroot.length)
		return string_empty();

	if (!string_equal(testpath.str, testroot.length, testroot.str, testroot.length))
		return string_empty();

	if (testroot.length && (testroot.str[testroot.length - 1] != '/') && (testroot.str[testroot.length - 1] != ':')) {
		if (testpath.str[testroot.length] != '/')
			return string_empty();
		++testroot.length;  // Make returned path relative (skip separator slash)
	}

	return string_substr(testpath.str, testpath.length, testroot.length, STRING_NPOS);
}

string_const_t
path_protocol(const char* uri, size_t length) {
	size_t end = string_find_string(uri, length, STRING_CONST("://"), 0);
	if (end == STRING_NPOS)
		return string_const(0, 0);
	return string_substr(uri, length, 0, end);
}

string_const_t
path_strip_protocol(const char* uri, size_t length) {
	size_t sepofs = string_find_string(uri, length, STRING_CONST("://"), 0);
	if (sepofs != STRING_NPOS) {
		bool has_drive_letter = (length > (sepofs + 4)) && (uri[sepofs + 4] == ':');
		sepofs += (has_drive_letter ? 3 : 2);
		return string_substr(uri, length, sepofs, length - sepofs);
	}
	return (string_const_t){uri, length};
}

static string_t
path_append_fragment(char* dst, size_t length, size_t capacity, const char* path, size_t pathlen) {
	size_t offset;
	bool lastsep, beginsep;
	lastsep = length && ((dst[length - 1] == '/') || (dst[length - 1] == '\\'));
	beginsep = pathlen && ((path[0] == '/') || (path[0] == '\\'));
	if (!lastsep && !beginsep && length && (length + 1 < capacity))
		dst[length++] = '/';
	offset = ((lastsep && beginsep) || (!length && beginsep)) ? 1 : 0;
	return string_append(dst, length, capacity, path + offset, pathlen - offset);
}

static string_t
path_prepend_fragment(char* dst, size_t length, size_t capacity, const char* path, size_t pathlen) {
	size_t offset, num;
	bool lastsep, beginsep;
	beginsep = length && (dst[0] == '/' || dst[0] == '\\');
	lastsep = ((path[pathlen - 1] == '/') || (path[pathlen - 1] == '\\'));
	if (lastsep && beginsep)
		--pathlen;
	offset = (!lastsep && !beginsep && length) ? 1 : 0;

	if (pathlen + offset < capacity) {
		num = length;
		if (pathlen + offset + num >= capacity)
			num = capacity - (pathlen + offset);
		memmove(dst + pathlen + offset, dst, num);
		if (offset)
			dst[pathlen] = '/';
	}

	num = pathlen;
	if (num >= capacity)
		num = capacity - 1;
	memcpy(dst, path, num);

	num = pathlen + offset + length;
	if (num >= capacity)
		num = capacity - 1;
	dst[num] = 0;

	return (string_t){dst, num};
}

static string_t
path_concat_impl(char* dest, size_t capacity, bool reallocate, const char* first, size_t first_length, va_list list) {
	va_list clist;
	string_t result;
	void* ptr;
	size_t psize;

	if (reallocate) {
		size_t totalsize = first_length + 1;
		va_copy(clist, list);
		do {
			ptr = va_arg(clist, void*);
			if (ptr) {
				psize = va_arg(clist, size_t);
				if (psize)
					totalsize += psize + 1;
			}
		} while (ptr);
		va_end(clist);

		if (totalsize >= capacity) {
			dest = dest ? memory_reallocate(dest, totalsize + 1, 0, capacity, MEMORY_NO_PRESERVE) :
			              memory_allocate(HASH_STRING, totalsize + 1, 0, MEMORY_PERSISTENT);
			capacity = totalsize + 1;
		}
	}

	result.str = dest;
	result.length = 0;

	if (first_length)
		result = string_copy(result.str, capacity, first, first_length);

	/*lint -e{838} */
	va_copy(clist, list);
	do {
		ptr = va_arg(clist, void*);
		if (ptr) {
			psize = va_arg(clist, size_t);
			if (psize)
				result = path_append_fragment(STRING_ARGS(result), capacity, ptr, psize);
		}
	} while (ptr);
	va_end(clist);
	return result;
}

string_t
path_concat(char* dest, size_t capacity, const char* first, size_t first_length, const char* second,
            size_t second_length) {
	return path_concat_varg(dest, capacity, first, first_length, second, second_length, nullptr);
}

string_t
path_concat_varg(char* dest, size_t capacity, const char* first, size_t first_length, ...) {
	va_list list;
	string_t result;
	va_start(list, first_length);
	result = path_concat_vlist(dest, capacity, first, first_length, list);
	va_end(list);
	return result;
}

string_t
path_concat_vlist(char* dest, size_t capacity, const char* first, size_t first_length, va_list list) {
	return path_concat_impl(dest, capacity, false, first, first_length, list);
}

string_t
path_allocate_concat(const char* first, size_t first_length, const char* second, size_t second_length) {
	return path_allocate_concat_varg(first, first_length, second, second_length, nullptr);
}

string_t
path_allocate_concat_varg(const char* first, size_t first_length, ...) {
	va_list list;
	string_t result;
	va_start(list, first_length);
	result = path_allocate_concat_vlist(first, first_length, list);
	va_end(list);
	return result;
}

string_t
path_allocate_concat_vlist(const char* first, size_t first_length, va_list list) {
	return path_concat_impl(0, 0, true, first, first_length, list);
}

string_t
path_append(char* base, size_t base_length, size_t base_capacity, const char* tail, size_t tail_length) {
	return path_append_fragment(base, base_length, base_capacity, tail, tail_length);
}

string_t
path_append_varg(char* base, size_t base_length, size_t base_capacity, const char* tail, size_t tail_length, ...) {
	va_list list;
	string_t result;
	va_start(list, tail_length);
	result = path_append_vlist(base, base_length, base_capacity, tail, tail_length, list);
	va_end(list);
	return result;
}

string_t
path_append_vlist(char* base, size_t base_length, size_t base_capacity, const char* tail, size_t tail_length,
                  va_list list) {
	va_list clist;
	string_t result;
	void* ptr;
	size_t psize;

	result = (string_t){base, base_length};
	result = path_append_fragment(STRING_ARGS(result), base_capacity, tail, tail_length);

	va_copy(clist, list);
	while (true) {
		ptr = va_arg(clist, void*);
		if (!ptr)
			break;
		psize = va_arg(clist, size_t);
		if (psize)
			result = path_append_fragment(STRING_ARGS(result), base_capacity, ptr, psize);
	}
	va_end(clist);
	return result;
}

string_t
path_prepend(char* tail, size_t tail_length, size_t tail_capacity, const char* base, size_t base_length) {
	return path_prepend_fragment(tail, tail_length, tail_capacity, base, base_length);
}

string_t
path_prepend_varg(char* tail, size_t tail_length, size_t tail_capacity, const char* base, size_t base_length, ...) {
	va_list list;
	string_t result;
	va_start(list, base_length);
	result = path_prepend_vlist(tail, tail_length, tail_capacity, base, base_length, list);
	va_end(list);
	return result;
}

string_t
path_prepend_vlist(char* tail, size_t tail_length, size_t tail_capacity, const char* base, size_t base_length,
                   va_list list) {
	va_list clist;
	string_t result;
	void* ptr;
	size_t psize;

	// TOSO: This should be done by putting fragments from back to front instead of prepending
	// (which is a memmove) each
	result = (string_t){tail, tail_length};
	result = path_prepend_fragment(STRING_ARGS(result), tail_capacity, base, base_length);

	va_copy(clist, list);
	while (true) {
		ptr = va_arg(clist, void*);
		if (!ptr)
			break;
		psize = va_arg(clist, size_t);
		if (psize)
			result = path_prepend_fragment(STRING_ARGS(result), tail_capacity, ptr, psize);
	}
	va_end(clist);
	return result;
}

bool
path_is_absolute(const char* path, size_t length) {
	size_t firstsep;
	if (!path || !length)
		return false;
	if ((path[0] == '/') || (path[0] == '\\'))
		return true;

	for (firstsep = 1; firstsep < length; ++firstsep) {
		if (path[firstsep] == ':') {
			if (firstsep == 1)
				return true;  // Windows style drive letter paths, "C:..."
			++firstsep;
			if ((firstsep < length) && ((path[firstsep] == '/') || (path[firstsep] == '\\')))
				return true;
			return false;
		}
		if ((path[firstsep] == '/') || (path[firstsep] == '\\'))
			return false;
	}
	return false;
}

string_t
path_absolute(char* path, size_t length, size_t capacity) {
	string_t abspath;
	if (!path_is_absolute(path, length)) {
		string_const_t cwd = environment_current_working_directory();
		abspath = path_clean(path, length, capacity);
		abspath = path_prepend(STRING_ARGS(abspath), capacity, cwd.str, cwd.length);
		abspath = path_clean(STRING_ARGS(abspath), capacity);
	} else {
		abspath = path_clean(path, length, capacity);
	}

	// Path is cleaned, discard any "/../" paths
	abspath = string_replace(STRING_ARGS(abspath), capacity, STRING_CONST("/../"), STRING_CONST("/"), true);

	if (abspath.length >= 3 && (abspath.str[length - 3] == '/') && (abspath.str[length - 2] == '.') &&
	    (abspath.str[length - 1] == '.')) {
		if (abspath.length == 3)
			abspath.length = 1;
		else
			abspath.length -= 3;
	}

	if (abspath.str)
		abspath.str[abspath.length] = 0;

	return abspath;
}

string_t
path_allocate_absolute(const char* path, size_t length) {
	string_t buffer = string_thread_buffer();
	string_t result = string_copy(STRING_ARGS(buffer), path, length);
	result = path_absolute(STRING_ARGS(result), buffer.length);
	return string_clone(STRING_ARGS(result));
}

string_t
path_make_temporary(char* buffer, size_t capacity) {
	char uintbuffer[18];
	string_const_t tmpdir = environment_temporary_directory();
	string_t filename = string_from_uint(uintbuffer, 18, random64(), true, 0, '0');
	return path_concat(buffer, capacity, tmpdir.str, tmpdir.length, filename.str, filename.length);
}
