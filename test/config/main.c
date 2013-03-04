/* main.c  -  Foundation config test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


application_t test_application( void )
{
	application_t app = {0};
	app.name = "Foundation config tests";
	app.short_name = "test_config";
	app.config_dir = "test_config";
	return app;
}


DECLARE_TEST( config, builtin )
{
	return 0;
}


void test_declare( void )
{
	ADD_TEST( config, builtin );
}
