/* main.c  -  Foundation system test  -  Public Domain  -  2013 Mattias Jansson
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#include <foundation/foundation.h>
#include <test/test.h>

static application_t
test_system_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation system tests"));
	app.short_name = string_const(STRING_CONST("test_system"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_system_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_system_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_system_initialize(void) {
	return 0;
}

static void
test_system_finalize(void) {
}

DECLARE_TEST(system, align) {
	EXPECT_INTGE(FOUNDATION_ALIGNOF(atomic32_t), 4);
	EXPECT_INTGE(FOUNDATION_ALIGNOF(atomic64_t), 8);
	EXPECT_INTGE(FOUNDATION_ALIGNOF(atomicptr_t), FOUNDATION_SIZE_POINTER);

	EXPECT_INTGE(FOUNDATION_ALIGNOF(event_stream_t), 8);
	EXPECT_INTGE(FOUNDATION_ALIGNOF(hashtable32_entry_t), 8);
	EXPECT_INTGE(FOUNDATION_ALIGNOF(hashtable64_entry_t), 8);
	EXPECT_INTGE(FOUNDATION_ALIGNOF(hashtable32_t), 8);
	EXPECT_INTGE(FOUNDATION_ALIGNOF(hashtable64_t), 8);
	EXPECT_INTGE(FOUNDATION_ALIGNOF(stream_t), 8);
	EXPECT_INTGE(FOUNDATION_ALIGNOF(stream_ringbuffer_t), 8);
	EXPECT_INTGE(FOUNDATION_ALIGNOF(stream_pipe_t), 8);
	EXPECT_INTGE(FOUNDATION_ALIGNOF(stream_buffer_t), 8);

	return 0;
}

DECLARE_TEST(system, builtin) {
	device_orientation_t orientation;
	char buffer[512];

#if FOUNDATION_PLATFORM_WINDOWS
	EXPECT_EQ(system_platform(), PLATFORM_WINDOWS);
#elif FOUNDATION_PLATFORM_ANDROID
	EXPECT_EQ(system_platform(), PLATFORM_ANDROID);
#elif FOUNDATION_PLATFORM_LINUX_RASPBERRYPI
	EXPECT_EQ(system_platform(), PLATFORM_RASPBERRYPI);
#elif FOUNDATION_PLATFORM_LINUX
	EXPECT_EQ(system_platform(), PLATFORM_LINUX);
#elif FOUNDATION_PLATFORM_MACOS
	EXPECT_EQ(system_platform(), PLATFORM_MACOS);
#elif FOUNDATION_PLATFORM_IOS
	EXPECT_EQ(system_platform(), PLATFORM_IOS);
#elif FOUNDATION_PLATFORM_BSD
	EXPECT_EQ(system_platform(), PLATFORM_BSD);
#elif FOUNDATION_PLATFORM_TIZEN
	EXPECT_EQ(system_platform(), PLATFORM_TIZEN);
#else
#error Unknown platform
#endif

#if FOUNDATION_ARCH_X86_64
	EXPECT_EQ(system_architecture(), ARCHITECTURE_X86_64);
#elif FOUNDATION_ARCH_X86
	EXPECT_EQ(system_architecture(), ARCHITECTURE_X86);
#elif FOUNDATION_ARCH_PPC_64
	EXPECT_EQ(system_architecture(), ARCHITECTURE_PPC_64);
#elif FOUNDATION_ARCH_PPC
	EXPECT_EQ(system_architecture(), ARCHITECTURE_PPC);
#elif FOUNDATION_ARCH_ARM8_64
	EXPECT_EQ(system_architecture(), ARCHITECTURE_ARM8_64);
#elif FOUNDATION_ARCH_ARM8
	EXPECT_EQ(system_architecture(), ARCHITECTURE_ARM8);
#elif FOUNDATION_ARCH_ARM7
	EXPECT_EQ(system_architecture(), ARCHITECTURE_ARM7);
#elif FOUNDATION_ARCH_ARM6
	EXPECT_EQ(system_architecture(), ARCHITECTURE_ARM6);
#elif FOUNDATION_ARCH_ARM5
	EXPECT_EQ(system_architecture(), ARCHITECTURE_ARM5);
#elif FOUNDATION_ARCH_MIPS_64
	EXPECT_EQ(system_architecture(), ARCHITECTURE_MIPS_64);
#elif FOUNDATION_ARCH_MIPS
	EXPECT_EQ(system_architecture(), ARCHITECTURE_MIPS);
#elif FOUNDATION_ARCH_GENERIC
	EXPECT_EQ(system_architecture(), ARCHITECTURE_GENERIC);
#else
#error Unknown architecture
#endif

#if FOUNDATION_ARCH_ENDIAN_LITTLE
	EXPECT_EQ(system_byteorder(), BYTEORDER_LITTLEENDIAN);
#else
	EXPECT_EQ(system_byteorder(), BYTEORDER_BIGENDIAN);
#endif

	EXPECT_GE(system_hardware_threads(), 1);

	EXPECT_NE(system_hostname(buffer, sizeof(buffer)).str, 0);
	EXPECT_GT(system_hostname(buffer, sizeof(buffer)).length, 0);
	EXPECT_NE(system_username(buffer, sizeof(buffer)).str, 0);
	EXPECT_GT(system_username(buffer, sizeof(buffer)).length, 0);
	EXPECT_NE(system_hostname(buffer, 2).str, 0);
	EXPECT_EQ(system_hostname(buffer, 2).length, 1);
	EXPECT_NE(system_username(buffer, 2).str, 0);
	EXPECT_EQ(system_username(buffer, 2).length, 1);

#if !FOUNDATION_PLATFORM_ANDROID
	EXPECT_NE(system_hostid(), 0);
#endif

	system_error_reset();
	EXPECT_EQ(system_error(), 0);

	EXPECT_CONSTSTRINGEQ(system_error_message(0), string_const(STRING_CONST("<no error>")));
	EXPECT_NE(system_error_message(1).str, 0);
	EXPECT_GT(system_error_message(1).length, 0);

	EXPECT_NE(system_language(), 0);
	EXPECT_NE(system_country(), 0);
	EXPECT_NE(system_locale(), 0);
	EXPECT_NE(system_locale_string(buffer, sizeof(buffer)).str, 0);
	EXPECT_EQ(system_locale_string(buffer, sizeof(buffer)).length, 4);
	EXPECT_NE(system_locale_string(buffer, 2).str, 0);
	EXPECT_EQ(system_locale_string(buffer, 2).length, 2);

	system_set_locale(LOCALE_FROM_LANGUAGE_COUNTRY(LANGUAGE_SWEDISH, COUNTRY_SWEDEN));
	EXPECT_EQ_MSGFORMAT(system_language(), LANGUAGE_SWEDISH, "language change was not picked up: 0x%04x",
	                    system_language());
	EXPECT_EQ_MSGFORMAT(system_country(), COUNTRY_SWEDEN, "country change was not picked up: 0x%04x", system_country());
	EXPECT_EQ_MSGFORMAT(system_locale(), LOCALE_FROM_LANGUAGE_COUNTRY(LANGUAGE_SWEDISH, COUNTRY_SWEDEN),
	                    "locale change was not picked up: 0x%08x", system_locale());
	EXPECT_STRINGEQ_MSGFORMAT(system_locale_string(buffer, sizeof(buffer)), string_const(STRING_CONST("svSE")),
	                          "locale change was not picked up: %s", buffer);

	orientation = system_device_orientation();
	system_set_device_orientation(DEVICEORIENTATION_PORTRAIT);
	EXPECT_EQ(system_device_orientation(), DEVICEORIENTATION_PORTRAIT);
	system_set_device_orientation(DEVICEORIENTATION_UNKNOWN);
	EXPECT_EQ(system_device_orientation(), DEVICEORIENTATION_UNKNOWN);
	system_set_device_orientation(orientation);

	return 0;
}

FOUNDATION_DECLARE_THREAD_LOCAL(int, tls_var, 0)

static void*
tls_thread(void* arg) {
	int counter = 0;
	int i;
	FOUNDATION_UNUSED(arg);
	for (i = 0; i < 1024; ++i) {
		EXPECT_EQ(get_thread_tls_var(), counter);
		thread_sleep(5);
		set_thread_tls_var(++counter);
		thread_sleep(5);
	}

	EXPECT_EQ(get_thread_tls_var(), counter);

	return 0;
}

DECLARE_TEST(system, thread) {
	thread_t thread[32];
	size_t ith;
	size_t threads_count = math_clamp(system_hardware_threads() * 2, 4, 32);

	for (ith = 0; ith < threads_count; ++ith)
		thread_initialize(&thread[ith], tls_thread, nullptr, STRING_CONST("tls_thread"), THREAD_PRIORITY_NORMAL, 0);
	for (ith = 0; ith < threads_count; ++ith)
		thread_start(&thread[ith]);

	test_wait_for_threads_startup(thread, threads_count);
	test_wait_for_threads_finish(thread, threads_count);

	for (ith = 0; ith < threads_count; ++ith) {
		EXPECT_EQ(thread_join(&thread[ith]), nullptr);
	}

	for (ith = 0; ith < threads_count; ++ith)
		thread_finalize(&thread[ith]);

	return 0;
}

static void
test_system_declare(void) {
	ADD_TEST(system, align);
	ADD_TEST(system, builtin);
	ADD_TEST(system, thread);
}

static test_suite_t test_system_suite = {test_system_application,
                                         test_system_memory_system,
                                         test_system_config,
                                         test_system_declare,
                                         test_system_initialize,
                                         test_system_finalize,
                                         0};

#if BUILD_MONOLITHIC

int
test_system_run(void);

int
test_system_run(void) {
	test_suite = test_system_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_system_suite;
}

#endif
