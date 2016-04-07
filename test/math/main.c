/* main.c  -  Foundation math test  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#include <foundation/foundation.h>
#include <test/test.h>

#if FOUNDATION_COMPILER_MSVC
//Test will do division by zero
#  pragma warning( disable : 4723 )
#endif

static application_t
test_math_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation math tests"));
	app.short_name = string_const(STRING_CONST("test_math"));
	app.company = string_const(STRING_CONST("Rampant Pixels"));
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
	EXPECT_GT(REAL_EPSILON, REAL_ZERO);
	EXPECT_LT(REAL_EPSILON, REAL_ONE);
	EXPECT_GT(REAL_MAX, REAL_MIN);
	EXPECT_GT(REAL_MIN, REAL_ZERO);

	return 0;
}

DECLARE_TEST(math, comparison) {
	real testreal, refreal;
	real onereal = REAL_ONE;
	real zeroreal = REAL_ZERO;

	testreal = REAL_C(42.42);
	refreal = testreal;

#if FOUNDATION_COMPILER_CLANG
// Yes, we want to compare floats
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wfloat-equal"
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

	testreal = math_real_inc(REAL_ZERO, 10);
	//EXPECT_NE(testreal, REAL_ZERO);
	EXPECT_FALSE(math_real_eq(testreal, REAL_ZERO, 0));
	EXPECT_FALSE(math_real_eqns(testreal, REAL_ZERO, 0));
	EXPECT_TRUE(math_real_eq(testreal, REAL_ZERO, 10));
	EXPECT_TRUE(math_real_eqns(testreal, REAL_ZERO, 10));
	EXPECT_TRUE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));
	EXPECT_TRUE(math_real_is_denormalized(testreal));

	testreal = math_real_inc(REAL_ZERO, 100);
	//EXPECT_NE(testreal, REAL_ZERO);
	EXPECT_FALSE(math_real_eq(testreal, REAL_ZERO, 0));
	EXPECT_FALSE(math_real_eqns(testreal, REAL_ZERO, 0));
	EXPECT_FALSE(math_real_eq(testreal, REAL_ZERO, 10));
	EXPECT_FALSE(math_real_eqns(testreal, REAL_ZERO, 10));
	EXPECT_TRUE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));
	EXPECT_TRUE(math_real_is_denormalized(testreal));

	testreal = math_real_dec(REAL_ZERO, 10);
	//EXPECT_NE(testreal, REAL_ZERO);
	EXPECT_FALSE(math_real_eq(testreal, REAL_ZERO, 0));
	EXPECT_FALSE(math_real_eqns(testreal, REAL_ZERO, 0));
	EXPECT_TRUE(math_real_eq(testreal, REAL_ZERO, 10));
	//EXPECT_TRUE( math_real_eqns( testreal, REAL_ZERO, 10 ) );
	EXPECT_TRUE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));
	EXPECT_TRUE(math_real_is_denormalized(testreal));

	testreal = math_real_dec(REAL_ZERO, 100);
	//EXPECT_NE(testreal, REAL_ZERO);
	EXPECT_FALSE(math_real_eq(testreal, REAL_ZERO, 0));
	EXPECT_FALSE(math_real_eqns(testreal, REAL_ZERO, 0));
	EXPECT_FALSE(math_real_eq(testreal, REAL_ZERO, 10));
	EXPECT_FALSE(math_real_eqns(testreal, REAL_ZERO, 10));
	EXPECT_TRUE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));
	EXPECT_TRUE(math_real_is_denormalized(testreal));

	testreal = math_real_inc(REAL_ZERO, 5);
	testreal = math_real_dec(testreal, 10);
	//EXPECT_NE(testreal, REAL_ZERO);
	EXPECT_FALSE(math_real_eq(testreal, REAL_ZERO, 0));
	EXPECT_FALSE(math_real_eqns(testreal, REAL_ZERO, 0));
	EXPECT_TRUE(math_real_eq(testreal, REAL_ZERO, 10));
	//EXPECT_TRUE( math_real_eqns( testreal, REAL_ZERO, 10 ) );
	EXPECT_TRUE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));

	testreal = math_real_dec(REAL_ZERO, 5);
	testreal = math_real_inc(testreal, 10);
	//EXPECT_NE(testreal, REAL_ZERO);
	EXPECT_FALSE(math_real_eq(testreal, REAL_ZERO, 0));
	EXPECT_FALSE(math_real_eqns(testreal, REAL_ZERO, 0));
	EXPECT_TRUE(math_real_eq(testreal, REAL_ZERO, 10));
	EXPECT_TRUE(math_real_eqns(testreal, REAL_ZERO, 10));
	EXPECT_TRUE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));

	testreal = math_real_inc(REAL_ZERO, -5);
	testreal = math_real_dec(testreal, -10);
	//EXPECT_NE(testreal, REAL_ZERO);
	EXPECT_FALSE(math_real_eq(testreal, REAL_ZERO, 0));
	EXPECT_FALSE(math_real_eqns(testreal, REAL_ZERO, 0));
	EXPECT_TRUE(math_real_eq(testreal, REAL_ZERO, 10));
	EXPECT_TRUE(math_real_eqns(testreal, REAL_ZERO, 10));
	EXPECT_TRUE(math_real_is_zero(testreal));
	EXPECT_FALSE(math_real_is_one(testreal));
	EXPECT_FALSE(math_real_is_nan(testreal));
	EXPECT_FALSE(math_real_is_inf(testreal));
	EXPECT_FALSE(math_real_is_uninitialized(testreal));
	EXPECT_TRUE(math_real_is_finite(testreal));

	testreal = math_real_dec(REAL_ZERO, -5);
	testreal = math_real_inc(testreal, -10);
	//EXPECT_NE(testreal, REAL_ZERO);
	EXPECT_FALSE(math_real_eq(testreal, REAL_ZERO, 0));
	EXPECT_FALSE(math_real_eqns(testreal, REAL_ZERO, 0));
	EXPECT_TRUE(math_real_eq(testreal, REAL_ZERO, 10));
	//EXPECT_TRUE( math_real_eqns( testreal, REAL_ZERO, 10 ) );
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

	testreal = REAL_ONE / REAL_MAX;
	EXPECT_REALNE(testreal, REAL_ZERO);
	EXPECT_TRUE(math_real_is_denormalized(testreal));
	EXPECT_REALZERO(math_real_undenormalize(testreal));
	EXPECT_FALSE(math_real_is_denormalized(REAL_ONE));
	EXPECT_REALONE(math_real_undenormalize(REAL_ONE));

#if FOUNDATION_COMPILER_CLANG
#  pragma clang diagnostic pop
#endif

	return 0;
}

DECLARE_TEST(math, trigonometry) {
	EXPECT_REALZERO(math_sin(REAL_ZERO));
	EXPECT_REALEPSILONZERO(math_sin(REAL_PI));
	EXPECT_REALEPSILONZERO(math_sin(REAL_TWOPI));
	EXPECT_REALONE(math_sin(REAL_HALFPI));

	EXPECT_REALONE(math_cos(REAL_ZERO));
	EXPECT_REALONE(-math_cos(REAL_PI));
	EXPECT_REALONE(math_cos(REAL_TWOPI));
	EXPECT_REALEPSILONZERO(math_cos(REAL_HALFPI));

	EXPECT_REALONE(math_tan(REAL_HALFPI * REAL_HALF));
	EXPECT_REALONE(-math_tan(REAL_HALFPI * REAL_HALF * REAL_THREE));

	EXPECT_REALZERO(math_asin(REAL_ZERO));
	EXPECT_REALEQ(math_asin(REAL_ONE), REAL_HALFPI);
	EXPECT_REALEQ(math_asin(-REAL_ONE), -REAL_HALFPI);

	EXPECT_REALEQ(math_acos(-REAL_ONE), REAL_PI);
	EXPECT_REALEQ(math_acos(REAL_ZERO), REAL_HALFPI);
	EXPECT_REALEPSILONZERO(math_acos(REAL_ONE));

	EXPECT_REALZERO(math_atan(REAL_ZERO));
	EXPECT_REALEQ(math_atan(REAL_ONE), REAL_HALFPI * REAL_HALF);
	EXPECT_REALEQ(math_atan(-REAL_ONE), -REAL_HALFPI * REAL_HALF);

	EXPECT_REALZERO(math_atan2(REAL_ZERO, REAL_ONE));
	EXPECT_REALEQ(math_atan2(REAL_ONE, -REAL_ONE), REAL_HALFPI * REAL_HALF * REAL_THREE);

	return 0;
}

DECLARE_TEST(math, squareroot) {
	EXPECT_REALEQ(math_sqrt(2), REAL_SQRT2);
	EXPECT_REALEQ(math_sqrt(3), REAL_SQRT3);

	EXPECT_REALEQ(math_rsqrt(2), REAL_ONE / REAL_SQRT2);
	EXPECT_REALEQ(math_rsqrt(3), REAL_ONE / REAL_SQRT3);

	return 0;
}

DECLARE_TEST(math, utility) {
	int i;

	EXPECT_REALONE(math_abs(REAL_ONE));
	EXPECT_REALONE(math_abs(-REAL_ONE));
	EXPECT_REALZERO(math_abs(REAL_ZERO));
	EXPECT_REALEQ(math_abs(REAL_MAX), REAL_MAX);
	EXPECT_REALEQ(math_abs(-REAL_MAX), REAL_MAX);
	EXPECT_REALEQ(math_abs(REAL_MIN), REAL_MIN);
	EXPECT_REALEQ(math_abs(-REAL_MIN), REAL_MIN);

	EXPECT_REALZERO(math_mod(REAL_ZERO, REAL_ONE));
	EXPECT_REALZERO(math_mod(REAL_ONE, REAL_ONE));
	EXPECT_REALZERO(math_mod(REAL_C(4.0), REAL_ONE));
	EXPECT_REALZERO(math_mod(REAL_C(1023.0), REAL_ONE));
	EXPECT_REALONE(math_mod(REAL_THREE, REAL_TWO));
	EXPECT_REALONE(-math_mod(-REAL_THREE, -REAL_TWO));

	EXPECT_EQ(math_floor(REAL_ZERO), 0);
	EXPECT_EQ(math_floor(REAL_C(0.999)), 0);
	EXPECT_EQ(math_floor(REAL_C(-0.1)), -1);
	EXPECT_EQ(math_floor(REAL_C(42.5)), 42);

	EXPECT_EQ(math_ceil(REAL_ZERO), 0);
	EXPECT_EQ(math_ceil(REAL_C(0.999)), 1);
	EXPECT_EQ(math_ceil(REAL_C(-0.1)), 0);
	EXPECT_EQ(math_ceil(REAL_C(42.5)), 43);
	EXPECT_EQ(math_ceil(REAL_C(42.45)), 43);

	EXPECT_EQ(math_floor64(REAL_ZERO), 0);
	EXPECT_EQ(math_floor64(REAL_C(0.999)), 0);
	EXPECT_EQ(math_floor64(REAL_C(-0.1)), -1);
	EXPECT_EQ(math_floor64(REAL_C(42.5)), 42);

	EXPECT_EQ(math_ceil64(REAL_ZERO), 0);
	EXPECT_EQ(math_ceil64(REAL_C(0.999)), 1);
	EXPECT_EQ(math_ceil64(REAL_C(-0.1)), 0);
	EXPECT_EQ(math_ceil64(REAL_C(42.5)), 43);
	EXPECT_EQ(math_ceil64(REAL_C(42.45)), 43);

	EXPECT_EQ(math_round(REAL_ZERO), 0);
	EXPECT_EQ(math_round(REAL_C(0.999)), 1);
	EXPECT_EQ(math_round(REAL_C(-0.1)), 0);
	EXPECT_EQ(math_round(REAL_C(42.5)), 43);
	EXPECT_EQ(math_round(REAL_C(42.45)), 42);

	EXPECT_EQ(math_trunc(REAL_ZERO), 0);
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

	EXPECT_REALZERO(math_smoothstep(REAL_ZERO));
	EXPECT_REALONE(math_smoothstep(REAL_ONE));
	EXPECT_REALEQ(math_smoothstep(REAL_HALF), REAL_HALF);

	EXPECT_REALZERO(math_smootherstep(REAL_ZERO));
	EXPECT_REALONE(math_smootherstep(REAL_ONE));
	EXPECT_REALEQ(math_smootherstep(REAL_HALF), REAL_HALF);

	EXPECT_REALZERO(math_lerp(REAL_ZERO, REAL_ZERO, REAL_ONE));
	EXPECT_REALZERO(math_lerp(REAL_ONE, REAL_ONE, REAL_ZERO));
	EXPECT_REALONE(math_lerp(REAL_ONE, REAL_ZERO, REAL_ONE));
	EXPECT_REALONE(math_lerp(REAL_ZERO, REAL_ONE, REAL_ZERO));
	EXPECT_REALEQ(math_lerp(REAL_HALF, REAL_ZERO, REAL_ONE), REAL_HALF);
	EXPECT_REALEQ(math_lerp(REAL_HALF, REAL_ZERO, REAL_ONE), REAL_HALF);

	EXPECT_REALZERO(math_unlerp(REAL_ZERO, REAL_ZERO, REAL_ONE));
	EXPECT_REALZERO(math_unlerp(REAL_ONE, REAL_ONE, REAL_ZERO));
	EXPECT_REALONE(math_unlerp(REAL_ONE, REAL_ZERO, REAL_ONE));
	EXPECT_REALONE(math_unlerp(REAL_ZERO, REAL_ONE, REAL_ZERO));
	EXPECT_REALEQ(math_unlerp(REAL_HALF, REAL_ZERO, REAL_ONE), REAL_HALF);
	EXPECT_REALEQ(math_unlerp(REAL_HALF, REAL_ZERO, REAL_ONE), REAL_HALF);

	EXPECT_REALONE(math_linear_remap(REAL_C(150.0), REAL_C(100.0), REAL_C(200.0), REAL_ZERO, REAL_TWO));

	EXPECT_REALZERO(math_clamp(-REAL_ONE, REAL_ZERO, REAL_ONE));
	EXPECT_REALZERO(math_clamp(REAL_ZERO, REAL_ZERO, REAL_ONE));
	EXPECT_REALEQ(math_clamp(REAL_HALF, REAL_ZERO, REAL_ONE), REAL_HALF);
	EXPECT_REALONE(math_clamp(REAL_ONE, REAL_ZERO, REAL_ONE));
	EXPECT_REALONE(math_clamp(REAL_TWO, REAL_ZERO, REAL_ONE));

	return 0;
}

DECLARE_TEST(math, exponentials) {
	EXPECT_REALONE(math_exp(REAL_ZERO));
	EXPECT_REALEQ(math_exp(REAL_ONE), REAL_E);

	EXPECT_REALONE(math_pow(REAL_ONE, REAL_ONE));
	EXPECT_REALONE(math_pow(REAL_ONE, REAL_ZERO));
	EXPECT_REALONE(math_pow(REAL_THREE, REAL_ZERO));
	EXPECT_REALEQ(math_pow(REAL_SQRT2, REAL_TWO), REAL_TWO);

	EXPECT_REALEQ(math_logn(REAL_TWO), REAL_LOGN2);
	EXPECT_REALEQ(math_logn(REAL_C(10.0)), REAL_LOGN10);

	EXPECT_REALONE(math_log2(REAL_TWO));
	EXPECT_REALEQ(math_log2(REAL_TWO * REAL_TWO), REAL_TWO);
	EXPECT_REALEQ(math_log2(REAL_TWO * REAL_TWO * REAL_TWO * REAL_TWO), REAL_C(4.0));

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

	EXPECT_EQ(math_inc_wrap_uint64(0xFFFFFFFFFFFFFFFEULL, 0, 0xFFFFFFFFFFFFFFFFULL),
	          0xFFFFFFFFFFFFFFFFULL);
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

	min32 = (int32_t)0x80000000L;//-2147483648
	max32 = (int32_t)0x7FFFFFFFL;// 2147483647
	EXPECT_EQ(math_inc_wrap_int32(max32 - 1, min32, max32), max32);
	EXPECT_EQ(math_inc_wrap_int32(max32, min32, max32), min32);
	EXPECT_EQ(math_inc_wrap_int32(1, 0, 1), 0);
	EXPECT_EQ(math_inc_wrap_int32(1, -1, 1), -1);
	EXPECT_EQ(math_inc_wrap_int32(-1, -1, 1), 0);
	EXPECT_EQ(math_inc_wrap_int32(42, 40, 42), 40);
	EXPECT_EQ(math_inc_wrap_int32(42, 40, 43), 43);

	min64 = (int64_t)0x8000000000000000LL;//-9223372036854775808
	max64 = (int64_t)0x7FFFFFFFFFFFFFFFLL;// 9223372036854775807
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

static test_suite_t test_math_suite = {
	test_math_application,
	test_math_memory_system,
	test_math_config,
	test_math_declare,
	test_math_initialize,
	test_math_finalize
};

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
