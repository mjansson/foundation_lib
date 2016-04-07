/* main.c  -  Foundation config test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
#include <test/test.h>

static hash_t write_section;
static hash_t write_key_0;
static hash_t write_key_1;
static hash_t write_key_2;
static hash_t write_key_3;
static hash_t write_key_4;
static hash_t write_key_5;
static hash_t write_key_6;
static hash_t write_key_7;

static string_const_t
string_mapper(hash_t hashval) {
	if (hashval == write_section) return string_const(STRING_CONST("write_test"));
	if (hashval == write_key_0) return string_const(STRING_CONST("key_0"));
	if (hashval == write_key_1) return string_const(STRING_CONST("key_1"));
	if (hashval == write_key_2) return string_const(STRING_CONST("key_2"));
	if (hashval == write_key_3) return string_const(STRING_CONST("key_3"));
	if (hashval == write_key_4) return string_const(STRING_CONST("key_4"));
	if (hashval == write_key_5) return string_const(STRING_CONST("key_5"));
	if (hashval == write_key_6) return string_const(STRING_CONST("key_6"));
	if (hashval == write_key_7) return string_const(STRING_CONST("key_7"));
	if (hashval == hash(STRING_CONST("base_key"))) return string_const(STRING_CONST("base_key"));
	if (hashval == hash(STRING_CONST("key"))) return string_const(STRING_CONST("key"));
	if (hashval == hash(STRING_CONST("first_section"))) return string_const(
		        STRING_CONST("first_section"));
	if (hashval == hash(STRING_CONST("notinvalidvalue"))) return string_const(
		            STRING_CONST("notinvalidvalue"));
	if (hashval == hash(STRING_CONST("emptyval"))) return string_const(STRING_CONST("emptyval"));
	if (hashval == hash(STRING_CONST("nonemptyval"))) return string_const(STRING_CONST("nonemptyval"));
	if (hashval == hash(STRING_CONST("section"))) return string_const(STRING_CONST("section"));
	if (hashval == hash(STRING_CONST("escapedstr"))) return string_const(STRING_CONST("escapedstr"));
	return string_null();
}

static application_t
test_config_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation config tests"));
	app.short_name = string_const(STRING_CONST("test_config"));
	app.company = string_const(STRING_CONST("Rampant Pixels"));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_config_memory_system(void) {
	return memory_system_malloc();
}

static int
test_config_initialize(void) {
	return 0;
}

static foundation_config_t
test_config_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static void
test_config_finalize(void) {
}

DECLARE_TEST(config, builtin) {
	//No built-ins yet
	return 0;
}

DECLARE_TEST(config, getset) {
	config_node_t root;

	hash_t invalid_section = hash("__section", 9);
	hash_t invalid_key = hash("__key", 5);

	hash_t test_section = hash("__test_config", 13);
	hash_t test_key = hash("__test_key", 10);

	config_initialize(&root);

	EXPECT_FALSE(config_bool(&root, invalid_section, invalid_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, invalid_section, invalid_key, HASH_NULL), 0);
	EXPECT_REALZERO(config_real(&root, invalid_section, invalid_key, HASH_NULL));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, invalid_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_bool(&root, false, test_section, test_key, HASH_ENVIRONMENT, HASH_NULL);
	config_set_bool(&root, false, test_section, HASH_NULL);
	config_set_bool(&root, true, test_section, test_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 1);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(1.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("true")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_TRUE);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_bool(&root, false, test_section, test_key, HASH_NULL);
	EXPECT_FALSE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("false")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_FALSE);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_int(&root, 0xdeadf00d, test_section, test_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0xdeadf00d);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), (real)0xdeadf00d);
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("3735941133")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("3735941133", 10));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_int(&root, -(int64_t)0x1001f00d, test_section, test_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), -(int64_t)0x1001f00d);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), -(real)0x1001f00d);
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("-268562445")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("-268562445", 10));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_int(&root, 0, test_section, test_key, HASH_NULL);
	EXPECT_FALSE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("0")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("0", 1));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_real(&root, REAL_C(1234.5678), test_section, test_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 1234);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(1234.5678));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_from_real_static(REAL_C(1234.5678), 4, 0, '0'));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL),
	          hash(STRING_ARGS(string_from_real_static(REAL_C(1234.5678), 4, 0, '0'))));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_real(&root, REAL_C(-1234.5678), test_section, test_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), -1234);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(-1234.5678));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_from_real_static(REAL_C(-1234.5678), 4, 0, '0'));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL),
	          hash(STRING_ARGS(string_from_real_static(REAL_C(-1234.5678), 4, 0, '0'))));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_real(&root, REAL_C(0.0), test_section, test_key, HASH_NULL);
	EXPECT_FALSE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("0")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("0", 1));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string(&root, STRING_CONST("stringvalue"), test_section, test_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("stringvalue")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("stringvalue", 11));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string(&root, STRING_CONST("1234"), test_section, test_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 1234);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(1234.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("1234")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("1234", 4));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string(&root, STRING_CONST("-1234.1"), test_section, test_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), -1234);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(-1234.1));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("-1234.1")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("-1234.1", 7));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string(&root, STRING_CONST(""), test_section, test_key, HASH_NULL);
	EXPECT_FALSE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string(&root, STRING_CONST("true"), test_section, test_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 1);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(1.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("true")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_TRUE);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string(&root, STRING_CONST("false"), test_section, test_key, HASH_NULL);
	EXPECT_FALSE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("false")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_FALSE);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string_constant(&root, STRING_CONST("stringvalue"), test_section, test_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("stringvalue")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("stringvalue", 11));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string_constant(&root, STRING_CONST("1234"), test_section, test_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 1234);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(1234.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("1234")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("1234", 4));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string_constant(&root, STRING_CONST("-1234.1"), test_section, test_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), -1234);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(-1234.1));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("-1234.1")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("-1234.1", 7));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string_constant(&root, STRING_CONST(""), test_section, test_key, HASH_NULL);
	EXPECT_FALSE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string_constant(&root, STRING_CONST("true"), test_section, test_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 1);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(1.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("true")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_TRUE);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string_constant(&root, STRING_CONST("false"), test_section, test_key, HASH_NULL);
	EXPECT_FALSE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("false")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_FALSE);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string_constant(&root, STRING_CONST("some random string"), test_section, test_key,
	                           HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("some random string")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL),
	          hash(STRING_CONST("some random string")));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string_constant(&root, nullptr, 0, test_section, test_key, HASH_NULL);
	EXPECT_FALSE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL), string_null());
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_finalize(&root);

	return 0;
}

DECLARE_TEST(config, variables) {
	config_node_t root;
	string_const_t refstr;

	hash_t invalid_section = hash(STRING_CONST("__section"));
	hash_t invalid_key = hash(STRING_CONST("__key"));

	hash_t test_section = hash(STRING_CONST("__test_expand"));
	hash_t test_key = hash(STRING_CONST("__test_var"));

	hash_t expand_section = hash(STRING_CONST("__expand_section"));
	hash_t expand_key = hash(STRING_CONST("__expand_key"));

	config_initialize(&root);

	EXPECT_FALSE(config_bool(&root, invalid_section, invalid_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, invalid_section, invalid_key, HASH_NULL), 0);
	EXPECT_REALZERO(config_real(&root, invalid_section, invalid_key, HASH_NULL));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, invalid_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string(&root, STRING_CONST("$(__expand_section:__expand_key)"), test_section, test_key,
	                  HASH_NULL);

	config_set_bool(&root, true, expand_section, expand_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 1);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(1.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("true")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_TRUE);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_bool(&root, false, expand_section, expand_key, HASH_NULL);
	EXPECT_FALSE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("false")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_FALSE);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_int(&root, 0xdeadf00d, expand_section, expand_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0xdeadf00d);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), (real)0xdeadf00d);
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("3735941133")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("3735941133", 10));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_int(&root, -(int64_t)0x1001f00d, expand_section, expand_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), -(int64_t)0x1001f00d);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), -(real)0x1001f00d);
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("-268562445")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("-268562445", 10));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_int(&root, 0, expand_section, expand_key, HASH_NULL);
	EXPECT_FALSE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("0")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("0", 1));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_real(&root, REAL_C(1234.5678), expand_section, expand_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 1234);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(1234.5678));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_from_real_static(REAL_C(1234.5678), 4, 0, '0'));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	refstr = string_from_real_static(REAL_C(1234.5678), 4, 0, '0');
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash(STRING_ARGS(refstr)));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_real(&root, REAL_C(-1234.5678), expand_section, expand_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), -1234);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(-1234.5678));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_from_real_static(REAL_C(-1234.5678), 4, 0, '0'));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	refstr = string_from_real_static(REAL_C(-1234.5678), 4, 0, '0');
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash(STRING_ARGS(refstr)));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_real(&root, REAL_C(0.0), expand_section, expand_key, HASH_NULL);
	EXPECT_FALSE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("0")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("0", 1));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string(&root, STRING_CONST("stringvalue"), expand_section, expand_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("stringvalue")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("stringvalue", 11));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string(&root, STRING_CONST("1234"), expand_section, expand_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 1234);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(1234.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("1234")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("1234", 4));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string(&root, STRING_CONST("-1234.1"), expand_section, expand_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), -1234);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(-1234.1));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("-1234.1")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("-1234.1", 7));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string(&root, STRING_CONST(""), expand_section, expand_key, HASH_NULL);
	EXPECT_FALSE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string(&root, STRING_CONST("true"), expand_section, expand_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 1);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(1.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("true")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_TRUE);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string(&root, STRING_CONST("false"), expand_section, expand_key, HASH_NULL);
	EXPECT_FALSE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("false")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_FALSE);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string_constant(&root, STRING_CONST("stringvalue"), expand_section, expand_key,
	                           HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("stringvalue")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("stringvalue", 11));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string_constant(&root, STRING_CONST("1234"), expand_section, expand_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 1234);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(1234.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("1234")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("1234", 4));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string_constant(&root, STRING_CONST("-1234.1"), expand_section, expand_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), -1234);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(-1234.1));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("-1234.1")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), hash("-1234.1", 7));
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string_constant(&root, STRING_CONST(""), expand_section, expand_key, HASH_NULL);
	EXPECT_FALSE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string_constant(&root, STRING_CONST("true"), expand_section, expand_key, HASH_NULL);
	EXPECT_TRUE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 1);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(1.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("true")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_TRUE);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_set_string_constant(&root, STRING_CONST("false"), expand_section, expand_key, HASH_NULL);
	EXPECT_FALSE(config_bool(&root, test_section, test_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, test_section, invalid_key, HASH_NULL));
	EXPECT_FALSE(config_bool(&root, invalid_section, test_key, HASH_NULL));
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, test_section, invalid_key, HASH_NULL), 0);
	EXPECT_EQ(config_int(&root, invalid_section, test_key, HASH_NULL), 0);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, test_section, invalid_key, HASH_NULL), REAL_C(0.0));
	EXPECT_REALEQ(config_real(&root, invalid_section, test_key, HASH_NULL), REAL_C(0.0));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("false")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, invalid_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_CONSTSTRINGEQ(config_string(&root, invalid_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_hash(&root, test_section, test_key, HASH_NULL), HASH_FALSE);
	EXPECT_EQ(config_hash(&root, test_section, invalid_key, HASH_NULL), HASH_EMPTY_STRING);
	EXPECT_EQ(config_hash(&root, invalid_section, test_key, HASH_NULL), HASH_EMPTY_STRING);

	config_finalize(&root);

	return 0;
}

DECLARE_TEST(config, numbers) {
	config_node_t root;

	hash_t test_section = hash("__test_numbers", 14);
	hash_t test_key = hash("__test_var", 10);

	config_initialize(&root);

	config_set_string(&root, STRING_CONST("1M"), test_section, test_key, HASH_NULL);
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 1024 * 1024);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), (real)(1024.0 * 1024.0));

	config_set_string_constant(&root, STRING_CONST("2M"), test_section, test_key, HASH_NULL);
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 2 * 1024 * 1024);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), (real)(2.0 * 1024.0 * 1024.0));

	config_set_string_constant(&root, STRING_CONST("0.1000m"), test_section, test_key, HASH_NULL);
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), (uint64_t)(0.1 * 1024.0 * 1024.0));
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), (real)(0.1 * 1024.0 * 1024.0));

	config_set_string_constant(&root, STRING_CONST("2.0M"), test_section, test_key, HASH_NULL);
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 2 * 1024 * 1024);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), (real)(2.0 * 1024.0 * 1024.0));

	config_set_string_constant(&root, STRING_CONST("2.5m"), test_section, test_key, HASH_NULL);
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 2 * 1024 * 1024 + 512 * 1024);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL),
	              (real)(2.0 * 1024.0 * 1024.0 + 512.0 * 1024.0));

	config_set_string(&root, STRING_CONST("2.5.M"), test_section, test_key, HASH_NULL);
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 2);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(2.5));

	config_set_string(&root, STRING_CONST("1k"), test_section, test_key, HASH_NULL);
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 1024);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(1024.0));

	config_set_string_constant(&root, STRING_CONST("2K"), test_section, test_key, HASH_NULL);
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 2 * 1024);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), (real)(2.0 * 1024.0));

	config_set_string_constant(&root, STRING_CONST("0.1000k"), test_section, test_key, HASH_NULL);
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), (uint64_t)(0.1 * 1024.0));
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), (real)(0.1 * 1024.0));

	config_set_string_constant(&root, STRING_CONST("2.0K"), test_section, test_key, HASH_NULL);
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 2 * 1024);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), (real)(2.0 * 1024.0));

	config_set_string_constant(&root, STRING_CONST("2.500k"), test_section, test_key, HASH_NULL);
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 2 * 1024 + 512);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), (real)(2.0 * 1024.0 + 512.0));

	config_set_string(&root, STRING_CONST("2.5.K"), test_section, test_key, HASH_NULL);
	EXPECT_EQ(config_int(&root, test_section, test_key, HASH_NULL), 2);
	EXPECT_REALEQ(config_real(&root, test_section, test_key, HASH_NULL), REAL_C(2.5));

	config_finalize(&root);

	return 0;
}

DECLARE_TEST(config, environment) {
	config_node_t root;

	hash_t test_section = hash("__test_env", 10);
	hash_t test_key = hash("__test_var", 10);

	config_initialize(&root);

	config_set_string(&root, STRING_CONST("$(environment:executable_name)"), test_section, test_key,
	                  HASH_NULL);
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     environment_executable_name());

	config_set_string(&root, STRING_CONST("$(environment:executable_directory)"), test_section,
	                  test_key, HASH_NULL);
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     environment_executable_directory());

	config_set_string(&root, STRING_CONST("$(environment:executable_path)"), test_section, test_key,
	                  HASH_NULL);
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     environment_executable_path());

	config_set_string(&root, STRING_CONST("$(environment:initial_working_directory)"), test_section,
	                  test_key, HASH_NULL);
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     environment_initial_working_directory());

	config_set_string(&root, STRING_CONST("$(environment:current_working_directory)"), test_section,
	                  test_key, HASH_NULL);
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     environment_current_working_directory());

	config_set_string(&root, STRING_CONST("$(environment:application_directory)"), test_section,
	                  test_key, HASH_NULL);
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     environment_application_directory());

	config_set_string(&root, STRING_CONST("$(environment:temporary_directory)"), test_section, test_key,
	                  HASH_NULL);
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     environment_temporary_directory());

	config_set_string(&root, STRING_CONST("$(environment:variable[HOME])"), test_section, test_key,
	                  HASH_NULL);
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     environment_variable(STRING_CONST("HOME")));

	config_set_string(&root, STRING_CONST("$(environment:variable[PATH])"), test_section, test_key,
	                  HASH_NULL);
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     environment_variable(STRING_CONST("PATH")));

	config_set_string(&root, STRING_CONST("$(environment:nonexisting)"), test_section, test_key,
	                  HASH_NULL);
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));

	config_set_string(&root, STRING_CONST("$(invalid_section_name:nonexisting)"), test_section,
	                  test_key, HASH_NULL);
	EXPECT_CONSTSTRINGEQ(config_string(&root, test_section, test_key, HASH_NULL),
	                     string_const(STRING_CONST("")));

	//Environment is only mapped in variable expansion
	EXPECT_CONSTSTRINGEQ(config_string(&root, HASH_ENVIRONMENT, HASH_APPLICATION_DIRECTORY, HASH_NULL),
	                     string_null());

	config_finalize(&root);

	return 0;
}

DECLARE_TEST(config, commandline) {
	config_node_t* root;

	const string_const_t cmdline[] = {
		string_const(STRING_CONST("--foo:bar=1234")),
		string_const(STRING_CONST("--not=valid")),
		string_const(STRING_CONST("---one:dash=toomany")),
		string_const(STRING_CONST("--first:set=1")),
		string_const(STRING_CONST("--first:set=")),
		string_const(STRING_CONST("--this:is=valid")),
		string_const(STRING_CONST("--a:boolean=true")),
		string_const(STRING_CONST("--a:nother=false")),
		string_const(STRING_CONST("--a:real=10.05")),
		string_const(STRING_CONST("--a:notnumber=1.0.1")),
		string_const(STRING_CONST("--a:number=101")),
		string_const(STRING_CONST("--a:quoted=\"foobar\"")),
		string_const(STRING_CONST("--this:is=notparsed"))
	};

	root = config_allocate();

	config_parse_commandline(root, cmdline, 12);   // Should skip last "--this:is=notparsed"

	EXPECT_CONSTSTRINGEQ(config_string(root, hash("foo", 3), hash("bar", 3), HASH_NULL),
	                     string_const(STRING_CONST("1234")));
	EXPECT_EQ(config_int(root, hash("foo", 3), hash("bar", 3), HASH_NULL), 1234);

	EXPECT_CONSTSTRINGEQ(config_string(root, hash("foo", 3), hash("not", 3), HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_int(root, hash("foo", 3), hash("not", 3), HASH_NULL), 0);

	EXPECT_CONSTSTRINGEQ(config_string(root, hash("not", 3), hash("valid", 5), HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_int(root, hash("not", 3), hash("valid", 5), HASH_NULL), 0);

	EXPECT_CONSTSTRINGEQ(config_string(root, hash("one", 3), hash("dash", 4), HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_int(root, hash("one", 3), hash("dash", 4), HASH_NULL), 0);

	EXPECT_CONSTSTRINGEQ(config_string(root, hash("first", 5), hash("set", 3), HASH_NULL),
	                     string_const(STRING_CONST("")));
	EXPECT_EQ(config_int(root, hash("first", 5), hash("set", 3), HASH_NULL), 0);

	EXPECT_CONSTSTRINGEQ(config_string(root, hash("this", 4), hash("is", 2), HASH_NULL),
	                     string_const(STRING_CONST("valid")));
	EXPECT_EQ(config_int(root, hash("this", 4), hash("is", 2), HASH_NULL), 0);

	EXPECT_CONSTSTRINGEQ(config_string(root, hash("a", 1), hash("boolean", 7), HASH_NULL),
	                     string_const(STRING_CONST("true")));
	EXPECT_EQ(config_int(root, hash("a", 1), hash("boolean", 7), HASH_NULL), 1);

	EXPECT_CONSTSTRINGEQ(config_string(root, hash("a", 1), hash("nother", 6), HASH_NULL),
	                     string_const(STRING_CONST("false")));
	EXPECT_EQ(config_int(root, hash("a", 1), hash("nother", 6), HASH_NULL), 0);

	EXPECT_CONSTSTRINGEQ(config_string(root, hash("a", 1), hash("real", 4), HASH_NULL),
	                     string_const(STRING_CONST("10.05")));
	EXPECT_REALEQ(config_real(root, hash("a", 1), hash("real", 4), HASH_NULL), REAL_C(10.05));

	EXPECT_CONSTSTRINGEQ(config_string(root, hash("a", 1), hash("notnumber", 9), HASH_NULL),
	                     string_const(STRING_CONST("1.0.1")));
	EXPECT_EQ(config_int(root, hash("a", 1), hash("notnumber", 9), HASH_NULL), 1);
	EXPECT_REALEQ(config_real(root, hash("a", 1), hash("notnumber", 9), HASH_NULL), REAL_C(1.0));

	EXPECT_CONSTSTRINGEQ(config_string(root, hash("a", 1), hash("number", 6), HASH_NULL),
	                     string_const(STRING_CONST("101")));
	EXPECT_EQ(config_int(root, hash("a", 1), hash("number", 6), HASH_NULL), 101);
	EXPECT_REALEQ(config_real(root, hash("a", 1), hash("number", 6), HASH_NULL), REAL_C(101.0));

	EXPECT_CONSTSTRINGEQ(config_string(root, hash("a", 1), hash("quoted", 6), HASH_NULL),
	                     string_const(STRING_CONST("foobar")));
	EXPECT_EQ(config_int(root, hash("a", 1), hash("quoted", 6), HASH_NULL), 0);

	config_deallocate(root);

	return 0;
}

DECLARE_TEST(config, readwrite) {
	stream_t* stream = fs_temporary_file();
	config_node_t* root = config_allocate();
	config_node_t* node;

	write_section = hash("write_test", 10);
	write_key_0 = hash("key_0", 5);
	write_key_1 = hash("key_1", 5);
	write_key_2 = hash("key_2", 5);
	write_key_3 = hash("key_3", 5);
	write_key_4 = hash("key_4", 5);
	write_key_5 = hash("key_5", 5);
	write_key_6 = hash("key_6", 5);
	write_key_7 = hash("key_7", 5);

	stream_set_binary(stream, false);

	config_set_string(root, STRING_CONST("foobar"), write_section, write_key_0, HASH_NULL);
	config_set_string_constant(root, STRING_CONST("another string"), write_section, write_key_1,
	                           HASH_NULL);
	config_set_int(root, 1234, write_section, write_key_2, HASH_NULL);
	config_set_real(root, REAL_C(12.34), write_section, write_key_3, HASH_NULL);
	config_set_bool(root, true, write_section, write_key_4, HASH_NULL);
	config_set_bool(root, false, write_section, write_key_5, HASH_NULL);
	config_set_string(root, STRING_CONST("$(environment:initial_working_directory)"), write_section,
	                  write_key_6, HASH_NULL);
	config_set_string_constant(root, STRING_CONST("98765"), write_section, write_key_7, HASH_NULL);

	node = config_node(root, write_section, HASH_NULL);
	config_write(node, stream, string_mapper);
	/*stream_seek( stream, 0, STREAM_SEEK_BEGIN );
	do
	{
		char* line = stream_read_line( stream, '\n' );
		log_infof( HASH_TEST, "%s", line );
		string_deallocate( line );
	} while( !stream_eos( stream ) );*/

	config_set_string(root, STRING_CONST("asdf"), write_section, write_key_0, HASH_NULL);
	config_set_string_constant(root, STRING_CONST("qwerty"), write_section, write_key_1, HASH_NULL);
	config_set_int(root, 54321, write_section, write_key_2, HASH_NULL);
	config_set_real(root, REAL_C(32.78), write_section, write_key_3, HASH_NULL);
	config_set_bool(root, false, write_section, write_key_4, HASH_NULL);
	config_set_bool(root, true, write_section, write_key_5, HASH_NULL);
	config_set_string(root, STRING_CONST("notavariable"), write_section, write_key_6, HASH_NULL);
	config_set_string_constant(root, STRING_CONST("12345"), write_section, write_key_7, HASH_NULL);

	stream_seek(stream, 0, STREAM_SEEK_BEGIN);
	config_parse(node, stream, false);

	EXPECT_CONSTSTRINGEQ(config_string(root, write_section, write_key_0, HASH_NULL),
	                     string_const(STRING_CONST("asdf")));
	EXPECT_CONSTSTRINGEQ(config_string(root, write_section, write_key_1, HASH_NULL),
	                     string_const(STRING_CONST("qwerty")));
	EXPECT_EQ(config_int(root, write_section, write_key_2, HASH_NULL), 54321);
	EXPECT_REALEQ(config_real(root, write_section, write_key_3, HASH_NULL), REAL_C(32.78));
	EXPECT_EQ(config_bool(root, write_section, write_key_4, HASH_NULL), false);
	EXPECT_EQ(config_bool(root, write_section, write_key_5, HASH_NULL), true);
	EXPECT_CONSTSTRINGEQ(config_string(root, write_section, write_key_6, HASH_NULL),
	                     string_const(STRING_CONST("notavariable")));
	EXPECT_CONSTSTRINGEQ(config_string(root, write_section, write_key_7, HASH_NULL),
	                     string_const(STRING_CONST("12345")));

	stream_seek(stream, 0, STREAM_SEEK_BEGIN);
	config_parse(node, stream, true);

	EXPECT_CONSTSTRINGEQ(config_string(root, write_section, write_key_0, HASH_NULL),
	                     string_const(STRING_CONST("foobar")));
	EXPECT_CONSTSTRINGEQ(config_string(root, write_section, write_key_1, HASH_NULL),
	                     string_const(STRING_CONST("another string")));
	EXPECT_EQ(config_int(root, write_section, write_key_2, HASH_NULL), 1234);
	EXPECT_REALEQ(config_real(root, write_section, write_key_3, HASH_NULL), REAL_C(12.34));
	EXPECT_EQ(config_bool(root, write_section, write_key_4, HASH_NULL), true);
	EXPECT_EQ(config_bool(root, write_section, write_key_5, HASH_NULL), false);
	EXPECT_CONSTSTRINGEQ(config_string(root, write_section, write_key_6, HASH_NULL),
	                     environment_initial_working_directory());
	EXPECT_CONSTSTRINGEQ(config_string(root, write_section, write_key_7, HASH_NULL),
	                     string_const(STRING_CONST("98765")));

	stream_seek(stream, 0, STREAM_SEEK_BEGIN);
	stream_truncate(stream, 0);

	stream_write_string(stream, STRING_CONST(
	                        "base_key = \"some value\"\n"
	                        "first_section = { key = avalue }\n"
	                        "notinvalidvalue = notinvalidvalue\n"
	                        "emptyval = \"\"\n"
	                        "nonemptyval = true\n"
	                        "section = {\n"
	                        "	nonemptyval = 1.0\n"
	                        "   escapedstr = \"{foo[]\\\"bar\\\"}\"\n"
	                        "}\n"));
	stream_seek(stream, 0, STREAM_SEEK_BEGIN);

	log_enable_stdout(false);
	config_parse(root, stream, true);
	log_enable_stdout(true);

	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("base_key")), HASH_NULL),
	                     string_const(STRING_CONST("some value")));
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("first_section")),
	                                   hash(STRING_CONST("key")), HASH_NULL),
	                     string_const(STRING_CONST("avalue")));
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("key")), HASH_NULL),
	                     string_null());
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("notinvalidvalue")), HASH_NULL),
	                     string_const(STRING_CONST("notinvalidvalue")));
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("invalidsection")),
	                                   hash(STRING_CONST("invalidvalue")), HASH_NULL),
	                     string_null());
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("emptyval")), HASH_NULL),
	                     string_empty());
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("invalidsection")),
	                                   hash(STRING_CONST("emptyval")), HASH_NULL),
	                     string_empty());
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("nonemptyval")), HASH_NULL),
	                     string_const(STRING_CONST("true")));
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("invalidsection")),
	                                   hash(STRING_CONST("nonemptyval")), HASH_NULL),
	                     string_empty());
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("section")),
	                                   hash(STRING_CONST("nonemptyval")), HASH_NULL),
	                     string_const(STRING_CONST("1")));
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("section")),
	                                   hash(STRING_CONST("escapedstr")), HASH_NULL),
	                     string_const(STRING_CONST("{foo[]\"bar\"}")));

	stream_seek(stream, 0, STREAM_SEEK_BEGIN);
	stream_truncate(stream, 0);

	config_write(root, stream, string_mapper);
	config_deallocate(root);

	root = config_allocate();

	log_enable_stdout(false);
	stream_seek(stream, 0, STREAM_SEEK_BEGIN);
	config_parse(root, stream, true);
	log_enable_stdout(true);

	EXPECT_CONSTSTRINGEQ(config_string(root, write_section, write_key_0, HASH_NULL),
	                     string_const(STRING_CONST("foobar")));
	EXPECT_CONSTSTRINGEQ(config_string(root, write_section, write_key_1, HASH_NULL),
	                     string_const(STRING_CONST("another string")));
	EXPECT_EQ(config_int(root, write_section, write_key_2, HASH_NULL), 1234);
	EXPECT_REALEQ(config_real(root, write_section, write_key_3, HASH_NULL), REAL_C(12.34));
	EXPECT_EQ(config_bool(root, write_section, write_key_4, HASH_NULL), true);
	EXPECT_EQ(config_bool(root, write_section, write_key_5, HASH_NULL), false);
	EXPECT_CONSTSTRINGEQ(config_string(root, write_section, write_key_6, HASH_NULL),
	                     environment_initial_working_directory());
	EXPECT_CONSTSTRINGEQ(config_string(root, write_section, write_key_7, HASH_NULL),
	                     string_const(STRING_CONST("98765")));
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("base_key")), HASH_NULL),
	                     string_const(STRING_CONST("some value")));
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("first_section")),
	                                   hash(STRING_CONST("key")), HASH_NULL),
	                     string_const(STRING_CONST("avalue")));
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("key")), HASH_NULL),
	                     string_null());
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("notinvalidvalue")), HASH_NULL),
	                     string_const(STRING_CONST("notinvalidvalue")));
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("invalidsection")),
	                                   hash(STRING_CONST("invalidvalue")), HASH_NULL),
	                     string_null());
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("emptyval")), HASH_NULL),
	                     string_empty());
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("invalidsection")),
	                                   hash(STRING_CONST("emptyval")), HASH_NULL),
	                     string_empty());
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("nonemptyval")), HASH_NULL),
	                     string_const(STRING_CONST("true")));
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("invalidsection")),
	                                   hash(STRING_CONST("nonemptyval")), HASH_NULL),
	                     string_empty());
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("section")),
	                                   hash(STRING_CONST("nonemptyval")), HASH_NULL),
	                     string_const(STRING_CONST("1")));
	EXPECT_CONSTSTRINGEQ(config_string(root, hash(STRING_CONST("section")),
	                                   hash(STRING_CONST("escapedstr")), HASH_NULL),
	                     string_const(STRING_CONST("{foo[]\"bar\"}")));

	stream_deallocate(stream);
	config_deallocate(root);

	return 0;
}

static void
test_config_declare(void) {
	ADD_TEST(config, builtin);
	ADD_TEST(config, getset);
	ADD_TEST(config, variables);
	ADD_TEST(config, numbers);
	ADD_TEST(config, environment);
	ADD_TEST(config, commandline);
	ADD_TEST(config, readwrite);
}

static test_suite_t test_config_suite = {
	test_config_application,
	test_config_memory_system,
	test_config_config,
	test_config_declare,
	test_config_initialize,
	test_config_finalize
};

#if BUILD_MONOLITHIC

int
test_config_run(void);

int
test_config_run(void) {
	test_suite = test_config_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_config_suite;
}

#endif
