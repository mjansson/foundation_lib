/* memory.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <foundation/foundation.h>
#include <foundation/internal.h>

#if FOUNDATION_PLATFORM_WINDOWS
#  include <foundation/windows.h>
#endif
#if FOUNDATION_PLATFORM_POSIX
#  include <foundation/posix.h>
#  include <sys/mman.h>
#endif

#if FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
#  include <malloc.h>
#endif

#if FOUNDATION_PLATFORM_PNACL
#  include <stdlib.h>
#endif

static memory_system_t _memory_system;
static memory_tracker_t _memory_no_tracker;

typedef FOUNDATION_ALIGN(8) struct {
	void*               storage;
	void*               end;
	atomicptr_t         head;
	void*               tail;
	size_t              size;
	size_t              maxchunk;
} atomic_linear_memory_t;

static atomic_linear_memory_t _memory_temporary;

#if BUILD_ENABLE_MEMORY_GUARD
#define MEMORY_GUARD_VALUE 0xDEADBEEF
#endif

#if BUILD_ENABLE_MEMORY_TRACKER

static memory_tracker_t _memory_tracker;

static void
_memory_track(void* addr, size_t size);

static void
_memory_untrack(void* addr);

#else

#define _memory_track(addr, size ) do { (void)sizeof( (addr) ); (void)sizeof( (size) ); } while(0)
#define _memory_untrack(addr) do { (void)sizeof( (addr) ); } while(0)

#endif

// Max align must at least be sizeof( size_t )
#if FOUNDATION_PLATFORM_ANDROID
#  define FOUNDATION_MAX_ALIGN  8
#else
#  define FOUNDATION_MAX_ALIGN  16
#endif

static void
_atomic_allocate_initialize(size_t storagesize) {
	if (storagesize < 1024)
		storagesize = _foundation_def.temporary_memory;
	_memory_temporary.storage   = memory_allocate(0, storagesize, 16, MEMORY_PERSISTENT);
	_memory_temporary.end       = pointer_offset(_memory_temporary.storage, storagesize);
	_memory_temporary.size      = storagesize;
	_memory_temporary.maxchunk  = (storagesize / 8);
	atomic_storeptr(&_memory_temporary.head, _memory_temporary.storage);
}

static void
_atomic_allocate_finalize(void) {
	if (_memory_temporary.storage)
		memory_deallocate(_memory_temporary.storage);
	memset(&_memory_temporary, 0, sizeof(_memory_temporary));
}

static void*
_atomic_allocate_linear(size_t chunksize) {
	void* old_head;
	void* new_head;
	void* return_pointer = 0;

	do {
		old_head = atomic_loadptr(&_memory_temporary.head);
		new_head = pointer_offset(old_head, chunksize);

		return_pointer = old_head;

		if (new_head > _memory_temporary.end) {
			new_head = pointer_offset(_memory_temporary.storage, chunksize);
			return_pointer = _memory_temporary.storage;
		}
	}
	while (!atomic_cas_ptr(&_memory_temporary.head, new_head, old_head));

	return return_pointer;
}

static FOUNDATION_CONSTCALL FOUNDATION_FORCEINLINE int unsigned
_memory_get_align(unsigned int align) {
	//All alignment in memory code is built around higher alignments
	//being multiples of lower alignments (powers of two).
	//4, 8, 16, ...
#if FOUNDATION_PLATFORM_ANDROID
	return align > 0 ? FOUNDATION_MAX_ALIGN : 0;
#elif FOUNDATION_PLATFORM_WINDOWS
	if (align < FOUNDATION_SIZE_POINTER)
		return FOUNDATION_SIZE_POINTER;
	align = math_align_poweroftwo(align);
	return (align < FOUNDATION_MAX_ALIGN) ? align : FOUNDATION_MAX_ALIGN;
#else
	if (align < FOUNDATION_SIZE_POINTER)
		return align ? FOUNDATION_SIZE_POINTER : 0;
	align = math_align_poweroftwo(align);
	return (align < FOUNDATION_MAX_ALIGN) ? align : FOUNDATION_MAX_ALIGN;
#endif
}

static FOUNDATION_CONSTCALL FOUNDATION_FORCEINLINE int unsigned
_memory_get_align_forced(unsigned int align) {
	align = _memory_get_align(align);
	return align > FOUNDATION_SIZE_POINTER ? align : FOUNDATION_SIZE_POINTER;
}

static FOUNDATION_CONSTCALL void*
_memory_align_pointer(void* p, unsigned int align) {
	uintptr_t address;
	uintptr_t mask;
	if (!p || !align)
		return p;

	address = (uintptr_t)p;
	mask = (uintptr_t)align - 1; //Align is always power-of-two
	if (address & mask) {
		address = (address & ~mask) + align;
		p = (void*)address;
	}

	return p;
}

int
_memory_initialize(const memory_system_t memory) {
	_memory_system = memory;
	return _memory_system.initialize();
}

void
_memory_preallocate(void) {
	hash_t tracker;

	if (!_memory_temporary.storage)
		_atomic_allocate_initialize((size_t)config_int(HASH_FOUNDATION, HASH_TEMPORARY_MEMORY));

	tracker = config_hash(HASH_FOUNDATION, HASH_MEMORY_TRACKER);
	if (tracker == HASH_LOCAL)
		memory_set_tracker(memory_tracker_local());
}

void
_memory_finalize(void) {
	memory_set_tracker(_memory_no_tracker);

	_atomic_allocate_finalize();
	_memory_system.finalize();
}

#if BUILD_ENABLE_MEMORY_GUARD

static void*
_memory_guard_initialize(void* memory, size_t size) {
	int guard_loop;
	uint32_t* guard_header = pointer_offset(memory, FOUNDATION_MAX_ALIGN);
	uint32_t* guard_footer = pointer_offset(memory, size + FOUNDATION_MAX_ALIGN * 2);
	*(size_t*)memory = size;
	for (guard_loop = 0; guard_loop < FOUNDATION_MAX_ALIGN / 4; ++guard_loop) {
		*guard_header++ = MEMORY_GUARD_VALUE;
		*guard_footer++ = MEMORY_GUARD_VALUE;
	}
	return pointer_offset(memory, FOUNDATION_MAX_ALIGN * 2);
}

static void*
_memory_guard_verify(void* memory) {
	int guard_loop;
	size_t    size = *(size_t*)pointer_offset(memory, -FOUNDATION_MAX_ALIGN * 2);
	uint32_t* guard_header = pointer_offset(memory, -FOUNDATION_MAX_ALIGN);
	uint32_t* guard_footer = pointer_offset(memory, size);
	for (guard_loop = 0; guard_loop < FOUNDATION_MAX_ALIGN / 4; ++guard_loop) {
		if (*guard_header != MEMORY_GUARD_VALUE)
			FOUNDATION_ASSERT_MSG(*guard_header == MEMORY_GUARD_VALUE, "Memory underwrite");
		if (*guard_footer != MEMORY_GUARD_VALUE)
			FOUNDATION_ASSERT_MSG(*guard_footer == MEMORY_GUARD_VALUE, "Memory overwrite");
		guard_header++;
		guard_footer++;
	}
	return pointer_offset(memory, -FOUNDATION_MAX_ALIGN * 2);
}

#endif

void*
memory_allocate(hash_t context, size_t size, unsigned int align, unsigned int hint) {
	void* p = 0;
	if (hint & MEMORY_TEMPORARY) {
		align = _memory_get_align_forced(align);
		if (_memory_temporary.storage && (size + align < _memory_temporary.maxchunk)) {
			p = _memory_align_pointer(_atomic_allocate_linear(size + align), align);
			FOUNDATION_ASSERT(!((uintptr_t)p & 1));
			if (hint & MEMORY_ZERO_INITIALIZED)
				memset(p, 0, (size_t)size);
		}
	}
	if (!p)
		p = _memory_system.allocate(context ? context : memory_context(), size, align, hint);
	_memory_track(p, size);
	return p;
}

void*
memory_reallocate(void* p, size_t size, unsigned int align, size_t oldsize) {
	FOUNDATION_ASSERT_MSG((p < _memory_temporary.storage) ||
	                      (p >= _memory_temporary.end), "Trying to reallocate temporary memory");
	_memory_untrack(p);
	p = _memory_system.reallocate(p, size, align, oldsize);
	_memory_track(p, size);
	return p;
}

void
memory_deallocate(void* p) {
	if ((p < _memory_temporary.storage) || (p >= _memory_temporary.end))
		_memory_system.deallocate(p);
	_memory_untrack(p);
}

#if BUILD_ENABLE_MEMORY_CONTEXT

FOUNDATION_DECLARE_THREAD_LOCAL(memory_context_t*, memory_context, 0)

void
memory_context_push(hash_t context_id) {
	memory_context_t* context = get_thread_memory_context();
	if (!context) {
		context = memory_allocate(0, sizeof(memory_context_t) + (sizeof(hash_t) *
		                                                         _foundation_def.memory_context_depth), 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		set_thread_memory_context(context);
	}
	context->context[ context->depth ] = context_id;
	if (context->depth < (_foundation_def.memory_context_depth - 1))
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
	return (context && (context->depth > 0)) ? context->context[ context->depth - 1 ] : 0;
}

void
memory_context_thread_finalize(void) {
	memory_context_t* context = get_thread_memory_context();
	if (context)
		memory_deallocate(context);
	set_thread_memory_context(0);
}

#endif

#if FOUNDATION_PLATFORM_WINDOWS

typedef long (*NtAllocateVirtualMemoryFn)(HANDLE, void**, ULONG, size_t*, ULONG, ULONG);
static NtAllocateVirtualMemoryFn NtAllocateVirtualMemory = 0;

#endif

static void*
_memory_allocate_malloc_raw(size_t size, unsigned int align, unsigned int hint) {
	FOUNDATION_UNUSED(hint);

	//If we align manually, we must be able to retrieve the original pointer for passing to free()
	//Thus all allocations need to go through that path

#if FOUNDATION_PLATFORM_WINDOWS

#  if FOUNDATION_SIZE_POINTER == 4
#    if BUILD_ENABLE_MEMORY_GUARD
	char* memory = _aligned_malloc((size_t)size + FOUNDATION_MAX_ALIGN * 3, align);
	if (memory)
		memory = _memory_guard_initialize(memory, (size_t)size);
	return memory;
#    else
	return _aligned_malloc((size_t)size, align);
#    endif
#  else
	unsigned int padding, extra_padding = 0;
	size_t allocate_size;
	char* raw_memory;
	void* memory;
	long vmres;

	if (!(hint & MEMORY_32BIT_ADDRESS)) {
		padding = (align > FOUNDATION_SIZE_POINTER ? align : FOUNDATION_SIZE_POINTER);
#if BUILD_ENABLE_MEMORY_GUARD
		extra_padding = FOUNDATION_MAX_ALIGN * 3;
#endif
		raw_memory = _aligned_malloc((size_t)size + padding + extra_padding, align);
		if (raw_memory) {
			memory = raw_memory +
			         padding; //Will be aligned since padding is multiple of alignment (minimum align/pad is pointer size)
			*((void**)memory - 1) = raw_memory;
			FOUNDATION_ASSERT(!((uintptr_t)raw_memory & 1));
			FOUNDATION_ASSERT(!((uintptr_t)memory & 1));
#if BUILD_ENABLE_MEMORY_GUARD
			memory = _memory_guard_initialize(memory, size);
			FOUNDATION_ASSERT(!((uintptr_t)memory & 1));
#endif
			return memory;
		}
		log_errorf(HASH_MEMORY, ERROR_OUT_OF_MEMORY, "Unable to allocate %llu bytes of memory", size);
		return 0;
	}

#    if BUILD_ENABLE_MEMORY_GUARD
	extra_padding = FOUNDATION_MAX_ALIGN * 3;
#    endif

	allocate_size = size + FOUNDATION_SIZE_POINTER + extra_padding + align;
	raw_memory = 0;

	vmres = NtAllocateVirtualMemory(INVALID_HANDLE_VALUE, &raw_memory, 1, &allocate_size,
	                                MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (vmres != 0) {
		log_errorf(HASH_MEMORY, ERROR_OUT_OF_MEMORY,
		           "Unable to allocate %llu bytes of memory in low 32bit address space", size);
		return 0;
	}

	memory = _memory_align_pointer(raw_memory + FOUNDATION_SIZE_POINTER, align);
	*((void**)memory - 1) = (void*)((uintptr_t)raw_memory | 1);
#    if BUILD_ENABLE_MEMORY_GUARD
	memory = _memory_guard_initialize(memory, size);
#    endif
	FOUNDATION_ASSERT(!((uintptr_t)raw_memory & 1));
	FOUNDATION_ASSERT(!((uintptr_t)memory & 1));
	return memory;
#  endif

#else

#  if FOUNDATION_SIZE_POINTER > 4
	if (!(hint & MEMORY_32BIT_ADDRESS))
#  endif
	{
#if BUILD_ENABLE_MEMORY_GUARD
		size_t extra_padding = FOUNDATION_MAX_ALIGN * 3;
#else
		size_t extra_padding = 0;
#endif
		size_t allocate_size = size + align + FOUNDATION_SIZE_POINTER + extra_padding;
		char* raw_memory = malloc(allocate_size);
		if (raw_memory) {
			void* memory = _memory_align_pointer(raw_memory + FOUNDATION_SIZE_POINTER, align);
			*((void**)memory - 1) = raw_memory;
			FOUNDATION_ASSERT(!((uintptr_t)raw_memory & 1));
			FOUNDATION_ASSERT(!((uintptr_t)memory & 1));
#if BUILD_ENABLE_MEMORY_GUARD
			memory = _memory_guard_initialize(memory, size);
			FOUNDATION_ASSERT(!((uintptr_t)memory & 1));
#endif
			return memory;
		}
		log_errorf(HASH_MEMORY, ERROR_OUT_OF_MEMORY,
		           STRING_CONST("Unable to allocate %" PRIsize " bytes of memory (%" PRIsize " requested)"), size,
		           allocate_size);
		return 0;
	}

#  if FOUNDATION_SIZE_POINTER > 4

	size_t allocate_size;
	char* raw_memory;
	void* memory;

#    if BUILD_ENABLE_MEMORY_GUARD
	unsigned int extra_padding = FOUNDATION_MAX_ALIGN * 3;
#else
	unsigned int extra_padding = 0;
#    endif

	allocate_size = size + align + FOUNDATION_SIZE_POINTER * 2 + extra_padding;

#ifndef MAP_UNINITIALIZED
#define MAP_UNINITIALIZED 0
#endif

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#    ifndef MAP_32BIT
	//On MacOSX app needs to be linked with -pagezero_size 10000 -image_base 100000000 to
	// 1) Free up low 4Gb address range by reducing page zero size
	// 2) Move executable base address above 4Gb to free up more memory address space
#define MMAP_REGION_START ((uintptr_t)0x10000)
#define MMAP_REGION_END   ((uintptr_t)0x80000000)
	static atomicptr_t baseaddr = { (void*)MMAP_REGION_START };
	bool retried = false;
	do {
		raw_memory = mmap(atomic_loadptr(&baseaddr), allocate_size, PROT_READ | PROT_WRITE,
		                  MAP_PRIVATE | MAP_ANONYMOUS | MAP_UNINITIALIZED, -1, 0);
		if (((uintptr_t)raw_memory >= MMAP_REGION_START) &&
		    (uintptr_t)(raw_memory + allocate_size) < MMAP_REGION_END) {
			atomic_storeptr(&baseaddr, pointer_offset(raw_memory, allocate_size));
			break;
		}
		if (raw_memory)
			munmap(raw_memory, allocate_size);
		raw_memory = 0;
		if (retried)
			break;
		retried = true;
		atomic_storeptr(&baseaddr, (void*)MMAP_REGION_START);
	}
	while (true);
#    else
	raw_memory = mmap(0, allocate_size, PROT_READ | PROT_WRITE,
	                  MAP_32BIT | MAP_PRIVATE | MAP_ANONYMOUS | MAP_UNINITIALIZED, -1, 0);
#    endif
	if (!raw_memory) {
		log_errorf(HASH_MEMORY, ERROR_OUT_OF_MEMORY,
		           STRING_CONST("Unable to allocate %" PRIsize " bytes of memory in low 32bit address space"),
		           size);
		return 0;
	}

	memory = _memory_align_pointer(raw_memory + FOUNDATION_SIZE_POINTER * 2, align);
	*((uintptr_t*)memory - 1) = ((uintptr_t)raw_memory | 1);
	*((uintptr_t*)memory - 2) = (uintptr_t)allocate_size;
	FOUNDATION_ASSERT(!((uintptr_t)raw_memory & 1));
	FOUNDATION_ASSERT(!((uintptr_t)memory & 1));
#    if BUILD_ENABLE_MEMORY_GUARD
	memory = _memory_guard_initialize(memory, size);
	FOUNDATION_ASSERT(!((uintptr_t)memory & 1));
#    endif

	return memory;

#  endif

#endif
}

static void*
_memory_allocate_malloc(hash_t context, size_t size, unsigned  int align, unsigned int hint) {
	void* block;
	FOUNDATION_UNUSED(context);
	align = _memory_get_align(align);
	block = _memory_allocate_malloc_raw(size, align, hint);
	if (block && (hint & MEMORY_ZERO_INITIALIZED))
		memset(block, 0, (size_t)size);
	return block;
}

static void
_memory_deallocate_malloc(void* p) {
#if FOUNDATION_SIZE_POINTER == 4
	if (!p)
		return;
#  if BUILD_ENABLE_MEMORY_GUARD
	p = _memory_guard_verify(p);
#  endif
#  if FOUNDATION_PLATFORM_WINDOWS
	_aligned_free(p);
#  else
	free(*((void**)p - 1));
#  endif

#else

	uintptr_t raw_ptr;

	if (!p)
		return;

#  if BUILD_ENABLE_MEMORY_GUARD
	p = _memory_guard_verify(p);
#  endif
	raw_ptr = *((uintptr_t*)p - 1);
	if (raw_ptr & 1) {
#  if FOUNDATION_PLATFORM_WINDOWS
		VirtualFree((void*)raw_ptr, 0, MEM_RELEASE);
#  else
		uintptr_t raw_size = *((uintptr_t*)p - 2);
		munmap((void*)raw_ptr, raw_size);
#  endif
	}
	else {
#  if FOUNDATION_PLATFORM_WINDOWS
		_aligned_free((void*)raw_ptr);
#  else
		free((void*)raw_ptr);
#  endif
	}

#endif
}

static void*
_memory_reallocate_malloc(void* p, size_t size, unsigned  int align, size_t oldsize) {
#if ( FOUNDATION_SIZE_POINTER == 4 ) && FOUNDATION_PLATFORM_WINDOWS
	align = _memory_get_align(align);
#  if BUILD_ENABLE_MEMORY_GUARD
	if (p) {
		p = _memory_guard_verify(p);
		p = _aligned_realloc(p, (size_t)size + FOUNDATION_MAX_ALIGN * 3, align);
	}
	else {
		p = _aligned_malloc((size_t)size + FOUNDATION_MAX_ALIGN * 3, align);
	}
	if (p)
		p = _memory_guard_initialize(p, (size_t)size);
	return p;
#  else
	return _aligned_realloc(p, (size_t)size, align);
#  endif
#else
	void* memory;
	void* raw_p;

	align = _memory_get_align(align);

	memory = p;
#  if BUILD_ENABLE_MEMORY_GUARD
	if (memory)
		memory = _memory_guard_verify(memory);
#  endif
	raw_p = memory ? *((void**)memory - 1) : 0;
	memory = 0;

#if FOUNDATION_PLATFORM_WINDOWS
	if (raw_p && !((uintptr_t)raw_p & 1)) {
		size_t padding = (align > FOUNDATION_SIZE_POINTER ? align : FOUNDATION_SIZE_POINTER);
#  if BUILD_ENABLE_MEMORY_GUARD
		size_t extra_padding = FOUNDATION_MAX_ALIGN * 3;
#  else
		size_t extra_padding = 0;
#  endif
		void* raw_memory = _aligned_realloc(raw_p, size + padding + extra_padding, align ? align : 8);
		if (raw_memory) {
			memory = pointer_offset(raw_memory, padding);
			*((void**)memory - 1) = raw_memory;
		}
#  if BUILD_ENABLE_MEMORY_GUARD
		memory = _memory_guard_initialize(memory, size);
#  endif
	}
	else {
#  if FOUNDATION_SIZE_POINTER == 4
		memory = _memory_allocate_malloc_raw(size, align, 0U);
#  else
		memory = _memory_allocate_malloc_raw(size, align,
		                                     (raw_p && ((uintptr_t)raw_p < 0xFFFFFFFFULL)) ?
		                                       MEMORY_32BIT_ADDRESS : 0U);
#  endif
		if (p && memory && oldsize)
			memcpy(memory, p, (size < oldsize) ? size : oldsize);
		_memory_deallocate_malloc(p);
	}

#else //!FOUNDATION_PLATFORM_WINDOWS

//If we're on ARM the realloc can return a 16-bit aligned address, causing raw pointer store to SIGILL
//Realigning does not work since the realloc memory copy preserve cannot be done properly. Revert to normal alloc-and-copy
//Same with alignment, since we cannot guarantee that the returned memory block offset from start of actual memory block
//is the same in the reallocated block as the original block, we need to alloc-and-copy to get alignment
//Memory guard introduces implicit alignments as well so alloc-and-copy for that
#if !FOUNDATION_ARCH_ARM && !FOUNDATION_ARCH_ARM_64 && !BUILD_ENABLE_MEMORY_GUARD
	if (!align && raw_p && !((uintptr_t)raw_p & 1)) {
		void* raw_memory = realloc(raw_p, (size_t)size + FOUNDATION_SIZE_POINTER);
		if (raw_memory) {
			*(void**)raw_memory = raw_memory;
			memory = pointer_offset(raw_memory, FOUNDATION_SIZE_POINTER);
		}
	}
	else
#endif
	{
#  if FOUNDATION_SIZE_POINTER == 4
#    if !BUILD_ENABLE_LOG
		FOUNDATION_UNUSED(raw_p);
#    endif
		memory = _memory_allocate_malloc_raw(size, align, 0U);
#  else
		memory = _memory_allocate_malloc_raw(size, align,
		                                     (raw_p && ((uintptr_t)raw_p < 0xFFFFFFFFULL)) ?
		                                       MEMORY_32BIT_ADDRESS : 0U);
#  endif
		if (p && memory && oldsize)
			memcpy(memory, p, (size < oldsize) ? (size_t)size : (size_t)oldsize);
		_memory_deallocate_malloc(p);
	}

#endif

	if (!memory) {
		string_const_t errmsg = system_error_message(0);
		log_panicf(HASH_MEMORY, ERROR_OUT_OF_MEMORY,
		           STRING_CONST("Unable to reallocate memory: %*s (%" PRIfixPTR ", raw %" PRIfixPTR ")"),
		           STRING_FORMAT(errmsg), (uintptr_t)p, (uintptr_t)raw_p);
	}

	return memory;
#endif
}

static int
_memory_initialize_malloc(void) {
#if FOUNDATION_PLATFORM_WINDOWS && ( FOUNDATION_SIZE_POINTER > 4 )
	NtAllocateVirtualMemory = (NtAllocateVirtualMemoryFn)GetProcAddress(GetModuleHandleA("ntdll.dll"),
	                          "NtAllocateVirtualMemory");
#endif
	return 0;
}

static void
_memory_finalize_malloc(void) {
}

memory_system_t
memory_system_malloc(void) {
	memory_system_t memsystem;
	memsystem.allocate = _memory_allocate_malloc;
	memsystem.reallocate = _memory_reallocate_malloc;
	memsystem.deallocate = _memory_deallocate_malloc;
	memsystem.initialize = _memory_initialize_malloc;
	memsystem.finalize = _memory_finalize_malloc;
	return memsystem;
}

#if BUILD_ENABLE_MEMORY_TRACKER

void
memory_set_tracker(memory_tracker_t tracker) {
	memory_tracker_t old_tracker = _memory_tracker;

	if ((old_tracker.track == tracker.track) && (old_tracker.untrack == tracker.untrack))
		return;

	_memory_tracker = _memory_no_tracker;

	if (old_tracker.finalize)
		old_tracker.finalize();

	if (tracker.initialize)
		tracker.initialize();

	_memory_tracker = tracker;
}

#if BUILD_ENABLE_MEMORY_TRACKER

static void
_memory_track(void* addr, size_t size) {
	if (_memory_tracker.track)
		_memory_tracker.track(addr, size);
}

static void
_memory_untrack(void* addr) {
	if (_memory_tracker.untrack)
		_memory_tracker.untrack(addr);
}

#endif

struct memory_tag_t {
	atomicptr_t   address;
	size_t        size;
	void*         trace[14];
};

typedef FOUNDATION_ALIGN(8) struct memory_tag_t memory_tag_t;

static hashtable_t*  _memory_table;
static memory_tag_t* _memory_tags;
static atomic32_t    _memory_tag_next;

static int
_memory_tracker_initialize(void) {
	log_debug(HASH_MEMORY, STRING_CONST("Initializing local memory tracker"));
	if (!_memory_tags)
		_memory_tags = memory_allocate(0, sizeof(memory_tag_t) * _foundation_def.memory_tracker_max, 16,
		                               MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	if (!_memory_table)
		_memory_table = hashtable_allocate(_foundation_def.memory_tracker_max);
	return 0;
}

static void
_memory_tracker_finalize(void) {
	if (_memory_table)
		hashtable_deallocate(_memory_table);
	if (_memory_tags) {
		unsigned int it;
		bool got_leaks = false;

		log_debug(HASH_MEMORY, STRING_CONST("Checking for memory leaks"));
		for (it = 0; it < _foundation_def.memory_tracker_max; ++it) {
			memory_tag_t* tag = _memory_tags + it;
			if (atomic_loadptr(&tag->address)) {
				char tracebuf[512];
				string_t trace = stacktrace_resolve(tracebuf, 512, tag->trace, 14, 0);
				void* addr = atomic_loadptr(&tag->address);
				log_warnf(HASH_MEMORY, WARNING_MEMORY,
				          STRING_CONST("Memory leak: %" PRIsize " bytes @ 0x%" PRIfixPTR " : tag %d\n%*s"),
				          tag->size, (uintptr_t)addr, it, (int)trace.length, trace.str);
				got_leaks = true;
			}
		}
		memory_deallocate(_memory_tags);

		if (!got_leaks)
			log_debug(HASH_MEMORY, STRING_CONST("No memory leaks detected"));
	}
}

static void
_memory_tracker_track(void* addr, size_t size) {
	if (addr) do {
			int32_t tag = atomic_exchange_and_add32(&_memory_tag_next, 1);
			if (tag >= (int32_t)_foundation_def.memory_tracker_max) {
				int32_t newtag = tag % (int32_t)_foundation_def.memory_tracker_max;
				atomic_cas32(&_memory_tag_next, newtag, tag + 1);
				tag = newtag;
			}
			if (!atomic_loadptr(&_memory_tags[ tag ].address) &&
			    atomic_cas_ptr(&_memory_tags[ tag ].address, addr, 0)) {
				_memory_tags[ tag ].size = size;
				stacktrace_capture(_memory_tags[ tag ].trace, 14, 3);
				hashtable_set(_memory_table, (uintptr_t)addr, (uintptr_t)(tag + 1));
				return;
			}
		}
		while (true);
}

static void
_memory_tracker_untrack(void* addr) {
	uintptr_t tag = addr ? hashtable_get(_memory_table, (uintptr_t)addr) : 0;
	if (tag) {
		--tag;
		atomic_storeptr(&_memory_tags[ tag ].address, 0);
	}
	//else if( addr )
	//	log_warnf( HASH_MEMORY, WARNING_SUSPICIOUS, "Untracked deallocation: " PRIfixPTR, addr );
}

#endif

memory_tracker_t
memory_tracker_local(void) {
	memory_tracker_t tracker = _memory_no_tracker;
#if BUILD_ENABLE_MEMORY_TRACKER
	tracker.track = _memory_tracker_track;
	tracker.untrack = _memory_tracker_untrack;
	tracker.initialize = _memory_tracker_initialize;
	tracker.finalize = _memory_tracker_finalize;
#endif
	return tracker;
}
