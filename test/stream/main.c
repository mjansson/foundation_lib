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
	EXPECT_EQ_MSG( stream_clone( 0 ), 0, "Clone null stream returned non-null" );

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

	stream_deallocate( in );
	stream_deallocate( out );
	stream_deallocate( err );

	return 0;
}


static void test_stream_declare( void )
{
	ADD_TEST( stream, std );
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
