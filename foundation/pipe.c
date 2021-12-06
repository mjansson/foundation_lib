/* pipe.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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
#include <io.h>
#include <fcntl.h>
#define close _close
#define read _read
#define write _write
typedef unsigned int pipe_size_t;
#elif FOUNDATION_PLATFORM_POSIX
#include <foundation/posix.h>
typedef size_t pipe_size_t;
#endif

static stream_vtable_t pipe_stream_vtable;

stream_t*
pipe_allocate(void) {
	stream_pipe_t* pipestream = memory_allocate(HASH_STREAM, sizeof(stream_pipe_t), 8, MEMORY_PERSISTENT);
	pipe_initialize(pipestream);
	return (stream_t*)pipestream;
}

void
pipe_initialize(stream_pipe_t* pipestream) {
	stream_t* stream = (stream_t*)pipestream;

	memset(stream, 0, sizeof(stream_pipe_t));
	stream_initialize(stream, system_byteorder());

	pipestream->type = STREAMTYPE_PIPE;
	pipestream->path = string_allocate_format(STRING_CONST("pipe://0x%" PRIfixPTR), (uintptr_t)pipestream);
	pipestream->mode = STREAM_OUT | STREAM_IN | STREAM_BINARY;
	pipestream->sequential = true;

#if FOUNDATION_PLATFORM_WINDOWS
	{
		// Inheritable by default so process can use for stdstreams
		HANDLE hread, hwrite;
		SECURITY_ATTRIBUTES security_attribs;
		memset(&security_attribs, 0, sizeof(security_attribs));
		security_attribs.nLength = sizeof(SECURITY_ATTRIBUTES);
		security_attribs.bInheritHandle = TRUE;
		security_attribs.lpSecurityDescriptor = 0;

		if (!CreatePipe(&hread, &hwrite, &security_attribs, 0)) {
			string_const_t errmsg = system_error_message(0);
			log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to create unnamed pipe: %.*s"),
			           STRING_FORMAT(errmsg));
		} else {
			pipestream->fd_read = _open_osfhandle((intptr_t)hread, _O_RDONLY | _O_BINARY);
			pipestream->fd_write = _open_osfhandle((intptr_t)hwrite, _O_WRONLY | _O_BINARY);
			if (!pipestream->fd_read || !pipestream->fd_write) {
				string_const_t errmsg = system_error_message(0);
				log_errorf(0, ERROR_SYSTEM_CALL_FAIL,
				           STRING_CONST("Unable to create unnamed pipe file descriptors: %.*s"), STRING_FORMAT(errmsg));
				if (pipestream->fd_read)
					close(pipestream->fd_read);
				else
					CloseHandle(hread);
				if (pipestream->fd_write)
					close(pipestream->fd_write);
				else
					CloseHandle(hwrite);
				pipestream->fd_read = pipestream->fd_write = 0;
			} else {
				unsigned long mode = PIPE_READMODE_BYTE | PIPE_WAIT;
				if (!SetNamedPipeHandleState(hread, &mode, 0, 0)) {
					string_const_t errmsg = system_error_message(0);
					log_errorf(0, ERROR_SYSTEM_CALL_FAIL,
					           STRING_CONST("Unable to create unnamed pipe handle state: %.*s"), STRING_FORMAT(errmsg));
				}
			}
		}
	}
#else
	int fds[2] = {0, 0};
	if (pipe(fds) < 0) {
		string_const_t errmsg = system_error_message(0);
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to create unnamed pipe: %.*s"),
		           STRING_FORMAT(errmsg));
	}
	pipestream->fd_read = fds[0];
	pipestream->fd_write = fds[1];
#endif

	pipestream->vtable = &pipe_stream_vtable;
}

static void
pipe_finalize(stream_t* stream) {
	stream_pipe_t* pipe = (stream_pipe_t*)stream;

	if (!pipe || (stream->type != STREAMTYPE_PIPE))
		return;

	if (pipe->fd_read)
		close(pipe->fd_read);
	pipe->fd_read = 0;

	if (pipe->fd_write)
		close(pipe->fd_write);
	pipe->fd_write = 0;
}

void
pipe_close_read(stream_t* stream) {
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	FOUNDATION_ASSERT(stream->type == STREAMTYPE_PIPE);

	if (pipestream->fd_read) {
		close(pipestream->fd_read);
		pipestream->fd_read = 0;
	}

	pipestream->mode &= ~STREAM_IN;
}

void
pipe_close_write(stream_t* stream) {
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	FOUNDATION_ASSERT(stream->type == STREAMTYPE_PIPE);

	if (pipestream->fd_write) {
		close(pipestream->fd_write);
		pipestream->fd_write = 0;
	}

	pipestream->mode &= ~STREAM_OUT;
}

#if FOUNDATION_PLATFORM_WINDOWS

void*
pipe_read_handle(stream_t* stream) {
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	intptr_t handle = stream && (stream->type == STREAMTYPE_PIPE) ? _get_osfhandle(pipestream->fd_read) : 0;
	return (void*)handle;
}

void*
pipe_write_handle(stream_t* stream) {
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	intptr_t handle = stream && (stream->type == STREAMTYPE_PIPE) ? _get_osfhandle(pipestream->fd_write) : 0;
	return (void*)handle;
}

#endif

int
pipe_read_fd(stream_t* stream) {
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	return stream && (stream->type == STREAMTYPE_PIPE) ? pipestream->fd_read : 0;
}

int
pipe_write_fd(stream_t* stream) {
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	return stream && (stream->type == STREAMTYPE_PIPE) ? pipestream->fd_write : 0;
}

static size_t
pipe_stream_read(stream_t* stream, void* dest, size_t size) {
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	FOUNDATION_ASSERT(stream->type == STREAMTYPE_PIPE);
	if (pipestream->fd_read && ((pipestream->mode & STREAM_IN) != 0)) {
		size_t total_read = 0;
		do {
			ssize_t was_read =
			    read(pipestream->fd_read, pointer_offset(dest, total_read), (pipe_size_t)(size - total_read));
			if (was_read <= 0) {
				pipestream->eos = true;
				break;
			}
			total_read += (size_t)was_read;
		} while (total_read < size);
		return total_read;
	}

	return 0;
}

static size_t
pipe_stream_write(stream_t* stream, const void* source, size_t size) {
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	FOUNDATION_ASSERT(stream->type == STREAMTYPE_PIPE);
	if (pipestream->fd_write && ((pipestream->mode & STREAM_OUT) != 0)) {
		size_t total_written = 0;
		do {
			ssize_t was_written = write(pipestream->fd_write, pointer_offset_const(source, total_written),
			                            (pipe_size_t)(size - total_written));
			if (was_written <= 0) {
				pipestream->eos = true;
				break;
			}
			total_written += (size_t)was_written;
		} while (total_written < size);
		return total_written;
	}

	return 0;
}

static bool
pipe_stream_eos(stream_t* stream) {
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	return !stream || (!pipestream->fd_read && !pipestream->fd_write) || pipestream->eos;
}

static void
pipe_stream_flush(stream_t* stream) {
	FOUNDATION_UNUSED(stream);
}

static void
pipe_stream_truncate(stream_t* stream, size_t size) {
	FOUNDATION_UNUSED(stream);
	FOUNDATION_UNUSED(size);
}

static size_t
pipe_stream_size(stream_t* stream) {
	FOUNDATION_UNUSED(stream);
	return 0;
}

static void
pipe_stream_seek(stream_t* stream, ssize_t offset, stream_seek_mode_t direction) {
	FOUNDATION_UNUSED(stream);
	FOUNDATION_UNUSED(offset);
	FOUNDATION_UNUSED(direction);
}

static size_t
pipe_stream_tell(stream_t* stream) {
	FOUNDATION_UNUSED(stream);
	return 0;
}

static tick_t
pipe_stream_lastmod(const stream_t* stream) {
	FOUNDATION_UNUSED(stream);
	return time_current();
}

static size_t
pipe_stream_available_read(stream_t* stream) {
	FOUNDATION_UNUSED(stream);
	return 0;
}

#if FOUNDATION_PLATFORM_WINDOWS
#undef read
#undef write
#endif

void
internal_pipe_stream_initialize(void) {
	pipe_stream_vtable.read = pipe_stream_read;
	pipe_stream_vtable.write = pipe_stream_write;
	pipe_stream_vtable.eos = pipe_stream_eos;
	pipe_stream_vtable.flush = pipe_stream_flush;
	pipe_stream_vtable.truncate = pipe_stream_truncate;
	pipe_stream_vtable.size = pipe_stream_size;
	pipe_stream_vtable.seek = pipe_stream_seek;
	pipe_stream_vtable.tell = pipe_stream_tell;
	pipe_stream_vtable.lastmod = pipe_stream_lastmod;
	pipe_stream_vtable.available_read = pipe_stream_available_read;
	pipe_stream_vtable.finalize = pipe_finalize;
}
