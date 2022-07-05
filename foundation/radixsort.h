/* radixsort.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file radixsort.h
\brief Radix sorter

Radix sorter for 32/64-bit integer and floating point values. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate a radix sort object. All data is stored in a single continuous memory block,
including sort buckets and resulting index arrays. Deallocate the sort object with a call to
#radixsort_deallocate.
\param type Data type
\param count Number of elements to sort
\return New radix sort object */
FOUNDATION_API radixsort_t*
radixsort_allocate(radixsort_data_t type, size_t count);

/*! Allocate a radix sort object for opaque custom data. All data is stored in a single continuous memory block,
including sort buckets and resulting index arrays. Deallocate the sort object with a call to
#radixsort_deallocate.
\param data_size Size of data type in bytes
\param count Number of elements to sort
\return New radix sort object */
FOUNDATION_API radixsort_t*
radixsort_allocate_custom(size_t data_size, size_t count);

/*! Deallocate a radix sort object previously allocated with a call to #radixsort_allocate.
\param sort Radix sort object to deallocate */
FOUNDATION_API void
radixsort_deallocate(radixsort_t* sort);

/*! Initialize a radix sort object. All data pointers should be set by the caller. Finalize
the sort object with a call to #radixsort_finalize.
\param sort Radix sort object
\param type Data type
\param count Number of elements to sort */
FOUNDATION_API void
radixsort_initialize(radixsort_t* sort, radixsort_data_t type, size_t count);

/*! Initialize a radix sort object for custom opaque data. All data pointers should be set by the caller. Finalize
the sort object with a call to #radixsort_finalize.
\param sort Radix sort object
\param data_size Size of data type in bytes
\param count Number of elements to sort */
FOUNDATION_API void
radixsort_initialize_custom(radixsort_t* sort, size_t data_size, size_t count);

/*! Finalize a radix sort object previously initialized with a call to #radixsort_initialize.
\param sort Radix sort object to finalize */
FOUNDATION_API void
radixsort_finalize(radixsort_t* sort);

/*! Perform radix sort. This will take advantage of temporal coherence if the input is
partially sorted and/or used in a previous sort call on this radix sort object.
\param sort Radix sort object
\param input Input data buffer of same type as radix sort object was
             initialized with
\param count Number of elements to sort, must be less or equal to maximum
             number radix sort object was initialized with
\return Sorted index array holding num indices into the input array, data type depending
        on sort index type (16-bit or 32-bit) */
FOUNDATION_API const void*
radixsort_sort(radixsort_t* sort, const void* input, size_t count);
