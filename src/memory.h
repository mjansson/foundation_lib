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

#include <platform.h>
#include <types.h>


FOUNDATION_API void*             memory_allocate( uint64_t size, unsigned int align, memory_hint_t hint );
FOUNDATION_API void*             memory_allocate_zero( uint64_t size, unsigned int align, memory_hint_t hint );
FOUNDATION_API void*             memory_reallocate( void* p, uint64_t size, unsigned int align );
FOUNDATION_API void              memory_deallocate( void* p );

FOUNDATION_API memory_system_t   memory_system_malloc( void );
