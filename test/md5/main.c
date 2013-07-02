/* main.c  -  Foundation md5 test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
	app.name = "Foundation md5 tests";
	app.short_name = "test_md5";
	app.config_dir = "test_md5";
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


DECLARE_TEST( md5, empty )
{
	md5_t* md5;
	char* md5str;
	
	md5 = md5_allocate();
	md5_finalize( md5 );
	md5str = md5_get_digest( md5 );

	EXPECT_STREQ( md5str, "D41D8CD98F00B204E9800998ECF8427E" );
	string_deallocate( md5str );

	md5_initialize( md5 );
	md5_finalize( md5 );
	md5str = md5_get_digest( md5 );

	EXPECT_STREQ( md5str, "D41D8CD98F00B204E9800998ECF8427E" );
	string_deallocate( md5str );

	md5_deallocate( md5 );
	
	return 0;
}


const char* digest_test_string =
"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";


DECLARE_TEST( md5, reference )
{
	md5_t* md5;
	char* md5str;

	md5 = md5_allocate();
	md5_finalize( md5 );

	md5_digest_raw( md5, "testing md5 implementation", 26 );
	md5_finalize( md5 );

	md5str = md5_get_digest( md5 );

	EXPECT_STREQ( md5str, "4E24E37E5E06F23210FA1518E97A50C4" );
	string_deallocate( md5str );

	md5_digest_raw( md5, "testing md5 implementation", 26 );
	md5_digest_raw( md5, "", 0 );
	md5_digest_raw( md5, "further testing md5 implementation with long buffer > 32 bytes", 62 );
	md5_finalize( md5 );
	md5str = md5_get_digest( md5 );

	EXPECT_STREQ( md5str, "BD870884942EA7B32A9CB2547B02B871" );
	string_deallocate( md5str );

	md5_digest_raw( md5, digest_test_string, 2000 );
	md5_finalize( md5 );
	md5str = md5_get_digest( md5 );

	EXPECT_STREQ( md5str, "137D3C94230A0E230C4DDFC97EACCCD2" );
	string_deallocate( md5str );

	return 0;
}


DECLARE_TEST( md5, streams )
{
	stream_t* test_stream;
	stream_t* unix_stream;
	stream_t* windows_stream;
	uint128_t digest, unix_digest, windows_digest;

	char unix_buffer[] =
	"tvqy0C9TO2MI7uyUqr\n\n"
	"QVcrmjDKDEcB3e7dpM\n\n"
	"bMpRv6uH0LWPjvNcNp\n"
	"jpkje9KYXhJjSA2TBy\n"
	"bTqnOPhOOZ5aWnUdgO\n"
	"fLapbWSZInasn1SJlk\n"
	"ytNFKkASDln05zw39X\n"
	"L8McmojqPmn41Y6CRN\n\n"
	"wZbKo0PZofDpbAMr1u\n"
	"u8GJIocbEBFzsyKo62\n"
	"FlQh1pjm9jBeoEJIHL\n"
	"v4ixQn77l4M7zbRHgJ\n"
	"DIVy0vvpNEzxFRyD3Z\n"
	"5OrJvr\n";

    char windows_buffer[] =
	"tvqy0C9TO2MI7uyUqr\r\n\r\n"
	"QVcrmjDKDEcB3e7dpM\r\n\r\n"
	"bMpRv6uH0LWPjvNcNp\r\n"
	"jpkje9KYXhJjSA2TBy\r\n"
	"bTqnOPhOOZ5aWnUdgO\r\n"
	"fLapbWSZInasn1SJlk\r\n"
	"ytNFKkASDln05zw39X\r\n"
	"L8McmojqPmn41Y6CRN\r\n\r\n"
	"wZbKo0PZofDpbAMr1u\r\n"
	"u8GJIocbEBFzsyKo62\r\n"
	"FlQh1pjm9jBeoEJIHL\r\n"
	"v4ixQn77l4M7zbRHgJ\r\n"
	"DIVy0vvpNEzxFRyD3Z\r\n"
	"5OrJvr\r\n";
		
	test_stream = buffer_stream_allocate( 0, STREAM_IN | STREAM_OUT | STREAM_BINARY, 0, 0, true, true );
	stream_write( test_stream, digest_test_string, 2000 );

	stream_seek( test_stream, 0, STREAM_SEEK_BEGIN );
	digest = stream_md5( test_stream );
	stream_deallocate( test_stream );

	EXPECT_TRUE( uint128_equal( digest, uint128_make( 0x230e0a23943c7d13ULL, 0xd2ccac7ec9df4d0cULL ) ) );
	
	unix_stream = buffer_stream_allocate( (void*)unix_buffer, STREAM_IN, string_length( unix_buffer ), string_length( unix_buffer ), false, false );
	windows_stream = buffer_stream_allocate( (void*)windows_buffer, STREAM_IN, string_length( windows_buffer ), string_length( windows_buffer ), false, false );
	
	stream_set_binary( unix_stream, false );
	stream_set_binary( windows_stream, false );

	unix_digest = stream_md5( unix_stream );
	windows_digest = stream_md5( windows_stream );

	EXPECT_TRUE( uint128_equal( unix_digest, uint128_make( 0xcf32b789c7e77197ULL, 0x2bff28c36c601093ULL ) ) );
	EXPECT_TRUE( uint128_equal( unix_digest, windows_digest ) );

	stream_set_binary( unix_stream, true );
	stream_set_binary( windows_stream, true );

	stream_seek( unix_stream, 0, STREAM_SEEK_BEGIN );
	stream_seek( windows_stream, 0, STREAM_SEEK_BEGIN );

	unix_digest = stream_md5( unix_stream );
	windows_digest = stream_md5( windows_stream );

	EXPECT_TRUE( uint128_equal( unix_digest, uint128_make( 0xcf32b789c7e77197ULL, 0x2bff28c36c601093ULL ) ) );
	EXPECT_TRUE( uint128_equal( windows_digest, uint128_make( 0xf77d63bbe1df9334ULL, 0x24d5cb05cd503e44ULL ) ) );
	EXPECT_TRUE( !uint128_equal( unix_digest, windows_digest ) );

	stream_deallocate( unix_stream );
	stream_deallocate( windows_stream );

	return 0;
}	


void test_declare( void )
{
	ADD_TEST( md5, empty );
	ADD_TEST( md5, reference );
	ADD_TEST( md5, streams );
}
