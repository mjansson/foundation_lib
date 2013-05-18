/* main.c  -  Foundation filesystem test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
	app.name = "Foundation filesystem tests";
	app.short_name = "test_fs";
	app.config_dir = "test_fs";
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


DECLARE_TEST( fs, directory )
{
	char* longpath;
	char* testpath = path_merge( environment_temporary_directory(), string_from_int_static( random64(), 0, 0 ) );

	if( !fs_is_file( testpath ) )
		fs_remove_file( testpath );
	if( !fs_is_directory( testpath ) )
		fs_make_directory( testpath );

	EXPECT_TRUE( fs_is_directory( testpath ) );

	fs_remove_directory( testpath );
	EXPECT_FALSE( fs_is_directory( testpath ) );

	longpath = path_merge( testpath, string_from_int_static( random64(), 0, 0 ) );
	EXPECT_FALSE( fs_is_directory( longpath ) );

	fs_make_directory( longpath );
	EXPECT_TRUE( fs_is_directory( longpath ) );

	fs_remove_directory( testpath );
	EXPECT_FALSE( fs_is_directory( testpath ) );
	EXPECT_FALSE( fs_is_directory( longpath ) );

	string_deallocate( longpath );
	string_deallocate( testpath );

	return 0;
}


DECLARE_TEST( fs, file )
{
	char* testpath = path_merge( environment_temporary_directory(), string_from_int_static( random64(), 0, 0 ) );
	char* copypath = path_merge( environment_temporary_directory(), string_from_int_static( random64(), 0, 0 ) );
	stream_t* teststream = 0;

	if( !fs_is_directory( environment_temporary_directory() ) )
		fs_make_directory( environment_temporary_directory() );

	if( fs_is_directory( testpath ) )
		fs_remove_directory( testpath );
	fs_remove_file( testpath );

	if( fs_is_directory( copypath ) )
		fs_remove_directory( copypath );
	fs_remove_file( copypath );


	teststream = fs_open_file( testpath, STREAM_IN );
	EXPECT_EQ( teststream, 0 );
	EXPECT_FALSE( fs_is_file( testpath ) );

	teststream = fs_open_file( testpath, STREAM_IN | STREAM_OUT );
	EXPECT_NE( teststream, 0 );
	EXPECT_TRUE( fs_is_file( testpath ) );

	stream_deallocate( teststream );
	fs_remove_file( testpath );
	teststream = fs_open_file( testpath, STREAM_IN );
	EXPECT_EQ( teststream, 0 );
	EXPECT_FALSE( fs_is_file( testpath ) );

	teststream = fs_open_file( testpath, STREAM_OUT );
	EXPECT_NE( teststream, 0 );
	EXPECT_TRUE( fs_is_file( testpath ) );

	stream_deallocate( teststream );
	teststream = 0;

	fs_copy_file( testpath, copypath );
	EXPECT_TRUE( fs_is_file( copypath ) );

	fs_remove_file( copypath );
	EXPECT_FALSE( fs_is_file( copypath ) );

	teststream = fs_open_file( testpath, STREAM_OUT );
	EXPECT_NE( teststream, 0 );
	EXPECT_TRUE( fs_is_file( testpath ) );

	stream_write_string( teststream, "testing testing" );

	stream_deallocate( teststream );
	teststream = 0;

	fs_copy_file( testpath, copypath );
	EXPECT_TRUE( fs_is_file( copypath ) );

	fs_remove_file( copypath );
	EXPECT_FALSE( fs_is_file( copypath ) );

	stream_deallocate( teststream );
	string_deallocate( testpath );
	string_deallocate( copypath );

	return 0;
}


DECLARE_TEST( fs, util )
{
	tick_t systime = time_system();
	tick_t lastmod = 0;
	char* testpath = path_merge( environment_temporary_directory(), string_from_int_static( random64(), 0, 0 ) );

	if( !fs_is_directory( environment_temporary_directory() ) )
		fs_make_directory( environment_temporary_directory() );

	if( fs_is_directory( testpath ) )
		fs_remove_directory( testpath );
	fs_remove_file( testpath );

	EXPECT_EQ( fs_last_modified( testpath ), 0 );

	thread_sleep( 1000 ); //For fs time granularity, make sure at least one second passed since systime

	stream_deallocate( fs_open_file( testpath, STREAM_OUT ) );
	EXPECT_TRUE( fs_is_file( testpath ) );
	EXPECT_GE( fs_last_modified( testpath ), systime );

	fs_remove_file( testpath );
	EXPECT_FALSE( fs_is_file( testpath ) );
	EXPECT_EQ( fs_last_modified( testpath ), 0 );

	stream_deallocate( fs_open_file( testpath, STREAM_OUT ) );
	EXPECT_TRUE( fs_is_file( testpath ) );
	EXPECT_GE( fs_last_modified( testpath ), systime );

	lastmod = fs_last_modified( testpath );

	thread_sleep( 5000 );

	EXPECT_EQ( fs_last_modified( testpath ), lastmod );

	fs_touch( testpath );
	EXPECT_GT( fs_last_modified( testpath ), lastmod );

	fs_remove_file( testpath );
	string_deallocate( testpath );

	return 0;
}


DECLARE_TEST( fs, query )
{
	uint64_t subpathid = random64();
	uint64_t subfileid = random64();
	char* testpath = path_merge( environment_temporary_directory(), string_from_int_static( random64(), 0, 0 ) );
	char* subtestpath = path_merge( testpath, string_from_int_static( subpathid, 0, 0 ) );
	char* filepath[8];
	char** subdirs;
	char** files;
	int ifp = 0;
	char* subfilepath;

	if( fs_is_file( testpath ) )
		fs_remove_file( testpath );
	if( !fs_is_directory( testpath ) )
		fs_make_directory( testpath );
	if( !fs_is_directory( subtestpath ) )
		fs_make_directory( subtestpath );

	for( ifp = 0; ifp < 8; ++ifp )
	{
		filepath[ifp] = path_merge( testpath, string_from_int_static( random64(), 0, 0 ) );
		filepath[ifp] = string_append( string_append( filepath[ifp], "." ), string_from_int_static( ifp, 0, 0 ) );
		stream_deallocate( fs_open_file( filepath[ifp], STREAM_OUT ) );
	}

	subfilepath = path_merge( subtestpath, string_from_int_static( subfileid, 0, 0 ) );
	subfilepath = string_append( subfilepath, ".0" );
	stream_deallocate( fs_open_file( subfilepath, STREAM_OUT ) );

	files = fs_files( filepath[0] );
	EXPECT_EQ( array_size( files ), 0 );
	string_array_deallocate( files );

	subdirs = fs_subdirs( subtestpath );
	EXPECT_EQ( array_size( subdirs ), 0 );
	string_array_deallocate( subdirs );

	files = fs_files( testpath );
	EXPECT_EQ( array_size( files ), 8 );
	string_array_deallocate( files );

	subdirs = fs_subdirs( testpath );
	EXPECT_EQ( array_size( subdirs ), 1 );
	string_array_deallocate( subdirs );

	files = fs_matching_files( testpath, "*", false );
	EXPECT_EQ( array_size( files ), 8 );
	string_array_deallocate( files );

	files = fs_matching_files( testpath, "*", true );
	EXPECT_EQ( array_size( files ), 9 );
	string_array_deallocate( files );

	files = fs_matching_files( testpath, "*.0", false );
	EXPECT_EQ( array_size( files ), 1 );
	string_array_deallocate( files );

	files = fs_matching_files( testpath, "*.0", true );
	EXPECT_EQ( array_size( files ), 2 );
	string_array_deallocate( files );

	files = fs_matching_files( testpath, "*.1", false );
	EXPECT_EQ( array_size( files ), 1 );
	string_array_deallocate( files );

	files = fs_matching_files( testpath, "*.1", true );
	EXPECT_EQ( array_size( files ), 1 );
	string_array_deallocate( files );

	files = fs_matching_files( testpath, "*.?", true );
	EXPECT_EQ( array_size( files ), 9 );
	{
		char* verifypath = string_from_int( subpathid, 0, 0 );
		verifypath = path_append( verifypath, string_from_int_static( subfileid, 0, 0 ) );
		verifypath = string_append( verifypath, ".0" );
		EXPECT_STREQ( files[8], verifypath );
		string_deallocate( verifypath );
	}
	string_array_deallocate( files );

	fs_remove_directory( testpath );

	string_array_deallocate( subdirs );
	string_array_deallocate( files );
	string_deallocate( subfilepath );
	for( ifp = 0; ifp < 8; ++ifp )
		string_deallocate( filepath[ifp] );
	string_deallocate( subtestpath );
	string_deallocate( testpath );
	return 0;
}


DECLARE_TEST( fs, event )
{
	event_stream_t* stream;
	event_block_t* block;
	event_t* event;

	char pathstr[] = "/some/path/to/some/file/being/tested";

	stream = fs_event_stream();
	EXPECT_NE( stream, 0 );

	fs_post_event( FOUNDATIONEVENT_FILE_CREATED, pathstr, 36 );

	block = event_stream_process( stream );
	event = event_next( block, 0 );
	EXPECT_NE( event, 0 );

	EXPECT_EQ( event->system, SYSTEM_FOUNDATION );
	EXPECT_EQ( event->id, FOUNDATIONEVENT_FILE_CREATED );
	EXPECT_STREQ( event->payload, pathstr );

	event = event_next( block, event );
	EXPECT_EQ( event, 0 );

	block = event_stream_process( stream );
	event = event_next( block, 0 );
	EXPECT_EQ( event, 0 );

	return 0;
}


DECLARE_TEST( fs, monitor )
{
	char* testpath = path_merge( environment_temporary_directory(), string_from_int_static( random64(), 0, 0 ) );
	char* filetestpath = path_merge( testpath, string_from_int_static( random64(), 0, 0 ) );

	stream_t* test_stream;

	event_stream_t* stream;
	event_block_t* block;
	event_t* event;
	
	stream = fs_event_stream();

	if( fs_is_file( testpath ) )
		fs_remove_file( testpath );
	if( !fs_is_directory( testpath ) )
		fs_make_directory( testpath );
	if( fs_is_file( filetestpath ) )
		fs_remove_file( filetestpath );

	stream_deallocate( fs_open_file( filetestpath, STREAM_OUT ) );
	fs_remove_file( filetestpath );

	block = event_stream_process( stream );
	event = event_next( block, 0 );
	EXPECT_EQ( event, 0 );

	fs_monitor( testpath );
	thread_sleep( 1000 );

	stream_deallocate( fs_open_file( filetestpath, STREAM_OUT ) );
	thread_sleep( 100 );

	block = event_stream_process( stream );
	event = event_next( block, 0 );
	EXPECT_NE( event, 0 );
	EXPECT_EQ( event->system, SYSTEM_FOUNDATION );
	EXPECT_EQ( event->id, FOUNDATIONEVENT_FILE_CREATED );
	EXPECT_STREQ( event->payload, filetestpath );

	event = event_next( block, event );
	EXPECT_EQ( event, 0 );

	test_stream = fs_open_file( filetestpath, STREAM_IN | STREAM_OUT );
	stream_write_string( test_stream, filetestpath );
	stream_deallocate( test_stream );
	thread_sleep( 100 );

	block = event_stream_process( stream );
	event = event_next( block, 0 );
	EXPECT_NE( event, 0 );
	EXPECT_EQ( event->system, SYSTEM_FOUNDATION );
	EXPECT_EQ( event->id, FOUNDATIONEVENT_FILE_MODIFIED );
	EXPECT_STREQ( event->payload, filetestpath );

	event = event_next( block, event );
	EXPECT_EQ( event, 0 );

	fs_remove_file( filetestpath );
	thread_sleep( 100 );

	block = event_stream_process( stream );
	event = event_next( block, 0 );
	EXPECT_NE( event, 0 );
	EXPECT_EQ( event->system, SYSTEM_FOUNDATION );
	EXPECT_EQ( event->id, FOUNDATIONEVENT_FILE_DELETED );
	EXPECT_STREQ( event->payload, filetestpath );

	event = event_next( block, event );
	EXPECT_EQ( event, 0 );

	fs_unmonitor( testpath );
	thread_sleep( 1000 );
	
	block = event_stream_process( stream );
	event = event_next( block, 0 );
	EXPECT_EQ( event, 0 );

	stream_deallocate( fs_open_file( filetestpath, STREAM_OUT ) );
	thread_sleep( 100 );

	block = event_stream_process( stream );
	event = event_next( block, 0 );
	EXPECT_EQ( event, 0 );

	fs_remove_file( filetestpath );
	thread_sleep( 100 );

	block = event_stream_process( stream );
	event = event_next( block, 0 );
	EXPECT_EQ( event, 0 );

	fs_remove_directory( testpath );

	string_deallocate( testpath );
	string_deallocate( filetestpath );

	return 0;
}


void test_declare( void )
{
	ADD_TEST( fs, directory );
	ADD_TEST( fs, file );
	ADD_TEST( fs, util );
	ADD_TEST( fs, query );
	ADD_TEST( fs, event );
	ADD_TEST( fs, monitor );
}
