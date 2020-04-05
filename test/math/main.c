/* main.c  -  Foundation math test  -  Public Domain  -  2013 Mattias Jansson
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

#if FOUNDATION_COMPILER_MSVC
// Test will do division by zero
#pragma warning(disable : 4723)
#endif

static application_t
test_math_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation math tests"));
	app.short_name = string_const(STRING_CONST("test_math"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_math_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_math_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_math_initialize(void) {
	return 0;
}

static void
test_math_finalize(void) {
}

DECLARE_TEST(math, constants) {
	EXPECT_GT(REAL_EPSILON, 0);
	EXPECT_LT(REAL_EPSILON, 1);
	EXPECT_GT(REAL_MAX, REAL_MIN);
	EXPECT_GT(REAL_MIN, 0);

	return 0;
}

DECLARE_TEST(math, comparison) {
	real testreal, refreal;
	real onereal = 1;
	real zeroreal = 0;

	testreal = REAL_C(42.42);
	refreal = testreal;

#if FOUNDATION_COMPILER_CLANG
// Yes, we want to compare floats
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#endif

	EXPECT_EQ(testreal, refreal);
	EXPECT_TRUE(math_real_eq(testreal, refreal, 0));
	EXPECT_TRUE(math_real_eqns(testreal, refreal, 0));
	EXPECT_FALSE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));

	testreal = math_real_dec(testreal, 10);
	EXPECT_NE(testreal, refreal);
	EXPECT_FALSE(math_real_eq(testreal, refreal, 0));
	EXPECT_FALSE(math_real_eqns(testreal, refreal, 0));
	EXPECT_TRUE(math_real_eq(testreal, refreal, 10));
	EXPECT_TRUE(math_real_eqns(testreal, refreal, 10));
	EXPECT_FALSE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));

	testreal = math_real_dec(testreal, 10);
	EXPECT_NE(testreal, refreal);
	EXPECT_FALSE(math_real_eq(testreal, refreal, 0));
	EXPECT_FALSE(math_real_eqns(testreal, refreal, 0));
	EXPECT_FALSE(math_real_eq(testreal, refreal, 10));
	EXPECT_FALSE(math_real_eqns(testreal, refreal, 10));
	EXPECT_FALSE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));

	testreal = math_real_inc(testreal, 20);
	EXPECT_EQ(testreal, refreal);
	EXPECT_TRUE(math_real_eq(testreal, refreal, 0));
	EXPECT_TRUE(math_real_eqns(testreal, refreal, 0));
	EXPECT_FALSE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));

	testreal = math_real_inc(testreal, 10);
	EXPECT_NE(testreal, refreal);
	EXPECT_FALSE(math_real_eq(testreal, refreal, 0));
	EXPECT_FALSE(math_real_eqns(testreal, refreal, 0));
	EXPECT_TRUE(math_real_eq(testreal, refreal, 10));
	EXPECT_TRUE(math_real_eqns(testreal, refreal, 10));
	EXPECT_FALSE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));

	testreal = math_real_inc(testreal, 10);
	EXPECT_NE(testreal, refreal);
	EXPECT_FALSE(math_real_eq(testreal, refreal, 0));
	EXPECT_FALSE(math_real_eqns(testreal, refreal, 0));
	EXPECT_FALSE(math_real_eq(testreal, refreal, 10));
	EXPECT_FALSE(math_real_eqns(testreal, refreal, 10));
	EXPECT_FALSE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));

	testreal = math_real_inc(0, 10);
	// EXPECT_NE(testreal, 0);
	EXPECT_FALSE(math_real_eq(testreal, 0, 0));
	EXPECT_FALSE(math_real_eqns(testreal, 0, 0));
	EXPECT_TRUE(math_real_eq(testreal, 0, 10));
	EXPECT_TRUE(math_real_eqns(testreal, 0, 10));
	EXPECT_TRUE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));
	EXPECT_TRUE(math_real_is_denormalized(testreal));

	testreal = math_real_inc(0, 100);
	// EXPECT_NE(testreal, 0);
	EXPECT_FALSE(math_real_eq(testreal, 0, 0));
	EXPECT_FALSE(math_real_eqns(testreal, 0, 0));
	EXPECT_FALSE(math_real_eq(testreal, 0, 10));
	EXPECT_FALSE(math_real_eqns(testreal, 0, 10));
	EXPECT_TRUE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));
	EXPECT_TRUE(math_real_is_denormalized(testreal));

	testreal = math_real_dec(0, 10);
	// EXPECT_NE(testreal, 0);
	EXPECT_FALSE(math_real_eq(testreal, 0, 0));
	EXPECT_FALSE(math_real_eqns(testreal, 0, 0));
	EXPECT_TRUE(math_real_eq(testreal, 0, 10));
	// EXPECT_TRUE( math_real_eqns( testreal, 0, 10 ) );
	EXPECT_TRUE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));
	EXPECT_TRUE(math_real_is_denormalized(testreal));

	testreal = math_real_dec(0, 100);
	// EXPECT_NE(testreal, 0);
	EXPECT_FALSE(math_real_eq(testreal, 0, 0));
	EXPECT_FALSE(math_real_eqns(testreal, 0, 0));
	EXPECT_FALSE(math_real_eq(testreal, 0, 10));
	EXPECT_FALSE(math_real_eqns(testreal, 0, 10));
	EXPECT_TRUE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));
	EXPECT_TRUE(math_real_is_denormalized(testreal));

	testreal = math_real_inc(0, 5);
	testreal = math_real_dec(testreal, 10);
	// EXPECT_NE(testreal, 0);
	EXPECT_FALSE(math_real_eq(testreal, 0, 0));
	EXPECT_FALSE(math_real_eqns(testreal, 0, 0));
	EXPECT_TRUE(math_real_eq(testreal, 0, 10));
	// EXPECT_TRUE( math_real_eqns( testreal, 0, 10 ) );
	EXPECT_TRUE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));

	testreal = math_real_dec(0, 5);
	testreal = math_real_inc(testreal, 10);
	// EXPECT_NE(testreal, 0);
	EXPECT_FALSE(math_real_eq(testreal, 0, 0));
	EXPECT_FALSE(math_real_eqns(testreal, 0, 0));
	EXPECT_TRUE(math_real_eq(testreal, 0, 10));
	EXPECT_TRUE(math_real_eqns(testreal, 0, 10));
	EXPECT_TRUE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));

	testreal = math_real_inc(0, -5);
	testreal = math_real_dec(testreal, -10);
	// EXPECT_NE(testreal, 0);
	EXPECT_FALSE(math_real_eq(testreal, 0, 0));
	EXPECT_FALSE(math_real_eqns(testreal, 0, 0));
	EXPECT_TRUE(math_real_eq(testreal, 0, 10));
	EXPECT_TRUE(math_real_eqns(testreal, 0, 10));
	EXPECT_TRUE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));

	testreal = math_real_dec(0, -5);
	testreal = math_real_inc(testreal, -10);
	// EXPECT_NE(testreal, 0);
	EXPECT_FALSE(math_real_eq(testreal, 0, 0));
	EXPECT_FALSE(math_real_eqns(testreal, 0, 0));
	EXPECT_TRUE(math_real_eq(testreal, 0, 10));
	// EXPECT_TRUE( math_real_eqns( testreal, 0, 10 ) );
	EXPECT_TRUE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));

	EXPECT_TRUE(math_real_is_nan(math_sqrt(REAL_C(-1.0))));
	EXPECT_TRUE(math_real_is_inf(onereal / zeroreal));
	EXPECT_TRUE(math_real_is_nan(-math_sqrt(REAL_C(-1.0))));
	EXPECT_TRUE(math_real_is_inf(-onereal / zeroreal));

	testreal = 1 / REAL_MAX;
	EXPECT_REALNE(testreal, 0);
	EXPECT_TRUE(math_real_is_denormalized(testreal));
	EXPECT_REALZERO(math_real_undenormalize(testreal));
	EXPECT_FALSE(math_real_is_denormalized(1));
	EXPECT_REALONE(math_real_undenormalize(1));

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic pop
#endif

	return 0;
}

DECLARE_TEST(math, trigonometry) {
	EXPECT_REALZERO(math_sin(0));
	EXPECT_REALEPSILONZERO(math_sin(REAL_PI));
	// EXPECT_REALEPSILONZERO(math_sin(REAL_TWOPI));
	EXPECT_REALONE(math_sin(REAL_HALFPI));

	EXPECT_REALONE(math_cos(0));
	EXPECT_REALONE(-math_cos(REAL_PI));
	EXPECT_REALONE(math_cos(REAL_TWOPI));
	EXPECT_REALEPSILONZERO(math_cos(REAL_HALFPI));

	EXPECT_REALONE(math_tan(REAL_HALFPI * REAL_C(0.5)));
	EXPECT_REALONE(-math_tan(REAL_HALFPI * REAL_C(0.5) * REAL_C(3.0)));

	EXPECT_REALZERO(math_asin(0));
	EXPECT_REALEQ(math_asin(1), REAL_HALFPI);
	EXPECT_REALEQ(math_asin(-1), -REAL_HALFPI);

	EXPECT_REALEQ(math_acos(-1), REAL_PI);
	EXPECT_REALEQ(math_acos(0), REAL_HALFPI);
	EXPECT_REALEPSILONZERO(math_acos(1));

	EXPECT_REALZERO(math_atan(0));
	EXPECT_REALEQ(math_atan(1), REAL_HALFPI * REAL_C(0.5));
	EXPECT_REALEQ(math_atan(-1), -REAL_HALFPI * REAL_C(0.5));

	EXPECT_REALZERO(math_atan2(0, 1));
	EXPECT_REALEQ(math_atan2(1, -1), REAL_HALFPI * REAL_C(0.5) * REAL_C(3.0));

	return 0;
}

DECLARE_TEST(math, squareroot) {
	EXPECT_REALEQ(math_sqrt(2), REAL_SQRT2);
	EXPECT_REALEQ(math_sqrt(3), REAL_SQRT3);

	EXPECT_REALEQ(math_rsqrt(2), 1 / REAL_SQRT2);
	EXPECT_REALEQ(math_rsqrt(3), 1 / REAL_SQRT3);

	return 0;
}

DECLARE_TEST(math, utility) {
	int i;

	EXPECT_REALONE(math_abs(1));
	EXPECT_REALONE(math_abs(-1));
	EXPECT_REALZERO(math_abs(0));
	EXPECT_REALEQ(math_abs(REAL_MAX), REAL_MAX);
	EXPECT_REALEQ(math_abs(-REAL_MAX), REAL_MAX);
	EXPECT_REALEQ(math_abs(REAL_MIN), REAL_MIN);
	EXPECT_REALEQ(math_abs(-REAL_MIN), REAL_MIN);

	EXPECT_REALZERO(math_mod(0, 1));
	EXPECT_REALZERO(math_mod(1, 1));
	EXPECT_REALZERO(math_mod(REAL_C(4.0), 1));
	EXPECT_REALZERO(math_mod(REAL_C(1023.0), 1));
	EXPECT_REALONE(math_mod(REAL_C(3.0), REAL_C(2.0)));
	EXPECT_REALONE(-math_mod(-REAL_C(3.0), -REAL_C(2.0)));

	EXPECT_EQ(math_floor(0), 0);
	EXPECT_EQ(math_floor(REAL_C(0.999)), 0);
	EXPECT_EQ(math_floor(REAL_C(-0.1)), -1);
	EXPECT_EQ(math_floor(REAL_C(42.5)), 42);

	EXPECT_EQ(math_ceil(0), 0);
	EXPECT_EQ(math_ceil(REAL_C(0.999)), 1);
	EXPECT_EQ(math_ceil(REAL_C(-0.1)), 0);
	EXPECT_EQ(math_ceil(REAL_C(42.5)), 43);
	EXPECT_EQ(math_ceil(REAL_C(42.45)), 43);

	EXPECT_EQ(math_floor64(0), 0);
	EXPECT_EQ(math_floor64(REAL_C(0.999)), 0);
	EXPECT_EQ(math_floor64(REAL_C(-0.1)), -1);
	EXPECT_EQ(math_floor64(REAL_C(42.5)), 42);

	EXPECT_EQ(math_ceil64(0), 0);
	EXPECT_EQ(math_ceil64(REAL_C(0.999)), 1);
	EXPECT_EQ(math_ceil64(REAL_C(-0.1)), 0);
	EXPECT_EQ(math_ceil64(REAL_C(42.5)), 43);
	EXPECT_EQ(math_ceil64(REAL_C(42.45)), 43);

	EXPECT_EQ(math_round(0), 0);
	EXPECT_EQ(math_round(REAL_C(0.999)), 1);
	EXPECT_EQ(math_round(REAL_C(-0.1)), 0);
	EXPECT_EQ(math_round(REAL_C(42.5)), 43);
	EXPECT_EQ(math_round(REAL_C(42.45)), 42);

	EXPECT_EQ(math_trunc(0), 0);
	EXPECT_EQ(math_trunc(REAL_C(0.999)), 0);
	EXPECT_EQ(math_trunc(REAL_C(-0.1)), 0);
	EXPECT_EQ(math_trunc(REAL_C(42.5)), 42);

	EXPECT_EQ(math_align_poweroftwo(2), 2);
	EXPECT_EQ(math_align_poweroftwo(3), 4);
	EXPECT_EQ(math_align_poweroftwo(4), 4);
	EXPECT_EQ(math_align_poweroftwo(33), 64);
	EXPECT_EQ(math_align_poweroftwo(134217729), 268435456);

	for (i = 1; i < 31; ++i) {
		EXPECT_TRUE(math_is_poweroftwo(math_align_poweroftwo((2U << i) - 1)));
		EXPECT_TRUE(math_is_poweroftwo(math_align_poweroftwo((2U << i))));
		EXPECT_TRUE(math_is_poweroftwo(math_align_poweroftwo((2U << i) + 1)));

		EXPECT_FALSE(math_is_poweroftwo((2U << i) - 1));
		EXPECT_TRUE(math_is_poweroftwo((2U << i)));
		EXPECT_FALSE(math_is_poweroftwo((2U << i) + 1));
	}

	EXPECT_EQ(math_align_up(1, 1), 1);
	EXPECT_EQ(math_align_up(1, 2), 2);
	EXPECT_EQ(math_align_up(17, 2), 18);
	EXPECT_EQ(math_align_up(43, 42), 84);

	EXPECT_REALZERO(math_smoothstep(0));
	EXPECT_REALONE(math_smoothstep(1));
	EXPECT_REALEQ(math_smoothstep(REAL_C(0.5)), REAL_C(0.5));

	EXPECT_REALZERO(math_smootherstep(0));
	EXPECT_REALONE(math_smootherstep(1));
	EXPECT_REALEQ(math_smootherstep(REAL_C(0.5)), REAL_C(0.5));

	EXPECT_REALZERO(math_lerp(0, 0, 1));
	EXPECT_REALZERO(math_lerp(1, 1, 0));
	EXPECT_REALONE(math_lerp(1, 0, 1));
	EXPECT_REALONE(math_lerp(0, 1, 0));
	EXPECT_REALEQ(math_lerp(REAL_C(0.5), 0, 1), REAL_C(0.5));
	EXPECT_REALEQ(math_lerp(REAL_C(0.5), 0, 1), REAL_C(0.5));

	EXPECT_REALZERO(math_unlerp(0, 0, 1));
	EXPECT_REALZERO(math_unlerp(1, 1, 0));
	EXPECT_REALONE(math_unlerp(1, 0, 1));
	EXPECT_REALONE(math_unlerp(0, 1, 0));
	EXPECT_REALEQ(math_unlerp(REAL_C(0.5), 0, 1), REAL_C(0.5));
	EXPECT_REALEQ(math_unlerp(REAL_C(0.5), 0, 1), REAL_C(0.5));

	EXPECT_REALONE(math_linear_remap(REAL_C(150.0), REAL_C(100.0), REAL_C(200.0), 0, REAL_C(2.0)));

	EXPECT_REALZERO(math_clamp(-1, 0, 1));
	EXPECT_REALZERO(math_clamp(0, 0, 1));
	EXPECT_REALEQ(math_clamp(REAL_C(0.5), 0, 1), REAL_C(0.5));
	EXPECT_REALONE(math_clamp(1, 0, 1));
	EXPECT_REALONE(math_clamp(REAL_C(2.0), 0, 1));

	return 0;
}

DECLARE_TEST(math, exponentials) {
	EXPECT_REALONE(math_exp(0));
	EXPECT_REALEQ(math_exp(1), REAL_E);

	EXPECT_REALONE(math_pow(1, 1));
	EXPECT_REALONE(math_pow(1, 0));
	EXPECT_REALONE(math_pow(REAL_C(3.0), 0));
	EXPECT_REALEQ(math_pow(REAL_SQRT2, REAL_C(2.0)), REAL_C(2.0));

	EXPECT_REALEQ(math_logn(REAL_C(2.0)), REAL_LOGN2);
	EXPECT_REALEQ(math_logn(REAL_C(10.0)), REAL_LOGN10);

	EXPECT_REALONE(math_log2(REAL_C(2.0)));
	EXPECT_REALEQ(math_log2(REAL_C(2.0) * REAL_C(2.0)), REAL_C(2.0));
	EXPECT_REALEQ(math_log2(REAL_C(2.0) * REAL_C(2.0) * REAL_C(2.0) * REAL_C(2.0)), REAL_C(4.0));

	return 0;
}

DECLARE_TEST(math, wrap) {
	int32_t min32, max32;
	int64_t min64, max64;

	EXPECT_EQ(math_inc_wrap_uint8(0xFE, 0, 0xFF), 0xFF);
	EXPECT_EQ(math_inc_wrap_uint8(0xFF, 0, 0xFF), 0);
	EXPECT_EQ(math_inc_wrap_uint8(1, 0, 1), 0);
	EXPECT_EQ(math_inc_wrap_uint8(42, 40, 42), 40);
	EXPECT_EQ(math_inc_wrap_uint8(42, 40, 43), 43);

	EXPECT_EQ(math_inc_wrap_uint16(0xFFFE, 0, 0xFFFF), 0xFFFF);
	EXPECT_EQ(math_inc_wrap_uint16(0xFFFF, 0, 0xFFFF), 0);
	EXPECT_EQ(math_inc_wrap_uint16(1, 0, 1), 0);
	EXPECT_EQ(math_inc_wrap_uint16(42, 40, 42), 40);
	EXPECT_EQ(math_inc_wrap_uint16(42, 40, 43), 43);

	EXPECT_EQ(math_inc_wrap_uint32(0xFFFFFFFE, 0, 0xFFFFFFFF), 0xFFFFFFFF);
	EXPECT_EQ(math_inc_wrap_uint32(0xFFFFFFFF, 0, 0xFFFFFFFF), 0);
	EXPECT_EQ(math_inc_wrap_uint32(1, 0, 1), 0);
	EXPECT_EQ(math_inc_wrap_uint32(42, 40, 42), 40);
	EXPECT_EQ(math_inc_wrap_uint32(42, 40, 43), 43);

	EXPECT_EQ(math_inc_wrap_uint64(0xFFFFFFFFFFFFFFFEULL, 0, 0xFFFFFFFFFFFFFFFFULL), 0xFFFFFFFFFFFFFFFFULL);
	EXPECT_EQ(math_inc_wrap_uint64(0xFFFFFFFFFFFFFFFFULL, 0, 0xFFFFFFFFFFFFFFFFULL), 0);
	EXPECT_EQ(math_inc_wrap_uint64(1, 0, 1), 0);
	EXPECT_EQ(math_inc_wrap_uint64(42, 40, 42), 40);
	EXPECT_EQ(math_inc_wrap_uint64(42, 40, 43), 43);

	EXPECT_EQ(math_inc_wrap_int8(126, -128, 127), 127);
	EXPECT_EQ(math_inc_wrap_int8(127, -128, 127), -128);
	EXPECT_EQ(math_inc_wrap_int8(1, 0, 1), 0);
	EXPECT_EQ(math_inc_wrap_int8(1, -1, 1), -1);
	EXPECT_EQ(math_inc_wrap_int8(-1, -1, 1), 0);
	EXPECT_EQ(math_inc_wrap_int8(42, 40, 42), 40);
	EXPECT_EQ(math_inc_wrap_int8(42, 40, 43), 43);

	EXPECT_EQ(math_inc_wrap_int16(32766, -32768, 32767), 32767);
	EXPECT_EQ(math_inc_wrap_int16(32767, -32768, 32767), -32768);
	EXPECT_EQ(math_inc_wrap_int16(1, 0, 1), 0);
	EXPECT_EQ(math_inc_wrap_int16(1, -1, 1), -1);
	EXPECT_EQ(math_inc_wrap_int16(-1, -1, 1), 0);
	EXPECT_EQ(math_inc_wrap_int16(42, 40, 42), 40);
	EXPECT_EQ(math_inc_wrap_int16(42, 40, 43), 43);

	min32 = (int32_t)0x80000000L;  //-2147483648
	max32 = (int32_t)0x7FFFFFFFL;  // 2147483647
	EXPECT_EQ(math_inc_wrap_int32(max32 - 1, min32, max32), max32);
	EXPECT_EQ(math_inc_wrap_int32(max32, min32, max32), min32);
	EXPECT_EQ(math_inc_wrap_int32(1, 0, 1), 0);
	EXPECT_EQ(math_inc_wrap_int32(1, -1, 1), -1);
	EXPECT_EQ(math_inc_wrap_int32(-1, -1, 1), 0);
	EXPECT_EQ(math_inc_wrap_int32(42, 40, 42), 40);
	EXPECT_EQ(math_inc_wrap_int32(42, 40, 43), 43);

	min64 = (int64_t)0x8000000000000000LL;  //-9223372036854775808
	max64 = (int64_t)0x7FFFFFFFFFFFFFFFLL;  // 9223372036854775807
	EXPECT_EQ(math_inc_wrap_int64(max64 - 1LL, min64, max64), max64);
	EXPECT_EQ(math_inc_wrap_int64(max64, min64, max64), min64);
	EXPECT_EQ(math_inc_wrap_int64(1, 0, 1), 0);
	EXPECT_EQ(math_inc_wrap_int64(1, -1, 1), -1);
	EXPECT_EQ(math_inc_wrap_int64(-1, -1, 1), 0);
	EXPECT_EQ(math_inc_wrap_int64(42, 40, 42), 40);
	EXPECT_EQ(math_inc_wrap_int64(42, 40, 43), 43);

	return 0;
}

static void
test_math_declare(void) {
	ADD_TEST(math, constants);
	ADD_TEST(math, comparison);
	ADD_TEST(math, trigonometry);
	ADD_TEST(math, squareroot);
	ADD_TEST(math, utility);
	ADD_TEST(math, exponentials);
	ADD_TEST(math, wrap);
}

static test_suite_t test_math_suite = {test_math_application,
                                       test_math_memory_system,
                                       test_math_config,
                                       test_math_declare,
                                       test_math_initialize,
                                       test_math_finalize,
                                       0};

#if BUILD_MONOLITHIC

int
test_math_run(void);

int
test_math_run(void) {
	test_suite = test_math_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_math_suite;
}

#endif
