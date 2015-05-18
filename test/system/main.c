/* main.c  -  Foundation system test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


static application_t test_system_application( void )
{
	application_t app;
	memset( &app, 0, sizeof( app ) );
	app.name = string_const( STRING_CONST( "Foundation system tests" ) );
	app.short_name = string_const( STRING_CONST( "test_system" ) );
	app.config_dir = string_const( STRING_CONST( "test_system" ) );
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_system_memory_system( void )
{
	return memory_system_malloc();
}


static int test_system_initialize( void )
{
	return 0;
}


static void test_system_shutdown( void )
{
}


DECLARE_TEST( system, align )
{
	EXPECT_INTGE( FOUNDATION_ALIGNOF( atomic32_t ), 4 );
	EXPECT_INTGE( FOUNDATION_ALIGNOF( atomic64_t ), 8 );
	EXPECT_INTGE( FOUNDATION_ALIGNOF( atomicptr_t ), FOUNDATION_SIZE_POINTER );

	EXPECT_INTGE( FOUNDATION_ALIGNOF( event_stream_t ), 8 );
	EXPECT_INTGE( FOUNDATION_ALIGNOF( hashtable32_entry_t ), 8 );
	EXPECT_INTGE( FOUNDATION_ALIGNOF( hashtable64_entry_t ), 8 );
	EXPECT_INTGE( FOUNDATION_ALIGNOF( hashtable32_t ), 8 );
	EXPECT_INTGE( FOUNDATION_ALIGNOF( hashtable64_t ), 8 );
	EXPECT_INTGE( FOUNDATION_ALIGNOF( object_base_t ), 8 );
	EXPECT_INTGE( FOUNDATION_ALIGNOF( objectmap_t ), 8 );
	EXPECT_INTGE( FOUNDATION_ALIGNOF( stream_t ), 8 );
	EXPECT_INTGE( FOUNDATION_ALIGNOF( stream_ringbuffer_t ), 8 );
	EXPECT_INTGE( FOUNDATION_ALIGNOF( stream_pipe_t ), 8 );
	EXPECT_INTGE( FOUNDATION_ALIGNOF( stream_buffer_t ), 8 );

	return 0;
}


DECLARE_TEST( system, builtin )
{
	device_orientation_t orientation;

#if FOUNDATION_PLATFORM_WINDOWS
	EXPECT_EQ( system_platform(), PLATFORM_WINDOWS );
#elif FOUNDATION_PLATFORM_ANDROID
	EXPECT_EQ( system_platform(), PLATFORM_ANDROID );
#elif FOUNDATION_PLATFORM_LINUX_RASPBERRYPI
	EXPECT_EQ( system_platform(), PLATFORM_RASPBERRYPI );
#elif FOUNDATION_PLATFORM_LINUX
	EXPECT_EQ( system_platform(), PLATFORM_LINUX );
#elif FOUNDATION_PLATFORM_MACOSX
	EXPECT_EQ( system_platform(), PLATFORM_MACOSX );
#elif FOUNDATION_PLATFORM_IOS
	EXPECT_EQ( system_platform(), PLATFORM_IOS );
#elif FOUNDATION_PLATFORM_PNACL
	EXPECT_EQ( system_platform(), PLATFORM_PNACL );
#elif FOUNDATION_PLATFORM_BSD
	EXPECT_EQ( system_platform(), PLATFORM_BSD );
#elif FOUNDATION_PLATFORM_TIZEN
	EXPECT_EQ( system_platform(), PLATFORM_TIZEN );
#else
#  error Unknown platform
#endif

#if FOUNDATION_ARCH_X86_64
	EXPECT_EQ( system_architecture(), ARCHITECTURE_X86_64 );
#elif FOUNDATION_ARCH_X86
	EXPECT_EQ( system_architecture(), ARCHITECTURE_X86 );
#elif FOUNDATION_ARCH_PPC_64
	EXPECT_EQ( system_architecture(), ARCHITECTURE_PPC_64 );
#elif FOUNDATION_ARCH_PPC
	EXPECT_EQ( system_architecture(), ARCHITECTURE_PPC );
#elif FOUNDATION_ARCH_ARM8_64
	EXPECT_EQ( system_architecture(), ARCHITECTURE_ARM8_64 );
#elif FOUNDATION_ARCH_ARM8
	EXPECT_EQ( system_architecture(), ARCHITECTURE_ARM8 );
#elif FOUNDATION_ARCH_ARM7
	EXPECT_EQ( system_architecture(), ARCHITECTURE_ARM7 );
#elif FOUNDATION_ARCH_ARM6
	EXPECT_EQ( system_architecture(), ARCHITECTURE_ARM6 );
#elif FOUNDATION_ARCH_ARM5
	EXPECT_EQ( system_architecture(), ARCHITECTURE_ARM5 );
#elif FOUNDATION_ARCH_MIPS_64
	EXPECT_EQ( system_architecture(), ARCHITECTURE_MIPS_64 );
#elif FOUNDATION_ARCH_MIPS
	EXPECT_EQ( system_architecture(), ARCHITECTURE_MIPS );
#elif FOUNDATION_ARCH_GENERIC
	EXPECT_EQ( system_architecture(), ARCHITECTURE_GENERIC );
#else
#  error Unknown architecture
#endif

#if FOUNDATION_ARCH_ENDIAN_LITTLE
	EXPECT_EQ( system_byteorder(), BYTEORDER_LITTLEENDIAN );
#else
	EXPECT_EQ( system_byteorder(), BYTEORDER_BIGENDIAN );
#endif

	EXPECT_GE( system_hardware_threads(), 1 );

	EXPECT_NE( system_hostname(), 0 );
	EXPECT_GT( string_length( system_hostname() ), 0 );
#if !FOUNDATION_PLATFORM_PNACL
	EXPECT_NE( system_hostid(), 0 );
#endif
	EXPECT_NE( system_username(), 0 );
	EXPECT_GT( string_length( system_username() ), 0 );

	system_error_reset();
	EXPECT_EQ( system_error(), 0 );

	EXPECT_STREQ( system_error_message( 0 ), "<no error>" );
	EXPECT_NE( system_error_message( 1 ), 0 );

	EXPECT_NE( system_language(), 0 );
	EXPECT_NE( system_country(), 0 );
	EXPECT_NE( system_locale(), 0 );
	EXPECT_NE( system_locale_string(), 0 );
	EXPECT_EQ( string_length( system_locale_string() ), 4 );

	config_set_string_constant( HASH_FOUNDATION, HASH_LOCALE, "svSE" );
	EXPECT_EQ_MSGFORMAT( system_language(), LANGUAGE_SWEDISH, "config language change was not picked up: 0x%04x", system_language() );
	EXPECT_EQ_MSGFORMAT( system_country(), COUNTRY_SWEDEN, "config country change was not picked up: 0x%04x", system_country() );
	EXPECT_EQ_MSGFORMAT( system_locale(), LOCALE_FROM_LANGUAGE_COUNTRY( LANGUAGE_SWEDISH, COUNTRY_SWEDEN ), "config locale change was not picked up: 0x%08x", system_locale() );
	EXPECT_STREQ_MSGFORMAT( system_locale_string(), "svSE", "config locale change was not picked up: %s", system_locale_string() );

	orientation = system_device_orientation();
	system_set_device_orientation( DEVICEORIENTATION_PORTRAIT );
	EXPECT_EQ( system_device_orientation(), DEVICEORIENTATION_PORTRAIT );
	system_set_device_orientation( DEVICEORIENTATION_UNKNOWN );
	EXPECT_EQ( system_device_orientation(), DEVICEORIENTATION_UNKNOWN );
	system_set_device_orientation( orientation );

	return 0;
}


static void test_system_declare( void )
{
	ADD_TEST( system, align );
	ADD_TEST( system, builtin );
}


static test_suite_t test_system_suite = {
	test_system_application,
	test_system_memory_system,
	test_system_declare,
	test_system_initialize,
	test_system_shutdown
};


#if BUILD_MONOLITHIC

int test_system_run( void );
int test_system_run( void )
{
	test_suite = test_system_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void );
test_suite_t test_suite_define( void )
{
	return test_system_suite;
}

#endif
