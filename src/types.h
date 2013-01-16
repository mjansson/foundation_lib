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
typedef int           (* error_callback_fn )( error_level_t, error_t );

//! Assert handler callback
typedef int           (* assert_handler_fn )( const char* condition, const char* file, int line, const char* msg );

typedef void*         (* memory_allocate_fn )( uint64_t size, unsigned int align, memory_hint_t hint );
typedef void*         (* memory_allocate_zero_fn )( uint64_t size, unsigned int align, memory_hint_t hint );
typedef void*         (* memory_reallocate_fn )( void* p, uint64_t size, unsigned int align );
typedef void          (* memory_deallocate_fn )( void* p );


// COMPLEX TYPES

//! Memory management callbacks
typedef struct _foundation_memory_system
{
	memory_allocate_fn              allocate;
	memory_allocate_zero_fn         allocate_zero;
	memory_reallocate_fn            reallocate;
	memory_deallocate_fn            deallocate;
} memory_system_t;

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

