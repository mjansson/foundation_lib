/* main.c  -  Foundation regex test  -  Public Domain  -  2013 Mattias Jansson
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
test_regex_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation regex tests"));
	app.short_name = string_const(STRING_CONST("test_regex"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_regex_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_regex_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_regex_initialize(void) {
	return 0;
}

static void
test_regex_finalize(void) {
}

DECLARE_TEST(regex, exact) {
	regex_t* regex = regex_compile(STRING_CONST("^(TEST\\20REGEX)$"));
	EXPECT_NE(regex, 0);

	EXPECT_TRUE(regex_match(regex, STRING_CONST("TEST REGEX"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST(" TEST REGEX"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("TEST REGEX "), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("TEST_REGEX"), 0, 0));

	regex_deallocate(regex);

	regex = regex_compile(STRING_CONST("(TEST REGEX)"));
	EXPECT_NE(regex, 0);

	EXPECT_TRUE(regex_match(regex, STRING_CONST("TEST REGEX"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST(" TEST REGEX"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("TEST REGEX "), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("TEST_REGEX"), 0, 0));
	EXPECT_TRUE(regex_match(0, "zero length string", 0, 0, 0));

	log_enable_stdout(false);
	regex->code[0] = 128;
	EXPECT_FALSE(regex_match(regex, STRING_CONST("TEST_REGEX"), 0, 0));
	log_enable_stdout(true);

	regex_deallocate(regex);

	regex = regex_compile(STRING_CONST("^[\\s]*^TEST$"));
	EXPECT_NE(regex, 0);
	EXPECT_TRUE(regex_match(regex, STRING_CONST("TEST"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("   TEST"), 0, 0));

	regex_deallocate(regex);

	EXPECT_TRUE(regex_match(0, STRING_CONST("TEST REGEX"), 0, 0));

	return 0;
}

DECLARE_TEST(regex, any) {
	regex_t* regex = regex_compile(STRING_CONST("^(.TEST.REGEX).$"));
	EXPECT_NE(regex, 0);

	EXPECT_FALSE(regex_match(regex, STRING_CONST("TEST REGEX"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST(" TEST REGEX"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("TEST REGEX "), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("TTEST_REGEX "), 0, 0));

	regex_deallocate(regex);

	regex = regex_compile(STRING_CONST("(.TEST.REGEX)."));
	EXPECT_NE(regex, 0);

	EXPECT_FALSE(regex_match(regex, STRING_CONST("TEST REGEX"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST(" TEST REGEX"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("TEST REGEX "), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("TTEST_REGEX "), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("RANDOM CRAP TEST_REGEX RANDOM CRAP"), 0, 0));

	regex_deallocate(regex);

	return 0;
}

DECLARE_TEST(regex, any_block) {
	regex_t* regex = regex_compile(STRING_CONST("^([ \\n\\r\\0\\S\\s\\d\\\\TESTREGEX])$"));
	EXPECT_NE(regex, 0);

	EXPECT_TRUE(regex_match(regex, STRING_CONST("T"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST(" TEST \\REGEX\t 0123456789 \n\r TEST!"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("\0"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST(" "), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("alphanum3r1CS"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("a"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("\0 "), 0, 0));

	regex_deallocate(regex);

	regex = regex_compile(STRING_CONST("^([ \\n\\r\\0\\t\\D\\\\T])"));
	EXPECT_NE(regex, 0);

	EXPECT_TRUE(regex_match(regex, STRING_CONST("T"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST(" TEST REGEX\t 0123456789 \n\r \\TEST!"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("a"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("0"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("a0"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("0a"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST(" "), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("\0 "), 0, 0));

	regex_deallocate(regex);

	return 0;
}

DECLARE_TEST(regex, quantifier) {
	string_const_t captures[16];
	regex_t* regex = regex_compile(STRING_CONST("^(.*)$"));
	EXPECT_NE(regex, 0);

	EXPECT_TRUE(regex_match(regex, STRING_CONST("any string will match this regex"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("\0"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST(" "), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST(""), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("any string will match this regex"), captures, 1));
	EXPECT_CONSTSTRINGEQ(captures[0], string_const(STRING_CONST("any string will match this regex")));

	regex_deallocate(regex);

	regex = regex_compile(STRING_CONST("^(.+)$"));
	EXPECT_NE(regex, 0);

	EXPECT_TRUE(regex_match(regex, STRING_CONST("any string will match this regex"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("\0"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST(" "), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST(""), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("any string will match this regex"), captures, 1));
	EXPECT_CONSTSTRINGEQ(captures[0], string_const(STRING_CONST("any string will match this regex")));

	regex_deallocate(regex);

	regex = regex_compile(STRING_CONST("^(.*?)$"));
	EXPECT_NE(regex, 0);

	EXPECT_TRUE(regex_match(regex, STRING_CONST("any string will match this regex"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("\0"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST(" "), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST(""), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("any string will match this regex"), captures, 1));
	EXPECT_CONSTSTRINGEQ(captures[0], string_const(STRING_CONST("any string will match this regex")));

	regex_deallocate(regex);

	regex = regex_compile(STRING_CONST("^(.+?)$"));
	EXPECT_NE(regex, 0);

	EXPECT_TRUE(regex_match(regex, STRING_CONST("any string will match this regex"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("\0"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST(" "), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST(""), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("any string will match this regex"), captures, 1));
	EXPECT_CONSTSTRINGEQ(captures[0], string_const(STRING_CONST("any string will match this regex")));

	regex_deallocate(regex);

	// The \\d\\64 construct is to be able to have meta \\d followed by literal 'd' without it
	// being parsed as \\dd hex value
	regex = regex_compile(STRING_CONST("^a.b+?b\\d\\64+?e*$"));
	EXPECT_NE(regex, 0);

	EXPECT_TRUE(regex_match(regex, STRING_CONST("aabbbb0deeeeeee"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("aabbbbeeeeeee"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("abbb1d"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("abb2de"), 0,
	                         0));  // group before decimal must be at least 4 chars
	EXPECT_FALSE(regex_match(regex, STRING_CONST("aabb2de0"), 0, 0));

	regex_deallocate(regex);

	regex = regex_compile(STRING_CONST("^[abc\\64]+en?d*[fo]*$"));

	EXPECT_TRUE(regex_match(regex, STRING_CONST("abcdaaabbbcdddcdabcdbabendddd"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("abcdaaabbbcdddcdabcdbabeddddfoooo"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("abcdaaabbbcdddcdabcdbabeddddfooood"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("aen"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("den"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("aabbbbecdend"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("end"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("aabbbbcddfood"), 0, 0));

	regex_deallocate(regex);

	regex = regex_compile(STRING_CONST("^[abcd]+"));

	EXPECT_TRUE(regex_match(regex, STRING_CONST("abcdaaabbbcdddcdabcdbabendddd"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("as long as it starts ok"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("will not match abcd"), 0, 0));

	regex_deallocate(regex);

	regex = regex_compile(STRING_CONST("^[abcd]+?"));

	EXPECT_TRUE(regex_match(regex, STRING_CONST("abcdaaabbbcdddcdabcdbabendddd"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("as long as it starts ok"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("will not match abcd"), 0, 0));

	regex_deallocate(regex);

	return 0;
}

DECLARE_TEST(regex, branch) {
	string_const_t captures[16];
	regex_t* regex = regex_compile(STRING_CONST("^(\\s+|\\S+)$"));
	EXPECT_NE(regex, 0);

	EXPECT_TRUE(regex_match(regex, STRING_CONST("anynonwhitespacestringwillmatchthisregex"), 0, 0));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("   \t\t\n\r  \t\v\n  "), 0, 0));

	memset(captures, 0, sizeof(string_const_t) * 16);
	EXPECT_FALSE(regex_match(regex, STRING_CONST("no mixed string will match this regex"), captures, 16));

	regex_deallocate(regex);

	// Craft regex that will require reallocations
	regex = regex_compile(STRING_CONST("^(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)(a|b)$"));
	EXPECT_NE(regex, 0);
	regex_deallocate(regex);

	regex =
	    regex_compile(STRING_CONST("^(abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz)"
	                               "(abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz)"
	                               "(abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz)"
	                               "(abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz)"
	                               "(abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz)"
	                               "(abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz)"
	                               "(abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz)"
	                               "(abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz)"
	                               "(abcdefghijklmnopqrstuvwxyz|abcdefghijklmnopqrstuvwxyz)"
	                               "$"));
	EXPECT_NE(regex, 0);
	regex_deallocate(regex);

	return 0;
}

DECLARE_TEST(regex, noanchor) {
	string_const_t captures[16];
	regex_t* regex = regex_compile(STRING_CONST("\\6D\\61tchthis(\\s+|\\S+)!"));
	EXPECT_NE(regex, 0);

	EXPECT_TRUE(regex_match(regex, STRING_CONST("anynonwhitespacestringwillmatchthisregex!"), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("   \t\t\n\r  \t\v\n  "), 0, 0));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("no mixed strings at end will matchthis reg ex !"), captures, 16));
	EXPECT_TRUE(regex_match(regex, STRING_CONST("but nonmixed at end will matchthisregex!"), captures, 16));

	regex_deallocate(regex);

	return 0;
}

DECLARE_TEST(regex, captures) {
	string_const_t captures[16];
	regex_t* regex = regex_compile(STRING_CONST("matchthis(\\s+|\\S+)!endof\\6cine([abcd\\\\]*)"));
	EXPECT_NE(regex, 0);

	EXPECT_FALSE(
	    regex_match(regex, STRING_CONST("no mixed strings at end will matchthis reg ex !endofline"), captures, 16));
	EXPECT_TRUE(
	    regex_match(regex, STRING_CONST("non mixed strings at end will matchthisregex!endofline"), captures, 16));
	EXPECT_TRUE(
	    regex_match(regex, STRING_CONST("non mixed strings at end will matchthis  \t\n\r  !endofline"), captures, 16));

	memset(captures, 0, sizeof(string_const_t) * 16);
	EXPECT_TRUE(regex_match(regex, STRING_CONST("but nonmixed at end will matchthisregex!endofline"), captures, 16));
	EXPECT_CONSTSTRINGEQ(captures[0], string_const(STRING_CONST("regex")));
	EXPECT_INTEQ(captures[0].length, 5);
	EXPECT_EQ(captures[1].length, 0);
	EXPECT_EQ(captures[2].str, 0);
	EXPECT_EQ(captures[2].length, 0);

	memset(captures, 0, sizeof(string_const_t) * 16);
	EXPECT_TRUE(
	    regex_match(regex, STRING_CONST("but nonmixed at end will matchthis  \t\n\r  !endofline"), captures, 16));
	EXPECT_CONSTSTRINGEQ(captures[0], string_const(STRING_CONST("  \t\n\r  ")));
	EXPECT_INTEQ(captures[0].length, 7);
	EXPECT_EQ(captures[1].length, 0);
	EXPECT_EQ(captures[2].str, 0);
	EXPECT_EQ(captures[2].length, 0);

	memset(captures, 0, sizeof(string_const_t) * 16);
	EXPECT_TRUE(
	    regex_match(regex, STRING_CONST("but nonmixed at end will matchthisstring!endofline\\aabbcc\\"), captures, 16));
	EXPECT_CONSTSTRINGEQ(captures[0], string_const(STRING_CONST("string")));
	EXPECT_INTEQ(captures[0].length, 6);
	EXPECT_CONSTSTRINGEQ(captures[1], string_const(STRING_CONST("\\aabbcc\\")));
	EXPECT_EQ(captures[1].length, 8);
	EXPECT_EQ(captures[2].str, 0);
	EXPECT_EQ(captures[2].length, 0);

	regex_deallocate(regex);

	regex = regex_compile(STRING_CONST("([^\\s]*)$"));

	EXPECT_TRUE(regex_match(regex, STRING_CONST("something at endofline"), captures, 16));
	EXPECT_CONSTSTRINGEQ(captures[0], string_const(STRING_CONST("endofline")));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("whitespace at endofline \t"), captures, 16));

	regex_deallocate(regex);

	regex = regex_compile(STRING_CONST("^([^abc]*)$"));

	EXPECT_TRUE(regex_match(regex, STRING_CONST("qwerty"), captures, 16));
	EXPECT_CONSTSTRINGEQ(captures[0], string_const(STRING_CONST("qwerty")));
	EXPECT_FALSE(regex_match(regex, STRING_CONST("qwerbty"), captures, 16));

	regex_deallocate(regex);

	return 0;
}

FOUNDATION_ALIGNED_STRUCT(regexbuffer_t, 8) {
	char buffer[sizeof(regex_t) + 8];
};

DECLARE_TEST(regex, invalid) {
	regex_t* regex;
	regex_t predef;
	struct regexbuffer_t buffer;

	regex = regex_compile(STRING_CONST("++??.+*?"));
	EXPECT_EQ(regex, nullptr);

	regex = regex_compile(STRING_CONST("(())()("));
	EXPECT_EQ(regex, nullptr);

	regex = regex_compile(STRING_CONST("[\\s]["));
	EXPECT_EQ(regex, nullptr);

	memset(&predef, 0, sizeof(predef));
	EXPECT_FALSE(regex_parse(&predef, STRING_CONST("test")));

	regex = (regex_t*)&buffer;
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_TRUE(regex_parse(regex, STRING_CONST("te")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("tes")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("te^")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("te$")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("te(capture)")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("(longcapture)")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("(t)")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("t)")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("te[test]")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("[test]")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("te.")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("t*+")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("t+*")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("te*")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("te*?")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("te?")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("te\\64")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("te\\6jk")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("te\\s")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("te\\0")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("te|")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("te|st")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("^?$?")));
	memset(&buffer, 0, sizeof(buffer));
	regex->code_allocated = 4;
	EXPECT_FALSE(regex_parse(regex, STRING_CONST("|?")));

	// Too long op, parser will fail
	regex =
	    regex_compile(STRING_CONST("^aaaaaaaaaaaaaabbbbbbbbbbbbbbbbbcccccccccccccccccc"
	                               "ddddddddddddeeeeeeeeeeeeeefffffffffffffggggggggggggg"
	                               "hhhhhhhhhhhhiiiiiiiiiiiiiijjjjjjjjjjjjjkkkkkkkkkkkkk"
	                               "llllllllllllmmmmmmmmmmmmmmnnnnnnnnnnnnnooooooooooooo"
	                               "ppppppppppppqqqqqqqqqqqqqqrrrrrrrrrrrrrsssssssssssss"
	                               "ttttttttttttuuuuuuuuuuuuuuvvvvvvvvvvvvvwwwwwwwwwwwww"
	                               "xxxxxxxxxxxxyyyyyyyyyyyyyyzzzzzzzzzzzzz \\n\\r\\0"));
	EXPECT_EQ(regex, 0);

	// Too long group, parser will fail
	regex =
	    regex_compile(STRING_CONST("^([aaaaaaaaaaaaaabbbbbbbbbbbbbbbbbcccccccccccccccccc"
	                               "ddddddddddddeeeeeeeeeeeeeefffffffffffffggggggggggggg"
	                               "hhhhhhhhhhhhiiiiiiiiiiiiiijjjjjjjjjjjjjkkkkkkkkkkkkk"
	                               "llllllllllllmmmmmmmmmmmmmmnnnnnnnnnnnnnooooooooooooo"
	                               "ppppppppppppqqqqqqqqqqqqqqrrrrrrrrrrrrrsssssssssssss"
	                               "ttttttttttttuuuuuuuuuuuuuuvvvvvvvvvvvvvwwwwwwwwwwwww"
	                               "xxxxxxxxxxxxyyyyyyyyyyyyyyzzzzzzzzzzzzz \\n\\r\\0])"));
	EXPECT_EQ(regex, 0);

	return 0;
}

static void
test_regex_declare(void) {
	ADD_TEST(regex, exact);
	ADD_TEST(regex, any);
	ADD_TEST(regex, any_block);
	ADD_TEST(regex, quantifier);
	ADD_TEST(regex, branch);
	ADD_TEST(regex, noanchor);
	ADD_TEST(regex, captures);
	ADD_TEST(regex, invalid);
}

static test_suite_t test_regex_suite = {test_regex_application,
                                        test_regex_memory_system,
                                        test_regex_config,
                                        test_regex_declare,
                                        test_regex_initialize,
                                        test_regex_finalize,
                                        0};

#if BUILD_MONOLITHIC

int
test_regex_run(void);

int
test_regex_run(void) {
	test_suite = test_regex_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_regex_suite;
}

#endif
