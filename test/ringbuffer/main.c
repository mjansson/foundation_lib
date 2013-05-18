/* main.c  -  Foundation ringbuffer test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
	app.name = "Foundation ringbuffer tests";
	app.short_name = "test_ringbuffer";
	app.config_dir = "test_ringbuffer";
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


DECLARE_TEST( ringbuffer, allocate )
{
	ringbuffer_t* buffer;
	char store[256];

	buffer = ringbuffer_allocate( 0 );
	EXPECT_EQ( ringbuffer_size( buffer ), 0 );
	EXPECT_EQ( ringbuffer_total_read( buffer ), 0 );
	EXPECT_EQ( ringbuffer_total_written( buffer ), 0 );

	EXPECT_EQ( ringbuffer_read( buffer, store, 0 ), 0 );
	EXPECT_EQ( ringbuffer_write( buffer, store, 0 ), 0 );
	EXPECT_EQ( ringbuffer_size( buffer ), 0 );
	EXPECT_EQ( ringbuffer_total_read( buffer ), 0 );
	EXPECT_EQ( ringbuffer_total_written( buffer ), 0 );

	EXPECT_EQ( ringbuffer_read( buffer, store, 256 ), 0 );
	EXPECT_EQ( ringbuffer_write( buffer, store, 256 ), 0 );
	EXPECT_EQ( ringbuffer_size( buffer ), 0 );
	EXPECT_EQ( ringbuffer_total_read( buffer ), 0 );
	EXPECT_EQ( ringbuffer_total_written( buffer ), 0 );

	ringbuffer_deallocate( buffer );

	buffer = ringbuffer_allocate( 128 );
	EXPECT_EQ( ringbuffer_size( buffer ), 128 );
	EXPECT_EQ( ringbuffer_total_read( buffer ), 0 );
	EXPECT_EQ( ringbuffer_total_written( buffer ), 0 );

	EXPECT_EQ( ringbuffer_write( buffer, store, 0 ), 0 );
	EXPECT_EQ( ringbuffer_read( buffer, store, 0 ), 0 );
	EXPECT_EQ( ringbuffer_size( buffer ), 128 );
	EXPECT_EQ( ringbuffer_total_read( buffer ), 0 );
	EXPECT_EQ( ringbuffer_total_written( buffer ), 0 );

	EXPECT_EQ( ringbuffer_write( buffer, store, 256 ), 127 );
	EXPECT_EQ( ringbuffer_read( buffer, store, 256 ), 127 );
	EXPECT_EQ( ringbuffer_size( buffer ), 128 );
	EXPECT_EQ( ringbuffer_total_read( buffer ), 127 );
	EXPECT_EQ( ringbuffer_total_written( buffer ), 127 );

	ringbuffer_deallocate( buffer );

	buffer = ringbuffer_allocate( 256 );
	EXPECT_EQ( ringbuffer_size( buffer ), 256 );
	EXPECT_EQ( ringbuffer_total_read( buffer ), 0 );
	EXPECT_EQ( ringbuffer_total_written( buffer ), 0 );

	EXPECT_EQ( ringbuffer_write( buffer, store, 0 ), 0 );
	EXPECT_EQ( ringbuffer_read( buffer, store, 0 ), 0 );
	EXPECT_EQ( ringbuffer_size( buffer ), 256 );
	EXPECT_EQ( ringbuffer_total_read( buffer ), 0 );
	EXPECT_EQ( ringbuffer_total_written( buffer ), 0 );

	EXPECT_EQ( ringbuffer_write( buffer, store, 256 ), 255 );
	EXPECT_EQ( ringbuffer_read( buffer, store, 256 ), 255 );
	EXPECT_EQ( ringbuffer_size( buffer ), 256 );
	EXPECT_EQ( ringbuffer_total_read( buffer ), 255 );
	EXPECT_EQ( ringbuffer_total_written( buffer ), 255 );

	ringbuffer_deallocate( buffer );

	buffer = ringbuffer_allocate( 512 );
	EXPECT_EQ( ringbuffer_size( buffer ), 512 );
	EXPECT_EQ( ringbuffer_total_read( buffer ), 0 );
	EXPECT_EQ( ringbuffer_total_written( buffer ), 0 );

	EXPECT_EQ( ringbuffer_write( buffer, store, 0 ), 0 );
	EXPECT_EQ( ringbuffer_read( buffer, store, 0 ), 0 );
	EXPECT_EQ( ringbuffer_size( buffer ), 512 );
	EXPECT_EQ( ringbuffer_total_read( buffer ), 0 );
	EXPECT_EQ( ringbuffer_total_written( buffer ), 0 );

	EXPECT_EQ( ringbuffer_write( buffer, store, 256 ), 256 );
	EXPECT_EQ( ringbuffer_read( buffer, store, 256 ), 256 );
	EXPECT_EQ( ringbuffer_size( buffer ), 512 );
	EXPECT_EQ( ringbuffer_total_read( buffer ), 256 );
	EXPECT_EQ( ringbuffer_total_written( buffer ), 256 );

	EXPECT_EQ( ringbuffer_write( buffer, store, 256 ), 256 );
	EXPECT_EQ( ringbuffer_read( buffer, store, 256 ), 256 );
	EXPECT_EQ( ringbuffer_size( buffer ), 512 );
	EXPECT_EQ( ringbuffer_total_read( buffer ), 512 );
	EXPECT_EQ( ringbuffer_total_written( buffer ), 512 );

	ringbuffer_deallocate( buffer );
	
	return 0;
}


DECLARE_TEST( ringbuffer, io )
{
	ringbuffer_t* buffer;
	char from[256];
	char to[256];
	unsigned int size, verify, loop, loops;
	unsigned int expected_size = 0;

	for( size = 0; size < 256; ++size )
		from[size] = (char)( random32() & 0xFF );
	
	buffer = ringbuffer_allocate( 512 );
	loops = 32;
	for( loop = 0; loop < loops; ++loop )
	{
		for( size = 0; size < 256; ++size )
		{
			ringbuffer_write( buffer, from, size );
			ringbuffer_read( buffer, to, size );

			for( verify = 0; verify < size; ++verify )
				EXPECT_EQ( to[verify], from[verify] );
		
			expected_size += size;
		}
	}
	EXPECT_EQ( ringbuffer_total_read( buffer ), expected_size );
	EXPECT_EQ( ringbuffer_total_written( buffer ), expected_size );

	return 0;
}


typedef struct
{
	stream_t* stream;
	
	object_t read_thread;
	object_t write_thread;

	char*    source_buffer;
	char*    dest_buffer;

	unsigned int buffer_size;
	
	tick_t   start_time;
	tick_t   end_time;
} ringbufferstream_test_t;
	

static void* read_thread( object_t thread, void* arg )
{
	ringbufferstream_test_t* test = arg;
	stream_read( test->stream, test->dest_buffer, test->buffer_size );
	test->end_time = time_current();
	return 0;
}


static void* write_thread( object_t thread, void* arg )
{
	ringbufferstream_test_t* test = arg;
	test->start_time = time_current();
	stream_write( test->stream, test->source_buffer, test->buffer_size );
	return 0;
}


DECLARE_TEST( ringbufferstream, threadedio )
{
	ringbufferstream_test_t test = {0};
	uint32_t* srcbuffer;
	unsigned int si;
	unsigned int loop, loops;
	real elapsed, throughput;
	unsigned int mbytes;

	mbytes = 256;
	loops = 16;
	
	test.buffer_size = mbytes * 1024 * 1024;
	test.source_buffer = memory_allocate( test.buffer_size, 0, MEMORY_PERSISTENT );
	test.dest_buffer   = memory_allocate_zero( test.buffer_size, 0, MEMORY_PERSISTENT );

	srcbuffer = (uint32_t*)test.source_buffer;
	for( si = 0; si < ( test.buffer_size / 4 ); ++si )
		srcbuffer[si] = random32();

	elapsed = 0;
	for( loop = 0; loop < loops; ++loop )
	{
		test.stream = ringbuffer_stream_allocate( 23477, test.buffer_size );

		test.read_thread = thread_create( read_thread, "reader", THREAD_PRIORITY_NORMAL, 0 );
		test.write_thread = thread_create( write_thread, "writer", THREAD_PRIORITY_NORMAL, 0 );

		thread_start( test.read_thread, &test );
		thread_start( test.write_thread, &test );
		thread_sleep( 100 );

		while( thread_is_running( test.read_thread ) || thread_is_running( test.write_thread ) )
			thread_sleep( 10 );

		thread_destroy( test.read_thread );
		thread_destroy( test.write_thread );

		for( si = 0; si < test.buffer_size; ++si )
			EXPECT_EQ( test.source_buffer[si], test.dest_buffer[si] );

		stream_deallocate( test.stream );

		elapsed += time_ticks_to_seconds( time_diff( test.start_time, test.end_time ) );
	}
	throughput = (real)( (float64_t)( mbytes * loops ) / (float64_t)elapsed );
	log_infof( "Ringbuffer throughput: %d MiB in %.2f sec -> %.2f MiB/sec", ( loops * mbytes ), (float32_t)elapsed, (float32_t)throughput );

	elapsed = 0;
	for( loop = 0; loop < loops; ++loop )
	{
		test.start_time = time_current();
		memcpy( test.dest_buffer, test.source_buffer, test.buffer_size );
		test.end_time = time_current();

		for( si = 0; si < test.buffer_size; ++si )
			EXPECT_EQ( test.source_buffer[si], test.dest_buffer[si] );

		elapsed += time_ticks_to_seconds( time_diff( test.start_time, test.end_time ) );
	}
	throughput = (real)( (float64_t)( mbytes * loops ) / (float64_t)elapsed );
	log_infof( "Memcpy     throughput: %d MiB in %.2f sec -> %.2f MiB/sec", ( loops * mbytes ), (float32_t)elapsed, (float32_t)throughput );

	return 0;
}


void test_declare( void )
{
	ADD_TEST( ringbuffer, allocate );
	ADD_TEST( ringbuffer, io );

	ADD_TEST( ringbufferstream, threadedio );
}
