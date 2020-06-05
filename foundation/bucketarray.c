/* bucketarray.c  -  Foundation library  -  Public Domain  -  2020 Mattias Jansson
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

void
bucketarray_initialize(bucketarray_t* array, size_t element_size, size_t bucket_element_count) {
	size_t bucket_shift = 4;
	size_t bucket_element_count_power_of_two = ((size_t)1 << bucket_shift);
	while (bucket_element_count_power_of_two < bucket_element_count) {
		bucket_element_count_power_of_two <<= 1;
		++bucket_shift;
	}

	array->bucket = nullptr;
	array->element_size = element_size;
	array->bucket_mask = ((size_t)1 << bucket_shift) - 1;
	array->bucket_shift = bucket_shift;
	array->bucket_count = 0;
	array->count = 0;
}

void
bucketarray_finalize(bucketarray_t* array) {
	for (size_t ibucket = 0; ibucket < array->bucket_count; ++ibucket)
		memory_deallocate(array->bucket[ibucket]);
	memory_deallocate(array->bucket);
}

void
bucketarray_reserve(bucketarray_t* array, size_t count) {
	if ((array->bucket_count << array->bucket_shift) >= count)
		return;

	size_t bucket_count = (count + array->bucket_mask) >> array->bucket_shift;
	array->bucket = memory_reallocate(array->bucket, sizeof(void*) * bucket_count, 0,
	                                  sizeof(void*) * array->bucket_count, MEMORY_PERSISTENT);

	size_t bucket_size = array->element_size << array->bucket_shift;
	for (size_t ibucket = array->bucket_count; ibucket < bucket_count; ++ibucket)
		array->bucket[ibucket] = memory_allocate(0, bucket_size, 0, MEMORY_PERSISTENT);

	array->bucket_count = bucket_count;
}

void
bucketarray_resize(bucketarray_t* array, size_t count) {
	bucketarray_reserve(array, count);
	array->count = count;
}

void
bucketarray_resize_fill(bucketarray_t* array, size_t count, uint8_t fill) {
	bucketarray_reserve(array, count);

	if (count > array->count) {
		size_t start_bucket_idx = array->count >> array->bucket_shift;
		size_t end_bucket_idx = count >> array->bucket_shift;
		size_t bucket_element_count = ((size_t)1 << array->bucket_shift);

		size_t bucket_index = start_bucket_idx;
		size_t index = array->count & array->bucket_mask;
		while (bucket_index < end_bucket_idx) {
			memset(pointer_offset(array->bucket[bucket_index], index), (int)fill,
			       (bucket_element_count - index) * array->element_size);
			index = 0;
			++bucket_index;
		}

		size_t last_bucket_count = count & array->bucket_mask;
		if (index < last_bucket_count)
			memset(pointer_offset(array->bucket[bucket_index], index), (int)fill,
			       (last_bucket_count - index) * array->element_size);
	}

	array->count = count;
}

void
bucketarray_clear(bucketarray_t* array) {
	array->count = 0;
}

void
bucketarray_push(bucketarray_t* array, void* element) {
	size_t bucket_idx = array->count >> array->bucket_shift;
	if (bucket_idx >= array->bucket_count)
		bucketarray_reserve(array, array->count + array->bucket_mask);

	size_t index = array->count & array->bucket_mask;
	memcpy(pointer_offset(array->bucket[bucket_idx], array->element_size * index), element, array->element_size);
	++array->count;
}

void
bucketarray_erase(bucketarray_t* array, size_t element) {
	size_t last_element = array->count - 1;
	if (element < last_element) {
		size_t to_bucket = element >> array->bucket_shift;
		size_t to_index = element & array->bucket_mask;

		size_t from_bucket = last_element >> array->bucket_shift;
		size_t from_index = last_element & array->bucket_mask;

		memcpy(pointer_offset(array->bucket[to_bucket], array->element_size * to_index),
		       pointer_offset(array->bucket[from_bucket], array->element_size * from_index), array->element_size);
	}
	--array->count;
}

void*
bucketarray_get(bucketarray_t* array, size_t index) {
	size_t bucket_idx = index >> array->bucket_shift;
	size_t element_index = index & array->bucket_mask;
	return pointer_offset(array->bucket[bucket_idx], array->element_size * element_index);
}

void
bucketarray_copy(bucketarray_t* array, void* destination) {
	void* current_destination = destination;
	size_t bucket_count = array->count >> array->bucket_shift;
	size_t bucket_size = array->element_size << array->bucket_shift;
	for (size_t ibucket = 0; ibucket < bucket_count; ++ibucket) {
		memcpy(current_destination, array->bucket[ibucket], bucket_size);
		current_destination = pointer_offset(current_destination, bucket_size);
	}
	size_t element_count = array->count & array->bucket_mask;
	if (element_count)
		memcpy(current_destination, array->bucket[bucket_count], array->element_size * element_count);
}
