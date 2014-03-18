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
    Memory allocation and deallocation. The actual implementation of allocation/deallocation can be
    controlled with the memory system declaration given to foundation_initialize at startup.

    Memory allocation/deallocation can be tracked with memory_set_tracker if the build configuration
    BUILD_ENABLE_MEMORY_TRACKER is set to 1. If not, all memory tracker calls are statically removed
    at compile time.

    Memory can be tracked by context, set as a hash value (usually a static string hash) if the build
    configuration BUILD_ENABLE_MEMORY_CONTEXT is set to 1. If not, all memory context calls are statically
    removed and context 0 is used everywhere. Memory contexts are stored per thread. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate memory without initialization. Memory will be aligned to the given alingment unless
    set to 0, which will cause alignment to be ignored. Memory hints can be given to allow the
    memory system to make better choises of how to allocate the memory (persistent, temporary, thread local,
    32-bit space...). The currently set memory context is used.
    \param size                  Requested size
    \param align                 Requested alignment
    \param hint                  Hint flags
    \return                      Memory address to a memory block of the requested size and alignment, 0 if out of memory */
FOUNDATION_API void*             memory_allocate( uint64_t size, unsigned int align, int hint );

/*! Allocate memory and initialize to zero. Except for initialization this works just as memory_allocate()
    \param size                  Requested size
    \param align                 Requested alignment
    \param hint                  Hint flags
    \return                      Memory address to a zeroed out memory block of the requested size and alignment, 0 if out of memory */
FOUNDATION_API void*             memory_allocate_zero( uint64_t size, unsigned int align, int hint );

/*! Allocate memory in the given context without initialization. Except for the explicit context this works
    just as memory_allocate()
    \param context               Memory context
    \param size                  Requested size
    \param align                 Requested alignment
    \param hint                  Hint flags
    \return                      Memory address to a zeroed out memory block of the requested size and alignment, 0 if out of memory */
FOUNDATION_API void*             memory_allocate_context( uint64_t context, uint64_t size, unsigned int align, int hint );

/*! Allocate memory in the given context and initialize to zero. Except for the explicit context this works
    just as memory_allocate_zero()
    \param context               Memory context
    \param size                  Requested size
    \param align                 Requested alignment
    \param hint                  Hint flags
    \return                      Memory address to a zeroed out memory block of the requested size and alignment, 0 if out of memory */
FOUNDATION_API void*             memory_allocate_zero_context( uint64_t context, uint64_t size, unsigned int align, int hint );

/*! Reallocate a memory block. Old memory content will be preserved, and if requested size is larger than the current size the
    new memory area will be uninitialized. The currently set memory context is used for any allocation. It is safe to pass null
    as the current memory block pointer.
    \param p                     Current memory block pointer
    \param size                  New requested size
    \param align                 New requested alignment
    \param oldsize               Previous size
    \return                      Memory address to a (potentially new) memory block of the requested size and alignment, 0 if out of memory */
FOUNDATION_API void*             memory_reallocate( void* p, uint64_t size, unsigned int align, uint64_t oldsize );

/*! Deallocate a memory block
    \param p                     Memory block pointer */
FOUNDATION_API void              memory_deallocate( void* p );

/*! \fn memory_context_push
    Set a new current memory context by pushing it onto the context stack. A call
    to memory_context_pop() will restore the previous memory context. Memory context
    stack is thread-local, setting a new memory context will not affect the
    current memory context of any other thread.
    \param context               New memory context */

/*! \fn memory_context_pop
    Pop the current memory context off the context stack and set the new current
    memory context to the previous memory context (the one below on the context stack).
    If no previous memory context is available on the stack, the current memory
    context is set to 0. */

/*! \fn memory_context
    Get the current memory context, or 0 if no context is set
    \return                      Current memory context */

/*! \fn memory_context_thread_deallocate
    Cleanup and deallocate any memory used for thread-local memory context data.
    Called internally when a foundation thread is about to exit. */

#if BUILD_ENABLE_MEMORY_CONTEXT

FOUNDATION_API void              memory_context_push( uint64_t context );
FOUNDATION_API void              memory_context_pop( void );
FOUNDATION_API uint64_t          memory_context( void );
FOUNDATION_API void              memory_context_thread_deallocate( void );

#else

#define memory_context_push( context )      /*lint -save -e506 -e751 */ do { (void)sizeof( context ); } while(0) /*lint -restore -e506 -e751 */
#define memory_context_pop()                do { /* */ } while(0)
#define memory_context()                    0
#define memory_context_thread_deallocate()  do { /* */ } while(0)

#endif

/*! \fn memory_set_tracker
    Set the current memory tracker
    \param tracker               New memory tracker declaration */

#if BUILD_ENABLE_MEMORY_TRACKER

FOUNDATION_API void              memory_set_tracker( memory_tracker_t tracker );

#else

#define memory_set_tracker( tracker )       /*lint -save -e506 -e751 */ do { (void)sizeof( tracker ); } while(0) /*lint -restore -e506 -e751 */

#endif

/*! Default malloc based memory system declaration
    \return                      Default malloc based memory system declation */
FOUNDATION_API memory_system_t   memory_system_malloc( void );

/*! Default local memory tracker declaration
    \return                      Default local memory tracker declaration */
FOUNDATION_API memory_tracker_t  memory_tracker_local( void );
