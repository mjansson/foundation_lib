/* main.c  -  Foundation test suite  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <foundation.h>


int main_initialize( void )
{
	application_t application;
	application.name = "Foundation Test Suite";
	application.short_name = "test_app";
	application.config_dir = "test_app";

	return foundation_initialize( memory_system_malloc(), application );
}


int main_run( void* main_arg )
{
	//Enumerate all test* applications in executable directory and run with process_* functionality
	return 0;
}


void main_shutdown( void )
{
	foundation_shutdown();
}

