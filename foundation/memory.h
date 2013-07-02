/* memory.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file memory.h
    Memory allocation and deallocation */

#include <foundation/platform.h>
#include <foundation/types.h>


FOUNDATION_API void*             memory_allocate( uint64_t size, unsigned int align, memory_hint_t hint );
FOUNDATION_API void*             memory_allocate_zero( uint64_t size, unsigned int align, memory_hint_t hint );
FOUNDATION_API void*             memory_allocate_context( uint16_t context, uint64_t size, unsigned int align, memory_hint_t hint );
FOUNDATION_API void*             memory_allocate_zero_context( uint16_t context, uint64_t size, unsigned int align, memory_hint_t hint );
FOUNDATION_API void*             memory_reallocate( void* p, uint64_t size, unsigned int align );
FOUNDATION_API void              memory_deallocate( void* p );

#if BUILD_ENABLE_MEMORY_CONTEXT

FOUNDATION_API void              memory_context_push( uint16_t context );
FOUNDATION_API void              memory_context_pop( void );
FOUNDATION_API uint16_t          memory_context( void );
FOUNDATION_API void              memory_context_thread_deallocate( void );

#else

#define memory_context_push( context )      /*lint -save -e506 -e751 */ do { (void)sizeof( context ); } while(0) /*lint -restore -e506 -e751 */
#define memory_context_pop()                do { /* */ } while(0)
#define memory_context()                    MEMORYCONTEXT_GLOBAL
#define memory_context_thread_deallocate()  do { /* */ } while(0)

#endif

#if BUILD_ENABLE_MEMORY_TRACKER

FOUNDATION_API void              memory_set_tracker( memory_tracker_t tracker );

#else

#define memory_set_tracker( tracker )       /*lint -save -e506 -e751 */ do { (void)sizeof( tracker ); } while(0) /*lint -restore -e506 -e751 */

#endif

FOUNDATION_API memory_system_t   memory_system_malloc( void );
FOUNDATION_API memory_tracker_t  memory_tracker_local( void );
