/* memory.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#include <foundation/foundation.h>
#include <foundation/internal.h>

#if FOUNDATION_PLATFORM_WINDOWS
#include <foundation/windows.h>
#endif
#if FOUNDATION_PLATFORM_POSIX
#include <foundation/posix.h>
#include <sys/mman.h>
#endif

#if FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
#include <malloc.h>
#endif

#if FOUNDATION_PLATFORM_WINDOWS && (FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG)
#include <malloc.h>
#endif

#if FOUNDATION_PLATFORM_APPLE
#define FOUNDATION_MIN_ALIGN 16
#elif FOUNDATION_ARCH_ARM_64 || FOUNDATION_ARCH_X86_64 || FOUNDATION_ARCH_PPC_64
#define FOUNDATION_MIN_ALIGN 16
#else
#define FOUNDATION_MIN_ALIGN 8
#endif

/*lint -e728 */
static const memory_tracker_t _memory_no_tracker;
static memory_system_t _memory_system;
static bool _memory_initialized;

typedef FOUNDATION_ALIGN(8) struct {
	atomic32_t allocations_total;
	atomic32_t allocations_current;
	atomic32_t allocated_total;
	atomic32_t allocated_current;
} memory_statistics_atomic_t;

FOUNDATION_STATIC_ASSERT(sizeof(memory_statistics_t) == sizeof(memory_statistics_atomic_t), "statistics sizes differs");

static memory_statistics_atomic_t _memory_stats;

#if BUILD_ENABLE_MEMORY_GUARD
#define MEMORY_GUARD_VALUE 0xDEADBEEF
#endif

#if BUILD_ENABLE_MEMORY_TRACKER

static memory_tracker_t _memory_tracker;
static memory_tracker_t _memory_tracker_preinit;

static void
_memory_track(void* addr, size_t size);

static void
_memory_untrack(void* addr);

#else

#define _memory_track(addr, size) \
	do {                          \
		(void)sizeof((addr));     \
		(void)sizeof((size));     \
	} while (0)
#define _memory_untrack(addr) \
	do {                      \
		(void)sizeof((addr)); \
	} while (0)

#endif

int
_memory_initialize(const memory_system_t memory) {
	int ret;
	_memory_system = memory;
	memset(&_memory_stats, 0, sizeof(_memory_stats));
	ret = _memory_system.initialize();
	if (ret == 0) {
		_memory_initialized = true;
#if BUILD_ENABLE_MEMORY_TRACKER
		if (_memory_tracker_preinit.initialize)
			memory_set_tracker(_memory_tracker_preinit);
#endif
	}
	atomic_thread_fence_release();
	return ret;
}

void
_memory_finalize(void) {
#if BUILD_ENABLE_MEMORY_TRACKER
	_memory_tracker_preinit = _memory_tracker;
	if (_memory_tracker.finalize)
		_memory_tracker.finalize();
#endif
	if (_memory_system.thread_finalize)
		_memory_system.thread_finalize();
	memory_set_tracker(_memory_no_tracker);
	_memory_system.finalize();
	_memory_initialized = false;
}

#if BUILD_ENABLE_MEMORY_GUARD

static void*
_memory_guard_initialize(void* memory, size_t size, size_t align) {
	if (align < (FOUNDATION_MIN_ALIGN * 2))
		align = FOUNDATION_MIN_ALIGN * 2;
	void* start = pointer_offset(memory, align);
	uint32_t* guard_header = pointer_offset(start, -FOUNDATION_MIN_ALIGN * 2);
	uint32_t* guard_footer = pointer_offset(start, size - (align * 2));
	for (int guard_loop = 0; guard_loop < (FOUNDATION_MIN_ALIGN * 2) / 4; ++guard_loop) {
		*(guard_header + guard_loop) = MEMORY_GUARD_VALUE;
		*(guard_footer + guard_loop) = MEMORY_GUARD_VALUE;
	}
	*guard_header = (uint32_t)align;
	*(guard_header + 1) = (uint32_t)size;
	return start;
}

static void*
_memory_guard_verify(void* memory) {
	uint32_t* guard_header = pointer_offset(memory, -FOUNDATION_MIN_ALIGN * 2);
	uint32_t align = *guard_header;
	uint32_t size = *(guard_header + 1);
	uint32_t* guard_footer = pointer_offset(memory, size - (align * 2));
	for (int guard_loop = 0; guard_loop < (FOUNDATION_MIN_ALIGN * 2) / 4; ++guard_loop) {
		if ((guard_loop > 1) && (*(guard_header + guard_loop) != MEMORY_GUARD_VALUE))
			FOUNDATION_ASSERT_MSG(*guard_header == MEMORY_GUARD_VALUE, "Memory underwrite");
		if (*(guard_footer + guard_loop) != MEMORY_GUARD_VALUE)
			FOUNDATION_ASSERT_MSG(*guard_footer == MEMORY_GUARD_VALUE, "Memory overwrite");
	}
	return pointer_offset(memory, -(int32_t)align);
}

#endif

void*
memory_allocate(hash_t context, size_t size, unsigned int align, unsigned int hint) {
	void* p = _memory_system.allocate(context ? context : memory_context(), size, align, hint);
	_memory_track(p, size);
	return p;
}

void*
memory_reallocate(void* p, size_t size, unsigned int align, size_t oldsize, unsigned int hint) {
	_memory_untrack(p);
	p = _memory_system.reallocate(p, size, align, oldsize, hint);
	_memory_track(p, size);
	return p;
}

void
memory_deallocate(void* p) {
	_memory_untrack(p);
	_memory_system.deallocate(p);
}

memory_statistics_t
memory_statistics(void) {
	memory_statistics_t stats;
	atomic_thread_fence_acquire();
	memcpy(&stats, &_memory_stats, sizeof(memory_statistics_t));
	return stats;
}

#if BUILD_ENABLE_MEMORY_CONTEXT

FOUNDATION_DECLARE_THREAD_LOCAL(memory_context_t*, memory_context, 0)

void
memory_context_push(hash_t context_id) {
	memory_context_t* context = get_thread_memory_context();
	if (!context) {
		context =
		    memory_allocate(0, sizeof(memory_context_t) + (sizeof(hash_t) * foundation_config().memory_context_depth),
		                    0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		set_thread_memory_context(context);
	}
	context->context[context->depth] = context_id;
	if (context->depth < (foundation_config().memory_context_depth - 1))
		++context->depth;
}

void
memory_context_pop(void) {
	memory_context_t* context = get_thread_memory_context();
	if (context && (context->depth > 0))
		--context->depth;
}

hash_t
memory_context(void) {
	memory_context_t* context = get_thread_memory_context();
	return (context && (context->depth > 0)) ? context->context[context->depth - 1] : 0;
}

void
memory_context_thread_finalize(void) {
	memory_context_t* context = get_thread_memory_context();
	if (context)
		memory_deallocate(context);
	set_thread_memory_context(0);
}

#else

#undef memory_context_push
#undef memory_context_pop
#undef memory_context
#undef memory_context_thread_finalize

void
memory_context_push(hash_t context_id) {
	FOUNDATION_UNUSED(context_id);
}

void
memory_context_pop(void) {
}

hash_t
memory_context(void) {
	return 0;
}

void
memory_context_thread_finalize(void) {
}

#endif

void
memory_thread_initialize(void) {
	if (_memory_system.thread_initialize)
		_memory_system.thread_initialize();
}

void
memory_thread_finalize(void) {
	if (_memory_system.thread_finalize)
		_memory_system.thread_finalize();
}

static void*
_memory_allocate_malloc_raw(size_t size, unsigned int align, unsigned int hint) {
	FOUNDATION_UNUSED(hint);
	void* memory;

#if BUILD_ENABLE_MEMORY_GUARD || FOUNDATION_PLATFORM_WINDOWS
	if (align < FOUNDATION_MIN_ALIGN)
		align = FOUNDATION_MIN_ALIGN;
#endif
#if BUILD_ENABLE_MEMORY_GUARD
	if (align < (FOUNDATION_MIN_ALIGN * 2))
		size += FOUNDATION_MIN_ALIGN * 4;
	else
		size += align * 2;
#endif
#if FOUNDATION_PLATFORM_WINDOWS
	memory = _aligned_malloc(size, align);
#else
	if (align > FOUNDATION_MIN_ALIGN) {
#if FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_ANDROID
		if (posix_memalign(&memory, align, size))
			memory = 0;
#else
		memory = aligned_alloc(align, size);
#endif
	} else {
		memory = malloc(size);
	}
#endif
#if BUILD_ENABLE_MEMORY_GUARD
	memory = _memory_guard_initialize(memory, size, align);
#endif
	if (!memory) {
		log_errorf(HASH_MEMORY, ERROR_OUT_OF_MEMORY, STRING_CONST("Unable to allocate %" PRIsize " bytes of memory"),
		           size);
	}
	return memory;
}

static void*
_memory_allocate_malloc(hash_t context, size_t size, unsigned int align, unsigned int hint) {
	void* block;
	FOUNDATION_UNUSED(context);
	block = _memory_allocate_malloc_raw(size, align, hint);
	if (block && (hint & MEMORY_ZERO_INITIALIZED))
		memset(block, 0, (size_t)size);
	return block;
}

static void
_memory_deallocate_malloc(void* p) {
	if (!p)
		return;

#if BUILD_ENABLE_MEMORY_GUARD
	p = _memory_guard_verify(p);
#endif
#if FOUNDATION_PLATFORM_WINDOWS
	_aligned_free(p);
#else
	free(p);
#endif
}

static void*
_memory_reallocate_malloc(void* p, size_t size, unsigned int align, size_t oldsize, unsigned int hint) {
#if BUILD_ENABLE_MEMORY_GUARD
	if (align < FOUNDATION_MIN_ALIGN)
		align = FOUNDATION_MIN_ALIGN;
#else
#if FOUNDATION_PLATFORM_WINDOWS
	return _aligned_realloc(p, size, align > FOUNDATION_MIN_ALIGN ? align : FOUNDATION_MIN_ALIGN);
#else
	if (align <= FOUNDATION_MIN_ALIGN)
		return realloc(p, size);
#endif
#endif
	void* memory = _memory_allocate_malloc_raw(size, align, hint);
	if (p && memory && oldsize && !(hint & MEMORY_NO_PRESERVE))
		memcpy(memory, p, (size < oldsize) ? (size_t)size : (size_t)oldsize);
	_memory_deallocate_malloc(p);

	if (!memory) {
		string_const_t errmsg = system_error_message(0);
		log_panicf(HASH_MEMORY, ERROR_OUT_OF_MEMORY,
		           STRING_CONST("Unable to reallocate memory (%" PRIsize " -> %" PRIsize " @ 0x%" PRIfixPTR "): %.*s"),
		           oldsize, size, (uintptr_t)p, STRING_FORMAT(errmsg));
	}

	return memory;
}

static int
_memory_initialize_malloc(void) {
	return 0;
}

static void
_memory_finalize_malloc(void) {
}

memory_system_t
memory_system_malloc(void) {
	memory_system_t memsystem;
	memset(&memsystem, 0, sizeof(memsystem));
	memsystem.allocate = _memory_allocate_malloc;
	memsystem.reallocate = _memory_reallocate_malloc;
	memsystem.deallocate = _memory_deallocate_malloc;
	memsystem.initialize = _memory_initialize_malloc;
	memsystem.finalize = _memory_finalize_malloc;
	return memsystem;
}

#if !BUILD_ENABLE_MEMORY_TRACKER

void
memory_set_tracker(memory_tracker_t tracker) {
	FOUNDATION_UNUSED(tracker);
}

void
memory_tracker_dump(memory_tracker_handler_fn handler) {
	FOUNDATION_UNUSED(handler);
}

#else

void
memory_set_tracker(memory_tracker_t tracker) {
	memory_tracker_t old_tracker = _memory_tracker;

	if ((old_tracker.track == tracker.track) && (old_tracker.untrack == tracker.untrack))
		return;

	_memory_tracker = _memory_no_tracker;

	if (old_tracker.abort)
		old_tracker.abort();

	if (old_tracker.finalize)
		old_tracker.finalize();

	if (_memory_initialized) {
		if (tracker.initialize)
			tracker.initialize();

		_memory_tracker = tracker;
	} else {
		_memory_tracker_preinit = tracker;
	}
}

static void
_memory_track(void* addr, size_t size) {
	if (addr && _memory_tracker.track)
		_memory_tracker.track(addr, size);
}

static void
_memory_untrack(void* addr) {
	if (addr && _memory_tracker.untrack)
		_memory_tracker.untrack(addr);
}

void
memory_tracker_dump(memory_tracker_handler_fn handler) {
	if (_memory_tracker.dump)
		_memory_tracker.dump(handler);
}

FOUNDATION_ALIGNED_STRUCT(memory_tag_t, 8) {
	atomicptr_t address;
	size_t size;
	void* trace[14];
};

typedef struct memory_tag_t memory_tag_t;

static memory_tag_t* _memory_tags;
static atomic32_t _memory_tag_next;
static bool _memory_tracker_initialized;

static int
_memory_tracker_initialize(void) {
	if (!_memory_tracker_initialized) {
		size_t size = sizeof(memory_tag_t) * foundation_config().memory_tracker_max;
		_memory_tags = memory_allocate(0, size, 8, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

#if BUILD_ENABLE_MEMORY_STATISTICS
		atomic_incr32(&_memory_stats.allocations_total, memory_order_relaxed);
		atomic_incr32(&_memory_stats.allocations_current, memory_order_relaxed);
		atomic_add32(&_memory_stats.allocated_total, (int32_t)size, memory_order_relaxed);
		atomic_add32(&_memory_stats.allocated_current, (int32_t)size, memory_order_relaxed);
		atomic_thread_fence_release();
#endif
		_memory_tracker_initialized = true;
	}

	return 0;
}

static void
_memory_tracker_cleanup(void) {
	_memory_tracker_initialized = false;
	if (_memory_tags) {
		memory_deallocate(_memory_tags);

#if BUILD_ENABLE_MEMORY_STATISTICS
		size_t size = sizeof(memory_tag_t) * foundation_config().memory_tracker_max;
		atomic_decr32(&_memory_stats.allocations_current, memory_order_relaxed);
		atomic_add32(&_memory_stats.allocated_current, -(int32_t)size, memory_order_relaxed);
		atomic_thread_fence_release();
#endif

		_memory_tags = nullptr;
	}
}

static void
_memory_tracker_finalize(void) {
#if FOUNDATION_PLATFORM_APPLE
	// Hack to allow system dispatch threads time to finalize
	// and free memory during shutdown
	thread_sleep(100);
#endif

	_memory_tracker_initialized = false;
	if (_memory_tags) {
		unsigned int it;

		for (it = 0; it < foundation_config().memory_tracker_max; ++it) {
			memory_tag_t* tag = _memory_tags + it;
			void* addr = atomic_load_ptr(&tag->address, memory_order_acquire);
			if (addr) {
				char tracebuf[512];
				string_t trace = stacktrace_resolve(tracebuf, sizeof(tracebuf), tag->trace,
				                                    sizeof(tag->trace) / sizeof(tag->trace[0]), 0);
				log_warnf(HASH_MEMORY, WARNING_MEMORY,
				          STRING_CONST("Memory leak: %" PRIsize " bytes @ 0x%" PRIfixPTR " : tag %d\n%.*s"), tag->size,
				          (uintptr_t)addr, it, STRING_FORMAT(trace));
			}
		}
	}
	_memory_tracker_cleanup();
}

static void
_memory_tracker_dump(memory_tracker_handler_fn handler) {
	if (_memory_tracker_initialized && _memory_tags) {
		unsigned int it;

		for (it = 0; it < foundation_config().memory_tracker_max; ++it) {
			memory_tag_t* tag = _memory_tags + it;
			void* addr = atomic_load_ptr(&tag->address, memory_order_acquire);
			if (addr) {
				if (handler(addr, tag->size, tag->trace, sizeof(tag->trace) / sizeof(tag->trace[0])))
					break;
			}
		}
	}
}

static void
_memory_tracker_track(void* addr, size_t size) {
	if (addr && _memory_tracker_initialized) {
		size_t limit = foundation_config().memory_tracker_max * 2;
		size_t loop = 0;
		do {
			int32_t tag = atomic_exchange_and_add32(&_memory_tag_next, 1, memory_order_acq_rel);
			while (tag >= (int32_t)foundation_config().memory_tracker_max) {
				int32_t newtag = tag % (int32_t)foundation_config().memory_tracker_max;
				if (atomic_cas32(&_memory_tag_next, newtag + 1, tag + 1, memory_order_release, memory_order_acquire))
					tag = newtag;
				else
					tag = atomic_exchange_and_add32(&_memory_tag_next, 1, memory_order_acq_rel);
			}
			if (atomic_cas_ptr(&_memory_tags[tag].address, addr, nullptr, memory_order_release, memory_order_acquire)) {
				_memory_tags[tag].size = size;
				stacktrace_capture(_memory_tags[tag].trace,
				                   sizeof(_memory_tags[tag].trace) / sizeof(_memory_tags[tag].trace[0]), 3);
				break;
			}
		} while (++loop < limit);

		// if (loop >= limit)
		//	log_warnf(HASH_MEMORY, WARNING_SUSPICIOUS,
		//	          STRING_CONST("Unable to track allocation: 0x%" PRIfixPTR " %" PRIsize "
		// bytes"), 	          (uintptr_t)addr, size);

#if BUILD_ENABLE_MEMORY_STATISTICS
		atomic_incr32(&_memory_stats.allocations_total, memory_order_relaxed);
		atomic_incr32(&_memory_stats.allocations_current, memory_order_relaxed);
		atomic_add32(&_memory_stats.allocated_total, (int32_t)size, memory_order_relaxed);
		atomic_add32(&_memory_stats.allocated_current, (int32_t)size, memory_order_relaxed);
		atomic_thread_fence_release();
#endif
	}
	// else if (addr) {
	//	log_warnf(HASH_MEMORY, WARNING_SUSPICIOUS,
	//	          STRING_CONST("Pre-init untracked allocation: 0x%" PRIfixPTR " %" PRIsize "
	// bytes"), 	          (uintptr_t)addr, size);
	//}
}

static void
_memory_tracker_untrack(void* addr) {
	int32_t tag = 0;
	size_t size = 0;
	bool found = false;
	if (addr && _memory_tracker_initialized) {
		int32_t maxtag = (int32_t)foundation_config().memory_tracker_max;
		int32_t iend = atomic_load32(&_memory_tag_next, memory_order_acquire) % maxtag;
		int32_t itag = iend ? iend - 1 : maxtag - 1;
		while (!found) {
			void* tagaddr = atomic_load_ptr(&_memory_tags[itag].address, memory_order_acquire);
			if (addr == tagaddr) {
				tag = itag;
				size = _memory_tags[itag].size;
				found = true;
			} else if (itag == iend)
				break;
			else if (itag)
				--itag;
			else
				itag = maxtag - 1;
		}
	}
	if (found) {
		atomic_store_ptr(&_memory_tags[tag].address, nullptr, memory_order_release);
#if BUILD_ENABLE_MEMORY_STATISTICS
		atomic_decr32(&_memory_stats.allocations_current, memory_order_relaxed);
		atomic_add32(&_memory_stats.allocated_current, -(int32_t)size, memory_order_relaxed);
		atomic_thread_fence_release();
#endif
	}
	// else if (addr) {
	//	log_warnf(HASH_MEMORY, WARNING_SUSPICIOUS, STRING_CONST("Pre-init untracked deallocation:
	// 0x%" PRIfixPTR), (uintptr_t)addr);
	//}
}

#endif

memory_tracker_t
memory_tracker_local(void) {
	memory_tracker_t tracker = _memory_no_tracker;
#if BUILD_ENABLE_MEMORY_TRACKER
	tracker.track = _memory_tracker_track;
	tracker.untrack = _memory_tracker_untrack;
	tracker.dump = _memory_tracker_dump;
	tracker.initialize = _memory_tracker_initialize;
	tracker.abort = _memory_tracker_cleanup;
	tracker.finalize = _memory_tracker_finalize;
#endif
	return tracker;
}

memory_tracker_t
memory_tracker_none(void) {
	return _memory_no_tracker;
}
