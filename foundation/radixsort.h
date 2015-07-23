/* radixsort.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file radixsort.h
\brief Radix sorter
\details Radix sorter for 32/64-bit integer and floating point values */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! \brief Allocate radix sorter
Allocate a radix sort object. All data is stored in a single continuous memory block,
including sort buckets and resulting index arrays. Deallocate the sort object with a call to
#radixsort_deallocate.
\param type Data type
\param num Number of elements to sort
\return New radix sort object */
FOUNDATION_API radixsort_t*
radixsort_allocate(radixsort_data_t type, radixsort_index_t num);

/*! \brief Deallocate radix sorter
Deallocate a radix sort object previously allocated with a call to #radixsort_allocate.
\param sort Radix sort object to deallocate */
FOUNDATION_API void
radixsort_deallocate(radixsort_t* sort);

/*! \brief Initialize radix sorter
Initialize a radix sort object. All data pointers should be set by the caller. Finalize
the sort object with a call to #radixsort_finalize.
\param sort Radix sort object
\param type Data type
\param num Number of elements to sort */
FOUNDATION_API void
radixsort_initialize(radixsort_t* sort, radixsort_data_t type, radixsort_index_t num);

/*! \brief Finalize radix sorter
Finalize a radix sort object previously initialized with a call to #radixsort_initialize.
\param sort Radix sort object to finalize */
FOUNDATION_API void
radixsort_finalize(radixsort_t* sort);

/*! \brief Radix sort
Perform radix sort. This will take advantage of temporal coherence if the input is
partially sorted and/or used in a previous sort call on this radix sort object.
\param sort Radix sort object
\param input Input data buffer of same type as radix sort object was
             initialized with
\param num Number of elements to sort, must be less or equal to maximum
           number radix sort object was initialized with
\return Sorted index array holding num indices into the input array */
FOUNDATION_API const radixsort_index_t*
radixsort_sort(radixsort_t* sort, const void* input, radixsort_index_t num);

