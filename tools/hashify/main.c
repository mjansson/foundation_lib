/* main.c  -  Foundation library tools  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


int main()
{
	application_t application;
	application.name = "hashify";
	application.short_name = "hashify";
	application.config_dir = "hashify";

	if( foundation_initialize( memory_system_malloc(), application ) < 0 )
		return -1;


	foundation_shutdown();

	return 0;
}
