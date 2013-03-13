/* main.c  -  Foundation event test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
	app.name = "Foundation event tests";
	app.short_name = "test_event";
	app.config_dir = "test_event";
	return app;
}


DECLARE_TEST( event, empty )
{
	event_stream_t* stream;
	event_block_t* block;
	event_block_t* old_block;
	event_t* event;
	
	stream = event_stream_allocate( 0 );
	block = event_stream_process( stream );

	event = event_first( block );
	EXPECT_EQ( event, 0 );

	event = event_next( block, event );
	EXPECT_EQ( event, 0 );

	old_block = block;
	block = event_stream_process( stream );
	EXPECT_NE( old_block, block );

	event = event_first( block );
	EXPECT_EQ( event, 0 );

	event = event_next( block, event );
	EXPECT_EQ( event, 0 );

	block = event_stream_process( stream );
	EXPECT_EQ( old_block, block );

	event = event_first( block );
	EXPECT_EQ( event, 0 );

	event = event_next( block, event );
	EXPECT_EQ( event, 0 );

	event_stream_deallocate( stream );
	

	stream = event_stream_allocate( 4096 );
	block = event_stream_process( stream );

	event = event_first( block );
	EXPECT_EQ( event, 0 );

	old_block = block;
	block = event_stream_process( stream );
	EXPECT_NE( old_block, block );

	event = event_first( block );
	EXPECT_EQ( event, 0 );

	event = event_next( block, event );
	EXPECT_EQ( event, 0 );

	block = event_stream_process( stream );
	EXPECT_EQ( old_block, block );

	event = event_first( block );
	EXPECT_EQ( event, 0 );

	event = event_next( block, event );
	EXPECT_EQ( event, 0 );

	event_stream_deallocate( stream );

	return 0;
}


DECLARE_TEST( event, immediate )
{
	event_stream_t* stream;
	event_block_t* block;
	event_t* event;
	uint16_t last_serial = 0;
	uint8_t buffer[128] = {0};
	
	stream = event_stream_allocate( 0 );

	event_post( stream, SYSTEM_FOUNDATION, FOUNDATIONEVENT_TERMINATE, 0, 0, 0 );

	block = event_stream_process( stream );
	event = event_first( block );
	EXPECT_NE( event, 0 );
	EXPECT_EQ( event->system, SYSTEM_FOUNDATION );
	EXPECT_EQ( event->id, FOUNDATIONEVENT_TERMINATE );
	EXPECT_EQ( event->size, sizeof( event_t ) );
	EXPECT_GT( event->serial, last_serial );
	EXPECT_EQ( event->object, 0 );
	EXPECT_EQ( event->flags, 0 );
	EXPECT_EQ( event_payload_size( event ), 0 );
	last_serial = event->serial;

	event = event_next( block, event );
	EXPECT_EQ( event, 0 );

	block = event_stream_process( stream );

	event = event_first( block );
	EXPECT_EQ( event, 0 );

	event = event_next( block, event );
	EXPECT_EQ( event, 0 );


	event_post( stream, SYSTEM_FOUNDATION, FOUNDATIONEVENT_TERMINATE, 13, 0, buffer );
	event_post( stream, SYSTEM_FOUNDATION, FOUNDATIONEVENT_TERMINATE + 1, 37, 0, buffer );

	block = event_stream_process( stream );
	event = event_first( block );
	EXPECT_NE( event, 0 );
	EXPECT_EQ( event->system, SYSTEM_FOUNDATION );
	EXPECT_EQ( event->id, FOUNDATIONEVENT_TERMINATE );
	EXPECT_EQ( event->size, sizeof( event_t ) + 16 );
	EXPECT_GT( event->serial, last_serial );
	EXPECT_EQ( event->object, 0 );
	EXPECT_EQ( event->flags, 0 );
	EXPECT_EQ( event_payload_size( event ), 16 );
	last_serial = event->serial;

	event = event_next( block, event );
	EXPECT_NE( event, 0 );
	EXPECT_EQ( event->system, SYSTEM_FOUNDATION );
	EXPECT_EQ( event->id, FOUNDATIONEVENT_TERMINATE + 1 );
	EXPECT_EQ( event->size, sizeof( event_t ) + 40 );
	EXPECT_GT( event->serial, last_serial );
	EXPECT_EQ( event->object, 0 );
	EXPECT_EQ( event->flags, 0 );
	EXPECT_EQ( event_payload_size( event ), 40 );
	last_serial = event->serial;


	event_stream_deallocate( stream );

	return 0;
}


DECLARE_TEST( event, immediate_threaded )
{
	return 0;
}


DECLARE_TEST( event, delay )
{
	event_stream_t* stream;
	event_block_t* block;
	event_t* event;
	tick_t delivery = 0;
	tick_t limit = 0;
	tick_t current = 0;
	uint8_t expect_event = FOUNDATIONEVENT_TERMINATE;
	
	stream = event_stream_allocate( 0 );

	delivery = time_current() + ( time_ticks_per_second() / 2 );
	limit = time_current() + ( time_ticks_per_second() * 20 );

	event_post_delay_ticks( stream, SYSTEM_FOUNDATION, expect_event, 0, 0, 0, ( time_ticks_per_second() / 2 ) );
	event_post_delay( stream, SYSTEM_FOUNDATION, expect_event + 1, 0, 0, 0, REAL_C( 0.51 ) );

	do
	{
		block = event_stream_process( stream );
		event = event_first( block );

		current = time_current();

		if( ( expect_event == FOUNDATIONEVENT_TERMINATE ) && event )
		{
			EXPECT_EQ( event->system, SYSTEM_FOUNDATION );
			EXPECT_EQ( event->id, expect_event );
			EXPECT_EQ( event->size, sizeof( event_t ) + 8 ); //8 bytes for additional timestamp payload
			EXPECT_EQ( event->object, 0 );
			EXPECT_EQ( event->flags, EVENTFLAG_DELAY );
			EXPECT_EQ( event_payload_size( event ), 0 );

			EXPECT_GE( current, delivery );
			EXPECT_GE( current, delivery );

			++expect_event;

			event = event_next( block, event );
		}

		if( ( expect_event == FOUNDATIONEVENT_TERMINATE + 1 ) && event )
		{
			EXPECT_EQ( event->system, SYSTEM_FOUNDATION );
			EXPECT_EQ( event->id, expect_event );
			EXPECT_EQ( event->size, sizeof( event_t ) + 8 ); //8 bytes for additional timestamp payload
			EXPECT_EQ( event->object, 0 );
			EXPECT_EQ( event->flags, EVENTFLAG_DELAY );
			EXPECT_EQ( event_payload_size( event ), 0 );

			EXPECT_GE( current, delivery );
			EXPECT_GE( current, delivery );

			++expect_event;

			event = event_next( block, event );

			EXPECT_EQ( event, 0 );
		}

		if( ( expect_event > FOUNDATIONEVENT_TERMINATE + 1 ) && !event )
			break;

		thread_yield();

	} while( time_current() < limit );

	EXPECT_EQ( event, 0 );
	EXPECT_EQ( expect_event, FOUNDATIONEVENT_TERMINATE + 2 );
	EXPECT_LE( time_current(), limit );

	//Reverse order of delivery
	delivery = time_current() + ( time_ticks_per_second() / 2 );
	limit = time_current() + ( time_ticks_per_second() * 20 );

	event_post_delay_ticks( stream, SYSTEM_FOUNDATION, expect_event, 0, 0, 0, ( time_ticks_per_second() / 2 ) );
	event_post_delay( stream, SYSTEM_FOUNDATION, expect_event + 1, 0, 0, 0, REAL_C( 0.41 ) );

	do
	{
		block = event_stream_process( stream );
		event = event_first( block );

		current = time_current();

		if( ( expect_event == FOUNDATIONEVENT_TERMINATE ) && event )
		{
			EXPECT_EQ( event->system, SYSTEM_FOUNDATION );
			EXPECT_EQ( event->id, expect_event );
			EXPECT_EQ( event->size, sizeof( event_t ) + 8 ); //8 bytes for additional timestamp payload
			EXPECT_EQ( event->object, 0 );
			EXPECT_EQ( event->flags, EVENTFLAG_DELAY );
			EXPECT_EQ( event_payload_size( event ), 0 );

			EXPECT_GE( current, delivery );
			EXPECT_GE( current, delivery );

			++expect_event;

			event = event_next( block, event );
		}

		if( ( expect_event == FOUNDATIONEVENT_TERMINATE + 1 ) && event )
		{
			EXPECT_EQ( event->system, SYSTEM_FOUNDATION );
			EXPECT_EQ( event->id, expect_event );
			EXPECT_EQ( event->size, sizeof( event_t ) + 8 ); //8 bytes for additional timestamp payload
			EXPECT_EQ( event->object, 0 );
			EXPECT_EQ( event->flags, EVENTFLAG_DELAY );
			EXPECT_EQ( event_payload_size( event ), 0 );

			EXPECT_GE( current, delivery );
			EXPECT_GE( current, delivery );

			++expect_event;

			event = event_next( block, event );

			EXPECT_EQ( event, 0 );
		}

		if( ( expect_event > FOUNDATIONEVENT_TERMINATE + 1 ) && !event )
			break;

		thread_yield();

	} while( time_current() < limit );

	EXPECT_EQ( event, 0 );
	EXPECT_EQ( expect_event, FOUNDATIONEVENT_TERMINATE + 2 );
	EXPECT_LE( time_current(), limit );

	return 0;
}


DECLARE_TEST( event, delay_threaded )
{
	return 0;
}


void test_declare( void )
{
	ADD_TEST( event, empty );
	ADD_TEST( event, immediate );
	ADD_TEST( event, delay );
	ADD_TEST( event, immediate_threaded );
	ADD_TEST( event, delay_threaded );
}
