/* string.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if FOUNDATION_PLATFORM_WINDOWS
#if FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL || FOUNDATION_COMPILER_CLANG
#define snprintf(p, s, ...) _snprintf_s(p, s, _TRUNCATE, __VA_ARGS__)
#define vsnprintf(s, n, format, arg) _vsnprintf_s(s, n, _TRUNCATE, format, arg)
#define sscanf sscanf_s
#elif FOUNDATION_COMPILER_GCC
_CRTIMP int __cdecl __MINGW_NOTHROW
_strnicmp(const char*, const char*, size_t);
#include <sys/types.h>
#endif
#define strncasecmp _strnicmp
#endif

#include <time.h>

string_t
string_allocate(size_t length, size_t capacity) {
	char* str;
	if (!capacity)
		return (string_t){0, 0};
	FOUNDATION_ASSERT(length < capacity);
	str = memory_allocate(HASH_STRING, capacity, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	return (string_t){str, length};
}

void
string_deallocate(char* str) {
	memory_deallocate(str);
}

string_t
string_clone(const char* str, size_t length) {
	char* clone = memory_allocate(HASH_STRING, length + 1, 0, MEMORY_PERSISTENT);
	if (length) {
		FOUNDATION_ASSERT(str);
		memcpy(clone, str, length);
	}
	clone[length] = 0;
	return (string_t){clone, length};
}

string_t
string_clone_string(string_const_t str) {
	return string_clone(STRING_ARGS(str));
}

string_t
string_allocate_format(const char* format, size_t length, ...) {
	int n;
	size_t capacity, lastcapacity;
	char* buffer;
	va_list list;

	if (!length) {
		buffer = memory_allocate(HASH_STRING, 1, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		return (string_t){buffer, 0};
	}
	FOUNDATION_ASSERT(format);

	capacity = length + 32;
	buffer = memory_allocate(HASH_STRING, capacity, 0, MEMORY_PERSISTENT);

	while (true) {
		va_start(list, length);
		n = vsnprintf(buffer, capacity, format, list);
		va_end(list);

		if ((n > -1) && ((unsigned int)n < capacity))
			break;

		lastcapacity = capacity;
		if (n > -1)
			capacity = (unsigned int)(n + 1);
		else
			capacity *= 2;

		buffer = memory_reallocate(buffer, capacity, 0, lastcapacity, MEMORY_NO_PRESERVE);
	}

	return (string_t){buffer, (unsigned int)n};
}

string_t
string_format(char* buffer, size_t capacity, const char* format, size_t length, ...) {
	va_list list;
	int n;

	if (!capacity)
		return (string_t){buffer, 0};

	if (!length) {
		buffer[0] = 0;
		return (string_t){buffer, 0};
	}

	va_start(list, length);
	n = vsnprintf(buffer, capacity, format, list);
	va_end(list);

	if ((n > -1) && ((unsigned int)n < capacity))
		return (string_t){buffer, (unsigned int)n};
	return (string_t){buffer, capacity - 1};
}

string_t
string_allocate_vformat(const char* format, size_t length, va_list list) {
	int n;
	size_t capacity, lastcapacity;
	char* buffer;
	va_list copy_list;

	if (!length) {
		buffer = memory_allocate(HASH_STRING, 1, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		return (string_t){buffer, 0};
	}

	capacity = length + 32;
	buffer = memory_allocate(HASH_STRING, capacity, 0, MEMORY_PERSISTENT);

	while (true) {
		va_copy(copy_list, list);
		n = vsnprintf(buffer, capacity, format, copy_list);
		va_end(copy_list);

		if ((n > -1) && ((unsigned int)n < capacity))
			break;

		lastcapacity = capacity;
		if (n > -1)
			capacity = (unsigned int)(n + 1);
		else
			capacity *= 2;

		buffer = memory_reallocate(buffer, capacity, 0, lastcapacity, MEMORY_NO_PRESERVE);
	}

	return (string_t){buffer, (unsigned int)n};
}

string_t
string_vformat(char* buffer, size_t capacity, const char* format, size_t length, va_list list) {
	int n;
	va_list copy_list;

	if (!capacity)
		return (string_t){buffer, 0};

	if (!length) {
		buffer[0] = 0;
		return (string_t){buffer, 0};
	}

	va_copy(copy_list, list);
	n = vsnprintf(buffer, capacity, format, copy_list);
	va_end(copy_list);

	if ((n > -1) && ((unsigned int)n < capacity))
		return (string_t){buffer, (unsigned int)n};
	return (string_t){buffer, capacity - 1};
}

size_t
string_length(const char* str) {
	return str ? strlen(str) : 0;
}

hash_t
string_hash(const char* str, size_t length) {
	return length ? hash(str, length) : HASH_EMPTY_STRING;
}

string_t
string_resize(char* str, size_t length, size_t capacity, size_t new_length, char c) {
	FOUNDATION_ASSERT(length <= capacity);
	if (new_length >= capacity) {
		str = capacity ? memory_reallocate(str, new_length + 1, 0, capacity, 0) :
                         memory_allocate(HASH_STRING, new_length + 1, 0, MEMORY_PERSISTENT);
	}
	if (length < new_length)
		memset(str + length, c, new_length - length);
	str[new_length] = 0;
	return (string_t){str, new_length};
}

string_t
string_copy(char* FOUNDATION_RESTRICT dst, size_t capacity, const char* FOUNDATION_RESTRICT src, size_t length) {
	if (capacity) {
		if (length) {
			if (length >= capacity)
				length = capacity - 1;
			if (dst != src)
				memcpy(dst, src, length);
		}
		dst[length] = 0;
		return (string_t){dst, length};
	}
	return (string_t){dst, 0};
}

string_const_t
string_strip(const char* str, size_t length, const char* delimiters, size_t delim_length) {
	size_t start, end, newlength;

	if (!length)
		return (string_const_t){str, 0};

	start = string_find_first_not_of(str, length, delimiters, delim_length, 0);
	end = string_find_last_not_of(str, length, delimiters, delim_length, length - 1);

	if (start != STRING_NPOS) {
		// Should always at least find same as start
		FOUNDATION_ASSERT(end != STRING_NPOS);
		newlength = (end - start) + 1;
		return (string_const_t){str + start, newlength};
	}

	return (string_const_t){str, 0};
}

string_t
string_replace(char* str, size_t length, size_t capacity, const char* key, size_t key_length, const char* newkey,
               size_t newkey_length, bool repeat) {
	size_t pos, oldlength, lastpos, replaced, needsize;
	ssize_t lendiff;

	FOUNDATION_ASSERT(length <= capacity);
	if (!capacity || !length || !key_length)
		return (string_t){str, length};

	oldlength = length;
	lastpos = STRING_NPOS;
	lendiff = (ssize_t)newkey_length - (ssize_t)key_length;
	pos = 0;
	replaced = 0;

	while ((pos = string_find_string(str, length, key, key_length, pos)) != STRING_NPOS) {
		/*lint --e{571} */
		if (repeat && (lastpos != STRING_NPOS) && (lendiff > 0) && (pos <= (lastpos + (size_t)lendiff))) {
			// Avoid infinite loop - found position did not move ahead more than
			// newly introduced characters in the remaining part of the string
			pos = lastpos + newkey_length;
			continue;
		}

		++replaced;

		if (lendiff <= 0) {
			// String is reducing or keeping length, just overwrite
			memcpy(str + pos, newkey, newkey_length);
			if (lendiff < 0) {
				memmove(str + pos + newkey_length, str + pos + key_length, (length - (pos + key_length)) + 1);
				FOUNDATION_ASSERT(length >= (size_t)(-lendiff));
				length -= (size_t)(-lendiff);
			}
		} else {
			needsize = length + (size_t)lendiff + 1;
			if (needsize > capacity) {
				// Best effort
				size_t dest_clamp = pos + newkey_length;
				if (dest_clamp < capacity) {
					size_t size_clamp = (length - (pos + key_length)) + 1;
					if (dest_clamp + size_clamp >= capacity)
						size_clamp = capacity - (dest_clamp + 1);
					if (size_clamp > 0)
						memmove(str + dest_clamp, str + pos + key_length, size_clamp);
					memcpy(str + pos, newkey, newkey_length);
					length = capacity - 1;
				} else {
					size_t size_clamp = capacity - pos;
					if (size_clamp > 0)
						memcpy(str + pos, newkey, size_clamp);
				}
			} else {
				memmove(str + pos + newkey_length, str + pos + key_length, (length - (pos + key_length)) + 1);
				memcpy(str + pos, newkey, newkey_length);
				length += (size_t)lendiff;
			}
		}

		lastpos = pos;
		if (!repeat)
			pos += newkey_length;
	}

	if (replaced && (length != oldlength))
		str[length] = 0;

	return (string_t){str, length};
}

static string_t
string_append_fragment(char* str, size_t length, size_t capacity, const char* suffix, size_t suffix_length) {
	size_t total_length;
	if (!capacity)
		return (string_t){str, 0};
	if (!suffix_length)
		return (string_t){str, length};

	total_length = length + suffix_length;
	if (total_length >= capacity) {
		if (length >= capacity)
			length = capacity - 1;
		total_length = capacity - 1;
		suffix_length = capacity - (length + 1);
	}

	if (suffix_length)
		memcpy(str + length, suffix, suffix_length);
	str[total_length] = 0;

	return (string_t){str, total_length};
}

string_t
string_append(char* str, size_t length, size_t capacity, const char* suffix, size_t suffix_length) {
	return string_append_fragment(str, length, capacity, suffix, suffix_length);
}

string_t
string_append_varg(char* str, size_t length, size_t capacity, const char* suffix, size_t suffix_length, ...) {
	va_list list;
	string_t result = string_append_fragment(str, length, capacity, suffix, suffix_length);
	va_start(list, suffix_length);
	result = string_append_vlist(STRING_ARGS(result), capacity, list);
	va_end(list);
	return result;
}

string_t
string_append_vlist(char* str, size_t length, size_t capacity, va_list list) {
	va_list clist;
	char* ptr;
	size_t psize;
	string_t result = {str, length};

	va_copy(clist, list);
	do {
		ptr = va_arg(clist, void*);
		if (ptr) {
			psize = va_arg(clist, size_t);
			if (psize)
				result = string_append_fragment(STRING_ARGS(result), capacity, ptr, psize);
		}
	} while (ptr);
	va_end(clist);

	return result;
}

static string_t
string_prepend_fragment(char* str, size_t length, size_t capacity, const char* prefix, size_t prefix_length) {
	size_t total_length, prefix_offset, prefix_mod;
	if (!capacity)
		return (string_t){str, 0};
	if (!prefix_length)
		return (string_t){str, length};

	total_length = length + prefix_length;
	prefix_offset = 0;
	if (total_length >= capacity) {
		if (length >= capacity)
			length = capacity - 1;
		total_length = capacity - 1;
		prefix_mod = capacity - (length + 1);
		prefix_offset = prefix_length - prefix_mod;
		prefix_length = prefix_mod;
	}

	if (length && prefix_length)
		memmove(str + prefix_length, str, length);
	if (prefix_length)
		memcpy(str, prefix + prefix_offset, prefix_length);
	str[total_length] = 0;  // clamped to capacity, which is > 0

	return (string_t){str, total_length};
}

string_t
string_prepend(char* str, size_t length, size_t capacity, const char* prefix, size_t prefix_length) {
	return string_prepend_fragment(str, length, capacity, prefix, prefix_length);
}

string_t
string_prepend_varg(char* str, size_t length, size_t capacity, const char* prefix, size_t prefix_length, ...) {
	va_list list;
	string_t result = string_prepend_fragment(str, length, capacity, prefix, prefix_length);
	va_start(list, prefix_length);
	result = string_prepend_vlist(STRING_ARGS(result), capacity, list);
	va_end(list);
	return result;
}

string_t
string_prepend_vlist(char* str, size_t length, size_t capacity, va_list list) {
	va_list clist;
	char* ptr;
	size_t psize;
	string_t result = {str, length};

	// TODO: Pre-calculate fragment positions and move directly into place to avoid multiple
	// memmoves of same data
	va_copy(clist, list);
	do {
		ptr = va_arg(clist, void*);
		if (ptr) {
			psize = va_arg(clist, size_t);
			if (psize)
				result = string_prepend_fragment(STRING_ARGS(result), capacity, ptr, psize);
		}
	} while (ptr);
	va_end(clist);

	return result;
}

string_t
string_allocate_concat(const char* prefix, size_t prefix_length, const char* suffix, size_t suffix_length) {
	char* buf;
	buf = memory_allocate(HASH_STRING, prefix_length + suffix_length + 1, 0, MEMORY_PERSISTENT);
	if (prefix_length)
		memcpy(buf, prefix, prefix_length);
	if (suffix_length)
		memcpy(buf + prefix_length, suffix, suffix_length);
	buf[prefix_length + suffix_length] = 0;
	return (string_t){buf, prefix_length + suffix_length};
}

string_t
string_allocate_concat_varg(const char* prefix, size_t prefix_length, const char* suffix, size_t suffix_length, ...) {
	va_list list;
	size_t length, total_length, psize;
	char* buf;
	void* ptr;

	total_length = prefix_length + suffix_length;
	va_start(list, suffix_length);
	do {
		ptr = va_arg(list, void*);
		if (ptr) {
			psize = va_arg(list, size_t);
			if (psize)
				total_length += psize;
		}
	} while (ptr);
	va_end(list);

	buf = memory_allocate(HASH_STRING, total_length + 1, 0, MEMORY_PERSISTENT);
	if (prefix_length) {
		FOUNDATION_ASSERT(prefix);
		memcpy(buf, prefix, prefix_length);
	}
	if (suffix_length) {
		FOUNDATION_ASSERT(suffix);
		memcpy(buf + prefix_length, suffix, suffix_length);
	}
	length = prefix_length + suffix_length;
	va_start(list, suffix_length);
	do {
		ptr = va_arg(list, void*);
		if (ptr) {
			psize = va_arg(list, size_t);
			if (psize) {
				memcpy(buf + length, ptr, psize);
				length += psize;
			}
		}
	} while (ptr);
	va_end(list);

	buf[total_length] = 0;
	return (string_t){buf, total_length};
}

string_t
string_allocate_concat_vlist(va_list list) {
	va_list clist;
	size_t length, total_length, psize;
	char* buf;
	void* ptr;

	total_length = 0;
	va_copy(clist, list);
	do {
		ptr = va_arg(clist, void*);
		if (ptr) {
			psize = va_arg(clist, size_t);
			if (psize)
				total_length += psize;
		}
	} while (ptr);
	va_end(clist);

	buf = memory_allocate(HASH_STRING, total_length + 1, 0, MEMORY_PERSISTENT);
	length = 0;
	va_copy(clist, list);  // lint !e838
	do {
		ptr = va_arg(clist, void*);
		if (ptr) {
			psize = va_arg(clist, size_t);
			if (psize) {
				memcpy(buf + length, ptr, psize);
				length += psize;
			}
		}
	} while (ptr);
	va_end(clist);

	buf[total_length] = 0;
	return (string_t){buf, total_length};
}

string_t
string_concat(char* str, size_t capacity, const char* prefix, size_t prefix_length, const char* suffix,
              size_t suffix_length) {
	string_t result = string_copy(str, capacity, prefix, prefix_length);
	return string_append(STRING_ARGS(result), capacity, suffix, suffix_length);
}

string_t
string_concat_varg(char* str, size_t capacity, const char* prefix, size_t prefix_length, const char* suffix,
                   size_t suffix_length, ...) {
	va_list list;
	string_t result = string_copy(str, capacity, prefix, prefix_length);
	result = string_append(STRING_ARGS(result), capacity, suffix, suffix_length);
	va_start(list, suffix_length);
	result = string_append_vlist(STRING_ARGS(result), capacity, list);
	va_end(list);
	return result;
}

string_t
string_concat_vlist(char* str, size_t capacity, va_list list) {
	va_list clist;
	void* ptr;
	size_t psize;
	string_t result = {str, 0};
	va_copy(clist, list);
	do {
		ptr = va_arg(clist, void*);
		if (ptr) {
			psize = va_arg(clist, size_t);
			if (psize)
				result = string_append_fragment(STRING_ARGS(result), capacity, ptr, psize);
		}
	} while (ptr);
	va_end(clist);

	return result;
}

void
string_split(const char* str, size_t length, const char* separators, size_t sep_length, string_const_t* left,
             string_const_t* right, bool allowempty) {
	size_t start, delim;

	start = (allowempty ? 0 : string_find_first_not_of(str, length, separators, sep_length, 0));
	if (start == STRING_NPOS) {
		if (left)
			*left = string_const(str, 0);
		if (right)
			*right = string_const(str, 0);
		return;
	}

	delim = string_find_first_of(str, length, separators, sep_length, start);
	if (delim != STRING_NPOS) {
		if (left)
			*left = string_const(str + start, delim - start);
		if (right) {
			delim = string_find_first_not_of(str, length, separators, sep_length, delim);
			if (delim != STRING_NPOS)
				*right = string_const(str + delim, length - delim);
			else
				*right = string_const(str, 0);
		}
	} else {
		if (left)
			*left = string_const(str + start, length - start);
		if (right)
			*right = string_const(str + length, 0);
	}
}

string_const_t
string_substr(const char* str, size_t length, size_t offset, size_t sub_length) {
	if (offset < length) {
		size_t end = offset + sub_length;
		if ((end < length) && (sub_length < length))
			return (string_const_t){str + offset, sub_length};
		return (string_const_t){str + offset, length - offset};
	}
	return string_null();
}

size_t
string_find(const char* str, size_t length, char c, size_t offset) {
	const void* found;
	if (offset >= length)
		return STRING_NPOS;
	found = memchr(str + offset, c, length - offset);
	if (found)
		return (size_t)pointer_diff(found, str);
	return STRING_NPOS;
}

size_t
string_find_string(const char* str, size_t length, const char* key, size_t key_length, size_t offset) {
	const char* found;
	char keychar;
	size_t last_offset;
	if (!key_length)
		return offset;
	if ((key_length > length) || (offset > (length - key_length)))
		return STRING_NPOS;

	last_offset = length - key_length;

	do {
		keychar = *key;
		found = memchr(str + offset, keychar, 1 + last_offset - offset);
		if (!found)
			break;

		if (memcmp(found, key, key_length) == 0)
			return (size_t)pointer_diff(found, str);

		offset = 1 + (size_t)pointer_diff(found, str);

	} while (offset <= last_offset);

	return STRING_NPOS;
}

size_t
string_rfind(const char* str, size_t length, char c, size_t offset) {
	if (offset >= length)
		offset = length - 1;  // zero length will wrap around

	// Wrap-around terminates
	while (offset != STRING_NPOS) {
		if (c == str[offset])
			return offset;
		--offset;
	}

	return STRING_NPOS;
}

size_t
string_rfind_string(const char* str, size_t length, const char* key, size_t key_length, size_t offset) {
	if (key_length > length)
		return STRING_NPOS;
	if (!key_length)
		return offset > length ? length : offset;

	if (offset >= length - key_length)
		offset = length - key_length;

	// Wrap-around terminates
	while (offset != STRING_NPOS) {
		if (memcmp(str + offset, key, key_length) == 0)
			return offset;
		--offset;
	}
	return STRING_NPOS;
}

size_t
string_find_first_of(const char* str, size_t length, const char* tokens, size_t token_length, size_t offset) {
	if (!token_length)
		return STRING_NPOS;

	while (offset < length) {
		if (string_find(tokens, token_length, str[offset], 0) != STRING_NPOS)
			return offset;
		++offset;
	}
	return STRING_NPOS;
}

size_t
string_find_last_of(const char* str, size_t length, const char* tokens, size_t token_length, size_t offset) {
	if (!token_length)
		return STRING_NPOS;
	if (offset >= length)
		offset = length - 1;

	// Wrap-around terminates
	while (offset != STRING_NPOS) {
		if (string_find(tokens, token_length, str[offset], 0) != STRING_NPOS)
			return offset;
		--offset;
	}

	return STRING_NPOS;
}

size_t
string_find_first_not_of(const char* str, size_t length, const char* tokens, size_t token_length, size_t offset) {
	if (offset >= length)
		return STRING_NPOS;
	if (!token_length)
		return offset;

	while (offset < length) {
		if (string_find(tokens, token_length, str[offset], 0) == STRING_NPOS)
			return offset;
		++offset;
	}
	return STRING_NPOS;
}

size_t
string_find_last_not_of(const char* str, size_t length, const char* tokens, size_t token_length, size_t offset) {
	if (offset >= length)
		offset = length - 1;
	if (!token_length)
		return offset;

	// Wrap-around terminates
	while (offset != STRING_NPOS) {
		if (string_find(tokens, token_length, str[offset], 0) == STRING_NPOS)
			return offset;
		--offset;
	}

	return STRING_NPOS;
}

bool
string_ends_with(const char* str, size_t length, const char* suffix, size_t suffix_length) {
	if (length < suffix_length)
		return false;
	if (!suffix_length)
		return true;
	return (memcmp(str + (length - suffix_length), suffix, suffix_length) == 0);
}

bool
string_equal(const char* rhs, size_t rhs_length, const char* lhs, size_t lhs_length) {
	if (rhs_length && (lhs_length == rhs_length)) {
		return (memcmp(rhs, lhs, rhs_length) == 0);
	}
	return (!rhs_length && !lhs_length);
}

bool
string_equal_nocase(const char* rhs, size_t rhs_length, const char* lhs, size_t lhs_length) {
	if (rhs_length && lhs_length) {
		return (rhs_length == lhs_length) && (strncasecmp(rhs, lhs, rhs_length) == 0);
	}
	return (!rhs_length && !lhs_length);
}

bool
string_equal_substr(const char* rhs, size_t rhs_length, size_t rhs_offset, const char* lhs, size_t lhs_length,
                    size_t lhs_offset) {
	size_t rhs_remain = (rhs_offset < rhs_length) ? (rhs_length - rhs_offset) : 0;
	size_t lhs_remain = (lhs_offset < lhs_length) ? (lhs_length - lhs_offset) : 0;
	return string_equal(rhs + rhs_offset, rhs_remain, lhs + lhs_offset, lhs_remain);
}

bool
string_equal_substr_nocase(const char* rhs, size_t rhs_length, size_t rhs_offset, const char* lhs, size_t lhs_length,
                           size_t lhs_offset) {
	size_t rhs_remain = (rhs_offset < rhs_length) ? (rhs_length - rhs_offset) : 0;
	size_t lhs_remain = (lhs_offset < lhs_length) ? (lhs_length - lhs_offset) : 0;
	return string_equal_nocase(rhs + rhs_offset, rhs_remain, lhs + lhs_offset, lhs_remain);
}

bool
string_match_pattern(const char* element, size_t element_length, const char* pattern, size_t pattern_length) {
	if (!pattern_length || ((*pattern == '*') && (pattern_length == 1)))
		return true;

	if (*pattern == '*') {
		if (string_match_pattern(element, element_length, pattern + 1, pattern_length - 1))
			return true;
		if (element_length)
			return string_match_pattern(element + 1, element_length - 1, pattern, pattern_length);
	}

	if (!element_length)
		return false;

	if (*pattern == '?')
		return string_match_pattern(element + 1, element_length - 1, pattern + 1, pattern_length - 1);
	else if (*element == *pattern)
		return string_match_pattern(element + 1, element_length - 1, pattern + 1, pattern_length - 1);

	return false;
}

size_t
string_explode(const char* str, size_t length, const char* delimiters, size_t delim_length, string_const_t* arr,
               size_t arrsize, bool allow_empty) {
	size_t token;
	size_t end;
	size_t count = 0;

	if (!length || !arrsize)
		return 0;

	if (!delim_length) {
		arr[count++] = string_const(str, length);
		return count;
	}

	token = 0;
	end = 0;

	while ((end < length) && (count < arrsize)) {
		if (!allow_empty)
			token = string_find_first_not_of(str, length, delimiters, delim_length, end);
		end = string_find_first_of(str, length, delimiters, delim_length, token);

		if (token != STRING_NPOS)
			arr[count++] = string_const(str + token, (end != STRING_NPOS) ? (end - token) : (length - token));
		if (allow_empty)
			token = end + 1;
	}

	return count;
}

string_t
string_merge(char* dst, size_t capacity, const string_const_t* array, size_t num, const char* delimiter,
             size_t delim_length) {
	string_t result;
	size_t i, limit;

	if (!num || !capacity) {
		if (capacity)
			dst[0] = 0;
		return (string_t){dst, 0};
	}

	--capacity;
	result.str = dst;
	result.length = 0;
	for (i = 0; i < (num - 1); ++i) {
		limit = capacity - result.length;
		if (array[i].length < limit)
			limit = array[i].length;
		if (limit)
			memcpy(result.str + result.length, array[i].str, limit);
		result.length += limit;

		limit = capacity - result.length;
		if (delim_length < limit)
			limit = delim_length;
		if (limit)
			memcpy(result.str + result.length, delimiter, limit);
		result.length += limit;
	}

	limit = capacity - result.length;
	if (array[i].length < limit)
		limit = array[i].length;
	if (limit)
		memcpy(result.str + result.length, array[i].str, limit);
	result.length += limit;
	result.str[result.length] = 0;

	return result;
}

string_t
string_merge_varg(char* dst, size_t capacity, const char* delimiter, size_t delim_length, const char* str,
                  size_t length, ...) {
	va_list list;
	string_t result;
	size_t prelimit, premerge;

	if (!capacity)
		return (string_t){dst, 0};

	result = string_copy(dst, capacity, str, length);
	if (result.length >= capacity - 1)
		return result;

	prelimit = result.length;
	result = string_append(STRING_ARGS(result), capacity, delimiter, delim_length);

	va_start(list, length);
	premerge = result.length;
	result = string_merge_vlist(result.str + result.length, capacity - result.length, delimiter, delim_length, list);
	result.str = dst;
	if (!result.length) {
		size_t newlength = prelimit;
		void* ptr = va_arg(list, void*);
		if (ptr)
			newlength = premerge;
		result.length = newlength;
		result.str[result.length] = 0;
	} else {
		result.length += premerge;
	}
	va_end(list);

	return result;
}

string_t
string_merge_vlist(char* dst, size_t capacity, const char* delimiter, size_t delim_length, va_list list) {
	va_list clist;
	string_t result;
	size_t i, limit, psize;
	void* ptr;

	if (!capacity)
		return (string_t){dst, 0};

	--capacity;
	result.str = dst;
	result.length = 0;
	i = 0;
	va_copy(clist, list);
	do {
		ptr = va_arg(clist, void*);
		if (ptr) {
			psize = va_arg(clist, size_t);

			if (i > 0) {
				limit = capacity - result.length;
				if (delim_length < limit)
					limit = delim_length;
				if (limit) {
					memcpy(result.str + result.length, delimiter, limit);
					result.length += limit;
				}
			}

			limit = capacity - result.length;
			if (psize < limit)
				limit = psize;
			if (!limit)
				break;
			memcpy(result.str + result.length, ptr, limit);
			result.length += limit;

			++i;
		}
	} while (ptr);

	result.str[result.length] = 0;
	return result;
}

void
string_array_deallocate_elements(string_t* array) {
	size_t i;
	size_t size = array_size(array);
	for (i = 0; i < size; ++i)
		string_deallocate(array[i].str);  // lint !e613
}

ssize_t
string_array_find(const string_const_t* array, size_t haystack_size, const char* needle, size_t needle_length) {
	size_t i;
	for (i = 0; i < haystack_size; ++i, ++array) {
		if (string_equal(array->str, array->length, needle, needle_length))
			return (ssize_t)i;
	}
	return -1;
}

#define get_bit_mask(bits) ((1U << (bits)) - 1)

static size_t
get_utf8_bytes_count(uint8_t lead) {
	if ((lead & 0xFE) == 0xFC)
		return 6;
	else if ((lead & 0xFC) == 0xF8)
		return 5;
	else if ((lead & 0xF8) == 0xF0)
		return 4;
	else if ((lead & 0xF0) == 0xE0)
		return 3;
	else if ((lead & 0xE0) == 0xC0)
		return 2;
	else
		return 1;
}

static size_t
get_bytes_count_as_utf8(uint32_t val) {
	if (val >= 0x04000000)
		return 6;
	else if (val >= 0x00200000)
		return 5;
	else if (val >= 0x00010000)
		return 4;
	else if (val >= 0x00000800)
		return 3;
	else if (val >= 0x00000080)
		return 2;
	return 1;
}

static size_t
encode_utf8(char* str, uint32_t val) {
	size_t num, j;

	if (val < 0x80) {
		*str = (char)val;
		return 1;
	}

	// Get number of _extra_ bytes
	num = get_bytes_count_as_utf8(val) - 1;

	*str++ = (char)((0x80U | (get_bit_mask(num) << (7 - num))) | ((val >> (6 * num)) & get_bit_mask(6U - num)));
	for (j = 1; j <= num; ++j)
		*str++ = (char)(0x80U | ((val >> (6 * (num - j))) & 0x3F));

	return num + 1;
}

uint32_t
string_glyph(const char* str, size_t length, size_t offset, size_t* consumed) {
	uint32_t glyph;
	unsigned char ext;
	size_t num, j;
	const char* cur;
	const char* end;

	if (offset >= length) {
		if (consumed)
			*consumed = 0;
		return 0;
	}

	cur = str + offset;
	if (!(*cur & 0x80)) {
		glyph = (unsigned char)*cur++;
		if (consumed)
			*consumed = 1;
	} else {
		// Convert through UTF-32
		end = str + length;
		ext = (unsigned char)*cur;
		num = get_utf8_bytes_count(ext) - 1;  // Subtract one to get number of _extra_ bytes
		glyph = ((uint32_t)ext & get_bit_mask(6 - num)) << (6 * num);
		++cur;
		for (j = 1; (j <= num) && (cur < end); ++j, ++cur) {
			ext = (unsigned char)*cur;
			glyph |= ((uint32_t)ext & 0x3F) << (6 * (num - j));
		}
		if (consumed)
			*consumed = j;
	}
	return glyph;
}

size_t
string_glyphs(const char* str, size_t length) {
	const char* end = pointer_offset_const(str, length);
	size_t num = 0;
	while (str && (str < end)) {
		++num;
		// Will catch invalid utf-8 sequences by overflowing str < end terminator
		str += get_utf8_bytes_count((uint8_t)*str);
	}
	return num;
}

int
string_glyph_length(const char* str, size_t length) {
	if (!length)
		return -1;
	const uint8_t* byte = (const uint8_t*)str;
	size_t byte_count = get_utf8_bytes_count(*byte);
	if (byte_count > length)
		return -1;
	for (uint ibyte = 1; ibyte < byte_count; ++ibyte) {
		if ((byte[ibyte] & 0xC0) != 0x80)
			return -1;
	}
	return (int)byte_count;
}

wchar_t*
wstring_allocate_from_string(const char* cstr, size_t length) {
	wchar_t* buffer;
	wchar_t* dest;
	size_t chars_count, bytes_count, i, j;
	uint32_t glyph;
	unsigned char ext;
	const char* cur;
	const char* end;

	if (!length) {
		buffer = memory_allocate(HASH_STRING, sizeof(wchar_t), 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		return buffer;
	}

	// Count number of wchar_t needed to represent string
	chars_count = 0;
	cur = cstr;
	end = cstr + length;
	for (i = 0; (i < length) && (cur < end);) {
		bytes_count = get_utf8_bytes_count((uint8_t)(*cur));
#if FOUNDATION_SIZE_WCHAR == 2
		if (bytes_count >= 4)
			chars_count += 2;  // final glyph > 0xFFFF
		else
			++chars_count;
#else
		++chars_count;  // wchar_t == UTF-32
#endif
		cur += bytes_count;
		i += bytes_count;
	}

	buffer = memory_allocate(HASH_STRING, sizeof(wchar_t) * (chars_count + 1), 0,
	                         MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

	dest = buffer;
	cur = cstr;
	for (i = 0; (i < length) && (cur < end);) {
		if (!(*cur & 0x80))
			*dest++ = (unsigned char)*cur++;
		else {
			// Convert through UTF-32
			ext = (unsigned char)*cur;
			bytes_count = get_utf8_bytes_count(ext) - 1;  // Subtract one to get number of _extra_ bytes
			glyph = ((uint32_t)ext & get_bit_mask(6 - bytes_count)) << (6 * bytes_count);
			++cur;
			for (j = 1; (j <= bytes_count) && (cur < end); ++j, ++cur) {
				ext = (unsigned char)*cur;
				glyph |= ((uint32_t)ext & 0x3F) << (6 * (bytes_count - j));
			}
#if FOUNDATION_SIZE_WCHAR == 2
			FOUNDATION_ASSERT((glyph < 0xD800) || (glyph > 0xDFFF));
			// FOUNDATION_ASSERT(glyph <= 0x10FFFF);
			if ((glyph < 0xD800) || (glyph > 0xDFFF)) {
				if (glyph <= 0xFFFF)
					*dest++ = (wchar_t)glyph;
				else if (glyph <= 0x10FFFF) {
					uint32_t val = glyph - 0x10000;
					*dest++ = (wchar_t)(0xD800 | ((val >> 10) & 0x3FF));
					*dest++ = (wchar_t)(0xDC00 | (val & 0x3FF));
				}
			}
#else
			*dest++ = (wchar_t)glyph;
#endif
			i += bytes_count;
		}
	}

	*dest = 0;

	return buffer;
}

void
wstring_from_string(wchar_t* dest, size_t capacity, const char* source, size_t length) {
	size_t i, j, num;
	uint32_t glyph;
	unsigned char ext;
	wchar_t* last = dest + (capacity - 1);
	const char* cur = source;
	const char* end = source + length;

	if (!capacity)
		return;

	/*lint -e{850} */
	for (i = 0; (i < length) && (cur < end) && (dest < last); ++i) {
		if (!(*cur & 0x80))
			*dest++ = (unsigned char)*cur++;
		else {
			// Convert through UTF-32
			ext = (unsigned char)(*cur);
			num = get_utf8_bytes_count(ext) - 1;  // Subtract one to get number of _extra_ bytes
			glyph = ((uint32_t)ext & get_bit_mask(6 - num)) << (6 * num);
			++cur;
			for (j = 1; (j <= num) && (cur < end); ++j, ++cur) {
				ext = (unsigned char)(*cur);
				glyph |= ((uint32_t)ext & 0x3F) << (6 * (num - j));
			}
#if FOUNDATION_SIZE_WCHAR == 2
			FOUNDATION_ASSERT((glyph < 0xD800) || (glyph > 0xDFFF));
			FOUNDATION_ASSERT(glyph <= 0x10FFFF);
			if ((glyph < 0xD800) || (glyph > 0xDFFF)) {
				if (glyph <= 0xFFFF)
					*dest++ = (wchar_t)glyph;
				else if (glyph <= 0x10FFFF) {
					uint32_t val = glyph - 0x10000;
					if (dest < (last - 1)) {
						*dest++ = (wchar_t)(0xD800 | ((val >> 10) & 0x3FF));
						*dest++ = (wchar_t)(0xDC00 | (val & 0x3FF));
					} else
						break;
				}
			}
#else
			*dest++ = (wchar_t)glyph;
#endif
			i += num;
		}
	}

	*dest = 0;
}

void
wstring_deallocate(wchar_t* str) {
	memory_deallocate(str);
}

size_t
wstring_length(const wchar_t* str) {
	return str ? wcslen(str) : 0;
}

bool
wstring_equal(const wchar_t* lhs, const wchar_t* rhs) {
	return wcscmp(lhs, rhs) == 0;
}

string_t
string_allocate_from_wstring(const wchar_t* str, size_t length) {
#if FOUNDATION_SIZE_WCHAR == 2
	return string_allocate_from_utf16((const uint16_t*)str, length);
#else
	return string_allocate_from_utf32((const uint32_t*)str, length);
#endif
}

string_t
string_allocate_from_utf16(const uint16_t* str, size_t length) {
	bool swap;
	char* buf;
	size_t i, curlen;
	uint32_t glyph, lval;

	curlen = 0;
	swap = false;

	/*lint -e{850} */
	for (i = 0; i < length; ++i) {
		glyph = str[i];
		if ((glyph == 0xFFFE) || (glyph == 0xFEFF)) {
			swap = (glyph != 0xFEFF);
			continue;  // BOM
		}
		if (swap)
			glyph = byteorder_swap16((uint16_t)glyph);
		if ((glyph >= 0xD800) && (glyph <= 0xDFFF)) {
			++i;
			lval = str[i];
			if (swap)
				lval = byteorder_swap16((uint16_t)lval);
			glyph = ((((glyph & 0x3FF) << 10) | (lval & 0x3FF)) + 0x10000);
		}

		curlen += get_bytes_count_as_utf8(glyph);
	}

	buf = memory_allocate(HASH_STRING, (curlen + 1), 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

	string_convert_utf16(buf, curlen + 1, str, length);

	return (string_t){buf, curlen};
}

string_t
string_allocate_from_utf32(const uint32_t* str, size_t length) {
	bool swap;
	char* buf;
	size_t i, curlen;
	uint32_t glyph;

	curlen = 0;
	swap = false;

	/*lint -e{850} */
	for (i = 0; i < length; ++i) {
		glyph = str[i];
		if ((glyph == 0x0000FEFF) || (glyph == 0xFFFE0000)) {
			swap = (glyph != 0x0000FEFF);
			continue;  // BOM
		}
		if (swap)
			glyph = byteorder_swap32(glyph);
		curlen += get_bytes_count_as_utf8(glyph);
	}

	buf = memory_allocate(HASH_STRING, (curlen + 1), 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

	string_convert_utf32(buf, curlen + 1, str, length);

	return (string_t){buf, curlen};
}

string_t
string_convert_utf16(char* dst, size_t capacity, const uint16_t* src, size_t length) {
	bool swap = false;
	uint32_t glyph, lval;
	size_t curlen = 0, numbytes = 0;
	size_t i;

	/*lint -e{850} */
	for (i = 0; (i < length) && (curlen < capacity); ++i) {
		// Convert through full UTF-32
		glyph = src[i];
		if ((glyph == 0xFFFE) || (glyph == 0xFEFF)) {
			swap = (glyph != 0xFEFF);
			continue;  // BOM
		}
		if (swap)
			glyph = byteorder_swap16((uint16_t)glyph);
		if ((glyph >= 0xD800) && (glyph <= 0xDFFF)) {
			++i;
			if (i >= length)
				continue;
			lval = src[i];
			if (swap)
				lval = byteorder_swap16((uint16_t)lval);
			glyph = ((((glyph & 0x3FF) << 10) | (lval & 0x3FF)) + 0x10000);
		}

		numbytes = get_bytes_count_as_utf8(glyph);
		if ((curlen + numbytes) < capacity)
			curlen += encode_utf8(dst + curlen, glyph);
	}

	dst[curlen] = 0;
	return (string_t){dst, curlen};
}

string_t
string_convert_utf32(char* dst, size_t capacity, const uint32_t* src, size_t length) {
	bool swap;
	uint32_t glyph;
	size_t curlen = 0, numbytes = 0;
	size_t i;

	swap = false;

	for (i = 0; (i < length) && (curlen < capacity); ++i) {
		glyph = src[i];
		if ((glyph == 0x0000FEFF) || (glyph == 0xFFFE0000)) {
			swap = (glyph != 0x0000FEFF);
			continue;  // BOM
		}
		if (swap)
			glyph = byteorder_swap32(glyph);

		numbytes = get_bytes_count_as_utf8(glyph);
		if ((curlen + numbytes) < capacity)
			curlen += encode_utf8(dst + curlen, glyph);
	}

	dst[curlen] = 0;
	return (string_t){dst, curlen};
}

#if BUILD_MAX_PATHLEN > 132
#define THREAD_BUFFER_SIZE BUILD_MAX_PATHLEN
#else
#define THREAD_BUFFER_SIZE 132
#endif
FOUNDATION_DECLARE_THREAD_LOCAL_ARRAY(char, convert_buffer, THREAD_BUFFER_SIZE)

string_t
string_from_int(char* buffer, size_t capacity, int64_t val, unsigned int width, char fill) {
	int len;
	if (!capacity)
		return (string_t){buffer, 0};
	len = snprintf(buffer, capacity, "%" PRId64, val);
	if ((unsigned int)len >= capacity) {
		buffer[capacity - 1] = 0;
		return (string_t){buffer, capacity - 1};
	}
	if (width >= capacity)
		width = (unsigned int)capacity - 1;
	if ((unsigned int)len < width) {
		size_t diff = width - (unsigned int)len;
		memmove(buffer + diff, buffer, (unsigned int)(len + 1));
		memset(buffer, fill, diff);
		return (string_t){buffer, width};
	}
	return (string_t){buffer, (unsigned int)len};
}

string_const_t
string_from_int_static(int64_t val, unsigned int width, char fill) {
	return string_to_const(string_from_int(get_thread_convert_buffer(), THREAD_BUFFER_SIZE, val, width, fill));
}

string_t
string_from_uint(char* buffer, size_t capacity, uint64_t val, bool hex, unsigned int width, char fill) {
	int len;
	if (!capacity)
		return (string_t){buffer, 0};
	len = snprintf(buffer, capacity, hex ? "%" PRIx64 : "%" PRIu64, val);
	if ((unsigned int)len >= capacity) {
		buffer[capacity - 1] = 0;
		return (string_t){buffer, capacity - 1};
	}
	if (width >= capacity)
		width = (unsigned int)capacity - 1;
	if ((unsigned int)len < width) {
		size_t diff = width - (unsigned int)len;
		memmove(buffer + diff, buffer, (unsigned int)(len + 1));
		memset(buffer, fill, diff);
		return (string_t){buffer, width};
	}
	return (string_t){buffer, (unsigned int)len};
}

string_const_t
string_from_uint_static(uint64_t val, bool hex, unsigned int width, char fill) {
	return string_to_const(string_from_uint(get_thread_convert_buffer(), THREAD_BUFFER_SIZE, val, hex, width, fill));
}

string_t
string_from_uint128(char* buffer, size_t capacity, const uint128_t val) {
	int len;
	if (!capacity)
		return (string_t){buffer, 0};
	len = snprintf(buffer, capacity, "%016" PRIx64 "%016" PRIx64, val.word[0], val.word[1]);
	if ((unsigned int)len >= capacity) {
		buffer[capacity - 1] = 0;
		return (string_t){buffer, capacity - 1};
	}
	return (string_t){buffer, (unsigned int)len};
}

string_const_t
string_from_uint128_static(const uint128_t val) {
	return string_to_const(string_from_uint128(get_thread_convert_buffer(), THREAD_BUFFER_SIZE, val));
}

string_t
string_from_uint256(char* buffer, size_t capacity, const uint256_t val) {
	int len;
	if (!capacity)
		return (string_t){buffer, 0};
	len = snprintf(buffer, capacity, "%016" PRIx64 "%016" PRIx64 "%016" PRIx64 "%016" PRIx64, val.word[0], val.word[1],
	               val.word[2], val.word[3]);
	if ((unsigned int)len >= capacity) {
		buffer[capacity - 1] = 0;
		return (string_t){buffer, capacity - 1};
	}
	return (string_t){buffer, (unsigned int)len};
}

string_const_t
string_from_uint256_static(const uint256_t val) {
	return string_to_const(string_from_uint256(get_thread_convert_buffer(), THREAD_BUFFER_SIZE, val));
}

string_t
string_from_uint512(char* buffer, size_t capacity, const uint512_t val) {
	int len;
	if (!capacity)
		return (string_t){buffer, 0};
	len = snprintf(
	    buffer, capacity,
	    "%016" PRIx64 "%016" PRIx64 "%016" PRIx64 "%016" PRIx64 "%016" PRIx64 "%016" PRIx64 "%016" PRIx64 "%016" PRIx64,
	    val.word[0], val.word[1], val.word[2], val.word[3], val.word[4], val.word[5], val.word[6], val.word[7]);
	if ((unsigned int)len >= capacity) {
		buffer[capacity - 1] = 0;
		return (string_t){buffer, capacity - 1};
	}
	return (string_t){buffer, (unsigned int)len};
}

string_const_t
string_from_uint512_static(const uint512_t val) {
	return string_to_const(string_from_uint512(get_thread_convert_buffer(), THREAD_BUFFER_SIZE, val));
}

string_t
string_from_real(char* buffer, size_t capacity, real val, unsigned int precision, unsigned int width, char fill) {
#if FOUNDATION_SIZE_REAL == 8
	return string_from_float64(buffer, capacity, val, precision, width, fill);
#else
	return string_from_float32(buffer, capacity, val, precision, width, fill);
#endif
}

string_t
string_from_float32(char* buffer, size_t capacity, float32_t val, unsigned int precision, unsigned int width,
                    char fill) {
	unsigned int ulen;
	size_t end;
	int len = -1;
	if (!capacity)
		return (string_t){buffer, 0};
	if (precision)
		len = snprintf(buffer, capacity, "%.*g", precision, (double)val);
	else
		len = snprintf(buffer, capacity, "%.*g", FLT_DECIMAL_DIG, (double)val);

	ulen = (unsigned int)len;
	if ((len < 0) || (ulen >= capacity)) {
		buffer[capacity - 1] = 0;
		return (string_t){buffer, capacity - 1};
	}
	if (width >= capacity)
		width = (unsigned int)capacity - 1;

	end = string_find_last_not_of(buffer, ulen, STRING_CONST("0"), STRING_NPOS);
	if (end != STRING_NPOS) {
		if (buffer[end] == '.')
			--end;
		if (end != (ulen - 1)) {
			++end;
			ulen = (unsigned int)end;
			buffer[end] = 0;
		}
	}

	// Some cleanups
	if (string_equal(buffer, ulen, "-0", 2) || string_equal(buffer, ulen, "-", 1)) {
		buffer[0] = '0';
		buffer[1] = 0;
		ulen = 1;
	}

	if (ulen < width) {
		unsigned int ofs = width - ulen;
		unsigned int copylen = ulen + 1;
		memmove(buffer + ofs, buffer, copylen);
		memset(buffer, fill, ofs);
		ulen = width;
	}

	return (string_t){buffer, ulen};
}

string_t
string_from_float64(char* buffer, size_t capacity, float64_t val, unsigned int precision, unsigned int width,
                    char fill) {
	unsigned int ulen;
	size_t end;
	int len = -1;
	if (!capacity)
		return (string_t){buffer, 0};
	if (precision)
		len = snprintf(buffer, capacity, "%.*lg", precision, val);
	else
		len = snprintf(buffer, capacity, "%.*lg", DBL_DECIMAL_DIG, val);

	ulen = (unsigned int)len;
	if ((len < 0) || (ulen >= capacity)) {
		buffer[capacity - 1] = 0;
		return (string_t){buffer, capacity - 1};
	}
	if (width >= capacity)
		width = (unsigned int)capacity - 1;

	end = string_find_last_not_of(buffer, ulen, STRING_CONST("0"), STRING_NPOS);
	if (end != STRING_NPOS) {
		if (buffer[end] == '.')
			--end;
		if (end != (ulen - 1)) {
			++end;
			ulen = (unsigned int)end;
			buffer[end] = 0;
		}
	}

	// Some cleanups
	if (string_equal(buffer, ulen, "-0", 2) || string_equal(buffer, ulen, "-", 1)) {
		buffer[0] = '0';
		buffer[1] = 0;
		ulen = 1;
	}

	if (ulen < width) {
		unsigned int ofs = width - ulen;
		unsigned int copylen = ulen + 1;
		memmove(buffer + ofs, buffer, copylen);
		memset(buffer, fill, ofs);
		ulen = width;
	}

	return (string_t){buffer, ulen};
}

string_const_t
string_from_real_static(real val, unsigned int precision, unsigned int width, char fill) {
	return string_to_const(
	    string_from_real(get_thread_convert_buffer(), THREAD_BUFFER_SIZE, val, precision, width, fill));
}

string_const_t
string_from_float32_static(float32_t val, unsigned int precision, unsigned int width, char fill) {
	return string_to_const(
	    string_from_float32(get_thread_convert_buffer(), THREAD_BUFFER_SIZE, val, precision, width, fill));
}

string_const_t
string_from_float64_static(float64_t val, unsigned int precision, unsigned int width, char fill) {
	return string_to_const(
	    string_from_float64(get_thread_convert_buffer(), THREAD_BUFFER_SIZE, val, precision, width, fill));
}

string_t
string_from_time(char* buffer, size_t capacity, tick_t t, bool local) {
	if (capacity < 25) {
		if (capacity)
			buffer[0] = 0;
		return (string_t){buffer, 0};
	}
	FOUNDATION_ASSERT(buffer);
	time_t ts = (time_t)(t / 1000LL);
#if FOUNDATION_PLATFORM_WINDOWS && (FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL)
	struct tm tm;
	errno_t err = local ? localtime_s(&tm, &ts) : gmtime_s(&tm, &ts);
	size_t len = !err ? strftime(buffer, capacity, "%a %b %d %H:%M:%S %Y", &tm) : 0;
#elif FOUNDATION_PLATFORM_WINDOWS
	struct tm* gtm = local ? localtime(&ts) : gmtime(&ts);
	size_t len = gtm ? strftime(buffer, capacity, "%a %b %d %H:%M:%S %Y", gtm) : 0;
#else
	struct tm tm;
	struct tm* gtm = local ? localtime_r(&ts, &tm) : gmtime_r(&ts, &tm);
	size_t len = gtm ? strftime(buffer, capacity, "%a %b %d %H:%M:%S %Y", gtm) : 0;
#endif
	return (string_t){buffer, len};
}

string_const_t
string_from_time_static(tick_t t, bool local) {
	return string_to_const(string_from_time(get_thread_convert_buffer(), THREAD_BUFFER_SIZE, t, local));
}

string_const_t
string_from_uuid_static(const uuid_t val) {
	return string_to_const(string_from_uuid(get_thread_convert_buffer(), THREAD_BUFFER_SIZE, val));
}

string_t
string_from_version(char* buffer, size_t capacity, const version_t version) {
	int len = -1;
	if (!capacity)
		return (string_t){buffer, 0};
	if (version.sub.control)
		len = snprintf(buffer, capacity, "%u.%u.%u-%u-%x", (uint32_t)version.sub.major, (uint32_t)version.sub.minor,
		               version.sub.revision, version.sub.build, version.sub.control);
	else if (version.sub.build)
		len = snprintf(buffer, capacity, "%u.%u.%u-%u", (uint32_t)version.sub.major, (uint32_t)version.sub.minor,
		               version.sub.revision, version.sub.build);
	else
		len = snprintf(buffer, capacity, "%u.%u.%u", (uint32_t)version.sub.major, (uint32_t)version.sub.minor,
		               version.sub.revision);
	if ((unsigned int)len >= capacity) {
		buffer[capacity - 1] = 0;
		return (string_t){buffer, capacity - 1};
	}
	return (string_t){buffer, (unsigned int)len};
}

string_const_t
string_from_version_static(const version_t version) {
	return string_to_const(string_from_version(get_thread_convert_buffer(), THREAD_BUFFER_SIZE, version));
}

int
string_to_int(const char* val, size_t length) {
	int ret = 0;
	char buf[16];
	if (length) {
		string_copy(buf, sizeof(buf), val, length);
		ret = (int)strtol(buf, nullptr, 10);
	}
	return ret;
}

unsigned int
string_to_uint(const char* val, size_t length, bool hex) {
	unsigned int ret = 0;
	char buf[16];
	if (length) {
		string_copy(buf, sizeof(buf), val, length);
		ret = (unsigned int)strtoul(buf, nullptr, hex ? 16 : 10);
	}
	return ret;
}

int64_t
string_to_int64(const char* val, size_t length) {
	int64_t ret = 0;
	char buf[32];
	if (length) {
		string_copy(buf, sizeof(buf), val, length);
		ret = (int64_t)strtoll(buf, nullptr, 10);
	}
	return ret;
}

uint64_t
string_to_uint64(const char* val, size_t length, bool hex) {
	uint64_t ret = 0;
	char buf[32];
	if (length) {
		string_copy(buf, sizeof(buf), val, length);
		ret = (uint64_t)strtoull(buf, nullptr, hex ? 16 : 10);
	}
	return ret;
}

uint128_t
string_to_uint128(const char* val, size_t length) {
	int iword = 1;
	uint128_t ret = uint128_null();
	char buf[33];
	size_t ofs;
	size_t buflen = string_copy(buf, sizeof(buf), val, length).length;
	ofs = string_find_first_not_of(buf, buflen, STRING_CONST(STRING_HEX), 0);
	if (ofs == STRING_NPOS)
		ofs = buflen;
	do {
		if (ofs <= 16) {
			buf[ofs] = 0;
			ofs = 0;
		} else
			ofs -= 16;
		sscanf(buf + ofs, "%016" PRIx64, &ret.word[iword--]);
	} while ((ofs > 0) && (iword >= 0));
	return ret;
}

uint256_t
string_to_uint256(const char* val, size_t length) {
	int iword = 3;
	uint256_t ret = uint256_null();
	char buf[65];
	size_t ofs;
	size_t buflen = string_copy(buf, sizeof(buf), val, length).length;
	ofs = string_find_first_not_of(buf, buflen, STRING_CONST(STRING_HEX), 0);
	if (ofs == STRING_NPOS)
		ofs = buflen;
	do {
		if (ofs <= 16) {
			buf[ofs] = 0;
			ofs = 0;
		} else
			ofs -= 16;
		sscanf(buf + ofs, "%016" PRIx64, &ret.word[iword--]);
	} while ((ofs > 0) && (iword >= 0));
	return ret;
}

uint512_t
string_to_uint512(const char* val, size_t length) {
	int iword = 7;
	uint512_t ret = uint512_null();
	char buf[129];
	size_t ofs;
	size_t buflen = string_copy(buf, sizeof(buf), val, length).length;
	ofs = string_find_first_not_of(buf, buflen, STRING_CONST(STRING_HEX), 0);
	if (ofs == STRING_NPOS)
		ofs = buflen;
	do {
		if (ofs <= 16) {
			buf[ofs] = 0;
			ofs = 0;
		} else {
			ofs -= 16;
		}
		sscanf(buf + ofs, "%016" PRIx64, &ret.word[iword--]);
	} while ((ofs > 0) && (iword >= 0));
	return ret;
}

float32_t
string_to_float32(const char* val, size_t length) {
	float32_t ret = 0.0f;
	char buf[32];
	if (length) {
		string_copy(buf, sizeof(buf), val, length);
		ret = (float32_t)strtod(buf, nullptr);
	}
	return ret;
}

float64_t
string_to_float64(const char* val, size_t length) {
	float64_t ret = 0.0;
	char buf[64];
	if (length) {
		string_copy(buf, sizeof(buf), val, length);
		ret = (float64_t)strtod(buf, nullptr);
	}
	return ret;
}

ssize_t
string_to_ssize(const char* val, size_t length) {
	return (ssize_t)string_to_int64(val, length);
}

size_t
string_to_size(const char* val, size_t length, bool hex) {
	return (size_t)string_to_uint64(val, length, hex);
}

real
string_to_real(const char* val, size_t length) {
	real ret = REAL_C(0.0);
	char buf[64];
	if (length) {
		string_copy(buf, sizeof(buf), val, length);
		ret = (real)strtod(buf, nullptr);
	}
	return ret;
}

version_t
string_to_version(const char* val, size_t length) {
	//%u.%u.%u-%u.%u
	if (length) {
		uint32_t num[5];
		size_t i;
		char buf[64];
		char* loop = buf;
		string_copy(buf, sizeof(buf), val, length);
		for (i = 0; i < 5; ++i) {
			num[i] = 0;
			if (*loop) {
				sscanf(loop, i < 4 ? "%u" : "%x", num + i);
				while (*loop && ((*loop >= '0') && (*loop < '9')))
					loop++;
				while (*loop && ((*loop < '0') || (*loop > '9')))
					loop++;
			}
		}
		return version_make(num[0], num[1], num[2], num[3], num[4]);
	}
	return version_make(0, 0, 0, 0, 0);
}

string_t
string_thread_buffer(void) {
	char* buffer = get_thread_convert_buffer();
	return (string_t){buffer, THREAD_BUFFER_SIZE};
}
