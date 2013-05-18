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


application_t test_application( void )
{
	application_t app = {0};
	app.name = "Foundation path tests";
	app.short_name = "test_path";
	app.config_dir = "test_path";
	app.flags = APPLICATION_UTILITY;
	return app;
}


int test_initialize( void )
{
	return 0;
}


void test_shutdown( void )
{
}


DECLARE_TEST( path, extract )
{
	const char* path1 = "file.ext";
	const char* path2 = "file";
	const char* path3 = "dir/";
	const char* path4 = "path/file.ext";
	const char* path5 = "path/file";
	const char* path6 = "path/dir/";
	const char* path7 = "more/some/path/file.ext";
	const char* path8 = "more/some/path/file";
	const char* path9 = "more/some/path/dir/";
	const char* path10 = "/abs/path/file.ext";
	const char* path11 = "/abs/path/file";
	const char* path12 = "/abs/path/dir/";
	const char* path13 = "C:/abs/path/file.ext";
	const char* path14 = "C:/abs/path/file";
	const char* path15 = "C:/abs/path/dir/";
	const char* path16 = "vfs://abs/path/file.ext";
	const char* path17 = "vfs://abs/path/file";
	const char* path18 = "vfs://abs/path/dir/";
	const char* path19 = "";
	const char* path20 = "/";
	const char* path21 = "asset://.path/file.ext";
	const char* path22 = "asset://.path/file";
	const char* path23 = "asset://.path/.dir/";
	char* testpath;
	
	testpath = path_base_file_name( path1 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name( path2 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name( path3 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_base_file_name( path4 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name( path5 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name( path6 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_base_file_name( path7 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name( path8 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name( path9 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_base_file_name( path10 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name( path11 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name( path12 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_base_file_name( path13 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name( path14 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name( path15 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_base_file_name( path16 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name( path17 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name( path18 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_base_file_name( path19 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_base_file_name( path20 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_base_file_name( path21 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name( path22 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name( path23 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );

	testpath = path_base_file_name_with_path( path1 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path2 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path3 ); EXPECT_STREQ( testpath, "dir" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path4 ); EXPECT_STREQ( testpath, "path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path5 ); EXPECT_STREQ( testpath, "path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path6 ); EXPECT_STREQ( testpath, "path/dir" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path7 ); EXPECT_STREQ( testpath, "more/some/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path8 ); EXPECT_STREQ( testpath, "more/some/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path9 ); EXPECT_STREQ( testpath, "more/some/path/dir" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path10 ); EXPECT_STREQ( testpath, "/abs/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path11 ); EXPECT_STREQ( testpath, "/abs/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path12 ); EXPECT_STREQ( testpath, "/abs/path/dir" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path13 ); EXPECT_STREQ( testpath, "C:/abs/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path14 ); EXPECT_STREQ( testpath, "C:/abs/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path15 ); EXPECT_STREQ( testpath, "C:/abs/path/dir" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path16 ); EXPECT_STREQ( testpath, "vfs://abs/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path17 ); EXPECT_STREQ( testpath, "vfs://abs/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path18 ); EXPECT_STREQ( testpath, "vfs://abs/path/dir" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path19 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path20 ); EXPECT_STREQ( testpath, "/" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path21 ); EXPECT_STREQ( testpath, "asset://.path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path22 ); EXPECT_STREQ( testpath, "asset://.path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path23 ); EXPECT_STREQ( testpath, "asset://.path/.dir" ); string_deallocate( testpath );

	testpath = path_file_extension( path1 ); EXPECT_STREQ( testpath, "ext" ); string_deallocate( testpath );
	testpath = path_file_extension( path2 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_extension( path3 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_extension( path4 ); EXPECT_STREQ( testpath, "ext" ); string_deallocate( testpath );
	testpath = path_file_extension( path5 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_extension( path6 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_extension( path7 ); EXPECT_STREQ( testpath, "ext" ); string_deallocate( testpath );
	testpath = path_file_extension( path8 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_extension( path9 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_extension( path10 ); EXPECT_STREQ( testpath, "ext" ); string_deallocate( testpath );
	testpath = path_file_extension( path11 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_extension( path12 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_extension( path13 ); EXPECT_STREQ( testpath, "ext" ); string_deallocate( testpath );
	testpath = path_file_extension( path14 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_extension( path15 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_extension( path16 ); EXPECT_STREQ( testpath, "ext" ); string_deallocate( testpath );
	testpath = path_file_extension( path17 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_extension( path18 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_extension( path19 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_extension( path20 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_extension( path21 ); EXPECT_STREQ( testpath, "ext" ); string_deallocate( testpath );
	testpath = path_file_extension( path22 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_extension( path23 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );

	testpath = path_file_name( path1 ); EXPECT_STREQ( testpath, "file.ext" ); string_deallocate( testpath );
	testpath = path_file_name( path2 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_file_name( path3 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_name( path4 ); EXPECT_STREQ( testpath, "file.ext" ); string_deallocate( testpath );
	testpath = path_file_name( path5 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_file_name( path6 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_name( path7 ); EXPECT_STREQ( testpath, "file.ext" ); string_deallocate( testpath );
	testpath = path_file_name( path8 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_file_name( path9 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_name( path10 ); EXPECT_STREQ( testpath, "file.ext" ); string_deallocate( testpath );
	testpath = path_file_name( path11 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_file_name( path12 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_name( path13 ); EXPECT_STREQ( testpath, "file.ext" ); string_deallocate( testpath );
	testpath = path_file_name( path14 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_file_name( path15 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_name( path16 ); EXPECT_STREQ( testpath, "file.ext" ); string_deallocate( testpath );
	testpath = path_file_name( path17 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_file_name( path18 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_name( path19 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_name( path20 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_file_name( path21 ); EXPECT_STREQ( testpath, "file.ext" ); string_deallocate( testpath );
	testpath = path_file_name( path22 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_file_name( path23 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );

	testpath = path_path_name( path1 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_path_name( path2 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_path_name( path3 ); EXPECT_STREQ( testpath, "dir" ); string_deallocate( testpath );
	testpath = path_path_name( path4 ); EXPECT_STREQ( testpath, "path" ); string_deallocate( testpath );
	testpath = path_path_name( path5 ); EXPECT_STREQ( testpath, "path" ); string_deallocate( testpath );
	testpath = path_path_name( path6 ); EXPECT_STREQ( testpath, "path/dir" ); string_deallocate( testpath );
	testpath = path_path_name( path7 ); EXPECT_STREQ( testpath, "more/some/path" ); string_deallocate( testpath );
	testpath = path_path_name( path8 ); EXPECT_STREQ( testpath, "more/some/path" ); string_deallocate( testpath );
	testpath = path_path_name( path9 ); EXPECT_STREQ( testpath, "more/some/path/dir" ); string_deallocate( testpath );
	testpath = path_path_name( path10 ); EXPECT_STREQ( testpath, "/abs/path" ); string_deallocate( testpath );
	testpath = path_path_name( path11 ); EXPECT_STREQ( testpath, "/abs/path" ); string_deallocate( testpath );
	testpath = path_path_name( path12 ); EXPECT_STREQ( testpath, "/abs/path/dir" ); string_deallocate( testpath );
	testpath = path_path_name( path13 ); EXPECT_STREQ( testpath, "C:/abs/path" ); string_deallocate( testpath );
	testpath = path_path_name( path14 ); EXPECT_STREQ( testpath, "C:/abs/path" ); string_deallocate( testpath );
	testpath = path_path_name( path15 ); EXPECT_STREQ( testpath, "C:/abs/path/dir" ); string_deallocate( testpath );
	testpath = path_path_name( path16 ); EXPECT_STREQ( testpath, "vfs://abs/path" ); string_deallocate( testpath );
	testpath = path_path_name( path17 ); EXPECT_STREQ( testpath, "vfs://abs/path" ); string_deallocate( testpath );
	testpath = path_path_name( path18 ); EXPECT_STREQ( testpath, "vfs://abs/path/dir" ); string_deallocate( testpath );
	testpath = path_path_name( path19 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_path_name( path20 ); EXPECT_STREQ( testpath, "/" ); string_deallocate( testpath );
	testpath = path_path_name( path21 ); EXPECT_STREQ( testpath, "asset://.path" ); string_deallocate( testpath );
	testpath = path_path_name( path22 ); EXPECT_STREQ( testpath, "asset://.path" ); string_deallocate( testpath );
	testpath = path_path_name( path23 ); EXPECT_STREQ( testpath, "asset://.path/.dir" ); string_deallocate( testpath );

	testpath = path_subpath_name( path1, "file" ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_subpath_name( path2, "" ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_subpath_name( path3, "" ); EXPECT_STREQ( testpath, "dir" ); string_deallocate( testpath );
	testpath = path_subpath_name( path4, "path" ); EXPECT_STREQ( testpath, "file.ext" ); string_deallocate( testpath );
	testpath = path_subpath_name( path5, "file" ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_subpath_name( path6, "path/dir" ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_subpath_name( path7, "more" ); EXPECT_STREQ( testpath, "some/path/file.ext" ); string_deallocate( testpath );
	testpath = path_subpath_name( path8, "/more" ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_subpath_name( path9, "more/some/path" ); EXPECT_STREQ( testpath, "dir" ); string_deallocate( testpath );
	testpath = path_subpath_name( path10, "ab" ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_subpath_name( path11, "abs/path" ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_subpath_name( path12, "/abs" ); EXPECT_STREQ( testpath, "path/dir" ); string_deallocate( testpath );
	testpath = path_subpath_name( path13, "/ab" ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_subpath_name( path14, "C:/abs/" ); EXPECT_STREQ( testpath, "path/file" ); string_deallocate( testpath );
	testpath = path_subpath_name( path15, "C:" ); EXPECT_STREQ( testpath, "abs/path/dir" ); string_deallocate( testpath );
	testpath = path_subpath_name( path16, "abs/path" ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_subpath_name( path17, "/abs" ); EXPECT_STREQ( testpath, "path/file" ); string_deallocate( testpath );
	testpath = path_subpath_name( path18, "/abs/path/" ); EXPECT_STREQ( testpath, "dir" ); string_deallocate( testpath );
	testpath = path_subpath_name( path19, "/" ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_subpath_name( path20, "/" ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_subpath_name( path21, "/.path" ); EXPECT_STREQ( testpath, "file.ext" ); string_deallocate( testpath );
	testpath = path_subpath_name( path22, ".path" ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_subpath_name( path23, "/" ); EXPECT_STREQ( testpath, ".path/.dir" ); string_deallocate( testpath );

	testpath = path_protocol( path1 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path2 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path3 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path4 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path5 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path6 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path7 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path8 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path9 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path10 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path11 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path12 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path13 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path14 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path15 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path16 ); EXPECT_STREQ( testpath, "vfs" ); string_deallocate( testpath );
	testpath = path_protocol( path17 ); EXPECT_STREQ( testpath, "vfs" ); string_deallocate( testpath );
	testpath = path_protocol( path18 ); EXPECT_STREQ( testpath, "vfs" ); string_deallocate( testpath );
	testpath = path_protocol( path19 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path20 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_protocol( path21 ); EXPECT_STREQ( testpath, "asset" ); string_deallocate( testpath );
	testpath = path_protocol( path22 ); EXPECT_STREQ( testpath, "asset" ); string_deallocate( testpath );
	testpath = path_protocol( path23 ); EXPECT_STREQ( testpath, "asset" ); string_deallocate( testpath );

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
	
	path1 = path_clean( path1, true );
	path2 = path_clean( path2, true );
	path3 = path_clean( path3, true );
	path4 = path_clean( path4, true );
	path5 = path_clean( path5, true );
	path6 = path_clean( path6, true );
	path7 = path_clean( path7, true );
	path8 = path_clean( path8, true );
	path9 = path_clean( path9, true );
	path10 = path_clean( path10, true );
	path11 = path_clean( path11, true );
	path12 = path_clean( path12, true );
	path13 = path_clean( path13, true );
	path14 = path_clean( path14, true );
	path15 = path_clean( path15, true );
	path16 = path_clean( path16, true );
	path17 = path_clean( path17, true );
	path18 = path_clean( path18, true );
	path19 = path_clean( path19, true );
	path20 = path_clean( path20, true );
	path21 = path_clean( path21, true );
	path22 = path_clean( path22, true );
	path23 = path_clean( path23, true );
	path24 = path_clean( path24, true );
	path25 = path_clean( path25, true );
	path26 = path_clean( path26, true );

	EXPECT_STREQ( path1, "/" );
	EXPECT_STREQ( path2, "/" );
	EXPECT_STREQ( path3, "/" );
	EXPECT_STREQ( path4, "/" );
	EXPECT_STREQ( path5, "/" );
	EXPECT_STREQ( path6, "/" );
	EXPECT_STREQ( path7, "/" );
	EXPECT_STREQ( path8, "/" );
	EXPECT_STREQ( path9, "http://" );
	EXPECT_STREQ( path10, "/" );
	EXPECT_STREQ( path11, "/" );
	EXPECT_STREQ( path12, "/" );
	EXPECT_STREQ( path13, "/" );
	EXPECT_STREQ( path14, "/" );
	EXPECT_STREQ( path15, "/" );
	EXPECT_STREQ( path16, "/" );
	EXPECT_STREQ( path17, "/" );
	EXPECT_STREQ( path18, "http://" );
	EXPECT_STREQ( path19, "/testing/path/ext" );
	EXPECT_STREQ( path20, "/testing/path/extend" );
	EXPECT_STREQ( path21, "/testing/path/extend/dyn" );
	EXPECT_STREQ( path22, "/testing/path/extend" );
	EXPECT_STREQ( path23, "http://some/file" );
	EXPECT_STREQ( path24, "http://path" );
	EXPECT_STREQ( path25, "http://path" );
	EXPECT_STREQ( path26, "http://path" );

	string_deallocate( path1 );
	string_deallocate( path2 );
	string_deallocate( path3 );
	string_deallocate( path4 );
	string_deallocate( path5 );
	string_deallocate( path6 );
	string_deallocate( path7 );
	string_deallocate( path8 );
	string_deallocate( path9 );
	string_deallocate( path10 );
	string_deallocate( path11 );
	string_deallocate( path12 );
	string_deallocate( path13 );
	string_deallocate( path14 );
	string_deallocate( path15 );
	string_deallocate( path16 );
	string_deallocate( path17 );
	string_deallocate( path18 );
	string_deallocate( path19 );
	string_deallocate( path20 );
	string_deallocate( path21 );
	string_deallocate( path22 );
	string_deallocate( path23 );
	string_deallocate( path24 );
	string_deallocate( path25 );
	string_deallocate( path26 );

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

	char* cwd_sub = path_path_name( cwd );
	char* cwd_sub_sub = path_path_name( cwd_sub );
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

	EXPECT_STREQ( path1, cwd );
	EXPECT_STREQ( path2, "/" );
	EXPECT_STREQ( path3, cwd );
	EXPECT_STREQ( path4, "/" );
	EXPECT_STREQ( path5, cwd_sub );
	EXPECT_STREQ( path6, "/" );
	EXPECT_STREQ( path7, cwd_sub_sub );
	EXPECT_STREQ( path8, cwd_test );
	EXPECT_STREQ( path9, "/test" );
	EXPECT_STREQ( path10, cwd_test );
	EXPECT_STREQ( path11, cwd_test_path );
	EXPECT_STREQ( path12, "/test/path" );
	EXPECT_STREQ( path13, "/test/path" );
	EXPECT_STREQ( path14, cwd_test_path );
	EXPECT_STREQ( path15, "vfs://test" );
	EXPECT_STREQ( path16, "vfs://test/path" );
	EXPECT_STREQ( path17, "vfs://test/file" );
	EXPECT_STREQ( path18, "vfs://test/path/.dir/file.ext" );
	EXPECT_STREQ( path19, "C:/.../file" );
	EXPECT_STREQ( path20, "D:/.f/.o/.o" );
	EXPECT_STREQ( path21, "/..." );

	string_deallocate( cwd_sub );
	string_deallocate( cwd_sub_sub );
	string_deallocate( cwd_test );
	string_deallocate( cwd_test_path );
	string_deallocate( path1 );
	string_deallocate( path2 );
	string_deallocate( path3 );
	string_deallocate( path4 );
	string_deallocate( path5 );
	string_deallocate( path6 );
	string_deallocate( path7 );
	string_deallocate( path8 );
	string_deallocate( path9 );
	string_deallocate( path10 );
	string_deallocate( path11 );
	string_deallocate( path12 );
	string_deallocate( path13 );
	string_deallocate( path14 );
	string_deallocate( path15 );
	string_deallocate( path16 );
	string_deallocate( path17 );
	string_deallocate( path18 );
	string_deallocate( path19 );
	string_deallocate( path20 );
	string_deallocate( path21 );

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
	EXPECT_TRUE( path_is_absolute( "/" ) );
	EXPECT_TRUE( path_is_absolute( "/test/" ) );
	EXPECT_TRUE( path_is_absolute( "C:/test" ) );
	EXPECT_TRUE( path_is_absolute( "C:test" ) );
	EXPECT_TRUE( path_is_absolute( "vfs://test" ) );
	EXPECT_TRUE( path_is_absolute( "vfs:///test" ) );
	EXPECT_TRUE( path_is_absolute( "vfs:///" ) );
	EXPECT_FALSE( path_is_absolute( "./" ) );
	EXPECT_FALSE( path_is_absolute( "test/" ) );
	EXPECT_FALSE( path_is_absolute( "vfs:/test" ) );

	return 0;
}


void test_declare( void )
{
	ADD_TEST( path, extract );
	ADD_TEST( path, clean );
	ADD_TEST( path, absolute );
	ADD_TEST( path, operations );
	ADD_TEST( path, query );
}
