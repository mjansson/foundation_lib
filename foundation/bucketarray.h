/* bucketarray.h  -  Foundation library  -  Public Domain  -  2020 Mattias Jansson
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

/*! \file bucketarray.h
    Bucketized array for POD types */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Initialize empty array
\param array Array pointer
\param element_size Size of a single element in array
\param bucket_element_count Number of elements in a bucket (power of two) */
FOUNDATION_API void
bucketarray_initialize(bucketarray_t* array, size_t element_size, size_t bucket_element_count);

/*! Initialize copy of array
\param array Array pointer
\param source Pointer to source array to copy */
FOUNDATION_API void
bucketarray_initialize_copy(bucketarray_t* array, bucketarray_t* source);

/*! Deallocate array memory
\param array Array pointer */
FOUNDATION_API void
bucketarray_finalize(bucketarray_t* array);

/*! Reserve storage memory
\param array Array pointer
\param count Number of items to reserve storage for */
FOUNDATION_API void
bucketarray_reserve(bucketarray_t* array, size_t count);

/*! Resize array
\param array Array pointer
\param count Number of items to resize array to */
FOUNDATION_API void
bucketarray_resize(bucketarray_t* array, size_t count);

/*! Append array
\param array Destination array pointer
\param append Array to append */
FOUNDATION_API void
bucketarray_append(bucketarray_t* array, bucketarray_t* append);

/*! Resize array and fill new data
\param array Array pointer
\param count Number of items to resize array to
\param fill Fill value */
FOUNDATION_API void
bucketarray_resize_fill(bucketarray_t* array, size_t count, uint8_t fill);

/*! Clear array
\param array Array pointer */
FOUNDATION_API void
bucketarray_clear(bucketarray_t* array);

/*! Clear array and free storage
\param array Array pointer */
FOUNDATION_API void
bucketarray_clear_and_free(bucketarray_t* array);

/*! Add element at end of array
\param array Array pointer
\param element Element to add */
FOUNDATION_API void
bucketarray_push(bucketarray_t* array, void* element);

/*! Erase element by swapping with last element
\param array Array pointer
\param index Element index to erase */
FOUNDATION_API void
bucketarray_erase(bucketarray_t* array, size_t index);

/*! Get indexed element
\param array Array pointer
\param index Element index */
FOUNDATION_API void*
bucketarray_get(bucketarray_t* array, size_t index);

/*! Get indexed element
\param array Array pointer
\param index Element index */
FOUNDATION_API const void*
bucketarray_get_const(const bucketarray_t* array, size_t index);

/*! Copy bucket array to flat buffer
\param array Array pointer
\param destination Destination buffer */
FOUNDATION_API void
bucketarray_copy(bucketarray_t* array, void* destination);

#define bucketarray_get_as(type, array, index) ((type*)bucketarray_get(array, index))
