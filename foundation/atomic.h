/* atomic.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#pragma once

/*! \file atomic.h
\brief Atomic operations and memory fences

Atomic operations and memory fences. For an excellent source of information on memory models,
atomic instructions and memory barrier/fences, go to
http://mintomic.github.io/lock-free/memory-model/
and/or
http://en.cppreference.com/w/cpp/atomic/memory_order

Atomic operations provide a means to atomically load, store and perform basic operations to
a 32/64 bit data location. On Windows all atomic store/modify functions also provide a full
memory fence (both acquire and release).

Signal fences guarantee memory order between threads on same core or between interrupt
and signal. Thread fences guarantee memory order between multiple threads on a multicore
system */

#include <foundation/platform.h>
#include <foundation/types.h>

#if FOUNDATION_PLATFORM_APPLE
#  include <libkern/OSAtomic.h>
#endif

/*! Atomically load 32 bit value
\param src   Value
\return      Current value */
static FOUNDATION_FORCEINLINE int32_t
atomic_load32(const atomic32_t* src);

/*! Atomically load 64 bit value
\param src   Value
\return      Current value */
static FOUNDATION_FORCEINLINE int64_t
atomic_load64(const atomic64_t* src);

/*! Atomically load pointer value
\param src   Value
\return      Current value */
static FOUNDATION_FORCEINLINE void*
atomic_load_ptr(atomicptr_t* src);

/*! Atomically store 32 bit value
\param dst   Target
\param val   Value to store */
static FOUNDATION_FORCEINLINE void
atomic_store32(atomic32_t* dst, int32_t val);

/*! Atomically store 64 bit value
\param dst   Target
\param val   Value to store */
static FOUNDATION_FORCEINLINE void
atomic_store64(atomic64_t* dst, int64_t val);

/*! Atomically store pointer value
\param dst   Target
\param val   Value to store */
static FOUNDATION_FORCEINLINE void
atomic_store_ptr(atomicptr_t* dst, void* val);

/*! Atomically add to the value of the 32 bit integer and returns its new value
\param val   Value to change
\param add   Value to add
\return      New value after addition */
static FOUNDATION_FORCEINLINE int32_t
atomic_add32(atomic32_t* val, int32_t add);

/*! Atomically add to the value of the 64 bit integer and returns its new value
\param val   Value to change
\param add   Value to add
\return      New value after addition */
static FOUNDATION_FORCEINLINE int64_t
atomic_add64(atomic64_t* val, int64_t add);

/*! Atomically increases the value of the 32 bit integer and returns its new value
\param val   Value to change
\return      New value after addition */
static FOUNDATION_FORCEINLINE int32_t
atomic_incr32(atomic32_t* val);

/*! Atomically increases the value of the 64 bit integer and returns its new value
\param val   Value to change
\return      New value after addition */
static FOUNDATION_FORCEINLINE int64_t
atomic_incr64(atomic64_t* val);

/*! Atomically decreases the value of the 32 bit integer and returns its new value
\param val   Value to change
\return      New value after addition */
static FOUNDATION_FORCEINLINE int32_t
atomic_decr32(atomic32_t* val);

/*! Atomically decreases the value of the 64 bit integer and returns its new value
\param val   Value to change
\return      New value after addition */
static FOUNDATION_FORCEINLINE int64_t
atomic_decr64(atomic64_t* val);

/*! Atomically add to the value of the 32 bit integer and returns its old value
\param val   Value to change
\param add   Value to add
\return      Old value before addition */
static FOUNDATION_FORCEINLINE int32_t
atomic_exchange_and_add32(atomic32_t* val, int32_t add);

/*! Atomically add to the value of the 64 bit integer and returns its old value
\param val   Value to change
\param add   Value to add
\return      Old value before addition */
static FOUNDATION_FORCEINLINE int64_t
atomic_exchange_and_add64(atomic64_t* val, int64_t add);

/*! Atomically compare and swap (CAS). The value in the destination location is compared to
the reference value, and if equal the new value is stored in the destination location.
\param dst   Value to change
\param val   Value to set
\param ref   Reference value
\return      true if operation was successful and new value stored,
             false if comparison failed and value was unchanged */
static FOUNDATION_FORCEINLINE bool
atomic_cas32(atomic32_t* dst, int32_t val, int32_t ref);

/*! Atomically compare and swap (CAS). The value in the destination location is compared to
the reference value, and if equal the new value is stored in the destination location.
\param dst   Value to change
\param val   Value to set
\param ref   Reference value
\return      true if operation was successful and new value stored,
             false if comparison failed and value was unchanged */
static FOUNDATION_FORCEINLINE bool
atomic_cas64(atomic64_t* dst, int64_t val, int64_t ref);

/*! Atomically compare and swap (CAS). The value in the destination location is compared to
the reference value, and if equal the new value is stored in the destination location.
\param dst   Value to change
\param val   Value to set
\param ref   Reference value
\return      true if operation was successful and new value stored,
             false if comparison failed and value was unchanged */
static FOUNDATION_FORCEINLINE bool
atomic_cas_ptr(atomicptr_t* dst, void* val, void* ref);

/*! Signal fence making prior writes made to other memory locations done by a thread on
the same core doing a release fence visible to the calling thread. Implemented as a compile
barrier on all supported platforms */
static FOUNDATION_FORCEINLINE void
atomic_signal_fence_acquire(void);

/*! Signal fence to make prior writes to functions doing an acquire fence in threads on
the same core. Implemented as a compile barrier on all supported platforms */
static FOUNDATION_FORCEINLINE void
atomic_signal_fence_release(void);

/*! Signal fence combining acquire and release order as well as providing a single total
order on all sequentially consistent fences for threads on the same core. Implemented as
a compile barrier on all supported platforms */
static FOUNDATION_FORCEINLINE void
atomic_signal_fence_sequentially_consistent(void);

/*! Thread fence making prior writes made to other memory locations done by a thread doing
a release fence visible to the calling thread. */
static FOUNDATION_FORCEINLINE void
atomic_thread_fence_acquire(void);

/*! Thread fence making prior writes visible to other threads to do an acquire fence. */
static FOUNDATION_FORCEINLINE void
atomic_thread_fence_release(void);

/*! Thread fence combining an acquire and release fence as well as enforcing a single
total order on all sequentially consistent fences. */
static FOUNDATION_FORCEINLINE void
atomic_thread_fence_sequentially_consistent(void);

// Implementations

#if FOUNDATION_ARCH_MIPS || (FOUNDATION_PLATFORM_LINUX_RASPBERRYPI && FOUNDATION_COMPILER_GCC && (FOUNDATION_GCC_VERSION <= 40800))
#  define FOUNDATION_MUTEX_64BIT_ATOMIC 1
FOUNDATION_API int64_t __foundation_sync_fetch_and_add_8(int64_t* val, int64_t add);
FOUNDATION_API int64_t __foundation_sync_add_and_fetch_8(int64_t* val, int64_t add);
FOUNDATION_API bool    __foundation_sync_bool_compare_and_swap_8(int64_t* val, int64_t oldval,
    int64_t newval);
#else
#  define FOUNDATION_MUTEX_64BIT_ATOMIC 0
#endif

static FOUNDATION_FORCEINLINE int32_t
atomic_load32(const atomic32_t* val) {
	return val->nonatomic;
}

static FOUNDATION_FORCEINLINE int64_t
atomic_load64(const atomic64_t* val) {
#if FOUNDATION_ARCH_X86
	int64_t result;
#  if FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL
	__asm {
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

static FOUNDATION_FORCEINLINE void*
atomic_load_ptr(atomicptr_t* val) {
	return val->nonatomic;
}

static FOUNDATION_FORCEINLINE void
atomic_store32(atomic32_t* dst, int32_t val) {
	dst->nonatomic = val;
}

static FOUNDATION_FORCEINLINE void
atomic_store64(atomic64_t* dst, int64_t val) {
#if FOUNDATION_ARCH_X86
#  if FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL
#    if FOUNDATION_COMPILER_MSVC
#      pragma warning(disable : 4731)
	__asm {
		push ebx;
	}
#    endif
	__asm {
		mov esi, dst;
		mov ebx, dword ptr val;
		mov ecx, dword ptr val[4];
		retry:
		cmpxchg8b [esi];
		jne retry;
	}
#    if FOUNDATION_COMPILER_MSVC
	__asm {
		pop ebx;
	}
#    endif
#  else
	int64_t expected = dst->nonatomic;
#    if FOUNDATION_COMPILER_GCC
	__sync_bool_compare_and_swap(&dst->nonatomic, expected, val);
#    else
	__asm volatile(
	  "1:    cmpxchg8b %0\n"
	  "      jne 1b"
	  : "=m"(dst->nonatomic)
	  : "b"((uint32_t)val), "c"((uint32_t)(val >> 32)), "A"(expected));
#    endif
#  endif
#else
	dst->nonatomic = val;
#endif
}

static FOUNDATION_FORCEINLINE void
atomic_store_ptr(atomicptr_t* dst, void* val) {
	dst->nonatomic = val;
}

static FOUNDATION_FORCEINLINE int32_t
atomic_exchange_and_add32(atomic32_t* val, int32_t add) {
#if FOUNDATION_PLATFORM_WINDOWS && ( FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL )
	return _InterlockedExchangeAdd((volatile long*)&val->nonatomic, add);
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_fetch_and_add(&val->nonatomic, add);
#else
#  error Not implemented
#endif
}

static FOUNDATION_FORCEINLINE int
atomic_add32(atomic32_t* val, int32_t add) {
#if FOUNDATION_PLATFORM_WINDOWS && ( FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL )
	int32_t old = (int32_t)_InterlockedExchangeAdd((volatile long*)&val->nonatomic, add);
	return (old + add);
#elif FOUNDATION_PLATFORM_APPLE
	return OSAtomicAdd32(add, (int*)&val->nonatomic);
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_add_and_fetch(&val->nonatomic, add);
#else
#  error Not implemented
#endif
}

static FOUNDATION_FORCEINLINE int
atomic_incr32(atomic32_t* val) {
	return atomic_add32(val, 1);
}

static FOUNDATION_FORCEINLINE int
atomic_decr32(atomic32_t* val) {
	return atomic_add32(val, -1);
}

static FOUNDATION_FORCEINLINE int64_t
atomic_exchange_and_add64(atomic64_t* val, int64_t add) {
#if FOUNDATION_PLATFORM_WINDOWS && ( FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL )
#  if FOUNDATION_ARCH_X86
	long long ref;
	do { ref = val->nonatomic; }
	while (_InterlockedCompareExchange64((volatile long long*)&val->nonatomic, ref + add, ref) != ref);
	return ref;
#  else //X86_64
	return _InterlockedExchangeAdd64(&val->nonatomic, add);
#  endif
#elif FOUNDATION_PLATFORM_APPLE
	int64_t ref;
	do { ref = (int64_t)val->nonatomic; }
	while (!OSAtomicCompareAndSwap64(ref, ref + add, (int64_t*)&val->nonatomic));
	return ref;
#elif FOUNDATION_MUTEX_64BIT_ATOMIC
	return __foundation_sync_fetch_and_add_8(&val->nonatomic, add);
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_fetch_and_add(&val->nonatomic, add);
#else
#  error Not implemented
#endif
}

static FOUNDATION_FORCEINLINE int64_t
atomic_add64(atomic64_t* val, int64_t add) {
#if FOUNDATION_PLATFORM_WINDOWS && ( FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL )
#  if FOUNDATION_ARCH_X86
	return atomic_exchange_and_add64(val, add) + add;
#  else
	return _InterlockedExchangeAdd64(&val->nonatomic, add) + add;
#endif
#elif FOUNDATION_PLATFORM_APPLE
	return OSAtomicAdd64(add, (int64_t*)&val->nonatomic);
#elif FOUNDATION_MUTEX_64BIT_ATOMIC
	return __foundation_sync_add_and_fetch_8(&val->nonatomic, add);
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_add_and_fetch(&val->nonatomic, add);
#else
#  error Not implemented
#endif
}

static FOUNDATION_FORCEINLINE int64_t
atomic_incr64(atomic64_t* val) {
	return atomic_add64(val, 1LL);
}

static FOUNDATION_FORCEINLINE int64_t
atomic_decr64(atomic64_t* val) {
	return atomic_add64(val, -1LL);
}

static FOUNDATION_FORCEINLINE bool
atomic_cas32(atomic32_t* dst, int32_t val, int32_t ref) {
#if FOUNDATION_PLATFORM_WINDOWS && ( FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL )
	return (_InterlockedCompareExchange((volatile long*)&dst->nonatomic, val,
	                                    ref) == ref) ? true : false;
#elif FOUNDATION_PLATFORM_APPLE
	return OSAtomicCompareAndSwap32(ref, val, (int32_t*)&dst->nonatomic);
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_bool_compare_and_swap(&dst->nonatomic, ref, val);
#else
#  error Not implemented
#endif
}

static FOUNDATION_FORCEINLINE bool
atomic_cas64(atomic64_t* dst, int64_t val, int64_t ref) {
#if FOUNDATION_PLATFORM_WINDOWS && ( FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL )
	return (_InterlockedCompareExchange64((volatile long long*)&dst->nonatomic, val,
	                                      ref) == ref) ? true : false;
#elif FOUNDATION_PLATFORM_APPLE
	return OSAtomicCompareAndSwap64(ref, val, (int64_t*)&dst->nonatomic);
#elif FOUNDATION_MUTEX_64BIT_ATOMIC
	return __foundation_sync_bool_compare_and_swap_8(&dst->nonatomic, ref, val);
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __sync_bool_compare_and_swap(&dst->nonatomic, ref, val);
#else
#  error Not implemented
#endif
}

static FOUNDATION_FORCEINLINE bool
atomic_cas_ptr(atomicptr_t* dst, void* val, void* ref) {
#  if FOUNDATION_SIZE_POINTER == 8
	return atomic_cas64((atomic64_t*)dst, (int64_t)(uintptr_t)val, (int64_t)(uintptr_t)ref);
#  elif FOUNDATION_SIZE_POINTER == 4
	return atomic_cas32((atomic32_t*)dst, (int32_t)(uintptr_t)val, (int32_t)(uintptr_t)ref);
#  else
#    error Unknown architecture (pointer size)
#  endif
}

static FOUNDATION_FORCEINLINE void atomic_signal_fence_acquire(void) {}
static FOUNDATION_FORCEINLINE void atomic_signal_fence_release(void) {}
static FOUNDATION_FORCEINLINE void atomic_signal_fence_sequentially_consistent(void) {}

#if !FOUNDATION_ARCH_ARM || !FOUNDATION_ARCH_THUMB
static FOUNDATION_FORCEINLINE void atomic_thread_fence_acquire(void) {}
static FOUNDATION_FORCEINLINE void atomic_thread_fence_release(void) {}
static FOUNDATION_FORCEINLINE void atomic_thread_fence_sequentially_consistent(void) {}
#endif


#if FOUNDATION_PLATFORM_WINDOWS && FOUNDATION_COMPILER_MSVC

#include <intrin.h>

FOUNDATION_API void
_atomic_thread_fence_sequentially_consistent(void);

#define atomic_signal_fence_acquire() _ReadWriteBarrier()
#define atomic_signal_fence_release() _ReadWriteBarrier()
#define atomic_signal_fence_sequentially_consistent() _ReadWriteBarrier()
#define atomic_thread_fence_acquire() _ReadWriteBarrier()
#define atomic_thread_fence_release() _ReadWriteBarrier()
#define atomic_thread_fence_sequentially_consistent() _atomic_thread_fence_sequentially_consistent()

#else

#define atomic_signal_fence_acquire() __asm volatile("" ::: "memory")
#define atomic_signal_fence_release() __asm volatile("" ::: "memory")
#define atomic_signal_fence_sequentially_consistent() __asm volatile("" ::: "memory")

#  if FOUNDATION_ARCH_ARM5 || FOUNDATION_ARCH_ARM6

// Fences compiled as standalone functions

#  elif FOUNDATION_ARCH_ARM

#define atomic_thread_fence_acquire() __asm volatile("dmb sy" ::: "memory")
#define atomic_thread_fence_release() __asm volatile("dmb st" ::: "memory")
#define atomic_thread_fence_sequentially_consistent() __asm volatile("dmb sy" ::: "memory")

#  else

#define atomic_thread_fence_acquire() __asm volatile("" ::: "memory")
#define atomic_thread_fence_release() __asm volatile("" ::: "memory")

#    if FOUNDATION_ARCH_MIPS
#define atomic_thread_fence_sequentially_consistent() __asm volatile("sync" ::: "memory")
#    elif FOUNDATION_ARCH_X86_64
#define atomic_thread_fence_sequentially_consistent() __asm volatile("lock; orl $0, (%%rsp)" ::: "memory")
#    elif FOUNDATION_ARCH_X86
#define atomic_thread_fence_sequentially_consistent() __asm volatile("lock; orl $0, (%%esp)" ::: "memory")
#    elif FOUNDATION_PLATFORM_PNACL
#define atomic_thread_fence_sequentially_consistent() __sync_synchronize()
#    else
#error atomic_thread_fence_sequentially_consistent not implemented for architecture
#    endif

#  endif

#endif

//Old deprecated names
#define atomic_loadptr atomic_load_ptr
#define atomic_storeptr atomic_store_ptr
