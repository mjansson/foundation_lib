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
    Simple resizable array of integral/POD types. Adapted and extended from
	stb_arr at http://nothings.org/stb.h */

#include <platform.h>
#include <types.h>


#define array_deallocate(a)               ( _array_verify(a) ? memory_deallocate( _array_raw(a) ), (a)=0, 0 : 0 )
#define array_size(a)                     ( _array_verify(a) ? _array_rawsize_const(a) : 0 )
#define array_capacity(a)                 ( _array_verify(a) ? _array_rawcapacity_const(a) : 0 )
#define array_reserve(a,n)                ( (void)_array_maybegrowfixed(a,(n)-array_capacity(a)) )

//Add element
#define array_push(a,v)                   ( (void)_array_maybegrow(a,1), (a)[ _array_rawsize(a)++ ] = (v) )
#define array_push_memcpy(a,v)            ( (void)_array_maybegrow(a,1), memcpy((a)+_array_rawsize(a)++,(v),sizeof(*(v))) )
#define array_insert(a,p,v)               ( (void)_array_maybegrow(a,1), memmove((a)+(p)+1,(a)+(p),sizeof(*(v))*(_array_rawsize(a)++-(p))), (a)[(p)] = (v) )
#define array_insert_memcpy(a,p,v)        ( (void)_array_maybegrow(a,1), memmove((a)+(p)+1,(a)+(p),sizeof(*(v))*(_array_rawsize(a)++-(p))), memcpy((a)+(p),(v),sizeof(*(v))) )

//Remove last element
#define array_pop(a)                      ( _array_verify(a) ? --_array_rawsize(a), 0 : 0 )
#define array_pop_safe(a)                 ( _array_verify(a) && _array_rawsize(a)>0 ? --_array_rawsize(a), 0 : 0 )

//Erase element without preserving order (swap-with-last)
#define array_erase(a,i)                  ( _array_verify(a) ? *((a)+i)=*((a)+(_array_rawsize(a)-1)), --_array_rawsize(a), 0 : 0 )
#define array_erase_safe(a,i)             ( _array_verify(a) && ((i)>=0) && ((i)<_array_rawsize(a)) ? array_erase(a,i) : 0 )
#define array_erase_memcpy(a,i)           ( _array_verify(a) ? memcpy((a)+i,(a)+(_array_rawsize(a)-1),sizeof(*((a)+i))), --_array_rawsize(a), 0 : 0 )
#define array_erase_memcpy_safe(a,i)      ( _array_verify(a) && ((i)>=0) && ((i)<_array_rawsize(a)) ? array_erase_memcpy(a,i) : 0 )

//Erase element and preserve order (memmove). Slower than array_erase
#define array_erase_ordered(a,i)          ( _array_verify(a) ? memmove( (a)+i, (a)+i+1, (_array_rawsize(a)-i-1)*sizeof(*(a)) ), --_array_rawsize(a), 0 : 0 )
#define array_erase_ordered_safe(a,i)     ( _array_verify(a) && ((i)>=0) && ((i)<_array_rawsize(a)) ? array_erase_ordered(a,i) : 0 )
#define array_erase_ordered_range(a,i,n)  ( _array_verify(a) ? memmove( (a)+i, (a)+i+n, (_array_rawsize(a)-i-n)*sizeof(*(a)) ), _array_rawsize(a)-=n, 0 : 0 )

//Copy content of one array to another, allocating more storage if needed
#define array_copy(dst,src)               ( _array_verify(src) ? (_array_maybegrowfixed((dst),(_array_rawsize_const(src)-(_array_verify(dst)?(_array_rawsize(dst)):0)))), memcpy( (dst), (src), (_array_rawsize_const(src))*sizeof(*(src)) ), (_array_rawsize(dst) = _array_rawsize_const(src)), 0 : array_clear(dst), 0 )

#define array_clear(a)                    ( _array_verify(a) ? _array_rawsize(a)=0, 0 : 0 )

#define array_grow(a,n)                   ( _array_growfn(&(a),(n),1,sizeof(*(a))), _array_rawsize(a)+=n, 0 )

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

#define _array_needgrow(a,n)         ( _array_verify(a)==0 || (_array_rawsize_const(a)+(n)) > _array_rawcapacity_const(a) )
#define _array_maybegrow(a,n)        ( _array_needgrow(a,(n)) ? _array_grow(a,n,2), 0 : 0 )
#define _array_maybegrowfixed(a,n)   ( _array_needgrow(a,(n)) ? _array_grow(a,n,1), 0 : 0 )
#define _array_grow(a,n,f)           ( _array_growfn(&(a),(n),(f),sizeof(*(a))) )

FOUNDATION_API void _array_growfn( void* arr, int increment, int factor, int itemsize );
FOUNDATION_API void _array_verifyfn( const void* const* arr );

