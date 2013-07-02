/* array.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file array.h
    Simple resizable array of integral/POD types. All functions are "unsafe" (not range checked)
	unless marked as "_safe". Safe to pass null pointers to all methods, array storage will be allocated and
    assigned as needed. Adapted and extended from stb_arr at http://nothings.org/stb.h */

#include <foundation/platform.h>
#include <foundation/types.h>


//! Free array memory and reset pointer to zero.
#define array_deallocate( array )                           ( _array_verify( array ) ? memory_deallocate( _array_raw( array ) ), (array)=0, 0 : 0 )

//! Get capacity of buffer in number of elements.
#define array_capacity( array )                             ( _array_verify( array ) ? _array_rawcapacity_const( array ) : 0 )

//! Reserve storage for given number of elements (never reduces storage and does not affect number of currently stored elements).
#define array_reserve( array, capacity )                    ( (void)_array_maybegrowfixed( array, (capacity) - array_capacity( array ) ) )

//! Get number of currently stored elements. Safe to pass null pointer.
#define array_size( array )                                 ( _array_verify( array ) ? _array_rawsize_const( array ) : 0 )

//! Add/remove elements without initialization (set new size to size+num and allocate new storage if needed).
#define array_grow( array, num )                            ( ( (num) > 0 ? array_reserve( array, array_size( array ) + (num) ), 0 : 0 ), ( _array_verify( array ) ? _array_rawsize( array ) += (num) : 0 ), 0 )

//! Set size to 0 (does not affect capacity or resize storage buffer).
#define array_clear( array )                                ( _array_verify( array ) ? _array_rawsize( array ) = 0, 0 : 0 )

//! Copy content of one array to another, allocating more storage if needed.
#define array_copy( dst, src )                              ( _array_verify( src ) && ( sizeof( *(src) ) == sizeof( *(dst) ) ) ? ( _array_maybegrowfixed( (dst), ( _array_rawsize_const( src )- ( _array_verify( dst ) ? ( _array_rawsize( dst ) ) : 0 ) ) ) ), memcpy( (dst), (src), ( _array_rawsize_const( src ) ) * sizeof( *(src) ) ), ( _array_rawsize( dst ) = _array_rawsize_const( src ) ), 0 : array_clear( dst ), 0 )

//! Add element at end of array
#define array_push( array, element )                        ( (void)_array_maybegrow( array, 1 ), (array)[ _array_rawsize( array )++ ] = (element) )

//! Add element at end of array, copy data using memcpy
#define array_push_memcpy( array, elementptr )              ( (void)_array_maybegrow( array, 1 ), memcpy( (array) + _array_rawsize( array )++, (elementptr), sizeof( *(elementptr) ) ) )

//! Add element at given position in array. Position is NOT range checked. Existing elements are moved using memmove.
#define array_insert( array, pos, element )                 ( (void)_array_maybegrow( array, 1 ), memmove( (array) + (pos) + 1, (array) + (pos), sizeof( *(array) ) * ( _array_rawsize( array )++ - (pos) ) ), (array)[(pos)] = (element) )

//! Add element at given position in array, copy data using memcpy. Position is NOT range checked. Existing elements are moved using memmove.
#define array_insert_memcpy( array, pos, elementptr )       ( (void)_array_maybegrow( array, 1 ), memmove( (array) + (pos) + 1, (array) + (pos), sizeof( *(array) ) * ( _array_rawsize( array )++ - (pos) ) ), memcpy( (array) + (pos), (elementptr), sizeof( *array ) ) )

#define array_insert_safe( array, pos, element )            do { int _clamped_pos = math_clamp( (pos), 0, array_size( array ) ); array_insert( array, _clamped_pos, element ); } while(0)
#define array_insert_memcpy_safe( array, pos, elementptr )  do { int _clamped_pos = math_clamp( (pos), 0, array_size( array ) ); array_insert_memcpy( array, _clamped_pos, elementptr ); } while(0)

//Remove last element
#define array_pop( array )                                  ( _array_verify( array ) ? --_array_rawsize( array ), 0 : 0 )
#define array_pop_safe( array )                             ( _array_verify( array ) && _array_rawsize( array ) > 0 ? --_array_rawsize( array ), 0 : 0 )

//Erase element without preserving order (swap-with-last), safe meaning range check before erase
#define array_erase( array, pos )                           ( _array_verify( array ) ? *((array) + (pos)) = *((array) + ( _array_rawsize( array ) - 1 )), --_array_rawsize( array ), 0 : 0 )
#define array_erase_safe( array, pos )                      ( _array_verify( array ) && ( (pos) >=0 ) && ( (pos) < _array_rawsize( array )) ? array_erase( array, pos ) : 0 )
#define array_erase_memcpy( array, pos )                    ( _array_verify( array ) ? memcpy( (array) + (pos), (array) + ( _array_rawsize( array ) - 1 ), sizeof( *(array) ) ), --_array_rawsize( array ), 0 : 0 )
#define array_erase_memcpy_safe( array, pos )               ( _array_verify( array ) && ( (pos) >= 0 ) && ( (pos) < _array_rawsize( array ) ) ? array_erase_memcpy( array, pos ) : 0 )

//Erase element and preserve order (memmove), safe meaning range check before erase. Slower than array_erase
#define array_erase_ordered( array, pos )                   ( _array_verify( array ) ? memmove( (array) + (pos), (array) + (pos) + 1, ( _array_rawsize( array ) - (pos) - 1 ) * sizeof( *(array) ) ), --_array_rawsize( array ), 0 : 0 )
#define array_erase_ordered_safe( array, pos )              ( _array_verify( array ) && ( (pos) >= 0 ) && ( (pos) < _array_rawsize( array ) ) ? array_erase_ordered( array, pos ) : 0 )
#define array_erase_ordered_range( array, pos, num )        ( _array_verify( array ) && (num) ? memmove( (array) + (pos), (array) + (pos) + (num), ( _array_rawsize( array ) - (pos) - (num) ) * sizeof( *(array) ) ), _array_rawsize( array ) -= (num), 0 : 0 )
#define array_erase_ordered_range_safe( array, pos, num )   do { int _clamped_start = math_clamp( (pos), 0, array_size( array ) ); int _clamped_end = math_clamp( ( (pos) + (num) ), 0, array_size( array ) ); if( _clamped_end > _clamped_start ) array_erase_ordered_range( array, _clamped_start, _clamped_end - _clamped_start ); } while(0)


// **** Internal implementation details below, not for direct use **** 

//Header size set to 16 bytes in order to align main array memory
#define _array_header_size           4UL
#if BUILD_DEBUG
#  define _array_verify(a)           ( _array_verifyfn((const void* const*)&(a)), (a) )
#else
#  define _array_verify(a)           (a)
#endif
#define _array_raw(a)                ( (int32_t*)(a)-_array_header_size )
#define _array_rawcapacity(a)        _array_raw(a)[0]
#define _array_rawsize(a)            _array_raw(a)[1]
#define _array_raw_const(a)          ( (const int32_t*)(a)-_array_header_size )
#define _array_rawcapacity_const(a)  _array_raw_const(a)[0]
#define _array_rawsize_const(a)      _array_raw_const(a)[1]

#define _array_needgrow(a,n)         ( ((n)>0) && ( _array_verify(a)==0 || (_array_rawsize_const(a)+(n)) > _array_rawcapacity_const(a) ) )
#define _array_maybegrow(a,n)        ( _array_needgrow(a,(n)) ? _array_grow(a,n,2), 0 : 0 )
#define _array_maybegrowfixed(a,n)   ( _array_needgrow(a,(n)) ? _array_grow(a,n,1), 0 : 0 )
#define _array_grow(a,n,f)           ( _array_growfn(&(a),(n),(f),sizeof(*(a))) )

FOUNDATION_API void _array_growfn( void* arr, int increment, int factor, int itemsize );
FOUNDATION_API void _array_verifyfn( const void* const* arr );

