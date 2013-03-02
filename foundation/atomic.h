/* atomic.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file atomic.h
    Atomic operations */

#include <foundation/platform.h>
#include <foundation/types.h>


#if FOUNDATION_PLATFORM_APPLE
#  include <libkern/OSAtomic.h>
#endif

/*! Atomically add to the value of the integer and returns its old value
    \param val                  Value
    \param add                  Value to add
    \return                     Old value before addition */
static FORCEINLINE int32_t      atomic_exchange_and_add32( volatile int32_t* val, int32_t add );

/*! Atomically add to the value of the integer and returns its new value
	\param val                  Value
	\param add                  Value to add
	\return                     New value after addition */
static FORCEINLINE int32_t      atomic_add32( volatile int32_t* val, int32_t add );

/*! Atomically increases the value of the integer and returns its new value
	\param val                  Value
	\param add                  Value to add
	\return                     New value after addition */
static FORCEINLINE int32_t      atomic_incr32( volatile int32_t* val );

/*! Atomically decreases the value of the integer and returns its new value
	\param val                  Value
	\param add                  Value to add
	\return                     New value after addition */
static FORCEINLINE int32_t      atomic_decr32( volatile int32_t* val );

/*! Atomically add to the value of the integer and returns its old value
    \param val                  Value
    \param add                  Value to add
    \return                     Old value before addition */
static FORCEINLINE int64_t      atomic_exchange_and_add64( volatile int64_t* val, int64_t add );

/*! Atomically add to the value of the integer and returns its new value
    \param val                  Value
    \param add                  Value to add
	\return                     New value after addition */
static FORCEINLINE int64_t      atomic_add64( volatile int64_t* val, int64_t add );

/*! Atomically increases the value of the integer and returns its new value
    \param val                  Value
    \param add                  Value to add
	\return                     New value after addition */
static FORCEINLINE int64_t      atomic_incr64( volatile int64_t* val );

/*! Atomically decreases the value of the integer and returns its new value
    \param val                  Value
    \param add                  Value to add
	\return                     New value after addition */
static FORCEINLINE int64_t      atomic_decr64( volatile int64_t* val );

/*! Atomically compare and swap (CAS). The value in the destination location is compared to the reference
    value, and if equal the new value is stored in the destination location.
    \param dst                  Pointer to destination value
    \param val                  Value to set
    \param ref                  Reference value
    \return                     true if operation was successful (new value stored), false if not */
static FORCEINLINE bool         atomic_cas32( volatile int32_t* dst, int32_t val, int32_t ref );

/*! Atomically compare and swap (CAS). The value in the destination location is compared to the reference
    value, and if equal the new value is stored in the destination location.
    \param dst                  Pointer to destination value
    \param val                  Value to set
    \param ref                  Reference value
    \return                     true if operation was successful (new value stored), false if not */
static FORCEINLINE bool         atomic_cas64( volatile int64_t* dst, int64_t val, int64_t ref );

/*! Atomically compare and swap (CAS). The value in the destination location is compared to the reference
    value, and if equal the new value is stored in the destination location.
    \param dst                  Pointer to destination value
    \param val                  Value to set
    \param ref                  Reference value
    \return                     true if operation was successful (new value stored), false if not */
#if FOUNDATION_PLATFORM_WINDOWS && ( FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL )
#  if FOUNDATION_PLATFORM_ARCH_X86
#    define                     atomic_cas_ptr( dst, val, ref ) ( ( _InterlockedCompareExchange( (long volatile*)dst, (long)val, (long)ref  ) == (long)ref ) ? true : false )
#  else
#    define                     atomic_cas_ptr( dst, val, ref ) ( ( _InterlockedCompareExchangePointer( (void**)dst, val, ref ) == ref ) ? true : false )
#  endif
#elif FOUNDATION_PLATFORM_IOS
#  define                       atomic_cas_ptr( dst, val, ref ) OSAtomicCompareAndSwapPtr( ref, val, (void* volatile*)dst )
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
#  define                       atomic_cas_ptr( dst, val, ref ) __sync_bool_compare_and_swap( dst, ref, val )
#else
static FORCEINLINE bool         atomic_cas_ptr( void** dst, void* val, void* ref );
#endif



static FORCEINLINE int32_t atomic_exchange_and_add32( volatile int32_t* val, int32_t add )
{
#if FOUNDATION_PLATFORM_WINDOWS && ( FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL )
	return _InterlockedExchangeAdd( (volatile long*)val, add );
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_fetch_and_add( val, add );
#else
#  error Not implemented
#endif
}


static FORCEINLINE int atomic_add32( volatile int32_t* val, int32_t add )
{
#if FOUNDATION_PLATFORM_WINDOWS && ( FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL )
	int32_t old = (int32_t)_InterlockedExchangeAdd( (volatile long*)val, add );
	return ( old + add );
#elif FOUNDATION_PLATFORM_APPLE
	return OSAtomicAdd32( add, val );
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_add_and_fetch( val, add );
#else 
#  error Not implemented
#endif
}

static FORCEINLINE int atomic_incr32( volatile int32_t* val ) { return atomic_add32( val, 1 ); }
static FORCEINLINE int atomic_decr32( volatile int32_t* val ) { return atomic_add32( val, -1 ); }


static FORCEINLINE int64_t atomic_exchange_and_add64( volatile int64_t* val, int64_t add )
{
#if FOUNDATION_PLATFORM_WINDOWS && ( FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL )
#  if FOUNDATION_PLATFORM_ARCH_X86
	long long ref;
	do { ref = *val; } while( _InterlockedCompareExchange64( (volatile long long*)val, ref + add, ref ) != ref );
	return ref;
#  else //X86_64
	return _InterlockedExchangeAdd64( val, add );
#  endif
#elif FOUNDATION_PLATFORM_IOS
	int64_t ref;
	do { ref = *val; } while( !OSAtomicCompareAndSwap64( ref, ref + add, val ) );
	return ref;
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_fetch_and_add( val, add );
#else 
#  error Not implemented
#endif
}


static FORCEINLINE int64_t atomic_add64( volatile int64_t* val, int64_t add )
{
#if FOUNDATION_PLATFORM_WINDOWS && ( FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL )
#  if FOUNDATION_PLATFORM_ARCH_X86
	return atomic_exchange_and_add64( val, add ) + add;
#  else
	return _InterlockedExchangeAdd64( val, add ) + add;
#endif
#elif FOUNDATION_PLATFORM_APPLE
	return OSAtomicAdd64( add, val );
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_add_and_fetch( val, add );
#else 
#  error Not implemented
#endif
}

static FORCEINLINE int64_t atomic_incr64( volatile int64_t* val ) { return atomic_add64( val, 1LL ); }
static FORCEINLINE int64_t atomic_decr64( volatile int64_t* val ) { return atomic_add64( val, -1LL ); }


static FORCEINLINE bool atomic_cas32( volatile int32_t* dst, int32_t val, int32_t ref )
{
#if FOUNDATION_PLATFORM_WINDOWS && ( FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL )
	return ( _InterlockedCompareExchange( (volatile long*)dst, val, ref ) == ref ) ? true : false;
#elif FOUNDATION_PLATFORM_IOS
	return OSAtomicCompareAndSwap32( ref, val, dst );
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_bool_compare_and_swap( dst, ref, val );
#else 
#  error Not implemented
#endif
}


static FORCEINLINE bool atomic_cas64( volatile int64_t* dst, int64_t val, int64_t ref )
{
#if FOUNDATION_PLATFORM_WINDOWS && ( FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL )
	return ( _InterlockedCompareExchange64( dst, val, ref ) == ref ) ? true : false;
#elif FOUNDATION_PLATFORM_IOS
	return OSAtomicCompareAndSwap64( ref, val, dst );
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_bool_compare_and_swap( dst, ref, val );
#else 
#  error Not implemented
#endif
}


#if FOUNDATION_PLATFORM_WINDOWS || FOUNDATION_PLATFORM_APPLE || FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
//atomic_cas_ptr defined above
#else
static FORCEINLINE bool atomic_cas_ptr( void** dst, void* val, void* ref )
{
#  if FOUNDATION_PLATFORM_POINTER_SIZE == 8
	return atomic_cas64( (volatile int64_t*)dst, (int64_t)val, (int64_t)ref );
#  elif FOUNDATION_PLATFORM_POINTER_SIZE == 4
	return atomic_cas32( (volatile int32_t*)dst, (int32_t)val, (int32_t)ref );
#  else
#    error Unknown architecture (pointer size)
#  endif
}
#endif
