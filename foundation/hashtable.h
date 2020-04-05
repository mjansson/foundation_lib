/* hashtable.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file hashtable.h
\brief Lock-free key-value mapping container

Simple lock-free container mapping 32/64-bit keys to values. Fixed size, thread-safe.
Limitation are:
<ul>
<li>Only maps 32/64 bit integers to 32/64 bit integers
<li>All keys must be non-zero
<li>Fixed maximum number of entries
<li>Only operations are get/set
<li>No true erase operation, only set to zero
</ul>
\todo Look into a lock-free implementation of hopscotch hashing
(http://en.wikipedia.org/wiki/Hopscotch_hashing) */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate storage for a 32-bit hash table of given size. The returned hash table should
be deallocated with a call to #hashtable32_deallocate.
\param bucket_count Bucket count
\return New hash table */
FOUNDATION_API hashtable32_t*
hashtable32_allocate(size_t bucket_count);

/*! Deallocate hash table previously allocated by a call to #hashtable32_allocate and free
resources and storage used by hash table
\param table Hash table object to deallocate */
FOUNDATION_API void
hashtable32_deallocate(hashtable32_t* table);

/*! Initialize a 32-bit hash table of given size. The returned hash table should be finalized
with a call to #hashtable32_finalize.
\param table Hash table
\param bucket_count Bucket count */
FOUNDATION_API void
hashtable32_initialize(hashtable32_t* table, size_t bucket_count);

/*! Finalize hash table previously initialized by a call to #hashtable32_initialize
\param table Hash table object to finalize */
FOUNDATION_API void
hashtable32_finalize(hashtable32_t* table);

/*! Set stored value for the given key. If multiple thread are concurrently setting
values for the same key there could be multiple slots holding values for the key
when the operations finish. This only affects "size" of the table and the rate at
which the table fills, but not get/set operations (which will act on the first found
slot for the key).
\param table Hash table
\param key Key
\param value New value
\return true if value set, false if table full */
FOUNDATION_API bool
hashtable32_set(hashtable32_t* table, uint32_t key, uint32_t value);

/*! Erase the value for a key by setting the value to zero. Erasing is limited by
the key still holding a slot in the table.
\param table Hash table
\param key Key */
FOUNDATION_API void
hashtable32_erase(hashtable32_t* table, uint32_t key);

/*! Get the value stored for the given key, or zero if no value stored
\param table Hash table
\param key Key
\return Value stored for key, zero if not found */
FOUNDATION_API uint32_t
hashtable32_get(hashtable32_t* table, uint32_t key);

/*! Get number of stored keys with non-zero values. Walks the table
so potentially slow.
\param table Hash table
\return Number of keys with non-zero values */
FOUNDATION_API size_t
hashtable32_size(hashtable32_t* table);

/*! Clear the entire table, resetting the stat to the state after
initial allocation, freeing up all slots in the table.
\param table Hash table */
FOUNDATION_API void
hashtable32_clear(hashtable32_t* table);

/*! Allocate storage for a 64-bit hash table of given size. The returned hash table should
be deallocated with a call to #hashtable64_deallocate.
\param bucket_count Bucket count
\return New hash table */
FOUNDATION_API hashtable64_t*
hashtable64_allocate(size_t bucket_count);

/*! Deallocate hash table previously allocated by a call to #hashtable64_allocate and free
resources and storage used by hash table
\param table Hash table object to deallocate */
FOUNDATION_API void
hashtable64_deallocate(hashtable64_t* table);

/*! Initialize a 64-bit hash table of given size. The returned hash table should
be finalized with a call to #hashtable64_finalize.
\param table Hash table
\param bucket_count Bucket count */
FOUNDATION_API void
hashtable64_initialize(hashtable64_t* table, size_t bucket_count);

/*! Finalize hash table previously initialized by a call to #hashtable64_initialize
\param table Hash table object to finalize */
FOUNDATION_API void
hashtable64_finalize(hashtable64_t* table);

/*! Set stored value for the given key
\param table Hash table
\param key Key
\param value New value
\return true if value set, false if table full */
FOUNDATION_API bool
hashtable64_set(hashtable64_t* table, uint64_t key, uint64_t value);

/*! Erase the value for a key by setting the value to zero. Erasing is limited by
the key still holding a slot in the table.
\param table Hash table
\param key Key */
FOUNDATION_API void
hashtable64_erase(hashtable64_t* table, uint64_t key);

/*! Get the value stored for the given key, or zero if no value stored
\param table Hash table
\param key Key
\return Value stored for key, zero if not found */
FOUNDATION_API uint64_t
hashtable64_get(hashtable64_t* table, uint64_t key);

/*! Get number of stored keys with non-zero values. Walks the table
so potentially slow.
\param table Hash table
\return Number of keys with non-zero values */
FOUNDATION_API size_t
hashtable64_size(hashtable64_t* table);

/*! Clear the entire table, resetting the stat to the state after
initial allocation.
\param table Hash table */
FOUNDATION_API void
hashtable64_clear(hashtable64_t* table);

/*!
\def hashtable_t
Defined alias for a hash table storing values the size of a pointer,
so 32 or 64 bit depending on architecture.

\def hashtable_clear
Clear the entire table, resetting the stat to the state after
initial allocation.

\def hashtable_allocate
Allocate storage for a pointer sized hash table of given size

\def hashtable_deallocate
Deallocate storage used by hash table

\def hashtable_set
Set stored value for the given key

\def hashtable_erase
Erase the value for a key by setting the value to zero. The key still holds
a slot in the table.

\def hashtable_get
Get the value stored for the given key, or zero if no value stored

\def hashtable_size
Get number of stored keys with non-zero values. Walks the table
so potentially slow.

\def hashtable_clear
Clear the entire table, resetting the stat to the state after
initialization.
*/

#if FOUNDATION_SIZE_POINTER == 4

#define hashtable_t hashtable32_t
#define hashtable_allocate hashtable32_allocate
#define hashtable_initialize hashtable32_initialize
#define hashtable_finalize hashtable32_finalize
#define hashtable_deallocate hashtable32_deallocate
#define hashtable_set hashtable32_set
#define hashtable_erase hashtable32_erase
#define hashtable_get hashtable32_get
#define hashtable_size hashtable32_size
#define hashtable_clear hashtable32_clear

#else

#define hashtable_t hashtable64_t
#define hashtable_allocate hashtable64_allocate
#define hashtable_initialize hashtable64_initialize
#define hashtable_finalize hashtable64_finalize
#define hashtable_deallocate hashtable64_deallocate
#define hashtable_set hashtable64_set
#define hashtable_erase hashtable64_erase
#define hashtable_get hashtable64_get
#define hashtable_size hashtable64_size
#define hashtable_clear hashtable64_clear

#endif
