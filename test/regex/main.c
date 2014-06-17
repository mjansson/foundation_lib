/* main.c  -  Foundation regex test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 * 
 * This library provides a cross-platform foundation library in C11 providing basic support data types and
 * functions to write applications and games in a platform-independent fashion. The latest source code is
 * always available at
 * 
 * https://github.com/rampantpixels/foundation_lib
 * 
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#include <foundation/foundation.h>
#include <test/test.h>


static application_t test_regex_application( void )
{
	application_t app = {0};
	app.name = "Foundation regex tests";
	app.short_name = "test_regex";
	app.config_dir = "test_regex";
	app.flags = APPLICATION_UTILITY;
	return app;
}


static memory_system_t test_regex_memory_system( void )
{
	return memory_system_malloc();
}


static int test_regex_initialize( void )
{
	return 0;
}


static void test_regex_shutdown( void )
{
}


DECLARE_TEST( regex, exact )
{
	regex_t* regex = regex_compile( "^(TEST REGEX)$" );
	EXPECT_NE( regex, 0 );
	
	EXPECT_TRUE( regex_match( regex, "TEST REGEX", 0, 0, 0 ) );
	EXPECT_FALSE( regex_match( regex, " TEST REGEX", 0, 0, 0 ) );
	EXPECT_FALSE( regex_match( regex, "TEST REGEX ", 0, 0, 0 ) );
	EXPECT_FALSE( regex_match( regex, "TEST_REGEX", 0, 0, 0 ) );
	
	regex_free( regex );

	regex = regex_compile( "(TEST REGEX)" );
	EXPECT_NE( regex, 0 );
	
	EXPECT_TRUE( regex_match( regex, "TEST REGEX", 0, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, " TEST REGEX", 0, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, "TEST REGEX ", 0, 0, 0 ) );
	EXPECT_FALSE( regex_match( regex, "TEST_REGEX", 0, 0, 0 ) );
	
	regex_free( regex );
	
	return 0;
}


DECLARE_TEST( regex, any )
{
	regex_t* regex = regex_compile( "^(.TEST.REGEX).$" );
	EXPECT_NE( regex, 0 );
	
	EXPECT_FALSE( regex_match( regex, "TEST REGEX", 0, 0, 0 ) );
	EXPECT_FALSE( regex_match( regex, " TEST REGEX", 0, 0, 0 ) );
	EXPECT_FALSE( regex_match( regex, "TEST REGEX ", 0, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, "TTEST_REGEX ", 0, 0, 0 ) );
	
	regex_free( regex );
	
	regex = regex_compile( "(.TEST.REGEX)." );
	EXPECT_NE( regex, 0 );
	
	EXPECT_FALSE( regex_match( regex, "TEST REGEX", 0, 0, 0 ) );
	EXPECT_FALSE( regex_match( regex, " TEST REGEX", 0, 0, 0 ) );
	EXPECT_FALSE( regex_match( regex, "TEST REGEX ", 0, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, "TTEST_REGEX ", 0, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, "RANDOM CRAP TEST_REGEX RANDOM CRAP", 0, 0, 0 ) );
	
	regex_free( regex );
	
	return 0;
}


DECLARE_TEST( regex, any_block )
{
	regex_t* regex = regex_compile( "^([ \\n\\r\\0\\S\\s\\d\\\\TESTREGEX])$" );
	EXPECT_NE( regex, 0 );
	
	EXPECT_TRUE( regex_match( regex, "T", 0, 0, 0 ) );
	EXPECT_FALSE( regex_match( regex, " TEST REGEX\t 0123456789 \n\r TEST!", 0, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, "\0", 1, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, " ", 1, 0, 0 ) );
	EXPECT_FALSE( regex_match( regex, "\0 ", 2, 0, 0 ) );
	
	regex_free( regex );

	regex = regex_compile( "^([ \\n\\r\\0\\S\\s\\d\\\\T])" );
	EXPECT_NE( regex, 0 );
	
	EXPECT_TRUE( regex_match( regex, "T", 0, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, " TEST REGEX\t 0123456789 \n\r TEST!", 0, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, "a", 1, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, " ", 1, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, "\0 ", 2, 0, 0 ) );
	
	regex_free( regex );
	
	return 0;
}


DECLARE_TEST( regex, quantifier )
{
	regex_capture_t captures[16];
	regex_t* regex = regex_compile( "^(.*)$" );
	EXPECT_NE( regex, 0 );
	
	EXPECT_TRUE( regex_match( regex, "any string will match this regex", 0, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, "\0", 1, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, " ", 0, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, "", 0, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, "any string will match this regex", 0, captures, 1 ) );
	EXPECT_STREQ_SUBSTR( captures[0].substring, "any string will match this regex", string_length( "any string will match this regex" ) );
	
	regex_free( regex );

	regex = regex_compile( "^(.+)$" );
	EXPECT_NE( regex, 0 );

	EXPECT_TRUE( regex_match( regex, "any string will match this regex", 0, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, "\0", 1, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, " ", 0, 0, 0 ) );
	EXPECT_FALSE( regex_match( regex, "", 0, 0, 0 ) );
	EXPECT_TRUE( regex_match( regex, "any string will match this regex", 0, captures, 1 ) );
	EXPECT_STREQ_SUBSTR( captures[0].substring, "any string will match this regex", string_length( "any string will match this regex" ) );
	
	regex_free( regex );
	
	return 0;
}


static void test_regex_declare( void )
{
	ADD_TEST( regex, exact );
	ADD_TEST( regex, any );
	ADD_TEST( regex, any_block );
	ADD_TEST( regex, quantifier );
}


test_suite_t test_regex_suite = {
	test_regex_application,
	test_regex_memory_system,
	test_regex_declare,
	test_regex_initialize,
	test_regex_shutdown
};


#if FOUNDATION_PLATFORM_ANDROID || FOUNDATION_PLATFORM_IOS

int test_regex_run( void );
int test_regex_run( void )
{
	test_suite = test_regex_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void )
{
	return test_regex_suite;
}

#endif
