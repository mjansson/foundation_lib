/* bits.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file bits.h
\brief Bit manipulation and byte order utility functions

Bit manipulation and byte order utility functions. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Swap byte order, 16 bit.
\param arg Value
\return    Byte-order swapped value */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint16_t
byteorder_swap16(uint16_t arg);

/*! Swap byte order, 32 bit.
\param arg Value
\return    Byte-order swapped value */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint32_t
byteorder_swap32(uint32_t arg);

/*! Swap byte order, 64 bit.
\param arg Value
\return    Byte-order swapped value */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint64_t
byteorder_swap64(uint64_t arg);

/*! Swap byte order, generic data.
\param buffer Pointer to value, swapped in-place
\param size   Number of bytes in data */
static FOUNDATION_FORCEINLINE FOUNDATION_PURECALL void
byteorder_swap(void* buffer, const size_t size);

/*! Convert from 16-bit native byte order to big endian byte order
(no-op for big endian systems).
\param arg Value
\return    Big endian value */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint16_t
byteorder_bigendian16(uint16_t arg);

/*! Convert from 32-bit native byte order to big endian byte order
(no-op for big endian systems).
\param arg Value
\return    Big endian value */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint32_t
byteorder_bigendian32(uint32_t arg);

/*! Convert from 64-bit native byte order to big endian byte order
(no-op for big endian systems).
\param arg Value
\return    Big endian value */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint64_t
byteorder_bigendian64(uint64_t arg);

/*! Convert generic data from native byte order to big endian
byte order (no-op for big endian systems).
\param buffer Pointer to value, converted in-place to big endian
\param size   Number of bytes in data */
static FOUNDATION_FORCEINLINE FOUNDATION_PURECALL void
byteorder_bigendian(void* buffer, const size_t size);

/*! Convert from 16-bit native byte order to little endian
byte order (no-op for little endian systems).
\param arg Value
\return    Little endian value */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint16_t
byteorder_littleendian16(uint16_t arg);

/*! Convert from 32-bit native byte order to little endian
byte order (no-op for little endian systems).
\param arg Value
\return    Little endian value */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint32_t
byteorder_littleendian32(uint32_t arg);

/*! Convert from 64-bit native byte order to little endian
byte order (no-op for little endian systems).
\param arg Value
\return    Little endian value */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint64_t
byteorder_littleendian64(uint64_t arg);

/*! Convert generic data from native byte order to little endian
byte order (no-op for little endian systems)
\param buffer Pointer to value, converted in-place to little endian
\param size   Number of bytes in data */
static FOUNDATION_FORCEINLINE FOUNDATION_PURECALL void
byteorder_littleendian(void* buffer, const size_t size);

// Implementations

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint16_t
byteorder_swap16(uint16_t arg) {
#if (FOUNDATION_COMPILER_GCC && (FOUNDATION_GCC_VERSION >= 40800)) || FOUNDATION_COMPILER_CLANG
	return __builtin_bswap16(arg);
#elif FOUNDATION_COMPILER_MSVC
#if _MSC_VER >= 1310
	return _byteswap_ushort(arg);
#else
	typedef union {
		uint16_t u16;
		uint8_t u8[2];
	} u16cast;
	u16cast in, out;
	in.u16 = arg;
	out.u8[0] = in.u8[1];
	out.u8[1] = in.u8[0];
	return out.u16;
#endif
#else
	typedef union {
		uint16_t u16;
		uint8_t u8[2];
	} u16cast;
	u16cast in = {.u16 = arg};
	u16cast out = {.u8[0] = in.u8[1], .u8[1] = in.u8[0]};
	return out.u16;
#endif
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint32_t
byteorder_swap32(uint32_t arg) {
#if FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __builtin_bswap32(arg);
#elif FOUNDATION_COMPILER_INTEL
	/*lint -save -e732 */
	return _bswap(arg);
	/*lint -restore */
#elif FOUNDATION_COMPILER_MSVC
#if _MSC_VER >= 1310
	return _byteswap_ulong(arg);
#else
	typedef union {
		uint32_t u32;
		uint8_t u8[4];
	} u32cast;
	u32cast in, out;
	in.u32 = arg;
	out.u8[0] = in.u8[3];
	out.u8[1] = in.u8[2];
	out.u8[2] = in.u8[1];
	out.u8[3] = in.u8[0];
	return out.u32;
#endif
#else
	typedef union {
		uint32_t u32;
		uint8_t u8[4];
	} u32cast;
	u32cast in = {.u32 = arg};
	u32cast out = {.u8[0] = in.u8[3], .u8[1] = in.u8[2], .u8[2] = in.u8[1], .u8[3] = in.u8[0]};
	return out.u32;
#endif
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint64_t
byteorder_swap64(uint64_t arg) {
#if FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
	return __builtin_bswap64(arg);
#elif FOUNDATION_COMPILER_INTEL
	/*lint -save -e732 */
	return _bswap64(arg);
	/*lint -restore */
#elif FOUNDATION_COMPILER_MSVC
#if _MSC_VER >= 1310
	return _byteswap_uint64(arg);
#else
	typedef union {
		uint64_t u64;
		uint8_t u8[8];
	} u64cast;
	u64cast in, out;
	in.u64 = arg;
	out.u8[0] = in.u8[7];
	out.u8[1] = in.u8[6];
	out.u8[2] = in.u8[5];
	out.u8[3] = in.u8[4];
	out.u8[4] = in.u8[3];
	out.u8[5] = in.u8[2];
	out.u8[6] = in.u8[1];
	out.u8[7] = in.u8[0];
	return out.u64;
#endif
#else
	typedef union {
		uint64_t u64;
		uint8_t u8[8];
	} u64cast;
	u64cast in = {.u64 = arg};
	u64cast out = {.u8[0] = in.u8[7],
	               .u8[1] = in.u8[6],
	               .u8[2] = in.u8[5],
	               .u8[3] = in.u8[4],
	               .u8[4] = in.u8[3],
	               .u8[5] = in.u8[2],
	               .u8[6] = in.u8[1],
	               .u8[7] = in.u8[0]};
	return out.u64;
#endif
}

static FOUNDATION_FORCEINLINE FOUNDATION_PURECALL void
byteorder_swap(void* buffer, const size_t size) {
	size_t i, num;
	char* src = (char*)buffer;
	char* dst = src + (size - 1);
	for (i = 0, num = size / 2; i < num; ++i, --dst, ++src) {
		char tmp = *dst;
		*dst = *src;
		*src = tmp;
	}
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint16_t
byteorder_bigendian16(uint16_t arg) {
#if FOUNDATION_ARCH_ENDIAN_BIG
	return arg;
#else
	return byteorder_swap16(arg);
#endif
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint32_t
byteorder_bigendian32(uint32_t arg) {
#if FOUNDATION_ARCH_ENDIAN_BIG
	return arg;
#else
	return byteorder_swap32(arg);
#endif
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint64_t
byteorder_bigendian64(uint64_t arg) {
#if FOUNDATION_ARCH_ENDIAN_BIG
	return arg;
#else
	return byteorder_swap64(arg);
#endif
}

static FOUNDATION_FORCEINLINE FOUNDATION_PURECALL void
byteorder_bigendian(void* buffer, const size_t size) {
#if FOUNDATION_ARCH_ENDIAN_LITTLE
	byteorder_swap(buffer, size);
#else
	FOUNDATION_UNUSED(buffer);
	FOUNDATION_UNUSED(size);
#endif
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint16_t
byteorder_littleendian16(uint16_t arg) {
#if FOUNDATION_ARCH_ENDIAN_LITTLE
	return arg;
#else
	return byteorder_swap16(arg);
#endif
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint32_t
byteorder_littleendian32(uint32_t arg) {
#if FOUNDATION_ARCH_ENDIAN_LITTLE
	return arg;
#else
	return byteorder_swap32(arg);
#endif
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint64_t
byteorder_littleendian64(uint64_t arg) {
#if FOUNDATION_ARCH_ENDIAN_LITTLE
	return arg;
#else
	return byteorder_swap64(arg);
#endif
}

static FOUNDATION_FORCEINLINE FOUNDATION_PURECALL void
byteorder_littleendian(void* buffer, const size_t size) {
#if FOUNDATION_ARCH_ENDIAN_BIG
	byteorder_swap(buffer, size);
#else
	FOUNDATION_UNUSED(buffer);
	FOUNDATION_UNUSED(size);
#endif
}
