/* test.h  -  Foundation test library  -  Public Domain  -  2013 Mattias Jansson
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

#pragma once

#include <foundation/foundation.h>

#if FOUNDATION_PLATFORM_WINDOWS
#include <foundation/windows.h>
#endif

#if defined(TEST_COMPILE) && TEST_COMPILE
#ifdef __cplusplus
#define TEST_EXTERN extern "C"
#else
#define TEST_EXTERN extern
#endif
#define TEST_API
#else
#ifdef __cplusplus
#define TEST_EXTERN extern "C"
#define TEST_API extern "C"
#else
#define TEST_EXTERN extern
#define TEST_API extern
#endif
#endif

#if FOUNDATION_COMPILER_CLANG
#if __has_warning("-Wdouble-promotion")
#pragma clang diagnostic ignored "-Wdouble-promotion"
#endif
#endif

#define FAILED_TEST ((void*)(uintptr_t)-1)

typedef void* (*test_fn)(void);

TEST_API void
test_add_test(test_fn test, const char* group_name, size_t group_length, const char* test_name, size_t test_length);

TEST_API int
test_run_all(void);

TEST_API FOUNDATION_NOINLINE void*
test_failed(void);

TEST_API FOUNDATION_NOINLINE void
test_prefail(void);

TEST_API FOUNDATION_NOINLINE void
test_event(event_t* event);

TEST_API void
test_set_fail_hook(void (*hook_fn)(void));

#if FOUNDATION_PLATFORM_IOS

TEST_API void
test_text_view_append(void* window, int tag, const char* msg, size_t length);

#endif

#define MAKE_TEST_FN(test_group, test_name) \
	FOUNDATION_PREPROCESSOR_JOIN(FOUNDATION_PREPROCESSOR_JOIN(test_group, test_name), _fn)

#define DECLARE_TEST(test_group, test_name) static FOUNDATION_NOINLINE void* MAKE_TEST_FN(test_group, test_name)(void)
#define ADD_TEST(test_group, test_name)                                                                            \
	test_add_test(MAKE_TEST_FN(test_group, test_name), STRING_CONST(FOUNDATION_PREPROCESSOR_TOSTRING(test_group)), \
	              STRING_CONST(FOUNDATION_PREPROCESSOR_TOSTRING(test_name)))
#define RETURN_FAILED_TEST return test_failed()

#define EXPECT_EQ(var, expect)                                                                                    \
	do {                                                                                                          \
		if (!((var) == (expect))) {                                                                               \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s != %s (at %s:%u)"),       \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__);                                                                                 \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_NE(var, expect)                                                                                    \
	do {                                                                                                          \
		if (((var) == (expect))) {                                                                                \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s == %s (at %s:%u)"),       \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__);                                                                                 \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_GE(var, expect)                                                                                    \
	do {                                                                                                          \
		if (!((var) >= (expect))) {                                                                               \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s < %s (at %s:%u)"),        \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__);                                                                                 \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_GT(var, expect)                                                                                    \
	do {                                                                                                          \
		if (!((var) > (expect))) {                                                                                \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s <= %s (at %s:%u)"),       \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__);                                                                                 \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_LE(var, expect)                                                                                    \
	do {                                                                                                          \
		if (!((var) <= (expect))) {                                                                               \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s > %s (at %s:%u)"),        \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__);                                                                                 \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_LT(var, expect)                                                                                    \
	do {                                                                                                          \
		if (!((var) < (expect))) {                                                                                \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s >= %s (at %s:%u)"),       \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__);                                                                                 \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_TRUE(var)                                                                                       \
	do {                                                                                                       \
		if (!(var)) {                                                                                          \
			test_prefail();                                                                                    \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s is false (at %s:%u)"), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), __FILE__, __LINE__);                             \
			RETURN_FAILED_TEST;                                                                                \
		}                                                                                                      \
	} while (0)
#define EXPECT_FALSE(var)                                                                                     \
	do {                                                                                                      \
		if ((var)) {                                                                                          \
			test_prefail();                                                                                   \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s is true (at %s:%u)"), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), __FILE__, __LINE__);                            \
			RETURN_FAILED_TEST;                                                                               \
		}                                                                                                     \
	} while (0)

#define EXPECT_EQ_MSG(var, expect, msg)                                                                           \
	do {                                                                                                          \
		if (!((var) == (expect))) {                                                                               \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s != %s (at %s:%u) : %s"),  \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__, msg);                                                                            \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_NE_MSG(var, expect, msg)                                                                           \
	do {                                                                                                          \
		if (((var) == (expect))) {                                                                                \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s == %s (at %s:%u) : %s"),  \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__, msg);                                                                            \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_GE_MSG(var, expect, msg)                                                                           \
	do {                                                                                                          \
		if (!((var) >= (expect))) {                                                                               \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s < %s (at %s:%u) : %s"),   \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__, msg);                                                                            \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_GT_MSG(var, expect, msg)                                                                           \
	do {                                                                                                          \
		if (!((var) > (expect))) {                                                                                \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s <= %s (at %s:%u) : %s"),  \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__, msg);                                                                            \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_LE_MSG(var, expect, msg)                                                                           \
	do {                                                                                                          \
		if (!((var) <= (expect))) {                                                                               \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s > %s (at %s:%u) : %s"),   \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__, msg);                                                                            \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_LT_MSG(var, expect, msg)                                                                           \
	do {                                                                                                          \
		if (!((var) < (expect))) {                                                                                \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s >= %s (at %s:%u) : %s"),  \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__, msg);                                                                            \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_TRUE_MSG(var, msg)                                                                                   \
	do {                                                                                                            \
		if (!(var)) {                                                                                               \
			test_prefail();                                                                                         \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s is false (at %s:%u) : %s"), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), __FILE__, __LINE__, msg);                             \
			RETURN_FAILED_TEST;                                                                                     \
		}                                                                                                           \
	} while (0)
#define EXPECT_FALSE_MSG(var, msg)                                                                                 \
	do {                                                                                                           \
		if ((var)) {                                                                                               \
			test_prefail();                                                                                        \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s is true (at %s:%u) : %s"), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), __FILE__, __LINE__, msg);                            \
			RETURN_FAILED_TEST;                                                                                    \
		}                                                                                                          \
	} while (0)

#define EXPECT_EQ_MSGFORMAT(var, expect, format, ...)                                                                 \
	do {                                                                                                              \
		if (!((var) == (expect))) {                                                                                   \
			test_prefail();                                                                                           \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s != %s (at %s:%u) : " format), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__,     \
			           __LINE__, __VA_ARGS__);                                                                        \
			RETURN_FAILED_TEST;                                                                                       \
		}                                                                                                             \
	} while (0)
#define EXPECT_NE_MSGFORMAT(var, expect, format, ...)                                                                 \
	do {                                                                                                              \
		if (((var) == (expect))) {                                                                                    \
			test_prefail();                                                                                           \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s == %s (at %s:%u) : " format), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__,     \
			           __LINE__, __VA_ARGS__);                                                                        \
			RETURN_FAILED_TEST;                                                                                       \
		}                                                                                                             \
	} while (0)
#define EXPECT_GE_MSGFORMAT(var, expect, format, ...)                                                                \
	do {                                                                                                             \
		if (!((var) >= (expect))) {                                                                                  \
			test_prefail();                                                                                          \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s < %s (at %s:%u) : " format), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__,    \
			           __LINE__, __VA_ARGS__);                                                                       \
			RETURN_FAILED_TEST;                                                                                      \
		}                                                                                                            \
	} while (0)
#define EXPECT_GT_MSGFORMAT(var, expect, format, ...)                                                                 \
	do {                                                                                                              \
		if (!((var) > (expect))) {                                                                                    \
			test_prefail();                                                                                           \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s <= %s (at %s:%u) : " format), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__,     \
			           __LINE__, __VA_ARGS__);                                                                        \
			RETURN_FAILED_TEST;                                                                                       \
		}                                                                                                             \
	} while (0)
#define EXPECT_LE_MSGFORMAT(var, expect, format, ...)                                                                \
	do {                                                                                                             \
		if (!((var) <= (expect))) {                                                                                  \
			test_prefail();                                                                                          \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s > %s (at %s:%u) : " format), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__,    \
			           __LINE__, __VA_ARGS__);                                                                       \
			RETURN_FAILED_TEST;                                                                                      \
		}                                                                                                            \
	} while (0)
#define EXPECT_LT_MSGFORMAT(var, expect, format, ...)                                                                 \
	do {                                                                                                              \
		if (!((var) < (expect))) {                                                                                    \
			test_prefail();                                                                                           \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE, STRING_CONST("Test failed, %s >= %s (at %s:%u) : " format), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__,     \
			           __LINE__, __VA_ARGS__);                                                                        \
			RETURN_FAILED_TEST;                                                                                       \
		}                                                                                                             \
	} while (0)
#define EXPECT_TRUE_MSGFORMAT(var, format, ...)                                                 \
	do {                                                                                        \
		if (!(var)) {                                                                           \
			test_prefail();                                                                     \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                       \
			           STRING_CONST("Test failed, %s is false (at %s:%u) : " format),           \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), __FILE__, __LINE__, __VA_ARGS__); \
			RETURN_FAILED_TEST;                                                                 \
		}                                                                                       \
	} while (0)
#define EXPECT_FALSE_MSGFORMAT(var, format, ...)                                                \
	do {                                                                                        \
		if ((var)) {                                                                            \
			test_prefail();                                                                     \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                       \
			           STRING_CONST("Test failed, %s is true (at %s:%u) : " format),            \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), __FILE__, __LINE__, __VA_ARGS__); \
			RETURN_FAILED_TEST;                                                                 \
		}                                                                                       \
	} while (0)

#define EXPECT_TYPEEQ(var, expect, type, fmt)                                                                         \
	do {                                                                                                              \
		type lvar_ = (type)var;                                                                                       \
		type rvar_ = (type)expect;                                                                                    \
		if (!(lvar_ == rvar_)) {                                                                                      \
			test_prefail();                                                                                           \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                             \
			           STRING_CONST("Test failed, %s != %s (%" fmt " != %" fmt ") (at %s:%u)"),                       \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), lvar_, rvar_, \
			           __FILE__, __LINE__);                                                                           \
			RETURN_FAILED_TEST;                                                                                       \
		}                                                                                                             \
	} while (0)
#define EXPECT_TYPENE(var, expect, type, fmt)                                                                         \
	do {                                                                                                              \
		type lvar_ = (type)var;                                                                                       \
		type rvar_ = (type)expect;                                                                                    \
		if (!(lvar_ != rvar_)) {                                                                                      \
			test_prefail();                                                                                           \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                             \
			           STRING_CONST("Test failed, %s == %s (%" fmt " == %" fmt ") (at %s:%u)"),                       \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), lvar_, rvar_, \
			           __FILE__, __LINE__);                                                                           \
			RETURN_FAILED_TEST;                                                                                       \
		}                                                                                                             \
	} while (0)
#define EXPECT_TYPEGE(var, expect, type, fmt)                                                                         \
	do {                                                                                                              \
		type lvar_ = (type)var;                                                                                       \
		type rvar_ = (type)expect;                                                                                    \
		if (!(lvar_ >= rvar_)) {                                                                                      \
			test_prefail();                                                                                           \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                             \
			           STRING_CONST("Test failed, %s < %s (%" fmt " < %" fmt ") (at %s:%u)"),                         \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), lvar_, rvar_, \
			           __FILE__, __LINE__);                                                                           \
			RETURN_FAILED_TEST;                                                                                       \
		}                                                                                                             \
	} while (0)
#define EXPECT_TYPEGT(var, expect, type, fmt)                                                                         \
	do {                                                                                                              \
		type lvar_ = (type)var;                                                                                       \
		type rvar_ = (type)expect;                                                                                    \
		if (!(lvar_ > rvar_)) {                                                                                       \
			test_prefail();                                                                                           \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                             \
			           STRING_CONST("Test failed, %s <= %s (%" fmt " <= %" fmt ") (at %s:%u)"),                       \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), lvar_, rvar_, \
			           __FILE__, __LINE__);                                                                           \
			RETURN_FAILED_TEST;                                                                                       \
		}                                                                                                             \
	} while (0)
#define EXPECT_TYPELE(var, expect, type, fmt)                                                                         \
	do {                                                                                                              \
		type lvar_ = (type)var;                                                                                       \
		type rvar_ = (type)expect;                                                                                    \
		if (!(lvar_ <= rvar_)) {                                                                                      \
			test_prefail();                                                                                           \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                             \
			           STRING_CONST("Test failed, %s > %s (%" fmt " > %" fmt ") (at %s:%u)"),                         \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), lvar_, rvar_, \
			           __FILE__, __LINE__);                                                                           \
			RETURN_FAILED_TEST;                                                                                       \
		}                                                                                                             \
	} while (0)
#define EXPECT_TYPELT(var, expect, type, fmt)                                                                         \
	do {                                                                                                              \
		type lvar_ = (type)var;                                                                                       \
		type rvar_ = (type)expect;                                                                                    \
		if (!(lvar_ < rvar_)) {                                                                                       \
			test_prefail();                                                                                           \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                             \
			           STRING_CONST("Test failed, %s >= %s (%" fmt " >= %" fmt ") (at %s:%u)"),                       \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), lvar_, rvar_, \
			           __FILE__, __LINE__);                                                                           \
			RETURN_FAILED_TEST;                                                                                       \
		}                                                                                                             \
	} while (0)

#define EXPECT_INTEQ(var, expect) EXPECT_TYPEEQ(var, expect, int, "d")
#define EXPECT_INTNE(var, expect) EXPECT_TYPENE(var, expect, int, "d")
#define EXPECT_INTGE(var, expect) EXPECT_TYPEGE(var, expect, int, "d")
#define EXPECT_INTGT(var, expect) EXPECT_TYPEGT(var, expect, int, "d")
#define EXPECT_INTLE(var, expect) EXPECT_TYPELE(var, expect, int, "d")
#define EXPECT_INTLT(var, expect) EXPECT_TYPELT(var, expect, int, "d")

#define EXPECT_UINTEQ(var, expect) EXPECT_TYPEEQ(var, expect, unsigned int, "u")
#define EXPECT_UINTNE(var, expect) EXPECT_TYPENE(var, expect, unsigned int, "u")
#define EXPECT_UINTGE(var, expect) EXPECT_TYPEGE(var, expect, unsigned int, "u")
#define EXPECT_UINTGT(var, expect) EXPECT_TYPEGT(var, expect, unsigned int, "u")
#define EXPECT_UINTLE(var, expect) EXPECT_TYPELE(var, expect, unsigned int, "u")
#define EXPECT_UINTLT(var, expect) EXPECT_TYPELT(var, expect, unsigned int, "u")

#define EXPECT_INT64EQ(var, expect) EXPECT_TYPEEQ(var, expect, int64_t, PRId64)
#define EXPECT_INT64NE(var, expect) EXPECT_TYPENE(var, expect, int64_t, PRId64)
#define EXPECT_INT64GE(var, expect) EXPECT_TYPEGE(var, expect, int64_t, PRId64)
#define EXPECT_INT64GT(var, expect) EXPECT_TYPEGT(var, expect, int64_t, PRId64)
#define EXPECT_INT64LE(var, expect) EXPECT_TYPELE(var, expect, int64_t, PRId64)
#define EXPECT_INT64LT(var, expect) EXPECT_TYPELT(var, expect, int64_t, PRId64)

#define EXPECT_UINT64EQ(var, expect) EXPECT_TYPEEQ(var, expect, uint64_t, PRIu64)
#define EXPECT_UINT64NE(var, expect) EXPECT_TYPENE(var, expect, uint64_t, PRIu64)
#define EXPECT_UINT64GE(var, expect) EXPECT_TYPEGE(var, expect, uint64_t, PRIu64)
#define EXPECT_UINT64GT(var, expect) EXPECT_TYPEGT(var, expect, uint64_t, PRIu64)
#define EXPECT_UINT64LE(var, expect) EXPECT_TYPELE(var, expect, uint64_t, PRIu64)
#define EXPECT_UINT64LT(var, expect) EXPECT_TYPELT(var, expect, uint64_t, PRIu64)

#define EXPECT_SIZEEQ(var, expect) EXPECT_TYPEEQ(var, expect, size_t, PRIsize)
#define EXPECT_SIZENE(var, expect) EXPECT_TYPENE(var, expect, size_t, PRIsize)
#define EXPECT_SIZEGE(var, expect) EXPECT_TYPEGE(var, expect, size_t, PRIsize)
#define EXPECT_SIZEGT(var, expect) EXPECT_TYPEGT(var, expect, size_t, PRIsize)
#define EXPECT_SIZELE(var, expect) EXPECT_TYPELE(var, expect, size_t, PRIsize)
#define EXPECT_SIZELT(var, expect) EXPECT_TYPELT(var, expect, size_t, PRIsize)

#define EXPECT_TICKEQ(var, expect) EXPECT_TYPEEQ(var, expect, tick_t, PRItick)
#define EXPECT_TICKNE(var, expect) EXPECT_TYPENE(var, expect, tick_t, PRItick)
#define EXPECT_TICKGE(var, expect) EXPECT_TYPEGE(var, expect, tick_t, PRItick)
#define EXPECT_TICKGT(var, expect) EXPECT_TYPEGT(var, expect, tick_t, PRItick)
#define EXPECT_TICKLE(var, expect) EXPECT_TYPELE(var, expect, tick_t, PRItick)
#define EXPECT_TICKLT(var, expect) EXPECT_TYPELT(var, expect, tick_t, PRItick)

#define EXPECT_HASHEQ(var, expect) EXPECT_TYPEEQ(var, expect, hash_t, PRIhash)
#define EXPECT_HASHNE(var, expect) EXPECT_TYPENE(var, expect, hash_t, PRIhash)
#define EXPECT_HASHGE(var, expect) EXPECT_TYPEGE(var, expect, hash_t, PRIhash)
#define EXPECT_HASHGT(var, expect) EXPECT_TYPEGT(var, expect, hash_t, PRIhash)
#define EXPECT_HASHLE(var, expect) EXPECT_TYPELE(var, expect, hash_t, PRIhash)
#define EXPECT_HASHLT(var, expect) EXPECT_TYPELT(var, expect, hash_t, PRIhash)

#define EXPECT_PTREQ(var, expect) EXPECT_TYPEEQ(var, expect, uintptr_t, PRIfixPTR)
#define EXPECT_PTRNE(var, expect) EXPECT_TYPENE(var, expect, uintptr_t, PRIfixPTR)

#define EXPECT_STRINGEQ(var, expect)                                                                                \
	do {                                                                                                            \
		string_t lhs = (var);                                                                                       \
		string_const_t rhs = (expect);                                                                              \
		if (!string_equal(STRING_ARGS(lhs), STRING_ARGS(rhs))) {                                                    \
			test_prefail();                                                                                         \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                           \
			           STRING_CONST("Test failed, %s != %s ('%.*s':%" PRIsize ", '%.*s':%" PRIsize ") (at %s:%u)"), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect),             \
			           STRING_FORMAT(lhs), lhs.length, STRING_FORMAT(rhs), rhs.length, __FILE__, __LINE__);         \
			RETURN_FAILED_TEST;                                                                                     \
		}                                                                                                           \
	} while (0)
#define EXPECT_STRINGEQ_MSG(var, expect, msg)                                                                        \
	do {                                                                                                             \
		string_t lhs = (var);                                                                                        \
		string_const_t rhs = (expect);                                                                               \
		if (!string_equal(STRING_ARGS(lhs), STRING_ARGS(rhs))) {                                                     \
			test_prefail();                                                                                          \
			log_errorf(                                                                                              \
			    HASH_TEST, ERROR_INTERNAL_FAILURE,                                                                   \
			    STRING_CONST("Test failed, %s != %s ('%.*s':%" PRIsize ", '%.*s':%" PRIsize ") (at %s:%u) : %s"),    \
			    FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), STRING_FORMAT(lhs), \
			    lhs.length, STRING_FORMAT(rhs), rhs.length, __FILE__, __LINE__, msg);                                \
			RETURN_FAILED_TEST;                                                                                      \
		}                                                                                                            \
	} while (0)
#define EXPECT_STRINGEQ_MSGFORMAT(var, expect, format, ...)                                                            \
	do {                                                                                                               \
		string_t lhs = (var);                                                                                          \
		string_const_t rhs = (expect);                                                                                 \
		if (!string_equal(STRING_ARGS(lhs), STRING_ARGS(rhs))) {                                                       \
			test_prefail();                                                                                            \
			log_errorf(                                                                                                \
			    HASH_TEST, ERROR_INTERNAL_FAILURE,                                                                     \
			    STRING_CONST("Test failed, %s != %s ('%.*s':%" PRIsize ", '%.*s':%" PRIsize ") (at %s:%u) : " format), \
			    FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), STRING_FORMAT(lhs),   \
			    lhs.length, STRING_FORMAT(rhs), rhs.length, __FILE__, __LINE__, __VA_ARGS__);                          \
			RETURN_FAILED_TEST;                                                                                        \
		}                                                                                                              \
	} while (0)

#define EXPECT_STRINGNE(var, expect)                                                                                \
	do {                                                                                                            \
		string_t lhs = (var);                                                                                       \
		string_const_t rhs = (expect);                                                                              \
		if (string_equal(STRING_ARGS((lhs)), STRING_ARGS((rhs)))) {                                                 \
			test_prefail();                                                                                         \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                           \
			           STRING_CONST("Test failed, %s == %s ('%.*s':%" PRIsize ", '%.*s':%" PRIsize ") (at %s:%u)"), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect),             \
			           STRING_FORMAT(lhs), lhs.length, STRING_FORMAT(rhs), rhs.length, __FILE__, __LINE__);         \
			RETURN_FAILED_TEST;                                                                                     \
		}                                                                                                           \
	} while (0)
#define EXPECT_STRINGNE_MSG(var, expect, msg)                                                                        \
	do {                                                                                                             \
		string_t lhs = (var);                                                                                        \
		string_const_t rhs = (expect);                                                                               \
		if (string_equal(STRING_ARGS(lhs), STRING_ARGS(rhs))) {                                                      \
			test_prefail();                                                                                          \
			log_errorf(                                                                                              \
			    HASH_TEST, ERROR_INTERNAL_FAILURE,                                                                   \
			    STRING_CONST("Test failed, %s == %s ('%.*s':%" PRIsize ", '%.*s':%" PRIsize ") (at %s:%u) : %s"),    \
			    FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), STRING_FORMAT(lhs), \
			    lhs.length, STRING_FORMAT(rhs), rhs.length, __FILE__, __LINE__, msg);                                \
			RETURN_FAILED_TEST;                                                                                      \
		}                                                                                                            \
	} while (0)
#define EXPECT_STRINGNE_MSGFORMAT(var, expect, format, ...)                                                            \
	do {                                                                                                               \
		string_t lhs = (var);                                                                                          \
		string_const_t rhs = (expect);                                                                                 \
		if (string_equal(STRING_ARGS((lhs)), STRING_ARGS((rhs)))) {                                                    \
			test_prefail();                                                                                            \
			log_errorf(                                                                                                \
			    HASH_TEST, ERROR_INTERNAL_FAILURE,                                                                     \
			    STRING_CONST("Test failed, %s == %s ('%.*s':%" PRIsize ", '%.*s':%" PRIsize ") (at %s:%u) : " format), \
			    FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), STRING_FORMAT(lhs),   \
			    lhs.length, STRING_FORMAT(rhs), rhs.length, __FILE__, __LINE__, __VA_ARGS__);                          \
			RETURN_FAILED_TEST;                                                                                        \
		}                                                                                                              \
	} while (0)

#define EXPECT_CONSTSTRINGEQ(var, expect)                                                                           \
	do {                                                                                                            \
		string_const_t lhs = (var);                                                                                 \
		string_const_t rhs = (expect);                                                                              \
		if (!string_equal(STRING_ARGS(lhs), STRING_ARGS(rhs))) {                                                    \
			test_prefail();                                                                                         \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                           \
			           STRING_CONST("Test failed, %s != %s ('%.*s':%" PRIsize ", '%.*s':%" PRIsize ") (at %s:%u)"), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect),             \
			           STRING_FORMAT(lhs), lhs.length, STRING_FORMAT(rhs), rhs.length, __FILE__, __LINE__);         \
			RETURN_FAILED_TEST;                                                                                     \
		}                                                                                                           \
	} while (0)
#define EXPECT_CONSTSTRINGEQ_MSG(var, expect, msg)                                                                   \
	do {                                                                                                             \
		string_const_t lhs = (var);                                                                                  \
		string_const_t rhs = (expect);                                                                               \
		if (!string_equal(STRING_ARGS(lhs), STRING_ARGS(rhs))) {                                                     \
			test_prefail();                                                                                          \
			log_errorf(                                                                                              \
			    HASH_TEST, ERROR_INTERNAL_FAILURE,                                                                   \
			    STRING_CONST("Test failed, %s != %s ('%.*s':%" PRIsize ", '%.*s':%" PRIsize ") (at %s:%u) : %s"),    \
			    FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), STRING_FORMAT(lhs), \
			    lhs.length, STRING_FORMAT(rhs), rhs.length, __FILE__, __LINE__, msg);                                \
			RETURN_FAILED_TEST;                                                                                      \
		}                                                                                                            \
	} while (0)
#define EXPECT_CONSTSTRINGEQ_MSGFORMAT(var, expect, format, ...)                                                       \
	do {                                                                                                               \
		string_const_t lhs = (var);                                                                                    \
		string_const_t rhs = (expect);                                                                                 \
		if (!string_equal(STRING_ARGS(lhs), STRING_ARGS(rhs))) {                                                       \
			test_prefail();                                                                                            \
			log_errorf(                                                                                                \
			    HASH_TEST, ERROR_INTERNAL_FAILURE,                                                                     \
			    STRING_CONST("Test failed, %s != %s ('%.*s':%" PRIsize ", '%.*s':%" PRIsize ") (at %s:%u) : " format), \
			    FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), STRING_FORMAT(lhs),   \
			    lhs.length, STRING_FORMAT(rhs), rhs.length, __FILE__, __LINE__, __VA_ARGS__);                          \
			RETURN_FAILED_TEST;                                                                                        \
		}                                                                                                              \
	} while (0)

#define EXPECT_CONSTSTRINGNE(var, expect)                                                                           \
	do {                                                                                                            \
		string_const_t lhs = (var);                                                                                 \
		string_const_t rhs = (expect);                                                                              \
		if (string_equal(STRING_ARGS((lhs)), STRING_ARGS((rhs)))) {                                                 \
			test_prefail();                                                                                         \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                           \
			           STRING_CONST("Test failed, %s == %s ('%.*s':%" PRIsize ", '%.*s':%" PRIsize ") (at %s:%u)"), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect),             \
			           STRING_FORMAT(lhs), lhs.length, STRING_FORMAT(rhs), rhs.length, __FILE__, __LINE__);         \
			RETURN_FAILED_TEST;                                                                                     \
		}                                                                                                           \
	} while (0)
#define EXPECT_CONSTSTRINGNE_MSG(var, expect, msg)                                                                   \
	do {                                                                                                             \
		string_const_t lhs = (var);                                                                                  \
		string_const_t rhs = (expect);                                                                               \
		if (string_equal(STRING_ARGS(lhs), STRING_ARGS(rhs))) {                                                      \
			test_prefail();                                                                                          \
			log_errorf(                                                                                              \
			    HASH_TEST, ERROR_INTERNAL_FAILURE,                                                                   \
			    STRING_CONST("Test failed, %s == %s ('%.*s':%" PRIsize ", '%.*s':%" PRIsize ") (at %s:%u) : %s"),    \
			    FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), STRING_FORMAT(lhs), \
			    lhs.length, STRING_FORMAT(rhs), rhs.length, __FILE__, __LINE__, msg);                                \
			RETURN_FAILED_TEST;                                                                                      \
		}                                                                                                            \
	} while (0)
#define EXPECT_CONSTSTRINGNE_MSGFORMAT(var, expect, format, ...)                                                       \
	do {                                                                                                               \
		string_const_t lhs = (var);                                                                                    \
		string_const_t rhs = (expect);                                                                                 \
		if (string_equal(STRING_ARGS((lhs)), STRING_ARGS((rhs)))) {                                                    \
			test_prefail();                                                                                            \
			log_errorf(                                                                                                \
			    HASH_TEST, ERROR_INTERNAL_FAILURE,                                                                     \
			    STRING_CONST("Test failed, %s == %s ('%.*s':%" PRIsize ", '%.*s':%" PRIsize ") (at %s:%u) : " format), \
			    FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), STRING_FORMAT(lhs),   \
			    lhs.length, STRING_FORMAT(rhs), rhs.length, __FILE__, __LINE__, __VA_ARGS__);                          \
			RETURN_FAILED_TEST;                                                                                        \
		}                                                                                                              \
	} while (0)

#define EXPECT_REALZERO(var)                                                                          \
	do {                                                                                              \
		real rval_ = (real)var;                                                                       \
		if (!math_real_is_zero(rval_)) {                                                              \
			test_prefail();                                                                           \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                             \
			           STRING_CONST("Test failed, %s is not zero real (at %s:%u): %.8" PRIreal),      \
			           FOUNDATION_PREPROCESSOR_TOSTRING(rval_), __FILE__, __LINE__, (double)((var))); \
			RETURN_FAILED_TEST;                                                                       \
		}                                                                                             \
	} while (0)
#define EXPECT_REALEPSILONZERO(var)                                                                                  \
	do {                                                                                                             \
		real rval_ = (real)var;                                                                                      \
		if (!math_real_is_epsilon_zero(rval_)) {                                                                     \
			test_prefail();                                                                                          \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                            \
			           STRING_CONST("Test failed, %s is not zero real (at %s:%u): %.8" PRIreal " (%.8" PRIreal ")"), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), __FILE__, __LINE__, (double)rval_, REAL_EPSILON);      \
			RETURN_FAILED_TEST;                                                                                      \
		}                                                                                                            \
	} while (0)
#define EXPECT_REALONE(var)                                                                       \
	do {                                                                                          \
		real rval_ = (real)var;                                                                   \
		if (!math_real_is_one(rval_)) {                                                           \
			test_prefail();                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                         \
			           STRING_CONST("Test failed, %s is not one real (at %s:%u): %.8" PRIreal),   \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), __FILE__, __LINE__, (double)rval_); \
			RETURN_FAILED_TEST;                                                                   \
		}                                                                                         \
	} while (0)
#define EXPECT_REALNE(var, expect)                                                                                \
	do {                                                                                                          \
		real rval_ = (real)var;                                                                                   \
		real eval_ = (real)expect;                                                                                \
		if (math_real_eq(rval_, eval_, 10)) {                                                                     \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                         \
			           STRING_CONST("Test failed, %s == %s real (at %s:%u): %.8" PRIreal " : %.8" PRIreal),       \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__, (double)rval_, (double)eval_);                                                   \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_REALEQ(var, expect)                                                                                \
	do {                                                                                                          \
		real rval_ = (real)var;                                                                                   \
		real eval_ = (real)expect;                                                                                \
		if (!math_real_eq(rval_, eval_, 10)) {                                                                    \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                         \
			           STRING_CONST("Test failed, %s != %s real (at %s:%u): %.8" PRIreal " : %.8" PRIreal),       \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__, (double)rval_, (double)eval_);                                                   \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_REALEQULPS(var, expect, ulps)                                                                      \
	do {                                                                                                          \
		real rval_ = (real)var;                                                                                   \
		real eval_ = (real)expect;                                                                                \
		if (!math_real_eq(rval_, eval_, (ulps))) {                                                                \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                         \
			           STRING_CONST("Test failed, %s != %s real (at %s:%u): %.8" PRIreal " : %.8" PRIreal),       \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__, (double)rval_, (double)eval_);                                                   \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_REALGE(var, expect) EXPECT_TYPEGE(var, expect, real, ".6" PRIreal)
#define EXPECT_REALGT(var, expect) EXPECT_TYPEGT(var, expect, real, ".6" PRIreal)
#define EXPECT_REALLE(var, expect) EXPECT_TYPELE(var, expect, real, ".6" PRIreal)
#define EXPECT_REALLT(var, expect) EXPECT_TYPELT(var, expect, real, ".6" PRIreal)

#define EXPECT_DBLZERO(var)                                                                  \
	do {                                                                                     \
		double rval_ = (double)var;                                                          \
		if (!math_double_is_zero(rval_)) {                                                   \
			test_prefail();                                                                  \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                    \
			           STRING_CONST("Test failed, %s is not zero double (at %s:%u): %.10f"), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(rval_), __FILE__, __LINE__, rval_);  \
			RETURN_FAILED_TEST;                                                              \
		}                                                                                    \
	} while (0)
#define EXPECT_DBLEPSILONZERO(var)                                                                     \
	do {                                                                                               \
		double rval_ = (double)var;                                                                    \
		if (!math_double_is_epsilon_zero(rval_)) {                                                     \
			test_prefail();                                                                            \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                              \
			           STRING_CONST("Test failed, %s is not zero double (at %s:%u): %.10f (%.10f)"),   \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), __FILE__, __LINE__, rval_, DBL_EPSILON); \
			RETURN_FAILED_TEST;                                                                        \
		}                                                                                              \
	} while (0)
#define EXPECT_DBLONE(var)                                                                 \
	do {                                                                                   \
		double rval_ = (double)var;                                                        \
		if (!math_double_is_one(rval_)) {                                                  \
			test_prefail();                                                                \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                  \
			           STRING_CONST("Test failed, %s is not one double (at %s:%u): %10f"), \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), __FILE__, __LINE__, rval_);  \
			RETURN_FAILED_TEST;                                                            \
		}                                                                                  \
	} while (0)
#define EXPECT_DBLNE(var, expect)                                                                                 \
	do {                                                                                                          \
		double rval_ = (double)var;                                                                               \
		double eval_ = (double)expect;                                                                            \
		if (math_double_eq(rval_, eval_, 10)) {                                                                   \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                         \
			           STRING_CONST("Test failed, %s == %s double (at %s:%u): %.10f : %.10f"),                    \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__, rval_, eval_);                                                                   \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_DBLEQ(var, expect)                                                                                 \
	do {                                                                                                          \
		double rval_ = (double)var;                                                                               \
		double eval_ = (double)expect;                                                                            \
		if (!math_double_eq(rval_, eval_, 10)) {                                                                  \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                         \
			           STRING_CONST("Test failed, %s != %s double (at %s:%u): %.10f : %.10f"),                    \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__, rval_, eval_);                                                                   \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_DBLEQULPS(var, expect, ulps)                                                                       \
	do {                                                                                                          \
		double rval_ = (double)var;                                                                               \
		double eval_ = (double)expect;                                                                            \
		if (!math_double_eq(rval_, eval_, (ulps))) {                                                              \
			test_prefail();                                                                                       \
			log_errorf(HASH_TEST, ERROR_INTERNAL_FAILURE,                                                         \
			           STRING_CONST("Test failed, %s != %s double (at %s:%u): %.10f : %.10f"),                    \
			           FOUNDATION_PREPROCESSOR_TOSTRING(var), FOUNDATION_PREPROCESSOR_TOSTRING(expect), __FILE__, \
			           __LINE__, rval_, eval_);                                                                   \
			RETURN_FAILED_TEST;                                                                                   \
		}                                                                                                         \
	} while (0)
#define EXPECT_DBLGE(var, expect) EXPECT_TYPEGE(var, expect, double, ".10f")
#define EXPECT_DBLGT(var, expect) EXPECT_TYPEGT(var, expect, double, ".10f")
#define EXPECT_DBLLE(var, expect) EXPECT_TYPELE(var, expect, double, ".10f")
#define EXPECT_DBLLT(var, expect) EXPECT_TYPELT(var, expect, double, ".10f")

// No inline to make sure compiler does not inline and reorder instructions
TEST_API void FOUNDATION_NOINLINE
test_wait_for_threads_startup(const thread_t* threads, size_t threads_count);

TEST_API void FOUNDATION_NOINLINE
test_wait_for_threads_finish(const thread_t* threads, size_t threads_count);

TEST_API void FOUNDATION_NOINLINE
test_wait_for_threads_join(thread_t* threads, size_t threads_count);

TEST_API void FOUNDATION_NOINLINE
test_exception_handler(const char*, size_t length) FOUNDATION_ATTRIBUTE(noreturn);

TEST_API int FOUNDATION_NOINLINE
test_error_handler(error_level_t level, error_t err);

TEST_API bool FOUNDATION_NOINLINE
test_should_terminate(void);

TEST_API bool FOUNDATION_NOINLINE
test_have_focus(void);

TEST_API void FOUNDATION_NOINLINE
test_set_suitable_working_directory(void);

TEST_API void FOUNDATION_NOINLINE
test_load_config(json_handler_fn handler);

typedef struct test_suite_t {
	application_t (*application)(void);
	memory_system_t (*memory_system)(void);
	foundation_config_t (*config)(void);
	void (*declare)(void);
	int (*initialize)(void);
	void (*finalize)(void);
	void (*event)(event_t*);
} test_suite_t;

TEST_API test_suite_t test_suite;
