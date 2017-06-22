/* ringbuffer.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#include <foundation/foundation.h>
#include <foundation/internal.h>

#define RINGBUFFER_FROM_STREAM( stream ) ((ringbuffer_t*)&stream->total_read)

static stream_vtable_t _ringbuffer_stream_vtable;

ringbuffer_t*
ringbuffer_allocate(size_t size) {
	ringbuffer_t* buffer = memory_allocate(0, sizeof(ringbuffer_t) + size, 0, MEMORY_PERSISTENT);

	ringbuffer_initialize(buffer, size);

	return buffer;
}

void
ringbuffer_initialize(ringbuffer_t* buffer, size_t size) {
	buffer->total_read = 0;
	buffer->total_write = 0;
	buffer->offset_read = 0;
	buffer->offset_write = 0;
	buffer->buffer_size = size;
}

void
ringbuffer_deallocate(ringbuffer_t* buffer) {
	ringbuffer_finalize(buffer);
	memory_deallocate(buffer);
}

void
ringbuffer_finalize(ringbuffer_t* buffer) {
	FOUNDATION_UNUSED(buffer);
}

size_t
ringbuffer_size(ringbuffer_t* buffer) {
	return buffer->buffer_size;
}

void
ringbuffer_reset(ringbuffer_t* buffer) {
	buffer->total_read = 0;
	buffer->total_write = 0;
	buffer->offset_read = 0;
	buffer->offset_write = 0;
}

size_t
ringbuffer_read(ringbuffer_t* buffer, void* dest, size_t num) {
	size_t do_read;
	size_t max_read;
	size_t buffer_size;
	size_t offset_read;
	size_t offset_write;

	buffer_size = buffer->buffer_size;
	offset_read = buffer->offset_read;
	offset_write = buffer->offset_write;

	if (offset_read > offset_write)
		max_read = buffer_size - offset_read;
	else
		max_read = offset_write - offset_read;

	do_read = num;
	if (do_read > max_read)
		do_read = max_read;

	if (!do_read)
		return 0;

	if (dest)
		memcpy(dest, buffer->buffer + offset_read, do_read);

	offset_read += do_read;
	if (offset_read == buffer_size)
		offset_read = 0;

	buffer->offset_read = offset_read;
	buffer->total_read += do_read;

	if ((do_read < num) && (offset_read == 0) && (offset_write > 0))
		do_read += ringbuffer_read(buffer, dest ? pointer_offset(dest, do_read) : dest, num - do_read);

	return do_read;
}

size_t
ringbuffer_write(ringbuffer_t* buffer, const void* source, size_t num) {
	size_t do_write;
	size_t max_write;
	size_t buffer_size;
	size_t offset_read;
	size_t offset_write;

	buffer_size = buffer->buffer_size;
	offset_read = buffer->offset_read;
	offset_write = buffer->offset_write;

	if (offset_write >= offset_read) {
		max_write = buffer_size - offset_write;
		//Don't read so write aligns to read, then the entire buffer is discarded
		if (max_write && !buffer->offset_read)
			--max_write;
	}
	else {
		//Don't read so write aligns to read, then the entire buffer is discarded
		max_write = offset_read - (offset_write + 1);
	}

	do_write = num;
	if (do_write > max_write)
		do_write = max_write;

	if (!do_write)
		return 0;

	memcpy(buffer->buffer + offset_write, source, do_write);

	offset_write += do_write;
	if (offset_write == buffer_size) {
		FOUNDATION_ASSERT_MSG(buffer->offset_read, "Ring buffer internal failure, discarded entire buffer");
		offset_write = 0;
	}

	buffer->offset_write = offset_write;
	buffer->total_write += do_write;

	if ((do_write < num) && (offset_write == 0) && (offset_read > 0))
		do_write += ringbuffer_write(buffer, pointer_offset_const(source, do_write), num - do_write);

	return do_write;
}

uint64_t
ringbuffer_total_read(ringbuffer_t* buffer) {
	return buffer->total_read;
}

uint64_t
ringbuffer_total_written(ringbuffer_t* buffer) {
	return buffer->total_write;
}

static size_t
_ringbuffer_stream_read(stream_t* stream, void* dest, size_t num) {
	stream_ringbuffer_t* rbstream = (stream_ringbuffer_t*)stream;
	ringbuffer_t* buffer = RINGBUFFER_FROM_STREAM(rbstream);

	size_t num_read = ringbuffer_read(buffer, dest, num);

	while (num_read < num) {
		rbstream->pending_read = 1;
		atomic_thread_fence_release();

		atomic_thread_fence_acquire();
		if (rbstream->pending_write)
			semaphore_post(&rbstream->signal_read);
		semaphore_wait(&rbstream->signal_write);

		rbstream->pending_read = 0;
		atomic_thread_fence_release();

		num_read += ringbuffer_read(buffer, dest ? pointer_offset(dest, num_read) : 0, num - num_read);
	}

	atomic_thread_fence_acquire();
	if (rbstream->pending_write)
		semaphore_post(&rbstream->signal_read);

	return num_read;
}

static size_t
_ringbuffer_stream_write(stream_t* stream, const void* source, size_t num) {
	stream_ringbuffer_t* rbstream = (stream_ringbuffer_t*)stream;
	ringbuffer_t* buffer = RINGBUFFER_FROM_STREAM(rbstream);

	size_t num_write = ringbuffer_write(buffer, source, num);

	while (num_write < num) {
		rbstream->pending_write = 1;
		atomic_thread_fence_release();

		atomic_thread_fence_acquire();
		if (rbstream->pending_read)
			semaphore_post(&rbstream->signal_write);
		semaphore_wait(&rbstream->signal_read);

		rbstream->pending_write = 0;
		atomic_thread_fence_release();

		num_write += ringbuffer_write(buffer, source ? pointer_offset_const(source, num_write) : 0,
		                              num - num_write);
	}

	atomic_thread_fence_acquire();
	if (rbstream->pending_read)
		semaphore_post(&rbstream->signal_write);

	return num_write;
}

static bool
_ringbuffer_stream_eos(stream_t* stream) {
	stream_ringbuffer_t* buffer = (stream_ringbuffer_t*)stream;
	return buffer->total_size ? (buffer->total_read >= buffer->total_size) : false;
}

static void
_ringbuffer_stream_flush(stream_t* stream) {
	FOUNDATION_UNUSED(stream);
}

static void
_ringbuffer_stream_truncate(stream_t* stream, size_t size) {
	stream_ringbuffer_t* buffer = (stream_ringbuffer_t*)stream;
	buffer->total_size = size;
}

static size_t
_ringbuffer_stream_size(stream_t* stream) {
	return ((stream_ringbuffer_t*)stream)->total_size;
}

static void
_ringbuffer_stream_seek(stream_t* stream, ssize_t offset, stream_seek_mode_t direction) {
	if ((direction != STREAM_SEEK_CURRENT) || (offset < 0)) {
		log_error(0, ERROR_UNSUPPORTED,
		          STRING_CONST("Invalid call, only forward seeking allowed on ringbuffer streams"));
		return;
	}

	/*lint -e{534,571} */
	_ringbuffer_stream_read(stream, 0, (size_t)offset);
}

static size_t
_ringbuffer_stream_tell(stream_t* stream) {
	stream_ringbuffer_t* buffer = (stream_ringbuffer_t*)stream;
	return (size_t)buffer->total_read;
}

static tick_t
_ringbuffer_stream_lastmod(const stream_t* stream) {
	FOUNDATION_UNUSED(stream);
	return time_system();
}

static size_t
_ringbuffer_stream_available_read(stream_t* stream) {
	stream_ringbuffer_t* buffer = (stream_ringbuffer_t*)stream;
	return (size_t)(buffer->total_write - buffer->total_read);
}

stream_t*
ringbuffer_stream_allocate(size_t buffer_size, size_t total_size) {
	stream_ringbuffer_t* bufferstream = memory_allocate(0, sizeof(stream_ringbuffer_t) + buffer_size, 0,
	                                                    MEMORY_PERSISTENT);

	ringbuffer_stream_initialize(bufferstream, buffer_size, total_size);

	return (stream_t*)bufferstream;
}

void
ringbuffer_stream_initialize(stream_ringbuffer_t* stream, size_t buffer_size, size_t total_size) {
	memset(stream, 0, sizeof(stream_ringbuffer_t));

	stream_initialize((stream_t*)stream, system_byteorder());

	stream->type = STREAMTYPE_RINGBUFFER;
	stream->sequential = 1;
	stream->path = string_allocate_format(STRING_CONST("ringbuffer://0x%" PRIfixPTR),
	                                      (uintptr_t)stream);
	stream->mode = STREAM_OUT | STREAM_IN | STREAM_BINARY;

	ringbuffer_initialize(RINGBUFFER_FROM_STREAM(stream), buffer_size);
	semaphore_initialize(&stream->signal_read, 0);
	semaphore_initialize(&stream->signal_write, 0);

	stream->total_size = total_size;

	stream->vtable = &_ringbuffer_stream_vtable;
}

static void
_ringbuffer_stream_finalize(stream_t* stream) {
	stream_ringbuffer_t* bufferstream = (stream_ringbuffer_t*)stream;

	if (!bufferstream || (stream->type != STREAMTYPE_RINGBUFFER))
		return;

	semaphore_finalize(&bufferstream->signal_read);
	semaphore_finalize(&bufferstream->signal_write);
}

void
_ringbuffer_stream_initialize(void) {
	//Setup global vtable
	_ringbuffer_stream_vtable.read = _ringbuffer_stream_read;
	_ringbuffer_stream_vtable.write = _ringbuffer_stream_write;
	_ringbuffer_stream_vtable.eos = _ringbuffer_stream_eos;
	_ringbuffer_stream_vtable.flush = _ringbuffer_stream_flush;
	_ringbuffer_stream_vtable.truncate = _ringbuffer_stream_truncate;
	_ringbuffer_stream_vtable.size = _ringbuffer_stream_size;
	_ringbuffer_stream_vtable.seek = _ringbuffer_stream_seek;
	_ringbuffer_stream_vtable.tell = _ringbuffer_stream_tell;
	_ringbuffer_stream_vtable.lastmod = _ringbuffer_stream_lastmod;
	_ringbuffer_stream_vtable.available_read = _ringbuffer_stream_available_read;
	_ringbuffer_stream_vtable.finalize = _ringbuffer_stream_finalize;
}


