/* stream.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic push
#if __has_warning("-Wreserved-identifier")
#pragma clang diagnostic ignored "-Wreserved-identifier"
#endif
#endif

#include <stdarg.h>

#if FOUNDATION_PLATFORM_WINDOWS
#include <foundation/windows.h>
#endif

#if FOUNDATION_PLATFORM_POSIX
#include <foundation/posix.h>
#include <sys/select.h>
#include <sys/stat.h>
#endif

static hashtable64_t* stream_protocol_table;

static stream_t*
stream_open_stdout_protocol(const char* path, size_t length, unsigned int mode) {
	FOUNDATION_UNUSED(path);
	FOUNDATION_UNUSED(length);
	stream_t* stream = stream_open_stdout();
	stream->mode = (mode & STREAM_BINARY) | STREAM_OUT;
	return stream;
}

static stream_t*
stream_open_stderr_protocol(const char* path, size_t length, unsigned int mode) {
	FOUNDATION_UNUSED(path);
	FOUNDATION_UNUSED(length);
	stream_t* stream = stream_open_stderr();
	stream->mode = (mode & STREAM_BINARY) | STREAM_OUT;
	return stream;
}

static stream_t*
stream_open_stdin_protocol(const char* path, size_t length, unsigned int mode) {
	FOUNDATION_UNUSED(path);
	FOUNDATION_UNUSED(length);
	stream_t* stream = stream_open_stdin();
	stream->mode = (mode & STREAM_BINARY) | STREAM_IN;
	return stream;
}

int
internal_stream_initialize(void) {
	stream_protocol_table = hashtable64_allocate(32);

	stream_set_protocol_handler(0, 0, fs_open_file);
#if FOUNDATION_PLATFORM_ANDROID
	stream_set_protocol_handler(STRING_CONST("asset"), asset_stream_open);
#endif
	stream_set_protocol_handler(STRING_CONST("file"), fs_open_file);
	stream_set_protocol_handler(STRING_CONST("stdout"), stream_open_stdout_protocol);
	stream_set_protocol_handler(STRING_CONST("stderr"), stream_open_stderr_protocol);
	stream_set_protocol_handler(STRING_CONST("stdin"), stream_open_stdin_protocol);
	return 0;
}

void
internal_stream_finalize(void) {
	hashtable64_deallocate(stream_protocol_table);
	stream_protocol_table = 0;
}

void
stream_set_protocol_handler(const char* protocol, size_t length, stream_open_fn fn) {
	hashtable64_set(stream_protocol_table, hash(protocol, length), (uintptr_t)fn);
}

stream_open_fn
stream_protocol_handler(const char* protocol, size_t length) {
	return (stream_open_fn)(uintptr_t)hashtable64_get(stream_protocol_table, hash(protocol, length));
}

void
stream_initialize(stream_t* stream, byteorder_t order) {
	stream->byteorder = (unsigned int)order;
	stream->sequential = 0;
	stream->reliable = 1;
	stream->inorder = 1;
	stream->persistent = 0;
	stream->compressed = 0;
	stream->swap = ((byteorder_t)stream->byteorder != system_byteorder()) ? 1 : 0;
	stream->mode = STREAM_BINARY;
	stream->path = (string_t){0, 0};
}

stream_t*
stream_open(const char* path, size_t length, unsigned int mode) {
	size_t protocol_end;
	stream_open_fn open_fn;

	// Check if protocol was given
	protocol_end = string_find_string(path, length, STRING_CONST("://"), 0);
	open_fn = stream_protocol_handler((protocol_end != STRING_NPOS) ? path : "",
	                                  (protocol_end != STRING_NPOS) ? protocol_end : 0);

	return open_fn ? open_fn(path, length, mode) : 0;
}

void
stream_deallocate(stream_t* stream) {
	if (!stream)
		return;
	stream_finalize(stream);
	memory_deallocate(stream);
}

void
stream_finalize(stream_t* stream) {
	if (stream->vtable && stream->vtable->finalize)
		stream->vtable->finalize(stream);

	string_deallocate(stream->path.str);

	stream->path = (string_t){0, 0};
	stream->type = STREAMTYPE_INVALID;
}

stream_t*
stream_clone(stream_t* stream) {
	if (stream->vtable->clone)
		return stream->vtable->clone(stream);
	return 0;
}

bool
stream_copy(stream_t* source, stream_t* destination) {
	bool success = true;
	const size_t buffersize = 4096;
	char* buffer = memory_allocate(0, buffersize, 0, MEMORY_PERSISTENT);
	while (!stream_eos(source)) {
		size_t read = stream_read(source, buffer, buffersize);
		if (read) {
			if (stream_write(destination, buffer, read) != read) {
				success = false;
				break;
			}
		}
	}
	memory_deallocate(buffer);
	return success;
}

void
stream_set_byteorder(stream_t* stream, byteorder_t byteorder) {
	stream->byteorder = (unsigned int)byteorder;
	stream->swap = (byteorder != system_byteorder()) ? 1 : 0;
}

void
stream_set_binary(stream_t* stream, bool binary) {
	if (binary)
		stream->mode |= STREAM_BINARY;
	else
		stream->mode &= ~STREAM_BINARY;
}

bool
stream_is_binary(const stream_t* stream) {
	return ((stream->mode & STREAM_BINARY) == 0) ? false : true;
}

bool
stream_is_sequential(const stream_t* stream) {
	return stream->sequential != 0;
}

bool
stream_is_swapped(const stream_t* stream) {
	return stream->swap != 0;
}

bool
stream_is_reliable(const stream_t* stream) {
	return stream->reliable != 0;
}

bool
stream_is_inorder(const stream_t* stream) {
	return stream->inorder != 0;
}

bool
stream_eos(stream_t* stream) {
	return (stream->vtable->eos ? stream->vtable->eos(stream) : false);
}

byteorder_t
stream_byteorder(const stream_t* stream) {
	return stream->byteorder;
}

string_const_t
stream_path(const stream_t* stream) {
	return string_to_const(stream->path);
}

tick_t
stream_last_modified(const stream_t* stream) {
	return (stream->vtable->lastmod ? stream->vtable->lastmod(stream) : time_system());
}

void
stream_seek(stream_t* stream, ssize_t offset, stream_seek_mode_t direction) {
	if (stream->vtable->seek)
		stream->vtable->seek(stream, offset, direction);
}

size_t
stream_tell(stream_t* stream) {
	return stream->vtable->tell ? stream->vtable->tell(stream) : 0;
}

size_t
stream_read(stream_t* stream, void* buffer, size_t size) {
	if (!(stream->mode & STREAM_IN))
		return 0;
	return stream->vtable->read(stream, buffer, size);
}

string_t
stream_read_line_buffer(stream_t* stream, char* dest, size_t count, char delimiter) {
	size_t i, read, total, limit, hardlimit;

	if (!(stream->mode & STREAM_IN) || !dest || (count < 2)) {
		if (dest && count)
			dest[0] = 0;
		return (string_t){dest, 0};
	}

	total = 0;
	hardlimit = stream_is_sequential(stream) ? 1 : 128;
	// Need to read one byte at a time since we can't scan back if overreading in sequential streams

	--count;
	while (!stream_eos(stream)) {
		limit = count - total;
		if (limit > hardlimit)
			limit = hardlimit;
		if (!limit)
			break;

		// This will initialize range [total,total+read) in dest array, making
		// access of potentially uninitialized dest array safe (see coverity markup below)
		read = stream->vtable->read(stream, dest + total, limit);
		if (!read)
			break;
		for (i = 0; i < read; ++i) {
			/* coverity[read_parm] */
			if (dest[total + i] == delimiter)
				break;
		}
		total += i;
		if (i < read) {
			if ((i + 1) < read) {
				// Sequential should never end up here reading one byte at a time
				FOUNDATION_ASSERT(!stream_is_sequential(stream));
				stream_seek(stream, (ssize_t)(1 + i) - (ssize_t)read, STREAM_SEEK_CURRENT);
			}
			break;
		}
	}

	dest[total] = 0;

	return (string_t){dest, total};
}

string_t
stream_read_line(stream_t* stream, char delimiter) {
	char buffer[128];
	char* outbuffer = 0;
	size_t outsize = 0;
	size_t cursize = 0;
	size_t read, i;
	size_t want_read = 128;

	if (!(stream->mode & STREAM_IN))
		return (string_t){0, 0};

	// Need to read one byte at a time since we can't scan back if overreading
	if (stream_is_sequential(stream))
		want_read = 1;

	while (!stream_eos(stream)) {
		read = stream->vtable->read(stream, buffer, want_read);
		if (!read)
			break;
		for (i = 0; i < read; ++i) {
			if (buffer[i] == delimiter)
				break;
		}
		if (cursize + i > outsize) {
			size_t nextsize;
			if (!outbuffer) {
				nextsize = (i >= 32 ? i + 1 : (i > 1 ? i + 1 : 32));
				outbuffer = memory_allocate(0, nextsize, 0, MEMORY_PERSISTENT);
			} else {
				nextsize = (outsize < 511 ? 512 : outsize + 513);  // Always aligns to 512 multiples
				FOUNDATION_ASSERT(!(nextsize % 512));
				outbuffer = memory_reallocate(outbuffer, nextsize, 0, outsize + 1, 0);
			}
			outsize = nextsize - 1;
		}
		if (i) {
			memcpy(outbuffer + cursize, buffer, i);  // lint !e613
			cursize += i;
		}
		if (i < read) {
			if ((i + 1) < read) {
				// Sequential should never end up here reading one byte at a time
				FOUNDATION_ASSERT(!stream_is_sequential(stream));
				stream_seek(stream, (ssize_t)(1 + i) - (ssize_t)read, STREAM_SEEK_CURRENT);
			}
			break;
		}
	}

	if (outbuffer)
		outbuffer[cursize] = 0;

	return (string_t){outbuffer, cursize};
}

size_t
stream_skip_whitespace(stream_t* stream) {
	char token;
	size_t read;
	size_t total = 0;

	if (!(stream->mode & STREAM_IN) || (stream->mode & STREAM_BINARY) || stream_is_sequential(stream))
		return 0;

	do {
		read = stream->vtable->read(stream, &token, 1);
		total += read;
	} while ((read > 0) && (string_find(STRING_CONST(STRING_WHITESPACE), token, 0) != STRING_NPOS));

	if (read) {
		stream_seek(stream, -1, STREAM_SEEK_CURRENT);
		--total;
	}

	return total;
}

size_t
stream_size(stream_t* stream) {
	return (stream->vtable->size ? stream->vtable->size(stream) : 0);
}

void
stream_determine_binary_mode(stream_t* stream, size_t size) {
	char fixed_buffer[32];
	char* buf = fixed_buffer;
	size_t cur;
	size_t actual_read, i;

	if (!(stream->mode & STREAM_IN) || stream_is_sequential(stream))
		return;

	if (!size)
		size = 8;

	if (size > sizeof(fixed_buffer))
		buf = memory_allocate(0, size, 0, MEMORY_TEMPORARY);
	memset(buf, 32, size);

	cur = stream_tell(stream);
	actual_read = stream_read(stream, buf, size);
	stream_seek(stream, (ssize_t)cur, STREAM_SEEK_BEGIN);

	stream->mode &= ~STREAM_BINARY;

	for (i = 0; i < actual_read; ++i) {
		// TODO: What about UTF-8?
		if (((buf[i] < 0x20) && (buf[i] != 0x09) && (buf[i] != 0x0a) && (buf[i] != 0x0d)) || (buf[i] > 0x7e)) {
			stream->mode |= STREAM_BINARY;
			break;
		}
	}

	if (buf != fixed_buffer)
		memory_deallocate(buf);
}

bool
stream_read_bool(stream_t* stream) {
	bool value = false;

	if (stream_is_binary(stream)) {
		char c = 0;
		stream_read(stream, &c, 1);
		value = (c ? true : false);
	} else {
		char buffer[6] = {0};
		string_t str = stream_read_string_buffer(stream, buffer, 6);
		value = !(!str.length || string_equal(str.str, str.length, STRING_CONST("false")) ||
		          (string_equal(str.str, str.length, STRING_CONST("0"))));
	}

	return value;
}

int8_t
stream_read_int8(stream_t* stream) {
	int8_t value = 0;
	if (stream_is_binary(stream))
		stream_read(stream, &value, 1);
	else {
		char buffer[6] = {0};
		string_t str = stream_read_string_buffer(stream, buffer, 6);
		value = (int8_t)string_to_int(str.str, str.length);
	}
	return value;
}

uint8_t
stream_read_uint8(stream_t* stream) {
	uint8_t value = 0;
	if (stream_is_binary(stream))
		stream_read(stream, &value, 1);
	else {
		char buffer[6] = {0};
		string_t str = stream_read_string_buffer(stream, buffer, 6);
		value = (uint8_t)string_to_uint(str.str, str.length, false);
	}
	return value;
}

int16_t
stream_read_int16(stream_t* stream) {
	int16_t value = 0;
	if (stream_is_binary(stream)) {
		stream_read(stream, &value, 2);
		if (stream->swap)
			value = (int16_t)byteorder_swap16((uint16_t)value);
	} else {
		char buffer[8] = {0};
		string_t str = stream_read_string_buffer(stream, buffer, 8);
		value = (int16_t)string_to_int(str.str, str.length);
	}
	return value;
}

uint16_t
stream_read_uint16(stream_t* stream) {
	uint16_t value = 0;
	if (stream_is_binary(stream)) {
		stream_read(stream, &value, 2);
		if (stream->swap)
			value = byteorder_swap16(value);
	} else {
		char buffer[8] = {0};
		string_t str = stream_read_string_buffer(stream, buffer, 8);
		value = (uint16_t)string_to_uint(str.str, str.length, false);
	}
	return value;
}

int32_t
stream_read_int32(stream_t* stream) {
	int32_t value = 0;
	if (stream_is_binary(stream)) {
		stream_read(stream, &value, 4);
		if (stream->swap)
			value = (int32_t)byteorder_swap32((uint32_t)value);
	} else {
		char buffer[12] = {0};
		string_t str = stream_read_string_buffer(stream, buffer, 12);
		value = (int32_t)string_to_int(str.str, str.length);
	}
	return value;
}

uint32_t
stream_read_uint32(stream_t* stream) {
	uint32_t value = 0;
	if (stream_is_binary(stream)) {
		stream_read(stream, &value, 4);
		if (stream->swap)
			value = byteorder_swap32(value);
	} else {
		char buffer[12] = {0};
		string_t str = stream_read_string_buffer(stream, buffer, 12);
		value = (uint32_t)string_to_uint(str.str, str.length, false);
	}
	return value;
}

int64_t
stream_read_int64(stream_t* stream) {
	int64_t value = 0;
	if (stream_is_binary(stream)) {
		stream_read(stream, &value, 8);
		if (stream->swap)
			value = (int64_t)byteorder_swap64((uint64_t)value);
	} else {
		char buffer[22] = {0};
		string_t str = stream_read_string_buffer(stream, buffer, 22);
		value = string_to_int64(str.str, str.length);
	}
	return value;
}

uint64_t
stream_read_uint64(stream_t* stream) {
	uint64_t value = 0;
	if (stream_is_binary(stream)) {
		stream_read(stream, &value, 8);
		if (stream->swap)
			value = byteorder_swap64(value);
	} else {
		char buffer[22] = {0};
		string_t str = stream_read_string_buffer(stream, buffer, 22);
		value = string_to_uint64(str.str, str.length, false);
	}
	return value;
}

uint128_t
stream_read_uint128(stream_t* stream) {
	uint128_t value;
	if (stream_is_binary(stream)) {
		value.word[0] = stream_read_uint64(stream);
		value.word[1] = stream_read_uint64(stream);
	} else {
		char buffer[34] = {0};
		string_t str = stream_read_string_buffer(stream, buffer, sizeof(buffer));
		value = string_to_uint128(str.str, str.length);
	}
	return value;
}

uint256_t
stream_read_uint256(stream_t* stream) {
	uint256_t value;
	if (stream_is_binary(stream)) {
		value.word[0] = stream_read_uint64(stream);
		value.word[1] = stream_read_uint64(stream);
		value.word[2] = stream_read_uint64(stream);
		value.word[3] = stream_read_uint64(stream);
	} else {
		char buffer[66] = {0};
		string_t str = stream_read_string_buffer(stream, buffer, sizeof(buffer));
		value = string_to_uint256(str.str, str.length);
	}
	return value;
}

uint512_t
stream_read_uint512(stream_t* stream) {
	uint512_t value;
	if (stream_is_binary(stream)) {
		value.word[0] = stream_read_uint64(stream);
		value.word[1] = stream_read_uint64(stream);
		value.word[2] = stream_read_uint64(stream);
		value.word[3] = stream_read_uint64(stream);
		value.word[4] = stream_read_uint64(stream);
		value.word[5] = stream_read_uint64(stream);
		value.word[6] = stream_read_uint64(stream);
		value.word[7] = stream_read_uint64(stream);
	} else {
		char buffer[129] = {0};
		string_t str = stream_read_string_buffer(stream, buffer, sizeof(buffer));
		value = string_to_uint512(str.str, str.length);
	}
	return value;
}

uuid_t
stream_read_uuid(stream_t* stream) {
	if (stream_is_binary(stream))
		return stream_read_uint128(stream);

	char buffer[37] = {0};
	string_t str = stream_read_string_buffer(stream, buffer, sizeof(buffer));
	return string_to_uuid(str.str, str.length);
}

float32_t
stream_read_float32(stream_t* stream) {
	float32_t value = 0;
	if (stream_is_binary(stream)) {
		stream_read(stream, &value, 4);
		if (stream->swap) {
			float32_cast_t cast;
			cast.fval = value;
			cast.uival = byteorder_swap32(cast.uival);
			value = cast.fval;
		}
	} else {
		char buffer[32] = {0};
		string_t str = stream_read_string_buffer(stream, buffer, 32);
		value = string_to_float32(str.str, str.length);
	}
	return value;
}

float64_t
stream_read_float64(stream_t* stream) {
	float64_t value = 0;
	if (stream_is_binary(stream)) {
		stream_read(stream, &value, 8);
		if (stream->swap) {
			float64_cast_t cast;
			cast.fval = value;
			cast.uival = byteorder_swap64(cast.uival);
			value = cast.fval;
		}
	} else {
		char buffer[64] = {0};
		string_t str = stream_read_string_buffer(stream, buffer, 64);
		value = string_to_float64(str.str, str.length);
	}
	return value;
}

string_t
stream_read_string(stream_t* stream) {
	char buffer[128];
	char* outbuffer = buffer;
	size_t outsize = sizeof(buffer);
	size_t cursize = 0;
	size_t read, i;
	bool binary = stream_is_binary(stream);

	if (!(stream->mode & STREAM_IN))
		return (string_t){0, 0};

	if (stream_is_sequential(stream)) {
		// Single byte reading since we can't seek backwards (and don't want to block on network
		// sockets)
		char c;
		if (!binary) {
			// Consume whitespace
			while (!stream_eos(stream)) {
				read = stream->vtable->read(stream, &c, 1);
				if (!read)
					break;
				if ((c != ' ') && (c != '\n') && (c != '\r') && (c != '\t')) {
					buffer[cursize++] = c;
					break;
				}
			}
		}

		if (cursize > 0) {
			while (!stream_eos(stream)) {
				read = stream->vtable->read(stream, &c, 1);
				if (!read)
					break;
				if (!c)
					break;
				if (!binary && ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t')))
					break;
				if (cursize + 1 >= outsize) {
					outsize += 512;
					if (outbuffer != buffer) {
						outbuffer = memory_reallocate(outbuffer, outsize, 0, cursize, 0);
					} else {
						outbuffer = memory_allocate(0, outsize, 0, MEMORY_PERSISTENT);
						memcpy(outbuffer, buffer, sizeof(buffer));
					}
				}
				outbuffer[cursize++] = c;
			}

			outbuffer[cursize] = 0;  // lint !e661
		}
	} else {
		if (!binary) {
			// Consume whitespace
			while (!stream_eos(stream)) {
				read = stream->vtable->read(stream, buffer, 16);
				if (!read)
					break;
				for (i = 0; i < read; ++i) {
					char c = buffer[i];
					if ((c != ' ') && (c != '\n') && (c != '\r') && (c != '\t'))
						break;
				}
				if (i < read) {
					stream_seek(stream, (ssize_t)i - (ssize_t)read, STREAM_SEEK_CURRENT);
					break;
				}
			}
		}

		while (!stream_eos(stream)) {
			if (outbuffer != buffer)
				read = stream->vtable->read(stream, buffer, sizeof(buffer));
			else
				read = stream->vtable->read(stream, buffer + cursize, sizeof(buffer) - cursize);
			if (!read)
				break;
			for (i = 0; i < read; ++i) {
				char c = buffer[i];
				if (!c)
					break;
				if (!binary && ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t')))
					break;
			}
			if (i) {
				if (cursize + i >= outsize) {
					outsize += 512;
					if (outbuffer != buffer) {
						outbuffer = memory_reallocate(outbuffer, outsize, 0, cursize, 0);
					} else {
						FOUNDATION_ASSERT(cursize == 0);  // Or internal assumptions about code flow is incorrect
						outbuffer = memory_allocate(0, outsize, 0, MEMORY_PERSISTENT);
						memcpy(outbuffer, buffer, i);
					}
				} else if (outbuffer != buffer)
					memcpy(outbuffer + cursize, buffer, i);
				cursize += i;
			}
			if (i < sizeof(buffer)) {
				if ((i + 1) < read)
					stream_seek(stream, (ssize_t)(1 + i) - (ssize_t)read, STREAM_SEEK_CURRENT);
				break;
			}
		}

		outbuffer[cursize] = 0;
	}

	if (outbuffer == buffer) {
		if (cursize == 0)
			return (string_t){0, 0};
		outbuffer = memory_allocate(0, cursize + 1, 0, MEMORY_PERSISTENT);
		memcpy(outbuffer, buffer, cursize);
		outbuffer[cursize] = 0;
	}

	return (string_t){outbuffer, cursize};
}

string_t
stream_read_string_buffer(stream_t* stream, char* outbuffer, size_t size) {
	char buffer[128];
	size_t cursize = 0;
	size_t read, i;
	bool binary = stream_is_binary(stream);

	if (!(stream->mode & STREAM_IN) || !outbuffer || !size) {
		if (outbuffer && size)
			outbuffer[0] = 0;
		return (string_t){outbuffer, 0};
	}

	--size;

	if (stream_is_sequential(stream)) {
		// Single byte reading since we can't seek backwards (and don't want to block on network
		// sockets)
		char c;
		if (!binary) {
			// Consume whitespace
			while (!stream_eos(stream)) {
				read = stream->vtable->read(stream, &c, 1);
				if (!read)
					break;
				if ((c != ' ') && (c != '\n') && (c != '\r') && (c != '\t')) {
					outbuffer[cursize++] = c;
					break;
				}
			}
		}

		if (cursize > 0) {
			while (!stream_eos(stream) && (cursize < size)) {
				read = stream->vtable->read(stream, &c, 1);
				if (!read)
					break;
				if (!c)
					break;
				if (!binary && ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t')))
					break;
				outbuffer[cursize++] = c;
			}
		}
	} else {
		if (!binary) {
			// Consume whitespace
			while (!stream_eos(stream)) {
				read = stream->vtable->read(stream, buffer, 16);
				if (!read)
					break;
				for (i = 0; i < read; ++i) {
					char c = buffer[i];
					if ((c != ' ') && (c != '\n') && (c != '\r') && (c != '\t'))
						break;
				}
				if (i < read) {
					stream_seek(stream, (ssize_t)i - (ssize_t)read, STREAM_SEEK_CURRENT);
					break;
				}
			}
		}

		while (!stream_eos(stream) && (cursize < size)) {
			read = stream->vtable->read(stream, buffer, 128);
			if (!read)
				break;
			for (i = 0; i < read; ++i) {
				char c = buffer[i];
				if (!c)
					break;
				if (!binary && ((c == ' ') || (c == '\n') || (c == '\r') || (c == '\t')))
					break;
			}
			if (!i)
				break;
			if (cursize + i > size)
				i = size - cursize;
			memcpy(outbuffer + cursize, buffer, i);
			cursize += i;
			if (i < 128) {
				if ((i + 1) < read)
					stream_seek(stream, (ssize_t)(1 + i) - (ssize_t)read, STREAM_SEEK_CURRENT);
				break;
			}
		}
	}

	if (cursize < size)
		outbuffer[cursize] = 0;

	return (string_t){outbuffer, cursize};
}

void
stream_buffer_read(stream_t* stream) {
	if (stream->vtable->buffer_read)
		stream->vtable->buffer_read(stream);
}

size_t
stream_available_read(stream_t* stream) {
	if (stream->vtable->available_read)
		return (unsigned int)stream->vtable->available_read(stream);
	return (unsigned int)(stream_size(stream) - stream_tell(stream));
}

bool
stream_digest(stream_t* stream, void* (*digester)(void*, const void*, size_t), void* data) {
	size_t cur, ic, lastc, size, limit;
	unsigned char buf[1025];
	bool ignore_lf = false;

	if (stream_is_sequential(stream) || !(stream->mode & STREAM_IN))
		return false;

	cur = stream_tell(stream);
	stream_seek(stream, 0, STREAM_SEEK_BEGIN);

	limit = sizeof(buf) - 1;
	buf[limit] = 0;

	while (!stream_eos(stream)) {
		size = stream->vtable->read(stream, buf, limit);
		if (!size)
			continue;
		if (stream->mode & STREAM_BINARY) {
			if (!digester(data, buf, size))
				return false;
		} else {
			// If last buffer ended with CR, ignore a leading LF
			lastc = 0;
			if (ignore_lf && (buf[0] == '\n'))
				lastc = 1;
			ignore_lf = false;

			// Digest one line at a time
			// Treat all line endings (LF, CR, CR+LF) as Unix style LF. If file has mixed line
			// endings (for example, one line ending in a single CR and next is empty and ending in
			// a
			// single LF), it will not work!
			/*lint -e{850} */
			for (ic = lastc; ic < size && ic < limit; ++ic) {
				bool was_cr = (buf[ic] == '\r');
				bool was_lf = (buf[ic] == '\n');
				if (was_cr || was_lf) {
					if (was_cr && (ic == limit - 1))
						ignore_lf = true;  // Make next buffer ignore leading LF as it is part of CR+LF
					buf[ic] = '\n';
					// Include the LF
					if (!digester(data, buf + lastc, (size_t)((ic - lastc) + 1)))
						return false;
					// Check for CR+LF
					if (was_cr && (buf[ic + 1] == '\n'))
						++ic;
					lastc = ic + 1;
				}
			}
			if (lastc < size) {
				if (!digester(data, buf + lastc, size - lastc))
					return false;
			}
		}
	}

	stream_seek(stream, (ssize_t)cur, STREAM_SEEK_BEGIN);
	return true;
}

uint128_t
stream_md5(stream_t* stream) {
	md5_t md5;
	uint128_t ret = uint128_null();

	if (stream->vtable->md5)
		return stream->vtable->md5(stream);

	md5_initialize(&md5);
	if (stream_digest(stream, (void* (*)(void*, const void*, size_t))md5_digest, &md5)) {
		md5_digest_finalize(&md5);
		ret = md5_get_digest_raw(&md5);
	}
	md5_finalize(&md5);

	return ret;
}

uint256_t
stream_sha256(stream_t* stream) {
	sha256_t sha;
	uint256_t ret = uint256_null();

	if (stream->vtable->sha256)
		return stream->vtable->sha256(stream);

	sha256_initialize(&sha);
	if (stream_digest(stream, (void* (*)(void*, const void*, size_t))sha256_digest, &sha)) {
		sha256_digest_finalize(&sha);
		ret = sha256_get_digest_raw(&sha);
	}
	sha256_finalize(&sha);

	return ret;
}

uint512_t
stream_sha512(stream_t* stream) {
	sha512_t sha;
	uint512_t ret = uint512_null();

	if (stream->vtable->sha512)
		return stream->vtable->sha512(stream);

	sha512_initialize(&sha);
	if (stream_digest(stream, (void* (*)(void*, const void*, size_t))sha512_digest, &sha)) {
		sha512_digest_finalize(&sha);
		ret = sha512_get_digest_raw(&sha);
	}
	sha512_finalize(&sha);

	return ret;
}

size_t
stream_write(stream_t* stream, const void* buffer, size_t size) {
	if (!(stream->mode & STREAM_OUT))
		return 0;
	return stream->vtable->write(stream, buffer, size);
}

void
stream_write_bool(stream_t* stream, bool data) {
	if (stream_is_binary(stream)) {
		char c = (data ? 1 : 0);
		stream_write(stream, &c, 1);
	} else {
		if (data)
			stream_write(stream, "true", 4);
		else
			stream_write(stream, "false", 5);
	}
}

void
stream_write_int8(stream_t* stream, int8_t data) {
	if (stream_is_binary(stream))
		stream_write(stream, &data, 1);
	else {
		string_const_t value = string_from_int_static((int32_t)data, 0, 0);
		stream_write_string(stream, value.str, value.length);
	}
}

void
stream_write_uint8(stream_t* stream, uint8_t data) {
	if (stream_is_binary(stream))
		stream_write(stream, &data, 1);
	else {
		string_const_t value = string_from_uint_static((uint32_t)data, false, 0, 0);
		stream_write_string(stream, value.str, value.length);
	}
}

void
stream_write_int16(stream_t* stream, int16_t data) {
	if (stream_is_binary(stream)) {
		if (stream->swap)
			data = (int16_t)byteorder_swap16((uint16_t)data);
		stream_write(stream, &data, 2);
	} else {
		string_const_t value = string_from_int_static(data, 0, 0);
		stream_write_string(stream, value.str, value.length);
	}
}

void
stream_write_uint16(stream_t* stream, uint16_t data) {
	if (stream_is_binary(stream)) {
		if (stream->swap)
			data = byteorder_swap16(data);
		stream_write(stream, &data, 2);
	} else {
		string_const_t value = string_from_uint_static(data, false, 0, 0);
		stream_write_string(stream, value.str, value.length);
	}
}

void
stream_write_int32(stream_t* stream, int32_t data) {
	if (stream_is_binary(stream)) {
		if (stream->swap)
			data = (int32_t)byteorder_swap32((uint32_t)data);
		stream_write(stream, &data, 4);
	} else {
		string_const_t value = string_from_int_static(data, 0, 0);
		stream_write_string(stream, value.str, value.length);
	}
}

void
stream_write_uint32(stream_t* stream, uint32_t data) {
	if (stream_is_binary(stream)) {
		if (stream->swap)
			data = byteorder_swap32(data);
		stream_write(stream, &data, 4);
	} else {
		string_const_t value = string_from_uint_static(data, false, 0, 0);
		stream_write_string(stream, value.str, value.length);
	}
}

void
stream_write_int64(stream_t* stream, int64_t data) {
	if (stream_is_binary(stream)) {
		if (stream->swap)
			data = (int64_t)byteorder_swap64((uint64_t)data);
		stream_write(stream, &data, 8);
	} else {
		string_const_t value = string_from_int_static(data, 0, 0);
		stream_write_string(stream, value.str, value.length);
	}
}

void
stream_write_uint64(stream_t* stream, uint64_t data) {
	if (stream_is_binary(stream)) {
		if (stream->swap)
			data = byteorder_swap64(data);
		stream_write(stream, &data, 8);
	} else {
		string_const_t value = string_from_uint_static(data, false, 0, 0);
		stream_write_string(stream, value.str, value.length);
	}
}

void
stream_write_uint128(stream_t* stream, uint128_t data) {
	if (stream_is_binary(stream)) {
		stream_write_uint64(stream, data.word[0]);
		stream_write_uint64(stream, data.word[1]);
	} else {
		string_const_t value = string_from_uint128_static(data);
		stream_write_string(stream, value.str, value.length);
	}
}

void
stream_write_uint256(stream_t* stream, uint256_t data) {
	if (stream_is_binary(stream)) {
		stream_write_uint64(stream, data.word[0]);
		stream_write_uint64(stream, data.word[1]);
		stream_write_uint64(stream, data.word[2]);
		stream_write_uint64(stream, data.word[3]);
	} else {
		string_const_t value = string_from_uint256_static(data);
		stream_write_string(stream, value.str, value.length);
	}
}

void
stream_write_uint512(stream_t* stream, uint512_t data) {
	if (stream_is_binary(stream)) {
		stream_write_uint64(stream, data.word[0]);
		stream_write_uint64(stream, data.word[1]);
		stream_write_uint64(stream, data.word[2]);
		stream_write_uint64(stream, data.word[3]);
		stream_write_uint64(stream, data.word[4]);
		stream_write_uint64(stream, data.word[5]);
		stream_write_uint64(stream, data.word[6]);
		stream_write_uint64(stream, data.word[7]);
	} else {
		string_const_t value = string_from_uint512_static(data);
		stream_write_string(stream, value.str, value.length);
	}
}

void
stream_write_uuid(stream_t* stream, uuid_t data) {
	if (stream_is_binary(stream)) {
		stream_write_uint128(stream, data);
	} else {
		string_const_t value = string_from_uuid_static(data);
		stream_write_string(stream, value.str, value.length);
	}
}

void
stream_write_float32(stream_t* stream, float32_t data) {
	if (stream_is_binary(stream)) {
		if (stream->swap) {
			float32_cast_t cast;
			cast.fval = data;
			cast.uival = byteorder_swap32(cast.uival);
			stream_write(stream, &cast.ival, 4);
		} else {
			stream_write(stream, &data, 4);
		}
	} else {
		string_const_t value = string_from_float32_static(data, 0, 0, 0);
		stream_write_string(stream, value.str, value.length);
	}
}

void
stream_write_float64(stream_t* stream, float64_t data) {
	if (stream_is_binary(stream)) {
		if (stream->swap) {
			float64_cast_t cast;
			cast.fval = data;
			cast.uival = byteorder_swap64(cast.uival);
			stream_write(stream, &cast.ival, 8);
		} else {
			stream_write(stream, &data, 8);
		}
	} else {
		string_const_t value = string_from_float64_static(data, 0, 0, 0);
		stream_write_string(stream, value.str, value.length);
	}
}

void
stream_write_string(stream_t* stream, const char* str, size_t length) {
	if (str && length)
		stream_write(stream, str, length);
	if (stream_is_binary(stream)) {
		char nullstr = 0;
		stream_write(stream, &nullstr, 1);
	}
}

void
stream_write_separator(stream_t* stream) {
	if (!stream_is_binary(stream))
		stream_write(stream, " ", 1);
}

void
stream_write_endl(stream_t* stream) {
	if (!stream_is_binary(stream))
		stream_write(stream, "\n", 1);
	stream_flush(stream);
}

void
stream_write_format(stream_t* stream, const char* format, size_t format_length, ...) {
	va_list list;
	string_t buffer;

	va_start(list, format_length);
	buffer = string_allocate_vformat(format, format_length, list);
	va_end(list);

	stream_write_string(stream, buffer.str, buffer.length);
	string_deallocate(buffer.str);
}

void
stream_truncate(stream_t* stream, size_t size) {
	if (stream->vtable->truncate)
		stream->vtable->truncate(stream, size);
}

void
stream_flush(stream_t* stream) {
	if (stream->vtable->flush)
		stream->vtable->flush(stream);
}

#include <stdio.h>

/*lint -e754 */
struct stream_std_t {
	FOUNDATION_DECLARE_STREAM;
	void* std;
	bool eos;
};

typedef FOUNDATION_ALIGN(8) struct stream_std_t stream_std_t;

static size_t
stream_stdin_read(stream_t* stream, void* buffer, size_t size) {
	stream_std_t* stdstream = (stream_std_t*)stream;
	FILE* stdfile = (FILE*)stdstream->std;
	char* bytebuffer = (char*)buffer;
	size_t read = 0;

	stdstream->eos = false;

	while (read < size) {
		int c = getc(stdfile);
		if (c == EOF) {
			stdstream->eos = true;
			break;
		}
		bytebuffer[read++] = (char)c;
	}

	return read;
}

static size_t
stream_stdout_write(stream_t* stream, const void* buffer, size_t size) {
	stream_std_t* stdstream = (stream_std_t*)stream;
	size_t was_written = fwrite(buffer, 1, (size_t)size, stdstream->std);
	return was_written;
}

static void
stream_stdout_flush(stream_t* stream) {
	fflush(((stream_std_t*)stream)->std);
}

static stream_t*
stream_std_clone(stream_t* stream) {
	stream_std_t* clone =
	    memory_allocate(HASH_STREAM, sizeof(stream_std_t), 8, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	memcpy(clone, stream, sizeof(stream_std_t));
	clone->path = string_clone(stream->path.str, stream->path.length);
	return (stream_t*)clone;
}

static bool
stream_stdin_eos(stream_t* stream) {
	return ((stream_std_t*)stream)->eos;
}

static size_t
stream_stdin_available_read(stream_t* stream) {
#if FOUNDATION_PLATFORM_WINDOWS

	HANDLE in_handle;
	DWORD size;
	FOUNDATION_UNUSED(stream);

	in_handle = GetStdHandle(STD_INPUT_HANDLE);
	size = GetFileSize(in_handle, 0);
	if (size != (DWORD)-1)
		return size;

#elif FOUNDATION_PLATFORM_POSIX

	fd_set fds;
	struct timeval timeout;
	int res;
	FOUNDATION_UNUSED(stream);

	memset(&timeout, 0, sizeof(timeout));
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);

	res = select(STDIN_FILENO + 1, &fds, 0, 0, &timeout);
	if ((res > 0) && FD_ISSET(STDIN_FILENO, &fds)) {
		struct stat buf;
		if (fstat(STDIN_FILENO, &buf) == 0)
			return (size_t)buf.st_size;
		return 1;
	}

#else
	FOUNDATION_UNUSED(stream);
#endif
	return 0;
}

static tick_t
stream_std_last_modified(const stream_t* stream) {
	FOUNDATION_UNUSED(stream);
	return time_system();
}

static stream_vtable_t stream_stdout_vtable = {
    0, stream_stdout_write, 0, stream_stdout_flush, 0, 0, 0, 0, stream_std_last_modified, 0, 0, 0, 0, 0,
    0, stream_std_clone};

static stream_vtable_t stream_stdin_vtable = {
    stream_stdin_read,           0, stream_stdin_eos, 0, 0, 0, 0, 0, stream_std_last_modified, 0, 0, 0, 0,
    stream_stdin_available_read, 0, stream_std_clone};

stream_t*
stream_open_stdout(void) {
	stream_std_t* stream =
	    memory_allocate(HASH_STREAM, sizeof(stream_std_t), 8, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	stream_initialize((stream_t*)stream, system_byteorder());
	stream->sequential = 1;
	stream->mode = STREAM_OUT;
	stream->type = STREAMTYPE_STDSTREAM;
	stream->vtable = &stream_stdout_vtable;
	stream->path = string_clone(STRING_CONST("stdout://"));
	stream->std = stdout;
	return (stream_t*)stream;
}

stream_t*
stream_open_stderr(void) {
	stream_std_t* stream =
	    memory_allocate(HASH_STREAM, sizeof(stream_std_t), 8, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	stream_initialize((stream_t*)stream, system_byteorder());
	stream->sequential = 1;
	stream->mode = STREAM_OUT;
	stream->type = STREAMTYPE_STDSTREAM;
	stream->vtable = &stream_stdout_vtable;
	stream->path = string_clone(STRING_CONST("stderr://"));
	stream->std = stderr;
	return (stream_t*)stream;
}

stream_t*
stream_open_stdin(void) {
	stream_std_t* stream =
	    memory_allocate(HASH_STREAM, sizeof(stream_std_t), 8, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	stream_initialize((stream_t*)stream, system_byteorder());
	stream->sequential = 1;
	stream->mode = STREAM_IN;
	stream->type = STREAMTYPE_STDSTREAM;
	stream->vtable = &stream_stdin_vtable;
	stream->path = string_clone(STRING_CONST("stdin://"));
	stream->std = stdin;
	return (stream_t*)stream;
}

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic pop
#endif
