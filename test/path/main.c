/* main.c  -  Foundation path test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


static application_t test_path_application( void )
{
	application_t app;
	memset( &app, 0, sizeof( app ) );
	app.name = string_const( STRING_CONST( "Foundation path tests" ) );
	app.short_name = string_const( STRING_CONST( "test_path" ) );
	app.config_dir = string_const( STRING_CONST( "test_path" ) );
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_path_memory_system( void )
{
	return memory_system_malloc();
}


static int test_path_initialize( void )
{
	return 0;
}


static void test_path_shutdown( void )
{
}


DECLARE_TEST( path, extract )
{
	const char path1[] = "file.ext";
	const char path2[] = "file";
	const char path3[] = "dir/";
	const char path4[] = "path/file.ext";
	const char path5[] = "path/file";
	const char path6[] = "path/dir/";
	const char path7[] = "more/some/path/file.ext";
	const char path8[] = "more/some/path/file";
	const char path9[] = "more/some/path/dir/";
	const char path10[] = "/abs/path/file.ext";
	const char path11[] = "/abs/path/file";
	const char path12[] = "/abs/path/dir/";
	const char path13[] = "C:/abs/path/file.ext";
	const char path14[] = "C:/abs/path/file";
	const char path15[] = "C:/abs/path/dir/";
	const char path16[] = "vfs://abs/path/file.ext";
	const char path17[] = "vfs://abs/path/file";
	const char path18[] = "vfs://abs/path/dir/";
	const char path19[] = "";
	const char path20[] = "/";
	const char path21[] = "asset://.path/file.ext";
	const char path22[] = "asset://.path/file";
	const char path23[] = "asset://.path/.dir/";
	string_const_t testpath;

	testpath = path_base_file_name( STRING_CONST( path1 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name( STRING_CONST( path2 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name( STRING_CONST( path3 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_base_file_name( STRING_CONST( path4 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name( STRING_CONST( path5 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name( STRING_CONST( path6 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_base_file_name( STRING_CONST( path7 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name( STRING_CONST( path8 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name( STRING_CONST( path9 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_base_file_name( STRING_CONST( path10 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name( STRING_CONST( path11 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name( STRING_CONST( path12 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_base_file_name( STRING_CONST( path13 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name( STRING_CONST( path14 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name( STRING_CONST( path15 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_base_file_name( STRING_CONST( path16 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name( STRING_CONST( path17 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name( STRING_CONST( path18 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_base_file_name( STRING_CONST( path19 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_base_file_name( STRING_CONST( path20 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_base_file_name( STRING_CONST( path21 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name( STRING_CONST( path22 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name( STRING_CONST( path23 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );

	testpath = path_base_file_name_with_directory( STRING_CONST( path1 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path2 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path3 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "dir" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path4 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "path/file" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path5 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "path/file" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path6 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "path/dir" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path7 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "more/some/path/file" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path8 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "more/some/path/file" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path9 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "more/some/path/dir" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path10 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "/abs/path/file" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path11 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "/abs/path/file" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path12 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "/abs/path/dir" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path13 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "C:/abs/path/file" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path14 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "C:/abs/path/file" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path15 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "C:/abs/path/dir" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path16 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "vfs://abs/path/file" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path17 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "vfs://abs/path/file" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path18 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "vfs://abs/path/dir" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path19 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path20 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "/" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path21 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "asset://.path/file" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path22 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "asset://.path/file" ) ) );
	testpath = path_base_file_name_with_directory( STRING_CONST( path23 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "asset://.path/.dir" ) ) );

	testpath = path_file_extension( STRING_CONST( path1 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "ext" ) ) );
	testpath = path_file_extension( STRING_CONST( path2 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_extension( STRING_CONST( path3 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_extension( STRING_CONST( path4 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "ext" ) ) );
	testpath = path_file_extension( STRING_CONST( path5 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_extension( STRING_CONST( path6 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_extension( STRING_CONST( path7 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "ext" ) ) );
	testpath = path_file_extension( STRING_CONST( path8 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_extension( STRING_CONST( path9 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_extension( STRING_CONST( path10 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "ext" ) ) );
	testpath = path_file_extension( STRING_CONST( path11 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_extension( STRING_CONST( path12 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_extension( STRING_CONST( path13 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "ext" ) ) );
	testpath = path_file_extension( STRING_CONST( path14 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_extension( STRING_CONST( path15 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_extension( STRING_CONST( path16 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "ext" ) ) );
	testpath = path_file_extension( STRING_CONST( path17 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_extension( STRING_CONST( path18 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_extension( STRING_CONST( path19 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_extension( STRING_CONST( path20 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_extension( STRING_CONST( path21 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "ext" ) ) );
	testpath = path_file_extension( STRING_CONST( path22 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_extension( STRING_CONST( path23 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );

	testpath = path_file_name( STRING_CONST( path1 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file.ext" ) ) );
	testpath = path_file_name( STRING_CONST( path2 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_file_name( STRING_CONST( path3 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_name( STRING_CONST( path4 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file.ext" ) ) );
	testpath = path_file_name( STRING_CONST( path5 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_file_name( STRING_CONST( path6 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_name( STRING_CONST( path7 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file.ext" ) ) );
	testpath = path_file_name( STRING_CONST( path8 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_file_name( STRING_CONST( path9 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_name( STRING_CONST( path10 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file.ext" ) ) );
	testpath = path_file_name( STRING_CONST( path11 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_file_name( STRING_CONST( path12 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_name( STRING_CONST( path13 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file.ext" ) ) );
	testpath = path_file_name( STRING_CONST( path14 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_file_name( STRING_CONST( path15 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_name( STRING_CONST( path16 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file.ext" ) ) );
	testpath = path_file_name( STRING_CONST( path17 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_file_name( STRING_CONST( path18 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_name( STRING_CONST( path19 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_name( STRING_CONST( path20 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_file_name( STRING_CONST( path21 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file.ext" ) ) );
	testpath = path_file_name( STRING_CONST( path22 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_file_name( STRING_CONST( path23 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );

	testpath = path_directory_name( STRING_CONST( path1 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_directory_name( STRING_CONST( path2 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_directory_name( STRING_CONST( path3 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "dir" ) ) );
	testpath = path_directory_name( STRING_CONST( path4 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "path" ) ) );
	testpath = path_directory_name( STRING_CONST( path5 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "path" ) ) );
	testpath = path_directory_name( STRING_CONST( path6 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "path/dir" ) ) );
	testpath = path_directory_name( STRING_CONST( path7 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "more/some/path" ) ) );
	testpath = path_directory_name( STRING_CONST( path8 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "more/some/path" ) ) );
	testpath = path_directory_name( STRING_CONST( path9 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "more/some/path/dir" ) ) );
	testpath = path_directory_name( STRING_CONST( path10 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "/abs/path" ) ) );
	testpath = path_directory_name( STRING_CONST( path11 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "/abs/path" ) ) );
	testpath = path_directory_name( STRING_CONST( path12 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "/abs/path/dir" ) ) );
	testpath = path_directory_name( STRING_CONST( path13 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "C:/abs/path" ) ) );
	testpath = path_directory_name( STRING_CONST( path14 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "C:/abs/path" ) ) );
	testpath = path_directory_name( STRING_CONST( path15 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "C:/abs/path/dir" ) ) );
	testpath = path_directory_name( STRING_CONST( path16 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "/abs/path" ) ) );
	testpath = path_directory_name( STRING_CONST( path17 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "/abs/path" ) ) );
	testpath = path_directory_name( STRING_CONST( path18 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "/abs/path/dir" ) ) );
	testpath = path_directory_name( STRING_CONST( path19 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_directory_name( STRING_CONST( path20 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "/" ) ) );
	testpath = path_directory_name( STRING_CONST( path21 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "/.path" ) ) );
	testpath = path_directory_name( STRING_CONST( path22 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "/.path" ) ) );
	testpath = path_directory_name( STRING_CONST( path23 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "/.path/.dir" ) ) );

	testpath = path_subdirectory_name( STRING_CONST( path1 ), STRING_CONST( "file" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path2 ), STRING_CONST( "" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path3 ), STRING_CONST( "" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "dir" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path4 ), STRING_CONST( "path" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file.ext" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path5 ), STRING_CONST( "file" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path6 ), STRING_CONST( "path/dir" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path7 ), STRING_CONST( "more" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "some/path/file.ext" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path8 ), STRING_CONST( "/more" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path9 ), STRING_CONST( "more/some/path" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "dir" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path10 ), STRING_CONST( "ab" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path11 ), STRING_CONST( "abs/path" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path12 ), STRING_CONST( "/abs" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "path/dir" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path13 ), STRING_CONST( "/ab" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path14 ), STRING_CONST( "C:/abs/" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "path/file" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path15 ), STRING_CONST( "C:" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "abs/path/dir" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path16 ), STRING_CONST( "abs/path" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path17 ), STRING_CONST( "/abs" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "path/file" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path18 ), STRING_CONST( "/abs/path/" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "dir" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path19 ), STRING_CONST( "/" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path20 ), STRING_CONST( "/" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path21 ), STRING_CONST( "/.path" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "file.ext" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path22 ), STRING_CONST( ".path" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_subdirectory_name( STRING_CONST( path23 ), STRING_CONST( "/" ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( ".path/.dir" ) ) );

	testpath = path_protocol( STRING_CONST( path1 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path2 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path3 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path4 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path5 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path6 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path7 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path8 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path9 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path10 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path11 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path12 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path13 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path14 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path15 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path16 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "vfs" ) ) );
	testpath = path_protocol( STRING_CONST( path17 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "vfs" ) ) );
	testpath = path_protocol( STRING_CONST( path18 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "vfs" ) ) );
	testpath = path_protocol( STRING_CONST( path19 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path20 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "" ) ) );
	testpath = path_protocol( STRING_CONST( path21 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "asset" ) ) );
	testpath = path_protocol( STRING_CONST( path22 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "asset" ) ) );
	testpath = path_protocol( STRING_CONST( path23 ) ); EXPECT_STRINGEQ( testpath, string_const( STRING_CONST( "asset" ) ) );

	return 0;
}


DECLARE_TEST( path, clean )
{
	char* path1 = string_clone( "" );
	char* path2 = string_clone( "/" );
	char* path3 = string_clone( "/." );
	char* path4 = string_clone( "./" );
	char* path5 = string_clone( "./." );
	char* path6 = string_clone( "././" );
	char* path7 = string_clone( "././//" );
	char* path8 = string_clone( "././//./////././////.//////.//." );
	char* path9 = string_clone( "http://././//./////././////.//////.//." );
	char* path10 = string_clone( "" );
	char* path11 = string_clone( "\\" );
	char* path12 = string_clone( "/\\." );
	char* path13 = string_clone( ".\\/" );
	char* path14 = string_clone( "./\\." );
	char* path15 = string_clone( ".\\.//\\" );
	char* path16 = string_clone( ".\\.\\\\\\" );
	char* path17 = string_clone( ".\\.\\\\\\.\\\\////\\///\\\\.\\.\\\\\\\\\\.\\\\\\\\\\\\.\\\\." );
	char* path18 = string_clone( "http://\\.\\.\\\\\\.\\\\\\\\//\\.\\.\\/\\/\\/\\//\\.\\\\\\\\\\\\.\\\\." );
	char* path19 = string_clone( "testing/path/ext" );
	char* path20 = string_clone( "testing/path/extend" );
	char* path21 = string_clone( "testing/path/extend/dyn" );
	char* path22 = string_clone( "testing/./\\\\/\\/./path/././//./extend/\\\\" );
	char* path23 = string_clone( "http://///\\.\\.\\\\\\.\\\\\\\\//\\.\\.\\/\\/\\/\\//\\.\\\\\\\\\\\\.\\\\.///some/file" );
	char* path24 = string_clone( "http://test/../../path" );
	char* path25 = string_clone( "http:///.//test/../../path" );
	char* path26 = string_clone( "http:///.//test/../../../../../../path" );

	path1 = path_clean( STRING_CONST( path1, true );
	path2 = path_clean( STRING_CONST( path2, true );
	path3 = path_clean( STRING_CONST( path3, true );
	path4 = path_clean( STRING_CONST( path4, true );
	path5 = path_clean( STRING_CONST( path5, true );
	path6 = path_clean( STRING_CONST( path6, true );
	path7 = path_clean( STRING_CONST( path7, true );
	path8 = path_clean( STRING_CONST( path8, true );
	path9 = path_clean( STRING_CONST( path9, true );
	path10 = path_clean( STRING_CONST( path10, true );
	path11 = path_clean( STRING_CONST( path11, true );
	path12 = path_clean( STRING_CONST( path12, true );
	path13 = path_clean( STRING_CONST( path13, true );
	path14 = path_clean( STRING_CONST( path14, true );
	path15 = path_clean( STRING_CONST( path15, true );
	path16 = path_clean( STRING_CONST( path16, true );
	path17 = path_clean( STRING_CONST( path17, true );
	path18 = path_clean( STRING_CONST( path18, true );
	path19 = path_clean( STRING_CONST( path19, true );
	path20 = path_clean( STRING_CONST( path20, true );
	path21 = path_clean( STRING_CONST( path21, true );
	path22 = path_clean( STRING_CONST( path22, true );
	path23 = path_clean( STRING_CONST( path23, true );
	path24 = path_clean( STRING_CONST( path24, true );
	path25 = path_clean( STRING_CONST( path25, true );
	path26 = path_clean( STRING_CONST( path26, true );

	EXPECT_STREQ( STRING_CONST( path1, "/" );
	EXPECT_STREQ( STRING_CONST( path2, "/" );
	EXPECT_STREQ( STRING_CONST( path3, "/" );
	EXPECT_STREQ( STRING_CONST( path4, "/" );
	EXPECT_STREQ( STRING_CONST( path5, "/" );
	EXPECT_STREQ( STRING_CONST( path6, "/" );
	EXPECT_STREQ( STRING_CONST( path7, "/" );
	EXPECT_STREQ( STRING_CONST( path8, "/" );
	EXPECT_STREQ( STRING_CONST( path9, "http://" );
	EXPECT_STREQ( STRING_CONST( path10, "/" );
	EXPECT_STREQ( STRING_CONST( path11, "/" );
	EXPECT_STREQ( STRING_CONST( path12, "/" );
	EXPECT_STREQ( STRING_CONST( path13, "/" );
	EXPECT_STREQ( STRING_CONST( path14, "/" );
	EXPECT_STREQ( STRING_CONST( path15, "/" );
	EXPECT_STREQ( STRING_CONST( path16, "/" );
	EXPECT_STREQ( STRING_CONST( path17, "/" );
	EXPECT_STREQ( STRING_CONST( path18, "http://" );
	EXPECT_STREQ( STRING_CONST( path19, "/testing/path/ext" );
	EXPECT_STREQ( STRING_CONST( path20, "/testing/path/extend" );
	EXPECT_STREQ( STRING_CONST( path21, "/testing/path/extend/dyn" );
	EXPECT_STREQ( STRING_CONST( path22, "/testing/path/extend" );
	EXPECT_STREQ( STRING_CONST( path23, "http://some/file" );
	EXPECT_STREQ( STRING_CONST( path24, "http://path" );
	EXPECT_STREQ( STRING_CONST( path25, "http://path" );
	EXPECT_STREQ( STRING_CONST( path26, "http://path" );

	string_deallocate( STRING_CONST( path1 );
	string_deallocate( STRING_CONST( path2 );
	string_deallocate( STRING_CONST( path3 );
	string_deallocate( STRING_CONST( path4 );
	string_deallocate( STRING_CONST( path5 );
	string_deallocate( STRING_CONST( path6 );
	string_deallocate( STRING_CONST( path7 );
	string_deallocate( STRING_CONST( path8 );
	string_deallocate( STRING_CONST( path9 );
	string_deallocate( STRING_CONST( path10 );
	string_deallocate( STRING_CONST( path11 );
	string_deallocate( STRING_CONST( path12 );
	string_deallocate( STRING_CONST( path13 );
	string_deallocate( STRING_CONST( path14 );
	string_deallocate( STRING_CONST( path15 );
	string_deallocate( STRING_CONST( path16 );
	string_deallocate( STRING_CONST( path17 );
	string_deallocate( STRING_CONST( path18 );
	string_deallocate( STRING_CONST( path19 );
	string_deallocate( STRING_CONST( path20 );
	string_deallocate( STRING_CONST( path21 );
	string_deallocate( STRING_CONST( path22 );
	string_deallocate( STRING_CONST( path23 );
	string_deallocate( STRING_CONST( path24 );
	string_deallocate( STRING_CONST( path25 );
	string_deallocate( STRING_CONST( path26 );

	return 0;
}


DECLARE_TEST( path, absolute )
{
	char* path1;
	char* path2;
	char* path3;
	char* path4;
	char* path5;
	char* path6;
	char* path7;
	char* path8;
	char* path9;
	char* path10;
	char* path11;
	char* path12;
	char* path13;
	char* path14;
	char* path15;
	char* path16;
	char* path17;
	char* path18;
	char* path19;
	char* path20;
	char* path21;

	const char* cwd = environment_current_working_directory();

	char* cwd_sub = path_directory_name( cwd );
	char* cwd_sub_sub = path_directory_name( cwd_sub );
	char* cwd_test = path_merge( cwd, "test" );
	char* cwd_test_path = path_merge( cwd, "test/path" );

	path1 = path_make_absolute( "" );
	path2 = path_make_absolute( "/" );
	path3 = path_make_absolute( "./" );
	path4 = path_make_absolute( "/." );
	path5 = path_make_absolute( "../" );
	path6 = path_make_absolute( "/.." );
	path7 = path_make_absolute( "../.." );
	path8 = path_make_absolute( "test" );
	path9 = path_make_absolute( "/test" );
	path10 = path_make_absolute( "test/"  );
	path11 = path_make_absolute( "test/path" );
	path12 = path_make_absolute( "/test/path" );
	path13 = path_make_absolute( "/test/path/" );
	path14 = path_make_absolute( "test/path/" );
	path15 = path_make_absolute( "vfs://test" );
	path16 = path_make_absolute( "vfs://test/path/" );
	path17 = path_make_absolute( "vfs://../test/path/../file" );
	path18 = path_make_absolute( "vfs://./test/.././///test/path/.dir/../.dir/file.ext" );
	path19 = path_make_absolute( "C:/../../../../../../.../path/../file/./././." );
	path20 = path_make_absolute( "d:f/o/o/./.o/../../../../.f/.o/.o/./" );
	path21 = path_make_absolute( "/../../test/../path/..\\.../" );

	EXPECT_STREQ( STRING_CONST( path1, cwd );
	EXPECT_STREQ( STRING_CONST( path2, "/" );
	EXPECT_STREQ( STRING_CONST( path3, cwd );
	EXPECT_STREQ( STRING_CONST( path4, "/" );
	EXPECT_STREQ( STRING_CONST( path5, cwd_sub );
	EXPECT_STREQ( STRING_CONST( path6, "/" );
	EXPECT_STREQ( STRING_CONST( path7, cwd_sub_sub );
	EXPECT_STREQ( STRING_CONST( path8, cwd_test );
	EXPECT_STREQ( STRING_CONST( path9, "/test" );
	EXPECT_STREQ( STRING_CONST( path10, cwd_test );
	EXPECT_STREQ( STRING_CONST( path11, cwd_test_path );
	EXPECT_STREQ( STRING_CONST( path12, "/test/path" );
	EXPECT_STREQ( STRING_CONST( path13, "/test/path" );
	EXPECT_STREQ( STRING_CONST( path14, cwd_test_path );
	EXPECT_STREQ( STRING_CONST( path15, "vfs://test" );
	EXPECT_STREQ( STRING_CONST( path16, "vfs://test/path" );
	EXPECT_STREQ( STRING_CONST( path17, "vfs://test/file" );
	EXPECT_STREQ( STRING_CONST( path18, "vfs://test/path/.dir/file.ext" );
	EXPECT_STREQ( STRING_CONST( path19, "C:/.../file" );
	EXPECT_STREQ( STRING_CONST( path20, "D:/.f/.o/.o" );
	EXPECT_STREQ( STRING_CONST( path21, "/..." );

	string_deallocate( cwd_sub );
	string_deallocate( cwd_sub_sub );
	string_deallocate( cwd_test );
	string_deallocate( cwd_test_path );
	string_deallocate( STRING_CONST( path1 );
	string_deallocate( STRING_CONST( path2 );
	string_deallocate( STRING_CONST( path3 );
	string_deallocate( STRING_CONST( path4 );
	string_deallocate( STRING_CONST( path5 );
	string_deallocate( STRING_CONST( path6 );
	string_deallocate( STRING_CONST( path7 );
	string_deallocate( STRING_CONST( path8 );
	string_deallocate( STRING_CONST( path9 );
	string_deallocate( STRING_CONST( path10 );
	string_deallocate( STRING_CONST( path11 );
	string_deallocate( STRING_CONST( path12 );
	string_deallocate( STRING_CONST( path13 );
	string_deallocate( STRING_CONST( path14 );
	string_deallocate( STRING_CONST( path15 );
	string_deallocate( STRING_CONST( path16 );
	string_deallocate( STRING_CONST( path17 );
	string_deallocate( STRING_CONST( path18 );
	string_deallocate( STRING_CONST( path19 );
	string_deallocate( STRING_CONST( path20 );
	string_deallocate( STRING_CONST( path21 );

	return 0;
}


DECLARE_TEST( path, operations )
{
	char* temp1;
	char* temp2;

	char* merge1;
	char* merge2;
	char* merge3;
	char* merge4;
	char* merge5;
	char* merge6;
	char* merge7;
	char* merge8;
	char* merge9;
	char* merge10;
	char* merge11;
	char* merge12;
	char* merge13;
	char* merge14;
	char* merge15;

	merge1 = path_merge( "", "" );
	merge2 = path_merge( "/", "" );
	merge3 = path_merge( "", "/" );
	merge4 = path_merge( "", "test" );
	merge5 = path_merge( "test", "" );
	merge6 = path_merge( "test", "test" );
	merge7 = path_merge( "/", "/" );
	merge8 = path_merge( "test/", "/test" );
	merge9 = path_merge( "/test", "test/" );
	merge10 = path_merge( "test://", "/test" );
	merge11 = path_merge( "test://", "../test" );
	merge12 = path_merge( "test://test", "../test" );
	merge13 = path_merge( "test://test", "../../test/" );
	merge14 = path_merge( "c:", "test" );
	merge15 = path_merge( "c:/", "/test" );

	EXPECT_STREQ( merge1, "" );
	EXPECT_STREQ( merge2, "/" );
	EXPECT_STREQ( merge3, "" );
	EXPECT_STREQ( merge4, "test" );
	EXPECT_STREQ( merge5, "test" );
	EXPECT_STREQ( merge6, "test/test" );
	EXPECT_STREQ( merge7, "/" );
	EXPECT_STREQ( merge8, "test/test" );
	EXPECT_STREQ( merge9, "/test/test" );
	EXPECT_STREQ( merge10, "test://test" );
	EXPECT_STREQ( merge11, "test://test" );
	EXPECT_STREQ( merge12, "test://test" );
	EXPECT_STREQ( merge13, "test://test" );
	EXPECT_STREQ( merge14, "C:/test" );
	EXPECT_STREQ( merge15, "C:/test" );

	string_deallocate( merge1 );
	string_deallocate( merge2 );
	string_deallocate( merge3 );
	string_deallocate( merge4 );
	string_deallocate( merge5 );
	string_deallocate( merge6 );
	string_deallocate( merge7 );
	string_deallocate( merge8 );
	string_deallocate( merge9 );
	string_deallocate( merge10 );
	string_deallocate( merge11 );
	string_deallocate( merge12 );
	string_deallocate( merge13 );
	string_deallocate( merge14 );
	string_deallocate( merge15 );

	//path_append
	//path_prepend

	temp1 = path_make_temporary();
	temp2 = path_make_temporary();

	EXPECT_NE( string_length( temp1 ), 0 );
	EXPECT_NE( string_length( temp2 ), 0 );
	EXPECT_FALSE( string_equal( temp1, temp2 ) );

	string_deallocate( temp1 );
	string_deallocate( temp2 );

	return 0;
}


DECLARE_TEST( path, query )
{
	EXPECT_TRUE( STRING_CONST( path_is_absolute( "/" ) );
	EXPECT_TRUE( STRING_CONST( path_is_absolute( "/test/" ) );
	EXPECT_TRUE( STRING_CONST( path_is_absolute( "C:/test" ) );
	EXPECT_TRUE( STRING_CONST( path_is_absolute( "C:test" ) );
	EXPECT_TRUE( STRING_CONST( path_is_absolute( "vfs://test" ) );
	EXPECT_TRUE( STRING_CONST( path_is_absolute( "vfs:///test" ) );
	EXPECT_TRUE( STRING_CONST( path_is_absolute( "vfs:///" ) );
	EXPECT_FALSE( STRING_CONST( path_is_absolute( "./" ) );
	EXPECT_FALSE( STRING_CONST( path_is_absolute( "test/" ) );
	EXPECT_FALSE( STRING_CONST( path_is_absolute( "vfs:/test" ) );

	return 0;
}


static void test_path_declare( void )
{
	ADD_TEST( STRING_CONST( path, extract );
	ADD_TEST( STRING_CONST( path, clean );
	ADD_TEST( STRING_CONST( path, absolute );
	ADD_TEST( STRING_CONST( path, operations );
	ADD_TEST( STRING_CONST( path, query );
}


static test_suite_t test_path_suite = {
	test_path_application,
	test_path_memory_system,
	test_path_declare,
	test_path_initialize,
	test_path_shutdown
};


#if BUILD_MONOLITHIC

int test_path_run( void );
int test_path_run( void )
{
	test_suite = test_path_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void );
test_suite_t test_suite_define( void )
{
	return test_path_suite;
}

#endif
