/* main.c  -  Foundation JSON test  -  Public Domain  -  2013 Mattias Jansson
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
test_json_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation JSON tests"));
	app.short_name = string_const(STRING_CONST("test_json"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
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

DECLARE_TEST(json, reference) {
	json_token_t tokens[128];
	size_t capacity = sizeof(tokens) / sizeof(tokens[0]);

	string_const_t compound =
	    string_const(STRING_CONST(""
	                              "\t{\"foo\" :{\"subobj\": false ,\
		\"val\" :1.2345e45 \
	} ,\"arr\" :[ \
		\"string\",\
		-.34523e-78,[\
			true, \
			\"subarr [] {} =:\", { \"key\": []}, [] \
		],[false],\
		{ \t\
			\"final\" : null \
		}\
		,{ }  \
		, 1234.43E+123 \
	]}"));

	memset(tokens, 0, sizeof(tokens));

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
	EXPECT_UINTEQ(tokens[0].value, 1);  // String starts with a tab character
	EXPECT_UINTEQ(tokens[0].value_length, compound.length - 1);
	EXPECT_INTEQ(tokens[1].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[1].id_length, 3);
	EXPECT_UINTEQ(tokens[1].value, 9);
	EXPECT_UINTEQ(tokens[1].value_length, 39);
	EXPECT_INTEQ(tokens[2].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[2].id_length, 6);
	EXPECT_UINTEQ(tokens[2].value_length, 5);
	EXPECT_INTEQ(tokens[3].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[3].id_length, 3);
	EXPECT_UINTEQ(tokens[3].value_length, 9);
	EXPECT_INTEQ(tokens[4].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[4].id_length, 3);
	EXPECT_UINTEQ(tokens[4].value_length, 7);
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
	EXPECT_UINTEQ(tokens[0].value, 1);
	EXPECT_UINTEQ(tokens[0].value_length, compound.length - 1);
	EXPECT_INTEQ(tokens[1].type, JSON_OBJECT);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[1].id, tokens[1].id_length),
	                     string_const(STRING_CONST("foo")));
	EXPECT_UINTEQ(tokens[1].value, 9);
	EXPECT_UINTEQ(tokens[1].value_length, 39);
	EXPECT_INTEQ(tokens[2].type, JSON_PRIMITIVE);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[2].id, tokens[2].id_length),
	                     string_const(STRING_CONST("subobj")));
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[2].value, tokens[2].value_length),
	                     string_const(STRING_CONST("false")));
	EXPECT_INTEQ(tokens[3].type, JSON_PRIMITIVE);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[3].id, tokens[3].id_length),
	                     string_const(STRING_CONST("val")));
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[3].value, tokens[3].value_length),
	                     string_const(STRING_CONST("1.2345e45")));
	EXPECT_INTEQ(tokens[4].type, JSON_ARRAY);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[4].id, tokens[4].id_length),
	                     string_const(STRING_CONST("arr")));
	EXPECT_UINTEQ(tokens[4].value_length, 7);
	EXPECT_INTEQ(tokens[5].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[5].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[5].value, tokens[5].value_length),
	                     string_const(STRING_CONST("string")));
	EXPECT_INTEQ(tokens[6].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[6].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[6].value, tokens[6].value_length),
	                     string_const(STRING_CONST("-.34523e-78")));
	EXPECT_INTEQ(tokens[7].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[7].id_length, 0);
	EXPECT_UINTEQ(tokens[7].value_length, 4);
	EXPECT_INTEQ(tokens[8].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[8].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[8].value, tokens[8].value_length),
	                     string_const(STRING_CONST("true")));
	EXPECT_INTEQ(tokens[9].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[9].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[9].value, tokens[9].value_length),
	                     string_const(STRING_CONST("subarr [] {} =:")));
	EXPECT_INTEQ(tokens[10].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[10].id_length, 0);
	EXPECT_UINTEQ(tokens[10].value, 116);
	EXPECT_UINTEQ(tokens[10].value_length, 12);
	EXPECT_INTEQ(tokens[11].type, JSON_ARRAY);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[11].id, tokens[11].id_length),
	                     string_const(STRING_CONST("key")));
	EXPECT_UINTEQ(tokens[11].value_length, 0);
	EXPECT_INTEQ(tokens[12].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[12].id_length, 0);
	EXPECT_UINTEQ(tokens[12].value_length, 0);
	EXPECT_INTEQ(tokens[13].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[13].id_length, 0);
	EXPECT_UINTEQ(tokens[13].value_length, 1);
	EXPECT_INTEQ(tokens[14].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[14].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[14].value, tokens[14].value_length),
	                     string_const(STRING_CONST("false")));
	EXPECT_INTEQ(tokens[15].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[15].id_length, 0);
	EXPECT_UINTEQ(tokens[15].value, 147);
	EXPECT_UINTEQ(tokens[15].value_length, 24);
	EXPECT_INTEQ(tokens[16].type, JSON_PRIMITIVE);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[16].id, tokens[16].id_length),
	                     string_const(STRING_CONST("final")));
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[16].value, tokens[16].value_length),
	                     string_const(STRING_CONST("null")));
	EXPECT_INTEQ(tokens[17].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[17].id_length, 0);
	EXPECT_UINTEQ(tokens[17].value, 174);
	EXPECT_UINTEQ(tokens[17].value_length, 3);
	EXPECT_INTEQ(tokens[18].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[18].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[18].value, tokens[18].value_length),
	                     string_const(STRING_CONST("1234.43E+123")));

	return 0;
}

DECLARE_TEST(json, simplified) {
	json_token_t tokens[128];
	size_t capacity = sizeof(tokens) / sizeof(tokens[0]);

	string_const_t simplified =
	    string_const(STRING_CONST("\
	foo ={subobj= false \
		val =1.2345e45 \
	} arr =[\
		string\
		-.34523e-78 [\
			true\
			\"subarr [] {} =:\" { key: []} []\
		] [false] \
		{ \t\
			final = null\
		}\
		{ }  \
		1234.43E+123 \
	]\
	"));

	string_const_t compound =
	    string_const(STRING_CONST("\
	{\"foo\" :{\"subobj\": false ,\
		\"val\" :1.2345e45 \
	} ,\"arr\" :[ \
		\"string\",\
		-.34523e-78,[\
			true, \
			\"subarr [] {} =:\", { \"key\": []}, [] \
		],[false],\
		{ \t\
			\"final\" : null \
		}\
		,{ } , \
		 1234.43E+123 \
	]\
	}"));

	memset(tokens, 0, sizeof(tokens));

	EXPECT_SIZEEQ(sjson_parse(STRING_CONST("{\"test\" : true}"), tokens, capacity), 2);
	EXPECT_SIZEEQ(sjson_parse(STRING_CONST("{\"test\" : true }"), tokens, 1), 2);
	EXPECT_SIZEEQ(sjson_parse(STRING_CONST("{\"test\" :true}"), nullptr, 0), 2);

	EXPECT_SIZEEQ(sjson_parse(STRING_ARGS(compound), nullptr, 0), 19);
	EXPECT_SIZEEQ(sjson_parse(STRING_ARGS(compound), tokens, 7), 19);
	EXPECT_INTEQ(tokens[0].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[0].id, 0);
	EXPECT_UINTEQ(tokens[0].id_length, 0);
	EXPECT_UINTEQ(tokens[0].value, 1);  // String starts with a tab character
	EXPECT_UINTEQ(tokens[0].value_length, compound.length - 1);
	EXPECT_INTEQ(tokens[1].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[1].id_length, 3);
	EXPECT_UINTEQ(tokens[1].value, 9);
	EXPECT_UINTEQ(tokens[1].value_length, 39);
	EXPECT_INTEQ(tokens[2].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[2].id_length, 6);
	EXPECT_UINTEQ(tokens[2].value_length, 5);
	EXPECT_INTEQ(tokens[3].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[3].id_length, 3);
	EXPECT_UINTEQ(tokens[3].value_length, 9);
	EXPECT_INTEQ(tokens[4].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[4].id_length, 3);
	EXPECT_UINTEQ(tokens[4].value_length, 7);
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
	EXPECT_UINTEQ(tokens[0].value, 1);
	EXPECT_UINTEQ(tokens[0].value_length, compound.length - 1);
	EXPECT_INTEQ(tokens[1].type, JSON_OBJECT);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[1].id, tokens[1].id_length),
	                     string_const(STRING_CONST("foo")));
	EXPECT_UINTEQ(tokens[1].value, 9);
	EXPECT_UINTEQ(tokens[1].value_length, 39);
	EXPECT_INTEQ(tokens[2].type, JSON_PRIMITIVE);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[2].id, tokens[2].id_length),
	                     string_const(STRING_CONST("subobj")));
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[2].value, tokens[2].value_length),
	                     string_const(STRING_CONST("false")));
	EXPECT_INTEQ(tokens[3].type, JSON_PRIMITIVE);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[3].id, tokens[3].id_length),
	                     string_const(STRING_CONST("val")));
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[3].value, tokens[3].value_length),
	                     string_const(STRING_CONST("1.2345e45")));
	EXPECT_INTEQ(tokens[4].type, JSON_ARRAY);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[4].id, tokens[4].id_length),
	                     string_const(STRING_CONST("arr")));
	EXPECT_UINTEQ(tokens[4].value_length, 7);
	EXPECT_INTEQ(tokens[5].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[5].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[5].value, tokens[5].value_length),
	                     string_const(STRING_CONST("string")));
	EXPECT_INTEQ(tokens[6].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[6].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[6].value, tokens[6].value_length),
	                     string_const(STRING_CONST("-.34523e-78")));
	EXPECT_INTEQ(tokens[7].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[7].id_length, 0);
	EXPECT_UINTEQ(tokens[7].value_length, 4);
	EXPECT_INTEQ(tokens[8].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[8].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[8].value, tokens[8].value_length),
	                     string_const(STRING_CONST("true")));
	EXPECT_INTEQ(tokens[9].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[9].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[9].value, tokens[9].value_length),
	                     string_const(STRING_CONST("subarr [] {} =:")));
	EXPECT_INTEQ(tokens[10].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[10].id_length, 0);
	EXPECT_UINTEQ(tokens[10].value, 116);
	EXPECT_UINTEQ(tokens[10].value_length, 12);
	EXPECT_INTEQ(tokens[11].type, JSON_ARRAY);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[11].id, tokens[11].id_length),
	                     string_const(STRING_CONST("key")));
	EXPECT_UINTEQ(tokens[11].value_length, 0);
	EXPECT_INTEQ(tokens[12].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[12].id_length, 0);
	EXPECT_UINTEQ(tokens[12].value_length, 0);
	EXPECT_INTEQ(tokens[13].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[13].id_length, 0);
	EXPECT_UINTEQ(tokens[13].value_length, 1);
	EXPECT_INTEQ(tokens[14].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[14].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[14].value, tokens[14].value_length),
	                     string_const(STRING_CONST("false")));
	EXPECT_INTEQ(tokens[15].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[15].id_length, 0);
	EXPECT_UINTEQ(tokens[15].value, 147);
	EXPECT_UINTEQ(tokens[15].value_length, 24);
	EXPECT_INTEQ(tokens[16].type, JSON_PRIMITIVE);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[16].id, tokens[16].id_length),
	                     string_const(STRING_CONST("final")));
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[16].value, tokens[16].value_length),
	                     string_const(STRING_CONST("null")));
	EXPECT_INTEQ(tokens[17].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[17].id_length, 0);
	EXPECT_UINTEQ(tokens[17].value, 174);
	EXPECT_UINTEQ(tokens[17].value_length, 3);
	EXPECT_INTEQ(tokens[18].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[18].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(compound.str + tokens[18].value, tokens[18].value_length),
	                     string_const(STRING_CONST("1234.43E+123")));

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
	EXPECT_UINTEQ(tokens[0].value, 1);  // Simplified first object also skip leading whitespace
	EXPECT_UINTEQ(tokens[0].value_length, simplified.length - 1);
	EXPECT_INTEQ(tokens[1].type, JSON_OBJECT);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[1].id, tokens[1].id_length),
	                     string_const(STRING_CONST("foo")));
	EXPECT_UINTEQ(tokens[1].value, 6);
	EXPECT_UINTEQ(tokens[1].value_length, 34);
	EXPECT_INTEQ(tokens[2].type, JSON_PRIMITIVE);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[2].id, tokens[2].id_length),
	                     string_const(STRING_CONST("subobj")));
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[2].value, tokens[2].value_length),
	                     string_const(STRING_CONST("false")));
	EXPECT_INTEQ(tokens[3].type, JSON_PRIMITIVE);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[3].id, tokens[3].id_length),
	                     string_const(STRING_CONST("val")));
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[3].value, tokens[3].value_length),
	                     string_const(STRING_CONST("1.2345e45")));
	EXPECT_INTEQ(tokens[4].type, JSON_ARRAY);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[4].id, tokens[4].id_length),
	                     string_const(STRING_CONST("arr")));
	EXPECT_UINTEQ(tokens[4].value_length, 7);
	EXPECT_INTEQ(tokens[5].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[5].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[5].value, tokens[5].value_length),
	                     string_const(STRING_CONST("string")));
	EXPECT_INTEQ(tokens[6].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[6].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[6].value, tokens[6].value_length),
	                     string_const(STRING_CONST("-.34523e-78")));
	EXPECT_SIZEEQ(sjson_parse(STRING_ARGS(simplified), tokens, capacity), 19);
	EXPECT_INTEQ(tokens[0].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[0].id, 0);
	EXPECT_UINTEQ(tokens[0].id_length, 0);
	EXPECT_UINTEQ(tokens[0].value, 1);
	EXPECT_UINTEQ(tokens[0].value_length, simplified.length - 1);
	EXPECT_INTEQ(tokens[1].type, JSON_OBJECT);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[1].id, tokens[1].id_length),
	                     string_const(STRING_CONST("foo")));
	EXPECT_UINTEQ(tokens[1].value, 6);
	EXPECT_UINTEQ(tokens[1].value_length, 34);
	EXPECT_INTEQ(tokens[2].type, JSON_PRIMITIVE);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[2].id, tokens[2].id_length),
	                     string_const(STRING_CONST("subobj")));
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[2].value, tokens[2].value_length),
	                     string_const(STRING_CONST("false")));
	EXPECT_INTEQ(tokens[3].type, JSON_PRIMITIVE);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[3].id, tokens[3].id_length),
	                     string_const(STRING_CONST("val")));
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[3].value, tokens[3].value_length),
	                     string_const(STRING_CONST("1.2345e45")));
	EXPECT_INTEQ(tokens[4].type, JSON_ARRAY);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[4].id, tokens[4].id_length),
	                     string_const(STRING_CONST("arr")));
	EXPECT_UINTEQ(tokens[4].value_length, 7);
	EXPECT_INTEQ(tokens[5].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[5].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[5].value, tokens[5].value_length),
	                     string_const(STRING_CONST("string")));
	EXPECT_INTEQ(tokens[6].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[6].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[6].value, tokens[6].value_length),
	                     string_const(STRING_CONST("-.34523e-78")));
	EXPECT_INTEQ(tokens[7].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[7].id_length, 0);
	EXPECT_UINTEQ(tokens[7].value_length, 4);
	EXPECT_INTEQ(tokens[8].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[8].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[8].value, tokens[8].value_length),
	                     string_const(STRING_CONST("true")));
	EXPECT_INTEQ(tokens[9].type, JSON_STRING);
	EXPECT_UINTEQ(tokens[9].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[9].value, tokens[9].value_length),
	                     string_const(STRING_CONST("subarr [] {} =:")));
	EXPECT_INTEQ(tokens[10].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[10].id_length, 0);
	EXPECT_UINTEQ(tokens[10].value, 98);
	EXPECT_UINTEQ(tokens[10].value_length, 10);
	EXPECT_INTEQ(tokens[11].type, JSON_ARRAY);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[11].id, tokens[11].id_length),
	                     string_const(STRING_CONST("key")));
	EXPECT_UINTEQ(tokens[11].value_length, 0);
	EXPECT_INTEQ(tokens[12].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[12].id_length, 0);
	EXPECT_UINTEQ(tokens[12].value_length, 0);
	EXPECT_INTEQ(tokens[13].type, JSON_ARRAY);
	EXPECT_UINTEQ(tokens[13].id_length, 0);
	EXPECT_UINTEQ(tokens[13].value_length, 1);
	EXPECT_INTEQ(tokens[14].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[14].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[14].value, tokens[14].value_length),
	                     string_const(STRING_CONST("false")));
	EXPECT_INTEQ(tokens[15].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[15].id_length, 0);
	EXPECT_UINTEQ(tokens[15].value, 125);
	EXPECT_UINTEQ(tokens[15].value_length, 21);
	EXPECT_INTEQ(tokens[16].type, JSON_PRIMITIVE);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[16].id, tokens[16].id_length),
	                     string_const(STRING_CONST("final")));
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[16].value, tokens[16].value_length),
	                     string_const(STRING_CONST("null")));
	EXPECT_INTEQ(tokens[17].type, JSON_OBJECT);
	EXPECT_UINTEQ(tokens[17].id_length, 0);
	EXPECT_UINTEQ(tokens[17].value, 148);
	EXPECT_UINTEQ(tokens[17].value_length, 3);
	EXPECT_INTEQ(tokens[18].type, JSON_PRIMITIVE);
	EXPECT_UINTEQ(tokens[18].id_length, 0);
	EXPECT_CONSTSTRINGEQ(string_const(simplified.str + tokens[18].value, tokens[18].value_length),
	                     string_const(STRING_CONST("1234.43E+123")));

	return 0;
}

DECLARE_TEST(json, random) {
	char buffer[256];
	size_t i, j, steps;

	json_token_t tokens[256];

	for (i = 0, steps = 1024 * 1024; i < steps; ++i) {
		for (j = 0; j < sizeof(buffer); ++j)
			buffer[j] = (char)random32_range(32, 127);
		json_parse(buffer, sizeof(buffer), tokens, sizeof(tokens) / sizeof(tokens[0]));
		sjson_parse(buffer, sizeof(buffer), tokens, sizeof(tokens) / sizeof(tokens[0]));
	}

	return 0;
}

static bool test_parse_failed = true;

static bool test_parse_realloc_failed = true;

static void
test_json_handler(const char* path, size_t path_size, const char* buffer, size_t size, const json_token_t* tokens,
                  size_t numtokens) {
	FOUNDATION_UNUSED(path);
	FOUNDATION_UNUSED(path_size);
	FOUNDATION_UNUSED(size);
	if (numtokens == 4) {
		test_parse_failed = false;
		if (tokens[0].child != 1)
			test_parse_failed = true;
		if (!string_equal(buffer + tokens[1].id, tokens[1].id_length, STRING_CONST("test")))
			test_parse_failed = true;
		if (!string_equal(buffer + tokens[1].value, tokens[1].value_length, STRING_CONST("foo")))
			test_parse_failed = true;
		if (tokens[1].sibling != 2)
			test_parse_failed = true;
		if (!string_equal(buffer + tokens[2].id, tokens[2].id_length, STRING_CONST("bar")))
			test_parse_failed = true;
		if (tokens[2].type != JSON_OBJECT)
			test_parse_failed = true;
		if (tokens[2].child != 3)
			test_parse_failed = true;
		if (!string_equal(buffer + tokens[3].id, tokens[3].id_length, STRING_CONST("val")))
			test_parse_failed = true;
		if (!string_equal(buffer + tokens[3].value, tokens[3].value_length, STRING_CONST("\\u0000\\u0001\\u0002\\t")))
			test_parse_failed = true;
	} else if (numtokens == 145) {
		size_t itok;
		test_parse_realloc_failed = false;
		for (itok = 1; itok < 145; ++itok) {
			if (!string_equal(buffer + tokens[itok].id, tokens[itok].id_length, buffer + tokens[itok].value,
			                  tokens[itok].value_length))
				test_parse_realloc_failed = true;
		}
	}
}

DECLARE_TEST(json, util) {
	char escaped[512];
	char unescaped[512];
	string_t str;

	str = json_escape(
	    escaped, sizeof(escaped),
	    STRING_CONST(
	        "Test escape \"\\\b\f\r\n\t\x0\x1\x2\x3\x4\x5\x6\x7\x8\x9\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x20"));
	EXPECT_CONSTSTRINGEQ(
	    string_to_const(str),
	    string_const(STRING_CONST("Test escape "
	                              "\\\"\\\\\\b\\f\\r\\n\\t\\u0000\\u0001\\u0002\\u0003\\u0004\\u0005\\u0006\\u0007\\b\\"
	                              "t\\u0010\\u0011\\u0012\\u0013\\u0014\\u0015\\u0016\\u0017\\u0018\\u0019 ")));
	str = json_unescape(unescaped, sizeof(unescaped), STRING_ARGS(str));
	EXPECT_CONSTSTRINGEQ(
	    string_to_const(str),
	    string_const(STRING_CONST(
	        "Test escape \"\\\b\f\r\n\t\x0\x1\x2\x3\x4\x5\x6\x7\x8\x9\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x20")));

	stream_t* teststream;
	string_t path;
	string_const_t directory;

	path = path_make_temporary(escaped, sizeof(escaped));
	path = string_append(STRING_ARGS(path), sizeof(escaped), STRING_CONST(".sjson"));
	directory = path_directory_name(STRING_ARGS(path));
	fs_make_directory(STRING_ARGS(directory));

	teststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_OUT | STREAM_CREATE | STREAM_TRUNCATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "test stream '%.*s' not created", STRING_FORMAT(path));

	stream_write_string(teststream, STRING_CONST("test = foo\nbar = {\nval = \"\\u0000\\u0001\\u0002\\t\" }"));

	stream_deallocate(teststream);

	sjson_parse_path(STRING_ARGS(path), test_json_handler);
	EXPECT_FALSE(test_parse_failed);

	path = path_make_temporary(escaped, sizeof(escaped));
	path = string_append(STRING_ARGS(path), sizeof(escaped), STRING_CONST(".sjson"));

	teststream = stream_open(STRING_ARGS(path), STREAM_IN | STREAM_OUT | STREAM_CREATE | STREAM_TRUNCATE);
	EXPECT_NE_MSGFORMAT(teststream, 0, "test stream '%.*s' not created", STRING_FORMAT(path));

	stream_write_string(teststream, STRING_CONST("0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"
	                                             "0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"
	                                             "0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"
	                                             "0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"
	                                             "0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"
	                                             "0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"
	                                             "0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"
	                                             "0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"
	                                             "0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"
	                                             "0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"
	                                             "0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"
	                                             "0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"
	                                             "0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"
	                                             "0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"
	                                             "0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"
	                                             "0 = 0\n1 = 1\n2 = 2\n3 = 3\n4 = 4\n5 = 5\n6 = 6\n7 = 7\n8 = 8\n"));

	stream_deallocate(teststream);

	test_parse_failed = true;
	test_parse_realloc_failed = true;

	sjson_parse_path(STRING_ARGS(directory), test_json_handler);
	EXPECT_FALSE(test_parse_failed);
	EXPECT_FALSE(test_parse_realloc_failed);

	return 0;
}

static void
test_json_declare(void) {
	ADD_TEST(json, reference);
	ADD_TEST(json, simplified);
	ADD_TEST(json, random);
	ADD_TEST(json, util);
}

static test_suite_t test_json_suite = {test_json_application,
                                       test_json_memory_system,
                                       test_json_config,
                                       test_json_declare,
                                       test_json_initialize,
                                       test_json_finalize,
                                       0};

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
