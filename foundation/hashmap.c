/* hashmap.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#define HASHMAP_MINBUCKETS 13

#define GET_BUCKET(map, key) (key % map->bucket_count)

hashmap_t*
hashmap_allocate(size_t bucket_count, size_t bucket_size) {
	hashmap_t* map;

	if (bucket_count < HASHMAP_MINBUCKETS)
		bucket_count = HASHMAP_MINBUCKETS;

	map = memory_allocate(0, sizeof(hashmap_t) + sizeof(hashmap_node_t*) * bucket_count, 0, MEMORY_PERSISTENT);

	hashmap_initialize(map, bucket_count, bucket_size);

	return map;
}

void
hashmap_initialize(hashmap_t* map, size_t bucket_count, size_t bucket_size) {
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
hashmap_deallocate(hashmap_t* map) {
	hashmap_finalize(map);
	memory_deallocate(map);
}

void
hashmap_finalize(hashmap_t* map) {
	size_t ibucket;
	for (ibucket = 0; ibucket < map->bucket_count; ++ibucket)
		array_deallocate(map->bucket[ibucket]);
}

void*
hashmap_insert(hashmap_t* map, hash_t key, void* value) {
	/*lint --e{613} */
	size_t ibucket = GET_BUCKET(map, key);
	hashmap_node_t* bucket = map->bucket[ibucket];
	size_t inode, nsize;
	for (inode = 0, nsize = array_size(bucket); inode < nsize; ++inode) {
		if (bucket[inode].key == key) {
			void* prev = bucket[inode].value;
			bucket[inode].value = value;
			return prev;
		}
	}
	{
		hashmap_node_t node = {key, value};
		array_push(map->bucket[ibucket], node);
		++map->node_count;
	}
	return 0;
}

void*
hashmap_erase(hashmap_t* map, hash_t key) {
	/*lint --e{613} */
	size_t ibucket = GET_BUCKET(map, key);
	hashmap_node_t* bucket = map->bucket[ibucket];
	size_t inode, nsize;
	for (inode = 0, nsize = array_size(bucket); inode < nsize; ++inode) {
		if (bucket[inode].key == key) {
			void* prev = bucket[inode].value;
			array_erase(map->bucket[ibucket], inode);
			--map->node_count;
			return prev;
		}
	}
	return 0;
}

void*
hashmap_lookup(hashmap_t* map, hash_t key) {
	/*lint --e{613} */
	size_t ibucket = GET_BUCKET(map, key);
	hashmap_node_t* bucket = map->bucket[ibucket];
	size_t inode, nsize;
	for (inode = 0, nsize = array_size(bucket); inode < nsize; ++inode) {
		if (bucket[inode].key == key)
			return bucket[inode].value;
	}
	return 0;
}

bool
hashmap_has_key(hashmap_t* map, hash_t key) {
	/*lint --e{613} */
	size_t ibucket = GET_BUCKET(map, key);
	hashmap_node_t* bucket = map->bucket[ibucket];
	size_t inode, nsize;
	for (inode = 0, nsize = array_size(bucket); inode < nsize; ++inode) {
		if (bucket[inode].key == key)
			return true;
	}
	return false;
}

size_t
hashmap_size(hashmap_t* map) {
	return map->node_count;
}

void
hashmap_clear(hashmap_t* map) {
	size_t ibucket;
	for (ibucket = 0; ibucket < map->bucket_count; ++ibucket)
		array_clear(map->bucket[ibucket]);
	map->node_count = 0;
}

void
hashmap_foreach(hashmap_t* map, void (*fn)(void*, void*), void* context) {
	for (size_t ibucket = 0; ibucket < map->bucket_count; ++ibucket) {
		hashmap_node_t* bucket = map->bucket[ibucket];
		for (size_t inode = 0, nsize = array_size(bucket); inode < nsize; ++inode)
			fn(bucket[inode].value, context);
	}
}
