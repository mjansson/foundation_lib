/* main.c  -  Foundation JSON test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

static application_t
test_json_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation JSON tests"));
	app.short_name = string_const(STRING_CONST("test_json"));
	app.config_dir = string_const(STRING_CONST("test"));
	app.company = string_const(STRING_CONST("Rampant Pixels"));
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}

static memory_system_t
test_json_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_json_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_json_initialize(void) {
	return 0;
}

static void
test_json_finalize(void) {
}

static json_token_t tokens[128];

DECLARE_TEST(json, reference) {
	size_t capacity = sizeof(tokens) / sizeof(tokens[0]);

	string_const_t compound = string_const(STRING_CONST("\
	{\"foo\" :{\"subobj\": false ,\
		\"val\" :1.2345e45 \
	} ,\"arr\" :[ \
		\"string\",\
		0.34523e-78,[\
			true, \
			\"subarr [] {} =:\", { \"key\": []}, [] \
		],[false],\
		{ \t\
			\"final\" : true \
		}\
		,{ }  \
		, 1234.43E+123 \
	]\
	}"));

	EXPECT_SIZEEQ(json_parse(0, 0, tokens, capacity), 0);
	EXPECT_EQ(tokens[0].type, JSON_UNDEFINED);
	EXPECT_SIZEEQ(json_parse(STRING_CONST("\"test\""), tokens, capacity), 1);
	EXPECT_EQ(tokens[0].type, JSON_STRING);
	EXPECT_CONSTSTRINGEQ(json_token_value("\"test\"", tokens + 0), string_const(STRING_CONST("test")));
	EXPECT_SIZEEQ(json_parse(STRING_CONST("\"test\""), nullptr, 0), 1);
	EXPECT_SIZEEQ(json_parse(STRING_CONST("[\"test\"]"), tokens, capacity), 2);
	EXPECT_SIZEEQ(json_parse(STRING_CONST("[\"test\"]"), tokens, 1), 2);
	EXPECT_SIZEEQ(json_parse(STRING_CONST("[\"test\"]"), nullptr, 0), 2);
	EXPECT_SIZEEQ(json_parse(STRING_CONST("{\"test\" : true}"), tokens, capacity), 2);
	EXPECT_SIZEEQ(json_parse(STRING_CONST("{\"test\" : true }"), tokens, 1), 2);
	EXPECT_SIZEEQ(json_parse(STRING_CONST("{\"test\" :true}"), nullptr, 0), 2);

	EXPECT_SIZEEQ(json_parse(STRING_ARGS(compound), nullptr, 0), 19);
	EXPECT_SIZEEQ(json_parse(STRING_ARGS(compound), tokens, 7), 19);
	EXPECT_INTEQ(tokens[0].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[0].id, 0);
	EXPECT_UINTEQ(tokens[0].id_length, 0);
	EXPECT_UINTEQ(tokens[0].value, 0);
	EXPECT_UINTEQ(tokens[0].value_length, 0);
	EXPECT_INTEQ(tokens[1].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[1].id_length, 3);
	EXPECT_UINTEQ(tokens[1].value_length, 0);
	EXPECT_INTEQ(tokens[2].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[2].id_length, 6);
	EXPECT_UINTEQ(tokens[2].value_length, 5);
	EXPECT_INTEQ(tokens[3].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[3].id_length, 3);
	EXPECT_UINTEQ(tokens[3].value_length, 9);
	EXPECT_INTEQ(tokens[4].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[4].id_length, 3);
	EXPECT_UINTEQ(tokens[4].value_length, 0);
	EXPECT_INTEQ(tokens[5].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[5].id_length, 0);
	EXPECT_UINTEQ(tokens[5].value_length, 6);
	EXPECT_INTEQ(tokens[6].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[6].id_length, 0);
	EXPECT_UINTEQ(tokens[6].value_length, 11);
	EXPECT_SIZEEQ(json_parse(STRING_ARGS(compound), tokens, capacity), 19);
	EXPECT_INTEQ(tokens[0].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[0].id, 0);
	EXPECT_UINTEQ(tokens[0].id_length, 0);
	EXPECT_UINTEQ(tokens[0].value, 0);
	EXPECT_UINTEQ(tokens[0].value_length, 0);
	EXPECT_INTEQ(tokens[1].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[1].id_length, 3);
	EXPECT_UINTEQ(tokens[1].value_length, 0);
	EXPECT_INTEQ(tokens[2].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[2].id_length, 6);
	EXPECT_UINTEQ(tokens[2].value_length, 5);
	EXPECT_INTEQ(tokens[3].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[3].id_length, 3);
	EXPECT_UINTEQ(tokens[3].value_length, 9);
	EXPECT_INTEQ(tokens[4].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[4].id_length, 3);
	EXPECT_UINTEQ(tokens[4].value_length, 0);
	EXPECT_INTEQ(tokens[5].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[5].id_length, 0);
	EXPECT_UINTEQ(tokens[5].value_length, 6);
	EXPECT_INTEQ(tokens[6].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[6].id_length, 0);
	EXPECT_UINTEQ(tokens[6].value_length, 11);
	EXPECT_INTEQ(tokens[7].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[7].id_length, 0);
	EXPECT_UINTEQ(tokens[7].value_length, 0);
	EXPECT_INTEQ(tokens[8].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[8].id_length, 0);
	EXPECT_UINTEQ(tokens[8].value_length, 4);
	EXPECT_INTEQ(tokens[9].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[9].id_length, 0);
	EXPECT_UINTEQ(tokens[9].value_length, 15);
	EXPECT_INTEQ(tokens[10].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[10].id_length, 0);
	EXPECT_UINTEQ(tokens[10].value_length, 0);
	EXPECT_INTEQ(tokens[11].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[11].id_length, 3);
	EXPECT_UINTEQ(tokens[11].value_length, 0);
	EXPECT_INTEQ(tokens[12].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[12].id_length, 0);
	EXPECT_UINTEQ(tokens[12].value_length, 0);
	EXPECT_INTEQ(tokens[13].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[13].id_length, 0);
	EXPECT_UINTEQ(tokens[13].value_length, 0);
	EXPECT_INTEQ(tokens[14].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[14].id_length, 0);
	EXPECT_UINTEQ(tokens[14].value_length, 5);
	EXPECT_INTEQ(tokens[15].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[15].id_length, 0);
	EXPECT_UINTEQ(tokens[15].value_length, 0);
	EXPECT_INTEQ(tokens[16].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[16].id_length, 5);
	EXPECT_UINTEQ(tokens[16].value_length, 4);
	EXPECT_INTEQ(tokens[17].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[17].id_length, 0);
	EXPECT_UINTEQ(tokens[17].value_length, 0);
	EXPECT_INTEQ(tokens[18].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[18].id_length, 0);
	EXPECT_UINTEQ(tokens[18].value_length, 12);

	return 0;
}

DECLARE_TEST(json, simplified) {
	size_t capacity = sizeof(tokens) / sizeof(tokens[0]);

	string_const_t simplified = string_const(STRING_CONST("\
	foo ={subobj= false \
		val =1.2345e45 \
	} arr =[\
		string\
		0.34523e-78 [\
			true\
			\"subarr [] {} =:\" { key: []} []\
		] [false] \
		{ \t\
			final = true\
		}\
		{ }  \
		1234.43E+123 \
	]\
	"));

	string_const_t compound = string_const(STRING_CONST("\
	{\"foo\" :{\"subobj\": false ,\
		\"val\" :1.2345e45 \
	} ,\"arr\" :[ \
		\"string\",\
		0.34523e-78,[\
			true, \
			\"subarr [] {} =:\", { \"key\": []}, [] \
		],[false],\
		{ \t\
			\"final\" : true \
		}\
		,{ } , \
		 1234.43E+123 \
	]\
	}"));

	EXPECT_SIZEEQ(sjson_parse(STRING_CONST("{\"test\" : true}"), tokens, capacity), 2);
	EXPECT_SIZEEQ(sjson_parse(STRING_CONST("{\"test\" : true }"), tokens, 1), 2);
	EXPECT_SIZEEQ(sjson_parse(STRING_CONST("{\"test\" :true}"), nullptr, 0), 2);

	EXPECT_SIZEEQ(sjson_parse(STRING_ARGS(compound), nullptr, 0), 19);
	EXPECT_SIZEEQ(sjson_parse(STRING_ARGS(compound), tokens, 7), 19);
	EXPECT_INTEQ(tokens[0].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[0].id, 0);
	EXPECT_UINTEQ(tokens[0].id_length, 0);
	EXPECT_UINTEQ(tokens[0].value, 0);
	EXPECT_UINTEQ(tokens[0].value_length, 0);
	EXPECT_INTEQ(tokens[1].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[1].id_length, 3);
	EXPECT_UINTEQ(tokens[1].value_length, 0);
	EXPECT_INTEQ(tokens[2].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[2].id_length, 6);
	EXPECT_UINTEQ(tokens[2].value_length, 5);
	EXPECT_INTEQ(tokens[3].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[3].id_length, 3);
	EXPECT_UINTEQ(tokens[3].value_length, 9);
	EXPECT_INTEQ(tokens[4].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[4].id_length, 3);
	EXPECT_UINTEQ(tokens[4].value_length, 0);
	EXPECT_INTEQ(tokens[5].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[5].id_length, 0);
	EXPECT_UINTEQ(tokens[5].value_length, 6);
	EXPECT_INTEQ(tokens[6].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[6].id_length, 0);
	EXPECT_UINTEQ(tokens[6].value_length, 11);
	EXPECT_SIZEEQ(sjson_parse(STRING_ARGS(compound), tokens, capacity), 19);
	EXPECT_INTEQ(tokens[0].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[0].id, 0);
	EXPECT_UINTEQ(tokens[0].id_length, 0);
	EXPECT_UINTEQ(tokens[0].value, 0);
	EXPECT_UINTEQ(tokens[0].value_length, 0);
	EXPECT_INTEQ(tokens[1].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[1].id_length, 3);
	EXPECT_UINTEQ(tokens[1].value_length, 0);
	EXPECT_INTEQ(tokens[2].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[2].id_length, 6);
	EXPECT_UINTEQ(tokens[2].value_length, 5);
	EXPECT_INTEQ(tokens[3].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[3].id_length, 3);
	EXPECT_UINTEQ(tokens[3].value_length, 9);
	EXPECT_INTEQ(tokens[4].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[4].id_length, 3);
	EXPECT_UINTEQ(tokens[4].value_length, 0);
	EXPECT_INTEQ(tokens[5].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[5].id_length, 0);
	EXPECT_UINTEQ(tokens[5].value_length, 6);
	EXPECT_INTEQ(tokens[6].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[6].id_length, 0);
	EXPECT_UINTEQ(tokens[6].value_length, 11);
	EXPECT_INTEQ(tokens[7].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[7].id_length, 0);
	EXPECT_UINTEQ(tokens[7].value_length, 0);
	EXPECT_INTEQ(tokens[8].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[8].id_length, 0);
	EXPECT_UINTEQ(tokens[8].value_length, 4);
	EXPECT_INTEQ(tokens[9].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[9].id_length, 0);
	EXPECT_UINTEQ(tokens[9].value_length, 15);
	EXPECT_INTEQ(tokens[10].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[10].id_length, 0);
	EXPECT_UINTEQ(tokens[10].value_length, 0);
	EXPECT_INTEQ(tokens[11].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[11].id_length, 3);
	EXPECT_UINTEQ(tokens[11].value_length, 0);
	EXPECT_INTEQ(tokens[12].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[12].id_length, 0);
	EXPECT_UINTEQ(tokens[12].value_length, 0);
	EXPECT_INTEQ(tokens[13].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[13].id_length, 0);
	EXPECT_UINTEQ(tokens[13].value_length, 0);
	EXPECT_INTEQ(tokens[14].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[14].id_length, 0);
	EXPECT_UINTEQ(tokens[14].value_length, 5);
	EXPECT_INTEQ(tokens[15].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[15].id_length, 0);
	EXPECT_UINTEQ(tokens[15].value_length, 0);
	EXPECT_INTEQ(tokens[16].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[16].id_length, 5);
	EXPECT_UINTEQ(tokens[16].value_length, 4);
	EXPECT_INTEQ(tokens[17].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[17].id_length, 0);
	EXPECT_UINTEQ(tokens[17].value_length, 0);
	EXPECT_INTEQ(tokens[18].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[18].id_length, 0);
	EXPECT_UINTEQ(tokens[18].value_length, 12);

	EXPECT_SIZEEQ(sjson_parse(0, 0, tokens, capacity), 0);
	EXPECT_EQ(tokens[0].type, JSON_OBJECT);
	EXPECT_SIZEEQ(sjson_parse(STRING_CONST("test = [test]"), tokens, capacity), 3);
	EXPECT_SIZEEQ(sjson_parse(STRING_CONST(" test = [test ]"), tokens, 1), 3);
	EXPECT_SIZEEQ(sjson_parse(STRING_CONST("test = [ test]"), nullptr, 0), 3);
	EXPECT_SIZEEQ(sjson_parse(STRING_CONST("test = { test = true}"), tokens, capacity), 3);
	EXPECT_SIZEEQ(sjson_parse(STRING_CONST(" test = {test= true }"), tokens, 1), 3);
	EXPECT_SIZEEQ(sjson_parse(STRING_CONST("test = {test =true}"), nullptr, 0), 3);

	EXPECT_SIZEEQ(sjson_parse(STRING_ARGS(simplified), nullptr, 0), 19);
	EXPECT_SIZEEQ(sjson_parse(STRING_ARGS(simplified), tokens, 7), 19);
	EXPECT_INTEQ(tokens[0].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[0].id, 0);
	EXPECT_UINTEQ(tokens[0].id_length, 0);
	EXPECT_UINTEQ(tokens[0].value, 0);
	EXPECT_UINTEQ(tokens[0].value_length, 0);
	EXPECT_INTEQ(tokens[1].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[1].id_length, 3);
	EXPECT_UINTEQ(tokens[1].value_length, 0);
	EXPECT_INTEQ(tokens[2].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[2].id_length, 6);
	EXPECT_UINTEQ(tokens[2].value_length, 5);
	EXPECT_INTEQ(tokens[3].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[3].id_length, 3);
	EXPECT_UINTEQ(tokens[3].value_length, 9);
	EXPECT_INTEQ(tokens[4].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[4].id_length, 3);
	EXPECT_UINTEQ(tokens[4].value_length, 0);
	EXPECT_INTEQ(tokens[5].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[5].id_length, 0);
	EXPECT_UINTEQ(tokens[5].value_length, 6);
	EXPECT_INTEQ(tokens[6].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[6].id_length, 0);
	EXPECT_UINTEQ(tokens[6].value_length, 11);
	EXPECT_SIZEEQ(sjson_parse(STRING_ARGS(simplified), tokens, capacity), 19);
	EXPECT_INTEQ(tokens[0].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[0].id, 0);
	EXPECT_UINTEQ(tokens[0].id_length, 0);
	EXPECT_UINTEQ(tokens[0].value, 0);
	EXPECT_UINTEQ(tokens[0].value_length, 0);
	EXPECT_INTEQ(tokens[1].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[1].id_length, 3);
	EXPECT_UINTEQ(tokens[1].value_length, 0);
	EXPECT_INTEQ(tokens[2].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[2].id_length, 6);
	EXPECT_UINTEQ(tokens[2].value_length, 5);
	EXPECT_INTEQ(tokens[3].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[3].id_length, 3);
	EXPECT_UINTEQ(tokens[3].value_length, 9);
	EXPECT_INTEQ(tokens[4].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[4].id_length, 3);
	EXPECT_UINTEQ(tokens[4].value_length, 0);
	EXPECT_INTEQ(tokens[5].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[5].id_length, 0);
	EXPECT_UINTEQ(tokens[5].value_length, 6);
	EXPECT_INTEQ(tokens[6].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[6].id_length, 0);
	EXPECT_UINTEQ(tokens[6].value_length, 11);
	EXPECT_INTEQ(tokens[7].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[7].id_length, 0);
	EXPECT_UINTEQ(tokens[7].value_length, 0);
	EXPECT_INTEQ(tokens[8].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[8].id_length, 0);
	EXPECT_UINTEQ(tokens[8].value_length, 4);
	EXPECT_INTEQ(tokens[9].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[9].id_length, 0);
	EXPECT_UINTEQ(tokens[9].value_length, 15);
	EXPECT_INTEQ(tokens[10].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[10].id_length, 0);
	EXPECT_UINTEQ(tokens[10].value_length, 0);
	EXPECT_INTEQ(tokens[11].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[11].id_length, 3);
	EXPECT_UINTEQ(tokens[11].value_length, 0);
	EXPECT_INTEQ(tokens[12].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[12].id_length, 0);
	EXPECT_UINTEQ(tokens[12].value_length, 0);
	EXPECT_INTEQ(tokens[13].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[13].id_length, 0);
	EXPECT_UINTEQ(tokens[13].value_length, 0);
	EXPECT_INTEQ(tokens[14].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[14].id_length, 0);
	EXPECT_UINTEQ(tokens[14].value_length, 5);
	EXPECT_INTEQ(tokens[15].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[15].id_length, 0);
	EXPECT_UINTEQ(tokens[15].value_length, 0);
	EXPECT_INTEQ(tokens[16].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[16].id_length, 5);
	EXPECT_UINTEQ(tokens[16].value_length, 4);
	EXPECT_INTEQ(tokens[17].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[17].id_length, 0);
	EXPECT_UINTEQ(tokens[17].value_length, 0);
	EXPECT_INTEQ(tokens[18].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[18].id_length, 0);
	EXPECT_UINTEQ(tokens[18].value_length, 12);

	return 0;
}

DECLARE_TEST(json, random) {
	char buffer[256];
	int i, j, steps;

	for (i = 0, steps = 1024 * 1024; i < steps; ++i) {
		for (j = 0; j < sizeof(buffer); ++j)
			buffer[j] = (char)random32_range(32, 127);
		json_parse(buffer, sizeof(buffer), nullptr, 0);
		sjson_parse(buffer, sizeof(buffer), nullptr, 0);
	}

	return 0;
}

static void
test_json_declare(void) {
	ADD_TEST(json, reference);
	ADD_TEST(json, simplified);
	ADD_TEST(json, random);
}

static test_suite_t test_json_suite = {
	test_json_application,
	test_json_memory_system,
	test_json_config,
	test_json_declare,
	test_json_initialize,
	test_json_finalize
};

#if BUILD_MONOLITHIC

int
test_json_run(void);

int
test_json_run(void) {
	test_suite = test_json_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_json_suite;
}

#endif
