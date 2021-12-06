/* atomic.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file atomic.h
\brief Atomic operations and memory fences

Atomic operations and memory fences. For an excellent source of information on memory models,
atomic instructions and memory barrier/fences, go to
http://mintomic.github.io/lock-free/memory-model/
and/or
http://en.cppreference.com/w/cpp/atomic/memory_order

Atomic operations provide a means to atomically load, store and perform basic operations to
a 32 & 64 bit data location.

Signal fences guarantee memory order between threads on same core or between interrupt
and signal. Thread fences guarantee memory order between multiple threads on a multicore
system */

#include <foundation/platform.h>
#include <foundation/types.h>

#if !FOUNDATION_COMPILER_MSVC
#include <stdatomic.h>
#endif

/*! Atomically load 32 bit value
\param src   Value
\param order The memory synchronization order for this operation. The order parameter cannot be
memory_order_release or memory_order_acq_rel
\return      Current value */
static FOUNDATION_FORCEINLINE int32_t
atomic_load32(const atomic32_t* src, memory_order order);

/*! Atomically load 64 bit value
\param src   Value
\param order The memory synchronization order for this operation. The order parameter cannot be
memory_order_release or memory_order_acq_rel
\return      Current value */
static FOUNDATION_FORCEINLINE int64_t
atomic_load64(const atomic64_t* src, memory_order order);

/*! Atomically load pointer value
\param src   Value
\param order The memory synchronization order for this operation. The order parameter cannot be
memory_order_release or memory_order_acq_rel
\return      Current value */
static FOUNDATION_FORCEINLINE void*
atomic_load_ptr(const atomicptr_t* src, memory_order order);

/*! Atomically store 32 bit value
\param dst   Target
\param order The memory synchronization order for this operation. The order argument cannot be
memory_order_acquire, memory_order_consume, or memory_order_acq_rel.
\param val   Value to store */
static FOUNDATION_FORCEINLINE void
atomic_store32(atomic32_t* dst, int32_t val, memory_order order);

/*! Atomically store 64 bit value
\param dst   Target
\param order The memory synchronization order for this operation. The order argument cannot be
memory_order_acquire, memory_order_consume, or memory_order_acq_rel.
\param val   Value to store */
static FOUNDATION_FORCEINLINE void
atomic_store64(atomic64_t* dst, int64_t val, memory_order order);

/*! Atomically store pointer value
\param dst   Target
\param order The memory synchronization order for this operation. The order argument cannot be
memory_order_acquire, memory_order_consume, or memory_order_acq_rel.
\param val   Value to store */
static FOUNDATION_FORCEINLINE void
atomic_store_ptr(atomicptr_t* dst, void* val, memory_order order);

/*! Atomically add to the value of the 32 bit integer and returns its new value
\param val   Value to change
\param add   Value to add
\param order The memory synchronization order for this operation
\return      New value after addition */
static FOUNDATION_FORCEINLINE int32_t
atomic_add32(atomic32_t* val, int32_t add, memory_order order);

/*! Atomically add to the value of the 64 bit integer and returns its new value
\param val   Value to change
\param add   Value to add
\param order The memory synchronization order for this operation
\return      New value after addition */
static FOUNDATION_FORCEINLINE int64_t
atomic_add64(atomic64_t* val, int64_t add, memory_order order);

/*! Atomically increases the value of the 32 bit integer and returns its new value
\param val   Value to change
\param order The memory synchronization order for this operation
\return      New value after increase */
static FOUNDATION_FORCEINLINE int32_t
atomic_incr32(atomic32_t* val, memory_order order);

/*! Atomically increases the value of the 64 bit integer and returns its new value
\param val   Value to change
\param order The memory synchronization order for this operation
\return      New value after increase */
static FOUNDATION_FORCEINLINE int64_t
atomic_incr64(atomic64_t* val, memory_order order);

/*! Atomically decreases the value of the 32 bit integer and returns its new value
\param val   Value to change
\param order The memory synchronization order for this operation
\return      New value after decrease */
static FOUNDATION_FORCEINLINE int32_t
atomic_decr32(atomic32_t* val, memory_order order);

/*! Atomically decreases the value of the 64 bit integer and returns its new value
\param val   Value to change
\param order The memory synchronization order for this operation
\return      New value after decrease */
static FOUNDATION_FORCEINLINE int64_t
atomic_decr64(atomic64_t* val, memory_order order);

/*! Atomically add to the value of the 32 bit integer and returns its old value
\param val   Value to change
\param add   Value to add
\param order The memory synchronization order for this operation
\return      Old value before addition */
static FOUNDATION_FORCEINLINE int32_t
atomic_exchange_and_add32(atomic32_t* val, int32_t add, memory_order order);

/*! Atomically add to the value of the 64 bit integer and returns its old value
\param val   Value to change
\param add   Value to add
\param order The memory synchronization order for this operation
\return      Old value before addition */
static FOUNDATION_FORCEINLINE int64_t
atomic_exchange_and_add64(atomic64_t* val, int64_t add, memory_order order);

/*! Atomically compare and swap (CAS). The value in the destination location is compared to
the reference value, and if equal the new value is stored in the destination location.
A weak compare-and-exchange operation might fail spuriously. That is, even when the contents
of memory referred to by expected and object are equal, it might return false.
\param dst   Value to change
\param val   Value to set
\param ref   Reference value
\param success The memory synchronization order for for the read-modify-write operation if the
comparison succeeds
\param failure The memory synchronization order for the load operation if the comparison fails. This
parameter cannot be memory_order_release or memory_order_acq_rel. You cannot specify it with a
memory synchronization order stronger than success
\return      true if operation was successful and new value stored, false if comparison failed and
value was unchanged */
static FOUNDATION_FORCEINLINE bool
atomic_cas32(atomic32_t* dst, int32_t val, int32_t ref, memory_order success, memory_order failure);

/*! Atomically compare and swap (CAS). The value in the destination location is compared to
the reference value, and if equal the new value is stored in the destination location.
A weak compare-and-exchange operation might fail spuriously. That is, even when the contents
of memory referred to by expected and object are equal, it might return false.
\param dst   Value to change
\param val   Value to set
\param ref   Reference value
\param success The memory synchronization order for for the read-modify-write operation if the
comparison succeeds
\param failure The memory synchronization order for the load operation if the comparison fails. This
parameter cannot be memory_order_release or memory_order_acq_rel. You cannot specify it with a
memory synchronization order stronger than success
\return      true if operation was successful and new value stored, false if comparison failed and
value was unchanged */
static FOUNDATION_FORCEINLINE bool
atomic_cas64(atomic64_t* dst, int64_t val, int64_t ref, memory_order success, memory_order failure);

/*! Atomically compare and swap (CAS). The value in the destination location is compared to
the reference value, and if equal the new value is stored in the destination location.
A weak compare-and-exchange operation might fail spuriously. That is, even when the contents
of memory referred to by expected and object are equal, it might return false.
\param dst   Value to change
\param val   Value to set
\param ref   Reference value
\param success The memory synchronization order for for the read-modify-write operation if the
comparison succeeds
\param failure The memory synchronization order for the load operation if the comparison fails. This
parameter cannot be memory_order_release or memory_order_acq_rel. You cannot specify it with a
memory synchronization order stronger than success
\return      true if operation was successful and new value stored, false if comparison failed and
value was unchanged */
static FOUNDATION_FORCEINLINE bool
atomic_cas_ptr(atomicptr_t* dst, void* val, void* ref, memory_order success, memory_order failure);

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
// Really, atomic load operations should be const-able
// C atomic_load_explicit(const volatile A* object, memory_order order);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-qual"

static FOUNDATION_FORCEINLINE int32_t
atomic_load32(const atomic32_t* src, memory_order order) {
	return atomic_load_explicit((atomic32_t*)src, order);
}

static FOUNDATION_FORCEINLINE int64_t
atomic_load64(const atomic64_t* src, memory_order order) {
	return atomic_load_explicit((atomic64_t*)src, order);
}

static FOUNDATION_FORCEINLINE void*
atomic_load_ptr(const atomicptr_t* src, memory_order order) {
	return atomic_load_explicit((atomicptr_t*)src, order);
}

#pragma clang diagnostic pop
#else

static FOUNDATION_FORCEINLINE int32_t
atomic_load32(const atomic32_t* src, memory_order order) {
	return atomic_load_explicit(src, order);
}

static FOUNDATION_FORCEINLINE int64_t
atomic_load64(const atomic64_t* src, memory_order order) {
	return atomic_load_explicit(src, order);
}

static FOUNDATION_FORCEINLINE void*
atomic_load_ptr(const atomicptr_t* src, memory_order order) {
	return atomic_load_explicit(src, order);
}

#endif

static FOUNDATION_FORCEINLINE void
atomic_store32(atomic32_t* dst, int32_t val, memory_order order) {
	atomic_store_explicit(dst, val, order);
}

static FOUNDATION_FORCEINLINE void
atomic_store64(atomic64_t* dst, int64_t val, memory_order order) {
	atomic_store_explicit(dst, val, order);
}

static FOUNDATION_FORCEINLINE void
atomic_store_ptr(atomicptr_t* dst, void* val, memory_order order) {
	atomic_store_explicit(dst, val, order);
}

static FOUNDATION_FORCEINLINE int32_t
atomic_add32(atomic32_t* val, int32_t add, memory_order order) {
	return atomic_fetch_add_explicit(val, add, order) + add;
}

static FOUNDATION_FORCEINLINE int64_t
atomic_add64(atomic64_t* val, int64_t add, memory_order order) {
	return atomic_fetch_add_explicit(val, add, order) + add;
}

static FOUNDATION_FORCEINLINE int32_t
atomic_incr32(atomic32_t* val, memory_order order) {
	return atomic_fetch_add_explicit(val, 1, order) + 1;
}

static FOUNDATION_FORCEINLINE int64_t
atomic_incr64(atomic64_t* val, memory_order order) {
	return atomic_fetch_add_explicit(val, 1, order) + 1;
}

static FOUNDATION_FORCEINLINE int32_t
atomic_decr32(atomic32_t* val, memory_order order) {
	return atomic_fetch_add_explicit(val, -1, order) - 1;
}

static FOUNDATION_FORCEINLINE int64_t
atomic_decr64(atomic64_t* val, memory_order order) {
	return atomic_fetch_add_explicit(val, -1, order) - 1;
}

static FOUNDATION_FORCEINLINE int32_t
atomic_exchange_and_add32(atomic32_t* val, int32_t add, memory_order order) {
	return atomic_fetch_add_explicit(val, add, order);
}

static FOUNDATION_FORCEINLINE int64_t
atomic_exchange_and_add64(atomic64_t* val, int64_t add, memory_order order) {
	return atomic_fetch_add_explicit(val, add, order);
}

static FOUNDATION_FORCEINLINE bool
atomic_cas32(atomic32_t* dst, int32_t val, int32_t ref, memory_order success, memory_order failure) {
	return atomic_compare_exchange_weak_explicit(dst, &ref, val, success, failure);
}

static FOUNDATION_FORCEINLINE bool
atomic_cas64(atomic64_t* dst, int64_t val, int64_t ref, memory_order success, memory_order failure) {
	return atomic_compare_exchange_weak_explicit(dst, &ref, val, success, failure);
}

static FOUNDATION_FORCEINLINE bool
atomic_cas_ptr(atomicptr_t* dst, void* val, void* ref, memory_order success, memory_order failure) {
	return atomic_compare_exchange_weak_explicit(dst, &ref, val, success, failure);
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
atomic_load32(const atomic32_t* val, memory_order order) {
	if (order != memory_order_relaxed)
		_ReadWriteBarrier();
	return val->nonatomic;
}

static FOUNDATION_FORCEINLINE int64_t
atomic_load64(const atomic64_t* val, memory_order order) {
	if (order != memory_order_relaxed)
		_ReadWriteBarrier();
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
atomic_load_ptr(const atomicptr_t* val, memory_order order) {
	if (order != memory_order_relaxed)
		_ReadWriteBarrier();
	return (void*)val->nonatomic;
}

static FOUNDATION_FORCEINLINE void
atomic_store32(atomic32_t* dst, int32_t val, memory_order order) {
	dst->nonatomic = val;
	if (order >= memory_order_release)
		_ReadWriteBarrier();
}

static FOUNDATION_FORCEINLINE void
atomic_store64(atomic64_t* dst, int64_t val, memory_order order) {
#if FOUNDATION_ARCH_X86
#pragma warning(disable : 4731)
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
	if (order >= memory_order_release) _ReadWriteBarrier();
}

static FOUNDATION_FORCEINLINE void
atomic_store_ptr(atomicptr_t* dst, void* val, memory_order order) {
	dst->nonatomic = val;
	if (order >= memory_order_release)
		_ReadWriteBarrier();
}

static FOUNDATION_FORCEINLINE int32_t
atomic_exchange_and_add32(atomic32_t* val, int32_t add, memory_order order) {
	FOUNDATION_UNUSED(order);
	return _InterlockedExchangeAdd((volatile long*)&val->nonatomic, add);
}

static FOUNDATION_FORCEINLINE int
atomic_add32(atomic32_t* val, int32_t add, memory_order order) {
	FOUNDATION_UNUSED(order);
	int32_t old = (int32_t)_InterlockedExchangeAdd((volatile long*)&val->nonatomic, add);
	return (old + add);
}

static FOUNDATION_FORCEINLINE int
atomic_incr32(atomic32_t* val, memory_order order) {
	FOUNDATION_UNUSED(order);
	return atomic_add32(val, 1, order);
}

static FOUNDATION_FORCEINLINE int
atomic_decr32(atomic32_t* val, memory_order order) {
	FOUNDATION_UNUSED(order);
	return atomic_add32(val, -1, order);
}

static FOUNDATION_FORCEINLINE int64_t
atomic_exchange_and_add64(atomic64_t* val, int64_t add, memory_order order) {
	FOUNDATION_UNUSED(order);
#if FOUNDATION_ARCH_X86
	long long ref;
	do {
		ref = val->nonatomic;
	} while (_InterlockedCompareExchange64((volatile long long*)&val->nonatomic, ref + add, ref) != ref);
	return ref;
#else  // X86_64
	return _InterlockedExchangeAdd64(&val->nonatomic, add);
#endif
}

static FOUNDATION_FORCEINLINE int64_t
atomic_add64(atomic64_t* val, int64_t add, memory_order order) {
	FOUNDATION_UNUSED(order);
#if FOUNDATION_ARCH_X86
	return atomic_exchange_and_add64(val, add, order) + add;
#else
	return _InterlockedExchangeAdd64(&val->nonatomic, add) + add;
#endif
}

static FOUNDATION_FORCEINLINE int64_t
atomic_incr64(atomic64_t* val, memory_order order) {
	FOUNDATION_UNUSED(order);
	return atomic_add64(val, 1LL, order);
}

static FOUNDATION_FORCEINLINE int64_t
atomic_decr64(atomic64_t* val, memory_order order) {
	FOUNDATION_UNUSED(order);
	return atomic_add64(val, -1LL, order);
}

static FOUNDATION_FORCEINLINE bool
atomic_cas32(atomic32_t* dst, int32_t val, int32_t ref, memory_order success, memory_order failure) {
	FOUNDATION_UNUSED(success);
	FOUNDATION_UNUSED(failure);
	return (_InterlockedCompareExchange((volatile long*)&dst->nonatomic, val, ref) == ref) ? true : false;
}

static FOUNDATION_FORCEINLINE bool
atomic_cas64(atomic64_t* dst, int64_t val, int64_t ref, memory_order success, memory_order failure) {
	FOUNDATION_UNUSED(success);
	FOUNDATION_UNUSED(failure);
	return (_InterlockedCompareExchange64((volatile long long*)&dst->nonatomic, val, ref) == ref) ? true : false;
}

static FOUNDATION_FORCEINLINE bool
atomic_cas_ptr(atomicptr_t* dst, void* val, void* ref, memory_order success, memory_order failure) {
	FOUNDATION_UNUSED(success);
	FOUNDATION_UNUSED(failure);
#if FOUNDATION_SIZE_POINTER == 8
	return atomic_cas64((atomic64_t*)dst, (int64_t)(uintptr_t)val, (int64_t)(uintptr_t)ref, success, failure);
#else
	return atomic_cas32((atomic32_t*)dst, (int32_t)(uintptr_t)val, (int32_t)(uintptr_t)ref, success, failure);
#endif
}

static FOUNDATION_FORCEINLINE void
atomic_signal_fence_acquire(void) {
}

static FOUNDATION_FORCEINLINE void
atomic_signal_fence_release(void) {
}

static FOUNDATION_FORCEINLINE void
atomic_signal_fence_sequentially_consistent(void) {
}

#include <intrin.h>

FOUNDATION_API void
internal_atomic_thread_fence_sequentially_consistent(void);

#define atomic_signal_fence_acquire() _ReadWriteBarrier()
#define atomic_signal_fence_release() _ReadWriteBarrier()
#define atomic_signal_fence_sequentially_consistent() _ReadWriteBarrier()
#define atomic_thread_fence_acquire() _ReadWriteBarrier()
#define atomic_thread_fence_release() _ReadWriteBarrier()
#define atomic_thread_fence_sequentially_consistent() internal_atomic_thread_fence_sequentially_consistent()

#else  // __STDC_NO_ATOMICS__
#error Atomic operations not implemented
#endif
