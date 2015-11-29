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

FOUNDATION_STATIC_ASSERT(FOUNDATION_ALIGNOF(object_base_t) >= 8, "object_base_t alignment");
FOUNDATION_STATIC_ASSERT(FOUNDATION_ALIGNOF(objectmap_t) >= 8, "objectmap_t alignment");

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

	map = memory_allocate(0, sizeof(objectmap_t) + (sizeof(void*) * size), 16, MEMORY_PERSISTENT);

	objectmap_initialize(map, size);

	return map;
}

void
objectmap_initialize(objectmap_t* map, size_t size) {
	unsigned int bits;
	unsigned int ip;
	uintptr_t next_indexshift;
	void** slot;

	//Number of bits needed to represent index
	bits = (unsigned int)math_round(math_log2((real)size));
	FOUNDATION_ASSERT_MSGFORMAT(bits < 50, "Invalid objectmap size %" PRIsize, size);

	memset(map, 0, sizeof(objectmap_t) + (sizeof(void*) * size));

	//Top two bits unused for Lua compatibility
	map->size_bits   = bits;
	map->id_max      = ((1ULL << (62ULL - bits)) - 1);
	map->size        = size;
	map->mask_index  = ((1ULL << bits) - 1ULL);
	map->mask_id     = (0x3FFFFFFFFFFFFFFFULL & ~map->mask_index);
	atomic_store64(&map->free, 0);
	atomic_store64(&map->id, 1);

	slot = map->map;
	for (ip = 0, next_indexshift = 3; ip < (size - 1); ++ip, next_indexshift += 2, ++slot)
		*slot = (void*)next_indexshift;
	*slot = (void*)((uintptr_t)-1);
}

void
objectmap_deallocate(objectmap_t* map) {
	objectmap_finalize(map);
	memory_deallocate(map);
}

void
objectmap_finalize(objectmap_t* map) {
	size_t i;

	if (!map)
		return;

	for (i = 0; i < map->size; ++i) {
		bool is_object = !((uintptr_t)map->map[i] & 1);
		if (is_object) {
			log_error(0, ERROR_MEMORY_LEAK,
			          STRING_CONST("Object still stored in objectmap when map deallocated"));
			break;
		}
	}
}

size_t
objectmap_size(const objectmap_t* map) {
	return (unsigned int)map->size;
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
	uint64_t raw, idx;
	uint64_t next, id;

	//Reserve spot in array, using tag for ABA protection
	uint64_t tag = (uint64_t)atomic_incr64(&map->id);
	//TODO: Look into double-ended implementation with allocation from tail and free push to head
	do {
		raw = (uint64_t)atomic_load64(&map->free);
		idx = raw & map->mask_index;
		if (idx >= map->size) {
			log_error(0, ERROR_OUT_OF_MEMORY, STRING_CONST("Map full, unable to reserve id"));
			return 0;
		}
		next = (uintptr_t)map->map[idx] >> 1;
		next = (next & map->mask_index) | (tag << map->size_bits);
	}
	while (!atomic_cas64(&map->free, (int64_t)next, (int64_t)raw));

	//Sanity check that slot isn't taken
	FOUNDATION_ASSERT_MSG((uintptr_t)map->map[idx] & 1,
	                      "Map failed sanity check, slot taken after reserve");
	map->map[idx] = 0;

	//Allocate ID
	id = tag & map->id_max;
	while (!id)
		id = (uint64_t)atomic_incr64(&map->id) & map->id_max; //Wrap-around handled by masking

	//Make sure id stays within correct bits (if fails, check objectmap allocation and the mask setup there)
	FOUNDATION_ASSERT(((id << map->size_bits) & map->mask_id) == (id << map->size_bits));

	return (id << map->size_bits) | idx;
}

bool
objectmap_free(objectmap_t* map, object_t id) {
	uint64_t idx;
	uint64_t raw, next, free;
	void* object;

	idx = id & map->mask_index;
	if ((uintptr_t)map->map[idx] & 1)
		return false; //Already free

	object = map->map[idx];
	if (!FOUNDATION_VALIDATE((((object_base_t*)object)->id & map->mask_id) == (id & map->mask_id)))
		return false;

	free = idx | ((uint64_t)atomic_incr64(&map->id) << map->size_bits);
	do {
		raw = (uint64_t)atomic_load64(&map->free);
		next = (raw & map->mask_index) | ((uint64_t)atomic_incr64(&map->id) << map->size_bits);
		map->map[idx] = (void*)((uintptr_t)(next << 1) | 1);
	}
	while (!atomic_cas64(&map->free, (int64_t)free, (int64_t)raw));

	return true;
}

bool
objectmap_set(objectmap_t* map, object_t id, void* object) {
	size_t idx;

	idx = (size_t)(id & map->mask_index);

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

