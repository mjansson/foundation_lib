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
	return app;
}


DECLARE_TEST( path, extract )
{
	const char* path1 = "file";
	const char* path2 = "file.ext";
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

	testpath = path_base_file_name_with_path( path1 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path2 ); EXPECT_STREQ( testpath, "file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path3 ); EXPECT_STREQ( testpath, "dir/" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path4 ); EXPECT_STREQ( testpath, "path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path5 ); EXPECT_STREQ( testpath, "path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path6 ); EXPECT_STREQ( testpath, "path/dir/" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path7 ); EXPECT_STREQ( testpath, "more/some/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path8 ); EXPECT_STREQ( testpath, "more/some/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path9 ); EXPECT_STREQ( testpath, "more/some/path/dir/" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path10 ); EXPECT_STREQ( testpath, "/abs/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path11 ); EXPECT_STREQ( testpath, "/abs/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path12 ); EXPECT_STREQ( testpath, "/abs/path/dir/" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path13 ); EXPECT_STREQ( testpath, "C:/abs/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path14 ); EXPECT_STREQ( testpath, "C:/abs/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path15 ); EXPECT_STREQ( testpath, "C:/abs/path/dir/" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path16 ); EXPECT_STREQ( testpath, "vfs://abs/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path17 ); EXPECT_STREQ( testpath, "vfs://abs/path/file" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path18 ); EXPECT_STREQ( testpath, "vfs://abs/path/dir/" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path19 ); EXPECT_STREQ( testpath, "" ); string_deallocate( testpath );
	testpath = path_base_file_name_with_path( path20 ); EXPECT_STREQ( testpath, "/" ); string_deallocate( testpath );
	
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

	return 0;
}


void test_declare( void )
{
	ADD_TEST( path, extract );
	ADD_TEST( path, clean );
}
