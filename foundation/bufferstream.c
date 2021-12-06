/* bufferstream.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

static stream_vtable_t buffer_stream_vtable;

stream_t*
buffer_stream_allocate(void* buffer, unsigned int mode, size_t size, size_t capacity, bool adopt, bool grow) {
	stream_buffer_t* stream = memory_allocate(HASH_STREAM, sizeof(stream_buffer_t), 8, MEMORY_PERSISTENT);
	buffer_stream_initialize(stream, buffer, mode, size, capacity, adopt, grow);
	return (stream_t*)stream;
}

void
buffer_stream_initialize(stream_buffer_t* stream, void* buffer, unsigned int mode, size_t size, size_t capacity,
                         bool adopt, bool grow) {
	memset(stream, 0, sizeof(stream_buffer_t));
	stream_initialize((stream_t*)stream, system_byteorder());

	if (!adopt && grow) {
		log_warn(0, WARNING_INVALID_VALUE, STRING_CONST("Cannot grow buffer streams that are not adopted"));
		grow = false;
	}

	if (!buffer) {
		size = 0;
		capacity = 0;
	}
	if (size > capacity)
		size = capacity;

	stream->type = STREAMTYPE_MEMORY;
	stream->path = string_allocate_format(STRING_CONST("buffer://0x%" PRIfixPTR "-0x%" PRIfixPTR), (uintptr_t)buffer,
	                                      (uintptr_t)pointer_offset(buffer, size));
	stream->mode = mode & (STREAM_OUT | STREAM_IN | STREAM_BINARY);
	stream->buffer = buffer;
	stream->size = size;
	stream->capacity = capacity;
	stream->own = adopt;
	stream->grow = (adopt && grow);
	stream->lastmod = time_current();

	if ((mode & STREAM_OUT) && (mode & STREAM_TRUNCATE))
		stream->size = 0;
	if (mode & STREAM_ATEND)
		stream->current = stream->size;

	stream->vtable = &buffer_stream_vtable;
}

static void
buffer_stream_finalize(stream_t* stream) {
	stream_buffer_t* bufferstream = (stream_buffer_t*)stream;

	if (!bufferstream || (stream->type != STREAMTYPE_MEMORY))
		return;

	if (bufferstream->own)
		memory_deallocate(bufferstream->buffer);
	bufferstream->buffer = 0;
}

static size_t
buffer_stream_read(stream_t* stream, void* dest, size_t size) {
	size_t available;
	stream_buffer_t* buffer_stream = (stream_buffer_t*)stream;

	FOUNDATION_ASSERT(buffer_stream->size >= buffer_stream->current);

	available = buffer_stream->size - buffer_stream->current;
	if (available < size)
		size = available;

	if (size > 0) {
		memcpy(dest, pointer_offset(buffer_stream->buffer, buffer_stream->current), size);
		buffer_stream->current += size;
		return size;
	}

	return 0;
}

static size_t
buffer_stream_write(stream_t* stream, const void* source, size_t size) {
	size_t available, want;
	stream_buffer_t* buffer_stream = (stream_buffer_t*)stream;

	FOUNDATION_ASSERT(buffer_stream->size >= buffer_stream->current);

	available = buffer_stream->size - buffer_stream->current;
	want = size;

	if (want > available) {
		if (buffer_stream->capacity >= (buffer_stream->current + want)) {
			available = want;
			buffer_stream->size = buffer_stream->current + want;
		} else if (buffer_stream->grow) {
			size_t prev_capacity = buffer_stream->capacity;
			available = want;
			buffer_stream->size = buffer_stream->current + want;
			buffer_stream->capacity = (buffer_stream->size < 1024) ? 1024 : buffer_stream->size + 1024;
			// tail segment from current to size will be overwritten
			buffer_stream->buffer =
			    memory_reallocate(buffer_stream->buffer, buffer_stream->capacity, 0, prev_capacity, 0);
		} else {
			available = buffer_stream->capacity - buffer_stream->current;
			buffer_stream->size = buffer_stream->capacity;
		}
	}

	buffer_stream->lastmod = time_current();

	size = (want < available) ? want : available;
	if (size > 0) {
		memcpy(pointer_offset(buffer_stream->buffer, buffer_stream->current), source, size);
		buffer_stream->current += size;
		return size;
	}

	return 0;
}

static bool
buffer_stream_eos(stream_t* stream) {
	stream_buffer_t* buffer_stream = (stream_buffer_t*)stream;
	return buffer_stream->current >= buffer_stream->size;
}

static void
buffer_stream_flush(stream_t* stream) {
	// lint --e{715, 818} stream unused and could be const, but it's really a vtable function
	FOUNDATION_UNUSED(stream);
}

static void
buffer_stream_truncate(stream_t* stream, size_t size) {
	stream_buffer_t* buffer_stream = (stream_buffer_t*)stream;
	if (buffer_stream->capacity >= size) {
		buffer_stream->size = size;
	} else if (buffer_stream->grow) {
		buffer_stream->capacity = size;
		buffer_stream->buffer =
		    memory_reallocate(buffer_stream->buffer, buffer_stream->capacity, 0, buffer_stream->current, 0);
		buffer_stream->size = buffer_stream->capacity;
	} else {
		buffer_stream->size = buffer_stream->capacity;
	}
	if (buffer_stream->current > buffer_stream->size)
		buffer_stream->current = buffer_stream->size;
	buffer_stream->lastmod = time_current();
}

/*lint -e{818} Function prototype must match stream interface */
static size_t
buffer_stream_size(stream_t* stream) {
	return ((const stream_buffer_t*)stream)->size;
}

static void
buffer_stream_seek(stream_t* stream, ssize_t offset, stream_seek_mode_t direction) {
	stream_buffer_t* buffer_stream = (stream_buffer_t*)stream;
	size_t new_current = 0;
	/*lint --e{571} Used when offset < 0*/
	size_t abs_offset = (size_t)((offset < 0) ? -offset : offset);
	if (direction == STREAM_SEEK_CURRENT) {
		if (offset < 0)
			new_current = (abs_offset > buffer_stream->current) ? 0 : (buffer_stream->current - abs_offset);
		else
			new_current = buffer_stream->current + abs_offset;
	} else if (direction == STREAM_SEEK_BEGIN)
		new_current = (offset > 0) ? abs_offset : 0;
	else if (direction == STREAM_SEEK_END)
		new_current = (offset < 0) ? buffer_stream->size - abs_offset : buffer_stream->size;

	if (new_current > buffer_stream->size)
		buffer_stream->current = buffer_stream->size;
	else
		buffer_stream->current = new_current;
}

static size_t
buffer_stream_tell(stream_t* stream) {
	return ((const stream_buffer_t*)stream)->current;
}

static tick_t
buffer_stream_lastmod(const stream_t* stream) {
	const stream_buffer_t* buffer_stream = (const stream_buffer_t*)stream;
	return buffer_stream->lastmod;
}

static size_t
buffer_stream_available_read(stream_t* stream) {
	const stream_buffer_t* buffer_stream = (const stream_buffer_t*)stream;
	return buffer_stream->size - buffer_stream->current;
}

void
internal_buffer_stream_initialize(void) {
	memset(&buffer_stream_vtable, 0, sizeof(buffer_stream_vtable));
	buffer_stream_vtable.read = buffer_stream_read;
	buffer_stream_vtable.write = buffer_stream_write;
	buffer_stream_vtable.eos = buffer_stream_eos;
	buffer_stream_vtable.flush = buffer_stream_flush;
	buffer_stream_vtable.truncate = buffer_stream_truncate;
	buffer_stream_vtable.size = buffer_stream_size;
	buffer_stream_vtable.seek = buffer_stream_seek;
	buffer_stream_vtable.tell = buffer_stream_tell;
	buffer_stream_vtable.lastmod = buffer_stream_lastmod;
	buffer_stream_vtable.available_read = buffer_stream_available_read;
	buffer_stream_vtable.finalize = buffer_stream_finalize;
}
