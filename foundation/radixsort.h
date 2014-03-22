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
    Radix sorter for 32/64-bit integer and floating point values */

#include <foundation/platform.h>
#include <foundation/types.h>


/*! Allocate a radix sort object. All data is stored in a single continuous memory block,
    including sort buckets and resulting index arrays.
    \param type                           Data type
	\param num                            Number of elements to sort
	\return                               New radix sort object */
FOUNDATION_API radixsort_t*               radixsort_allocate( radixsort_data_t type, radixsort_index_t num );

/*! Deallocate a radix sort object
	\param sort                           Radix sort object to deallocate */
FOUNDATION_API void                       radixsort_deallocate( radixsort_t* sort );

/*! Perform radix sort. This will take advantage of temporal coherence if the input is partially sorted
    and/or used in a previous sort call on this radix sort object.
    \param sort                           Radix sort object
	\param input                          Input data buffer of same type as radix sort object was initialized with
	\param num                            Number of elements to sort, must be less or equal to maximum number radix sort object was initialized with
	\return                               Sorted index array holding num indices into the input array */
FOUNDATION_API const radixsort_index_t*   radixsort( radixsort_t* sort, const void* input, radixsort_index_t num );

