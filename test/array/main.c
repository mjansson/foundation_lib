/* main.c  -  Foundation array test  -  Public Domain  -  2013 Mattias Jansson
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
#include <test/test.h>

static application_t
test_array_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation array tests"));
	app.short_name = string_const(STRING_CONST("test_array"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_array_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_array_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_array_initialize(void) {
	return 0;
}

static void
test_array_finalize(void) {
}

typedef FOUNDATION_ALIGNED_STRUCT(basic_type, 8) {
	int intval;
	float32_t floatval;
	object_t objval;
	int _padding;
}
basic_t;

typedef FOUNDATION_ALIGNED_STRUCT(combined_type, 8) {
	int intval;
	int _ipadding;
	union {
		int intval;
		real realval;
		void* ptrval;
		basic_t basicval;
		object_t objval;
	} unionval;
	char charval;
	char _cpadding[7];
	basic_t basicval;
	void* ptrval;
}
combine_t;

static int
assert_ignore(hash_t context, const char* condition, size_t cond_length, const char* file, size_t file_length,
              unsigned int line, const char* msg, size_t msg_length) {
	FOUNDATION_UNUSED(context);
	FOUNDATION_UNUSED(condition);
	FOUNDATION_UNUSED(cond_length);
	FOUNDATION_UNUSED(file);
	FOUNDATION_UNUSED(file_length);
	FOUNDATION_UNUSED(line);
	FOUNDATION_UNUSED(msg);
	FOUNDATION_UNUSED(msg_length);
	return 0;
}

DECLARE_TEST(array, allocation) {
	void** array_ptr = 0;
	int* array_int = 0;
	object_t* array_obj = 0;
	basic_t* array_basic = 0;
	combine_t* array_combine = 0;
	int array_fail[_array_header_size + 1] = {0, 0, 0, 0, 0};
	int* array_fail_ptr = array_fail + _array_header_size;

	EXPECT_EQ(array_size(array_ptr), 0);
	EXPECT_EQ(array_size(array_int), 0);
	EXPECT_EQ(array_size(array_obj), 0);
	EXPECT_EQ(array_size(array_basic), 0);
	EXPECT_EQ(array_size(array_combine), 0);

	EXPECT_EQ(array_capacity(array_ptr), 0);
	EXPECT_EQ(array_capacity(array_int), 0);
	EXPECT_EQ(array_capacity(array_obj), 0);
	EXPECT_EQ(array_capacity(array_basic), 0);
	EXPECT_EQ(array_capacity(array_combine), 0);

	// Verify invalid arrays are caught (bad watermark and size/capacity)
	assert_set_handler(assert_ignore);
	EXPECT_EQ(internal_array_verifyfn((const void* const*)&array_fail_ptr), nullptr);
	array_fail[2] = 0x52524145U;
	array_fail[1] = 32;
	EXPECT_EQ(internal_array_verifyfn((const void* const*)&array_fail_ptr), nullptr);
	array_fail[0] = 32;
	EXPECT_NE(internal_array_verifyfn((const void* const*)&array_fail_ptr), nullptr);
	assert_set_handler(0);

	// Reserve
	{
		array_reserve(array_ptr, 0);
		array_reserve(array_int, 0);
		array_reserve(array_obj, 0);
		array_reserve(array_basic, 0);
		array_reserve(array_combine, 0);

		EXPECT_EQ(array_size(array_ptr), 0);
		EXPECT_EQ(array_size(array_int), 0);
		EXPECT_EQ(array_size(array_obj), 0);
		EXPECT_EQ(array_size(array_basic), 0);
		EXPECT_EQ(array_size(array_combine), 0);

		EXPECT_EQ(array_capacity(array_ptr), 0);
		EXPECT_EQ(array_capacity(array_int), 0);
		EXPECT_EQ(array_capacity(array_obj), 0);
		EXPECT_EQ(array_capacity(array_basic), 0);
		EXPECT_EQ(array_capacity(array_combine), 0);

		EXPECT_EQ(array_ptr, 0);
		EXPECT_EQ(array_int, 0);
		EXPECT_EQ(array_obj, 0);
		EXPECT_EQ(array_basic, 0);
		EXPECT_EQ(array_combine, 0);

		array_reserve(array_ptr, 32);
		array_reserve(array_int, 33);
		array_reserve(array_obj, 63);
		array_reserve(array_basic, 1024);
		array_reserve(array_combine, 32742);

		EXPECT_EQ(array_size(array_ptr), 0);
		EXPECT_EQ(array_size(array_int), 0);
		EXPECT_EQ(array_size(array_obj), 0);
		EXPECT_EQ(array_size(array_basic), 0);
		EXPECT_EQ(array_size(array_combine), 0);

		EXPECT_EQ(array_capacity(array_ptr), 32);
		EXPECT_EQ(array_capacity(array_int), 33);
		EXPECT_EQ(array_capacity(array_obj), 63);
		EXPECT_EQ(array_capacity(array_basic), 1024);
		EXPECT_EQ(array_capacity(array_combine), 32742);
	}
	// Deallocate
	{
		EXPECT_EQ(array_deallocate(array_ptr), nullptr);
		EXPECT_EQ(array_deallocate(array_int), nullptr);
		EXPECT_EQ(array_deallocate(array_obj), nullptr);
		EXPECT_EQ(array_deallocate(array_basic), nullptr);
		EXPECT_EQ(array_deallocate(array_combine), nullptr);

		EXPECT_EQ(array_size(array_ptr), 0);
		EXPECT_EQ(array_size(array_int), 0);
		EXPECT_EQ(array_size(array_obj), 0);
		EXPECT_EQ(array_size(array_basic), 0);
		EXPECT_EQ(array_size(array_combine), 0);

		EXPECT_EQ(array_capacity(array_ptr), 0);
		EXPECT_EQ(array_capacity(array_int), 0);
		EXPECT_EQ(array_capacity(array_obj), 0);
		EXPECT_EQ(array_capacity(array_basic), 0);
		EXPECT_EQ(array_capacity(array_combine), 0);

		EXPECT_EQ(array_ptr, 0);
		EXPECT_EQ(array_int, 0);
		EXPECT_EQ(array_obj, 0);
		EXPECT_EQ(array_basic, 0);
		EXPECT_EQ(array_combine, 0);
	}
	// Grow
	{
		array_grow(array_ptr, 75284);
		array_grow(array_int, 8295);
		array_grow(array_obj, 610);
		array_grow(array_basic, 24);
		array_grow(array_combine, 0);

		EXPECT_EQ(array_size(array_ptr), 75284);
		EXPECT_EQ(array_size(array_int), 8295);
		EXPECT_EQ(array_size(array_obj), 610);
		EXPECT_EQ(array_size(array_basic), 24);
		EXPECT_EQ(array_size(array_combine), 0);

		EXPECT_EQ(array_capacity(array_ptr), 75284);
		EXPECT_EQ(array_capacity(array_int), 8295);
		EXPECT_EQ(array_capacity(array_obj), 610);
		EXPECT_EQ(array_capacity(array_basic), 24);
		EXPECT_EQ(array_capacity(array_combine), 0);

		array_grow(array_ptr, 75284);
		array_grow(array_int, 8295);
		array_grow(array_obj, 610);
		array_grow(array_basic, 24);
		array_grow(array_combine, 0);

		EXPECT_EQ(array_size(array_ptr), 75284 * 2);
		EXPECT_EQ(array_size(array_int), 8295 * 2);
		EXPECT_EQ(array_size(array_obj), 610 * 2);
		EXPECT_EQ(array_size(array_basic), 24 * 2);
		EXPECT_EQ(array_size(array_combine), 0);

		EXPECT_EQ(array_capacity(array_ptr), 75284 * 2);
		EXPECT_EQ(array_capacity(array_int), 8295 * 2);
		EXPECT_EQ(array_capacity(array_obj), 610 * 2);
		EXPECT_EQ(array_capacity(array_basic), 24 * 2);
		EXPECT_EQ(array_capacity(array_combine), 0);

		array_grow(array_ptr, -1);
		array_grow(array_int, -2);
		array_grow(array_obj, -3);
		array_grow(array_basic, -4);
		array_grow(array_combine, 0);  // Grow is not bounds checked

		EXPECT_EQ(array_size(array_ptr), 75284 * 2 - 1);
		EXPECT_EQ(array_size(array_int), 8295 * 2 - 2);
		EXPECT_EQ(array_size(array_obj), 610 * 2 - 3);
		EXPECT_EQ(array_size(array_basic), 24 * 2 - 4);
		EXPECT_EQ(array_size(array_combine), 0);

		EXPECT_EQ(array_capacity(array_ptr), 75284 * 2);
		EXPECT_EQ(array_capacity(array_int), 8295 * 2);
		EXPECT_EQ(array_capacity(array_obj), 610 * 2);
		EXPECT_EQ(array_capacity(array_basic), 24 * 2);
		EXPECT_EQ(array_capacity(array_combine), 0);

		EXPECT_EQ(array_combine, 0);
	}
	// Clear
	{
		array_clear(array_ptr);
		array_clear(array_int);
		array_clear(array_obj);
		array_clear(array_basic);
		array_clear(array_combine);

		EXPECT_EQ(array_size(array_ptr), 0);
		EXPECT_EQ(array_size(array_int), 0);
		EXPECT_EQ(array_size(array_obj), 0);
		EXPECT_EQ(array_size(array_basic), 0);
		EXPECT_EQ(array_size(array_combine), 0);

		EXPECT_EQ(array_capacity(array_ptr), 75284 * 2);
		EXPECT_EQ(array_capacity(array_int), 8295 * 2);
		EXPECT_EQ(array_capacity(array_obj), 610 * 2);
		EXPECT_EQ(array_capacity(array_basic), 24 * 2);
		EXPECT_EQ(array_capacity(array_combine), 0);

		EXPECT_EQ(array_combine, 0);

		array_deallocate(array_ptr);
		array_deallocate(array_int);
		array_deallocate(array_obj);
		array_deallocate(array_basic);
		array_deallocate(array_combine);

		array_clear(array_ptr);
		array_clear(array_int);
		array_clear(array_obj);
		array_clear(array_basic);
		array_clear(array_combine);

		EXPECT_EQ(array_size(array_ptr), 0);
		EXPECT_EQ(array_size(array_int), 0);
		EXPECT_EQ(array_size(array_obj), 0);
		EXPECT_EQ(array_size(array_basic), 0);
		EXPECT_EQ(array_size(array_combine), 0);

		EXPECT_EQ(array_capacity(array_ptr), 0);
		EXPECT_EQ(array_capacity(array_int), 0);
		EXPECT_EQ(array_capacity(array_obj), 0);
		EXPECT_EQ(array_capacity(array_basic), 0);
		EXPECT_EQ(array_capacity(array_combine), 0);

		EXPECT_EQ(array_ptr, 0);
		EXPECT_EQ(array_int, 0);
		EXPECT_EQ(array_obj, 0);
		EXPECT_EQ(array_basic, 0);
		EXPECT_EQ(array_combine, 0);
	}
	// Reserve + grow
	{
		void** array_ptr_prev = 0;
		int* array_int_prev = 0;
		object_t* array_obj_prev = 0;
		basic_t* array_basic_prev = 0;
		combine_t* array_combine_prev = 0;

		array_reserve(array_ptr, 1);
		array_reserve(array_int, 2);
		array_reserve(array_obj, 3);
		array_reserve(array_basic, 4);
		array_reserve(array_combine, 5);

		EXPECT_EQ(array_size(array_ptr), 0);
		EXPECT_EQ(array_size(array_int), 0);
		EXPECT_EQ(array_size(array_obj), 0);
		EXPECT_EQ(array_size(array_basic), 0);
		EXPECT_EQ(array_size(array_combine), 0);

		EXPECT_EQ(array_capacity(array_ptr), 1);
		EXPECT_EQ(array_capacity(array_int), 2);
		EXPECT_EQ(array_capacity(array_obj), 3);
		EXPECT_EQ(array_capacity(array_basic), 4);
		EXPECT_EQ(array_capacity(array_combine), 5);

		EXPECT_NE(array_ptr, 0);
		EXPECT_NE(array_int, 0);
		EXPECT_NE(array_obj, 0);
		EXPECT_NE(array_basic, 0);
		EXPECT_NE(array_combine, 0);

		array_ptr_prev = array_ptr;
		array_int_prev = array_int;
		array_obj_prev = array_obj;
		array_basic_prev = array_basic;
		array_combine_prev = array_combine;

		array_grow(array_ptr, 1);
		array_grow(array_int, 2);
		array_grow(array_obj, 3);
		array_grow(array_basic, 4);
		array_grow(array_combine, 5);

		EXPECT_EQ(array_size(array_ptr), 1);
		EXPECT_EQ(array_size(array_int), 2);
		EXPECT_EQ(array_size(array_obj), 3);
		EXPECT_EQ(array_size(array_basic), 4);
		EXPECT_EQ(array_size(array_combine), 5);

		EXPECT_EQ(array_capacity(array_ptr), 1);
		EXPECT_EQ(array_capacity(array_int), 2);
		EXPECT_EQ(array_capacity(array_obj), 3);
		EXPECT_EQ(array_capacity(array_basic), 4);
		EXPECT_EQ(array_capacity(array_combine), 5);

		EXPECT_EQ(array_ptr, array_ptr_prev);
		EXPECT_EQ(array_int, array_int_prev);
		EXPECT_EQ(array_obj, array_obj_prev);
		EXPECT_EQ(array_basic, array_basic_prev);
		EXPECT_EQ(array_combine, array_combine_prev);

		array_grow(array_ptr, 1);
		array_grow(array_int, 2);
		array_grow(array_obj, 3);
		array_grow(array_basic, 4);
		array_grow(array_combine, 5);

		EXPECT_EQ(array_size(array_ptr), 1 * 2);
		EXPECT_EQ(array_size(array_int), 2 * 2);
		EXPECT_EQ(array_size(array_obj), 3 * 2);
		EXPECT_EQ(array_size(array_basic), 4 * 2);
		EXPECT_EQ(array_size(array_combine), 5 * 2);

		EXPECT_EQ(array_capacity(array_ptr), 1 * 2);
		EXPECT_EQ(array_capacity(array_int), 2 * 2);
		EXPECT_EQ(array_capacity(array_obj), 3 * 2);
		EXPECT_EQ(array_capacity(array_basic), 4 * 2);
		EXPECT_EQ(array_capacity(array_combine), 5 * 2);

		/* Reallocation might be in-place so pointers might be same
		EXPECT_NE( array_ptr, array_ptr_prev );
		EXPECT_NE( array_int, array_int_prev );
		EXPECT_NE( array_obj, array_obj_prev );
		EXPECT_NE( array_basic, array_basic_prev );
		EXPECT_NE( array_combine, array_combine_prev );*/
	}

	array_deallocate(array_ptr);
	array_deallocate(array_int);
	array_deallocate(array_obj);
	array_deallocate(array_basic);
	array_deallocate(array_combine);

	return 0;
}

DECLARE_TEST(array, copy) {
	int i;
	void** array_ptr = 0;
	int* array_int = 0;
	object_t* array_obj = 0;
	basic_t* array_basic = 0;
	combine_t* array_combine = 0;

	void** copy_ptr = 0;
	int* copy_int = 0;
	object_t* copy_obj = 0;
	basic_t* copy_basic = 0;
	combine_t* copy_combine = 0;

	for (i = 0; i < 255; ++i) {
		basic_t basic;
		combine_t combine;

		memset(&basic, 0, sizeof(basic));
		basic.intval = i;
		basic.floatval = (float32_t)i;
		basic.objval = (object_t)(i + 1);

		memset(&combine, 0, sizeof(combine));
		combine.basicval.intval = i;
		combine.basicval.objval = (object_t)(i + 1);
		combine.intval = i + 2;
		combine.ptrval = 0;
		combine.charval = (char)i;
		combine.unionval.basicval.intval = i + 3;
		combine.unionval.basicval.objval = (object_t)(i + 4);

		array_push(array_ptr, (void*)((uintptr_t)i));
		array_push(array_int, i);
		array_push(array_obj, (object_t)i);
		array_push(array_basic, basic);
		array_push(array_combine, combine);
	}

	EXPECT_EQ(array_copy(copy_ptr, array_ptr), array_size(array_ptr));
	EXPECT_EQ(array_copy(copy_int, array_int), array_size(array_int));
	EXPECT_EQ(array_copy(copy_obj, array_obj), array_size(array_obj));
	EXPECT_EQ(array_copy(copy_basic, array_basic), array_size(array_basic));
	EXPECT_EQ(array_copy(copy_combine, array_combine), array_size(array_combine));

	EXPECT_EQ(array_size(copy_ptr), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_int), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_obj), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_basic), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_combine), array_size(array_ptr));

	for (i = 0; i < 255; ++i) {
		EXPECT_EQ(copy_ptr[i], array_ptr[i]);
		EXPECT_EQ(copy_int[i], array_int[i]);
		EXPECT_EQ(copy_obj[i], array_obj[i]);
		EXPECT_EQ(memcmp(copy_basic + i, array_basic + i, sizeof(*copy_basic)), 0);
		EXPECT_EQ(memcmp(copy_combine + i, array_combine + i, sizeof(*copy_combine)), 0);
	}

	EXPECT_EQ(array_copy(copy_ptr, array_ptr), array_size(array_ptr));
	EXPECT_EQ(array_copy(copy_int, array_int), array_size(array_int));
	EXPECT_EQ(array_copy(copy_obj, array_obj), array_size(array_obj));
	EXPECT_EQ(array_copy(copy_basic, array_basic), array_size(array_basic));
	EXPECT_EQ(array_copy(copy_combine, array_combine), array_size(array_combine));

	EXPECT_EQ(array_size(copy_ptr), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_int), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_obj), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_basic), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_combine), array_size(array_ptr));

	for (i = 0; i < 255; ++i) {
		EXPECT_EQ(copy_ptr[i], array_ptr[i]);
		EXPECT_EQ(copy_int[i], array_int[i]);
		EXPECT_EQ(copy_obj[i], array_obj[i]);
		EXPECT_EQ(memcmp(copy_basic + i, array_basic + i, sizeof(*copy_basic)), 0);
		EXPECT_EQ(memcmp(copy_combine + i, array_combine + i, sizeof(*copy_combine)), 0);
	}

	for (i = 0; i < 255; ++i) {
		basic_t basic;
		combine_t combine;

		memset(&basic, 0, sizeof(basic));
		basic.intval = i;
		basic.objval = (object_t)(i + 1);

		memset(&combine, 0, sizeof(combine));
		combine.basicval.intval = i;
		combine.basicval.objval = (object_t)(i + 1);
		combine.intval = i + 2;
		combine.ptrval = 0;
		combine.charval = (char)i;
		combine.unionval.basicval.intval = i + 3;
		combine.unionval.basicval.objval = (object_t)(i + 4);

		array_push(copy_ptr, (void*)((uintptr_t)i));
		array_push(copy_int, i);
		array_push(copy_obj, (object_t)i);
		array_push(copy_basic, basic);
		array_push(copy_combine, combine);
	}

	array_erase(copy_ptr, 0);
	array_erase(copy_int, 0);
	array_erase(copy_obj, 0);
	array_erase(copy_basic, 0);
	array_erase(copy_combine, 0);

	array_copy(copy_ptr, array_ptr);
	array_copy(copy_int, array_int);
	array_copy(copy_obj, array_obj);
	array_copy(copy_basic, array_basic);
	array_copy(copy_combine, array_combine);

	EXPECT_EQ(array_size(copy_ptr), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_int), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_obj), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_basic), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_combine), array_size(array_ptr));

	for (i = 0; i < 255; ++i) {
		EXPECT_EQ(copy_ptr[i], array_ptr[i]);
		EXPECT_EQ(copy_int[i], array_int[i]);
		EXPECT_EQ(copy_obj[i], array_obj[i]);
		EXPECT_EQ(memcmp(copy_basic + i, array_basic + i, sizeof(*copy_basic)), 0);
		EXPECT_EQ(memcmp(copy_combine + i, array_combine + i, sizeof(*copy_combine)), 0);
	}

	array_clear(copy_ptr);
	array_clear(copy_int);
	array_clear(copy_obj);
	array_clear(copy_basic);
	array_clear(copy_combine);

	array_copy(copy_ptr, array_ptr);
	array_copy(copy_int, array_int);
	array_copy(copy_obj, array_obj);
	array_copy(copy_basic, array_basic);
	array_copy(copy_combine, array_combine);

	EXPECT_EQ(array_size(copy_ptr), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_int), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_obj), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_basic), array_size(array_ptr));
	EXPECT_EQ(array_size(copy_combine), array_size(array_ptr));

	for (i = 0; i < 255; ++i) {
		EXPECT_EQ(copy_ptr[i], array_ptr[i]);
		EXPECT_EQ(copy_int[i], array_int[i]);
		EXPECT_EQ(copy_obj[i], array_obj[i]);
		EXPECT_EQ(memcmp(copy_basic + i, array_basic + i, sizeof(*copy_basic)), 0);
		EXPECT_EQ(memcmp(copy_combine + i, array_combine + i, sizeof(*copy_combine)), 0);
	}

	array_deallocate(array_ptr);
	array_deallocate(array_int);
	array_deallocate(array_obj);
	array_deallocate(array_basic);
	array_deallocate(array_combine);

	array_deallocate(copy_ptr);
	array_deallocate(copy_int);
	array_deallocate(copy_obj);
	array_deallocate(copy_basic);
	array_deallocate(copy_combine);

	return 0;
}

DECLARE_TEST(array, pushpop) {
	int i, j;
	void** array_ptr = 0;
	int* array_int = 0;
	object_t* array_obj = 0;
	basic_t* array_basic = 0;
	combine_t* array_combine = 0;

	// Push to empty
	{
		basic_t basic;
		combine_t combine;

		memset(&basic, 0, sizeof(basic));
		basic.intval = 1;
		basic.objval = 2;

		memset(&combine, 0, sizeof(combine));
		combine.basicval.intval = 3;
		combine.basicval.objval = 4;
		combine.charval = 5;
		combine.intval = 6;
		combine.ptrval = 0;
		combine.unionval.realval = REAL_C(1.0);

		EXPECT_NE(array_push(array_ptr, 0), nullptr);
		EXPECT_NE(array_push(array_int, 0), nullptr);
		EXPECT_NE(array_push(array_obj, 0), nullptr);
		EXPECT_NE(array_push(array_basic, basic), nullptr);
		EXPECT_NE(array_push(array_combine, combine), nullptr);

		EXPECT_EQ(array_size(array_ptr), 1);
		EXPECT_EQ(array_size(array_int), 1);
		EXPECT_EQ(array_size(array_obj), 1);
		EXPECT_EQ(array_size(array_basic), 1);
		EXPECT_EQ(array_size(array_combine), 1);

		EXPECT_EQ(array_capacity(array_ptr), 1);
		EXPECT_EQ(array_capacity(array_int), 1);
		EXPECT_EQ(array_capacity(array_obj), 1);
		EXPECT_EQ(array_capacity(array_basic), 1);
		EXPECT_EQ(array_capacity(array_combine), 1);

		EXPECT_EQ(array_ptr[0], 0);
		EXPECT_EQ(array_int[0], 0);
		EXPECT_EQ(array_obj[0], 0);
		EXPECT_EQ(memcmp(array_basic, &basic, sizeof(basic)), 0);
		EXPECT_EQ(memcmp(array_combine, &combine, sizeof(combine)), 0);
	}
	// Pop
	{
		array_pop(array_ptr);
		array_pop(array_int);
		array_pop(array_obj);
		array_pop(array_basic);
		array_pop(array_combine);

		EXPECT_EQ(array_size(array_ptr), 0);
		EXPECT_EQ(array_size(array_int), 0);
		EXPECT_EQ(array_size(array_obj), 0);
		EXPECT_EQ(array_size(array_basic), 0);
		EXPECT_EQ(array_size(array_combine), 0);

		EXPECT_EQ(array_capacity(array_ptr), 1);
		EXPECT_EQ(array_capacity(array_int), 1);
		EXPECT_EQ(array_capacity(array_obj), 1);
		EXPECT_EQ(array_capacity(array_basic), 1);
		EXPECT_EQ(array_capacity(array_combine), 1);

		array_pop_safe(array_ptr);
		array_pop_safe(array_int);
		array_pop_safe(array_obj);
		array_pop_safe(array_basic);
		array_pop_safe(array_combine);

		EXPECT_EQ(array_size(array_ptr), 0);
		EXPECT_EQ(array_size(array_int), 0);
		EXPECT_EQ(array_size(array_obj), 0);
		EXPECT_EQ(array_size(array_basic), 0);
		EXPECT_EQ(array_size(array_combine), 0);

		EXPECT_EQ(array_capacity(array_ptr), 1);
		EXPECT_EQ(array_capacity(array_int), 1);
		EXPECT_EQ(array_capacity(array_obj), 1);
		EXPECT_EQ(array_capacity(array_basic), 1);
		EXPECT_EQ(array_capacity(array_combine), 1);
	}
	// Push to non-empty
	for (i = 0; i < 255; ++i) {
		basic_t basic;
		combine_t combine;

		memset(&basic, 0, sizeof(basic));
		basic.intval = i;
		basic.objval = (object_t)(i + 1);

		memset(&combine, 0, sizeof(combine));
		combine.basicval.intval = i;
		combine.basicval.objval = (object_t)(i + 1);
		combine.intval = i + 2;
		combine.ptrval = 0;
		combine.charval = (char)i;
		combine.unionval.basicval.intval = i + 3;
		combine.unionval.basicval.objval = (object_t)(i + 4);

		if (!(i % 2)) {
			array_push(array_ptr, (void*)((uintptr_t)i));
			array_push(array_int, i);
			array_push(array_obj, (object_t)i);
			array_push(array_basic, basic);
			array_push(array_combine, combine);
		} else {
			void* iptr = (void*)((uintptr_t)i);
			object_t iobj = (object_t)i;
			array_push_memcpy(array_ptr, &iptr);
			array_push_memcpy(array_int, &i);
			array_push_memcpy(array_obj, &iobj);
			array_push_memcpy(array_basic, &basic);
			array_push_memcpy(array_combine, &combine);
		}

		EXPECT_EQ(array_size(array_ptr), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_int), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_obj), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_basic), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_combine), (unsigned int)i + 1);

		EXPECT_GE(array_capacity(array_ptr), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_int), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_obj), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_basic), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_combine), (unsigned int)i + 1);

		for (j = 0; j <= i; ++j) {
			EXPECT_EQ(array_ptr[j], (void*)((uintptr_t)j));
			EXPECT_EQ(array_int[j], j);
			EXPECT_EQ(array_obj[j], (object_t)j);
			EXPECT_EQ(array_basic[j].intval, j);
			EXPECT_EQ(array_basic[j].objval, (object_t)j + 1);
			EXPECT_EQ(array_combine[j].basicval.intval, j);
			EXPECT_EQ(array_combine[j].basicval.objval, (object_t)j + 1);
			EXPECT_EQ(array_combine[j].intval, j + 2);
			EXPECT_EQ(array_combine[j].charval, (char)j);
			EXPECT_EQ(array_combine[j].ptrval, 0);
			EXPECT_EQ(array_combine[j].unionval.basicval.intval, j + 3);
			EXPECT_EQ(array_combine[j].unionval.basicval.objval, (object_t)j + 4);
		}
	}
	// Pop while non-empty
	for (; i > 0; --i) {
		array_pop(array_ptr);
		array_pop(array_int);
		array_pop(array_obj);
		array_pop(array_basic);
		array_pop(array_combine);

		EXPECT_EQ(array_size(array_ptr), (unsigned int)i - 1);
		EXPECT_EQ(array_size(array_int), (unsigned int)i - 1);
		EXPECT_EQ(array_size(array_obj), (unsigned int)i - 1);
		EXPECT_EQ(array_size(array_basic), (unsigned int)i - 1);
		EXPECT_EQ(array_size(array_combine), (unsigned int)i - 1);

		EXPECT_GE(array_capacity(array_ptr), (unsigned int)i - 1);
		EXPECT_GE(array_capacity(array_int), (unsigned int)i - 1);
		EXPECT_GE(array_capacity(array_obj), (unsigned int)i - 1);
		EXPECT_GE(array_capacity(array_basic), (unsigned int)i - 1);
		EXPECT_GE(array_capacity(array_combine), (unsigned int)i - 1);

		for (j = 0; j < i; ++j) {
			EXPECT_EQ(array_ptr[j], (void*)((uintptr_t)j));
			EXPECT_EQ(array_int[j], j);
			EXPECT_EQ(array_obj[j], (object_t)j);
			EXPECT_EQ(array_basic[j].intval, j);
			EXPECT_EQ(array_basic[j].objval, (object_t)j + 1);
			EXPECT_EQ(array_combine[j].basicval.intval, j);
			EXPECT_EQ(array_combine[j].basicval.objval, (object_t)j + 1);
			EXPECT_EQ(array_combine[j].intval, j + 2);
			EXPECT_EQ(array_combine[j].charval, (char)j);
			EXPECT_EQ(array_combine[j].ptrval, 0);
			EXPECT_EQ(array_combine[j].unionval.basicval.intval, j + 3);
			EXPECT_EQ(array_combine[j].unionval.basicval.objval, (object_t)j + 4);
		}
	}
	// Interleaved push-pops
	for (i = 0; i < 255; ++i) {
		basic_t basic;
		combine_t combine;

		memset(&basic, 0, sizeof(basic));
		memset(&combine, 0, sizeof(combine));
		for (j = 0; j < 2; ++j) {
			basic.intval = i + j;
			basic.objval = (object_t)(i + j + 1);

			combine.basicval.intval = i + j;
			combine.basicval.objval = (object_t)(i + j + 1);
			combine.intval = i + j + 2;
			combine.ptrval = 0;
			combine.charval = (char)(i + j);
			combine.unionval.basicval.intval = i + j + 3;
			combine.unionval.basicval.objval = (object_t)(i + j + 4);

			if (!(i % 2)) {
				array_push(array_ptr, (void*)((uintptr_t)(i + j)));
				array_push(array_int, i + j);
				array_push(array_obj, (object_t)(i + j));
				array_push(array_basic, basic);
				array_push(array_combine, combine);
			} else {
				void* iptr = (void*)((uintptr_t)(i + j));
				object_t iobj = (object_t)(i + j);
				int ii = i + j;
				array_push_memcpy(array_ptr, &iptr);
				array_push_memcpy(array_int, &ii);
				array_push_memcpy(array_obj, &iobj);
				array_push_memcpy(array_basic, &basic);
				array_push_memcpy(array_combine, &combine);
			}
		}

		array_pop(array_ptr);
		array_pop(array_int);
		array_pop(array_obj);
		array_pop(array_basic);
		array_pop(array_combine);

		EXPECT_EQ(array_size(array_ptr), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_int), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_obj), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_basic), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_combine), (unsigned int)i + 1);

		EXPECT_GE(array_capacity(array_ptr), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_int), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_obj), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_basic), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_combine), (unsigned int)i + 1);

		for (j = 0; j <= i; ++j) {
			EXPECT_EQ(array_ptr[j], (void*)((uintptr_t)j));
			EXPECT_EQ(array_int[j], j);
			EXPECT_EQ(array_obj[j], (object_t)j);
			EXPECT_EQ(array_basic[j].intval, j);
			EXPECT_EQ(array_basic[j].objval, (object_t)(j + 1));
			EXPECT_EQ(array_combine[j].basicval.intval, j);
			EXPECT_EQ(array_combine[j].basicval.objval, (object_t)(j + 1));
			EXPECT_EQ(array_combine[j].intval, j + 2);
			EXPECT_EQ(array_combine[j].charval, (char)j);
			EXPECT_EQ(array_combine[j].ptrval, 0);
			EXPECT_EQ(array_combine[j].unionval.basicval.intval, j + 3);
			EXPECT_EQ(array_combine[j].unionval.basicval.objval, (object_t)(j + 4));
		}
	}

	array_deallocate(array_ptr);
	array_deallocate(array_int);
	array_deallocate(array_obj);
	array_deallocate(array_basic);
	array_deallocate(array_combine);

	return 0;
}

DECLARE_TEST(array, inserterase) {
	unsigned int clamped_i;
	int i, j, k;
	bool found;

	void** array_ptr = 0;
	int* array_int = 0;
	object_t* array_obj = 0;
	basic_t* array_basic = 0;
	combine_t* array_combine = 0;

	void** copy_ptr = 0;
	int* copy_int = 0;
	object_t* copy_obj = 0;
	basic_t* copy_basic = 0;
	combine_t* copy_combine = 0;

	// Insert in empty
	{
		basic_t basic;
		combine_t combine;

		memset(&basic, 0, sizeof(basic));
		basic.intval = 1;
		basic.objval = 2;

		memset(&combine, 0, sizeof(combine));
		combine.basicval.intval = 3;
		combine.basicval.objval = 4;
		combine.charval = 5;
		combine.intval = 6;
		combine.ptrval = 0;
		combine.unionval.realval = REAL_C(1.0);

		array_insert(array_ptr, 0, 0);
		array_insert(array_int, 0, 0);
		array_insert(array_obj, 0, 0);
		array_insert(array_basic, 0, basic);
		array_insert(array_combine, 0, combine);

		EXPECT_EQ(array_size(array_ptr), 1);
		EXPECT_EQ(array_size(array_int), 1);
		EXPECT_EQ(array_size(array_obj), 1);
		EXPECT_EQ(array_size(array_basic), 1);
		EXPECT_EQ(array_size(array_combine), 1);

		EXPECT_EQ(array_capacity(array_ptr), 1);
		EXPECT_EQ(array_capacity(array_int), 1);
		EXPECT_EQ(array_capacity(array_obj), 1);
		EXPECT_EQ(array_capacity(array_basic), 1);
		EXPECT_EQ(array_capacity(array_combine), 1);

		EXPECT_EQ(array_ptr[0], 0);
		EXPECT_EQ(array_int[0], 0);
		EXPECT_EQ(array_obj[0], 0);
		EXPECT_EQ(memcmp(array_basic, &basic, sizeof(basic)), 0);
		EXPECT_EQ(memcmp(array_combine, &combine, sizeof(combine)), 0);
	}
	// Erase
	{
		int small_neg = -1;
		int large_neg = -1234;

		array_erase(array_ptr, 0);
		array_erase(array_int, 0);
		array_erase(array_obj, 0);
		array_erase(array_basic, 0);
		array_erase(array_combine, 0);

		EXPECT_EQ(array_size(array_ptr), 0);
		EXPECT_EQ(array_size(array_int), 0);
		EXPECT_EQ(array_size(array_obj), 0);
		EXPECT_EQ(array_size(array_basic), 0);
		EXPECT_EQ(array_size(array_combine), 0);

		EXPECT_EQ(array_capacity(array_ptr), 1);
		EXPECT_EQ(array_capacity(array_int), 1);
		EXPECT_EQ(array_capacity(array_obj), 1);
		EXPECT_EQ(array_capacity(array_basic), 1);
		EXPECT_EQ(array_capacity(array_combine), 1);

		array_erase_safe(array_ptr, small_neg);
		array_erase_safe(array_int, large_neg);
		array_erase_safe(array_obj, 0);
		array_erase_safe(array_basic, 1);
		array_erase_safe(array_combine, 1234);

		EXPECT_EQ(array_size(array_ptr), 0);
		EXPECT_EQ(array_size(array_int), 0);
		EXPECT_EQ(array_size(array_obj), 0);
		EXPECT_EQ(array_size(array_basic), 0);
		EXPECT_EQ(array_size(array_combine), 0);

		EXPECT_EQ(array_capacity(array_ptr), 1);
		EXPECT_EQ(array_capacity(array_int), 1);
		EXPECT_EQ(array_capacity(array_obj), 1);
		EXPECT_EQ(array_capacity(array_basic), 1);
		EXPECT_EQ(array_capacity(array_combine), 1);
	}
	// Insert safe in empty
	{
		basic_t basic;
		combine_t combine;
		int small_neg = -1;
		int large_neg = -1234;

		memset(&basic, 0, sizeof(basic));
		basic.intval = 1;
		basic.objval = 2;

		memset(&combine, 0, sizeof(combine));
		combine.basicval.intval = 3;
		combine.basicval.objval = 4;
		combine.charval = 5;
		combine.intval = 6;
		combine.ptrval = 0;
		combine.unionval.realval = REAL_C(1.0);

		array_insert_safe(array_ptr, large_neg, 0);
		array_insert_safe(array_int, small_neg, 0);
		array_insert_safe(array_obj, 0, 0);
		array_insert_safe(array_basic, 1, basic);
		array_insert_safe(array_combine, 1234, combine);

		EXPECT_EQ(array_size(array_ptr), 1);
		EXPECT_EQ(array_size(array_int), 1);
		EXPECT_EQ(array_size(array_obj), 1);
		EXPECT_EQ(array_size(array_basic), 1);
		EXPECT_EQ(array_size(array_combine), 1);

		EXPECT_EQ(array_capacity(array_ptr), 1);
		EXPECT_EQ(array_capacity(array_int), 1);
		EXPECT_EQ(array_capacity(array_obj), 1);
		EXPECT_EQ(array_capacity(array_basic), 1);
		EXPECT_EQ(array_capacity(array_combine), 1);

		EXPECT_EQ(array_ptr[0], 0);
		EXPECT_EQ(array_int[0], 0);
		EXPECT_EQ(array_obj[0], 0);
		EXPECT_EQ(memcmp(array_basic, &basic, sizeof(basic)), 0);
		EXPECT_EQ(memcmp(array_combine, &combine, sizeof(combine)), 0);
	}

	array_deallocate(array_ptr);
	array_deallocate(array_int);
	array_deallocate(array_obj);
	array_deallocate(array_basic);
	array_deallocate(array_combine);

	// Insert sequence
	for (i = 0; i < 255; ++i) {
		basic_t basic;
		combine_t combine;

		memset(&basic, 0, sizeof(basic));
		basic.intval = i;
		basic.objval = (object_t)(i + 1);

		memset(&combine, 0, sizeof(combine));
		combine.basicval.intval = i + 2;
		combine.basicval.objval = (object_t)(i + 3);
		combine.charval = (char)(i + 4);
		combine.intval = i + 5;
		combine.ptrval = (void*)((uintptr_t)(i + 6));
		combine.unionval.basicval.intval = i + 7;
		combine.unionval.basicval.objval = (object_t)(i + 8);

		clamped_i = (unsigned int)math_clamp(129 - i, 0, (int)array_size(array_ptr));

		array_insert_safe(array_ptr, 129 - i, (void*)((uintptr_t)i));
		array_insert_safe(array_int, 129 - i, i);
		array_insert_safe(array_obj, 129 - i, (object_t)i);
		array_insert_safe(array_basic, 129 - i, basic);
		array_insert_safe(array_combine, 129 - i, combine);

		EXPECT_EQ(array_size(array_ptr), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_int), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_obj), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_basic), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_combine), (unsigned int)i + 1);

		EXPECT_GE(array_capacity(array_ptr), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_int), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_obj), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_basic), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_combine), (unsigned int)i + 1);

		EXPECT_EQ_MSGFORMAT(array_ptr[clamped_i], (void*)((uintptr_t)i), "Pass %d", i);
		EXPECT_EQ(array_int[clamped_i], i);
		EXPECT_EQ(array_obj[clamped_i], (object_t)i);
		EXPECT_EQ(memcmp(array_basic + clamped_i, &basic, sizeof(basic)), 0);
		EXPECT_EQ(memcmp(array_combine + clamped_i, &combine, sizeof(combine)), 0);
	}

	array_clear(array_ptr);
	array_clear(array_int);
	array_clear(array_obj);
	array_clear(array_basic);
	array_clear(array_combine);

	// Insert sequence
	for (i = 0; i < 255; ++i) {
		basic_t basic;
		combine_t combine;

		memset(&basic, 0, sizeof(basic));
		basic.intval = i;
		basic.objval = (object_t)(i + 1);

		memset(&combine, 0, sizeof(combine));
		combine.basicval.intval = i + 2;
		combine.basicval.objval = (object_t)(i + 3);
		combine.charval = (char)(i + 4);
		combine.intval = i + 5;
		combine.ptrval = (void*)((uintptr_t)(i + 6));
		combine.unionval.basicval.intval = i + 7;
		combine.unionval.basicval.objval = (object_t)(i + 8);

		clamped_i = (unsigned int)math_clamp(129 - i, 0, (int)array_size(array_ptr));

		array_insert(array_ptr, clamped_i, (void*)((uintptr_t)i));
		array_insert(array_int, clamped_i, i);
		array_insert(array_obj, clamped_i, (object_t)i);
		array_insert(array_basic, clamped_i, basic);
		array_insert(array_combine, clamped_i, combine);

		EXPECT_EQ(array_size(array_ptr), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_int), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_obj), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_basic), (unsigned int)i + 1);
		EXPECT_EQ(array_size(array_combine), (unsigned int)i + 1);

		EXPECT_GE(array_capacity(array_ptr), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_int), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_obj), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_basic), (unsigned int)i + 1);
		EXPECT_GE(array_capacity(array_combine), (unsigned int)i + 1);

		EXPECT_EQ(array_ptr[clamped_i], (void*)((uintptr_t)i));
		EXPECT_EQ(array_int[clamped_i], i);
		EXPECT_EQ(array_obj[clamped_i], (object_t)i);
		EXPECT_EQ(memcmp(array_basic + clamped_i, &basic, sizeof(basic)), 0);
		EXPECT_EQ(memcmp(array_combine + clamped_i, &combine, sizeof(combine)), 0);
	}

	array_deallocate(array_ptr);
	array_deallocate(array_int);
	array_deallocate(array_obj);
	array_deallocate(array_basic);
	array_deallocate(array_combine);

	// Erase single items
	for (i = 0; i < 255; ++i) {
		basic_t basic;
		combine_t combine;

		memset(&basic, 0, sizeof(basic));
		basic.intval = i;
		basic.objval = (object_t)(i + 1);

		memset(&combine, 0, sizeof(combine));
		combine.basicval.intval = i + 2;
		combine.basicval.objval = (object_t)(i + 3);
		combine.charval = (char)(i + 4);
		combine.intval = i + 5;
		combine.ptrval = (void*)((uintptr_t)(i + 6));
		combine.unionval.basicval.intval = i + 7;
		combine.unionval.basicval.objval = (object_t)(i + 8);

		clamped_i = (unsigned int)math_clamp(129 - i, 0, (int)array_size(array_ptr));

		array_insert(array_ptr, clamped_i, (void*)((uintptr_t)i));
		array_insert(array_int, clamped_i, i);
		array_insert(array_obj, clamped_i, (object_t)i);
		array_insert(array_basic, clamped_i, basic);
		array_insert(array_combine, clamped_i, combine);
	}
	for (i = 0; i < 255; ++i) {
		void* ptrval;
		int intval;
		object_t objval;
		basic_t basic;
		combine_t combine;

		clamped_i = (unsigned int)math_clamp(129 - i, 0, (int)array_size(array_ptr));

		ptrval = array_ptr[clamped_i];
		intval = array_int[clamped_i];
		objval = array_obj[clamped_i];
		basic = array_basic[clamped_i];
		combine = array_combine[clamped_i];

		array_copy(copy_ptr, array_ptr);
		array_copy(copy_int, array_int);
		array_copy(copy_obj, array_obj);
		array_copy(copy_basic, array_basic);
		array_copy(copy_combine, array_combine);

		array_erase(array_ptr, clamped_i);
		array_erase(array_int, clamped_i);
		array_erase(array_obj, clamped_i);
		array_erase(array_basic, clamped_i);
		array_erase(array_combine, clamped_i);

		EXPECT_EQ(array_size(array_ptr), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_int), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_obj), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_basic), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_combine), (unsigned int)(254 - i));

		for (j = 0; j < 254 - i; ++j) {
			EXPECT_NE(array_ptr[j], ptrval);
			EXPECT_NE(array_int[j], intval);
			EXPECT_NE(array_obj[j], objval);
			EXPECT_NE(memcmp(array_basic + j, &basic, sizeof(basic)), 0);
			EXPECT_NE(memcmp(array_combine + j, &combine, sizeof(combine)), 0);
		}

		for (j = 0; j < (int)array_size(copy_ptr); ++j) {
			if (j == (int)clamped_i)
				continue;

			found = false;
			for (k = 0; k < 254 - i; ++k) {
				if (array_ptr[k] == copy_ptr[j]) {
					EXPECT_EQ(array_ptr[k], copy_ptr[j]);
					EXPECT_EQ(array_int[k], copy_int[j]);
					EXPECT_EQ(array_obj[k], copy_obj[j]);
					EXPECT_EQ(memcmp(array_basic + k, copy_basic + j, sizeof(basic)), 0);
					EXPECT_EQ(memcmp(array_combine + k, copy_combine + j, sizeof(combine)), 0);
					found = true;
					break;
				}
			}
			EXPECT_TRUE(found);
		}

		array_deallocate(copy_ptr);
		array_deallocate(copy_int);
		array_deallocate(copy_obj);
		array_deallocate(copy_basic);
		array_deallocate(copy_combine);
	}

	// Erase single item, safe
	for (i = 0; i < 255; ++i) {
		basic_t basic;
		combine_t combine;

		memset(&basic, 0, sizeof(basic));
		basic.intval = i;
		basic.objval = (object_t)(i + 1);

		memset(&combine, 0, sizeof(combine));
		combine.basicval.intval = i + 2;
		combine.basicval.objval = (object_t)(i + 3);
		combine.charval = (char)(i + 4);
		combine.intval = i + 5;
		combine.ptrval = (void*)((uintptr_t)(i + 6));
		combine.unionval.basicval.intval = i + 7;
		combine.unionval.basicval.objval = (object_t)(i + 8);

		clamped_i = (unsigned int)math_clamp(129 - i, 0, (int)array_size(array_ptr));

		array_insert(array_ptr, clamped_i, (void*)((uintptr_t)i));
		array_insert(array_int, clamped_i, i);
		array_insert(array_obj, clamped_i, (object_t)i);
		array_insert(array_basic, clamped_i, basic);
		array_insert(array_combine, clamped_i, combine);
	}
	for (i = 0; i < 255; ++i) {
		void* ptrval;
		int intval;
		object_t objval;
		basic_t basic;
		combine_t combine;
		int small_neg = -1;
		int large_neg = -1234;
		int huge_neg = -123456;

		clamped_i = (unsigned int)math_clamp(129 - i, 0, (int)array_size(array_ptr));

		ptrval = array_ptr[clamped_i];
		intval = array_int[clamped_i];
		objval = array_obj[clamped_i];
		basic = array_basic[clamped_i];
		combine = array_combine[clamped_i];

		array_copy(copy_ptr, array_ptr);
		array_copy(copy_int, array_int);
		array_copy(copy_obj, array_obj);
		array_copy(copy_basic, array_basic);
		array_copy(copy_combine, array_combine);

		array_erase_safe(array_ptr, clamped_i);
		array_erase_safe(array_int, clamped_i);
		array_erase_safe(array_obj, clamped_i);
		array_erase_safe(array_basic, clamped_i);
		array_erase_safe(array_combine, clamped_i);

		array_erase_safe(array_ptr, large_neg);
		array_erase_safe(array_int, small_neg);
		array_erase_safe(array_obj, 1024);
		array_erase_safe(array_basic, 123456);
		array_erase_safe(array_combine, huge_neg);

		EXPECT_EQ(array_size(array_ptr), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_int), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_obj), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_basic), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_combine), (unsigned int)(254 - i));

		for (j = 0; j < 254 - i; ++j) {
			EXPECT_NE(array_ptr[j], ptrval);
			EXPECT_NE(array_int[j], intval);
			EXPECT_NE(array_obj[j], objval);
			EXPECT_NE(memcmp(array_basic + j, &basic, sizeof(basic)), 0);
			EXPECT_NE(memcmp(array_combine + j, &combine, sizeof(combine)), 0);
		}

		for (j = 0; j < (int)array_size(copy_ptr); ++j) {
			if (j == (int)clamped_i)
				continue;

			found = false;
			for (k = 0; k < 254 - i; ++k) {
				if (array_ptr[k] == copy_ptr[j]) {
					EXPECT_EQ(array_ptr[k], copy_ptr[j]);
					EXPECT_EQ(array_int[k], copy_int[j]);
					EXPECT_EQ(array_obj[k], copy_obj[j]);
					EXPECT_EQ(memcmp(array_basic + k, copy_basic + j, sizeof(basic)), 0);
					EXPECT_EQ(memcmp(array_combine + k, copy_combine + j, sizeof(combine)), 0);
					found = true;
					break;
				}
			}
			EXPECT_TRUE(found);
		}

		array_deallocate(copy_ptr);
		array_deallocate(copy_int);
		array_deallocate(copy_obj);
		array_deallocate(copy_basic);
		array_deallocate(copy_combine);
	}

	// Erase single item, memcpy
	for (i = 0; i < 255; ++i) {
		basic_t basic;
		combine_t combine;

		memset(&basic, 0, sizeof(basic));
		basic.intval = i;
		basic.objval = (object_t)(i + 1);

		memset(&combine, 0, sizeof(combine));
		combine.basicval.intval = i + 2;
		combine.basicval.objval = (object_t)(i + 3);
		combine.charval = (char)(i + 4);
		combine.intval = i + 5;
		combine.ptrval = (void*)((uintptr_t)(i + 6));
		combine.unionval.basicval.intval = i + 7;
		combine.unionval.basicval.objval = (object_t)(i + 8);

		clamped_i = (unsigned int)math_clamp(129 - i, 0, (int)array_size(array_ptr));

		array_insert(array_ptr, clamped_i, (void*)((uintptr_t)i));
		array_insert(array_int, clamped_i, i);
		array_insert(array_obj, clamped_i, (object_t)i);
		array_insert_memcpy(array_basic, clamped_i, &basic);
		array_insert_memcpy(array_combine, clamped_i, &combine);
	}
	for (i = 0; i < 255; ++i) {
		void* ptrval;
		int intval;
		object_t objval;
		basic_t basic;
		combine_t combine;

		clamped_i = (unsigned int)math_clamp(129 - i, 0, (int)array_size(array_ptr));

		ptrval = array_ptr[clamped_i];
		intval = array_int[clamped_i];
		objval = array_obj[clamped_i];
		basic = array_basic[clamped_i];
		combine = array_combine[clamped_i];

		array_copy(copy_ptr, array_ptr);
		array_copy(copy_int, array_int);
		array_copy(copy_obj, array_obj);
		array_copy(copy_basic, array_basic);
		array_copy(copy_combine, array_combine);

		array_erase_memcpy(array_ptr, clamped_i);
		array_erase_memcpy(array_int, clamped_i);
		array_erase_memcpy(array_obj, clamped_i);
		array_erase_memcpy(array_basic, clamped_i);
		array_erase_memcpy(array_combine, clamped_i);

		EXPECT_EQ(array_size(array_ptr), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_int), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_obj), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_basic), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_combine), (unsigned int)(254 - i));

		for (j = 0; j < 254 - i; ++j) {
			EXPECT_NE(array_ptr[j], ptrval);
			EXPECT_NE(array_int[j], intval);
			EXPECT_NE(array_obj[j], objval);
			EXPECT_NE(memcmp(array_basic + j, &basic, sizeof(basic)), 0);
			EXPECT_NE(memcmp(array_combine + j, &combine, sizeof(combine)), 0);
		}

		for (j = 0; j < (int)array_size(copy_ptr); ++j) {
			if (j == (int)clamped_i)
				continue;

			found = false;
			for (k = 0; k < 254 - i; ++k) {
				if (array_ptr[k] == copy_ptr[j]) {
					EXPECT_EQ(array_ptr[k], copy_ptr[j]);
					EXPECT_EQ(array_int[k], copy_int[j]);
					EXPECT_EQ(array_obj[k], copy_obj[j]);
					EXPECT_EQ(memcmp(array_basic + k, copy_basic + j, sizeof(basic)), 0);
					EXPECT_EQ(memcmp(array_combine + k, copy_combine + j, sizeof(combine)), 0);
					found = true;
					break;
				}
			}
			EXPECT_TRUE(found);
		}

		array_deallocate(copy_ptr);
		array_deallocate(copy_int);
		array_deallocate(copy_obj);
		array_deallocate(copy_basic);
		array_deallocate(copy_combine);
	}

	// Erase single item, memcpy safe
	for (i = 0; i < 255; ++i) {
		basic_t basic;
		combine_t combine;

		memset(&basic, 0, sizeof(basic));
		basic.intval = i;
		basic.objval = (object_t)(i + 1);

		memset(&combine, 0, sizeof(combine));
		combine.basicval.intval = i + 2;
		combine.basicval.objval = (object_t)(i + 3);
		combine.charval = (char)(i + 4);
		combine.intval = i + 5;
		combine.ptrval = (void*)((uintptr_t)(i + 6));
		combine.unionval.basicval.intval = i + 7;
		combine.unionval.basicval.objval = (object_t)(i + 8);

		clamped_i = (unsigned int)math_clamp(129 - i, 0, (int)array_size(array_ptr));

		array_insert(array_ptr, clamped_i, (void*)((uintptr_t)i));
		array_insert(array_int, clamped_i, i);
		array_insert(array_obj, clamped_i, (object_t)i);
		array_insert_memcpy(array_basic, clamped_i, &basic);
		array_insert_memcpy(array_combine, clamped_i, &combine);
	}
	for (i = 0; i < 255; ++i) {
		void* ptrval;
		int intval;
		object_t objval;
		basic_t basic;
		combine_t combine;
		int small_neg = -1;
		int large_neg = -1234;
		int huge_neg = -123456;

		clamped_i = (unsigned int)math_clamp(129 - i, 0, (int)array_size(array_ptr));

		ptrval = array_ptr[clamped_i];
		intval = array_int[clamped_i];
		objval = array_obj[clamped_i];
		basic = array_basic[clamped_i];
		combine = array_combine[clamped_i];

		array_copy(copy_ptr, array_ptr);
		array_copy(copy_int, array_int);
		array_copy(copy_obj, array_obj);
		array_copy(copy_basic, array_basic);
		array_copy(copy_combine, array_combine);

		array_erase_memcpy_safe(array_ptr, clamped_i);
		array_erase_memcpy_safe(array_int, clamped_i);
		array_erase_memcpy_safe(array_obj, clamped_i);
		array_erase_memcpy_safe(array_basic, clamped_i);
		array_erase_memcpy_safe(array_combine, clamped_i);

		array_erase_memcpy_safe(array_ptr, large_neg);
		array_erase_memcpy_safe(array_int, small_neg);
		array_erase_memcpy_safe(array_obj, 1024);
		array_erase_memcpy_safe(array_basic, 123456);
		array_erase_memcpy_safe(array_combine, huge_neg);

		EXPECT_EQ(array_size(array_ptr), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_int), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_obj), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_basic), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_combine), (unsigned int)(254 - i));

		for (j = 0; j < 254 - i; ++j) {
			EXPECT_NE(array_ptr[j], ptrval);
			EXPECT_NE(array_int[j], intval);
			EXPECT_NE(array_obj[j], objval);
			EXPECT_NE(memcmp(array_basic + j, &basic, sizeof(basic)), 0);
			EXPECT_NE(memcmp(array_combine + j, &combine, sizeof(combine)), 0);
		}

		for (j = 0; j < (int)array_size(copy_ptr); ++j) {
			if (j == (int)clamped_i)
				continue;

			found = false;
			for (k = 0; k < 254 - i; ++k) {
				if (array_ptr[k] == copy_ptr[j]) {
					EXPECT_EQ(array_ptr[k], copy_ptr[j]);
					EXPECT_EQ(array_int[k], copy_int[j]);
					EXPECT_EQ(array_obj[k], copy_obj[j]);
					EXPECT_EQ(memcmp(array_basic + k, copy_basic + j, sizeof(basic)), 0);
					EXPECT_EQ(memcmp(array_combine + k, copy_combine + j, sizeof(combine)), 0);
					found = true;
					break;
				}
			}
			EXPECT_TRUE(found);
		}

		array_deallocate(copy_ptr);
		array_deallocate(copy_int);
		array_deallocate(copy_obj);
		array_deallocate(copy_basic);
		array_deallocate(copy_combine);
	}

	// Erase single item, ordered
	for (i = 0; i < 255; ++i) {
		basic_t basic;
		combine_t combine;

		memset(&basic, 0, sizeof(basic));
		basic.intval = i;
		basic.objval = (object_t)(i + 1);

		memset(&combine, 0, sizeof(combine));
		combine.basicval.intval = i + 2;
		combine.basicval.objval = (object_t)(i + 3);
		combine.charval = (char)(i + 4);
		combine.intval = i + 5;
		combine.ptrval = (void*)((uintptr_t)(i + 6));
		combine.unionval.basicval.intval = i + 7;
		combine.unionval.basicval.objval = (object_t)(i + 8);

		clamped_i = (unsigned int)math_clamp(129 - i, 0, (int)array_size(array_ptr));

		array_insert(array_ptr, clamped_i, (void*)((uintptr_t)i));
		array_insert(array_int, clamped_i, i);
		array_insert(array_obj, clamped_i, (object_t)i);
		array_insert_memcpy(array_basic, clamped_i, &basic);
		array_insert_memcpy(array_combine, clamped_i, &combine);
	}
	for (i = 0; i < 255; ++i) {
		void* ptrval;
		int intval;
		object_t objval;
		basic_t basic;
		combine_t combine;

		clamped_i = (unsigned int)math_clamp(129 - i, 0, (int)array_size(array_ptr));

		ptrval = array_ptr[clamped_i];
		intval = array_int[clamped_i];
		objval = array_obj[clamped_i];
		basic = array_basic[clamped_i];
		combine = array_combine[clamped_i];

		array_copy(copy_ptr, array_ptr);
		array_copy(copy_int, array_int);
		array_copy(copy_obj, array_obj);
		array_copy(copy_basic, array_basic);
		array_copy(copy_combine, array_combine);

		array_erase_ordered(array_ptr, clamped_i);
		array_erase_ordered(array_int, clamped_i);
		array_erase_ordered(array_obj, clamped_i);
		array_erase_ordered(array_basic, clamped_i);
		array_erase_ordered(array_combine, clamped_i);

		EXPECT_EQ(array_size(array_ptr), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_int), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_obj), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_basic), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_combine), (unsigned int)(254 - i));

		for (j = 0; j < 254 - i; ++j) {
			EXPECT_NE(array_ptr[j], ptrval);
			EXPECT_NE(array_int[j], intval);
			EXPECT_NE(array_obj[j], objval);
			EXPECT_NE(memcmp(array_basic + j, &basic, sizeof(basic)), 0);
			EXPECT_NE(memcmp(array_combine + j, &combine, sizeof(combine)), 0);
		}

		for (j = 0; j < (int)array_size(copy_ptr); ++j) {
			if (j == (int)clamped_i)
				continue;

			found = false;
			for (k = 0; k < 254 - i; ++k) {
				if (array_ptr[k] == copy_ptr[j]) {
					EXPECT_EQ(array_ptr[k], copy_ptr[j]);
					EXPECT_EQ(array_int[k], copy_int[j]);
					EXPECT_EQ(array_obj[k], copy_obj[j]);
					EXPECT_EQ(memcmp(array_basic + k, copy_basic + j, sizeof(basic)), 0);
					EXPECT_EQ(memcmp(array_combine + k, copy_combine + j, sizeof(combine)), 0);
					found = true;
					break;
				}
			}
			EXPECT_TRUE(found);
		}

		array_deallocate(copy_ptr);
		array_deallocate(copy_int);
		array_deallocate(copy_obj);
		array_deallocate(copy_basic);
		array_deallocate(copy_combine);
	}

	// Erase single item, ordered safe
	for (i = 0; i < 255; ++i) {
		basic_t basic;
		combine_t combine;

		memset(&basic, 0, sizeof(basic));
		basic.intval = i;
		basic.objval = (object_t)(i + 1);

		memset(&combine, 0, sizeof(combine));
		combine.basicval.intval = i + 2;
		combine.basicval.objval = (object_t)(i + 3);
		combine.charval = (char)(i + 4);
		combine.intval = i + 5;
		combine.ptrval = (void*)((uintptr_t)(i + 6));
		combine.unionval.basicval.intval = i + 7;
		combine.unionval.basicval.objval = (object_t)(i + 8);

		clamped_i = (unsigned int)math_clamp(129 - i, 0, (int)array_size(array_ptr));

		array_insert(array_ptr, clamped_i, (void*)((uintptr_t)i));
		array_insert(array_int, clamped_i, i);
		array_insert(array_obj, clamped_i, (object_t)i);
		array_insert_memcpy(array_basic, clamped_i, &basic);
		array_insert_memcpy(array_combine, clamped_i, &combine);
	}
	for (i = 0; i < 255; ++i) {
		void* ptrval;
		int intval;
		object_t objval;
		basic_t basic;
		combine_t combine;
		int small_neg = -1;
		int large_neg = -1234;
		int huge_neg = -123456;

		clamped_i = (unsigned int)math_clamp(129 - i, 0, (int)array_size(array_ptr));

		ptrval = array_ptr[clamped_i];
		intval = array_int[clamped_i];
		objval = array_obj[clamped_i];
		basic = array_basic[clamped_i];
		combine = array_combine[clamped_i];

		array_copy(copy_ptr, array_ptr);
		array_copy(copy_int, array_int);
		array_copy(copy_obj, array_obj);
		array_copy(copy_basic, array_basic);
		array_copy(copy_combine, array_combine);

		array_erase_ordered_safe(array_ptr, clamped_i);
		array_erase_ordered_safe(array_int, clamped_i);
		array_erase_ordered_safe(array_obj, clamped_i);
		array_erase_ordered_safe(array_basic, clamped_i);
		array_erase_ordered_safe(array_combine, clamped_i);

		array_erase_ordered_safe(array_ptr, large_neg);
		array_erase_ordered_safe(array_int, small_neg);
		array_erase_ordered_safe(array_obj, 1024);
		array_erase_ordered_safe(array_basic, 123456);
		array_erase_ordered_safe(array_combine, huge_neg);

		EXPECT_EQ(array_size(array_ptr), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_int), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_obj), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_basic), (unsigned int)(254 - i));
		EXPECT_EQ(array_size(array_combine), (unsigned int)(254 - i));

		for (j = 0; j < 254 - i; ++j) {
			EXPECT_NE(array_ptr[j], ptrval);
			EXPECT_NE(array_int[j], intval);
			EXPECT_NE(array_obj[j], objval);
			EXPECT_NE(memcmp(array_basic + j, &basic, sizeof(basic)), 0);
			EXPECT_NE(memcmp(array_combine + j, &combine, sizeof(combine)), 0);
		}

		for (j = 0; j < (int)array_size(copy_ptr); ++j) {
			if (j == (int)clamped_i)
				continue;

			found = false;
			for (k = 0; k < 254 - i; ++k) {
				if (array_ptr[k] == copy_ptr[j]) {
					EXPECT_EQ(array_ptr[k], copy_ptr[j]);
					EXPECT_EQ(array_int[k], copy_int[j]);
					EXPECT_EQ(array_obj[k], copy_obj[j]);
					EXPECT_EQ(memcmp(array_basic + k, copy_basic + j, sizeof(basic)), 0);
					EXPECT_EQ(memcmp(array_combine + k, copy_combine + j, sizeof(combine)), 0);
					found = true;
					break;
				}
			}
			EXPECT_TRUE(found);
		}

		array_deallocate(copy_ptr);
		array_deallocate(copy_int);
		array_deallocate(copy_obj);
		array_deallocate(copy_basic);
		array_deallocate(copy_combine);
	}

	array_deallocate(array_ptr);
	array_deallocate(array_int);
	array_deallocate(array_obj);
	array_deallocate(array_basic);
	array_deallocate(array_combine);

	// Range erase
	for (i = 0; i < 255; ++i) {
		basic_t basic;
		combine_t combine;

		memset(&basic, 0, sizeof(basic));
		basic.intval = i;
		basic.objval = (object_t)(i + 1);

		memset(&combine, 0, sizeof(combine));
		combine.basicval.intval = i + 2;
		combine.basicval.objval = (object_t)(i + 3);
		combine.charval = (char)(i + 4);
		combine.intval = i + 5;
		combine.ptrval = (void*)((uintptr_t)(i + 6));
		combine.unionval.basicval.intval = i + 7;
		combine.unionval.basicval.objval = (object_t)(i + 8);

		clamped_i = (unsigned int)math_clamp(129 - i, 0, (int)array_size(array_ptr));

		array_insert(array_ptr, clamped_i, (void*)((uintptr_t)i));
		array_insert(array_int, clamped_i, i);
		array_insert(array_obj, clamped_i, (object_t)i);
		array_insert_memcpy(array_basic, clamped_i, &basic);
		array_insert_memcpy(array_combine, clamped_i, &combine);
	}

	array_copy(copy_ptr, array_ptr);
	array_copy(copy_int, array_int);
	array_copy(copy_obj, array_obj);
	array_copy(copy_basic, array_basic);
	array_copy(copy_combine, array_combine);

	// Erasing 0 is always safe
	{
		unsigned int zero = 0;
		// Compiler catches negative (signed) values with -Weverything
#if !FOUNDATION_COMPILER_CLANG
		int small_neg = -1;
		int large_neg = -1234;
		array_erase_ordered_range(array_ptr, large_neg, zero);
		array_erase_ordered_range(array_int, small_neg, zero);
#endif
		array_erase_ordered_range(array_obj, 0, zero);
		array_erase_ordered_range(array_basic, 1, zero);
		array_erase_ordered_range(array_combine, 1234, zero);
	}

	EXPECT_EQ(array_size(array_ptr), 255);
	EXPECT_EQ(array_size(array_int), 255);
	EXPECT_EQ(array_size(array_obj), 255);
	EXPECT_EQ(array_size(array_basic), 255);
	EXPECT_EQ(array_size(array_combine), 255);

	for (i = 0; i < 255; ++i) {
		EXPECT_EQ(array_ptr[i], copy_ptr[i]);
		EXPECT_EQ(array_int[i], copy_int[i]);
		EXPECT_EQ(array_obj[i], copy_obj[i]);
		EXPECT_EQ(memcmp(array_basic + i, copy_basic + i, sizeof(basic_t)), 0);
		EXPECT_EQ(memcmp(array_combine + i, copy_combine + i, sizeof(combine_t)), 0);
	}

	// Erase first
	array_erase_ordered_range(array_ptr, 0, 1);
	array_erase_ordered_range(array_int, 0, 1);
	array_erase_ordered_range(array_obj, 0, 1);
	array_erase_ordered_range(array_basic, 0, 1);
	array_erase_ordered_range(array_combine, 0, 1);

	EXPECT_EQ(array_size(array_ptr), 254U);
	EXPECT_EQ(array_size(array_int), 254U);
	EXPECT_EQ(array_size(array_obj), 254U);
	EXPECT_EQ(array_size(array_basic), 254U);
	EXPECT_EQ(array_size(array_combine), 254U);

	for (i = 0; i < 254; ++i) {
		EXPECT_EQ(array_ptr[i], copy_ptr[i + 1]);
		EXPECT_EQ(array_int[i], copy_int[i + 1]);
		EXPECT_EQ(array_obj[i], copy_obj[i + 1]);
		EXPECT_EQ(memcmp(array_basic + i, copy_basic + i + 1, sizeof(basic_t)), 0);
		EXPECT_EQ(memcmp(array_combine + i, copy_combine + i + 1, sizeof(combine_t)), 0);
	}

	// Erase last
	array_erase_ordered_range(array_ptr, 253, 1);
	array_erase_ordered_range(array_int, 253, 1);
	array_erase_ordered_range(array_obj, 253, 1);
	array_erase_ordered_range(array_basic, 253, 1);
	array_erase_ordered_range(array_combine, 253, 1);

	EXPECT_EQ(array_size(array_ptr), 253);
	EXPECT_EQ(array_size(array_int), 253);
	EXPECT_EQ(array_size(array_obj), 253);
	EXPECT_EQ(array_size(array_basic), 253);
	EXPECT_EQ(array_size(array_combine), 253);

	for (i = 0; i < 253; ++i) {
		EXPECT_EQ(array_ptr[i], copy_ptr[i + 1]);
		EXPECT_EQ(array_int[i], copy_int[i + 1]);
		EXPECT_EQ(array_obj[i], copy_obj[i + 1]);
		EXPECT_EQ(memcmp(array_basic + i, copy_basic + i + 1, sizeof(basic_t)), 0);
		EXPECT_EQ(memcmp(array_combine + i, copy_combine + i + 1, sizeof(combine_t)), 0);
	}

	// Erase range
	array_erase_ordered_range(array_ptr, 37, 63);
	array_erase_ordered_range(array_int, 37, 63);
	array_erase_ordered_range(array_obj, 37, 63);
	array_erase_ordered_range(array_basic, 37, 63);
	array_erase_ordered_range(array_combine, 37, 63);

	EXPECT_EQ(array_size(array_ptr), 190);
	EXPECT_EQ(array_size(array_int), 190);
	EXPECT_EQ(array_size(array_obj), 190);
	EXPECT_EQ(array_size(array_basic), 190);
	EXPECT_EQ(array_size(array_combine), 190);

	for (i = 0; i < 37; ++i) {
		EXPECT_EQ(array_ptr[i], copy_ptr[i + 1]);
		EXPECT_EQ(array_int[i], copy_int[i + 1]);
		EXPECT_EQ(array_obj[i], copy_obj[i + 1]);
		EXPECT_EQ(memcmp(array_basic + i, copy_basic + i + 1, sizeof(basic_t)), 0);
		EXPECT_EQ(memcmp(array_combine + i, copy_combine + i + 1, sizeof(combine_t)), 0);
	}

	for (; i < 190; ++i) {
		EXPECT_EQ(array_ptr[i], copy_ptr[i + 64]);
		EXPECT_EQ(array_int[i], copy_int[i + 64]);
		EXPECT_EQ(array_obj[i], copy_obj[i + 64]);
		EXPECT_EQ(memcmp(array_basic + i, copy_basic + i + 64, sizeof(basic_t)), 0);
		EXPECT_EQ(memcmp(array_combine + i, copy_combine + i + 64, sizeof(combine_t)), 0);
	}

	// Erase range safe
	{
		int small_neg = -10;
		int large_neg = -1234;
		array_erase_ordered_range_safe(array_ptr, large_neg, large_neg);
		array_erase_ordered_range_safe(array_int, large_neg, -large_neg);
		array_erase_ordered_range_safe(array_obj, 10, small_neg);
		array_erase_ordered_range_safe(array_basic, 1234, small_neg);
		array_erase_ordered_range_safe(array_combine, 1234, 1234);
	}

	EXPECT_EQ(array_size(array_ptr), 190);
	EXPECT_EQ(array_size(array_int), 190);
	EXPECT_EQ(array_size(array_obj), 190);
	EXPECT_EQ(array_size(array_basic), 190);
	EXPECT_EQ(array_size(array_combine), 190);

	for (i = 0; i < 37; ++i) {
		EXPECT_EQ(array_ptr[i], copy_ptr[i + 1]);
		EXPECT_EQ(array_int[i], copy_int[i + 1]);
		EXPECT_EQ(array_obj[i], copy_obj[i + 1]);
		EXPECT_EQ(memcmp(array_basic + i, copy_basic + i + 1, sizeof(basic_t)), 0);
		EXPECT_EQ(memcmp(array_combine + i, copy_combine + i + 1, sizeof(combine_t)), 0);
	}

	for (; i < 190; ++i) {
		EXPECT_EQ(array_ptr[i], copy_ptr[i + 64]);
		EXPECT_EQ(array_int[i], copy_int[i + 64]);
		EXPECT_EQ(array_obj[i], copy_obj[i + 64]);
		EXPECT_EQ(memcmp(array_basic + i, copy_basic + i + 64, sizeof(basic_t)), 0);
		EXPECT_EQ(memcmp(array_combine + i, copy_combine + i + 64, sizeof(combine_t)), 0);
	}

	// Erase range safe overlap start
	array_erase_ordered_range_safe(array_ptr, -1234, 1236);
	array_erase_ordered_range_safe(array_int, -134, 136);
	array_erase_ordered_range_safe(array_obj, -1, 3);
	array_erase_ordered_range_safe(array_basic, 0, 2);
	array_erase_ordered_range_safe(array_combine, -234, 236);

	EXPECT_EQ(array_size(array_ptr), 188);
	EXPECT_EQ(array_size(array_int), 188);
	EXPECT_EQ(array_size(array_obj), 188);
	EXPECT_EQ(array_size(array_basic), 188);
	EXPECT_EQ(array_size(array_combine), 188);

	for (i = 0; i < 35; ++i) {
		EXPECT_EQ(array_ptr[i], copy_ptr[i + 3]);
		EXPECT_EQ(array_int[i], copy_int[i + 3]);
		EXPECT_EQ(array_obj[i], copy_obj[i + 3]);
		EXPECT_EQ(memcmp(array_basic + i, copy_basic + i + 3, sizeof(basic_t)), 0);
		EXPECT_EQ(memcmp(array_combine + i, copy_combine + i + 3, sizeof(combine_t)), 0);
	}

	for (; i < 188; ++i) {
		EXPECT_EQ(array_ptr[i], copy_ptr[i + 66]);
		EXPECT_EQ(array_int[i], copy_int[i + 66]);
		EXPECT_EQ(array_obj[i], copy_obj[i + 66]);
		EXPECT_EQ(memcmp(array_basic + i, copy_basic + i + 66, sizeof(basic_t)), 0);
		EXPECT_EQ(memcmp(array_combine + i, copy_combine + i + 66, sizeof(combine_t)), 0);
	}

	// Erase range safe overlap end
	array_erase_ordered_range_safe(array_ptr, 180, 1236);
	array_erase_ordered_range_safe(array_int, 180, 136);
	array_erase_ordered_range_safe(array_obj, 180, 8);
	array_erase_ordered_range_safe(array_basic, 180, 8);
	array_erase_ordered_range_safe(array_combine, 180, 180);

	EXPECT_EQ(array_size(array_ptr), 180);
	EXPECT_EQ(array_size(array_int), 180);
	EXPECT_EQ(array_size(array_obj), 180);
	EXPECT_EQ(array_size(array_basic), 180);
	EXPECT_EQ(array_size(array_combine), 180);

	for (i = 0; i < 35; ++i) {
		EXPECT_EQ(array_ptr[i], copy_ptr[i + 3]);
		EXPECT_EQ(array_int[i], copy_int[i + 3]);
		EXPECT_EQ(array_obj[i], copy_obj[i + 3]);
		EXPECT_EQ(memcmp(array_basic + i, copy_basic + i + 3, sizeof(basic_t)), 0);
		EXPECT_EQ(memcmp(array_combine + i, copy_combine + i + 3, sizeof(combine_t)), 0);
	}

	for (; i < 180; ++i) {
		EXPECT_EQ(array_ptr[i], copy_ptr[i + 66]);
		EXPECT_EQ(array_int[i], copy_int[i + 66]);
		EXPECT_EQ(array_obj[i], copy_obj[i + 66]);
		EXPECT_EQ(memcmp(array_basic + i, copy_basic + i + 66, sizeof(basic_t)), 0);
		EXPECT_EQ(memcmp(array_combine + i, copy_combine + i + 66, sizeof(combine_t)), 0);
	}

	// Erase range safe
	array_erase_ordered_range_safe(array_ptr, 100, 10);
	array_erase_ordered_range_safe(array_int, 100, 10);
	array_erase_ordered_range_safe(array_obj, 100, 10);
	array_erase_ordered_range_safe(array_basic, 100, 10);
	array_erase_ordered_range_safe(array_combine, 100, 10);

	EXPECT_EQ(array_size(array_ptr), 170);
	EXPECT_EQ(array_size(array_int), 170);
	EXPECT_EQ(array_size(array_obj), 170);
	EXPECT_EQ(array_size(array_basic), 170);
	EXPECT_EQ(array_size(array_combine), 170);

	for (i = 0; i < 35; ++i) {
		EXPECT_EQ(array_ptr[i], copy_ptr[i + 3]);
		EXPECT_EQ(array_int[i], copy_int[i + 3]);
		EXPECT_EQ(array_obj[i], copy_obj[i + 3]);
		EXPECT_EQ(memcmp(array_basic + i, copy_basic + i + 3, sizeof(basic_t)), 0);
		EXPECT_EQ(memcmp(array_combine + i, copy_combine + i + 3, sizeof(combine_t)), 0);
	}

	for (; i < 100; ++i) {
		EXPECT_EQ(array_ptr[i], copy_ptr[i + 66]);
		EXPECT_EQ(array_int[i], copy_int[i + 66]);
		EXPECT_EQ(array_obj[i], copy_obj[i + 66]);
		EXPECT_EQ(memcmp(array_basic + i, copy_basic + i + 66, sizeof(basic_t)), 0);
		EXPECT_EQ(memcmp(array_combine + i, copy_combine + i + 66, sizeof(combine_t)), 0);
	}

	for (; i < 170; ++i) {
		EXPECT_EQ(array_ptr[i], copy_ptr[i + 76]);
		EXPECT_EQ(array_int[i], copy_int[i + 76]);
		EXPECT_EQ(array_obj[i], copy_obj[i + 76]);
		EXPECT_EQ(memcmp(array_basic + i, copy_basic + i + 76, sizeof(basic_t)), 0);
		EXPECT_EQ(memcmp(array_combine + i, copy_combine + i + 76, sizeof(combine_t)), 0);
	}

	array_deallocate(copy_ptr);
	array_deallocate(copy_int);
	array_deallocate(copy_obj);
	array_deallocate(copy_basic);
	array_deallocate(copy_combine);

	array_deallocate(array_ptr);
	array_deallocate(array_int);
	array_deallocate(array_obj);
	array_deallocate(array_basic);
	array_deallocate(array_combine);

	return 0;
}

DECLARE_TEST(array, resize) {
	int* intarr = 0;
	int* prevarr = 0;

	array_resize(intarr, 139);

	EXPECT_NE(intarr, 0);
	EXPECT_EQ(array_size(intarr), 139);

	intarr[37] = 37;

	prevarr = intarr;
	array_resize(intarr, 139);

	EXPECT_EQ(intarr, prevarr);
	EXPECT_EQ(intarr[37], 37);
	EXPECT_EQ(array_size(intarr), 139);

	array_resize(intarr, 38);

	EXPECT_NE(intarr, 0);
	EXPECT_EQ(intarr[37], 37);
	EXPECT_EQ(array_size(intarr), 38);

	array_reserve(intarr, 738);
	prevarr = intarr;
	array_resize(intarr, 738);
	array_resize(intarr, 738);

	EXPECT_EQ(intarr, prevarr);
	EXPECT_EQ(intarr[37], 37);
	EXPECT_EQ(array_size(intarr), 738);

	array_resize(intarr, 0);

	EXPECT_NE(intarr, 0);
	EXPECT_EQ(array_size(intarr), 0);

	array_deallocate(intarr);

	EXPECT_EQ(intarr, 0);

	array_reserve(intarr, 15);
	EXPECT_EQ(array_capacity(intarr), 15);
	EXPECT_EQ(array_size(intarr), 0);

	array_grow(intarr, 2);
	EXPECT_EQ(array_capacity(intarr), 15);
	EXPECT_EQ(array_size(intarr), 2);

	array_grow(intarr, 15);
	EXPECT_EQ(array_capacity(intarr), 17);
	EXPECT_EQ(array_size(intarr), 17);

	array_deallocate(intarr);

	EXPECT_EQ(intarr, 0);

	return 0;
}

static void
test_array_declare(void) {
	ADD_TEST(array, allocation);
	ADD_TEST(array, copy);
	ADD_TEST(array, pushpop);
	ADD_TEST(array, inserterase);
	ADD_TEST(array, resize);
}

static test_suite_t test_array_suite = {test_array_application,
                                        test_array_memory_system,
                                        test_array_config,
                                        test_array_declare,
                                        test_array_initialize,
                                        test_array_finalize,
                                        0};

#if BUILD_MONOLITHIC

int
test_array_run(void);

int
test_array_run(void) {
	test_suite = test_array_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_array_suite;
}

#endif
