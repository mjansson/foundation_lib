/* pipe.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 * 
 * This library provides a cross-platform foundation library in C11 providing basic support data types and
 * functions to write applications and games in a platform-independent fashion. The latest source code is
 * always available at
 * 
 * https://github.com/rampantpixels/foundation_lib
 * 
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#include <foundation/foundation.h>
#include <foundation/internal.h>

#if FOUNDATION_PLATFORM_WINDOWS
#include <foundation/windows.h>
#elif FOUNDATION_PLATFORM_POSIX
#include <foundation/posix.h>
#endif


typedef struct ALIGN(8) _foundation_stream_pipe
{
	FOUNDATION_DECLARE_STREAM;

#if FOUNDATION_PLATFORM_WINDOWS
	HANDLE    handle_read;
	HANDLE    handle_write;
#elif FOUNDATION_PLATFORM_POSIX
	int       fd_read;
	int       fd_write;
#endif
} stream_pipe_t;

static stream_vtable_t _pipe_stream_vtable;


stream_t* pipe_allocate( void )
{
	stream_pipe_t* pipestream = memory_allocate_zero_context( MEMORYCONTEXT_STREAM, sizeof( stream_pipe_t ), 0, MEMORY_PERSISTENT );
	stream_t* stream = (stream_t*)pipestream;

	_stream_initialize( stream, system_byteorder() );

	pipestream->type = STREAMTYPE_PIPE;
	pipestream->path = string_format( "pipe://" STRING_FORMAT_POINTER, pipe );
	pipestream->mode = STREAM_OUT | STREAM_IN | STREAM_BINARY;
	pipestream->sequential = true;

#if FOUNDATION_PLATFORM_WINDOWS
	{
		//Inheritable by default
		SECURITY_ATTRIBUTES security_attribs = {0};
		security_attribs.nLength = sizeof( SECURITY_ATTRIBUTES ); 
		security_attribs.bInheritHandle = TRUE; 
		security_attribs.lpSecurityDescriptor = 0;

		if( !CreatePipe( &pipestream->handle_read, &pipestream->handle_write, &security_attribs, 0 ) )
			log_warnf( WARNING_SYSTEM_CALL_FAIL, "Unable to create unnamed pipe: %s", system_error_message( 0 ) );
	}
#elif FOUNDATION_PLATFORM_POSIX
	int fds[2] = { 0, 0 };
	if( pipe( fds ) < 0 )
		log_warnf( WARNING_SYSTEM_CALL_FAIL, "Unable to create unnamed pipe: %s", system_error_message( 0 ) );
	else
	{
		pipestream->fd_read = fds[0];
		pipestream->fd_write = fds[1];
	}
#endif

	pipestream->vtable = &_pipe_stream_vtable;

	return stream;
}


static void _pipe_stream_deallocate( stream_t* stream )
{
#if FOUNDATION_PLATFORM_WINDOWS
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	if( pipestream->handle_read )
		CloseHandle( pipestream->handle_read );
	pipestream->handle_read = 0;

	if( pipestream->handle_write )
		CloseHandle( pipestream->handle_write );
	pipestream->handle_write = 0;
#elif FOUNDATION_PLATFORM_POSIX
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	if( pipestream->fd_read )
		close( pipestream->fd_read );
	pipestream->fd_read = 0;

	if( pipestream->fd_write )
		close( pipestream->fd_write );
	pipestream->fd_write = 0;	
#endif
}


#if FOUNDATION_PLATFORM_WINDOWS


void* pipe_read_handle( stream_t* stream )
{
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	return ( stream->type == STREAMTYPE_PIPE ) ? pipestream->handle_read : 0;
}


void* pipe_write_handle( stream_t* stream )
{
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	return ( stream->type == STREAMTYPE_PIPE ) ? pipestream->handle_write : 0;
}


void pipe_close_read( stream_t* pipe )
{
}


void pipe_close_write( stream_t* pipe )
{
}


#elif FOUNDATION_PLATFORM_POSIX

int pipe_read_fd( stream_t* stream )
{
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	return ( stream->type == STREAMTYPE_PIPE ) ? pipestream->fd_read : 0;
}


int pipe_write_fd( stream_t* stream )
{
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	return ( stream->type == STREAMTYPE_PIPE ) ? pipestream->fd_write : 0;
}


void pipe_close_read( stream_t* stream )
{
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	if( stream->type != STREAMTYPE_PIPE )
		return;

	if( pipestream->fd_read )
	{
		close( pipestream->fd_read );
		pipestream->fd_read = 0;
	}
}


void pipe_close_write( stream_t* stream )
{
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	if( stream->type != STREAMTYPE_PIPE )
		return;

	if( pipestream->fd_write )
	{
		close( pipestream->fd_write );
		pipestream->fd_write = 0;
	}
}


#endif


static uint64_t _pipe_stream_read( stream_t* stream, void* dest, uint64_t num )
{
#if FOUNDATION_PLATFORM_WINDOWS
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	if( pipestream->handle_read )
	{
		unsigned int num_read = 0;
		ReadFile( pipestream->handle_read, dest, (unsigned int)num, &num_read, 0 );
		return num_read;
	}
#elif FOUNDATION_PLATFORM_POSIX
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	if( pipestream->fd_read )
	{
		int num_read = read( pipestream->fd_read, dest, num );
		if( num_read < 0 )
			num_read = 0;
		return (unsigned int)num_read;
	}	
#endif

	return 0;
}


static uint64_t _pipe_stream_write( stream_t* stream, const void* source, uint64_t num )
{
#if FOUNDATION_PLATFORM_WINDOWS
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	if( pipestream->handle_write )
	{
		unsigned int num_written = 0;
		WriteFile( pipestream->handle_write, source, (unsigned int)num, &num_written, 0 );
		return num_written;
	}
#elif FOUNDATION_PLATFORM_POSIX
	stream_pipe_t* pipestream = (stream_pipe_t*)stream;
	if( pipestream->fd_write )
	{
		int num_written = write( pipestream->fd_read, source, num );
		if( num_written < 0 )
			num_written = 0;
		return (unsigned int)num_written;
	}	
#endif

	return 0;
}


static bool _pipe_stream_isopen( const stream_t* stream )
{
	return stream != 0;
}


static bool _pipe_stream_eos( stream_t* stream )
{
	return !_pipe_stream_isopen( stream );
}


static void _pipe_stream_flush( stream_t* stream )
{
}


static void _pipe_stream_truncate( stream_t* stream, uint64_t size )
{
}


static uint64_t _pipe_stream_size( stream_t* stream )
{
	return 0;
}


static void _pipe_stream_seek( stream_t* stream, int64_t offset, stream_seek_mode_t direction )
{
}


static int64_t _pipe_stream_tell( stream_t* stream )
{
	return 0;
}


static uint64_t _pipe_stream_lastmod( const stream_t* stream )
{
	return time_current();
}


static uint64_t _pipe_stream_available_read( stream_t* stream )
{
	return 0;
}


void _pipe_stream_initialize( void )
{
	_pipe_stream_vtable.read = _pipe_stream_read;
	_pipe_stream_vtable.write = _pipe_stream_write;
	_pipe_stream_vtable.isopen = _pipe_stream_isopen;
	_pipe_stream_vtable.eos = _pipe_stream_eos;
	_pipe_stream_vtable.flush = _pipe_stream_flush;
	_pipe_stream_vtable.truncate = _pipe_stream_truncate;
	_pipe_stream_vtable.size = _pipe_stream_size;
	_pipe_stream_vtable.seek = _pipe_stream_seek;
	_pipe_stream_vtable.tell = _pipe_stream_tell;
	_pipe_stream_vtable.lastmod = _pipe_stream_lastmod;
	_pipe_stream_vtable.available_read = _pipe_stream_available_read;
	_pipe_stream_vtable.deallocate = _pipe_stream_deallocate;
}
