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

All atomic load/store operations use relaxed memory order. To enforce acquire or release
semantics use the explicit fence functions provided.

Signal fences guarantee memory order between threads on same core or between interrupt
and signal. Thread fences guarantee memory order between multiple threads on a multicore
system */

#include <foundation/platform.h>
#include <foundation/types.h>

#if !FOUNDATION_COMPILER_MSVC
#  include <stdatomic.h>
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
atomic_load_ptr(const atomicptr_t* src);

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

#if !FOUNDATION_COMPILER_MSVC && !defined(__STDC_NO_ATOMICS__)

// C11
#if FOUNDATION_COMPILER_CLANG
// Really, atomic operations should be const-able
// C atomic_load_explicit(const volatile A* object, memory_order order);
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wcast-qual"

static FOUNDATION_FORCEINLINE int32_t
atomic_load32(const atomic32_t* src) {
	return atomic_load_explicit((atomic32_t*)src, memory_order_relaxed);
}

static FOUNDATION_FORCEINLINE int64_t
atomic_load64(const atomic64_t* src) {
	return atomic_load_explicit((atomic64_t*)src, memory_order_relaxed);
}

static FOUNDATION_FORCEINLINE void*
atomic_load_ptr(const atomicptr_t* src) {
	return atomic_load_explicit((atomicptr_t*)src, memory_order_relaxed);
}

#  pragma clang diagnostic pop
#else

static FOUNDATION_FORCEINLINE int32_t
atomic_load32(const atomic32_t* src) {
	return atomic_load_explicit(src, memory_order_relaxed);
}

static FOUNDATION_FORCEINLINE int64_t
atomic_load64(const atomic64_t* src) {
	return atomic_load_explicit(src, memory_order_relaxed);
}

static FOUNDATION_FORCEINLINE void*
atomic_load_ptr(const atomicptr_t* src) {
	return atomic_load_explicit(src, memory_order_relaxed);
}

#endif

static FOUNDATION_FORCEINLINE void
atomic_store32(atomic32_t* dst, int32_t val) {
	atomic_store_explicit(dst, val, memory_order_relaxed);
}

static FOUNDATION_FORCEINLINE void
atomic_store64(atomic64_t* dst, int64_t val) {
	atomic_store_explicit(dst, val, memory_order_relaxed);
}

static FOUNDATION_FORCEINLINE void
atomic_store_ptr(atomicptr_t* dst, void* val) {
	atomic_store_explicit(dst, val, memory_order_relaxed);
}

static FOUNDATION_FORCEINLINE int32_t
atomic_add32(atomic32_t* val, int32_t add) {
	return atomic_fetch_add_explicit(val, add, memory_order_relaxed) + add;
}

static FOUNDATION_FORCEINLINE int64_t
atomic_add64(atomic64_t* val, int64_t add) {
	return atomic_fetch_add_explicit(val, add, memory_order_relaxed) + add;
}

static FOUNDATION_FORCEINLINE int32_t
atomic_incr32(atomic32_t* val) {
	return atomic_fetch_add_explicit(val, 1, memory_order_relaxed) + 1;
}

static FOUNDATION_FORCEINLINE int64_t
atomic_incr64(atomic64_t* val) {
	return atomic_fetch_add_explicit(val, 1, memory_order_relaxed) + 1;
}

static FOUNDATION_FORCEINLINE int32_t
atomic_decr32(atomic32_t* val) {
	return atomic_fetch_add_explicit(val, -1, memory_order_relaxed) - 1;
}

static FOUNDATION_FORCEINLINE int64_t
atomic_decr64(atomic64_t* val) {
	return atomic_fetch_add_explicit(val, -1, memory_order_relaxed) - 1;
}

static FOUNDATION_FORCEINLINE int32_t
atomic_exchange_and_add32(atomic32_t* val, int32_t add) {
	return atomic_fetch_add_explicit(val, add, memory_order_relaxed);
}

static FOUNDATION_FORCEINLINE int64_t
atomic_exchange_and_add64(atomic64_t* val, int64_t add) {
	return atomic_fetch_add_explicit(val, add, memory_order_relaxed);
}

static FOUNDATION_FORCEINLINE bool
atomic_cas32(atomic32_t* dst, int32_t val, int32_t ref) {
	return atomic_compare_exchange_weak_explicit(dst, &ref, val,
	                                             memory_order_relaxed, memory_order_relaxed);
}

static FOUNDATION_FORCEINLINE bool
atomic_cas64(atomic64_t* dst, int64_t val, int64_t ref) {
	return atomic_compare_exchange_weak_explicit(dst, &ref, val,
	                                             memory_order_relaxed, memory_order_relaxed);
}

static FOUNDATION_FORCEINLINE bool
atomic_cas_ptr(atomicptr_t* dst, void* val, void* ref) {
	return atomic_compare_exchange_weak_explicit(dst, &ref, val,
	                                             memory_order_relaxed, memory_order_relaxed);
}

static FOUNDATION_FORCEINLINE void
atomic_signal_fence_acquire(void) {
	atomic_signal_fence(memory_order_acquire);
}

static FOUNDATION_FORCEINLINE void
atomic_signal_fence_release(void) {
	atomic_signal_fence(memory_order_release);
}

static FOUNDATION_FORCEINLINE void
atomic_signal_fence_sequentially_consistent(void) {
	atomic_signal_fence(memory_order_seq_cst);
}

static FOUNDATION_FORCEINLINE void
atomic_thread_fence_acquire(void) {
	atomic_thread_fence(memory_order_acquire);
}

static FOUNDATION_FORCEINLINE void
atomic_thread_fence_release(void) {
	atomic_thread_fence(memory_order_release);
}

static FOUNDATION_FORCEINLINE void
atomic_thread_fence_sequentially_consistent(void) {
	atomic_signal_fence(memory_order_seq_cst);
}

#elif FOUNDATION_COMPILER_MSVC

// Microsoft

static FOUNDATION_FORCEINLINE int32_t
atomic_load32(const atomic32_t* val) {
	return val->nonatomic;
}

static FOUNDATION_FORCEINLINE int64_t
atomic_load64(const atomic64_t* val) {
#if FOUNDATION_ARCH_X86
	int64_t result;
	__asm {
		mov esi, val;
		mov ebx, eax;
		mov ecx, edx;
		lock cmpxchg8b [esi];
		mov dword ptr result, eax;
		mov dword ptr result[4], edx;
	}
	return result;
#else
	return val->nonatomic;
#endif
}

static FOUNDATION_FORCEINLINE void*
atomic_load_ptr(const atomicptr_t* val) {
	return val->nonatomic;
}

static FOUNDATION_FORCEINLINE void
atomic_store32(atomic32_t* dst, int32_t val) {
	dst->nonatomic = val;
}

static FOUNDATION_FORCEINLINE void
atomic_store64(atomic64_t* dst, int64_t val) {
#if FOUNDATION_ARCH_X86
#  pragma warning(disable : 4731)
	__asm {
		push ebx;
		mov esi, dst;
		mov ebx, dword ptr val;
		mov ecx, dword ptr val[4];
		retry:
		cmpxchg8b [esi];
		jne retry;
		pop ebx;
	}
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
	return _InterlockedExchangeAdd((volatile long*)&val->nonatomic, add);
}

static FOUNDATION_FORCEINLINE int
atomic_add32(atomic32_t* val, int32_t add) {
	int32_t old = (int32_t)_InterlockedExchangeAdd((volatile long*)&val->nonatomic, add);
	return (old + add);
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
#if FOUNDATION_ARCH_X86
	long long ref;
	do { ref = val->nonatomic; }
	while (_InterlockedCompareExchange64((volatile long long*)&val->nonatomic, ref + add, ref) != ref);
	return ref;
#else //X86_64
	return _InterlockedExchangeAdd64(&val->nonatomic, add);
#endif
}

static FOUNDATION_FORCEINLINE int64_t
atomic_add64(atomic64_t* val, int64_t add) {
#if FOUNDATION_ARCH_X86
	return atomic_exchange_and_add64(val, add) + add;
#else
	return _InterlockedExchangeAdd64(&val->nonatomic, add) + add;
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
	return (_InterlockedCompareExchange((volatile long*)&dst->nonatomic, val,
	                                    ref) == ref) ? true : false;
}

static FOUNDATION_FORCEINLINE bool
atomic_cas64(atomic64_t* dst, int64_t val, int64_t ref) {
	return (_InterlockedCompareExchange64((volatile long long*)&dst->nonatomic, val,
	                                      ref) == ref) ? true : false;
}

static FOUNDATION_FORCEINLINE bool
atomic_cas_ptr(atomicptr_t* dst, void* val, void* ref) {
#if FOUNDATION_SIZE_POINTER == 8
	return atomic_cas64((atomic64_t*)dst, (int64_t)(uintptr_t)val, (int64_t)(uintptr_t)ref);
#else
	return atomic_cas32((atomic32_t*)dst, (int32_t)(uintptr_t)val, (int32_t)(uintptr_t)ref);
#endif
}

static FOUNDATION_FORCEINLINE void atomic_signal_fence_acquire(void) {}
static FOUNDATION_FORCEINLINE void atomic_signal_fence_release(void) {}
static FOUNDATION_FORCEINLINE void atomic_signal_fence_sequentially_consistent(void) {}

#include <intrin.h>

FOUNDATION_API void
_atomic_thread_fence_sequentially_consistent(void);

#define atomic_signal_fence_acquire() _ReadWriteBarrier()
#define atomic_signal_fence_release() _ReadWriteBarrier()
#define atomic_signal_fence_sequentially_consistent() _ReadWriteBarrier()
#define atomic_thread_fence_acquire() _ReadWriteBarrier()
#define atomic_thread_fence_release() _ReadWriteBarrier()
#define atomic_thread_fence_sequentially_consistent() _atomic_thread_fence_sequentially_consistent()

#else // __STDC_NO_ATOMICS__
#  error Atomic operations not implemented
#endif
