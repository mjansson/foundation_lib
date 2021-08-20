/* uuid.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file uuid.h
\brief UUID

UUID generation (version 1, 3 and 4) and utility functions */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Predefined UUID namespace "dns" */
FOUNDATION_API const uuid_t UUID_DNS;

/*! Generate UUID based on time (and host id) - version 1.
\return Time-based UUID */
FOUNDATION_API uuid_t
uuid_generate_time(void);

/*! Generate UUID based on namespace and name - version 3.
\param ns Namespace
\param name Name
\param length Length of name
\return Name-based UUID */
FOUNDATION_API uuid_t
uuid_generate_name(const uuid_t ns, const char* name, size_t length);

/*! Generate UUID based on random numbers - version 4.
\return Random-based UUID */
FOUNDATION_API uuid_t
uuid_generate_random(void);

/*! Check if UUIDs are equal.
\param u0 First UUID
\param u1 Second UUID
\return true if equal, false if not */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uuid_equal(const uuid_t u0, const uuid_t u1);

/*! Make null UUID.
\return Null UUID */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uuid_t
uuid_null(void);

/*! Check if UUID is null.
\param uuid UUID
\return true if null, false if not */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uuid_is_null(const uuid_t uuid);

// Implementation

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uuid_equal(const uuid_t u0, const uuid_t u1) {
	return uint128_equal(u0, u1);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uuid_t
uuid_null(void) {
	return uint128_make(0, 0);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uuid_is_null(const uuid_t uuid) {
	return uint128_is_null(uuid);
}

#define uuid_make(w0, w1) uint128_make(w0, w1)
#define uuid_make_constant(w0, w1) \
	{                              \
		{ w0, w1 }                 \
	}
#define uuid_decl(a, b, c, d, e)                                                                               \
	uuid_make(((uint64_t)0x##c << 48ULL) | ((uint64_t)0x##b << 32ULL) | (uint64_t)0x##a,                       \
	          (((uint64_t)0x##e & 0xFF) << 56ULL) | (((uint64_t)0x##e & 0xFF00) << 40ULL) |                    \
	              (((uint64_t)0x##e & 0xFF0000) << 24ULL) | (((uint64_t)0x##e & 0xFF000000) << 8ULL) |         \
	              (((uint64_t)0x##e & 0xFF00000000) >> 8ULL) | (((uint64_t)0x##e & 0xFF0000000000) >> 24ULL) | \
	              (((uint64_t)0x##d & 0xFF) << 8ULL) | (((uint64_t)0x##d & 0xFF00) >> 8ULL))
#define uuid_decl_constant(a, b, c, d, e)                                                                       \
	uuid_make_constant(((uint64_t)0x##c << 48ULL) | ((uint64_t)0x##b << 32ULL) | (uint64_t)0x##a,               \
	                   (((uint64_t)0x##e & 0xFF) << 56ULL) | (((uint64_t)0x##e & 0xFF00) << 40ULL) |            \
	                       (((uint64_t)0x##e & 0xFF0000) << 24ULL) | (((uint64_t)0x##e & 0xFF000000) << 8ULL) | \
	                       (((uint64_t)0x##e & 0xFF00000000) >> 8ULL) |                                         \
	                       (((uint64_t)0x##e & 0xFF0000000000) >> 24ULL) | (((uint64_t)0x##d & 0xFF) << 8ULL) | \
	                       (((uint64_t)0x##d & 0xFF00) >> 8ULL))
#define uuid_decl_vals(a, b, c, d, e)                                                                  \
	uuid_make(((uint64_t)c << 48ULL) | ((uint64_t)b << 32ULL) | (uint64_t)a,                           \
	          (((uint64_t)e & 0xFF) << 56ULL) | (((uint64_t)e & 0xFF00) << 40ULL) |                    \
	              (((uint64_t)e & 0xFF0000) << 24ULL) | (((uint64_t)e & 0xFF000000) << 8ULL) |         \
	              (((uint64_t)e & 0xFF00000000) >> 8ULL) | (((uint64_t)e & 0xFF0000000000) >> 24ULL) | \
	              (((uint64_t)d & 0xFF) << 8ULL) | (((uint64_t)d & 0xFF00) >> 8ULL))
