/* objectmap.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

void
_object_initialize(object_base_t* obj, object_t id) {
	obj->id = id;
	atomic_store32(&obj->ref, 1);
}

objectmap_t*
objectmap_allocate(size_t size) {
	objectmap_t* map;

	if (size < 3)
		size = 3;

	map = memory_allocate(0, sizeof(objectmap_t) + (sizeof(void*) * size), 0, MEMORY_PERSISTENT);

	objectmap_initialize(map, size);

	return map;
}

void
objectmap_initialize(objectmap_t* map, size_t size) {
	//Number of bits needed to represent index
	unsigned int bits = (unsigned int)math_round(math_log2((real)size));
	FOUNDATION_ASSERT_MSGFORMAT(bits <= 26, "Invalid objectmap size %" PRIsize, size);

	//Needed for object check in objectmap_lookup of unused index
	memset(map, 0, sizeof(objectmap_t) + (sizeof(void*) * size));

	map->free        = 0;
	map->id          = 0;
	map->index_bits  = bits;
	map->id_max      = ((1U << (32U - bits)) - 1);
	map->size        = (uint32_t)size;
	map->mask_index  = ((1U << bits) - 1U);
	map->mask_id     = (0xFFFFFFFFU & ~map->mask_index);
	map->autolink    = 0;
	semaphore_initialize(&map->write, 1);
	log_info(0, STRING_CONST("Initialized objectmap"));
}

void
objectmap_deallocate(objectmap_t* map) {
	objectmap_finalize(map);
	memory_deallocate(map);
}

void
objectmap_finalize(objectmap_t* map) {
	uint32_t i;

	if (!map)
		return;

#if BUILD_DEBUG || BUILD_RELEASE
	for (i = 0; (i < map->size) && (i < map->autolink); ++i) {
		bool is_object = !((uintptr_t)map->map[i] & 1);
		if (is_object) {
			log_error(0, ERROR_MEMORY_LEAK,
			          STRING_CONST("Object still stored in objectmap when map deallocated"));
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
	uintptr_t ptr;

	/*lint --e{613} Performance path (no ptr checks)*/
	ptr = (uintptr_t)map->map[idx];
	return (ptr & 1) ? 0 : (void*)ptr;
}

object_t
objectmap_reserve(objectmap_t* map) {
	uint32_t raw, idx, tag, next;

	semaphore_wait(&map->write);
	{
		//Reserve spot in array, using tag for ABA protection
		tag = ++map->id & map->id_max;
		if (!tag)
			tag = ++map->id & map->id_max; //Wrap-around handled by masking

		raw = map->free;
		idx = raw & map->mask_index;
		if (idx >= map->size) {
			semaphore_post(&map->write);
			log_error(0, ERROR_OUT_OF_MEMORY, STRING_CONST("Map full, unable to reserve id"));
			return 0;
		}
		if (idx >= map->autolink) {
			next = ++map->autolink;
		}
		else {
			next = (uint32_t)((uintptr_t)map->map[idx] >> 1) & map->mask_index;
			//Sanity check that slot isn't taken
			FOUNDATION_ASSERT_MSG((uintptr_t)map->map[idx] & 1,
			                      "Map failed sanity check, slot taken after reserve");
		}
		map->free = next | (tag << map->index_bits);
		map->map[idx] = 0;
	}
	semaphore_post(&map->write);

	//Make sure id stays within correct bits (if fails, check objectmap allocation and the mask setup there)
	FOUNDATION_ASSERT(((tag << map->index_bits) & map->mask_id) == (tag << map->index_bits));

	return (tag << map->index_bits) | idx;
}

bool
objectmap_free(objectmap_t* map, object_t id) {
	uint32_t idx, raw, next, free;
	void* object;

	semaphore_wait(&map->write);
	{
		idx = id & map->mask_index;
		object = map->map[idx];
		if (((uintptr_t)map->map[idx] & 1) ||
		    !FOUNDATION_VALIDATE((((object_base_t*)object)->id & map->mask_id) == (id & map->mask_id))) {
			semaphore_post(&map->write);
			return false;
		}

		uint32_t tag = ++map->id & map->id_max;
		if (!tag)
			tag = ++map->id & map->id_max; //Wrap-around handled by masking

		free = idx | (tag << map->index_bits);
		raw = map->free;
		next = (raw & map->mask_index) | (tag << map->index_bits);
		map->map[idx] = (void*)((uintptr_t)(next << 1) | 1);
		map->free = free;
	}
	semaphore_post(&map->write);

	return true;
}

bool
objectmap_set(objectmap_t* map, object_t id, void* object) {
	uint32_t idx;

	idx = (id & map->mask_index);

	//Sanity check, can't set free slot, and non-free slot should be initialized to 0 in reserve function
	FOUNDATION_ASSERT(!(((uintptr_t)map->map[idx]) & 1));
	if (FOUNDATION_VALIDATE(!map->map[idx])) {
		map->map[idx] = object;
		return true;
	}
	return false;
}

void*
objectmap_lookup_ref(const objectmap_t* map, object_t id) {
	void* object;
	int32_t ref;
	do {
		ref = 0;
		object = map->map[ id & map->mask_index ];
		if (object && !((uintptr_t)object & 1) &&
		        ((((object_base_t*)object)->id & map->mask_id) == (id & map->mask_id))) {
			object_base_t* base_obj = object;
			ref = atomic_load32(&base_obj->ref);
			if (ref && atomic_cas32(&base_obj->ref, ref + 1, ref))
				return object;
		}
	}
	while (ref);
	return 0;
}

bool
objectmap_lookup_unref(const objectmap_t* map, object_t id, object_deallocate_fn deallocate) {
	void* object;
	int32_t ref;
	do {
		ref = 0;
		object = map->map[ id & map->mask_index ];
		if (object && !((uintptr_t)object & 1) &&
		        ((((object_base_t*)object)->id & map->mask_id) == (id & map->mask_id))) {
			object_base_t* base_obj = object;
			ref = atomic_load32(&base_obj->ref);
			if (ref && atomic_cas32(&base_obj->ref, ref - 1, ref)) {
				if (ref == 1) {
					deallocate(id, object);
					return false;
				}
				return true;
			}
		}
	}
	while (ref);
	return false;
}

