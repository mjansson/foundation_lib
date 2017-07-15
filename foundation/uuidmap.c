/* uuidmap.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#define UUIDMAP_MINBUCKETS     13

#define GET_BUCKET(map, key) ((key.word[0] ^ key.word[1]) % (uint64_t)map->num_buckets)

uuidmap_t*
uuidmap_allocate(size_t buckets, size_t bucketsize) {
	uuidmap_t* map;

	if (buckets < UUIDMAP_MINBUCKETS)
		buckets = UUIDMAP_MINBUCKETS;

	map = memory_allocate(0, sizeof(uuidmap_t) + sizeof(uuidmap_node_t*) * buckets, 0,
	                      MEMORY_PERSISTENT);

	uuidmap_initialize(map, buckets, bucketsize);

	return map;
}

void
uuidmap_initialize(uuidmap_t* map, size_t buckets, size_t bucketsize) {
	size_t ibucket;

	map->num_buckets = buckets;
	map->num_nodes   = 0;

	for (ibucket = 0; ibucket < buckets; ++ibucket) {
		map->bucket[ibucket] = 0;
		if (bucketsize)
			array_reserve(map->bucket[ibucket], bucketsize);
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
	for (ibucket = 0; ibucket < map->num_buckets; ++ibucket)
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
		uuidmap_node_t node = { key, value };
		array_push(map->bucket[ibucket], node);
		++map->num_nodes;
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
			--map->num_nodes;
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
	return map->num_nodes;
}

void
uuidmap_clear(uuidmap_t* map) {
	size_t ibucket;
	for (ibucket = 0; ibucket < map->num_buckets; ++ibucket)
		array_clear(map->bucket[ibucket]);
	map->num_nodes = 0;
}
