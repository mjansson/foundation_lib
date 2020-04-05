/* objectmap.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

objectmap_t*
objectmap_allocate(size_t count) {
	objectmap_t* map;

	if (count < 3)
		count = 3;

	map = memory_allocate(0, sizeof(objectmap_t) + (sizeof(objectmap_entry_t) * count), 0, MEMORY_PERSISTENT);

	objectmap_initialize(map, count);

	return map;
}

void
objectmap_initialize(objectmap_t* map, size_t count) {
	// Number of bits needed to represent index
	uint32_t bits = (uint32_t)math_round(math_log2((real)count) + REAL_C(0.5));
	FOUNDATION_ASSERT_MSGFORMAT(bits <= OBJECTMAP_INDEXBITS, "Invalid objectmap size %" PRIsize, count);
	if (bits > OBJECTMAP_INDEXBITS) {
		bits = OBJECTMAP_INDEXBITS;
		// If bits == OBJECTMAP_INDEXBITS we need size to be one less than maximum size
		// in order to detect map overflow when index >= size
		count = ((1U << bits) - 1U) - 1U;
	}

	// Needed for object check in objectmap_lookup of unused index
	memset(map, 0, sizeof(objectmap_t) + (sizeof(objectmap_entry_t) * count));

	map->size = (uint32_t)count;
	semaphore_initialize(&map->write, 1);
}

void
objectmap_deallocate(objectmap_t* map) {
	objectmap_finalize(map);
	memory_deallocate(map);
}

void
objectmap_finalize(objectmap_t* map) {
	if (!map)
		return;

#if BUILD_DEBUG || BUILD_RELEASE
	atomic_thread_fence_acquire();
	for (uint32_t i = 0; (i < map->size) && (i < map->autolink); ++i) {
		if (atomic_load32(&map->map[i].ref, memory_order_relaxed)) {
			log_error(0, ERROR_MEMORY_LEAK,
			          STRING_CONST("Object still stored or slot reserved in objectmap when map deallocated"));
			break;
		}
	}
#endif

	semaphore_finalize(&map->write);
}

size_t
objectmap_size(const objectmap_t* map) {
	return map->size;
}

void*
objectmap_raw_lookup(const objectmap_t* map, size_t idx) {
	uint32_t ref = (uint32_t)atomic_load32(&map->map[idx].ref, memory_order_acquire);
	return ref ? map->map[idx].ptr : nullptr;
}

object_t
objectmap_raw_id(const objectmap_t* map, size_t idx) {
	uint32_t ref = (uint32_t)atomic_load32(&map->map[idx].ref, memory_order_acquire);
	return ref ? (ref & ~OBJECTMAP_INDEXMASK) | (uint32_t)idx : 0;
}

static uint32_t
objectmap_next_tag(objectmap_t* map) {
	uint32_t tag;
	do {
		tag = (++map->tag) & OBJECTMAP_TAGMASK;
	} while (!tag);
	return tag;
}

object_t
objectmap_reserve(objectmap_t* map) {
	uint32_t idx, tag, tagshifted, next;

	semaphore_wait(&map->write);
	{
		idx = map->free;
		if (idx >= map->size) {
			semaphore_post(&map->write);
			log_error(0, ERROR_OUT_OF_MEMORY, STRING_CONST("Map full, unable to reserve id"));
			return 0;
		}

		// Sanity check that slot isn't taken
		FOUNDATION_ASSERT_MSG(atomic_load32(&map->map[idx].ref, memory_order_acquire) == 0,
		                      "Map failed sanity check, slot taken after reserve");

		if (idx >= map->autolink) {
			next = ++map->autolink;
		} else {
			next = ((uint32_t)((uintptr_t)map->map[idx].ptr)) & OBJECTMAP_INDEXMASK;
		}
		map->free = next;
		map->map[idx].ptr = nullptr;

		tag = objectmap_next_tag(map);
		tagshifted = tag << OBJECTMAP_INDEXBITS;
		atomic_store32(&map->map[idx].ref, (int32_t)tagshifted, memory_order_release);
	}
	semaphore_post(&map->write);

	return tagshifted | idx;
}

bool
objectmap_free(objectmap_t* map, object_t id) {
	uint32_t idx, next, tag, ref, reftag;

	semaphore_wait(&map->write);
	{
		idx = id & OBJECTMAP_INDEXMASK;
		tag = id >> OBJECTMAP_INDEXBITS;
		if (idx >= map->size) {
			semaphore_post(&map->write);
			return false;
		}

		ref = (uint32_t)atomic_load32(&map->map[idx].ref, memory_order_acquire);
		reftag = ref >> OBJECTMAP_INDEXBITS;
		if (tag != reftag) {
			semaphore_post(&map->write);
			return false;
		}

		next = map->free;
		map->free = idx;
		map->map[idx].ptr = (void*)((uintptr_t)next);
		atomic_store32(&map->map[idx].ref, 0, memory_order_release);
	}
	semaphore_post(&map->write);

	return true;
}

bool
objectmap_set(objectmap_t* map, object_t id, void* object) {
	uint32_t idx, tag, reftag;

	idx = id & OBJECTMAP_INDEXMASK;
	tag = id & ~OBJECTMAP_INDEXMASK;
	if (idx >= map->size)
		return false;
	reftag = (uint32_t)atomic_load32(&map->map[idx].ref, memory_order_acquire);

	// Sanity check, can't set free slot, and non-free slot should be initialized to
	// matching tag and zero ref count in reserve function
	if (!map->map[idx].ptr && (tag == reftag)) {
		map->map[idx].ptr = object;
		atomic_store32(&map->map[idx].ref, (int32_t)(reftag | 1), memory_order_release);
		return true;
	}
	return false;
}

void*
objectmap_acquire(objectmap_t* map, object_t id) {
	uint32_t idx = id & OBJECTMAP_INDEXMASK;
	uint32_t tag = id >> OBJECTMAP_INDEXBITS;
	if (idx >= map->size)
		return 0;
	uint32_t ref = (uint32_t)atomic_load32(&map->map[idx].ref, memory_order_acquire);
	uint32_t refcount = ref & OBJECTMAP_INDEXMASK;
	uint32_t reftag = ref >> OBJECTMAP_INDEXBITS;
	while ((tag == reftag) && refcount) {
		uint32_t newref = (reftag << OBJECTMAP_INDEXBITS) | (refcount + 1);
		if (atomic_cas32(&map->map[idx].ref, (int32_t)newref, (int32_t)ref, memory_order_release, memory_order_acquire))
			return map->map[idx].ptr;
		ref = (uint32_t)atomic_load32(&map->map[idx].ref, memory_order_acquire);
		refcount = ref & OBJECTMAP_INDEXMASK;
		reftag = ref >> OBJECTMAP_INDEXBITS;
	}
	return 0;
}

bool
objectmap_release(objectmap_t* map, object_t id, object_deallocate_fn deallocate) {
	uint32_t idx = id & OBJECTMAP_INDEXMASK;
	uint32_t tag = id >> OBJECTMAP_INDEXBITS;
	if (idx >= map->size)
		return 0;
	uint32_t ref = (uint32_t)atomic_load32(&map->map[idx].ref, memory_order_acquire);
	uint32_t refcount = ref & OBJECTMAP_INDEXMASK;
	uint32_t reftag = ref >> OBJECTMAP_INDEXBITS;
	while ((tag == reftag) && refcount) {
		uint32_t newref = (reftag << OBJECTMAP_INDEXBITS) | (refcount - 1);
		if (atomic_cas32(&map->map[idx].ref, (int32_t)newref, (int32_t)ref, memory_order_release,
		                 memory_order_acquire)) {
			if (refcount == 1) {
				deallocate(map->map[idx].ptr);
				objectmap_free(map, id);
			}
			return true;
		}
		ref = (uint32_t)atomic_load32(&map->map[idx].ref, memory_order_acquire);
		refcount = ref & OBJECTMAP_INDEXMASK;
		reftag = ref >> OBJECTMAP_INDEXBITS;
	}
	return false;
}
