/* array.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file array.h
\brief Simple resizable array of integral/POD types

Simple resizable array of integral/POD types. All functions are "unsafe" (not range checked)
unless marked as "_safe". It is safe to pass null pointers to all methods as a null pointer is
considered a valid array (array storage will be allocated and assigned as needed).

Note that all public APIs are defines as macros. Some calls might evaluate argument expressions
multiple times.

Arrays are not inherently thread safe. Synchronization in a multithreaded use case must be done
by caller.

Example usage:

<code>int** arr = 0;
array_push( arr, 10 ); //arr is now allocated and arr[0] now equals 10
array_pop( arr ); //arr size is now 0, arr still allocated
array_deallocate( arr ); //arr is now deallocated and arr now equals null</code>

Adapted and extended from stb_arr at http://nothings.org/stb.h */

#include <foundation/platform.h>
#include <foundation/types.h>
#include <foundation/math.h>
#include <foundation/memory.h>

/*! Deallocate array memory and reset array pointer to zero. Value of expression
is a null pointer.
\param array Array pointer */
#define array_deallocate(array) /*lint -e{522}*/ \
	(_array_verify(array) ? (memory_deallocate(_array_raw(array)), ((array) = 0)) : 0)

/*! Get capacity of array in number of elements. Capacity indicates the size of the allocated
memory block (maximum size of array).
\param array Array pointer */
#define array_capacity(array) (_array_verify(array) ? _array_rawcapacity_const(array) : 0)

/*! Reserve storage for given number of elements. Never reduces storage and does not affect
number of currently stored elements.
\param array    Array pointer
\param capacity New capacity */
#define array_reserve(array, capacity) \
	((void)_array_maybegrowfixed(array, (int)(capacity) - (int)array_capacity(array)))

/*!  Get number of currently stored elements.
\param array Array pointer */
#define array_count(array) (_array_verify(array) ? _array_rawsize_const(array) : 0)
#define array_size(array) array_count(array)

/*! Add or remove elements without initialization, if size is positive or negative respectively.
Sets new size to array_size(array)+count and allocates new storage if new size is larger than
array capacity. Value of expression is the new size of the array.
\param array Array pointer
\param count Number of elements to grow/shrink */
#define array_grow(array, count) ((void)_array_resize(array, (size_t)((ssize_t)array_size(array) + (ssize_t)count)))

/*! Resize to given absolute size without initialization. Sets new size to count and allocate
new storage if new size is larger than array capacity.
\param array Array pointer
\param count New size */
#define array_resize(array, count) ((void)_array_resize(array, count))

/*! Set size to 0. Does not affect capacity or resize array storage buffer.
\param array Array pointer */
#define array_clear(array) (_array_verify(array) ? (_array_rawsize(array) = 0) : 0)

/*! Copy content of one array to another, setting new destination array size to source array
size and allocating more storage if new destination size is larger than destination array
capacity. Value of the expression is the size of the destination array after copy.
\param dst Destination array
\param src Source array */
#define array_copy(dst, src)                                                                                          \
	((_array_verify(src) && _array_elementsize(src) == _array_elementsize(dst) &&                                     \
	  _array_maybegrowfixed((dst), (_array_rawsize_const(src) - (_array_verify(dst) ? (_array_rawsize(dst)) : 0)))) ? \
	     (memcpy((dst), (src), (_array_rawsize_const(src)) * _array_elementsize(src)),                                \
	      (_array_rawsize(dst) = _array_rawsize_const(src))) :                                                        \
         array_clear(dst))

/*! Add element at end of array with assignment. Value of expression is new array pointer.
\param array   Array pointer
\param element New element */
#define array_push(array, element) /*lint -e522*/ \
	(_array_maybegrow(array, 1) ? (((array)[_array_rawsize(array)++] = (element)), (array)) : (array))

/*! Add element at end of array copying data with memcpy. Value of expression
is new array pointer.
\param array      Array pointer
\param elementptr Pointer to new element */
#define array_push_memcpy(array, elementptr) /*lint -e{506,522}*/                               \
	(_array_maybegrow(array, 1) ?                                                               \
	     (memcpy((array) + _array_rawsize(array)++, (elementptr), sizeof(*(array))), (array)) : \
         (array))

/*! Add element at given position in array with assignment. Position is NOT range checked.
Existing elements are moved using memmove. Value of expression is new array pointer.
\param array   Array pointer
\param pos     Position
\param element New element */
#define array_insert(array, pos, element)                                                                  \
	(_array_maybegrow(array, 1) ? (memmove((array) + (pos) + 1, (array) + (pos),                           \
	                                       _array_elementsize(array) * (_array_rawsize(array)++ - (pos))), \
	                               (array)[(pos)] = (element), (array)) :                                  \
                                  (array))

/*! Add element at given position in array, copy data using memcpy. Position is NOT range
checked. Existing elements are moved using memmove. Value of expression is new array pointer.
\param array      Array pointer
\param pos        Position
\param elementptr Pointer to new element */
#define array_insert_memcpy(array, pos, elementptr)                                                        \
	(_array_maybegrow(array, 1) ? (memmove((array) + (pos) + 1, (array) + (pos),                           \
	                                       _array_elementsize(array) * (_array_rawsize(array)++ - (pos))), \
	                               memcpy((array) + (pos), (elementptr), sizeof(*(array))), (array)) :     \
                                  (array))

/*! Add element at given position in array with assignment. Position IS range checked and
clamped to array size. Existing elements are moved using memmove.
\param array   Array pointer
\param pos     Position
\param element New element */
#define array_insert_safe(array, pos, element)                                                            \
	do {                                                                                                  \
		uint32_t _arr_size = array_size(array);                                                           \
		uint32_t _clamped_pos = ((uint32_t)(pos)&0x80000000U) ? 0 : math_min((uint32_t)(pos), _arr_size); \
		array_insert(array, _clamped_pos, element);                                                       \
	} while (0)

/*! Add element at given position in array, copy data using memcpy. Position IS range
checked and clamped to array size. Existing elements are moved using memmove.
\param array      Array pointer
\param pos        Position
\param elementptr Pointer to new element */
#define array_insert_memcpy_safe(array, pos, elementptr)                                                  \
	do {                                                                                                  \
		uint32_t _arr_size = array_size(array);                                                           \
		uint32_t _clamped_pos = ((uint32_t)(pos)&0x80000000U) ? 0 : math_min((uint32_t)(pos), _arr_size); \
		array_insert_memcpy(array, _clamped_pos, elementptr);                                             \
	} while (0)

/*! Remove last element. Array size is NOT validated, will cause undefined behaviour if
called on an empty array.
\param array Array pointer */
#define array_pop(array) (_array_verify(array) ? --_array_rawsize(array) : 0)

/*! Remove last element. Array size IS validated, safe to call on an empty array.
\param array Array pointer */
#define array_pop_safe(array) (_array_verify(array) && (_array_rawsize(array) > 0) ? --_array_rawsize(array) : 0)

/*! Erase element at given position without preserving order, swap-with-last using assignment.
Position is NOT ranged checked.
\param array Array pointer
\param pos   Position */
#define array_erase(array, pos) \
	(_array_verify(array) ? *((array) + (pos)) = *((array) + (_array_rawsize(array) - 1)), --_array_rawsize(array) : 0)

/*! Erase element at given position without preserving order, swap-with-last by copy data
using memcpy. Position is NOT ranged checked.
\param array Array pointer
\param pos   Position */
#define array_erase_memcpy(array, pos)                                                                     \
	(_array_verify(array) ?                                                                                \
	     (((uint32_t)(pos) != (_array_rawsize(array) - 1) ?                                                \
	           memcpy((array) + (pos), (array) + (_array_rawsize(array) - 1), _array_elementsize(array)) : \
               0),                                                                                         \
	      --_array_rawsize(array)) :                                                                       \
         0)

/*! Erase element at given position without preserving order, swap-with-last using assignment.
Position IS ranged checked.
\param array Array pointer
\param pos   Position */
#define array_erase_safe(array, pos) \
	(_array_verify(array) && _array_verify_index(array, pos) ? array_erase(array, pos) : 0)

/*! Erase element at given position without preserving order, swap-with-last by copy data
using memcpy. Position IS ranged checked.
\param array Array pointer
\param pos   Position */
#define array_erase_memcpy_safe(array, pos) \
	(_array_verify(array) && _array_verify_index(array, pos) ? array_erase_memcpy(array, pos) : 0)

/*! Erase element at given position and preserve order by memmove remaining elements in array.
Position is NOT ranged checked.
\param array Array pointer
\param pos   Position */
#define array_erase_ordered(array, pos)                                                               \
	(_array_verify(array) ? (memmove((array) + (pos), (array) + (pos) + 1,                            \
	                                 (_array_rawsize(array) - (pos)-1U) * _array_elementsize(array)), \
	                         --_array_rawsize(array)) :                                               \
                            0)

/*! Erase element at given position and preserve order by memmove remaining elements in array.
Position IS ranged checked.
\param array Array pointer
\param pos   Position */
#define array_erase_ordered_safe(array, pos) \
	(_array_verify(array) && _array_verify_index(array, pos) ? array_erase_ordered(array, (uint32_t)pos) : 0)

/*! Erase a range of elements at given position and preserve order by memmove remaining elements
in array. Position and number of elements are NOT ranged checked
\param array      Array pointer
\param pos        Starting position
\param count      Number of elements to erase */
#define array_erase_ordered_range(array, pos, count)                                 \
	(_array_verify(array) && (/*lint -e506 */ (count) > 0) ?                         \
	 memmove((array) + (pos), (array) + (pos) + (count),                             \
	         (_array_rawsize(array) - (pos) - (count)) * _array_elementsize(array)), \
	 (_array_rawsize(array) -= (count)) : 0)

/*! Erase a range of elements at given position and preserve order by memmove remaining elements
in array. Position and number of elements ARE ranged checked
\param array Array pointer
\param pos   Starting position
\param count Number of elements to erase */
#define array_erase_ordered_range_safe(array, pos, count)                                                       \
	do {                                                                                                        \
		uint32_t _arr_size = array_size(array);                                                                 \
		uint32_t _clamped_start = ((uint32_t)(pos)&0x80000000U) ? 0 : math_min((uint32_t)(pos), _arr_size);     \
		uint32_t _clamped_end =                                                                                 \
		    ((uint32_t)((pos) + (count)) & 0x80000000U) ? 0 : math_min((uint32_t)((pos) + (count)), _arr_size); \
		if (_clamped_end > _clamped_start)                                                                      \
			array_erase_ordered_range(array, _clamped_start, _clamped_end - _clamped_start);                    \
	} while (0)

// **** Internal implementation details below, not for direct use ****

/*! \internal Header size set to 16 bytes in order to align main array memory */
#define _array_header_size 4UL

#if BUILD_DEBUG
#define _array_verify(a) ((a) && internal_array_verifyfn((const void* const*)&(a)))
#else
#define _array_verify(a) (a)
#endif

/*! \internal Access the raw array header area */
#define _array_raw(a) ((uint32_t*)(a)-_array_header_size)
#define _array_rawcapacity(a) _array_raw(a)[0]
#define _array_rawsize(a) _array_raw(a)[1]
#define _array_rawelementsize(a) _array_raw(a)[3]
#define _array_raw_const(a) ((const uint32_t*)(a)-_array_header_size)
#define _array_rawcapacity_const(a) _array_raw_const(a)[0]
#define _array_rawsize_const(a) _array_raw_const(a)[1]
#define _array_elementsize(a) ((size_t)(pointer_diff(&(a)[1], &(a)[0])))
#define _array_needgrow(a, n) /*lint -e506 */ \
	(((n) > 0) && (!_array_verify(a) || (_array_rawsize_const(a) + (uint)(n)) > _array_rawcapacity_const(a)))
#define _array_maybegrow(a, n) (_array_needgrow(a, (n)) ? _array_grow(a, n, 2) : (a))
#define _array_maybegrowfixed(a, n) (_array_needgrow(a, (n)) ? _array_grow(a, (uint)(n), 1) : (a))
#define _array_grow(a, n, f) (internal_array_growfn((void**)&(a), (uint)(n), (f), _array_elementsize(a)))
#define _array_resize(a, n) (internal_array_resizefn((void**)&(a), (uint)(n), _array_elementsize(a)))

#define _array_verify_index(a, n) ((uint32_t)(n) < _array_rawsize(a))

/*! \internal Array memory allocation function. This will reallocate array storage with the given
parameters, resulting in a total size of(factor * previous_capacity + increment) elements,
with each element size given by itemsize.
\param arr       Pointer to array
\param count     Number of elements to increment storage with
\param factor    Factor to multiply previous capacity with
\param itemsize  Size of a single item
\return          New array pointer */
FOUNDATION_API void*
internal_array_growfn(void** arr, size_t count, size_t factor, size_t itemsize);

/*! \internal Array resize function. This will reallocate array storage if needed
\param arr      Pointer to array
\param count    Number of elements to resize to
\param itemsize Size of a single item
\return         New array pointer */
FOUNDATION_API void*
internal_array_resizefn(void** arr, size_t count, size_t itemsize);

/*! \internal Verify array integrity. Will cause an assert if array is not valid.
\param arr      Pointer to array
\return         Array if valid, null if invalid */
FOUNDATION_API const void*
internal_array_verifyfn(const void* const* arr);
