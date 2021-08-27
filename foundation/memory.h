/* memory.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#pragma once

/*! \file memory.h
\brief Memory management

Memory allocation and deallocation. The actual implementation of allocation/deallocation can be
controlled with the memory system declaration given to #foundation_initialize at startup.

Memory allocation/deallocation can be tracked with memory_set_tracker if the build configuration
#BUILD_ENABLE_MEMORY_TRACKER is set to 1. If not, all memory tracker calls are statically removed
at compile time.

Memory can be tracked by context, set as a hash value (usually a static string hash) if the build
configuration #BUILD_ENABLE_MEMORY_CONTEXT is set to 1. If not, all memory context calls are
statically removed and context 0 is used everywhere. Memory contexts are stored per thread. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate memory. Memory will be aligned to the given alingment unless
set to 0, which will cause alignment to be ignored. Memory hints can be given to allow the
memory system to make better choises of how to allocate the memory.
If context is 0, the currently set memory context scope is used (see #memory_context,
#memory_context_push and #memory_context_pop)
\param context Memory context
\param size    Requested size
\param align   Requested alignment
\param hint    Hint flags
\return        Memory address to a memory block of the requested size and alignment,
               0 if out of memory */
FOUNDATION_API void*
memory_allocate(hash_t context, size_t size, unsigned int align, unsigned int hint);

/*! Reallocate a memory block. Old memory content will be preserved, and if requested size is
larger than the current size the new memory area above the old size will be uninitialized.
The currently set memory context is used for any allocation. It is safe to pass null
as the current memory block pointer.
\param p       Current memory block pointer
\param size    New requested size
\param align   New requested alignment
\param oldsize Previous size
\param hint    Hint flags
\return        Memory address to a (potentially new) memory block of the requested size
               and alignment, 0 if out of memory */
FOUNDATION_API void*
memory_reallocate(void* p, size_t size, unsigned int align, size_t oldsize, unsigned int hint);

/*! Deallocate a memory block. Safe to pass a null pointer.
\param p Memory block pointer */
FOUNDATION_API void
memory_deallocate(void* p);

/*! Query the size of a memory block
\param p Pointer to memory block
\return Size of memory block in bytes */
FOUNDATION_API size_t
memory_size(const void* p);

/*! Verify the integrity of the memory block
\param p Pointer to a memory block
\return true if integrity checks passed, false if compromised */
FOUNDATION_API bool
memory_verify(const void* p);

/*! Set a new current memory context by pushing it onto the context stack. A call
to #memory_context_pop will restore the previous memory context. Memory context
stack is thread-local, setting a new memory context will not affect the
current memory context of any other thread.
\param context New memory context */
FOUNDATION_API void
memory_context_push(hash_t context);

/*! Pop the current memory context off the context stack and set the new current
memory context to the previous memory context (the one below on the context stack).
If no previous memory context is available on the stack, the current memory
context is set to 0. */
FOUNDATION_API void
memory_context_pop(void);

/*! Get the current memory context, or 0 if no context is set
\return Current memory context */
FOUNDATION_API hash_t
memory_context(void);

/*! Cleanup and deallocate any memory used for thread-local memory context data.
Called internally when a foundation thread is about to exit. */
FOUNDATION_API void
memory_context_thread_finalize(void);

/*! Initialize per-thread resources used by memory system.
Called internally when a foundation thread is about to start. */
FOUNDATION_API void
memory_thread_initialize(void);

/*! Cleanup and deallocate any per-thread resources used by memory system.
Called internally when a foundation thread is about to exit. */
FOUNDATION_API void
memory_thread_finalize(void);

/*! Set the current memory tracker, see #memory_tracker_local for a default implementation.
This function can be called prior to library initialization, in which case the
tracker will be set once the library is initialized.
\param tracker New memory tracker declaration */
FOUNDATION_API void
memory_set_tracker(memory_tracker_t tracker);

/*! Dump all tracked allocations
\param handler Function receiving allocation data */
FOUNDATION_API void
memory_tracker_dump(memory_tracker_handler_fn handler);

/*! Get the default malloc based memory system declaration for passing to #foundation_initialize
\return Default malloc based memory system declation */
FOUNDATION_API memory_system_t
memory_system_malloc(void);

/*! Get the default local memory tracker declaration for passing to #memory_set_tracker
\return Default local memory tracker declaration */
FOUNDATION_API memory_tracker_t
memory_tracker_local(void);

/*! No memory tracker
\return No memory tracker declaration */
FOUNDATION_API memory_tracker_t
memory_tracker_none(void);

/*! Get the memory statistics since initialization
\return Memory statistics */
FOUNDATION_API memory_statistics_t
memory_statistics(void);

#if !BUILD_ENABLE_MEMORY_CONTEXT

#define memory_context_push(context) /*lint -save -e506 -e751 */ \
	do {                                                         \
		(void)sizeof(context);                                   \
	} while (0) /*lint -restore -e506 -e751 */
#define memory_context_pop() \
	do { /* */               \
	} while (0)
#define memory_context() 0
#define memory_context_thread_finalize() \
	do { /* */                           \
	} while (0)

#endif
