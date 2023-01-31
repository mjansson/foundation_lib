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
extern size_t
malloc_size(const void* ptr);
#define FOUNDATION_MIN_ALIGN 16
#elif FOUNDATION_ARCH_ARM_64 || FOUNDATION_ARCH_X86_64 || FOUNDATION_ARCH_PPC_64
#define FOUNDATION_MIN_ALIGN 16
#else
#define FOUNDATION_MIN_ALIGN 8
#endif

/*lint -e728 */
static const memory_tracker_t memory_no_tracker;
static memory_system_t memory_system_current;
static bool memory_initialized;

typedef FOUNDATION_ALIGN(8) struct {
	atomic64_t allocations_total;
	atomic64_t allocations_current;
	atomic64_t allocated_total;
	atomic64_t allocated_current;
} memory_statistics_atomic_t;

FOUNDATION_STATIC_ASSERT(sizeof(memory_statistics_t) == sizeof(memory_statistics_atomic_t), "statistics sizes differs");

static memory_statistics_atomic_t memory_stats;

#if BUILD_ENABLE_MEMORY_GUARD
#define MEMORY_GUARD_VALUE 0xDEADBEEF
#endif

#if BUILD_ENABLE_MEMORY_TRACKER

static memory_tracker_t memory_tracker_current;
static memory_tracker_t memory_tracker_preinit;

static void
memory_track(void* addr, size_t size);

static void
memory_untrack(void* addr);

#else

#define memory_track(addr, size) \
	do {                         \
		(void)sizeof((addr));    \
		(void)sizeof((size));    \
	} while (0)
#define memory_untrack(addr)  \
	do {                      \
		(void)sizeof((addr)); \
	} while (0)

#endif

int
internal_memory_initialize(const memory_system_t memory) {
	int ret;
	memory_system_current = memory;
	memset(&memory_stats, 0, sizeof(memory_stats));
	ret = memory_system_current.initialize();
	if (ret == 0) {
		memory_initialized = true;
#if BUILD_ENABLE_MEMORY_TRACKER
		if (memory_tracker_preinit.initialize)
			memory_set_tracker(memory_tracker_preinit);
#endif
	}
	atomic_thread_fence_release();
	return ret;
}

void
internal_memory_finalize(void) {
#if BUILD_ENABLE_MEMORY_TRACKER
	memory_tracker_preinit = memory_tracker_current;
	if (memory_tracker_current.finalize)
		memory_tracker_current.finalize();
#endif
	if (memory_system_current.thread_finalize)
		memory_system_current.thread_finalize();
	memory_set_tracker(memory_no_tracker);
	memory_system_current.finalize();
	memory_initialized = false;
}

#if BUILD_ENABLE_MEMORY_GUARD

static void*
memory_guard_initialize(void* memory, size_t size, size_t align) {
	if (align < (FOUNDATION_MIN_ALIGN * 2))
		align = FOUNDATION_MIN_ALIGN * 2;
	void* start = pointer_offset(memory, align);
	uint32_t* guard_header = pointer_offset(start, -FOUNDATION_MIN_ALIGN * 2);
	uint32_t* guard_footer = pointer_offset(start, size);
	for (int guard_loop = 0; guard_loop < (FOUNDATION_MIN_ALIGN * 2) / 4; ++guard_loop) {
		*(guard_header + guard_loop) = MEMORY_GUARD_VALUE;
		*(guard_footer + guard_loop) = MEMORY_GUARD_VALUE;
	}
	*guard_header = (uint32_t)align;
	*(guard_header + 1) = (uint32_t)size;
	return start;
}

static void*
memory_guard_verify(void* memory) {
	uint32_t* guard_header = pointer_offset(memory, -FOUNDATION_MIN_ALIGN * 2);
	uint32_t align = *guard_header;
	uint32_t size = *(guard_header + 1);
	uint32_t* guard_footer = pointer_offset(memory, size);
	for (int guard_loop = 0; guard_loop < (FOUNDATION_MIN_ALIGN * 2) / 4; ++guard_loop) {
		if ((guard_loop > 1) && (*(guard_header + guard_loop) != MEMORY_GUARD_VALUE)) {
			FOUNDATION_ASSERT_MSG(*guard_header == MEMORY_GUARD_VALUE, "Memory underwrite");
			return 0;
		}
		if (*(guard_footer + guard_loop) != MEMORY_GUARD_VALUE) {
			FOUNDATION_ASSERT_MSG(*guard_footer == MEMORY_GUARD_VALUE, "Memory overwrite");
			return 0;
		}
	}
	return pointer_offset(memory, -(int32_t)align);
}

#endif

void*
memory_allocate(hash_t context, size_t size, unsigned int align, unsigned int hint) {
	void* p = memory_system_current.allocate(context ? context : memory_context(), size, align, hint);
	memory_track(p, size);
	return p;
}

void*
memory_reallocate(void* p, size_t size, unsigned int align, size_t oldsize, unsigned int hint) {
	memory_untrack(p);
	p = memory_system_current.reallocate(p, size, align, oldsize, hint);
	memory_track(p, size);
	return p;
}

void
memory_deallocate(void* p) {
	memory_untrack(p);
	memory_system_current.deallocate(p);
}

size_t
memory_size(const void* p) {
	return p ? memory_system_current.usable_size(p) : 0;
}

bool
memory_verify(const void* p) {
	return memory_system_current.verify ? memory_system_current.verify(p) : true;
}

memory_statistics_t
memory_statistics(void) {
	memory_statistics_t stats;
	atomic_thread_fence_acquire();
	memcpy(&stats, &memory_stats, sizeof(memory_statistics_t));
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
	if (memory_system_current.thread_initialize)
		memory_system_current.thread_initialize();
}

void
memory_thread_finalize(void) {
	if (memory_system_current.thread_finalize)
		memory_system_current.thread_finalize();
}

static void*
memory_allocate_malloc_raw(size_t size, unsigned int align, unsigned int hint) {
	FOUNDATION_UNUSED(hint);
	void* memory;

#if BUILD_ENABLE_MEMORY_GUARD || FOUNDATION_PLATFORM_WINDOWS
	if (align < FOUNDATION_MIN_ALIGN)
		align = FOUNDATION_MIN_ALIGN;
#endif
#if BUILD_ENABLE_MEMORY_GUARD
	size_t actual_size = size;
	if (align < (FOUNDATION_MIN_ALIGN * 2))
		size += FOUNDATION_MIN_ALIGN * 4;
	else
		size += align * 2;
#endif
#if FOUNDATION_PLATFORM_WINDOWS
#if BUILD_ENABLE_MEMORY_GUARD > 1
	size_t paged_size = size;
	if (paged_size % 4096)
		paged_size += 4096 - (paged_size % 4096);
	memory = VirtualAlloc(0, paged_size + 8192, MEM_COMMIT, PAGE_READWRITE);
	VirtualFree(memory, 4096, MEM_DECOMMIT);
	memory = pointer_offset(memory, 4096);
	VirtualFree(pointer_offset(memory, paged_size), 4096, MEM_DECOMMIT);
#else
	memory = _aligned_malloc(size, align);
#endif
#else
#if BUILD_ENABLE_MEMORY_GUARD > 1
	size_t page_size = (size_t)sysconf(_SC_PAGESIZE);
	size_t paged_size = size;
	if (paged_size % page_size)
		paged_size += page_size - (paged_size % page_size);
	memory = mmap(0, paged_size + (page_size * 3), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	size_t* size_store = memory;
	*size_store = paged_size;
	munmap(pointer_offset(memory, page_size), page_size);
	memory = pointer_offset(memory, page_size * 2);
	munmap(pointer_offset(memory, paged_size), page_size);
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
#endif
#if BUILD_ENABLE_MEMORY_GUARD
	memory = memory_guard_initialize(memory, actual_size, align);
#endif
	if (!memory) {
		log_errorf(HASH_MEMORY, ERROR_OUT_OF_MEMORY, STRING_CONST("Unable to allocate %" PRIsize " bytes of memory"),
		           size);
	}
	return memory;
}

static void*
memory_allocate_malloc(hash_t context, size_t size, unsigned int align, unsigned int hint) {
	void* block;
	FOUNDATION_UNUSED(context);
	block = memory_allocate_malloc_raw(size, align, hint);
	if (block && (hint & MEMORY_ZERO_INITIALIZED))
		memset(block, 0, (size_t)size);
	return block;
}

static void
memory_deallocate_malloc(void* p) {
	if (!p)
		return;

#if BUILD_ENABLE_MEMORY_GUARD
	p = memory_guard_verify(p);
#endif
#if FOUNDATION_PLATFORM_WINDOWS
#if BUILD_ENABLE_MEMORY_GUARD > 1
	VirtualFree(pointer_offset(p, -4096), 0, MEM_RELEASE);
#else
	_aligned_free(p);
#endif
#else
#if BUILD_ENABLE_MEMORY_GUARD > 1
	size_t page_size = (size_t)sysconf(_SC_PAGESIZE);
	size_t* size_store = pointer_offset(p, -(page_size * 2));
	munmap(p, *size_store);
	munmap(size_store, page_size);
#else
	free(p);
#endif
#endif
}

static void*
memory_reallocate_malloc(void* p, size_t size, unsigned int align, size_t oldsize, unsigned int hint) {
	void* block = nullptr;
#if BUILD_ENABLE_MEMORY_GUARD
	if (align < FOUNDATION_MIN_ALIGN)
		align = FOUNDATION_MIN_ALIGN;
#else
#if FOUNDATION_PLATFORM_WINDOWS
	block = _aligned_realloc(p, size, align > FOUNDATION_MIN_ALIGN ? align : FOUNDATION_MIN_ALIGN);
#else
	if (align <= FOUNDATION_MIN_ALIGN)
		block = realloc(p, size);
#endif
#endif
	if (!block) {
		block = memory_allocate_malloc_raw(size, align, hint);
		if (p && block && oldsize && !(hint & MEMORY_NO_PRESERVE))
			memcpy(block, p, (size < oldsize) ? (size_t)size : (size_t)oldsize);
		memory_deallocate_malloc(p);
	}

	if (!block) {
		string_const_t errmsg = system_error_message(0);
		log_panicf(HASH_MEMORY, ERROR_OUT_OF_MEMORY,
		           STRING_CONST("Unable to reallocate memory (%" PRIsize " -> %" PRIsize " @ 0x%" PRIfixPTR "): %.*s"),
		           oldsize, size, (uintptr_t)p, STRING_FORMAT(errmsg));
	} else {
		if ((hint & MEMORY_ZERO_INITIALIZED) && block && (size > oldsize))
			memset(pointer_offset(block, oldsize), 0, (size - oldsize));
	}

	return block;
}

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic push
#if __has_warning("-Wcast-qual")
#pragma clang diagnostic ignored "-Wcast-qual"
#endif
#endif

static size_t
memory_usable_size_malloc(const void* p) {
#if BUILD_ENABLE_MEMORY_GUARD
	const uint32_t* guard_header = pointer_offset_const(p, -FOUNDATION_MIN_ALIGN * 2);
	return guard_header[1];
#else
#if FOUNDATION_PLATFORM_WINDOWS
	return _msize((void*)p);
#elif FOUNDATION_PLATFORM_APPLE
	return malloc_size(p);
#else
	return malloc_usable_size((void*)p);
#endif
#endif
}

static bool
memory_verify_malloc(const void* p) {
#if BUILD_ENABLE_MEMORY_GUARD
	return memory_guard_verify((void*)p) != 0;
#else
	FOUNDATION_UNUSED(p);
	return true;
#endif
}

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic pop
#endif

static int
memory_initialize_malloc(void) {
	return 0;
}

static void
memory_finalize_malloc(void) {
}

memory_system_t
memory_system_malloc(void) {
	memory_system_t memsystem;
	memset(&memsystem, 0, sizeof(memsystem));
	memsystem.allocate = memory_allocate_malloc;
	memsystem.reallocate = memory_reallocate_malloc;
	memsystem.deallocate = memory_deallocate_malloc;
	memsystem.usable_size = memory_usable_size_malloc;
	memsystem.verify = memory_verify_malloc;
	memsystem.initialize = memory_initialize_malloc;
	memsystem.finalize = memory_finalize_malloc;
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
	memory_tracker_t old_tracker = memory_tracker_current;

	if ((old_tracker.track == tracker.track) && (old_tracker.untrack == tracker.untrack))
		return;

	memory_tracker_current = memory_no_tracker;

	if (old_tracker.abort)
		old_tracker.abort();

	if (old_tracker.finalize)
		old_tracker.finalize();

	if (memory_initialized) {
		if (tracker.initialize)
			tracker.initialize();

		memory_tracker_current = tracker;
	} else {
		memory_tracker_preinit = tracker;
	}
}

static void
memory_track(void* addr, size_t size) {
	if (addr && memory_tracker_current.track)
		memory_tracker_current.track(addr, size);
}

static void
memory_untrack(void* addr) {
	if (addr && memory_tracker_current.untrack)
		memory_tracker_current.untrack(addr);
}

void
memory_tracker_dump(memory_tracker_handler_fn handler) {
	if (memory_tracker_current.dump)
		memory_tracker_current.dump(handler);
}

FOUNDATION_ALIGNED_STRUCT(memory_tag_t, 8) {
	void* address;
	size_t size;
	uint64_t counter;
	void* trace[14];
};

typedef struct memory_tag_t memory_tag_t;

struct memory_tag_bucket_t {
	atomic32_t lock;
	memory_tag_t* tags;
	size_t size;
	size_t capacity;
};

typedef struct memory_tag_bucket_t memory_tag_bucket_t;

static memory_tag_bucket_t* memory_tag_map;
static bool memory_tracker_initialized;

#define MEMORY_TAG_MAP_SIZE 3947

static int
memory_tracker_initialize(void) {
	if (!memory_tracker_initialized) {
		size_t size = sizeof(memory_tag_bucket_t) * MEMORY_TAG_MAP_SIZE;
		memory_tag_map = memory_system_current.allocate(0, size, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		memory_tracker_initialized = true;
	}

	return 0;
}

static void
memory_tracker_cleanup(void) {
	memory_tracker_initialized = false;
	if (memory_tag_map) {
		memory_system_current.deallocate(memory_tag_map);
		memory_tag_map = nullptr;
	}
}

static void
memory_tracker_finalize(void) {
#if FOUNDATION_PLATFORM_APPLE
	// Hack to allow system dispatch threads time to finalize
	// and free memory during shutdown
	thread_sleep(100);
#endif

	memory_tracker_initialized = false;
	if (memory_tag_map) {
		for (uint ibucket = 0; ibucket < MEMORY_TAG_MAP_SIZE; ++ibucket) {
			memory_tag_bucket_t* bucket = memory_tag_map + ibucket;
			memory_tag_t* tag = bucket->tags;
			for (size_t itag = 0; tag && (itag < bucket->size); ++itag, ++tag) {
				void* addr = tag->address;
				if (addr) {
					char tracebuf[1024];
					string_t trace = stacktrace_resolve(tracebuf, sizeof(tracebuf), tag->trace,
					                                    sizeof(tag->trace) / sizeof(tag->trace[0]), 0);
					log_warnf(
					    HASH_MEMORY, WARNING_MEMORY,
					    STRING_CONST("Memory leak allocation %" PRIu64 ": %" PRIsize " bytes @ 0x%" PRIfixPTR "\n%.*s"),
					    tag->counter, tag->size, (uintptr_t)addr, STRING_FORMAT(trace));
				}
			}
			if (bucket->tags)
				memory_system_current.deallocate(bucket->tags);
			bucket->tags = 0;
			bucket->size = 0;
			bucket->capacity = 0;
		}
	}
	memory_tracker_cleanup();
}

static void
memory_tracker_dump_impl(memory_tracker_handler_fn handler) {
	if (!memory_tracker_initialized || !memory_tag_map)
		return;
	for (uint ibucket = 0; ibucket < MEMORY_TAG_MAP_SIZE; ++ibucket) {
		memory_tag_bucket_t* bucket = memory_tag_map + ibucket;
		memory_tag_t* tag = bucket->tags;
		for (size_t itag = 0; tag && (itag < bucket->size); ++itag, ++tag) {
			void* addr = tag->address;
			if (addr) {
				if (handler(addr, tag->size, tag->trace, sizeof(tag->trace) / sizeof(tag->trace[0])))
					break;
			}
		}
	}
}

static void
memory_tracker_track(void* addr, size_t size) {
	if (!addr || !memory_tracker_initialized || !memory_tag_map)
		return;

	uintptr_t bucket_index = (uintptr_t)addr % MEMORY_TAG_MAP_SIZE;
	memory_tag_bucket_t* bucket = memory_tag_map + bucket_index;
	while (!atomic_cas32(&bucket->lock, 1, 0, memory_order_acquire, memory_order_acquire))
		thread_yield();

	if (bucket->size >= bucket->capacity) {
		size_t new_capacity = (bucket->capacity ? (bucket->capacity << 2) : 1024);
		bucket->tags = memory_system_current.reallocate(bucket->tags, sizeof(memory_tag_t) * new_capacity, 8,
		                                                sizeof(memory_tag_t) * bucket->capacity, MEMORY_PERSISTENT);
		bucket->capacity = new_capacity;
	}

	memory_tag_t* tag = bucket->tags + bucket->size;
	tag->address = addr;
	tag->size = size;
	tag->counter = (uint64_t)atomic_incr64(&memory_stats.allocations_total, memory_order_relaxed);
	stacktrace_capture(tag->trace, sizeof(tag->trace) / sizeof(tag->trace[0]), 3);

	++bucket->size;
	atomic_store32(&bucket->lock, 0, memory_order_release);

#if BUILD_ENABLE_MEMORY_STATISTICS
	atomic_incr64(&memory_stats.allocations_current, memory_order_relaxed);
	atomic_add64(&memory_stats.allocated_total, (int32_t)size, memory_order_relaxed);
	atomic_add64(&memory_stats.allocated_current, (int32_t)size, memory_order_relaxed);
	atomic_thread_fence_release();
#endif
}

static void
memory_tracker_untrack(void* addr) {
	if (!addr || !memory_tracker_initialized || !memory_tag_map)
		return;

	uintptr_t bucket_index = (uintptr_t)addr % MEMORY_TAG_MAP_SIZE;
	memory_tag_bucket_t* bucket = memory_tag_map + bucket_index;
	while (!atomic_cas32(&bucket->lock, 1, 0, memory_order_acquire, memory_order_acquire))
		thread_yield();

	bool found = false;
	size_t size = 0;
	memory_tag_t* tag = bucket->tags;
	for (size_t itag = 0; itag < bucket->size; ++itag, ++tag) {
		if (tag->address == addr) {
			size = tag->size;
			// Swap with last
			if (itag < (bucket->size - 1))
				memcpy(tag, bucket->tags + (bucket->size - 1), sizeof(memory_tag_t));
			--bucket->size;
			found = true;
			break;
		}
	}

	FOUNDATION_ASSERT_MSG(found, "Deallocating untracked pointer");

	atomic_store32(&bucket->lock, 0, memory_order_release);

#if BUILD_ENABLE_MEMORY_STATISTICS
	atomic_decr64(&memory_stats.allocations_current, memory_order_relaxed);
	atomic_add64(&memory_stats.allocated_current, -(int32_t)size, memory_order_relaxed);
	atomic_thread_fence_release();
#endif
}

#endif

memory_tracker_t
memory_tracker_local(void) {
	memory_tracker_t tracker = memory_no_tracker;
#if BUILD_ENABLE_MEMORY_TRACKER
	tracker.track = memory_tracker_track;
	tracker.untrack = memory_tracker_untrack;
	tracker.dump = memory_tracker_dump_impl;
	tracker.initialize = memory_tracker_initialize;
	tracker.abort = memory_tracker_cleanup;
	tracker.finalize = memory_tracker_finalize;
#endif
	return tracker;
}

memory_tracker_t
memory_tracker_none(void) {
	return memory_no_tracker;
}
