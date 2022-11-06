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

#pragma once

/*! \file uuidmap.h
\brief Simple container mapping UUID values to pointers

Simple container mapping UUID values to pointers. Access is not atomic
and therefor not thread safe. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate new UUID map with the given bucket count and size. Minimum bucket
count is 13, minimum bucket size is 8. UUID map should be deallocated with a call to
#uuidmap_deallocate
\param bucket_count Bucket count
\param bucket_size Bucket size
\return New UUID map */
FOUNDATION_API uuidmap_t*
uuidmap_allocate(size_t bucket_count, size_t bucket_size);

/*! Deallocate a UUID map previously allocated with #uuidmap_allocate
\param map UUID map */
FOUNDATION_API void
uuidmap_deallocate(uuidmap_t* map);

/*! Initialize new UUID map with the given bucket count and size. Minimum bucket
count is 13, minimum bucket size is 8. UUID map should be finalized with a call to
#uuidmap_finalize
\param map UUID map to initialize
\param bucket_count Bucket count
\param bucket_size Bucket size */
FOUNDATION_API void
uuidmap_initialize(uuidmap_t* map, size_t bucket_count, size_t bucket_size);

/*! Finalize a UUID map previously initialized with #uuidmap_initialize and free resources
\param map UUID map */
FOUNDATION_API void
uuidmap_finalize(uuidmap_t* map);

/*! Insert a new key-value mapping. Will replace any previously stored mapping for the
given key.
\param map UUID map
\param key Key
\param value Value
\return Previously stored value, 0 if no value previously stored for key */
FOUNDATION_API void*
uuidmap_insert(uuidmap_t* map, uuid_t key, void* value);

/*! Erase any value mapping for the given key.
\param map UUID map
\param key Key
\return Previously stored value, 0 if no value previously stored for key */
FOUNDATION_API void*
uuidmap_erase(uuidmap_t* map, uuid_t key);

/*! Lookup the stored value mapping for the given key
\param map UUID map
\param key Key
\return Stored value, 0 if no value stored for key */
FOUNDATION_API void*
uuidmap_lookup(uuidmap_t* map, uuid_t key);

/*! Query if there is any value mapping stored for the given key.
\param map UUID map
\param key Key
\return true if there is a value mapping stored for the key, false if not */
FOUNDATION_API bool
uuidmap_has_key(uuidmap_t* map, uuid_t key);

/*! Get the number of key-value mappings stored in the UUID map.
\param map UUID map
\return Number of keys stored */
FOUNDATION_API size_t
uuidmap_size(uuidmap_t* map);

/*! Clear map and erase all key-value mappings.
\param map UUID map */
FOUNDATION_API void
uuidmap_clear(uuidmap_t* map);

/*! Call function for each value in hashmap
\param fn Function to call */
FOUNDATION_API void
uuidmap_foreach(uuidmap_t* map, void (*fn)(void*, void*), void* context);
