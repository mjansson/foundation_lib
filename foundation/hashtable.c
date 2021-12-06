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

#include <foundation/foundation.h>
#include <foundation/internal.h>

static FOUNDATION_FORCEINLINE uint32_t
hashtable32_hash(uint32_t key) {
	key ^= key >> 16;
	key *= 0x85ebca6b;
	key ^= key >> 13;
	key *= 0xc2b2ae35;
	key ^= key >> 16;
	return key;
}

static FOUNDATION_FORCEINLINE uint64_t
hashtable64_hash(uint64_t key) {
	key ^= key >> 33;
	key *= 0xff51afd7ed558ccd;
	key ^= key >> 33;
	key *= 0xc4ceb9fe1a85ec53;
	key ^= key >> 33;
	return key;
}

hashtable32_t*
hashtable32_allocate(size_t bucket_count) {
	size_t size = sizeof(hashtable32_t) + sizeof(hashtable32_entry_t) * bucket_count;
	hashtable32_t* table = memory_allocate(0, size, 8, MEMORY_PERSISTENT);

	hashtable32_initialize(table, bucket_count);

	return table;
}

void
hashtable32_initialize(hashtable32_t* table, size_t bucket_count) {
	memset(table, 0, sizeof(hashtable32_t) + sizeof(hashtable32_entry_t) * bucket_count);
	table->capacity = bucket_count;
}

void
hashtable32_deallocate(hashtable32_t* table) {
	hashtable32_finalize(table);
	memory_deallocate(table);
}

void
hashtable32_finalize(hashtable32_t* table) {
	FOUNDATION_UNUSED(table);
}

bool
hashtable32_set(hashtable32_t* table, uint32_t key, uint32_t value) {
	/*lint --e{613} */
	size_t ie, eend;

	FOUNDATION_ASSERT(key);

	ie = eend = hashtable32_hash(key) % table->capacity;
	do {
		uint32_t current_key = (uint32_t)atomic_load32(&table->entries[ie].key, memory_order_consume);

		if ((current_key == key) || (!current_key && atomic_cas32(&table->entries[ie].key, (int32_t)key, 0,
		                                                          memory_order_release, memory_order_acquire))) {
			table->entries[ie].value = value;
			return true;
		}

		ie = (ie + 1) % table->capacity;
	} while (ie != eend);

	return false;
}

void
hashtable32_erase(hashtable32_t* table, uint32_t key) {
	/*lint --e{613} */
	size_t ie, eend;
	uint32_t current_key;

	FOUNDATION_ASSERT(key);

	ie = eend = hashtable32_hash(key) % table->capacity;
	do {
		current_key = (uint32_t)atomic_load32(&table->entries[ie].key, memory_order_consume);

		if (current_key == key) {
			table->entries[ie].value = 0;
			return;
		}

		ie = (ie + 1) % table->capacity;
	} while (current_key && (ie != eend));
}

uint32_t
hashtable32_get(hashtable32_t* table, uint32_t key) {
	/*lint --e{613} */
	size_t ie, eend;
	uint32_t current_key;

	FOUNDATION_ASSERT(key);

	ie = eend = hashtable32_hash(key) % table->capacity;
	do {
		current_key = (uint32_t)atomic_load32(&table->entries[ie].key, memory_order_consume);

		if (current_key == key)
			return table->entries[ie].value;

		ie = (ie + 1) % table->capacity;
	} while (current_key && (ie != eend));

	return 0;
}

uint32_t
hashtable32_raw(hashtable32_t* table, size_t slot) {
	if (!atomic_load32(&table->entries[slot].key, memory_order_acquire))
		return 0;
	return table->entries[slot].value;
}

size_t
hashtable32_size(hashtable32_t* table) {
	size_t count = 0;
	size_t ie;
	for (ie = 0; ie < table->capacity; ++ie) {
		if (atomic_load32(&table->entries[ie].key, memory_order_consume) && table->entries[ie].value)
			++count;
	}
	return count;
}

void
hashtable32_clear(hashtable32_t* table) {
	memset(table->entries, 0, sizeof(hashtable32_entry_t) * table->capacity);
}

hashtable64_t*
hashtable64_allocate(size_t bucket_count) {
	size_t size = sizeof(hashtable64_t) + sizeof(hashtable64_entry_t) * bucket_count;
	hashtable64_t* table = memory_allocate(0, size, 8, MEMORY_PERSISTENT);

	hashtable64_initialize(table, bucket_count);

	return table;
}

void
hashtable64_initialize(hashtable64_t* table, size_t bucket_count) {
	memset(table, 0, sizeof(hashtable64_t) + sizeof(hashtable64_entry_t) * bucket_count);
	table->capacity = bucket_count;
}

void
hashtable64_deallocate(hashtable64_t* table) {
	hashtable64_finalize(table);
	memory_deallocate(table);
}

void
hashtable64_finalize(hashtable64_t* table) {
	FOUNDATION_UNUSED(table);
}

bool
hashtable64_set(hashtable64_t* table, uint64_t key, uint64_t value) {
	size_t ie, eend;

	FOUNDATION_ASSERT(key);

	ie = eend = hashtable64_hash(key) % table->capacity;
	do {
		uint64_t current_key = (uint64_t)atomic_load64(&table->entries[ie].key, memory_order_consume);

		if ((current_key == key) || (!current_key && atomic_cas64(&table->entries[ie].key, (int64_t)key, 0,
		                                                          memory_order_release, memory_order_acquire))) {
			table->entries[ie].value = value;
			return true;
		}

		ie = (ie + 1) % table->capacity;
	} while (ie != eend);

	return false;
}

void
hashtable64_erase(hashtable64_t* table, uint64_t key) {
	size_t ie, eend;
	uint64_t current_key;

	FOUNDATION_ASSERT(key);

	ie = eend = hashtable64_hash(key) % table->capacity;
	do {
		current_key = (uint64_t)atomic_load64(&table->entries[ie].key, memory_order_consume);

		if (current_key == key) {
			table->entries[ie].value = 0;
			return;
		}

		ie = (ie + 1) % table->capacity;
	} while (current_key && (ie != eend));
}

uint64_t
hashtable64_get(hashtable64_t* table, uint64_t key) {
	size_t ie, eend;
	uint64_t current_key;

	FOUNDATION_ASSERT(key);

	ie = eend = hashtable64_hash(key) % table->capacity;
	do {
		current_key = (uint64_t)atomic_load64(&table->entries[ie].key, memory_order_consume);

		if (current_key == key)
			return table->entries[ie].value;

		ie = (ie + 1) % table->capacity;
	} while (current_key && (ie != eend));

	return 0;
}

uint64_t
hashtable64_raw(hashtable64_t* table, size_t slot) {
	if (!atomic_load64(&table->entries[slot].key, memory_order_acquire))
		return 0;
	return table->entries[slot].value;
}

size_t
hashtable64_size(hashtable64_t* table) {
	size_t count = 0;
	size_t ie;
	for (ie = 0; ie < table->capacity; ++ie) {
		if (atomic_load64(&table->entries[ie].key, memory_order_consume) && table->entries[ie].value)
			++count;
	}
	return count;
}

void
hashtable64_clear(hashtable64_t* table) {
	memset(table->entries, 0, sizeof(hashtable64_entry_t) * table->capacity);
}
