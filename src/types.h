/* types.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file types.h
    Foundation types */

#include <platform.h>


// PRIMITIVE TYPES

//! Error severity level
typedef enum
{
	ERRORLEVEL_NONE    = 0,
	ERRORLEVEL_DEBUG,
	ERRORLEVEL_INFO,
	ERRORLEVEL_WARNING,
	ERRORLEVEL_ERROR,
	ERRORLEVEL_PANIC
} error_level_t;

//! Error identifiers
typedef enum
{
	ERROR_NONE              = 0,
	ERROR_INVALID_VALUE,
	ERROR_UNSUPPORTED,
	ERROR_NOT_IMPLEMENTED,
	ERROR_OUT_OF_MEMORY,
	ERROR_INTERNAL_FAILURE,
	ERROR_MALLOC_FAILURE,
	ERROR_MEMORY_LEAK,
	ERROR_ACCESS_DENIED,
	ERROR_EXCEPTION,
	ERROR_SYSTEM_CALL_FAIL,
	ERROR_SCRIPT,
	ERROR_UNKNOWN_TYPE,
	ERROR_UNKNOWN_RESOURCE,
	ERROR_MEMORY_ALIGNMENT,
	ERROR_DEPRECATED
} error_t;

//! Warning classes
typedef enum
{
	WARNING_PERFORMANCE,
	WARNING_DEPRECATED,
	WARNING_BAD_DATA,
	WARNING_MEMORY,
	WARNING_UNSUPPORTED,
	WARNING_SUSPICIOUS,
	WARNING_SCRIPT,
	WARNING_SYSTEM_CALL_FAIL,
	WARNING_DEADLOCK
} warning_class_t;

//! Memory hints
typedef enum
{
	MEMORY_TEMPORARY,
	MEMORY_PERSISTENT
} memory_hint_t;

//! \Platform identifiers. For compile-time platform selection, use the FOUNDATION_PLATFORM_[...] preprocessor macros
typedef enum
{
	PLATFORM_WINDOWS   = 1,
	PLATFORM_LINUX,
	PLATFORM_MACOSX,
	PLATFORM_IOS,
	PLATFORM_ANDROID,
	PLATFORM_RASPBERRYPI
} platform_t;

/*! Architecture identifiers for all architectures the engine supports. For compile-time selection of
    architecture, use the FOUNDATION_PLATFORM_ARCH_[...] preprocessor macros. */
typedef enum
{
	/*! x86 (Classic 32-bit x86 compatible CPUs) */
	ARCHITECTURE_X86          = 0,

	/*! x86-64 (x86 with 64-bit extensions) */
	ARCHITECTURE_X86_64       = 1,

	/*! PowerPC 32-bit */
	ARCHITECTURE_PPC          = 2,
	
	/*! PowerPC 64-bit */
	ARCHITECTURE_PPC_64       = 3,

	/*! ARM 6 */
	ARCHITECTURE_ARM6         = 6,

	/*! ARM 7 */
	ARCHITECTURE_ARM7         = 7
} architecture_t;

//! Machine byte order identifiers
typedef enum
{
	BYTEORDER_LITTLEENDIAN = 0,
	BYTEORDER_BIGENDIAN    = 1
} byteorder_t;

//! Open modes for streams
typedef enum
{
	STREAM_IN                  = 0x0001,
	STREAM_OUT                 = 0x0002,
	STREAM_TRUNCATE            = 0x0010,
	STREAM_ATEND               = 0x0020,
	STREAM_BINARY              = 0x0100,
	STREAM_SYNC                = 0x0200
} stream_mode_t;

typedef enum
{
	STREAMTYPE_INVALID         = 0,
	STREAMTYPE_MEMORY,
	STREAMTYPE_FILE,
	STREAMTYPE_SOCKET,
	STREAMTYPE_RINGBUFFER,
	STREAMTYPE_ASSET
} stream_type_t;

//! Stream seek directions
typedef enum
{
	STREAM_SEEK_BEGIN          = 0x0000,
	STREAM_SEEK_CURRENT        = 0x0001,
	STREAM_SEEK_END            = 0x0002
} stream_seek_mode_t;

//! GUID
typedef uint128_t        guid_t;

//! Hash value
typedef uint64_t         hash_t;

//! Tick type
typedef uint64_t         tick_t;

//! Deltatime type
typedef real             deltatime_t;

//! Object handle
typedef uint64_t         object_t;

//! Error handler callback
typedef int           (* error_callback_fn )( error_level_t level, error_t error );

//! Assert handler callback
typedef int           (* assert_handler_fn )( const char* condition, const char* file, int line, const char* msg );

//! Log output callback
typedef void          (* log_callback_fn )( int severity, const char* msg );

typedef void*         (* memory_allocate_fn )( uint64_t size, unsigned int align, memory_hint_t hint );
typedef void*         (* memory_allocate_zero_fn )( uint64_t size, unsigned int align, memory_hint_t hint );
typedef void*         (* memory_reallocate_fn )( void* p, uint64_t size, unsigned int align );
typedef void          (* memory_deallocate_fn )( void* p );

//! Callback function for writing profiling data to a stream
typedef void          (* profile_write_fn)( void*, uint64_t );


// COMPLEX TYPES

//! Memory management callbacks
typedef struct _foundation_memory_system
{
	memory_allocate_fn              allocate;
	memory_allocate_zero_fn         allocate_zero;
	memory_reallocate_fn            reallocate;
	memory_deallocate_fn            deallocate;
} memory_system_t;

//! Application declaration
typedef struct _foundation_application
{
	const char*                     name;
	const char*                     short_name;
	const char*                     config_dir;
} application_t;

typedef struct _foundation_error_frame
{
	const char*                     name;
	const char*                     data;
} error_frame_t;

typedef struct _foundation_error_context
{
	error_frame_t                   frame[BUILD_ERROR_CONTEXT_MAX_DEPTH];
	int                             depth;
} error_context_t;

//! Object map
typedef struct _foundation_objectmap
{
	ALIGN(16) volatile uint64_t     free;
	uint64_t                        size;
	ALIGN(16) volatile uint64_t     id;
	uint64_t                        size_bits;
	uint64_t                        id_max;
	uint64_t                        mask_index;
	uint64_t                        mask_id;
	void*                           map[];
} objectmap_t;


// OPAQUE COMPLEX TYPES

typedef struct _foundation_stream           stream_t;
