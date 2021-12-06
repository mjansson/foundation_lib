/* main.c  -  Foundation random tests  -  Public Domain  -  2013 Mattias Jansson
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

static unsigned int test_hist[64];
static unsigned int test_bits[64];
static atomic32_t test_thread_hist[64];
static atomic32_t test_thread_bits[64];
static unsigned int test_slice32 = 0x8000000U;        //( 1U << 32U ) / 32U;
static uint64_t test_slice64 = 0x400000000000000ULL;  //( 1ULL << 64ULL ) / 64ULL;

static application_t
test_random_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation random tests"));
	app.short_name = string_const(STRING_CONST("test_random"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_random_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_random_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_random_initialize(void) {
	return 0;
}

static void
test_random_finalize(void) {
}

DECLARE_TEST(random, distribution32) {
	unsigned int pass_count = 512000 * 16;
	unsigned int max_num = 0, min_num = 0xFFFFFFFF;
	unsigned int num;
	unsigned int i, j;
	real diff;

	// Force allocations
	random32();

	memset(test_bits, 0, sizeof(unsigned int) * 32);
	memset(test_hist, 0, sizeof(unsigned int) * 32);
	for (i = 0; i < pass_count; ++i) {
		num = random32();
		for (j = 0; j < 32; ++j) {
			if (num & (1 << j))
				++test_bits[j];
			if ((num >= (test_slice32 * j)) && ((j == 31) || (num < (test_slice32 * (j + 1)))))
				++test_hist[j];
		}
	}

	/*log_debugf( "Bit distribution:" );
	for( j = 0; j < 32; ++j )
	    log_debugf( "%2u: %u", j, test_bits[j] );
	log_debugf( "Value distribution:" );
	for( j = 0; j < 32; ++j )
	    log_debugf( "%08x-%08x: %u", ( test_slice32 * j ), ( test_slice32 * ( j + 1 ) ) - 1, test_hist[j] );*/

	for (j = 0; j < 32; ++j) {
		if (test_bits[j] < min_num)
			min_num = test_bits[j];
		if (test_bits[j] > max_num)
			max_num = test_bits[j];
	}
	diff = (real)(max_num - min_num) / ((real)min_num + ((real)(max_num - min_num) / REAL_C(2.0)));

	for (j = 0; j < 32; ++j)
		EXPECT_GT(test_bits[j], 0U);
	EXPECT_LT(diff,
	          0.004);  // << "Bits: min " << min_num << " : max " << max_num << " : diff " << diff;

	// log_debugf( "Bits: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	max_num = 0;
	min_num = 0xFFFFFFFF;
	for (j = 0; j < 32; ++j) {
		if (test_hist[j] < min_num)
			min_num = test_hist[j];
		if (test_hist[j] > max_num)
			max_num = test_hist[j];
	}
	diff = (real)(max_num - min_num) / ((real)min_num + ((real)(max_num - min_num) / REAL_C(2.0)));

	for (j = 0; j < 32; ++j)
		EXPECT_GT(test_hist[j], 0U);
	EXPECT_LT(diff,
	          0.02);  // << "Histograms: min " << min_num << " : max " << max_num << " : diff " << diff;

	// log_debugf( "Histograms: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	// Verify range distribution
	memset(test_bits, 0, sizeof(unsigned int) * 32);
	for (i = 0; i < pass_count; ++i) {
		num = random32_range((j + 1) * 32, j * 32);
		EXPECT_GE(num, j * 32U);
		EXPECT_LT(num, (j + 1) * 32U);
		++test_bits[num % 32];
		EXPECT_INTEQ(random32_range(i, i + 1), (int)i);
	}

	// Verify distribution...
	max_num = 0;
	min_num = 0xFFFFFFFF;
	for (i = 0; i < 32; ++i) {
		if (test_bits[i] < min_num)
			min_num = test_bits[i];
		if (test_bits[i] > max_num)
			max_num = test_bits[i];
	}
	diff = (real)(max_num - min_num) / ((real)min_num + ((real)(max_num - min_num) / REAL_C(2.0)));

	for (i = 0; i < 32; ++i)
		EXPECT_GT(test_bits[i], 0U);
	EXPECT_LT(diff,
	          0.02);  // << "Range distribution: min " << min_num << " : max " << max_num << " : diff " << diff;

	// log_debugf( "Range distribution: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	return 0;
}

DECLARE_TEST(random, distribution64) {
	int pass_count = 512000 * 16;
	unsigned int max_num = 0, min_num = 0xFFFFFFFF;
	int i;
	uint64_t j;
	real diff;

	memset(test_bits, 0, sizeof(unsigned int) * 64);
	memset(test_hist, 0, sizeof(unsigned int) * 64);
	for (i = 0; i < pass_count; ++i) {
		uint64_t num = random64();
		for (j = 0; j < 64; ++j) {
			if (num & (1ULL << j))
				++test_bits[j];
			if ((num >= (test_slice64 * j)) && ((j == 63) || (num < (test_slice64 * (j + 1)))))
				++test_hist[j];
		}
	}

	/*log_debugf( "Bit distribution:" );
	for( j = 0; j < 64; ++j )
	    log_debugf( "%2u: %u", (unsigned int)j, test_bits[j] );
	log_debugf( "Value distribution:" );
	for( j = 0; j < 64; ++j )
	    log_debugf( "%016llx-%016llx: %u", ( test_slice64 * j ), ( test_slice64 * ( j + 1 ) ) - 1, test_hist[j] );*/

	for (j = 0; j < 64; ++j) {
		if (test_bits[j] < min_num)
			min_num = test_bits[j];
		if (test_bits[j] > max_num)
			max_num = test_bits[j];
	}
	diff = (real)(max_num - min_num) / ((real)min_num + ((real)(max_num - min_num) / REAL_C(2.0)));

	for (j = 0; j < 64; ++j)
		EXPECT_GT(test_bits[j], 0U);
	EXPECT_LT(diff,
	          0.004);  // << "Bits: min " << min_num << " : max " << max_num << " : diff " << diff;

	// log_debugf( "Bits: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	max_num = 0;
	min_num = 0xFFFFFFFF;
	for (j = 0; j < 64; ++j) {
		if (test_hist[j] < min_num)
			min_num = test_hist[j];
		if (test_hist[j] > max_num)
			max_num = test_hist[j];
	}
	diff = (real)(max_num - min_num) / ((real)min_num + ((real)(max_num - min_num) / REAL_C(2.0)));

	for (j = 0; j < 64; ++j)
		EXPECT_GT(test_hist[j], 0U);
	EXPECT_LT(diff,
	          0.02);  // << "Histograms: min " << min_num << " : max " << max_num << " : diff " << diff;

	// log_debugf( "Histograms: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	// Verify range distribution
	memset(test_bits, 0, sizeof(unsigned int) * 64);
	for (i = 0; i < pass_count; ++i) {
		uint64_t num = random64_range((j + 1) * 64, j * 64);
		uint64_t range = (uint64_t)i;
		EXPECT_GE(num, j * 64U);
		EXPECT_LT(num, (j + 1) * 64U);
		++test_bits[num % 64];
		EXPECT_TYPEEQ(random64_range(range, range + 1), range, uint64_t, PRIu64);
	}

	// Verify distribution...
	max_num = 0;
	min_num = 0xFFFFFFFF;
	for (i = 0; i < 64; ++i) {
		if (test_bits[i] < min_num)
			min_num = test_bits[i];
		if (test_bits[i] > max_num)
			max_num = test_bits[i];
	}
	diff = (real)(max_num - min_num) / ((real)min_num + ((real)(max_num - min_num) / REAL_C(2.0)));

	for (i = 0; i < 64; ++i)
		EXPECT_GT(test_bits[i], 0U);
	EXPECT_LT(diff,
	          0.02);  // << "Range distribution: min " << min_num << " : max " << max_num << " : diff " << diff;

	// log_debugf( "Range distribution: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	return 0;
}

DECLARE_TEST(random, distribution_real) {
	int pass_count = 512000 * 16;
	int i, idx;
	real diff, num;
	unsigned int max_num = 0, min_num = 0xFFFFFFFF;

	memset(test_hist, 0, sizeof(unsigned int) * 64);
	for (i = 0; i < pass_count; ++i) {
		num = random_normalized();
		EXPECT_GE(num, 0);
		EXPECT_LT(num, 1);
		idx = math_floor(num * REAL_C(64.0));
		++test_hist[(idx < 0 ? 0 : (idx > 63 ? 63 : idx))];
	}

	/*log_debugf( "Value distribution:" );
	for( i = 0; i < 64; ++i )
	    log_debugf( "%lf-%lf: %u", ( (1.0/64.0) * (double)i ), ( (1.0/64.0) * (double)( i + 1 ) ) - 1, test_hist[i]
	);*/

	for (i = 0; i < 64; ++i) {
		if (test_hist[i] < min_num)
			min_num = test_hist[i];
		if (test_hist[i] > max_num)
			max_num = test_hist[i];
	}
	diff = (real)(max_num - min_num) / ((real)min_num + ((real)(max_num - min_num) / REAL_C(2.0)));

	for (i = 0; i < 64; ++i)
		EXPECT_GT(test_hist[i], 0U);
	EXPECT_LT(diff,
	          0.02);  // << "Histograms: min " << min_num << " : max " << max_num << " : diff " << diff;

	// log_debugf( "Histograms: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	return 0;
}

static void*
random_thread(void* arg) {
	unsigned int pass_count = 512000 * 8;
	unsigned int i, j;
	unsigned int num;
	FOUNDATION_UNUSED(arg);

	for (i = 0; i < pass_count; ++i) {
		num = random32();
		for (j = 0; j < 32; ++j) {
			if (num & (1 << j))
				atomic_incr32(&test_thread_bits[j], memory_order_relaxed);
			if ((num >= (test_slice32 * j)) && ((j == 31) || (num < (test_slice32 * (j + 1)))))
				atomic_incr32(&test_thread_hist[j], memory_order_relaxed);
		}
	}
	return 0;
}

DECLARE_TEST(random, threads) {
	// Launch max 32 threads
	thread_t thread[32];
	size_t threads_count = math_clamp(system_hardware_threads() * 4, 4, 32);
	int max_num = 0, min_num = 0x7FFFFFFF;
	size_t i, j;
	real diff;
	for (i = 0; i < 32; ++i) {
		atomic_store32(&test_thread_bits[i], 0, memory_order_relaxed);
		atomic_store32(&test_thread_hist[i], 0, memory_order_relaxed);
	}

	for (i = 0; i < threads_count; ++i)
		thread_initialize(&thread[i], random_thread, 0, STRING_CONST("random"), THREAD_PRIORITY_NORMAL, 0);
	for (i = 0; i < threads_count; ++i)
		thread_start(&thread[i]);

	test_wait_for_threads_startup(thread, threads_count);
	test_wait_for_threads_finish(thread, threads_count);

	for (i = 0; i < threads_count; ++i)
		thread_finalize(&thread[i]);

	/*log_debugf( "Bit distribution:" );
	for( j = 0; j < 32; ++j )
	    log_debugf( "%2d: %d", j, atomic_load32( &test_thread_bits[j] ) );
	log_debugf( "Value distribution:" );
	for( j = 0; j < 32; ++j )
	    log_debugf( "%08x-%08x: %u", ( test_slice32 * j ), ( test_slice32 * ( j + 1 ) ) - 1, atomic_load32(
	&test_thread_hist[j] ) );*/

	for (j = 0; j < threads_count; ++j) {
		if (atomic_load32(&test_thread_bits[j], memory_order_acquire) < min_num)
			min_num = atomic_load32(&test_thread_bits[j], memory_order_acquire);
		if (atomic_load32(&test_thread_bits[j], memory_order_acquire) > max_num)
			max_num = atomic_load32(&test_thread_bits[j], memory_order_acquire);
	}
	diff = (real)(max_num - min_num) / ((real)min_num + ((real)(max_num - min_num) / REAL_C(2.0)));

	for (j = 0; j < threads_count; ++j)
		EXPECT_GT(atomic_load32(&test_thread_bits[j], memory_order_acquire), 0);
	EXPECT_LT(diff,
	          0.004);  // << "Bits: min " << min_num << " : max " << max_num << " : diff " << diff;

	// log_debugf( "Bits: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	max_num = 0;
	min_num = 0x7FFFFFFF;
	for (j = 0; j < threads_count; ++j) {
		if (atomic_load32(&test_thread_hist[j], memory_order_acquire) < min_num)
			min_num = atomic_load32(&test_thread_hist[j], memory_order_acquire);
		if (atomic_load32(&test_thread_hist[j], memory_order_acquire) > max_num)
			max_num = atomic_load32(&test_thread_hist[j], memory_order_acquire);
	}
	diff = (real)(max_num - min_num) / ((real)min_num + ((real)(max_num - min_num) / REAL_C(2.0)));

	for (j = 0; j < threads_count; ++j)
		EXPECT_GT(atomic_load32(&test_thread_hist[j], memory_order_acquire), 0);
	EXPECT_LT(diff,
	          0.02);  // << "Histograms: min " << min_num << " : max " << max_num << " : diff " << diff;

	// log_debugf( "Histograms: min %u : max %u : diff %.5lf", min_num, max_num, (double)diff );

	return 0;
}

DECLARE_TEST(random, util) {
	int i;
	int32_t ival32;
	uint32_t val32;
	uint64_t val64;
	real val;
	int pass_count = 512000;
	real weights[] = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f};
	real noweights[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	for (i = 0; i < pass_count; ++i) {
		val32 = random32_range(10, 100);
		EXPECT_GE(val32, 10);
		EXPECT_LT(val32, 100);

		val32 = random32_range(100, 10);
		EXPECT_GE(val32, 10);
		EXPECT_LT(val32, 100);

		val64 = random64_range(10, 100);
		EXPECT_GE(val64, 10);
		EXPECT_LT(val64, 100);

		val64 = random64_range(100, 10);
		EXPECT_GE(val64, 10);
		EXPECT_LT(val64, 100);

		val = random_range(REAL_C(0.0), REAL_C(100.0));
		EXPECT_GE(val, REAL_C(0.0));
		EXPECT_LT(val, REAL_C(100.0));

		val = random_range(REAL_C(100.0), REAL_C(0.0));
		EXPECT_GE(val, REAL_C(0.0));
		EXPECT_LT(val, REAL_C(100.0));

		// TODO: Check distribution of these functions
		ival32 = random32_gaussian_range(-32, 32);
		EXPECT_GE(ival32, -32);
		EXPECT_LT(ival32, 32);

		ival32 = random32_gaussian_range(32, -32);
		EXPECT_GE(ival32, -32);
		EXPECT_LT(ival32, 32);

		val = random_gaussian_range(REAL_C(-32.0), REAL_C(32.0));
		EXPECT_GE(val, REAL_C(-32.0));
		EXPECT_LT(val, REAL_C(32.0));

		val = random_gaussian_range(REAL_C(32.0), REAL_C(-32.0));
		EXPECT_GE(val, REAL_C(-32.0));
		EXPECT_LT(val, REAL_C(32.0));

		ival32 = random32_triangle_range(-128, -64);
		EXPECT_GE(ival32, -128);
		EXPECT_LT(ival32, 64);

		ival32 = random32_triangle_range(128, -64);
		EXPECT_GE(ival32, -64);
		EXPECT_LT(ival32, 128);

		val = random_triangle_range(REAL_C(-128.0), REAL_C(-64.0));
		EXPECT_GE(val, REAL_C(-128.0));
		EXPECT_LT(val, REAL_C(-64.0));

		val = random_triangle_range(REAL_C(128.0), REAL_C(-64.0));
		EXPECT_GE(val, REAL_C(-64.0));
		EXPECT_LT(val, REAL_C(128.0));

		val32 = random32_weighted(10, weights);
		EXPECT_LT(val32, 10);

		val32 = random32_weighted(10, noweights);
		EXPECT_LT(val32, 10);

		EXPECT_EQ(random32_weighted(1, weights), 0);
	}

	return 0;
}

static void
test_random_declare(void) {
	ADD_TEST(random, distribution32);
	ADD_TEST(random, distribution64);
	ADD_TEST(random, distribution_real);
	ADD_TEST(random, threads);
	ADD_TEST(random, util);
}

static test_suite_t test_random_suite = {test_random_application,
                                         test_random_memory_system,
                                         test_random_config,
                                         test_random_declare,
                                         test_random_initialize,
                                         test_random_finalize,
                                         0};

#if BUILD_MONOLITHIC

int
test_random_run(void);

int
test_random_run(void) {
	test_suite = test_random_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_random_suite;
}

#endif
