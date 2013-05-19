/* test.h  -  Foundation test library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#pragma once

#include <foundation/foundation.h>


#if defined( TEST_COMPILE ) && TEST_COMPILE
#  ifdef __cplusplus
#  define TEST_EXTERN extern "C"
#  else
#  define TEST_EXTERN extern
#  endif
#  define TEST_API
#else
#  ifdef __cplusplus
#  define TEST_EXTERN extern "C"
#  define TEST_API extern "C"
#  else
#  define TEST_EXTERN extern
#  define TEST_API extern
#  endif
#endif

#define FAILED_TEST ((void*)(uintptr_t)1)

typedef void* (* test_fn)( void );

TEST_API void test_add_test( test_fn test, const char* group, const char* name );

#define _MAKE_TEST_FN( test_group, test_name ) FOUNDATION_PREPROCESSOR_JOIN( FOUNDATION_PREPROCESSOR_JOIN( test_group, test_name ), _fn )

#define DECLARE_TEST( test_group, test_name ) void* _MAKE_TEST_FN( test_group, test_name )( void )
#define ADD_TEST( test_group, test_name ) test_add_test( _MAKE_TEST_FN( test_group, test_name ), FOUNDATION_PREPROCESSOR_TOSTRING( test_group ), FOUNDATION_PREPROCESSOR_TOSTRING( test_name ) )

#define EXPECT_EQ( var, expect ) do { if( !((var) == (expect)) ) { log_warnf( WARNING_SUSPICIOUS, "Test failed, %s != %s (at %s:%u)", FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, __LINE__ ); return FAILED_TEST; } } while(0)
#define EXPECT_NE( var, expect ) do { if( ((var) == (expect)) ) { log_warnf( WARNING_SUSPICIOUS, "Test failed, %s == %s (at %s:%u)", FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, __LINE__ ); return FAILED_TEST; } } while(0)
#define EXPECT_GE( var, expect ) do { if( !((var) >= (expect)) ) { log_warnf( WARNING_SUSPICIOUS, "Test failed, %s < %s (at %s:%u)", FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, __LINE__ ); return FAILED_TEST; } } while(0)
#define EXPECT_GT( var, expect ) do { if( !((var) > (expect)) ) { log_warnf( WARNING_SUSPICIOUS, "Test failed, %s <= %s (at %s:%u)", FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, __LINE__ ); return FAILED_TEST; } } while(0)
#define EXPECT_LE( var, expect ) do { if( !((var) <= (expect)) ) { log_warnf( WARNING_SUSPICIOUS, "Test failed, %s > %s (at %s:%u)", FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, __LINE__ ); return FAILED_TEST; } } while(0)
#define EXPECT_LT( var, expect ) do { if( !((var) < (expect)) ) { log_warnf( WARNING_SUSPICIOUS, "Test failed, %s >= %s (at %s:%u)", FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, __LINE__ ); return FAILED_TEST; } } while(0)
#define EXPECT_TRUE( var ) do { if( !(var) ) { log_warnf( WARNING_SUSPICIOUS, "Test failed, %s is false (at %s:%u)", FOUNDATION_PREPROCESSOR_TOSTRING(var), __FILE__, __LINE__ ); return FAILED_TEST; } } while(0)
#define EXPECT_FALSE( var ) do { if( (var) ) { log_warnf( WARNING_SUSPICIOUS, "Test failed, %s is true (at %s:%u)", FOUNDATION_PREPROCESSOR_TOSTRING(var), __FILE__, __LINE__ ); return FAILED_TEST; } } while(0)

#define EXPECT_STREQ( var, expect ) do { if( !string_equal( (var), (expect) ) ) { log_warnf( WARNING_SUSPICIOUS, "Test failed, %s != %s ('%s', '%s') (at %s:%u)", FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), (var), (expect), __FILE__, __LINE__ ); return FAILED_TEST; } } while(0)

#define EXPECT_REALZERO( var ) do { if( !math_realzero(var) ) { log_warnf( WARNING_SUSPICIOUS, "Test failed, %s is not zero real (at %s:%u): %.6" STRING_FORMAT_REAL, FOUNDATION_PREPROCESSOR_TOSTRING(var), __FILE__, __LINE__, (real)((var)) ); return FAILED_TEST; } } while(0)
#define EXPECT_REALONE( var ) do { if( !math_realone(var) ) { log_warnf( WARNING_SUSPICIOUS, "Test failed, %s is not one real (at %s:%u): %.6" STRING_FORMAT_REAL, FOUNDATION_PREPROCESSOR_TOSTRING(var), __FILE__, __LINE__, (real)((var)) ); return FAILED_TEST; } } while(0)
#define EXPECT_REALNE( var, expect ) do { if( math_realeq( (var), (expect), 10 ) ) { log_warnf( WARNING_SUSPICIOUS, "Test failed, %s == %s real (at %s:%u): %.6" STRING_FORMAT_REAL " : %.6" STRING_FORMAT_REAL, FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, __LINE__, (real)((var)), (real)((expect)) ); return FAILED_TEST; } } while(0)
#define EXPECT_REALEQ( var, expect ) do { if( !math_realeq( (var), (expect), 10 ) ) { log_warnf( WARNING_SUSPICIOUS, "Test failed, %s != %s real (at %s:%u): %.6" STRING_FORMAT_REAL " : %.6" STRING_FORMAT_REAL, FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, __LINE__, (real)((var)), (real)((expect)) ); return FAILED_TEST; } } while(0)
#define EXPECT_REALEQULPS( var, expect, ulps ) do { if( !math_realeq( (var), (expect), (ulps) ) ) { log_warnf( WARNING_SUSPICIOUS, "Test failed, %s != %s real (at %s:%u): %.6" STRING_FORMAT_REAL " : %.6" STRING_FORMAT_REAL, FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, __LINE__, (real)((var)), (real)((expect)) ); return FAILED_TEST; } } while(0)

TEST_API void test_wait_for_threads_startup( const object_t* threads, unsigned int num_threads );
TEST_API void test_wait_for_threads_finish( const object_t* threads, unsigned int num_threads );
TEST_API void test_wait_for_threads_exit( const object_t* threads, unsigned int num_threads );
