/* virtualarray.h  -  Foundation library  -  Public Domain  -  2020 Mattias Jansson
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

/*! \file virtualarray.h
\brief Array of integral/POD types backed by commit-on-write virtual memory

Simple array of integral/POD types. All functions are "unsafe" (not range checked) unless marked
as "_safe". Unlike the normal array in array.h a null pointer is not a valid array, as the
virtual storage must be initialized before first use.

Array storage is pre-allocated as a large range of virtual memory which will NOT be comitted
to physical memory pages. It is thus "free" in that it does not increase the comitted memory
size of the process. Memory pages will be comitted by the OS as pages are touched/written to
the first time as the array grows (and thus the granularity of which the array commit size
increases is the memory page size of the process/OS).

If the expected memory usage is low (below 2 memory pages, i.e 8KiB) it will simply be allocated
through the normal memory allocation and use already committed pages.

First array element will be 16 byte aligned. Alignment between elements is the responsibility
of the caller.

Arrays are not inherently thread safe. Synchronization in a multithreaded use case must be done
by caller. */

#include <foundation/platform.h>
#include <foundation/types.h>
#include <foundation/math.h>
#include <foundation/memory.h>

/*! Allocate an array of the given type and size
\param element_size Size of an element in the array
\param capacity Expected maximum capacity of the array
\return New array */
FOUNDATION_API virtualarray_t*
virtualarray_allocate(uint element_size, uint capacity);

/*! Initialize an array of the given type and size
\param array Array to initialize
\param element_size Size of an element in the array
\param capacity Expected maximum capacity of the array */
FOUNDATION_API void
virtualarray_initialize(virtualarray_t* array, size_t element_size, size_t capacity);

/*! Initialize an array to a copy of the given array
\param array Array to initialize
\param source Source array to copy */
FOUNDATION_API void
virtualarray_initialize_copy(virtualarray_t* array, virtualarray_t* source);

/*! Finalize an array
\param array Array to finalize */
FOUNDATION_API void
virtualarray_finalize(virtualarray_t* array);

/*! Deallocate an array
\param array Array to deallocate */
FOUNDATION_API void
virtualarray_deallocate(virtualarray_t* array);

/*! Clear the array but do not free memory
\param array Array to clear */
FOUNDATION_API void
virtualarray_clear(virtualarray_t* array);

/*! Clear the array and free memory
\param array Array to clear */
FOUNDATION_API void
virtualarray_clear_and_free(virtualarray_t* array);

/*! Reset the array by clearing and setting a new element size
\param array Array to reset
\param element_size New element size */
FOUNDATION_API void
virtualarray_reset(virtualarray_t* array, size_t element_size);

/*! Resize the array, NOT initializing the new elements
\param array Array to resize
\param count New number of elements
\return Pointer to first element in array */
FOUNDATION_API void*
virtualarray_resize(virtualarray_t* array, size_t count);

/*! Resize the array, zero initializing the new elements
\param array Array to resize
\param count New number of elements
\return Pointer to first element in array */
FOUNDATION_API void*
virtualarray_resize_zero(virtualarray_t* array, size_t count);

/*! Resize the array, initializing the new elements with the given pattern
\param array Array to resize
\param count New number of elements
\param pattern Pattern to intialize with
\return Pointer to first element in array */
FOUNDATION_API void*
virtualarray_resize_fill(virtualarray_t* array, size_t count, uint8_t pattern);

/*! Push an element to the end of the array
\param array Array
\param element Element pointer */
FOUNDATION_API void
virtualarray_push_raw(virtualarray_t* array, void* element);

/*! Get the storage array and verify type size
\param array Array
\param element_size Element size
\return Element storage pointer */
FOUNDATION_API void*
virtualarray_get_verified_type(virtualarray_t* array, size_t element_size);

/*! Push an element to the end of the array and verify type size
\param array Array
\param element Element pointer
\param element_size Element size */
static FOUNDATION_FORCEINLINE void
virtualarray_push_verified_type(virtualarray_t* array, void* element, size_t element_size) {
	FOUNDATION_ASSERT_MSG(element_size == array->element_size, "Invalid virtual array element pushed");
	virtualarray_push_raw(array, element);
}

#define virtualarray_get_as(array, type) ((type*)virtualarray_get_verified_type(array, sizeof(type)))
#define virtualarray_push(array, element) virtualarray_push_verified_type(array, &element, sizeof(element))
