/* platform.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file platform.h
\brief Platform abstractions

This file unifies platform definitions and data types across supported platforms
and compilers. If you need to override platform or architecture, predefine one of
<code>FOUNDATION_PLATFORM_[...]</code> and <code>FOUNDATION_ARCH_[...]</code> to 1

All preprocessor macros are designed to be always defined and used by value, i.e
a check should be performed like <code>\#if FOUNDATION_PLATFORM_[...]</code> rather
than <code>\#ifdef FOUNDATION_PLATFORM_[...]</code>, since this solves the potential
problem of typos in preprocessor checks (the <code>\#if</code> test will most likely
catch the typo with an not defined error, whereas the <code>\#ifdef</code> macro
will just resolve to false)

Use the macros defined in this file for platform/arch definitions, attributes and
thread local storage to ensure maximum portability across supported platforms */

/*lint -e717 We use do {} while(0) constructs in macros deliberately */

#if !defined(FOUNDATION_COMPILE)
#define FOUNDATION_COMPILE 0
#endif

#if !defined(BUILD_DYNAMIC_LINK)
#define BUILD_DYNAMIC_LINK 0
#endif

// Platforms
#define FOUNDATION_PLATFORM_ANDROID 0
#define FOUNDATION_PLATFORM_BSD 0
#define FOUNDATION_PLATFORM_IOS 0
#define FOUNDATION_PLATFORM_IOS_SIMULATOR 0
#define FOUNDATION_PLATFORM_LINUX 0
#define FOUNDATION_PLATFORM_LINUX_RASPBERRYPI 0
#define FOUNDATION_PLATFORM_MACOS 0
#define FOUNDATION_PLATFORM_WINDOWS 0
#define FOUNDATION_PLATFORM_TIZEN 0

// Platform traits and groups
#define FOUNDATION_PLATFORM_APPLE 0
#define FOUNDATION_PLATFORM_POSIX 0

#define FOUNDATION_PLATFORM_FAMILY_MOBILE 0
#define FOUNDATION_PLATFORM_FAMILY_DESKTOP 0
#define FOUNDATION_PLATFORM_FAMILY_CONSOLE 0

// Architectures
#define FOUNDATION_ARCH_ARM 0
#define FOUNDATION_ARCH_ARM5 0
#define FOUNDATION_ARCH_ARM6 0
#define FOUNDATION_ARCH_ARM7 0
#define FOUNDATION_ARCH_ARM8 0
#define FOUNDATION_ARCH_ARM_64 0
#define FOUNDATION_ARCH_ARM8_64 0
#define FOUNDATION_ARCH_X86 0
#define FOUNDATION_ARCH_X86_64 0
#define FOUNDATION_ARCH_PPC 0
#define FOUNDATION_ARCH_PPC_64 0
#define FOUNDATION_ARCH_IA64 0
#define FOUNDATION_ARCH_MIPS 0
#define FOUNDATION_ARCH_MIPS_64 0
#define FOUNDATION_ARCH_GENERIC 0

// Architecture details
#define FOUNDATION_ARCH_SSE2 0
#define FOUNDATION_ARCH_SSE3 0
#define FOUNDATION_ARCH_SSE4 0
#define FOUNDATION_ARCH_SSE4_FMA3 0
#define FOUNDATION_ARCH_AVX 0
#define FOUNDATION_ARCH_NEON 0
#define FOUNDATION_ARCH_THUMB 0

#define FOUNDATION_ARCH_ENDIAN_LITTLE 0
#define FOUNDATION_ARCH_ENDIAN_BIG 0

// Compilers
#define FOUNDATION_COMPILER_CLANG 0
#define FOUNDATION_COMPILER_GCC 0
#define FOUNDATION_COMPILER_MSVC 0
#define FOUNDATION_COMPILER_INTEL 0

// First, platforms and architectures

// Android
#if defined(__ANDROID__)

#undef FOUNDATION_PLATFORM_ANDROID
#define FOUNDATION_PLATFORM_ANDROID 1

// Compatibile platforms
#undef FOUNDATION_PLATFORM_POSIX
#define FOUNDATION_PLATFORM_POSIX 1

#define FOUNDATION_PLATFORM_NAME "Android"

// Architecture and detailed description
#if defined(__arm__)
#undef FOUNDATION_ARCH_ARM
#define FOUNDATION_ARCH_ARM 1
#ifdef __ARM_ARCH_7A__
#undef FOUNDATION_ARCH_ARM7
#define FOUNDATION_ARCH_ARM7 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Android ARMv7"
#elif defined(__ARM_ARCH_5TE__)
#undef FOUNDATION_ARCH_ARM5
#define FOUNDATION_ARCH_ARM5 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Android ARMv5"
#else
#error Unsupported ARM architecture
#endif
#elif defined(__aarch64__)
#undef FOUNDATION_ARCH_ARM
#define FOUNDATION_ARCH_ARM 1
#undef FOUNDATION_ARCH_ARM_64
#define FOUNDATION_ARCH_ARM_64 1
// Assume ARMv8 for now
//#    if defined( __ARM_ARCH ) && ( __ARM_ARCH == 8 )
#undef FOUNDATION_ARCH_ARM8_64
#define FOUNDATION_ARCH_ARM8_64 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Android ARM64v8"
//#    else
//#      error Unrecognized AArch64 architecture
//#    endif
#elif defined(__i386__)
#undef FOUNDATION_ARCH_X86
#define FOUNDATION_ARCH_X86 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Android x86"
#elif defined(__x86_64__)
#undef FOUNDATION_ARCH_X86_64
#define FOUNDATION_ARCH_X86_64 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Android x86-64"
#elif (defined(__mips__) && defined(__mips64))
#undef FOUNDATION_ARCH_MIPS
#define FOUNDATION_ARCH_MIPS 1
#undef FOUNDATION_ARCH_MIPS_64
#define FOUNDATION_ARCH_MIPS_64 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Android MIPS64"
#ifndef _MIPS_ISA
#define _MIPS_ISA 7 /*_MIPS_ISA_MIPS64*/
#endif
#elif defined(__mips__)
#undef FOUNDATION_ARCH_MIPS
#define FOUNDATION_ARCH_MIPS 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Android MIPS"
#ifndef _MIPS_ISA
#define _MIPS_ISA 6 /*_MIPS_ISA_MIPS32*/
#endif
#else
#error Unknown architecture
#endif

// Traits
#if FOUNDATION_ARCH_MIPS
#if defined(__MIPSEL__) || defined(__MIPSEL) || defined(_MIPSEL)
#undef FOUNDATION_ARCH_ENDIAN_LITTLE
#define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#else
#undef FOUNDATION_ARCH_ENDIAN_BIG
#define FOUNDATION_ARCH_ENDIAN_BIG 1
#endif
#elif defined(__AARCH64EB__) || defined(__ARMEB__)
#undef FOUNDATION_ARCH_ENDIAN_BIG
#define FOUNDATION_ARCH_ENDIAN_BIG 1
#else
#undef FOUNDATION_ARCH_ENDIAN_LITTLE
#define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#endif

#undef FOUNDATION_PLATFORM_FAMILY_MOBILE
#define FOUNDATION_PLATFORM_FAMILY_MOBILE 1

#undef FOUNDATION_PLATFORM_FAMILY_CONSOLE
#define FOUNDATION_PLATFORM_FAMILY_CONSOLE 1

// Tizen
#elif defined(__TIZEN__)

#undef FOUNDATION_PLATFORM_TIZEN
#define FOUNDATION_PLATFORM_TIZEN 1

// Compatibile platforms
#undef FOUNDATION_PLATFORM_POSIX
#define FOUNDATION_PLATFORM_POSIX 1

#define FOUNDATION_PLATFORM_NAME "Tizen"

// Architecture and detailed description
#if defined(__arm__)
#undef FOUNDATION_ARCH_ARM
#define FOUNDATION_ARCH_ARM 1
#ifdef __ARM_ARCH_7A__
#undef FOUNDATION_ARCH_ARM7
#define FOUNDATION_ARCH_ARM7 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Tizen ARMv7"
#elif defined(__ARM_ARCH_5TE__)
#undef FOUNDATION_ARCH_ARM5
#define FOUNDATION_ARCH_ARM5 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Tizen ARMv5"
#else
#error Unsupported ARM architecture
#endif
#elif defined(__i386__)
#undef FOUNDATION_ARCH_X86
#define FOUNDATION_ARCH_X86 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Tizen x86"
#elif defined(__x86_64__)
#undef FOUNDATION_ARCH_X86_64
#define FOUNDATION_ARCH_X86_64 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Tizen x86-64"
#else
#error Unknown architecture
#endif

// Traits
#if defined(__AARCH64EB__) || defined(__ARMEB__)
#undef FOUNDATION_ARCH_ENDIAN_BIG
#define FOUNDATION_ARCH_ENDIAN_BIG 1
#else
#undef FOUNDATION_ARCH_ENDIAN_LITTLE
#define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#endif

#undef FOUNDATION_PLATFORM_FAMILY_MOBILE
#define FOUNDATION_PLATFORM_FAMILY_MOBILE 1

#undef FOUNDATION_PLATFORM_FAMILY_CONSOLE
#define FOUNDATION_PLATFORM_FAMILY_CONSOLE 1

// macOS and iOS
#elif (defined(__APPLE__) && __APPLE__)

#undef FOUNDATION_PLATFORM_APPLE
#define FOUNDATION_PLATFORM_APPLE 1

#undef FOUNDATION_PLATFORM_POSIX
#define FOUNDATION_PLATFORM_POSIX 1

#include <TargetConditionals.h>

#if defined(__IPHONE__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || \
    (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR)

#undef FOUNDATION_PLATFORM_IOS
#define FOUNDATION_PLATFORM_IOS 1

#define FOUNDATION_PLATFORM_NAME "iOS"

#if defined(__arm__)
#undef FOUNDATION_ARCH_ARM
#define FOUNDATION_ARCH_ARM 1
#if defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7S__)
#undef FOUNDATION_ARCH_ARM7
#define FOUNDATION_ARCH_ARM7 1
#define FOUNDATION_PLATFORM_DESCRIPTION "iOS ARMv7"
#ifndef __ARM_NEON__
#error Missing ARM NEON support
#endif
#elif defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6__)
#undef FOUNDATION_ARCH_ARM6
#define FOUNDATION_ARCH_ARM6 1
#define FOUNDATION_PLATFORM_DESCRIPTION "iOS ARMv6"
#else
#error Unrecognized ARM architecture
#endif
#elif defined(__arm64__)
#undef FOUNDATION_ARCH_ARM
#define FOUNDATION_ARCH_ARM 1
#undef FOUNDATION_ARCH_ARM_64
#define FOUNDATION_ARCH_ARM_64 1
#if defined(__ARM64_ARCH_8__)
#undef FOUNDATION_ARCH_ARM8_64
#define FOUNDATION_ARCH_ARM8_64 1
#define FOUNDATION_PLATFORM_DESCRIPTION "iOS ARM64v8"
#else
#error Unrecognized ARM architecture
#endif
#elif defined(__i386__)
#undef FOUNDATION_PLATFORM_IOS_SIMULATOR
#define FOUNDATION_PLATFORM_IOS_SIMULATOR 1
#undef FOUNDATION_ARCH_X86
#define FOUNDATION_ARCH_X86 1
#define FOUNDATION_PLATFORM_DESCRIPTION "iOS x86 (simulator)"
#elif defined(__x86_64__)
#undef FOUNDATION_PLATFORM_IOS_SIMULATOR
#define FOUNDATION_PLATFORM_IOS_SIMULATOR 1
#undef FOUNDATION_ARCH_X86_64
#define FOUNDATION_ARCH_X86_64 1
#define FOUNDATION_PLATFORM_DESCRIPTION "iOS x86_64 (simulator)"
#else
#error Unknown architecture
#endif

#undef FOUNDATION_ARCH_ENDIAN_LITTLE
#define FOUNDATION_ARCH_ENDIAN_LITTLE 1

#undef FOUNDATION_PLATFORM_FAMILY_MOBILE
#define FOUNDATION_PLATFORM_FAMILY_MOBILE 1

#undef FOUNDATION_PLATFORM_FAMILY_CONSOLE
#define FOUNDATION_PLATFORM_FAMILY_CONSOLE 1

#elif defined(__MACH__)

#undef FOUNDATION_PLATFORM_MACOS
#define FOUNDATION_PLATFORM_MACOS 1

#define FOUNDATION_PLATFORM_NAME "macOS"

#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64)
#undef FOUNDATION_ARCH_X86_64
#define FOUNDATION_ARCH_X86_64 1
#undef FOUNDATION_ARCH_ENDIAN_LITTLE
#define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#define FOUNDATION_PLATFORM_DESCRIPTION "macOS x86-64"
#elif defined(__i386__) || defined(__intel__)
#undef FOUNDATION_ARCH_X86
#define FOUNDATION_ARCH_X86 1
#undef FOUNDATION_ARCH_ENDIAN_LITTLE
#define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#define FOUNDATION_PLATFORM_DESCRIPTION "macOS x86"
#elif defined(__arm64__) || defined(__aarch64__)
#undef FOUNDATION_ARCH_ARM
#define FOUNDATION_ARCH_ARM 1
#undef FOUNDATION_ARCH_ARM_64
#define FOUNDATION_ARCH_ARM_64 1
#undef FOUNDATION_ARCH_ENDIAN_LITTLE
#define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#if defined(__ARM64_ARCH_8__)
#undef FOUNDATION_ARCH_ARM8_64
#define FOUNDATION_ARCH_ARM8_64 1
#define FOUNDATION_PLATFORM_DESCRIPTION "macOS ARM64v8"
#else
#error Unrecognized ARM architecture
#endif
#else
#error Unknown architecture
#endif

#undef FOUNDATION_PLATFORM_FAMILY_DESKTOP
#define FOUNDATION_PLATFORM_FAMILY_DESKTOP 1

#else
#error Unknown Apple Platform
#endif

// Linux
#elif (defined(__linux__) || defined(__linux))

#undef FOUNDATION_PLATFORM_LINUX
#define FOUNDATION_PLATFORM_LINUX 1

#undef FOUNDATION_PLATFORM_POSIX
#define FOUNDATION_PLATFORM_POSIX 1

#define FOUNDATION_PLATFORM_NAME "Linux"

#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64)
#undef FOUNDATION_ARCH_X86_64
#define FOUNDATION_ARCH_X86_64 1
#undef FOUNDATION_ARCH_ENDIAN_LITTLE
#define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Linux x86-64"
#elif defined(__i386__) || defined(__intel__) || defined(_M_IX86)
#undef FOUNDATION_ARCH_X86
#define FOUNDATION_ARCH_X86 1
#undef FOUNDATION_ARCH_ENDIAN_LITTLE
#define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Linux x86"

#elif defined(__powerpc64__) || defined(__POWERPC64__)
#undef FOUNDATION_ARCH_PPC_64
#define FOUNDATION_ARCH_PPC_64 1
#undef FOUNDATION_ARCH_ENDIAN_BIG
#define FOUNDATION_ARCH_ENDIAN_BIG 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Linux PPC64"
#elif defined(__powerpc__) || defined(__POWERPC__)
#undef FOUNDATION_ARCH_PPC
#define FOUNDATION_ARCH_PPC 1
#undef FOUNDATION_ARCH_ENDIAN_BIG
#define FOUNDATION_ARCH_ENDIAN_BIG 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Linux PPC"
#elif defined(__arm__)
#undef FOUNDATION_ARCH_ARM
#define FOUNDATION_ARCH_ARM 1
#if defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7S__)
#undef FOUNDATION_ARCH_ARM7
#define FOUNDATION_ARCH_ARM7 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Linux ARMv7"
#ifndef __ARM_NEON__
#error Missing ARM NEON support
#endif
#elif defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6ZK__)
#undef FOUNDATION_ARCH_ARM6
#define FOUNDATION_ARCH_ARM6 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Linux ARMv6"
#else
#error Unrecognized ARM architecture
#endif

// Traits
#if defined(__ARMEB__)
#undef FOUNDATION_ARCH_ENDIAN_BIG
#define FOUNDATION_ARCH_ENDIAN_BIG 1
#else
#undef FOUNDATION_ARCH_ENDIAN_LITTLE
#define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#endif

#elif defined(__arm64__) || defined(__aarch64__)
#undef FOUNDATION_ARCH_ARM
#define FOUNDATION_ARCH_ARM 1
#undef FOUNDATION_ARCH_ARM_64
#define FOUNDATION_ARCH_ARM_64 1
#undef FOUNDATION_ARCH_ENDIAN_LITTLE
#define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#if defined(__ARM64_ARCH_8__)
#undef FOUNDATION_ARCH_ARM8_64
#define FOUNDATION_ARCH_ARM8_64 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Linux ARM64v8"
#else
#error Unrecognized ARM architecture
#endif

// Traits
#if defined(__AARCH64EB__)
#undef FOUNDATION_ARCH_ENDIAN_BIG
#define FOUNDATION_ARCH_ENDIAN_BIG 1
#else
#undef FOUNDATION_ARCH_ENDIAN_LITTLE
#define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#endif

#else
#error Unknown architecture
#endif

#if defined(__raspberrypi__)
#undef FOUNDATION_PLATFORM_LINUX_RASPBERRYPI
#define FOUNDATION_PLATFORM_LINUX_RASPBERRYPI 1
#endif

#undef FOUNDATION_PLATFORM_FAMILY_DESKTOP
#define FOUNDATION_PLATFORM_FAMILY_DESKTOP 1

// BSD family
#elif (defined(__BSD__) || defined(__FreeBSD__))

#undef FOUNDATION_PLATFORM_BSD
#define FOUNDATION_PLATFORM_BSD 1

#undef FOUNDATION_PLATFORM_POSIX
#define FOUNDATION_PLATFORM_POSIX 1

#define FOUNDATION_PLATFORM_NAME "BSD"

#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64)
#undef FOUNDATION_ARCH_X86_64
#define FOUNDATION_ARCH_X86_64 1
#undef FOUNDATION_ARCH_ENDIAN_LITTLE
#define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#define FOUNDATION_PLATFORM_DESCRIPTION "BSD x86-64"
#elif defined(__i386__) || defined(__intel__) || defined(_M_IX86)
#undef FOUNDATION_ARCH_X86
#define FOUNDATION_ARCH_X86 1
#undef FOUNDATION_ARCH_ENDIAN_LITTLE
#define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#define FOUNDATION_PLATFORM_DESCRIPTION "BSD x86"

#elif defined(__powerpc64__) || defined(__POWERPC64__)
#undef FOUNDATION_ARCH_PPC_64
#define FOUNDATION_ARCH_PPC_64 1
#undef FOUNDATION_ARCH_ENDIAN_BIG
#define FOUNDATION_ARCH_ENDIAN_BIG 1
#define FOUNDATION_PLATFORM_DESCRIPTION "BSD PPC64"
#elif defined(__powerpc__) || defined(__POWERPC__)
#undef FOUNDATION_ARCH_PPC
#define FOUNDATION_ARCH_PPC 1
#undef FOUNDATION_ARCH_ENDIAN_BIG
#define FOUNDATION_ARCH_ENDIAN_BIG 1
#define FOUNDATION_PLATFORM_DESCRIPTION "BSD PPC"

#else
#error Unknown architecture
#endif

#undef FOUNDATION_PLATFORM_FAMILY_DESKTOP
#define FOUNDATION_PLATFORM_FAMILY_DESKTOP 1

// Windows
#elif defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)

#undef FOUNDATION_PLATFORM_WINDOWS
#define FOUNDATION_PLATFORM_WINDOWS 1

#define FOUNDATION_PLATFORM_NAME "Windows"

#if defined(__x86_64__) || defined(_M_AMD64) || defined(_AMD64_)
#undef FOUNDATION_ARCH_X86_64
#define FOUNDATION_ARCH_X86_64 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Windows x86-64"
#elif defined(__x86__) || defined(_M_IX86) || defined(_X86_)
#undef FOUNDATION_ARCH_X86
#define FOUNDATION_ARCH_X86 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Windows x86"
#elif defined(__ia64__) || defined(_M_IA64) || defined(_IA64_)
#undef FOUNDATION_ARCH_IA64
#define FOUNDATION_ARCH_IA64 1
#define FOUNDATION_PLATFORM_DESCRIPTION "Windows IA-64"

#else
#error Unknown architecture
#endif

#undef FOUNDATION_ARCH_ENDIAN_LITTLE
#define FOUNDATION_ARCH_ENDIAN_LITTLE 1

#undef FOUNDATION_PLATFORM_FAMILY_DESKTOP
#define FOUNDATION_PLATFORM_FAMILY_DESKTOP 1

#else
#error Unknown platform
#endif

// Export/import attribute
#if BUILD_DYNAMIC_LINK && FOUNDATION_PLATFORM_WINDOWS
#define FOUNDATION_EXPORT_LINK __declspec(dllexport)
#define FOUNDATION_IMPORT_LINK __declspec(dllimport)
#else
#define FOUNDATION_EXPORT_LINK
#define FOUNDATION_IMPORT_LINK
#endif
#if FOUNDATION_COMPILE
#ifdef __cplusplus
#define FOUNDATION_EXTERN extern "C" FOUNDATION_IMPORT_LINK
#define FOUNDATION_API extern "C" FOUNDATION_EXPORT_LINK
#else
#define FOUNDATION_EXTERN extern FOUNDATION_IMPORT_LINK
#define FOUNDATION_API extern FOUNDATION_EXPORT_LINK
#endif
#else
#ifdef __cplusplus
#define FOUNDATION_EXTERN extern "C" FOUNDATION_IMPORT_LINK
#define FOUNDATION_API extern "C" FOUNDATION_IMPORT_LINK
#else
#define FOUNDATION_EXTERN extern FOUNDATION_IMPORT_LINK
#define FOUNDATION_API extern FOUNDATION_IMPORT_LINK
#endif
#endif

// Utility macros
#define FOUNDATION_PREPROCESSOR_TOSTRING(x) FOUNDATION_PREPROCESSOR_TOSTRING_IMPL(x)
#define FOUNDATION_PREPROCESSOR_TOSTRING_IMPL(x) #x

#define FOUNDATION_PREPROCESSOR_JOIN(a, b) FOUNDATION_PREPROCESSOR_JOIN_WRAP(a, b)
#define FOUNDATION_PREPROCESSOR_JOIN_WRAP(a, b) FOUNDATION_PREPROCESSOR_JOIN_IMPL(a, b)
#define FOUNDATION_PREPROCESSOR_JOIN_IMPL(a, b) a##b

#define FOUNDATION_PREPROCESSOR_NARGS(...) \
	FOUNDATION_PREPROCESSOR_JOIN(          \
	    FOUNDATION_PREPROCESSOR_NARGS_WRAP(__VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, ), )
#define FOUNDATION_PREPROCESSOR_NARGS_WRAP(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _, \
                                           ...)                                                                      \
	_

#define FOUNDATION_PREPROCESSOR_ELEM(n, ...) FOUNDATION_PREPROCESSOR_ELEM_I(n, __VA_ARGS__)
#define FOUNDATION_PREPROCESSOR_ELEM_I(n, ...) \
	FOUNDATION_PREPROCESSOR_JOIN(FOUNDATION_PREPROCESSOR_JOIN(FOUNDATION_PREPROCESSOR_ELEM_, n)(__VA_ARGS__, ), )

#define FOUNDATION_PREPROCESSOR_ELEM_0(_0, ...) _0
#define FOUNDATION_PREPROCESSOR_ELEM_1(_0, _1, ...) _1
#define FOUNDATION_PREPROCESSOR_ELEM_2(_0, _1, _2, ...) _2
#define FOUNDATION_PREPROCESSOR_ELEM_3(_0, _1, _2, _3, ...) _3
#define FOUNDATION_PREPROCESSOR_ELEM_4(_0, _1, _2, _3, _4, ...) _4
#define FOUNDATION_PREPROCESSOR_ELEM_5(_0, _1, _2, _3, _4, _5, ...) _5
#define FOUNDATION_PREPROCESSOR_ELEM_6(_0, _1, _2, _3, _4, _5, _6, ...) _6
#define FOUNDATION_PREPROCESSOR_ELEM_7(_0, _1, _2, _3, _4, _5, _6, _7, ...) _7
#define FOUNDATION_PREPROCESSOR_ELEM_8(_0, _1, _2, _3, _4, _5, _6, _7, _8, ...) _8
#define FOUNDATION_PREPROCESSOR_ELEM_9(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, ...) _9
#define FOUNDATION_PREPROCESSOR_ELEM_10(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, ...) _10
#define FOUNDATION_PREPROCESSOR_ELEM_11(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, ...) _11
#define FOUNDATION_PREPROCESSOR_ELEM_12(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, ...) _12
#define FOUNDATION_PREPROCESSOR_ELEM_13(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, ...) _13
#define FOUNDATION_PREPROCESSOR_ELEM_14(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, ...) _14
#define FOUNDATION_PREPROCESSOR_ELEM_15(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define FOUNDATION_PREPROCESSOR_ELEM_16(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, \
                                        ...)                                                                       \
	_16

// Architecture details
#if defined(__SSE2__) || FOUNDATION_ARCH_X86_64
#undef FOUNDATION_ARCH_SSE2
#define FOUNDATION_ARCH_SSE2 1
#endif

#ifdef __SSE3__
#undef FOUNDATION_ARCH_SSE3
#define FOUNDATION_ARCH_SSE3 1
#endif

#ifdef __SSE4_1__
#undef FOUNDATION_ARCH_SSE4
#define FOUNDATION_ARCH_SSE4 1
#endif

#ifdef __AVX__
#undef FOUNDATION_ARCH_AVX
#define FOUNDATION_ARCH_AVX 1
#endif

#if defined(__ARM_NEON__) || defined(__ARM_NEON)
#undef FOUNDATION_ARCH_NEON
#define FOUNDATION_ARCH_NEON 1
#endif

#ifdef __thumb__
#undef FOUNDATION_ARCH_THUMB
#define FOUNDATION_ARCH_THUMB 1
#endif

// Compilers

#if defined(__clang__)

#undef FOUNDATION_COMPILER_CLANG
#define FOUNDATION_COMPILER_CLANG 1

#define FOUNDATION_COMPILER_NAME "clang"
#define FOUNDATION_COMPILER_DESCRIPTION \
	FOUNDATION_COMPILER_NAME            \
	" " FOUNDATION_PREPROCESSOR_TOSTRING(__clang_major__) "." FOUNDATION_PREPROCESSOR_TOSTRING(__clang_minor__)
#define FOUNDATION_CLANG_VERSION (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)

#define FOUNDATION_RESTRICT __restrict
#define FOUNDATION_THREADLOCAL _Thread_local

#define FOUNDATION_ATTRIBUTE(x) __attribute__((__##x##__))
#define FOUNDATION_ATTRIBUTE2(x, y) __attribute__((__##x##__(y)))
#define FOUNDATION_ATTRIBUTE3(x, y, z) __attribute__((__##x##__(y, z)))
#define FOUNDATION_ATTRIBUTE4(x, y, z, w) __attribute__((__##x##__(y, z, w)))

#define FOUNDATION_DEPRECATED FOUNDATION_ATTRIBUTE(deprecated)
#define FOUNDATION_FORCEINLINE inline FOUNDATION_ATTRIBUTE(always_inline)
#define FOUNDATION_NOINLINE FOUNDATION_ATTRIBUTE(noinline)
#define FOUNDATION_PURECALL FOUNDATION_ATTRIBUTE(pure)
#define FOUNDATION_CONSTCALL FOUNDATION_ATTRIBUTE(const)
#define FOUNDATION_PRINTFCALL(start, num) FOUNDATION_ATTRIBUTE4(format, printf, start, num)
#define FOUNDATION_ALIGN(alignment) FOUNDATION_ATTRIBUTE2(aligned, alignment)
#define FOUNDATION_ALIGNOF(type) __alignof__(type)
#define FOUNDATION_ALIGNED_STRUCT(name, alignment) struct __attribute__((__aligned__(alignment))) name

#define FOUNDATION_LIKELY(x) __builtin_expect(!!(x), 1)
#define FOUNDATION_UNLIKELY(x) __builtin_expect(!!(x), 0)

#if FOUNDATION_PLATFORM_WINDOWS
#if (FOUNDATION_CLANG_VERSION < 30800)
#error CLang 3.8 or later is required
#endif
#define STDCALL FOUNDATION_ATTRIBUTE(stdcall)
#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 0
#endif
#ifndef __MSVCRT_VERSION__
#define __MSVCRT_VERSION__ 0x0800
#endif
#define USE_NO_MINGW_SETJMP_TWO_ARGS 1
#if __has_warning("-Wunknown-pragmas")
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#endif
#if __has_warning("-Wformat-non-iso")
#pragma clang diagnostic ignored "-Wformat-non-iso"
#endif
#else
#define STDCALL
#endif

#if __has_warning("-Wreserved-id-macro")
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#if __has_warning("-Wcovered-switch-default")
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#endif
#if __has_warning("-Wmissing-field-initializers")
#pragma clang diagnostic ignored "-Wmissing-field-initializers"
#endif

#elif defined(__GNUC__)

#undef FOUNDATION_COMPILER_GCC
#define FOUNDATION_COMPILER_GCC 1

#define FOUNDATION_COMPILER_NAME "gcc"
#define FOUNDATION_COMPILER_DESCRIPTION \
	FOUNDATION_COMPILER_NAME            \
	" " FOUNDATION_PREPROCESSOR_TOSTRING(__GNUC__) "." FOUNDATION_PREPROCESSOR_TOSTRING(__GNUC_MINOR__)

#define FOUNDATION_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#define FOUNDATION_RESTRICT __restrict
#define FOUNDATION_THREADLOCAL __thread

#define FOUNDATION_ATTRIBUTE(x) __attribute__((__##x##__))
#define FOUNDATION_ATTRIBUTE2(x, y) __attribute__((__##x##__(y)))
#define FOUNDATION_ATTRIBUTE3(x, y, z) __attribute__((__##x##__(y, z)))
#define FOUNDATION_ATTRIBUTE4(x, y, z, w) __attribute__((__##x##__(y, z, w)))

#define FOUNDATION_DEPRECATED FOUNDATION_ATTRIBUTE(deprecated)
#define FOUNDATION_FORCEINLINE inline FOUNDATION_ATTRIBUTE(always_inline)
#define FOUNDATION_NOINLINE FOUNDATION_ATTRIBUTE(noinline)
#define FOUNDATION_PURECALL FOUNDATION_ATTRIBUTE(pure)
#define FOUNDATION_CONSTCALL FOUNDATION_ATTRIBUTE(const)
#define FOUNDATION_PRINTFCALL(start, num) FOUNDATION_ATTRIBUTE4(format, printf, start, num)
#define FOUNDATION_ALIGN(alignment) FOUNDATION_ATTRIBUTE2(aligned, alignment)
#define FOUNDATION_ALIGNOF(type) __alignof__(type)
#define FOUNDATION_ALIGNED_STRUCT(name, alignment) struct FOUNDATION_ALIGN(alignment) name

#define FOUNDATION_LIKELY(x) __builtin_expect(!!(x), 1)
#define FOUNDATION_UNLIKELY(x) __builtin_expect(!!(x), 0)

#if FOUNDATION_PLATFORM_WINDOWS
#define STDCALL FOUNDATION_ATTRIBUTE(stdcall)
#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 0
#endif
#ifndef __MSVCRT_VERSION__
#define __MSVCRT_VERSION__ 0x0800
#endif
#pragma GCC diagnostic ignored "-Wformat"
#pragma GCC diagnostic ignored "-Wformat-extra-args"
#pragma GCC diagnostic ignored "-Wpedantic"
#else
#define STDCALL
#endif

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

// Intel
#elif defined(__ICL) || defined(__ICC) || defined(__INTEL_COMPILER)

#undef FOUNDATION_COMPILER_INTEL
#define FOUNDATION_COMPILER_INTEL 1

#define FOUNDATION_COMPILER_NAME "intel"
#if defined(__ICL)
#define FOUNDATION_COMPILER_DESCRIPTION FOUNDATION_COMPILER_NAME " " FOUNDATION_PREPROCESSOR_TOSTRING(__ICL)
#elif defined(__ICC)
#define FOUNDATION_COMPILER_DESCRIPTION FOUNDATION_COMPILER_NAME " " FOUNDATION_PREPROCESSOR_TOSTRING(__ICC)
#endif

#define FOUNDATION_RESTRICT __restrict
#define FOUNDATION_THREADLOCAL __declspec(thread)

#define FOUNDATION_ATTRIBUTE(x)
#define FOUNDATION_ATTRIBUTE2(x, y)
#define FOUNDATION_ATTRIBUTE3(x, y, z)
#define FOUNDATION_ATTRIBUTE4(x, y, z, w)

#define FOUNDATION_DEPRECATED
#define FOUNDATION_FORCEINLINE __forceinline
#define FOUNDATION_NOINLINE __declspec(noinline)
#define FOUNDATION_PURECALL
#define FOUNDATION_CONSTCALL
#define FOUNDATION_PRINTFCALL(start, num)
#define FOUNDATION_ALIGN(alignment) __declspec(align(alignment))
#define FOUNDATION_ALIGNOF(type) __alignof(type)
#define FOUNDATION_ALIGNED_STRUCT(name, alignment) FOUNDATION_ALIGN(alignment) struct name

#define FOUNDATION_LIKELY(x) __builtin_expect(!!(x), 1)
#define FOUNDATION_UNLIKELY(x) __builtin_expect(!!(x), 0)

#if FOUNDATION_PLATFORM_WINDOWS
#define STDCALL __stdcall
#define va_copy(d, s) ((d) = (s))
#else
#define STDCALL
#endif

#include <intrin.h>

// Microsoft
#elif defined(_MSC_VER)

#undef FOUNDATION_COMPILER_MSVC
#define FOUNDATION_COMPILER_MSVC 1

#define FOUNDATION_COMPILER_NAME "msvc"
#define FOUNDATION_COMPILER_DESCRIPTION FOUNDATION_COMPILER_NAME " " FOUNDATION_PREPROCESSOR_TOSTRING(_MSC_VER)

#define FOUNDATION_ATTRIBUTE(x)
#define FOUNDATION_ATTRIBUTE2(x, y)
#define FOUNDATION_ATTRIBUTE3(x, y, z)
#define FOUNDATION_ATTRIBUTE4(x, y, z, w)

#define FOUNDATION_RESTRICT restrict
#define FOUNDATION_THREADLOCAL __declspec(thread)

#define FOUNDATION_DEPRECATED __declspec(deprecated)
#define FOUNDATION_FORCEINLINE __forceinline
#define FOUNDATION_NOINLINE __declspec(noinline)
#define FOUNDATION_PURECALL __declspec(noalias)
#define FOUNDATION_CONSTCALL __declspec(noalias)
#define FOUNDATION_PRINTFCALL(start, num)
#define FOUNDATION_ALIGN(alignment) __declspec(align(alignment))
#define FOUNDATION_ALIGNOF(type) __alignof(type)
#define FOUNDATION_ALIGNED_STRUCT(name, alignment) FOUNDATION_ALIGN(alignment) struct name

#define FOUNDATION_LIKELY(x) (x)
#define FOUNDATION_UNLIKELY(x) (x)

#pragma warning(disable : 4054)
#pragma warning(disable : 4055)
#pragma warning(disable : 4127)
#pragma warning(disable : 4132)
#pragma warning(disable : 4200)
#pragma warning(disable : 4204)
#pragma warning(disable : 4702)
#pragma warning(disable : 4706)
#ifdef __cplusplus
#pragma warning(disable : 4100)
#pragma warning(disable : 4510)
#pragma warning(disable : 4512)
#pragma warning(disable : 4610)
#endif

#if FOUNDATION_PLATFORM_WINDOWS
#define STDCALL __stdcall
#else
#define STDCALL
#endif

#if defined(FOUNDATION_COMPILE) && FOUNDATION_COMPILE && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#ifndef _LINT
#define _Static_assert static_assert
#endif

#if _MSC_VER < 1800
#define va_copy(d, s) ((d) = (s))
#endif

#include <intrin.h>

#else

#warning Unknown compiler

#define FOUNDATION_COMPILER_NAME "unknown"
#define FOUNDATION_COMPILER_DESCRIPTION "unknown"

#define FOUNDATION_RESTRICT
#define FOUNDATION_THREADLOCAL

#define FOUNDATION_DEPRECATED
#define FOUNDATION_FORCEINLINE
#define FOUNDATION_NOINLINE
#define FOUNDATION_PURECALL
#define FOUNDATION_CONSTCALL
#define FOUNDATION_ALIGN
#define FOUNDATION_ALIGNOF
#define FOUNDATION_ALIGNED_STRUCT(name, alignment) struct name

#define FOUNDATION_LIKELY(x) (x)
#define FOUNDATION_UNLIKELY(x) (x)

#endif

#if FOUNDATION_PLATFORM_POSIX
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic push
#if __has_warning("-Wundef")
#pragma clang diagnostic ignored "-Wundef"
#endif
#if __has_warning("-Wsign-conversion")
#pragma clang diagnostic ignored "-Wsign-conversion"
#endif
#if __has_warning("-Wunknown-attributes")
#pragma clang diagnostic ignored "-Wunknown-attributes"
#endif
#endif

// Base data types
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#if !FOUNDATION_PLATFORM_WINDOWS
#include <wchar.h>
#endif
#if (FOUNDATION_PLATFORM_POSIX && !FOUNDATION_PLATFORM_APPLE)
#include <sys/types.h>
#endif

#ifndef __cplusplus
#define nullptr ((void*)0)
#endif

#if !defined(__cplusplus) && !defined(__bool_true_false_are_defined)
typedef enum { false = 0, true = 1 } bool;
#endif

#if FOUNDATION_COMPILER_MSVC
typedef enum memory_order {
	memory_order_relaxed,
	memory_order_consume,
	memory_order_acquire,
	memory_order_release,
	memory_order_acq_rel,
	memory_order_seq_cst
} memory_order;
#endif

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic pop
#endif

typedef float float32_t;
typedef double float64_t;

struct uint128_t {
	uint64_t word[2];
};
typedef struct uint128_t uint128_t;

struct uint256_t {
	uint64_t word[4];
};
typedef struct uint256_t uint256_t;

struct uint512_t {
	uint64_t word[8];
};
typedef struct uint512_t uint512_t;

#define FLOAT32_C(x) (x##f)
#define FLOAT64_C(x) (x)

#ifndef FOUNDATION_SIZE_REAL
#define FOUNDATION_SIZE_REAL 4
#endif

#if FOUNDATION_SIZE_REAL == 8
typedef float64_t real;
#define REAL_C(x) FLOAT64_C(x)
#else
typedef float32_t real;
#define REAL_C(x) FLOAT32_C(x)
#endif

// Pointer size
#if FOUNDATION_ARCH_ARM_64 || FOUNDATION_ARCH_X86_64 || FOUNDATION_ARCH_PPC_64 || FOUNDATION_ARCH_IA64 || \
    FOUNDATION_ARCH_MIPS_64
#define FOUNDATION_SIZE_POINTER 8
#else
#define FOUNDATION_SIZE_POINTER 4
#endif

// wchar_t size
#if FOUNDATION_PLATFORM_LINUX_RASPBERRYPI
#define FOUNDATION_SIZE_WCHAR 4
#else
#if WCHAR_MAX > 0xffff
#define FOUNDATION_SIZE_WCHAR 4
#else
#define FOUNDATION_SIZE_WCHAR 2
#endif
#endif

#if FOUNDATION_COMPILER_MSVC || defined(__STDC_NO_ATOMICS__)

// Atomic types
FOUNDATION_ALIGNED_STRUCT(atomic32_t, 4) {
	volatile int32_t nonatomic;
};
typedef struct atomic32_t atomic32_t;

FOUNDATION_ALIGNED_STRUCT(atomic64_t, 8) {
	volatile int64_t nonatomic;
};
typedef struct atomic64_t atomic64_t;

FOUNDATION_ALIGNED_STRUCT(atomicptr_t, FOUNDATION_SIZE_POINTER) {
	volatile void* nonatomic;
};
typedef struct atomicptr_t atomicptr_t;

#else

typedef volatile _Atomic(int32_t) atomic32_t;
typedef volatile _Atomic(int64_t) atomic64_t;
typedef volatile _Atomic(void*) atomicptr_t;

#endif

// Pointer arithmetic
#define pointer_offset(ptr, ofs) (void*)((char*)(ptr) + (ptrdiff_t)(ofs))
#define pointer_offset_const(ptr, ofs) (const void*)((const char*)(ptr) + (ptrdiff_t)(ofs))
#define pointer_diff(first, second) (ptrdiff_t)((const char*)(first) - (const char*)(second))

#include <string.h>

// String argument helpers
#define STRING_CONST(s) (s), (sizeof((s)) - 1)
#define STRING_ARGS(s) (s).str, (s).length
#define STRING_ARGS_CAPACITY(s) (s).str, (s).length, (s).length + 1
#define STRING_FORMAT(s) (int)(s).length, (s).str

// Misc
#if defined(__COVERITY__)
#define FOUNDATION_UNUSED_SINGLE(x) ((void)(x))
#elif FOUNDATION_COMPILER_GCC
#define FOUNDATION_UNUSED_SINGLE(x) ((void)sizeof((x)))
#else
#define FOUNDATION_UNUSED_SINGLE(x) (/*lint --e{505,550,818,866} */ (void)sizeof((x), 0))
#endif

#define FOUNDATION_UNUSED_ARGS_0(...)
#define FOUNDATION_UNUSED_ARGS_1(...) FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__))
#define FOUNDATION_UNUSED_ARGS_2(...)                                      \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_1(__VA_ARGS__))
#define FOUNDATION_UNUSED_ARGS_3(...)                                      \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_1(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_2(__VA_ARGS__))
#define FOUNDATION_UNUSED_ARGS_4(...)                                      \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_1(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_2(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_3(__VA_ARGS__))
#define FOUNDATION_UNUSED_ARGS_5(...)                                      \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_1(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_2(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_3(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_4(__VA_ARGS__))
#define FOUNDATION_UNUSED_ARGS_6(...)                                      \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_1(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_2(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_3(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_4(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_5(__VA_ARGS__))
#define FOUNDATION_UNUSED_ARGS_7(...)                                      \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_1(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_2(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_3(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_4(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_5(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_6(__VA_ARGS__))
#define FOUNDATION_UNUSED_ARGS_8(...)                                      \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_1(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_2(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_3(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_4(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_5(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_6(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_7(__VA_ARGS__))
#define FOUNDATION_UNUSED_ARGS_9(...)                                      \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_1(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_2(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_3(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_4(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_5(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_6(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_7(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_8(__VA_ARGS__))
#define FOUNDATION_UNUSED_ARGS_10(...)                                     \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_1(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_2(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_3(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_4(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_5(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_6(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_7(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_8(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_9(__VA_ARGS__))
#define FOUNDATION_UNUSED_ARGS_11(...)                                     \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_1(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_2(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_3(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_4(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_5(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_6(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_7(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_8(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_9(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_10(__VA_ARGS__))
#define FOUNDATION_UNUSED_ARGS_12(...)                                      \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_1(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_2(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_3(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_4(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_5(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_6(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_7(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_8(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_9(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_10(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_11(__VA_ARGS__))
#define FOUNDATION_UNUSED_ARGS_13(...)                                      \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_1(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_2(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_3(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_4(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_5(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_6(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_7(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_8(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_9(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_10(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_11(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_12(__VA_ARGS__))
#define FOUNDATION_UNUSED_ARGS_14(...)                                      \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_1(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_2(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_3(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_4(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_5(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_6(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_7(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_8(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_9(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_10(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_11(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_12(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_13(__VA_ARGS__))
#define FOUNDATION_UNUSED_ARGS_15(...)                                      \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_1(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_2(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_3(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_4(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_5(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_6(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_7(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_8(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_9(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_10(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_11(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_12(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_13(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_14(__VA_ARGS__))
#define FOUNDATION_UNUSED_ARGS_16(...)                                      \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_0(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_1(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_2(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_3(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_4(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_5(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_6(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_7(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_8(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_9(__VA_ARGS__));  \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_10(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_11(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_12(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_13(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_14(__VA_ARGS__)); \
	FOUNDATION_UNUSED_SINGLE(FOUNDATION_PREPROCESSOR_ELEM_15(__VA_ARGS__))

#define FOUNDATION_UNUSED(...)    \
	FOUNDATION_PREPROCESSOR_JOIN( \
	    FOUNDATION_UNUSED_VARARGS_WRAP(FOUNDATION_PREPROCESSOR_NARGS(__VA_ARGS__), __VA_ARGS__), )
#define FOUNDATION_UNUSED_VARARGS(...) \
	FOUNDATION_PREPROCESSOR_JOIN(      \
	    FOUNDATION_UNUSED_VARARGS_WRAP(FOUNDATION_PREPROCESSOR_NARGS(__VA_ARGS__), __VA_ARGS__), )
#define FOUNDATION_UNUSED_VARARGS_WRAP(n, ...) FOUNDATION_UNUSED_VARARGS_IMPL(n, __VA_ARGS__)
#define FOUNDATION_UNUSED_VARARGS_IMPL(n, ...) \
	FOUNDATION_PREPROCESSOR_JOIN(FOUNDATION_PREPROCESSOR_JOIN(FOUNDATION_UNUSED_ARGS_, n)(__VA_ARGS__, ), )

#define FOUNDATION_DECLARE_THREAD_LOCAL(type, name, init)             \
	static FOUNDATION_THREADLOCAL type internal_thread_##name = init; \
	static FOUNDATION_FORCEINLINE void set_thread_##name(type val) {  \
		internal_thread_##name = val;                                 \
	}                                                                 \
	static FOUNDATION_FORCEINLINE type get_thread_##name(void) {      \
		return internal_thread_##name;                                \
	}

#define FOUNDATION_DECLARE_THREAD_LOCAL_ARRAY(type, name, arrsize)            \
	static FOUNDATION_THREADLOCAL type internal_thread_##name[arrsize] = {0}; \
	static FOUNDATION_FORCEINLINE type* get_thread_##name(void) {             \
		return internal_thread_##name;                                        \
	}

// Utility functions for large integer types
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint128_t
uint128_make(const uint64_t w0, const uint64_t w1);

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint128_t
uint128_null(void);

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uint128_equal(const uint128_t u0, const uint128_t u1);

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uint128_is_null(const uint128_t u0);

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint256_t
uint256_make(const uint64_t w0, const uint64_t w1, const uint64_t w2, const uint64_t w3);

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint256_t
uint256_null(void);

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uint256_equal(const uint256_t u0, const uint256_t u1);

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uint256_is_null(const uint256_t u0);

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint512_t
uint512_make(const uint64_t w0, const uint64_t w1, const uint64_t w2, const uint64_t w3, const uint64_t w4,
             const uint64_t w5, const uint64_t w6, const uint64_t w7);

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint512_t
uint512_null(void);

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uint512_equal(const uint512_t u0, const uint512_t u1);

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uint512_is_null(const uint512_t u0);

// Implementations
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint128_t
uint128_make(const uint64_t w0, const uint64_t w1) {
	uint128_t u = {{w0, w1}};
	return u;
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint128_t
uint128_null(void) {
	return uint128_make(0, 0);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uint128_equal(const uint128_t u0, const uint128_t u1) {
	return u0.word[0] == u1.word[0] && u0.word[1] == u1.word[1];
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uint128_is_null(const uint128_t u0) {
	return !u0.word[0] && !u0.word[1];
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint256_t
uint256_make(const uint64_t w0, const uint64_t w1, const uint64_t w2, const uint64_t w3) {
	uint256_t u = {{w0, w1, w2, w3}};
	return u;
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint256_t
uint256_null(void) {
	return uint256_make(0, 0, 0, 0);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uint256_equal(const uint256_t u0, const uint256_t u1) {
	return u0.word[0] == u1.word[0] && u0.word[1] == u1.word[1] && u0.word[2] == u1.word[2] && u0.word[3] == u1.word[3];
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uint256_is_null(const uint256_t u0) {
	return !u0.word[0] && !u0.word[1] && !u0.word[2] && !u0.word[3];
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint512_t
uint512_make(const uint64_t w0, const uint64_t w1, const uint64_t w2, const uint64_t w3, const uint64_t w4,
             const uint64_t w5, const uint64_t w6, const uint64_t w7) {
	uint512_t u = {{w0, w1, w2, w3, w4, w5, w6, w7}};
	return u;
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint512_t
uint512_null(void) {
	return uint512_make(0, 0, 0, 0, 0, 0, 0, 0);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uint512_equal(const uint512_t u0, const uint512_t u1) {
	return u0.word[0] == u1.word[0] && u0.word[1] == u1.word[1] && u0.word[2] == u1.word[2] &&
	       u0.word[3] == u1.word[3] && u0.word[4] == u1.word[4] && u0.word[5] == u1.word[5] &&
	       u0.word[6] == u1.word[6] && u0.word[7] == u1.word[7];
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uint512_is_null(const uint512_t u0) {
	return !u0.word[0] && !u0.word[1] && !u0.word[2] && !u0.word[3] && !u0.word[4] && !u0.word[5] && !u0.word[6] &&
	       !u0.word[7];
}

// Format specifiers for 64bit and pointers
#include <inttypes.h>
#define PRIsize "zu"
#define PRItick PRIi64
#define PRIhash PRIx64

#if FOUNDATION_SIZE_REAL == 8
#define PRIreal "lf"
#else
#define PRIreal "f"
#endif

#if FOUNDATION_SIZE_POINTER == 8
#define PRIfixPTR "016" PRIXPTR
#else
#define PRIfixPTR "08" PRIXPTR
#endif

#if defined(FOUNDATION_PLATFORM_DOXYGEN) && FOUNDATION_PLATFORM_DOXYGEN
#define FOUNDATION_PRINTFCALL(x, y)
#endif

#include <foundation/build.h>

#ifdef __cplusplus
#define FOUNDATION_FLEXIBLE_ARRAY 0
#else
#define FOUNDATION_FLEXIBLE_ARRAY
#endif

/*!
\def FOUNDATION_COMPILE
Defined to 1 when compiling the foundation library, undefined (or zero) when using the library

\def FOUNDATION_EXTERN
Declare a variable visible to users of the library

\def FOUNDATION_API
Declare a function visible to users of the library

\def FOUNDATION_PLATFORM_ANDROID
Defined to 1 if compiling for android platforms, 0 otherwise

\def FOUNDATION_PLATFORM_IOS
Defined to 1 if compiling for iOS platforms (iPhone/iPad and simulators), 0 otherwise

\def FOUNDATION_PLATFORM_IOS_SIMULATOR
Defined to 1 if compiling for iOS simulator (also has FOUNDATION_PLATFORM_IOS defined to 1), 0 otherwise

\def FOUNDATION_PLATFORM_MACOS
Defined to 1 if compiling for macOS, 0 otherwise

\def FOUNDATION_PLATFORM_LINUX
Defined to 1 if compiling for Linux, 0 otherwise

\def FOUNDATION_PLATFORM_LINUX_RASPBERRYPI
Defined to 1 if compiling for Raspberry Pi (also has FOUNDATION_PLATFORM_LINUX defined to 1), 0 otherwise

\def FOUNDATION_PLATFORM_BSD
Defined to 1 if compiling for BSD, 0 otherwise

\def FOUNDATION_PLATFORM_WINDOWS
Defined to 1 if compiling for Windows, 0 otherwise

\def FOUNDATION_PLATFORM_TIZEN
Defined to 1 if compiling for Tizen, 0 otherwise

\def FOUNDATION_PLATFORM_APPLE
Defined to 1 if compiling for Apple platforms (macOS, iOS, iOS simulator), 0 otherwise

\def FOUNDATION_PLATFORM_POSIX
Defined to 1 if compiling for POSIX platforms (Linux, BSD, macOS, iOS, iOS simulator, Android), 0 otherwise

\def FOUNDATION_PLATFORM_FAMILY_MOBILE
Defined to 1 if compiling for mobile platforms (iOS, iOS simulator, Android), 0 otherwise

\def FOUNDATION_PLATFORM_FAMILY_DESKTOP
Defined to 1 if compiling for desktop platforms (Windows, macOS, Linux, Raspberry Pi, BSD), 0 otherwise

\def FOUNDATION_PLATFORM_FAMILY_CONSOLE
Defined to 1 if compiling for console platforms (iOS, iOS simulator, Android, Tizen), 0 otherwise

\def FOUNDATION_ARCH_ARM
Defined to 1 if compiling for ARM architectures, 0 otherwise

\def FOUNDATION_ARCH_ARM5
Defined to 1 if compiling for ARMv5 architectures, 0 otherwise

\def FOUNDATION_ARCH_ARM6
Defined to 1 if compiling for ARMv6 architectures, 0 otherwise

\def FOUNDATION_ARCH_ARM7
Defined to 1 if compiling for ARMv7 architectures, 0 otherwise

\def FOUNDATION_ARCH_ARM8
Defined to 1 if compiling for ARMv8 architectures, 0 otherwise

\def FOUNDATION_ARCH_ARM_64
Defined to 1 if compiling for 64-bit ARM architectures, 0 otherwise

\def FOUNDATION_ARCH_ARM8_64
Defined to 1 if compiling for 64-bit ARMv8 architectures, 0 otherwise

\def FOUNDATION_ARCH_X86
Defined to 1 if compiling for x86 architectures, 0 otherwise

\def FOUNDATION_ARCH_X86_64
Defined to 1 if compiling for x86-64 architectures, 0 otherwise

\def FOUNDATION_ARCH_PPC
Defined to 1 if compiling for PPC architectures, 0 otherwise

\def FOUNDATION_ARCH_PPC_64
Defined to 1 if compiling for 64-bit PPC architectures, 0 otherwise

\def FOUNDATION_ARCH_IA64
Defined to 1 if compiling for IA64 architectures, 0 otherwise

\def FOUNDATION_ARCH_MIPS
Defined to 1 if compiling for MIPS architectures, 0 otherwise

\def FOUNDATION_ARCH_MIPS_64
Defined to 1 if compiling for 64-bit MIPS architectures, 0 otherwise

\def FOUNDATION_ARCH_SSE2
Defined to 1 if compiling with SSE2 instruction set enabled, 0 otherwise

\def FOUNDATION_ARCH_SSE3
Defined to 1 if compiling with SSE3 instruction set enabled, 0 otherwise

\def FOUNDATION_ARCH_SSE4
Defined to 1 if compiling with SSE4 instruction set enabled, 0 otherwise

\def FOUNDATION_ARCH_SSE4_FMA3
Defined to 1 if compiling with SSE4 instruction set (including FMA3 instruction) enabled, 0 otherwise

\def FOUNDATION_ARCH_NEON
Defined to 1 if compiling with NEON instruction set enabled, 0 otherwise

\def FOUNDATION_ARCH_THUMB
Defined to 1 if compiling for ARM THUMB instruction set, 0 otherwise

\def FOUNDATION_ARCH_ENDIAN_LITTLE
Defined to 1 if compiling for little endian architectures, 0 otherwise

\def FOUNDATION_ARCH_ENDIAN_BIG
Defined to 1 if compiling for big endian architectures, 0 otherwise

\def FOUNDATION_COMPILER_CLANG
Defined to 1 if compiling with clang, 0 otherwise

\def FOUNDATION_COMPILER_GCC
Defined to 1 if compiling with GCC, 0 otherwise

\def FOUNDATION_COMPILER_MSVC
Defined to 1 if compiling with Microsoft compiler, 0 otherwise

\def FOUNDATION_COMPILER_INTEL
Defined to 1 if compiling with Intel compiler, 0 otherwise

\def FOUNDATION_COMPILER_NAME
A string naming the compiler used

\def FOUNDATION_COMPILER_DESCRIPTION
A string with a more detailed description of the compiler used, name and version.

\def FOUNDATION_SIZE_REAL
Declare the size of a real number, in bytes. Either 4 or 8 for 32 or 64 bit float
point values, respectively. Default to 4 bytes (32 bit).

\def FOUNDATION_SIZE_POINTER
Defines the size of a pointer on the current architecture, in bytes. Either 4 or 8 bytes
for 32 and 64 bit architectures, respectively.

\def FOUNDATION_SIZE_WCHAR
Defines the size of the wchar_t type, in bytes, depending on platform

\def FOUNDATION_DECLARE_THREAD_LOCAL
Declare a thread-local variable of the given type, name and initial value. Only works
for types that can be safely cast through a uintptr_t (integers, pointers...). This will
also declare and implement two inlined functions to set and get value of the variable,
called get_thread_[name] and set_thread_[name]. For example, to declare a thread-local
integer and use the get/set functions:
<code>FOUNDATION_DECLARE_THREAD_LOCAL( int, myvar, 0 );
set_thread_myvar( 1 );
int currentval = get_thread_myvar();</code>
This macro is guaranteed to work across all supported platforms even when TLS is not
available. For maximum portability use these macros instead of THREADLOCAL.
\param type    Data type
\param name    Variable name
\param init    Initial value

\def FOUNDATION_DECLARE_THREAD_LOCAL_ARRAY
Declare a thread-local array of the given type, name and array size. This will also declare
and implement one inlined function to get the array pointer value of the variable, called
get_thread_[name]. For example, to declare a thread-local integer array and use the get/set
functions:
<code>FOUNDATION_DECLARE_THREAD_LOCAL_ARRAY( int, myvar, 10 );
int* currentarr = get_thread_myvar(); //Get thread-local array storage
currentarr[2] = 10;
int val = currentarr[2];</code>
\param type    Data type
\param name    Variable name
\param arrsize Size of array in number of elements

\def FOUNDATION_RESTRICT
Restrict attribute, defined to nothing if compiler does not support restrict

\def FOUNDATION_THREADLOCAL
Thread local attribute, defined to nothing if the compiler/platform/architecture does not
support thread local variables. For full platform support, use #FOUNDATION_DECLARE_THREAD_LOCAL
instead.

\def FOUNDATION_DEPRECATED
Deprecated attribute, marking a function/variable as deprecated

\def FOUNDATION_FORCEINLINE
Attribute to force function to be inlined

\def FOUNDATION_NOINLINE
Attribute to prevent function from being inlined

\def FOUNDATION_PURECALL
Attribute declaring function to be pure, meaning it has no effects except the return value
and the return value depends only on the parameters and/or global variables.

\def FOUNDATION_CONSTCALL
Attribute declaring function to be const, meaning it does not examine any values except
the arguments, and has no effects except the return value. Basically this is just slightly
more strict class than the PURECALL attribute, since function is not allowed to read global
memory. Note that a function that has pointer arguments and examines the data pointed to
must not be declared const. Likewise, a function that calls a non-const function must
usually not be const. It does not make sense for a const function to return void.

\def FOUNDATION_ALIGN
Variable or type attribute declaring the variable/type to have the specifiedmemory alignment
\param alignment Alignment

\def FOUNDATION_ALIGNOF
Get the alignment of the given type or variable
\param type Type or variable

\def FOUNDATION_ALIGNED_STRUCT
Declare an aligned struct type
\param name Struct name
\param alignment Alignment

\typedef float32_t
Floating point type guaranteed to be 32-bit in size

\typedef float64_t
Floating point type guaranteed to be 64-bit in size

\typedef real
Floating point type of the size chosen in build config (32 or 64 bit). See
#FOUNDATION_SIZE_REAL for declaring size used.

\def FLOAT32_C
Declare a 32-bit floating point constant. Use for automatic suffixing, for example
FLOAT32_C(1.0)
\param x Constant value

\def FLOAT64_C
Declare a 64-bit floating point constant. Use for automatic suffixing, for example
FLOAT64_C(1.0)
\param x Constant value

\def REAL_C
Declare a real constant. Use for automatic suffixing depending on floating point notation used,
for example REAL_C(1.0)
\param x Constant value

\struct uint128_t
128-bit unsigned integer type

\struct uint256_t
256-bit unsigned integer type

\struct atomic32_t
32-bit atomic integer, use atomic_* functions to load/store values atomically
(see atomic.h documentation)

\struct atomic64_t
64-bit atomic integer, use atomic_* functions to load/store values atomically
(see atomic.h documentation)

\struct atomicptr_t
Atomic pointer, use atomic_* functions to load/store values atomically
(see atomic.h documentation)

\fn uint128_t uint128_make(const uint64_t high, const uint64_t low)
Declare a 128-bit unsigned int value from high and high 64-bit components
\param high    High 64 bits
\param low     Low 64 bits
\return        128-bit integer value

\fn bool uint128_equal(const uint128_t u0, const uint128_t u1)
Query if two 128-bit unsigned int values are equal
\param u0      First value
\param u1      Second value
\return        true if values are equal, false if not

\fn uint128_t uint128_null(void)
Declare a zero (null) 128-bit unsigned int value
\return        Zero 128-bit value

\fn bool uint128_is_null(const uint128_t u0)
Query if a 128-bit unsigned int value is zero (null)
\param u0      value
\return        true if value is zero (null), false if not

\fn uint256_t uint256_make(const uint64_t w0, const uint64_t w1, const uint64_t w2, const uint64_t w3)
Declare a 256-bit unsigned int value from four 64-bit components (high order word first, low order word last)
\param w0      High 64 bits word
\param w1      64 bit word
\param w2      64 bit word
\param w3      Low 64 bit word
\return        256-bit integer value

\fn bool uint256_equal(const uint256_t u0, const uint256_t u1)
Query if two 256-bit unsigned int values are equal
\param u0      First value
\param u1      Second value
\return        true if values are equal, false if not

\fn uint256_t uint256_null(void)
Declare a zero (null) 256-bit unsigned int value
\return        Zero 256-bit value

\fn bool uint256_is_null(const uint256_t u0)
Query if a 256-bit unsigned int value is zero (null)
\param u0      Value
\return        true if value is zero (null), false if not

\fn uint256_t uint512_make(const uint64_t w0, const uint64_t w1, const uint64_t w2, const uint64_t w3, const uint64_t
w4, const uint64_t w5, const uint64_t w6, const uint64_t w7) Declare a 512-bit unsigned int value from eight 64-bit
components (high order word first, low order word last) \param w0      High 64 bits word \param w1      64 bit word
\param w2      64 bit word
\param w3      64 bit word
\param w4      64 bit word
\param w5      64 bit word
\param w6      64 bit word
\param w7      Low 64 bit word
\return        512-bit integer value

\fn bool uint512_equal(const uint512_t u0, const uint512_t u1)
Query if two 512-bit unsigned int values are equal
\param u0      First value
\param u1      Second value
\return        true if values are equal, false if not

\fn uint512_t uint512_null(void)
Declare a zero (null) 512-bit unsigned int value
\return        Zero 512-bit value

\fn bool uint512_is_null(const uint512_t u0)
Query if a 512-bit unsigned int value is zero (null)
\param u0      Value
\return        true if value is zero (null), false if not

\def pointer_offset
Offset a non-const pointer the given number of bytes, disregarding type of pointer
\param ptr     Pointer
\param ofs     Offset in bytes (positive or negative)

\def pointer_offset_const
Offset a const pointer the given number of bytes, disregarding type of pointer
\param ptr     Pointer
\param ofs     Offset in bytes (positive or negative)

\def pointer_diff
Calculate the offset in bytes between two pointers (from first to second), disregarding
type of pointer
\param first   First pointer
\param second  Second pointer

\def PRIreal
Printf-style format declaration for a real variable.
Use like other standard PRI* format specifiers, like
<code>string_format( "Value: %" PRIreal, realval );</code>

\def PRItick
Printf-style format declaration for a tick_t variable.
Use like other standard PRI* format specifiers, like
<code>string_format( "Value: %" PRItick, tickval );</code>

\def PRIhash
Printf-style format declaration for a hash_t variable.
Use like other standard PRI* format specifiers, like
<code>string_format( "Value: %" PRIhash, hashval );</code>

\def PRIfixPTR
Printf-style format declaration for a pointer variable producing a fixed-size string
(padding with zeroes). Use like other standard PRI* format specifiers, like
<code>string_format( "Value: %" PRIfixPTR, ptr );</code>

\def STRING_CONST
Expand to two arguments, string pointer and length, as in <code>s, sizeof(s)-1</code>.
Useful when declaring constant strings as arguments to string functions, for example
<code>string_hash(STRING_CONST("foo"))</code>
\param s Constant string declaration

\def STRING_ARGS
Expand to two arguments, string pointer and length, as in <code>s.str, s.length</code>.
Useful when passing a string_t to string functions, for example
<code>string_t mystr = ...; string_hash(STRING_ARGS(mystr));</code>

\def STRING_ARGS_CAPACITY
Expand to three arguments, string pointer, length and capacity, as in
<code>s.str, s.length, s.length+1</code>

\def STRING_FORMAT
Expand to two arguments, legnth and string pointer, as in <code>(int)s.length, s.str</code>.
Useful when passing a string_t to a string format argument, for example
<code>string_t mystr = ...; log_infof(0, STRING_CONST("Mystring: %.*s"), STRING_FORMAT(mystr));</code>

*/
