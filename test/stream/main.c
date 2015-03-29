/* main.c  -  Foundation stream test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#if FOUNDATION_PLATFORM_WINDOWS
#include <foundation/windows.h>
#endif

#if FOUNDATION_PLATFORM_POSIX
#include <foundation/posix.h>
#endif


static application_t test_stream_application( void )
{
	application_t app;
	memset( &app, 0, sizeof( app ) );
	app.name = "Foundation stream tests";
	app.short_name = "test_stream";
	app.config_dir = "test_stream";
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_stream_memory_system( void )
{
	return memory_system_malloc();
}


static int test_stream_initialize( void )
{
	return 0;
}


static void test_stream_shutdown( void )
{
}


DECLARE_TEST( stream, std )
{
	tick_t now;
	stream_t* in;
	stream_t* out;
	stream_t* err;
	stream_t* in_clone;
	stream_t* out_clone;
	stream_t* err_clone;

	in = stream_open( "stdin://", STREAM_OUT | STREAM_IN | STREAM_BINARY );
	out = stream_open( "stdout://", STREAM_OUT | STREAM_IN );
	err = stream_open( "stderr://", STREAM_OUT | STREAM_IN | STREAM_BINARY );

	EXPECT_NE_MSG( in, 0, "Failed to open stdin://" );
	EXPECT_NE_MSG( out, 0, "Failed to open stdout://" );
	EXPECT_NE_MSG( err, 0, "Failed to open stderr://" );

	EXPECT_NE_MSG( in, 0, "Failed to open stdin://" );
	EXPECT_NE_MSG( out, 0, "Failed to open stdout://" );
	EXPECT_NE_MSG( err, 0, "Failed to open stderr://" );

	EXPECT_EQ_MSG( stream_tell( in ), 0, "stdin pos is not 0");
	EXPECT_EQ_MSG( stream_tell( out ), 0, "stdout pos is not 0");
	EXPECT_EQ_MSG( stream_tell( err ), 0, "stderr pos is not 0");

	stream_seek( in, 1, STREAM_SEEK_BEGIN );
	stream_seek( out, 1, STREAM_SEEK_CURRENT );
	stream_seek( err, 1, STREAM_SEEK_END );

	EXPECT_EQ_MSG( stream_tell( in ), 0, "stdin pos is not 0");
	EXPECT_EQ_MSG( stream_tell( out ), 0, "stdout pos is not 0");
	EXPECT_EQ_MSG( stream_tell( err ), 0, "stderr pos is not 0");

	EXPECT_EQ_MSG( stream_eos( in ), false, "stdin at eos" );
	EXPECT_EQ_MSG( stream_eos( out ), false, "stdout at eos" );
	EXPECT_EQ_MSG( stream_eos( err ), false, "stderr at eos" );

	EXPECT_EQ_MSG( stream_size( in ), 0, "stdin size is not 0");
	EXPECT_EQ_MSG( stream_size( out ), 0, "stdout size is not 0");
	EXPECT_EQ_MSG( stream_size( err ), 0, "stderr size is not 0");

	EXPECT_EQ_MSG( stream_is_binary( in ), true, "stdin did not preserve binary mode" );
	EXPECT_EQ_MSG( stream_is_binary( out ), false, "stdout did not preserve ascii mode" );
	EXPECT_EQ_MSG( stream_is_binary( err ), true, "stderr did not preserve binary mode" );

	EXPECT_EQ_MSG( stream_is_sequential( in ), true, "stdin not sequential" );
	EXPECT_EQ_MSG( stream_is_sequential( out ), true, "stdout not sequential" );
	EXPECT_EQ_MSG( stream_is_sequential( err ), true, "stderr not sequential" );

	EXPECT_EQ_MSG( stream_is_reliable( in ), true, "stdin not reliable" );
	EXPECT_EQ_MSG( stream_is_reliable( out ), true, "stdout not reliable" );
	EXPECT_EQ_MSG( stream_is_reliable( err ), true, "stderr not reliable" );

	EXPECT_EQ_MSG( stream_is_inorder( in ), true, "stdin not inorder" );
	EXPECT_EQ_MSG( stream_is_inorder( out ), true, "stdout not inorder" );
	EXPECT_EQ_MSG( stream_is_inorder( err ), true, "stderr not inorder" );

	EXPECT_EQ_MSG( stream_is_swapped( in ), false, "stdin is swapped" );
	EXPECT_EQ_MSG( stream_is_swapped( out ), false, "stdout is swapped" );
	EXPECT_EQ_MSG( stream_is_swapped( err ), false, "stderr is swapped" );

	EXPECT_EQ_MSG( stream_byteorder( in ), system_byteorder(), "stdin not native byteorder" );
	EXPECT_EQ_MSG( stream_byteorder( out ), system_byteorder(), "stdout not native byteorder" );
	EXPECT_EQ_MSG( stream_byteorder( err ), system_byteorder(), "stderr not native byteorder" );

	stream_set_byteorder( in, BYTEORDER_LITTLEENDIAN );
	stream_set_byteorder( out, BYTEORDER_LITTLEENDIAN );
	stream_set_byteorder( err, BYTEORDER_LITTLEENDIAN );

	EXPECT_EQ_MSG( stream_byteorder( in ), BYTEORDER_LITTLEENDIAN, "stdin not little endian after explicit set" );
	EXPECT_EQ_MSG( stream_byteorder( out ), BYTEORDER_LITTLEENDIAN, "stdout not little endian after explicit set" );
	EXPECT_EQ_MSG( stream_byteorder( err ), BYTEORDER_LITTLEENDIAN, "stderr not little endian after explicit set" );

	stream_set_byteorder( in, BYTEORDER_BIGENDIAN );
	stream_set_byteorder( out, BYTEORDER_BIGENDIAN );
	stream_set_byteorder( err, BYTEORDER_BIGENDIAN );

	EXPECT_EQ_MSG( stream_byteorder( in ), BYTEORDER_BIGENDIAN, "stdin not big endian after explicit set" );
	EXPECT_EQ_MSG( stream_byteorder( out ), BYTEORDER_BIGENDIAN, "stdout not big endian after explicit set" );
	EXPECT_EQ_MSG( stream_byteorder( err ), BYTEORDER_BIGENDIAN, "stderr not big endian after explicit set" );

	stream_set_binary( in, true );
	stream_set_binary( out, true );
	stream_set_binary( err, true );

	EXPECT_EQ_MSG( stream_is_binary( in ), true, "stdin did not set binary mode" );
	EXPECT_EQ_MSG( stream_is_binary( out ), true, "stdout did not set binary mode" );
	EXPECT_EQ_MSG( stream_is_binary( err ), true, "stderr did not set binary mode" );

	stream_set_binary( in, false );
	stream_set_binary( out, false );
	stream_set_binary( err, false );

	EXPECT_EQ_MSG( stream_is_binary( in ), false, "stdin did not set ascii mode" );
	EXPECT_EQ_MSG( stream_is_binary( out ), false, "stdout did not set asii mode" );
	EXPECT_EQ_MSG( stream_is_binary( err ), false, "stderr did not set ascii mode" );

	EXPECT_STREQ_MSG( stream_path( in ), "stdin://", "stdin has wrong path" );
	EXPECT_STREQ_MSG( stream_path( out ), "stdout://", "stdout has wrong path" );
	EXPECT_STREQ_MSG( stream_path( err ), "stderr://", "stderr has wrong path" );

	now = time_system();

	EXPECT_GE_MSG( stream_last_modified( in ), now, "stdin has noncurrent last modification" );
	EXPECT_GE_MSG( stream_last_modified( out ), now, "stdout has noncurrent last modification" );
	EXPECT_GE_MSG( stream_last_modified( err ), now, "stderr has noncurrent last modification" );

	stream_set_binary( in, true );
	stream_set_byteorder( out, BYTEORDER_LITTLEENDIAN );

	in_clone = stream_clone( in );
	out_clone = stream_clone( out );
	err_clone = stream_clone( err );

	EXPECT_NE_MSG( in_clone, 0, "Clone stdin stream returned null" );
	EXPECT_NE_MSG( out_clone, 0, "Clone stdout stream returned null" );
	EXPECT_NE_MSG( err_clone, 0, "Clone stderr stream returned null" );

	EXPECT_STREQ_MSG( stream_path( in_clone ), "stdin://", "stdin clone has wrong path" );
	EXPECT_STREQ_MSG( stream_path( out_clone ), "stdout://", "stdout clone has wrong path" );
	EXPECT_STREQ_MSG( stream_path( err_clone ), "stderr://", "stderr clone has wrong path" );

	EXPECT_EQ_MSG( stream_is_binary( in_clone ), stream_is_binary( in ), "stdin did not preserve mode" );
	EXPECT_EQ_MSG( stream_is_binary( out_clone ), stream_is_binary( out ), "stdout clone did not preserve mode" );
	EXPECT_EQ_MSG( stream_is_binary( err_clone ), stream_is_binary( err ), "stderr did not preserve mode" );

	EXPECT_EQ_MSG( stream_byteorder( in_clone ), stream_byteorder( in ), "stdin did not preserve byte order" );
	EXPECT_EQ_MSG( stream_byteorder( out_clone ), stream_byteorder( out ), "stdout clone did not preserve byte order" );
	EXPECT_EQ_MSG( stream_byteorder( err_clone ), stream_byteorder( err ), "stderr did not preserve byte order" );

	stream_seek( in_clone, 1, STREAM_SEEK_BEGIN );
	stream_seek( out_clone, 1, STREAM_SEEK_CURRENT );
	stream_seek( err_clone, 1, STREAM_SEEK_END );

	EXPECT_EQ_MSG( stream_tell( in_clone ), 0, "stdin clone pos is not 0");
	EXPECT_EQ_MSG( stream_tell( out_clone ), 0, "stdout clone pos is not 0");
	EXPECT_EQ_MSG( stream_tell( err_clone ), 0, "stderr clone pos is not 0");

	EXPECT_EQ_MSG( stream_eos( in_clone ), false, "stdin clone at eos" );
	EXPECT_EQ_MSG( stream_eos( out_clone ), false, "stdout clone at eos" );
	EXPECT_EQ_MSG( stream_eos( err_clone ), false, "stderr clone at eos" );

	EXPECT_EQ_MSG( stream_size( in_clone ), 0, "stdin clone size is not 0");
	EXPECT_EQ_MSG( stream_size( out_clone ), 0, "stdout clone size is not 0");
	EXPECT_EQ_MSG( stream_size( err_clone ), 0, "stderr clone size is not 0");

	stream_deallocate( in_clone );
	stream_deallocate( out_clone );
	stream_deallocate( err_clone );

	stream_buffer_read( in );
	stream_buffer_read( out );
	stream_buffer_read( err );

	EXPECT_GE_MSG( stream_available_read( in ), 0, "stdin invalid value for available bytes to read" );
	EXPECT_EQ_MSG( stream_available_read( out ), 0, "stdout not empty as expected" );
	EXPECT_EQ_MSG( stream_available_read( err ), 0, "stderr not empty as expected" );

	EXPECT_EQ_MSG( uint128_is_null( stream_md5( in ) ), true, "stdin md5 not null" );
	EXPECT_EQ_MSG( uint128_is_null( stream_md5( out ) ), true, "stdout md5 not null" );
	EXPECT_EQ_MSG( uint128_is_null( stream_md5( err ) ), true, "stderr md5 not null" );

	stream_truncate( in, 256 );
	stream_truncate( out, 256 );
	stream_truncate( err, 256 );

	stream_flush( in );
	stream_flush( out );
	stream_flush( err );

	EXPECT_GE_MSG( stream_available_read( in ), 0, "stdin invalid value for available bytes to read" );
	EXPECT_EQ_MSG( stream_available_read( out ), 0, "stdout not empty as expected" );
	EXPECT_EQ_MSG( stream_available_read( err ), 0, "stderr not empty as expected" );

	//Verify that eos is set when stdin is closed
	close( STDIN_FILENO );
	EXPECT_EQ_MSG( stream_read_int8( in ), 0, "stdin read when closed did not return 0" );
	EXPECT_EQ_MSG( stream_eos( in ), true, "stdin not at eos when closed" );

	stream_deallocate( in );
	stream_deallocate( out );
	stream_deallocate( err );

	return 0;
}


DECLARE_TEST( stream, readwrite_binary )
{
	char write_buffer[1024];
	char read_buffer[1024];
	stream_t* teststream;
	char* path;
	char* directory;
	char* line;
	int i;
	uint64_t was_read;

	path = path_make_temporary();
	directory = path_directory_name( path );
	fs_make_directory( directory );
	teststream = stream_open( path, STREAM_IN | STREAM_OUT | STREAM_BINARY | STREAM_CREATE );

	EXPECT_NE_MSGFORMAT( teststream, 0, "test stream '%s' not created", path );

	for( i = 0; i < 1024; ++i )
		write_buffer[i] = (char)( i + 63 );

	EXPECT_EQ_MSG( stream_write( teststream, write_buffer, 0 ), 0, "write zero bytes failed" );
	EXPECT_EQ_MSG( stream_write( teststream, write_buffer, 1 ), 1, "write one byte failed" );
	EXPECT_EQ_MSG( stream_write( teststream, write_buffer, 1024 ), 1024, "write 1024 bytes failed" );

	stream_write_bool( teststream, true );
	stream_write_int8( teststream, 127 );
	stream_write_uint8( teststream, 178 );
	stream_write_int16( teststream, -1234 );
	stream_write_uint16( teststream, 45678 );
	stream_write_int32( teststream, -1324567 );
	stream_write_uint32( teststream, 3245678U );
	stream_write_int64( teststream, 123456789012LL );
	stream_write_uint64( teststream, 8712634987126ULL );
	stream_write_float32( teststream, 1.0f );
	stream_write_float64( teststream, -1.0 );
	stream_write_string( teststream, "test string\nwith some newlines\nin the string" );
	stream_write_endl( teststream );
	stream_write_format( teststream, "formatted output with a null pointer 0x%" PRIfixPTR, (void*)0 );

	EXPECT_EQ_MSGFORMAT( stream_tell( teststream ), 1025 + 43 + 45 + 40 + FOUNDATION_SIZE_POINTER*2, "stream position not expected after writes (%lld)", stream_tell( teststream ) );
	stream_seek( teststream, 0, STREAM_SEEK_BEGIN );
	EXPECT_EQ_MSG( stream_tell( teststream ), 0, "stream position not null after seek" );

	read_buffer[0] = 123;
	EXPECT_EQ_MSG( stream_read( teststream, read_buffer, 0 ), 0, "read zero bytes failed" );
	EXPECT_EQ_MSG( read_buffer[0], 123, "buffer modified when reading zero bytes" );
	EXPECT_EQ_MSG( stream_read( teststream, read_buffer, 1 ), 1, "read one byte failed" );
	EXPECT_EQ_MSG( read_buffer[0], write_buffer[0], "data not read correctly when reading one byte" );
	EXPECT_EQ_MSG( stream_read( teststream, read_buffer, 1024 ), 1024, "read 1024 bytes failed" );
	for( i = 0; i < 1024; ++i )
		EXPECT_EQ_MSGFORMAT( read_buffer[i], write_buffer[i], "data not read correctly in pos %d when reading 1024 bytes", i );

	EXPECT_EQ_MSG( stream_read_bool( teststream ), true, "read boolean failed" );
	EXPECT_EQ_MSG( stream_read_int8( teststream ), 127, "read int8 failed" );
	EXPECT_EQ_MSG( stream_read_uint8( teststream ), 178, "read uint8 failed" );
	EXPECT_EQ_MSG( stream_read_int16( teststream ), -1234, "read int16 failed" );
	EXPECT_EQ_MSG( stream_read_uint16( teststream ), 45678, "read uint16 failed" );
	EXPECT_EQ_MSG( stream_read_int32( teststream ), -1324567, "read int32 failed" );
	EXPECT_EQ_MSG( stream_read_uint32( teststream ), 3245678U, "read uint32 failed" );
	EXPECT_EQ_MSG( stream_read_int64( teststream ), 123456789012LL, "read int64 failed" );
	EXPECT_EQ_MSG( stream_read_uint64( teststream ), 8712634987126ULL, "read uint64 failed" );
	EXPECT_EQ_MSG( stream_read_float32( teststream ), 1.0f, "read float32 failed" );
	EXPECT_EQ_MSG( stream_read_float64( teststream ), -1.0, "read float64 failed" );

	EXPECT_EQ_MSG( stream_read_line_buffer( teststream, read_buffer, 1024, '\n' ), 11, "read line buffer failed" );
	EXPECT_STREQ_MSG( read_buffer, "test string", "read line buffer failed data" );

	line = stream_read_line( teststream, '\n' );
	EXPECT_NE_MSG( line, 0, "read line failed" );
	EXPECT_STREQ_MSG( line, "with some newlines", "read line failed data" );
	string_deallocate( line );

	line = stream_read_string( teststream );
	EXPECT_NE_MSG( line, 0, "read string failed" );
	EXPECT_STREQ_MSG( line, "in the string", "read string failed data" );
	string_deallocate( line );

	read_buffer[0] = 0;
	was_read = stream_read_string_buffer( teststream, read_buffer, 1024 );
	EXPECT_EQ_MSGFORMAT( was_read, 39 + FOUNDATION_SIZE_POINTER*2, "read string buffer failed (%lld)", was_read );
#if FOUNDATION_SIZE_POINTER == 8
	EXPECT_STREQ_MSG( read_buffer, "formatted output with a null pointer 0x0000000000000000", "read string buffer data failed" );
#else
	EXPECT_STREQ_MSG( read_buffer, "formatted output with a null pointer 0x00000000", "read string buffer data failed" );
#endif

	read_buffer[0] = 0;
	EXPECT_EQ_MSGFORMAT( stream_read_line_buffer( teststream, read_buffer, 1024, '\n' ), 0, "read line buffer failed at end of stream, read %s", read_buffer );

	stream_deallocate( teststream );
	fs_remove_file( path );

	teststream = stream_open( path, STREAM_IN | STREAM_BINARY | STREAM_CREATE );

	EXPECT_NE_MSGFORMAT( teststream, 0, "test stream '%s' not created", path );

	for( i = 0; i < 1024; ++i )
		write_buffer[i] = (char)( i + 63 );

	EXPECT_EQ_MSG( stream_write( teststream, write_buffer, 0 ), 0, "write zero bytes failed" );
	EXPECT_EQ_MSG( stream_write( teststream, write_buffer, 1 ), 0, "write one bytes failed (wrote in read-only stream)" );
	EXPECT_EQ_MSG( stream_write( teststream, write_buffer, 1024 ), 0, "write 1024 bytes failed (wrote in read-only stream)" );

	stream_write_bool( teststream, true );
	stream_write_int8( teststream, 127 );
	stream_write_uint8( teststream, 178 );
	stream_write_int16( teststream, -1234 );
	stream_write_uint16( teststream, 45678 );
	stream_write_int32( teststream, -1324567 );
	stream_write_uint32( teststream, 3245678U );
	stream_write_int64( teststream, 123456789012LL );
	stream_write_uint64( teststream, 8712634987126ULL );
	stream_write_float32( teststream, 1.0f );
	stream_write_float64( teststream, -1.0 );
	stream_write_string( teststream, "test string\nwith some newlines\nin the string" );
	stream_write_endl( teststream );
	stream_write_format( teststream, "formatted output with a null pointer 0x%" PRIfixPTR, (void*)0 );

	EXPECT_EQ_MSG( stream_tell( teststream ), 0, "stream position in read-only stream not null after writes" );
	stream_seek( teststream, 0, STREAM_SEEK_BEGIN );
	EXPECT_EQ_MSG( stream_tell( teststream ), 0, "stream position in read-only stream not null after seek" );

	for( i = 0; i < 1024; ++i )
		read_buffer[i] = (char)( i + 79 );
	EXPECT_EQ_MSG( stream_read( teststream, read_buffer, 0 ), 0, "read zero bytes failed" );
	EXPECT_EQ_MSGFORMAT( read_buffer[0], 79, "buffer modified when reading zero bytes (%d)", (int)read_buffer[0] );
	EXPECT_EQ_MSG( stream_read( teststream, read_buffer, 1 ), 0, "read one byte from read-only stream did not fail as expected" );
	EXPECT_EQ_MSG( read_buffer[0], 79, "buffer modified when reading from zero size read-only stream" );
	EXPECT_EQ_MSG( stream_read( teststream, read_buffer, 1024 ), 0, "read 1024 bytes from read-only stream did not fail as expected" );
	for( i = 0; i < 1024; ++i )
		EXPECT_EQ_MSG( read_buffer[i], (char)( i + 79 ), "buffer modified when reading from zero size read-only stream" );

	EXPECT_EQ_MSG( stream_read_bool( teststream ), false, "read boolean did not fail as expected" );
	EXPECT_EQ_MSG( stream_read_int8( teststream ), 0, "read int8 did not fail as expected" );
	EXPECT_EQ_MSG( stream_read_uint8( teststream ), 0, "read uint8 did not fail as expected" );
	EXPECT_EQ_MSG( stream_read_int16( teststream ), 0, "read int16 did not fail as expected" );
	EXPECT_EQ_MSG( stream_read_uint16( teststream ), 0, "read uint16 did not fail as expected" );
	EXPECT_EQ_MSG( stream_read_int32( teststream ), 0, "read int32 did not fail as expected" );
	EXPECT_EQ_MSG( stream_read_uint32( teststream ), 0, "read uint32 did not fail as expected" );
	EXPECT_EQ_MSG( stream_read_int64( teststream ), 0, "read int64 did not fail as expected" );
	EXPECT_EQ_MSG( stream_read_uint64( teststream ), 0, "read uint64 did not fail as expected" );
	EXPECT_EQ_MSG( stream_read_float32( teststream ), 0, "read float32 did not fail as expected" );
	EXPECT_EQ_MSG( stream_read_float64( teststream ), 0, "read float64 did not fail as expected" );

	read_buffer[0] = 0;
	EXPECT_EQ_MSG( stream_read_line_buffer( teststream, read_buffer, 1024, '\n' ), 0, "read line buffer did not fail as expected" );
	EXPECT_STREQ_MSG( read_buffer, "", "read line buffer failed data" );

	line = stream_read_line( teststream, '\n' );
	EXPECT_EQ_MSG( line, 0, "read line did not fail as expected" );

	line = stream_read_string( teststream );
	EXPECT_EQ_MSG( line, 0, "read string did not fail as expected" );

	read_buffer[0] = 0;
	was_read = stream_read_string_buffer( teststream, read_buffer, 1024 );
	EXPECT_EQ_MSGFORMAT( was_read, 0, "read string buffer failed (%lld)", was_read );
	EXPECT_STREQ_MSG( read_buffer, "", "read string buffer data failed" );

	stream_deallocate( teststream );
	fs_remove_file( path );

	string_deallocate( path );
	string_deallocate( directory );

	return 0;
}


static void test_stream_declare( void )
{
	ADD_TEST( stream, std );
	ADD_TEST( stream, readwrite_binary );
}


test_suite_t test_stream_suite = {
	test_stream_application,
	test_stream_memory_system,
	test_stream_declare,
	test_stream_initialize,
	test_stream_shutdown
};


#if BUILD_MONOLITHIC

int test_stream_run( void );
int test_stream_run( void )
{
	test_suite = test_stream_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void );
test_suite_t test_suite_define( void )
{
	return test_stream_suite;
}

#endif
