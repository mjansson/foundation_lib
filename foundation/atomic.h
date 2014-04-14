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
    Atomic operations and memory fences. For an excellent source of information on memory models, atomic
    instructions and memory barrier/fences, go to http://mintomic.github.io/lock-free/memory-model/ and/or
    http://en.cppreference.com/w/cpp/atomic/memory_order

    Atomic operations provide a means to atomically load, store and perform basic operations to a 32/64 bit data
    location. On Windows all atomic store/modify functions also provide a full memory fence (both acquire and release).

    Signal fences guarantee memory order between threads on same core or between interrupt and signal.

    Thread fences guarantee memory order between multiple threads and a multicore system */

#include <foundation/platform.h>
#include <foundation/types.h>


#if FOUNDATION_PLATFORM_APPLE
#  include <libkern/OSAtomic.h>
#endif

/*! Atomically load the value
    \param src                  Value
    \return                     Current value */
static FORCEINLINE int32_t      atomic_load32( atomic32_t* src );

/*! Atomically load the value
    \param src                  Value
    \return                     Current value */
static FORCEINLINE int64_t      atomic_load64( atomic64_t* src );

/*! Atomically load the value
    \param src                  Value
    \return                     Current value */
static FORCEINLINE void*        atomic_loadptr( atomicptr_t* src );

/*! Atomically store the value
    \param dst                  Target
    \param val                  Value to store */
static FORCEINLINE void         atomic_store32( atomic32_t* dst, int32_t val );

/*! Atomically store the value
    \param dst                  Target
    \param val                  Value to store */
static FORCEINLINE void         atomic_store64( atomic64_t* dst, int64_t val );

/*! Atomically store the value
    \param dst                  Target
    \param val                  Value to store */
static FORCEINLINE void         atomic_storeptr( atomicptr_t* dst, void* val );

/*! Atomically add to the value of the integer and returns its old value
    \param val                  Value to change
    \param add                  Value to add
    \return                     Old value before addition */
static FORCEINLINE int32_t      atomic_exchange_and_add32( atomic32_t* val, int32_t add );

/*! Atomically add to the value of the integer and returns its new value
	\param val                  Value to change
	\param add                  Value to add
	\return                     New value after addition */
static FORCEINLINE int32_t      atomic_add32( atomic32_t* val, int32_t add );

/*! Atomically increases the value of the integer and returns its new value
	\param val                  Value to change
	\return                     New value after addition */
static FORCEINLINE int32_t      atomic_incr32( atomic32_t* val );

/*! Atomically decreases the value of the integer and returns its new value
	\param val                  Value to change
	\return                     New value after addition */
static FORCEINLINE int32_t      atomic_decr32( atomic32_t* val );

/*! Atomically add to the value of the integer and returns its old value
    \param val                  Value to change
    \param add                  Value to add
    \return                     Old value before addition */
static FORCEINLINE int64_t      atomic_exchange_and_add64( atomic64_t* val, int64_t add );

/*! Atomically add to the value of the integer and returns its new value
    \param val                  Value to change
    \param add                  Value to add
	\return                     New value after addition */
static FORCEINLINE int64_t      atomic_add64( atomic64_t* val, int64_t add );

/*! Atomically increases the value of the integer and returns its new value
    \param val                  Value to change
	\return                     New value after addition */
static FORCEINLINE int64_t      atomic_incr64( atomic64_t* val );

/*! Atomically decreases the value of the integer and returns its new value
    \param val                  Value to change
	\return                     New value after addition */
static FORCEINLINE int64_t      atomic_decr64( atomic64_t* val );

/*! Atomically compare and swap (CAS). The value in the destination location is compared to the reference
    value, and if equal the new value is stored in the destination location.
    \param dst                  Value to change
    \param val                  Value to set
    \param ref                  Reference value
    \return                     true if operation was successful (new value stored), false if not */
static FORCEINLINE bool         atomic_cas32( atomic32_t* dst, int32_t val, int32_t ref );

/*! Atomically compare and swap (CAS). The value in the destination location is compared to the reference
    value, and if equal the new value is stored in the destination location.
    \param dst                  Value to change
    \param val                  Value to set
    \param ref                  Reference value
    \return                     true if operation was successful (new value stored), false if not */
static FORCEINLINE bool         atomic_cas64( atomic64_t* dst, int64_t val, int64_t ref );

/*! Atomically compare and swap (CAS). The value in the destination location is compared to the reference
    value, and if equal the new value is stored in the destination location.
    \param dst                  Value to change
    \param val                  Value to set
    \param ref                  Reference value
    \return                     true if operation was successful (new value stored), false if not */
static FORCEINLINE bool         atomic_cas_ptr( atomicptr_t* dst, void* val, void* ref );

/*! Signal fence to make prior writes to other memory locations visible. Implemented as a compile
    barrier on all supported platforms */
static FORCEINLINE void         atomic_signal_fence_acquire( void );

/*! Signal fence to make prior writes to other memory locations visible to functions doing a
    acquire fence. Implemented as a compile barrier on all supported platforms */
static FORCEINLINE void         atomic_signal_fence_release( void );

/*! Signal fence combining acquire and release order as well as providing a single total order
    on all sequentially consistent fences. Implemented as a compile barrier on all supported platforms */
static FORCEINLINE void         atomic_signal_fence_sequentially_consistent( void );

/*! Thread fence making prior writes made to other memory locations done by a thread doing a release
    fance visible to the calling thread. */
static FORCEINLINE void         atomic_thread_fence_acquire( void );

/*! Thread fence making prior writes visible to other thread to do an acquire fence. */
static FORCEINLINE void         atomic_thread_fence_release( void );

/*! Thread fence combining an acquire and release fence as well as enforcing a single total order
on all sequentially consistent fences. */
static FORCEINLINE void         atomic_thread_fence_sequentially_consistent( void );


// Implementations

static FORCEINLINE int32_t atomic_load32( atomic32_t* val )
{
	return val->nonatomic;
}	


static FORCEINLINE int64_t atomic_load64( atomic64_t* val )
{
#if FOUNDATION_ARCH_X86
	uint64_t result;
#  if FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL
    __asm
	{
		mov esi, val;
		mov ebx, eax;
		mov ecx, edx;
		lock cmpxchg8b [esi];
		mov dword ptr result, eax;
		mov dword ptr result[4], edx;
	}
#  else
	__asm volatile(
		"movl %%ebx, %%eax\n"
		"movl %%ecx, %%edx\n"
		"lock; cmpxchg8b %1"
		: "=&A"(result)
		: "m"(val->nonatomic));
#  endif
	return result;
#else
	return val->nonatomic;
#endif
}


static FORCEINLINE void* atomic_loadptr( atomicptr_t* val )
{
	return val->nonatomic;
}


static FORCEINLINE void atomic_store32( atomic32_t* dst, int32_t val )
{
	dst->nonatomic = val;
}


static FORCEINLINE void atomic_store64( atomic64_t* dst, int64_t val )
{
#if FOUNDATION_ARCH_X86
#  if FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL
	__asm
	{
		mov esi, dst;
		mov ebx, dword ptr val;
		mov ecx, dword ptr val[4];
	retry:
		cmpxchg8b [esi];
		jne retry;
	}
#  else
	uint64_t expected = dst->nonatomic;
	__asm volatile(
		"1:    cmpxchg8b %0\n"
		"      jne 1b"
		: "=m"(dst->nonatomic)
		: "b"((uint32_t)val), "c"((uint32_t)(val >> 32)), "A"(expected));
#  endif
#else
	dst->nonatomic = val;
#endif
}


static FORCEINLINE void atomic_storeptr( atomicptr_t* dst, void* val )
{
	dst->nonatomic = val;
}


static FORCEINLINE int32_t atomic_exchange_and_add32( atomic32_t* val, int32_t add )
{
#if FOUNDATION_PLATFORM_WINDOWS
	return _InterlockedExchangeAdd( (volatile long*)&val->nonatomic, add );
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_fetch_and_add( &val->nonatomic, add );
#else
#  error Not implemented
#endif
}


static FORCEINLINE int atomic_add32( atomic32_t* val, int32_t add )
{
#if FOUNDATION_PLATFORM_WINDOWS
	int32_t old = (int32_t)_InterlockedExchangeAdd( (volatile long*)&val->nonatomic, add );
	return ( old + add );
#elif FOUNDATION_PLATFORM_APPLE
	return OSAtomicAdd32( add, (int*)&val->nonatomic );
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_add_and_fetch( &val->nonatomic, add );
#else 
#  error Not implemented
#endif
}

static FORCEINLINE int atomic_incr32( atomic32_t* val ) { return atomic_add32( val, 1 ); }
static FORCEINLINE int atomic_decr32( atomic32_t* val ) { return atomic_add32( val, -1 ); }


static FORCEINLINE int64_t atomic_exchange_and_add64( atomic64_t* val, int64_t add )
{
#if FOUNDATION_PLATFORM_WINDOWS
#  if FOUNDATION_ARCH_X86
	long long ref;
	do { ref = val->nonatomic; } while( _InterlockedCompareExchange64( (volatile long long*)&val->nonatomic, ref + add, ref ) != ref );
	return ref;
#  else //X86_64
	return _InterlockedExchangeAdd64( &val->nonatomic, add );
#  endif
#elif FOUNDATION_PLATFORM_APPLE
	int64_t ref;
	do { ref = (int64_t)val->nonatomic; } while( !OSAtomicCompareAndSwap64( ref, ref + add, (int64_t*)&val->nonatomic ) );
	return ref;
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_fetch_and_add( &val->nonatomic, add );
#else 
#  error Not implemented
#endif
}


static FORCEINLINE int64_t atomic_add64( atomic64_t* val, int64_t add )
{
#if FOUNDATION_PLATFORM_WINDOWS
#  if FOUNDATION_ARCH_X86
	return atomic_exchange_and_add64( val, add ) + add;
#  else
	return _InterlockedExchangeAdd64( &val->nonatomic, add ) + add;
#endif
#elif FOUNDATION_PLATFORM_APPLE
	return OSAtomicAdd64( add, (int64_t*)&val->nonatomic );
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_add_and_fetch( &val->nonatomic, add );
#else 
#  error Not implemented
#endif
}

static FORCEINLINE int64_t atomic_incr64( atomic64_t* val ) { return atomic_add64( val, 1LL ); }
static FORCEINLINE int64_t atomic_decr64( atomic64_t* val ) { return atomic_add64( val, -1LL ); }


static FORCEINLINE bool atomic_cas32( atomic32_t* dst, int32_t val, int32_t ref )
{
#if FOUNDATION_PLATFORM_WINDOWS
	return ( _InterlockedCompareExchange( (volatile long*)&dst->nonatomic, val, ref ) == ref ) ? true : false;
#elif FOUNDATION_PLATFORM_APPLE
	return OSAtomicCompareAndSwap32( ref, val, (int32_t*)&dst->nonatomic );
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_bool_compare_and_swap( &dst->nonatomic, ref, val );
#else 
#  error Not implemented
#endif
}


static FORCEINLINE bool atomic_cas64( atomic64_t* dst, int64_t val, int64_t ref )
{
#if FOUNDATION_PLATFORM_WINDOWS
	return ( _InterlockedCompareExchange64( (volatile long long*)&dst->nonatomic, val, ref ) == ref ) ? true : false;
#elif FOUNDATION_PLATFORM_APPLE
	return OSAtomicCompareAndSwap64( ref, val, (int64_t*)&dst->nonatomic );
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_bool_compare_and_swap( &dst->nonatomic, ref, val );
#else 
#  error Not implemented
#endif
}


static FORCEINLINE bool atomic_cas_ptr( atomicptr_t* dst, void* val, void* ref )
{
#  if FOUNDATION_ARCH_POINTER_SIZE == 8
	return atomic_cas64( (atomic64_t*)dst, (int64_t)(uintptr_t)val, (int64_t)(uintptr_t)ref );
#  elif FOUNDATION_ARCH_POINTER_SIZE == 4
	return atomic_cas32( (atomic32_t*)dst, (int32_t)(uintptr_t)val, (int32_t)(uintptr_t)ref );
#  else
#    error Unknown architecture (pointer size)
#  endif
}


static FORCEINLINE void atomic_signal_fence_acquire( void ) {}
static FORCEINLINE void atomic_signal_fence_release( void ) {}
static FORCEINLINE void atomic_signal_fence_sequentially_consistent( void ) {}

#if !FOUNDATION_ARCH_ARM || !FOUNDATION_ARCH_THUMB
static FORCEINLINE void atomic_thread_fence_acquire( void ) {}
static FORCEINLINE void atomic_thread_fence_release( void ) {}
static FORCEINLINE void atomic_thread_fence_sequentially_consistent( void ) {}
#endif


#if FOUNDATION_PLATFORM_WINDOWS

#define atomic_signal_fence_acquire() _ReadWriteBarrier()
#define atomic_signal_fence_release() _ReadWriteBarrier()
#define atomic_signal_fence_sequentially_consistent() _ReadWriteBarrier()
#define atomic_thread_fence_acquire() _ReadWriteBarrier()
#define atomic_thread_fence_release() _ReadWriteBarrier()
#define atomic_thread_fence_sequentially_consistent() MemoryBarrier()

#else

#define atomic_signal_fence_acquire() __asm volatile("" ::: "memory")
#define atomic_signal_fence_release() __asm volatile("" ::: "memory")
#define atomic_signal_fence_sequentially_consistent() __asm volatile("" ::: "memory")

#  if FOUNDATION_ARCH_ARM && FOUNDATION_ARCH_THUMB

// TODO: Fences compiled as standalone functions (no mcr in thumb mode)

#  elif FOUNDATION_ARCH_ARM || FOUNDATION_ARCH_ARM_64

#define atomic_thread_fence_acquire() __asm volatile("mcr p15, 0, %0, c7, c10, 5" :: "r"(0) : "memory")
#define atomic_thread_fence_release() __asm volatile("mcr p15, 0, %0, c7, c10, 5" :: "r"(0) : "memory")
#define atomic_thread_fence_sequentially_consistent() __asm volatile("mcr p15, 0, %0, c7, c10, 5" :: "r"(0) : "memory")

#  else

#define atomic_thread_fence_acquire() __asm volatile("" ::: "memory")
#define atomic_thread_fence_release() __asm volatile("" ::: "memory")
#    if FOUNDATION_ARCH_X86
#define atomic_thread_fence_sequentially_consistent() __asm volatile("lock; orl $0, (%%rsp)" ::: "memory")
#    else
#define atomic_thread_fence_sequentially_consistent() __asm volatile("lock; orl $0, (%%esp)" ::: "memory")
#    endif

#  endif

#endif
