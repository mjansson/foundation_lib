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
	string_t path1 = string_clone( STRING_CONST( "" ) );
	string_t path2 = string_clone( STRING_CONST( "/" ) );
	string_t path3 = string_clone( STRING_CONST( "/." ) );
	string_t path4 = string_clone( STRING_CONST( "./" ) );
	string_t path5 = string_clone( STRING_CONST( "./." ) );
	string_t path6 = string_clone( STRING_CONST( "././" ) );
	string_t path7 = string_clone( STRING_CONST( "././//" ) );
	string_t path8 = string_clone( STRING_CONST( "././//./////././////.//////.//." ) );
	string_t path9 = string_clone( STRING_CONST( "http://././//./////././////.//////.//." ) );
	string_t path10 = string_clone( STRING_CONST( "" ) );
	string_t path11 = string_clone( STRING_CONST( "\\" ) );
	string_t path12 = string_clone( STRING_CONST( "/\\." ) );
	string_t path13 = string_clone( STRING_CONST( ".\\/" ) );
	string_t path14 = string_clone( STRING_CONST( "./\\." ) );
	string_t path15 = string_clone( STRING_CONST( ".\\.//\\" ) );
	string_t path16 = string_clone( STRING_CONST( ".\\.\\\\\\" ) );
	string_t path17 = string_clone( STRING_CONST( ".\\.\\\\\\.\\\\////\\///\\\\.\\.\\\\\\\\\\.\\\\\\\\\\\\.\\\\." ) );
	string_t path18 = string_clone( STRING_CONST( "http://\\.\\.\\\\\\.\\\\\\\\//\\.\\.\\/\\/\\/\\//\\.\\\\\\\\\\\\.\\\\." ) );
	string_t path19 = string_clone( STRING_CONST( "testing/path/ext" ) );
	string_t path20 = string_clone( STRING_CONST( "testing/path/extend" ) );
	string_t path21 = string_clone( STRING_CONST( "testing/path/extend/dyn" ) );
	string_t path22 = string_clone( STRING_CONST( "testing/./\\\\/\\/./path/././//./extend/\\\\" ) );
	string_t path23 = string_clone( STRING_CONST( "http://///\\.\\.\\\\\\.\\\\\\\\//\\.\\.\\/\\/\\/\\//\\.\\\\\\\\\\\\.\\\\.///some/file" ) );
	string_t path24 = string_clone( STRING_CONST( "http://test/../../path" ) );
	string_t path25 = string_clone( STRING_CONST( "http:///.//test/../../path" ) );
	string_t path26 = string_clone( STRING_CONST( "http:///.//test/../../../../../../path" ) );

	path1 = path_clean( STRING_ARGS_CAPACITY( path1 ), true, true );
	path2 = path_clean( STRING_ARGS_CAPACITY( path2 ), true, true );
	path3 = path_clean( STRING_ARGS_CAPACITY( path3 ), true, true );
	path4 = path_clean( STRING_ARGS_CAPACITY( path4 ), true, true );
	path5 = path_clean( STRING_ARGS_CAPACITY( path5 ), true, true );
	path6 = path_clean( STRING_ARGS_CAPACITY( path6 ), true, true );
	path7 = path_clean( STRING_ARGS_CAPACITY( path7 ), true, true );
	path8 = path_clean( STRING_ARGS_CAPACITY( path8 ), true, true );
	path9 = path_clean( STRING_ARGS_CAPACITY( path9 ), true, true );
	path10 = path_clean( STRING_ARGS_CAPACITY( path10 ), true, true );
	path11 = path_clean( STRING_ARGS_CAPACITY( path11 ), true, true );
	path12 = path_clean( STRING_ARGS_CAPACITY( path12 ), true, true );
	path13 = path_clean( STRING_ARGS_CAPACITY( path13 ), true, true );
	path14 = path_clean( STRING_ARGS_CAPACITY( path14 ), true, true );
	path15 = path_clean( STRING_ARGS_CAPACITY( path15 ), true, true );
	path16 = path_clean( STRING_ARGS_CAPACITY( path16 ), true, true );
	path17 = path_clean( STRING_ARGS_CAPACITY( path17 ), true, true );
	path18 = path_clean( STRING_ARGS_CAPACITY( path18 ), true, true );
	path19 = path_clean( STRING_ARGS_CAPACITY( path19 ), true, true );
	path20 = path_clean( STRING_ARGS_CAPACITY( path20 ), true, true );
	path21 = path_clean( STRING_ARGS_CAPACITY( path21 ), true, true );
	path22 = path_clean( STRING_ARGS_CAPACITY( path22 ), true, true );
	path23 = path_clean( STRING_ARGS_CAPACITY( path23 ), true, true );
	path24 = path_clean( STRING_ARGS_CAPACITY( path24 ), true, true );
	path25 = path_clean( STRING_ARGS_CAPACITY( path25 ), true, true );
	path26 = path_clean( STRING_ARGS_CAPACITY( path26 ), true, true );

	EXPECT_STRINGEQ( path1, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path2, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path3, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path4, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path5, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path6, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path7, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path8, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path9, string_const( STRING_CONST( "http://" ) ) );
	EXPECT_STRINGEQ( path10, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path11, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path12, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path13, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path14, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path15, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path16, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path17, string_const( STRING_CONST( "/" ) ) );
	EXPECT_STRINGEQ( path18, string_const( STRING_CONST( "http://" ) ) );
	EXPECT_STRINGEQ( path19, string_const( STRING_CONST( "/testing/path/ext" ) ) );
	EXPECT_STRINGEQ( path20, string_const( STRING_CONST( "/testing/path/extend" ) ) );
	EXPECT_STRINGEQ( path21, string_const( STRING_CONST( "/testing/path/extend/dyn" ) ) );
	EXPECT_STRINGEQ( path22, string_const( STRING_CONST( "/testing/path/extend" ) ) );
	EXPECT_STRINGEQ( path23, string_const( STRING_CONST( "http://some/file" ) ) );
	EXPECT_STRINGEQ( path24, string_const( STRING_CONST( "http://path" ) ) );
	EXPECT_STRINGEQ( path25, string_const( STRING_CONST( "http://path" ) ) );
	EXPECT_STRINGEQ( path26, string_const( STRING_CONST( "http://path" ) ) );

	string_deallocate( path1.str );
	string_deallocate( path2.str );
	string_deallocate( path3.str );
	string_deallocate( path4.str );
	string_deallocate( path5.str );
	string_deallocate( path6.str );
	string_deallocate( path7.str );
	string_deallocate( path8.str );
	string_deallocate( path9.str );
	string_deallocate( path10.str );
	string_deallocate( path11.str );
	string_deallocate( path12.str );
	string_deallocate( path13.str );
	string_deallocate( path14.str );
	string_deallocate( path15.str );
	string_deallocate( path16.str );
	string_deallocate( path17.str );
	string_deallocate( path18.str );
	string_deallocate( path19.str );
	string_deallocate( path20.str );
	string_deallocate( path21.str );
	string_deallocate( path22.str );
	string_deallocate( path23.str );
	string_deallocate( path24.str );
	string_deallocate( path25.str );
	string_deallocate( path26.str );

	return 0;
}


DECLARE_TEST( path, absolute )
{
	string_t path1;
	string_t path2;
	string_t path3;
	string_t path4;
	string_t path5;
	string_t path6;
	string_t path7;
	string_t path8;
	string_t path9;
	string_t path10;
	string_t path11;
	string_t path12;
	string_t path13;
	string_t path14;
	string_t path15;
	string_t path16;
	string_t path17;
	string_t path18;
	string_t path19;
	string_t path20;
	string_t path21;

	const char* cwd = environment_current_working_directory();

	char* cwd_sub = path_directory_name( cwd );
	char* cwd_sub_sub = path_directory_name( cwd_sub );
	char* cwd_test = path_merge( cwd, string_const( STRING_CONST( "test" );
	char* cwd_test_path = path_merge( cwd, string_const( STRING_CONST( "test/path" );

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
	EXPECT_STREQ( STRING_CONST( path2, string_const( STRING_CONST( "/" );
	EXPECT_STREQ( STRING_CONST( path3, cwd );
	EXPECT_STREQ( STRING_CONST( path4, string_const( STRING_CONST( "/" );
	EXPECT_STREQ( STRING_CONST( path5, cwd_sub );
	EXPECT_STREQ( STRING_CONST( path6, string_const( STRING_CONST( "/" );
	EXPECT_STREQ( STRING_CONST( path7, cwd_sub_sub );
	EXPECT_STREQ( STRING_CONST( path8, cwd_test );
	EXPECT_STREQ( STRING_CONST( path9, string_const( STRING_CONST( "/test" );
	EXPECT_STREQ( STRING_CONST( path10, cwd_test );
	EXPECT_STREQ( STRING_CONST( path11, cwd_test_path );
	EXPECT_STREQ( STRING_CONST( path12, string_const( STRING_CONST( "/test/path" );
	EXPECT_STREQ( STRING_CONST( path13, string_const( STRING_CONST( "/test/path" );
	EXPECT_STREQ( STRING_CONST( path14, cwd_test_path );
	EXPECT_STREQ( STRING_CONST( path15, string_const( STRING_CONST( "vfs://test" );
	EXPECT_STREQ( STRING_CONST( path16, string_const( STRING_CONST( "vfs://test/path" );
	EXPECT_STREQ( STRING_CONST( path17, string_const( STRING_CONST( "vfs://test/file" );
	EXPECT_STREQ( STRING_CONST( path18, string_const( STRING_CONST( "vfs://test/path/.dir/file.ext" );
	EXPECT_STREQ( STRING_CONST( path19, string_const( STRING_CONST( "C:/.../file" );
	EXPECT_STREQ( STRING_CONST( path20, string_const( STRING_CONST( "D:/.f/.o/.o" );
	EXPECT_STREQ( STRING_CONST( path21, string_const( STRING_CONST( "/..." );

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

	merge1 = path_merge( "", string_const( STRING_CONST( "" );
	merge2 = path_merge( "/", string_const( STRING_CONST( "" );
	merge3 = path_merge( "", string_const( STRING_CONST( "/" );
	merge4 = path_merge( "", string_const( STRING_CONST( "test" );
	merge5 = path_merge( "test", string_const( STRING_CONST( "" );
	merge6 = path_merge( "test", string_const( STRING_CONST( "test" );
	merge7 = path_merge( "/", string_const( STRING_CONST( "/" );
	merge8 = path_merge( "test/", string_const( STRING_CONST( "/test" );
	merge9 = path_merge( "/test", string_const( STRING_CONST( "test/" );
	merge10 = path_merge( "test://", string_const( STRING_CONST( "/test" );
	merge11 = path_merge( "test://", string_const( STRING_CONST( "../test" );
	merge12 = path_merge( "test://test", string_const( STRING_CONST( "../test" );
	merge13 = path_merge( "test://test", string_const( STRING_CONST( "../../test/" );
	merge14 = path_merge( "c:", string_const( STRING_CONST( "test" );
	merge15 = path_merge( "c:/", string_const( STRING_CONST( "/test" );

	EXPECT_STREQ( merge1, string_const( STRING_CONST( "" );
	EXPECT_STREQ( merge2, string_const( STRING_CONST( "/" );
	EXPECT_STREQ( merge3, string_const( STRING_CONST( "" );
	EXPECT_STREQ( merge4, string_const( STRING_CONST( "test" );
	EXPECT_STREQ( merge5, string_const( STRING_CONST( "test" );
	EXPECT_STREQ( merge6, string_const( STRING_CONST( "test/test" );
	EXPECT_STREQ( merge7, string_const( STRING_CONST( "/" );
	EXPECT_STREQ( merge8, string_const( STRING_CONST( "test/test" );
	EXPECT_STREQ( merge9, string_const( STRING_CONST( "/test/test" );
	EXPECT_STREQ( merge10, string_const( STRING_CONST( "test://test" );
	EXPECT_STREQ( merge11, string_const( STRING_CONST( "test://test" );
	EXPECT_STREQ( merge12, string_const( STRING_CONST( "test://test" );
	EXPECT_STREQ( merge13, string_const( STRING_CONST( "test://test" );
	EXPECT_STREQ( merge14, string_const( STRING_CONST( "C:/test" );
	EXPECT_STREQ( merge15, string_const( STRING_CONST( "C:/test" );

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
