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

#pragma once

/*! \file hashmap.h
\brief Simple container mapping hash values to pointers

Simple container mapping hash values to pointers. Access is not atomic
and therefor not thread safe. For a thread safe alternative look at hashtable.h
instead, or provide external synchronization in caller. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate new hash map with the given bucket count and size. Minimum bucket
count is 13, minimum bucket size is 8. Hash map should be deallocated with a call to
#hashmap_deallocate
\param bucket_count Bucket count
\param bucket_size Bucket size
\return New hash map */
FOUNDATION_API hashmap_t*
hashmap_allocate(size_t bucket_count, size_t bucket_size);

/*! Deallocate a hash map previously allocated with #hashmap_allocate
\param map Hash map */
FOUNDATION_API void
hashmap_deallocate(hashmap_t* map);

/*! Initialize new hash map with the given bucket count and size. Minimum bucket
count is 13, minimum bucket size is 8. Hash map should be finalized with a call to
#hashmap_finalize
\param map Hash map to initialize
\param bucket_count Bucket count
\param bucket_size Bucket size */
FOUNDATION_API void
hashmap_initialize(hashmap_t* map, size_t bucket_count, size_t bucket_size);

/*! Finalize a hash map previously initialized with #hashmap_initialize and free resources
\param map Hash map */
FOUNDATION_API void
hashmap_finalize(hashmap_t* map);

/*! Insert a new key-value mapping. Will replace any previously stored mapping for the
given key.
\param map Hash map
\param key Key
\param value Value
\return Previously stored value, 0 if no value previously stored for key */
FOUNDATION_API void*
hashmap_insert(hashmap_t* map, hash_t key, void* value);

/*! Erase any value mapping for the given key.
\param map Hash map
\param key Key
\return Previously stored value, 0 if no value previously stored for key */
FOUNDATION_API void*
hashmap_erase(hashmap_t* map, hash_t key);

/*! Lookup the stored value mapping for the given key
\param map Hash map
\param key Key
\return Stored value, 0 if no value stored for key */
FOUNDATION_API void*
hashmap_lookup(hashmap_t* map, hash_t key);

/*! Query if there is any value mapping stored for the given key.
\param map Hash map
\param key Key
\return true if there is a value mapping stored for the key, false if not */
FOUNDATION_API bool
hashmap_has_key(hashmap_t* map, hash_t key);

/*! Get the number of key-value mappings stored in the hash map.
\param map Hash map
\return Number of keys stored */
FOUNDATION_API size_t
hashmap_size(hashmap_t* map);

/*! Clear map and erase all key-value mappings.
\param map Hash map */
FOUNDATION_API void
hashmap_clear(hashmap_t* map);

/*! Call function for each value in hashmap
\param fn Function to call */
FOUNDATION_API void
hashmap_foreach(hashmap_t* map, void (*fn)(void*, void*), void* context);
