/* hashmap.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 * 
 * This library provides a cross-platform foundation library in C11 providing basic support data types and
 * functions to write applications and games in a platform-independent fashion. The latest source code is
 * always available at
 * 
 * https://github.com/rampantpixels/foundation_lib
 * 
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#include <foundation/foundation.h>


typedef struct _foundation_hashmap_node
{
	hash_t                key;
	void*                 value;
} hashmap_node_t;


typedef hashmap_node_t*   hashmap_bucket_t;


struct _foundation_hashmap
{
	unsigned int          num_buckets;
	unsigned int          num_nodes;
	hashmap_bucket_t      bucket[];
};
	

hashmap_t* hashmap_allocate( unsigned int buckets, unsigned int bucketsize )
{
	hashmap_t* map;
	unsigned int ibucket;

	if( buckets < HASHMAP_MINBUCKETS )
		buckets = HASHMAP_MINBUCKETS;
	if( bucketsize < HASHMAP_MINBUCKETSIZE )
		bucketsize = HASHMAP_MINBUCKETSIZE;
	
	map = memory_allocate( sizeof( hashmap_t ) + sizeof( hashmap_bucket_t ) * buckets, 0, MEMORY_PERSISTENT );

	map->num_buckets = buckets;
	map->num_nodes   = 0;

	for( ibucket = 0; ibucket < buckets; ++ibucket )
	{
		map->bucket[ibucket] = 0;
		array_reserve( map->bucket[ibucket], (int)bucketsize );
	}
	
	return map;
}


void hashmap_deallocate( hashmap_t* map )
{
	unsigned int ibucket;
	for( ibucket = 0; ibucket < map->num_buckets; ++ibucket )
		array_deallocate( map->bucket[ibucket] );
	memory_deallocate( map );
}


void* hashmap_insert( hashmap_t* map, hash_t key, void* value )
{
	unsigned int ibucket = key % map->num_buckets;
	hashmap_bucket_t bucket = map->bucket[ibucket];
	unsigned int inode, nsize;
	for( inode = 0, nsize = array_size( bucket ); inode < nsize; ++inode )
	{
		if( bucket[inode].key == key )
		{
			void* prev = bucket[inode].value;
			bucket[inode].value = value;
			return prev;
		}
	}
	{
		hashmap_node_t node = { key, value };
		array_push( map->bucket[ibucket], node );
		++map->num_nodes;
	}
	return 0;
}


void* hashmap_erase( hashmap_t* map, hash_t key )
{
	unsigned int ibucket = key % map->num_buckets;
	hashmap_bucket_t bucket = map->bucket[ibucket];
	unsigned int inode, nsize;
	for( inode = 0, nsize = array_size( bucket ); inode < nsize; ++inode )
	{
		if( bucket[inode].key == key )
		{
			void* prev = bucket[inode].value;
			array_erase( map->bucket[ibucket], inode );
			--map->num_nodes;
			return prev;
		}
	}
	return 0;
}


void* hashmap_lookup( hashmap_t* map, hash_t key )
{
	unsigned int ibucket = key % map->num_buckets;
	hashmap_bucket_t bucket = map->bucket[ibucket];
	unsigned int inode, nsize;
	for( inode = 0, nsize = array_size( bucket ); inode < nsize; ++inode )
	{
		if( bucket[inode].key == key )
			return bucket[inode].value;
	}
	return 0;
}


bool hashmap_has_key( hashmap_t* map, hash_t key )
{
	unsigned int ibucket = key % map->num_buckets;
	hashmap_bucket_t bucket = map->bucket[ibucket];
	unsigned int inode, nsize;
	for( inode = 0, nsize = array_size( bucket ); inode < nsize; ++inode )
	{
		if( bucket[inode].key == key )
			return true;
	}
	return false;
}


unsigned int hashmap_size( hashmap_t* map )
{
	return map->num_nodes;
}


void hashmap_clear( hashmap_t* map )
{
	unsigned int ibucket;
	for( ibucket = 0; ibucket < map->num_buckets; ++ibucket )
		array_clear( map->bucket[ibucket] );
	map->num_nodes = 0;
}
