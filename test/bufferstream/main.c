/* main.c  -  Foundation bufferstream test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


static application_t test_bufferstream_application( void )
{
	application_t app;
	memset( &app, 0, sizeof( app ) );
	app.name = "Foundation bufferstream tests";
	app.short_name = "test_bufferstream";
	app.config_dir = "test_bufferstream";
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_bufferstream_memory_system( void )
{
	return memory_system_malloc();
}


static int test_bufferstream_initialize( void )
{
	return 0;
}


static void test_bufferstream_shutdown( void )
{
}


DECLARE_TEST( bufferstream, null )
{
	stream_t* stream = 0;
	tick_t curtime = time_current();
	uint8_t readbuffer[1024] = {0};
	uint8_t writebuffer[1024] = {0};
	uint128_t md5zero;

	md5zero = uint128_null();

	stream = buffer_stream_allocate( 0, 0, 0, 0, false, false );
	EXPECT_NE( stream, 0 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 0 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_FALSE( stream_is_binary( stream ) );
	EXPECT_FALSE( stream_is_sequential( stream ) );
	EXPECT_TRUE( stream_is_reliable( stream ) );
	EXPECT_TRUE( stream_is_inorder( stream ) );
	EXPECT_TRUE( string_equal_substr( stream_path( stream ), "buffer://0x", 11 ) );
	EXPECT_GE( stream_last_modified( stream ), curtime );
	EXPECT_EQ( stream_available_read( stream ), 0 );
	EXPECT_TRUE( uint128_equal( stream_md5( stream ), md5zero ) );

	//No buffer and not growing - all read/write ops should do nothing
	EXPECT_EQ( stream_read( stream, readbuffer, 1024 ), 0 );
	EXPECT_EQ( stream_write( stream, writebuffer, 1024 ), 0 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 0 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_EQ( stream_available_read( stream ), 0 );
	EXPECT_TRUE( uint128_equal( stream_md5( stream ), md5zero ) );

	stream_truncate( stream, 1024 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 0 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_EQ( stream_available_read( stream ), 0 );
	EXPECT_TRUE( uint128_equal( stream_md5( stream ), md5zero ) );

	EXPECT_EQ( stream_read( stream, readbuffer, 1024 ), 0 );
	EXPECT_EQ( stream_write( stream, writebuffer, 1024 ), 0 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 0 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_EQ( stream_available_read( stream ), 0 );
	EXPECT_TRUE( uint128_equal( stream_md5( stream ), md5zero ) );

	stream_deallocate( stream );

	return 0;
}


DECLARE_TEST( bufferstream, zero )
{
	stream_t* stream = 0;
	tick_t curtime = time_current();
	uint8_t readbuffer[1024] = {0};
	uint8_t writebuffer[1024] = {0};
	uint8_t backing_store[1024] = {0};
	uint128_t md5null;

	{
		md5_t* md5 = md5_allocate();
		md5_digest_finalize( md5 );
		md5null = md5_get_digest_raw( md5 );
		md5_deallocate( md5 );
	}

	stream = buffer_stream_allocate( backing_store, STREAM_IN | STREAM_OUT, 0, 0, false, false );
	EXPECT_NE( stream, 0 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 0 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_FALSE( stream_is_binary( stream ) );
	EXPECT_FALSE( stream_is_sequential( stream ) );
	EXPECT_TRUE( stream_is_reliable( stream ) );
	EXPECT_TRUE( stream_is_inorder( stream ) );
	EXPECT_TRUE( string_equal_substr( stream_path( stream ), "buffer://0x", 11 ) );
	EXPECT_GE( stream_last_modified( stream ), curtime );
	EXPECT_EQ( stream_available_read( stream ), 0 );
	EXPECT_TRUE( uint128_equal( stream_md5( stream ), md5null ) );

	//No buffer and not growing - all read/write ops should do nothing
	EXPECT_EQ( stream_read( stream, readbuffer, 1024 ), 0 );
	EXPECT_EQ( stream_write( stream, writebuffer, 1024 ), 0 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 0 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_EQ( stream_available_read( stream ), 0 );
	EXPECT_TRUE( uint128_equal( stream_md5( stream ), md5null ) );

	stream_truncate( stream, 1024 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 0 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_EQ( stream_available_read( stream ), 0 );
	EXPECT_TRUE( uint128_equal( stream_md5( stream ), md5null ) );

	EXPECT_EQ( stream_read( stream, readbuffer, 1024 ), 0 );
	EXPECT_EQ( stream_write( stream, writebuffer, 1024 ), 0 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 0 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_EQ( stream_available_read( stream ), 0 );
	EXPECT_TRUE( uint128_equal( stream_md5( stream ), md5null ) );

	stream_deallocate( stream );

	return 0;
}


DECLARE_TEST( bufferstream, null_grow )
{
	stream_t* stream = 0;
	tick_t curtime = time_current();
	char readbuffer[1024] = {0};
	char writebuffer[1024] = {0};
	uint128_t md5null;
	size_t slength;

	{
		md5_t* md5 = md5_allocate();
		md5_digest_finalize( md5 );
		md5null = md5_get_digest_raw( md5 );
		md5_deallocate( md5 );
	}

	stream = buffer_stream_allocate( 0, STREAM_IN | STREAM_OUT, 0, 0, true, true );
	EXPECT_NE( stream, 0 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 0 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_FALSE( stream_is_binary( stream ) );
	EXPECT_FALSE( stream_is_sequential( stream ) );
	EXPECT_TRUE( stream_is_reliable( stream ) );
	EXPECT_TRUE( stream_is_inorder( stream ) );
	EXPECT_TRUE( string_equal_substr( stream_path( stream ), "buffer://0x", 11 ) );
	EXPECT_GE( stream_last_modified( stream ), curtime );
	EXPECT_EQ( stream_available_read( stream ), 0 );
	EXPECT_TRUE( uint128_equal( stream_md5( stream ), md5null ) );

	string_copy( writebuffer, "MD5 test string for which the value is precomputed", 1024 );
	slength = string_length( writebuffer );
	EXPECT_EQ( stream_write( stream, writebuffer, slength ), slength );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), slength );
	EXPECT_EQ( stream_tell( stream ), slength );
	EXPECT_EQ( stream_available_read( stream ), 0 );

	stream_seek( stream, 0, STREAM_SEEK_BEGIN );
	EXPECT_EQ( stream_read( stream, readbuffer, 1024 ), slength );
	EXPECT_TRUE( string_equal( readbuffer, writebuffer ) );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), slength );
	EXPECT_EQ( stream_tell( stream ), slength );
	EXPECT_EQ( stream_available_read( stream ), 0 );

	stream_truncate( stream, 1024 );
	EXPECT_FALSE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 1024 );
	EXPECT_EQ( stream_tell( stream ), slength );
	EXPECT_EQ( stream_available_read( stream ), 1024 - slength );

	EXPECT_EQ( stream_read( stream, readbuffer, 1024 ), 1024 - slength );
	EXPECT_EQ( stream_write( stream, writebuffer, 1024 ), 1024 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 1024 * 2 );
	EXPECT_EQ( stream_tell( stream ), 1024 * 2 );
	EXPECT_EQ( stream_available_read( stream ), 0 );

	stream_deallocate( stream );

	return 0;
}


DECLARE_TEST( bufferstream, zero_grow )
{
	stream_t* stream = 0;
	tick_t curtime = time_current();
	char readbuffer[1024] = {0};
	char writebuffer[1024] = {0};
	uint8_t* backing_store = memory_allocate( 0, 315, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED );
	uint128_t md5null;
	size_t slength;

	{
		md5_t* md5 = md5_allocate();
		md5_digest_finalize( md5 );
		md5null = md5_get_digest_raw( md5 );
		md5_deallocate( md5 );
	}

	stream = buffer_stream_allocate( backing_store, STREAM_IN | STREAM_OUT, 0, 315, true, true );
	EXPECT_NE( stream, 0 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 0 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_FALSE( stream_is_binary( stream ) );
	EXPECT_FALSE( stream_is_sequential( stream ) );
	EXPECT_TRUE( stream_is_reliable( stream ) );
	EXPECT_TRUE( stream_is_inorder( stream ) );
	EXPECT_TRUE( string_equal_substr( stream_path( stream ), "buffer://0x", 11 ) );
	EXPECT_GE( stream_last_modified( stream ), curtime );
	EXPECT_EQ( stream_available_read( stream ), 0 );
	EXPECT_TRUE( uint128_equal( stream_md5( stream ), md5null ) );

	string_copy( writebuffer, "MD5 test string for which the value is precomputed", 1024 );
	slength = string_length( writebuffer );
	EXPECT_EQ( stream_write( stream, writebuffer, slength ), slength );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), slength );
	EXPECT_EQ( stream_tell( stream ), slength );
	EXPECT_EQ( stream_available_read( stream ), 0 );

	stream_seek( stream, 0, STREAM_SEEK_BEGIN );
	EXPECT_EQ( stream_read( stream, readbuffer, 1024 ), slength );
	EXPECT_TRUE( string_equal( readbuffer, writebuffer ) );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), slength );
	EXPECT_EQ( stream_tell( stream ), slength );
	EXPECT_EQ( stream_available_read( stream ), 0 );

	stream_truncate( stream, 1024 );
	EXPECT_FALSE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 1024 );
	EXPECT_EQ( stream_tell( stream ), slength );
	EXPECT_EQ( stream_available_read( stream ), 1024 - slength );

	EXPECT_EQ( stream_read( stream, readbuffer, 1024 ), 1024 - slength );
	EXPECT_EQ( stream_write( stream, writebuffer, 1024 ), 1024 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 1024 * 2 );
	EXPECT_EQ( stream_tell( stream ), 1024 * 2 );
	EXPECT_EQ( stream_available_read( stream ), 0 );

	stream_deallocate( stream );

	return 0;
}


DECLARE_TEST( bufferstream, zero_nogrow )
{
	stream_t* stream = 0;
	tick_t curtime = time_current();
	char readbuffer[1024] = {0};
	char writebuffer[1024] = {0};
	uint8_t* backing_store = memory_allocate( 0, 1024, 0, MEMORY_PERSISTENT );
	uint128_t md5null;
	size_t slength;

	{
		md5_t* md5 = md5_allocate();
		md5_digest_finalize( md5 );
		md5null = md5_get_digest_raw( md5 );
		md5_deallocate( md5 );
	}

	stream = buffer_stream_allocate( backing_store, STREAM_IN | STREAM_OUT, 0, 1024, true, false );
	EXPECT_NE( stream, 0 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 0 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_FALSE( stream_is_binary( stream ) );
	EXPECT_FALSE( stream_is_sequential( stream ) );
	EXPECT_TRUE( stream_is_reliable( stream ) );
	EXPECT_TRUE( stream_is_inorder( stream ) );
	EXPECT_TRUE( string_equal_substr( stream_path( stream ), "buffer://0x", 11 ) );
	EXPECT_GE( stream_last_modified( stream ), curtime );
	EXPECT_EQ( stream_available_read( stream ), 0 );
	EXPECT_TRUE( uint128_equal( stream_md5( stream ), md5null ) );

	string_copy( writebuffer, "MD5 test string for which the value is precomputed", 1024 );
	slength = string_length( writebuffer );
	EXPECT_EQ( stream_write( stream, writebuffer, slength ), slength );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), slength );
	EXPECT_EQ( stream_tell( stream ), slength );
	EXPECT_EQ( stream_available_read( stream ), 0 );

	stream_seek( stream, 0, STREAM_SEEK_BEGIN );
	EXPECT_EQ( stream_read( stream, readbuffer, 1024 ), slength );
	EXPECT_TRUE( string_equal( readbuffer, writebuffer ) );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), slength );
	EXPECT_EQ( stream_tell( stream ), slength );
	EXPECT_EQ( stream_available_read( stream ), 0 );

	stream_truncate( stream, 2048 );
	EXPECT_FALSE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 1024 );
	EXPECT_EQ( stream_tell( stream ), slength );
	EXPECT_EQ( stream_available_read( stream ), 1024 - slength );

	EXPECT_EQ( stream_read( stream, readbuffer, 1024 ), 1024 - slength );
	EXPECT_EQ( stream_write( stream, writebuffer, 1024 ), 0 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 1024 );
	EXPECT_EQ( stream_tell( stream ), 1024 );
	EXPECT_EQ( stream_available_read( stream ), 0 );

	stream_deallocate( stream );

	return 0;
}


DECLARE_TEST( bufferstream, sized_grow )
{
	stream_t* stream = 0;
	tick_t curtime = time_current();
	char readbuffer[1024] = {0};
	char writebuffer[1024] = {0};
	uint8_t* backing_store = memory_allocate( 0, 1024, 0, MEMORY_PERSISTENT );
	uint128_t md5null;
	size_t slength;

	{
		md5_t* md5 = md5_allocate();
		md5_digest_finalize( md5 );
		md5null = md5_get_digest_raw( md5 );
		md5_deallocate( md5 );
	}

	stream = buffer_stream_allocate( backing_store, STREAM_IN | STREAM_OUT, 315, 1024, true, true );
	EXPECT_NE( stream, 0 );
	EXPECT_FALSE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 315 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_FALSE( stream_is_binary( stream ) );
	EXPECT_FALSE( stream_is_sequential( stream ) );
	EXPECT_TRUE( stream_is_reliable( stream ) );
	EXPECT_TRUE( stream_is_inorder( stream ) );
	EXPECT_TRUE( string_equal_substr( stream_path( stream ), "buffer://0x", 11 ) );
	EXPECT_GE( stream_last_modified( stream ), curtime );
	EXPECT_EQ( stream_available_read( stream ), 315 );
	EXPECT_FALSE( uint128_equal( stream_md5( stream ), md5null ) );

	string_copy( writebuffer, "MD5 test string for which the value is precomputed", 1024 );
	slength = string_length( writebuffer );
	EXPECT_EQ( stream_write( stream, writebuffer, slength ), slength );
	EXPECT_FALSE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 315 );
	EXPECT_EQ( stream_tell( stream ), slength );
	EXPECT_EQ( stream_available_read( stream ), 315 - slength );

	stream_seek( stream, 0, STREAM_SEEK_BEGIN );
	EXPECT_EQ( stream_read( stream, readbuffer, 1024 ), 315 );
	EXPECT_TRUE( string_equal_substr( readbuffer, writebuffer, slength ) );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 315 );
	EXPECT_EQ( stream_tell( stream ), 315 );
	EXPECT_EQ( stream_available_read( stream ), 0 );

	stream_truncate( stream, 2048 );
	EXPECT_FALSE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 2048 );
	EXPECT_EQ( stream_tell( stream ), 315 );
	EXPECT_EQ( stream_available_read( stream ), 2048 - 315 );

	EXPECT_EQ( stream_read( stream, readbuffer, 1024 ), 1024 );
	EXPECT_EQ( stream_write( stream, writebuffer, 1024 ), 1024 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 2048 + 315 );
	EXPECT_EQ( stream_tell( stream ), 2048 + 315 );
	EXPECT_EQ( stream_available_read( stream ), 0 );

	stream_deallocate( stream );

	return 0;
}


DECLARE_TEST( bufferstream, sized_nogrow )
{
	stream_t* stream = 0;
	tick_t curtime = time_current();
	char readbuffer[1024] = {0};
	char writebuffer[1024] = {0};
	uint8_t* backing_store = memory_allocate( 0, 1024, 0, MEMORY_PERSISTENT );
	uint128_t md5null;
	size_t slength;

	{
		md5_t* md5 = md5_allocate();
		md5_digest_finalize( md5 );
		md5null = md5_get_digest_raw( md5 );
		md5_deallocate( md5 );
	}

	stream = buffer_stream_allocate( backing_store, STREAM_IN | STREAM_OUT, 315, 1024, false, false );
	EXPECT_NE( stream, 0 );
	EXPECT_FALSE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 315 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_FALSE( stream_is_binary( stream ) );
	EXPECT_FALSE( stream_is_sequential( stream ) );
	EXPECT_TRUE( stream_is_reliable( stream ) );
	EXPECT_TRUE( stream_is_inorder( stream ) );
	EXPECT_TRUE( string_equal_substr( stream_path( stream ), "buffer://0x", 11 ) );
	EXPECT_GE( stream_last_modified( stream ), curtime );
	EXPECT_EQ( stream_available_read( stream ), 315 );
	EXPECT_FALSE( uint128_equal( stream_md5( stream ), md5null ) );

	string_copy( writebuffer, "MD5 test string for which the value is precomputed", 1024 );
	slength = string_length( writebuffer );
	EXPECT_EQ( stream_write( stream, writebuffer, slength ), slength );
	EXPECT_FALSE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 315 );
	EXPECT_EQ( stream_tell( stream ), slength );
	EXPECT_EQ( stream_available_read( stream ), 315 - slength );

	stream_seek( stream, 0, STREAM_SEEK_BEGIN );
	EXPECT_EQ( stream_read( stream, readbuffer, 1024 ), 315 );
	EXPECT_TRUE( string_equal_substr( readbuffer, writebuffer, slength ) );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 315 );
	EXPECT_EQ( stream_tell( stream ), 315 );
	EXPECT_EQ( stream_available_read( stream ), 0 );

	stream_truncate( stream, 2048 );
	EXPECT_FALSE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 1024 );
	EXPECT_EQ( stream_tell( stream ), 315 );
	EXPECT_EQ( stream_available_read( stream ), 1024 - 315 );

	EXPECT_EQ( stream_read( stream, readbuffer, 1024 ), 1024 - 315 );
	EXPECT_EQ( stream_write( stream, writebuffer, 1024 ), 0 );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 1024 );
	EXPECT_EQ( stream_tell( stream ), 1024 );
	EXPECT_EQ( stream_available_read( stream ), 0 );

	stream_deallocate( stream );
	memory_deallocate( backing_store );

	return 0;
}


static void test_bufferstream_declare( void )
{
	ADD_TEST( bufferstream, null );
	ADD_TEST( bufferstream, zero );
	ADD_TEST( bufferstream, null_grow );
	ADD_TEST( bufferstream, zero_grow );
	ADD_TEST( bufferstream, zero_nogrow );
	ADD_TEST( bufferstream, sized_grow );
	ADD_TEST( bufferstream, sized_nogrow );
}


static test_suite_t test_bufferstream_suite = {
	test_bufferstream_application,
	test_bufferstream_memory_system,
	test_bufferstream_declare,
	test_bufferstream_initialize,
	test_bufferstream_shutdown
};


#if BUILD_MONOLITHIC

int test_bufferstream_run( void );
int test_bufferstream_run( void )
{
	test_suite = test_bufferstream_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void );
test_suite_t test_suite_define( void )
{
	return test_bufferstream_suite;
}

#endif
