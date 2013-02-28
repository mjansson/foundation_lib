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


application_t test_application( void )
{
	application_t app = {0};
	app.name = "Foundation bufferstream tests";
	app.short_name = "test_bufferstream";
	app.config_dir = "test_bufferstream";
	return app;
}


DECLARE_TEST( bufferstream, null )
{
	stream_t* stream = 0;
	tick_t curtime = time_current();
	uint8_t readbuffer[1024] = {0};
	uint8_t writebuffer[1024] = {0};
	uint128_t md5null;
	uint128_t md5zero;

	{
		md5_t* md5 = md5_allocate();
		md5_initialize( md5 );
		md5_finalize( md5 );
		md5null = md5_get_digest_raw( md5 );
		md5_deallocate( md5 );

		md5zero = uint128_null();
	}

	stream = buffer_stream_allocate( 0, 0, 0, 0, false, false );
	EXPECT_NE( stream, 0 );
	EXPECT_TRUE( stream_is_open( stream ) );
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
	EXPECT_TRUE( stream_is_open( stream ) );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 0 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_EQ( stream_available_read( stream ), 0 );
	EXPECT_TRUE( uint128_equal( stream_md5( stream ), md5zero ) );

	stream_truncate( stream, 1024 );
	EXPECT_TRUE( stream_is_open( stream ) );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 0 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_EQ( stream_available_read( stream ), 0 );
	EXPECT_TRUE( uint128_equal( stream_md5( stream ), md5zero ) );

	EXPECT_EQ( stream_read( stream, readbuffer, 1024 ), 0 );
	EXPECT_EQ( stream_write( stream, writebuffer, 1024 ), 0 );
	EXPECT_TRUE( stream_is_open( stream ) );
	EXPECT_TRUE( stream_eos( stream ) );
	EXPECT_EQ( stream_size( stream ), 0 );
	EXPECT_EQ( stream_tell( stream ), 0 );
	EXPECT_EQ( stream_available_read( stream ), 0 );
	EXPECT_TRUE( uint128_equal( stream_md5( stream ), md5zero ) );

	stream_deallocate( stream );

	return 0;
}


void test_declare( void )
{
	ADD_TEST( bufferstream, null );
}
