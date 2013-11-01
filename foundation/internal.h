/* internal.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#pragma once

/*! \file internal.h
    Internal types */

#include <foundation/platform.h>
#include <foundation/types.h>


#define FOUNDATION_DECLARE_RINGBUFFER \
	uint64_t           total_read;    \
	uint64_t           total_write;   \
	unsigned int       offset_read;   \
	unsigned int       offset_write;  \
	unsigned int       buffer_size;   \
	char               buffer[]


struct _foundation_ringbuffer
{
	FOUNDATION_DECLARE_RINGBUFFER;
};


//Stream vtable
typedef uint64_t  (*stream_read_fn)( stream_t*, void*, uint64_t );
typedef uint64_t  (*stream_write_fn)( stream_t*, const void*, uint64_t );
typedef bool      (*stream_eos_fn)( stream_t* );
typedef void      (*stream_flush_fn)( stream_t* );
typedef void      (*stream_truncate_fn)( stream_t*, uint64_t );
typedef uint64_t  (*stream_size_fn)( stream_t* );
typedef void      (*stream_seek_fn)( stream_t*, int64_t, stream_seek_mode_t );
typedef int64_t   (*stream_tell_fn)( stream_t* );
typedef uint64_t  (*stream_lastmod_fn)( const stream_t* );
typedef uint128_t (*stream_md5_fn)( stream_t* );
typedef void      (*stream_buffer_read_fn)( stream_t* );
typedef uint64_t  (*stream_available_read_fn)( stream_t* );
typedef void      (*stream_deallocate_fn)( stream_t* );
typedef stream_t* (*stream_clone_fn)( stream_t* );

typedef struct _foundation_stream_vtable
{
	stream_read_fn           read;
	stream_write_fn          write;
	stream_eos_fn            eos;
	stream_flush_fn          flush;
	stream_truncate_fn       truncate;
	stream_size_fn           size;
	stream_seek_fn           seek;
	stream_tell_fn           tell;
	stream_lastmod_fn        lastmod;
	stream_md5_fn            md5;
	stream_buffer_read_fn    buffer_read;
	stream_available_read_fn available_read;
	stream_deallocate_fn     deallocate;
	stream_clone_fn          clone;
} stream_vtable_t;

#define FOUNDATION_DECLARE_STREAM                     \
	unsigned int             type:16;                 \
	unsigned int             sequential:1;            \
	unsigned int             reliable:1;              \
	unsigned int             inorder:1;               \
	unsigned int             swap:1;                  \
	unsigned int             unused_streamflags:12;   \
	byteorder_t              byteorder;               \
	unsigned int             mode;                    \
	char*                    path;                    \
	stream_vtable_t*         vtable

struct ALIGN(8) _foundation_stream
{
	FOUNDATION_DECLARE_STREAM;
};


//Internal entry points

FOUNDATION_API void _stream_initialize( stream_t* stream, byteorder_t order );

FOUNDATION_API void _ringbuffer_stream_initialize( void );
FOUNDATION_API void _buffer_stream_initialize( void );
#if FOUNDATION_PLATFORM_ANDROID
FOUNDATION_API void _asset_stream_initialize( void );
#endif
FOUNDATION_API void _pipe_stream_initialize( void );

FOUNDATION_API int _log_initialize( void );
FOUNDATION_API void _log_shutdown( void );

FOUNDATION_API int _memory_initialize( const memory_system_t memory );
FOUNDATION_API void _memory_preallocate( void );
FOUNDATION_API void _memory_shutdown( void );

FOUNDATION_API int _time_initialize( void );
FOUNDATION_API void _time_shutdown( void );

FOUNDATION_API int _thread_initialize( void );
FOUNDATION_API void _thread_shutdown( void );

FOUNDATION_API int _environment_initialize( const application_t application );
FOUNDATION_API void _environment_shutdown( void );

FOUNDATION_API int _library_initialize( void );
FOUNDATION_API void _library_shutdown( void );

FOUNDATION_API int _system_initialize( void );
FOUNDATION_API void _system_shutdown( void );

FOUNDATION_API int _fs_initialize( void );
FOUNDATION_API void _fs_shutdown( void );

FOUNDATION_API int _random_initialize( void );
FOUNDATION_API void _random_shutdown( void );

FOUNDATION_API int _config_initialize( void );
FOUNDATION_API void _config_shutdown( void );

FOUNDATION_API void _profile_thread_cleanup( void );

FOUNDATION_API void _static_hash_initialize( void );
FOUNDATION_API void _static_hash_shutdown( void );

FOUNDATION_API void foundation_startup( void );


// Internal functions

FOUNDATION_API uint32_t      hashtable32_raw( hashtable32_t* table, uint32_t key );
FOUNDATION_API uint64_t      hashtable64_raw( hashtable64_t* table, uint64_t key );
