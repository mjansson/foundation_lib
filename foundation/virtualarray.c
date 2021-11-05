/* virtualarray.c  -  Foundation library  -  Public Domain  -  2020 Mattias Jansson
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

#include "virtualarray.h"
#include "memory.h"
#include "windows.h"

virtualarray_t*
virtualarray_allocate(uint element_size, uint capacity) {
	virtualarray_t* array = memory_allocate(0, sizeof(virtualarray_t), 0, MEMORY_PERSISTENT);
	array->capacity = capacity;
	array->count = 0;
	array->element_size = element_size;
	array->flags = 0;
	array->storage = 0;
	return array;
}

void
virtualarray_initialize(virtualarray_t* array, size_t element_size, size_t capacity) {
	array->capacity = capacity;
	array->count = 0;
	array->element_size = (uint)element_size;
	array->flags = 0;
	array->storage = 0;
}

void
virtualarray_initialize_copy(virtualarray_t* array, virtualarray_t* source) {
	virtualarray_initialize(array, source->element_size, source->capacity);
	virtualarray_resize(array, source->count);
	memcpy(array->storage, source->storage, array->element_size * array->count);
}

void
virtualarray_finalize(virtualarray_t* array) {
	virtualarray_clear_and_free(array);
}

void
virtualarray_deallocate(virtualarray_t* array) {
	virtualarray_finalize(array);
	memory_deallocate(array);
}

void
virtualarray_clear(virtualarray_t* array) {
	array->count = 0;
}

static size_t page_size;

static void*
virtualarray_allocate_storage(uint element_size, size_t* capacity, uint* flags) {
#if FOUNDATION_PLATFORM_WINDOWS
	if (!page_size) {
		SYSTEM_INFO system_info;
		memset(&system_info, 0, sizeof(system_info));
		GetSystemInfo(&system_info);
		page_size = system_info.dwPageSize;
	}
#else
	if (!page_size)
		page_size = sysconf(_SC_PAGESIZE);
#endif

	size_t virtual_threshold = 4 * 4096;
	size_t size_needed = (*capacity) * element_size;
	if (size_needed < virtual_threshold) {
		*flags = VIRTUALARRAY_MEMORY_ALLOCATED;
		return memory_allocate(0, size_needed, 0, MEMORY_PERSISTENT);
	}

	*flags = 0;
	size_needed *= 2;
	size_t num_pages = size_needed / page_size;
	if (size_needed & (page_size - 1))
		++num_pages;
	size_needed = num_pages * page_size;
	*capacity = (size_needed / element_size);
#if FOUNDATION_PLATFORM_WINDOWS
	return VirtualAlloc(0, size_needed, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
#else
	return mmap(0, size_needed, PROT_READ | PROT_WRITE, flags, -1, 0);
#endif
}

static void
virtualarray_free_storage(uint flags, size_t size, void* storage) {
	if (flags & VIRTUALARRAY_MEMORY_ALLOCATED) {
		memory_deallocate(storage);
		return;
	}

#if FOUNDATION_PLATFORM_WINDOWS
	VirtualFree(storage, 0, MEM_RELEASE);
#else
	munmap(storage, size);
#endif
}

void
virtualarray_clear_and_free(virtualarray_t* array) {
	size_t size_allocated = array->capacity * array->element_size;
	virtualarray_free_storage(array->flags, size_allocated, array->storage);
	array->storage = 0;
	array->count = 0;
	array->flags = 0;
}

void*
virtualarray_resize(virtualarray_t* array, size_t count) {
	if (count < array->capacity) {
		if (!array->storage)
			array->storage = virtualarray_allocate_storage(array->element_size, &array->capacity, &array->flags);
		array->count = count;
		return array->storage;
	}

	uint new_flags = 0;
	size_t new_capacity = ((array->capacity * 2) > count) ? (array->capacity * 2) : (count * 2);
	void* new_storage = virtualarray_allocate_storage(array->element_size, &new_capacity, &new_flags);
	memcpy(new_storage, array->storage, array->element_size * array->count);
	size_t size_allocated = array->capacity * array->element_size;
	virtualarray_free_storage(array->flags, size_allocated, array->storage);
	array->capacity = new_capacity;
	array->count = count;
	array->storage = new_storage;
	array->flags = new_flags;

	return array->storage;
}

void*
virtualarray_resize_zero(virtualarray_t* array, size_t count) {
	return virtualarray_resize_fill(array, count, 0);
}

void*
virtualarray_resize_fill(virtualarray_t* array, size_t count, uint8_t pattern) {
	size_t old_count = array->count;
	virtualarray_resize(array, count);
	if (count > old_count)
		memset(pointer_offset(array->storage, old_count * array->element_size), (int)pattern,
		       (count - old_count) * array->element_size);
	return array->storage;
}

void
virtualarray_push_raw(virtualarray_t* array, void* element) {
	if (!array->storage || (array->count == array->capacity))
		virtualarray_resize(array, array->count);
	memcpy(pointer_offset(array->storage, array->count * array->element_size), element, array->element_size);
	++array->count;
}

void*
virtualarray_get_verified_type(virtualarray_t* array, size_t element_size) {
	FOUNDATION_ASSERT_MSG(array->element_size == element_size, "Access virtual array using bad type");
	return array->storage;
}
