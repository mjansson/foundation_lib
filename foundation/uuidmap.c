/* uuidmap.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#define UUIDMAP_MINBUCKETS 13

#define GET_BUCKET(map, key) ((size_t)((key.word[0] ^ key.word[1]) % (uint64_t)map->bucket_count))

uuidmap_t*
uuidmap_allocate(size_t bucket_count, size_t bucket_size) {
	uuidmap_t* map;

	if (bucket_count < UUIDMAP_MINBUCKETS)
		bucket_count = UUIDMAP_MINBUCKETS;

	map = memory_allocate(0, sizeof(uuidmap_t) + sizeof(uuidmap_node_t*) * bucket_count, 0, MEMORY_PERSISTENT);

	uuidmap_initialize(map, bucket_count, bucket_size);

	return map;
}

void
uuidmap_initialize(uuidmap_t* map, size_t bucket_count, size_t bucket_size) {
	size_t ibucket;

	map->bucket_count = bucket_count;
	map->node_count = 0;

	for (ibucket = 0; ibucket < bucket_count; ++ibucket) {
		map->bucket[ibucket] = 0;
		if (bucket_size)
			array_reserve(map->bucket[ibucket], bucket_size);
	}
}

void
uuidmap_deallocate(uuidmap_t* map) {
	uuidmap_finalize(map);
	memory_deallocate(map);
}

void
uuidmap_finalize(uuidmap_t* map) {
	size_t ibucket;
	for (ibucket = 0; ibucket < map->bucket_count; ++ibucket)
		array_deallocate(map->bucket[ibucket]);
}

void*
uuidmap_insert(uuidmap_t* map, uuid_t key, void* value) {
	/*lint --e{613} */
	size_t ibucket = GET_BUCKET(map, key);
	uuidmap_node_t* bucket = map->bucket[ibucket];
	size_t inode, nsize;
	for (inode = 0, nsize = array_size(bucket); inode < nsize; ++inode) {
		if (uuid_equal(bucket[inode].key, key)) {
			void* prev = bucket[inode].value;
			bucket[inode].value = value;
			return prev;
		}
	}
	{
		uuidmap_node_t node = {key, value};
		array_push(map->bucket[ibucket], node);
		++map->node_count;
	}
	return 0;
}

void*
uuidmap_erase(uuidmap_t* map, uuid_t key) {
	/*lint --e{613} */
	size_t ibucket = GET_BUCKET(map, key);
	uuidmap_node_t* bucket = map->bucket[ibucket];
	size_t inode, nsize;
	for (inode = 0, nsize = array_size(bucket); inode < nsize; ++inode) {
		if (uuid_equal(bucket[inode].key, key)) {
			void* prev = bucket[inode].value;
			array_erase(map->bucket[ibucket], inode);
			--map->node_count;
			return prev;
		}
	}
	return 0;
}

void*
uuidmap_lookup(uuidmap_t* map, uuid_t key) {
	/*lint --e{613} */
	size_t ibucket = GET_BUCKET(map, key);
	uuidmap_node_t* bucket = map->bucket[ibucket];
	size_t inode, nsize;
	for (inode = 0, nsize = array_size(bucket); inode < nsize; ++inode) {
		if (uuid_equal(bucket[inode].key, key))
			return bucket[inode].value;
	}
	return 0;
}

bool
uuidmap_has_key(uuidmap_t* map, uuid_t key) {
	/*lint --e{613} */
	size_t ibucket = GET_BUCKET(map, key);
	uuidmap_node_t* bucket = map->bucket[ibucket];
	size_t inode, nsize;
	for (inode = 0, nsize = array_size(bucket); inode < nsize; ++inode) {
		if (uuid_equal(bucket[inode].key, key))
			return true;
	}
	return false;
}

size_t
uuidmap_size(uuidmap_t* map) {
	return map->node_count;
}

void
uuidmap_clear(uuidmap_t* map) {
	size_t ibucket;
	for (ibucket = 0; ibucket < map->bucket_count; ++ibucket)
		array_clear(map->bucket[ibucket]);
	map->node_count = 0;
}

void
uuidmap_foreach(uuidmap_t* map, void (*fn)(void*, void*), void* context) {
	for (size_t ibucket = 0; ibucket < map->bucket_count; ++ibucket) {
		uuidmap_node_t* bucket = map->bucket[ibucket];
		for (size_t inode = 0, nsize = array_size(bucket); inode < nsize; ++inode)
			fn(bucket[inode].value, context);
	}
}
