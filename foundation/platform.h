/* platform.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#if !defined( FOUNDATION_COMPILE )
#  define FOUNDATION_COMPILE 0
#endif

#if FOUNDATION_COMPILE
#  ifdef __cplusplus
#  define FOUNDATION_EXTERN extern "C"
#  define FOUNDATION_API extern "C"
#  else
#  define FOUNDATION_EXTERN extern
#  define FOUNDATION_API extern
#  endif
#else
#  ifdef __cplusplus
#  define FOUNDATION_EXTERN extern "C"
#  define FOUNDATION_API extern "C"
#  else
#  define FOUNDATION_EXTERN extern
#  define FOUNDATION_API extern
#  endif
#endif

//Platforms
#define FOUNDATION_PLATFORM_ANDROID 0
#define FOUNDATION_PLATFORM_BSD 0
#define FOUNDATION_PLATFORM_IOS 0
#define FOUNDATION_PLATFORM_IOS_SIMULATOR 0
#define FOUNDATION_PLATFORM_LINUX 0
#define FOUNDATION_PLATFORM_LINUX_RASPBERRYPI 0
#define FOUNDATION_PLATFORM_MACOSX 0
#define FOUNDATION_PLATFORM_WINDOWS 0
#define FOUNDATION_PLATFORM_PNACL 0
#define FOUNDATION_PLATFORM_TIZEN 0

//Platform traits and groups
#define FOUNDATION_PLATFORM_APPLE 0
#define FOUNDATION_PLATFORM_POSIX 0

#define FOUNDATION_PLATFORM_FAMILY_MOBILE 0
#define FOUNDATION_PLATFORM_FAMILY_DESKTOP 0
#define FOUNDATION_PLATFORM_FAMILY_CONSOLE 0

//Architectures
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

//Architecture details
#define FOUNDATION_ARCH_SSE2 0
#define FOUNDATION_ARCH_SSE3 0
#define FOUNDATION_ARCH_SSE4 0
#define FOUNDATION_ARCH_SSE4_FMA3 0
#define FOUNDATION_ARCH_NEON 0
#define FOUNDATION_ARCH_THUMB 0

#define FOUNDATION_ARCH_ENDIAN_LITTLE 0
#define FOUNDATION_ARCH_ENDIAN_BIG 0

//Compilers
#define FOUNDATION_COMPILER_CLANG 0
#define FOUNDATION_COMPILER_GCC 0
#define FOUNDATION_COMPILER_MSVC 0
#define FOUNDATION_COMPILER_INTEL 0

//First, platforms and architectures

#if defined( __pnacl__ )

#  undef  FOUNDATION_PLATFORM_PNACL
#  define FOUNDATION_PLATFORM_PNACL 1

#  define FOUNDATION_PLATFORM_NAME "PNaCl"
#  define FOUNDATION_PLATFORM_DESCRIPTION "PNaCl"

#  undef  FOUNDATION_ARCH_GENERIC
#  define FOUNDATION_ARCH_GENERIC 1

#  undef  FOUNDATION_ARCH_ENDIAN_LITTLE
#  define FOUNDATION_ARCH_ENDIAN_LITTLE 1

#  ifdef __STRICT_ANSI__
#    undef __STRICT_ANSI__
#  endif

// Android
#elif defined( __ANDROID__ )

#  undef  FOUNDATION_PLATFORM_ANDROID
#  define FOUNDATION_PLATFORM_ANDROID 1

// Compatibile platforms
#  undef  FOUNDATION_PLATFORM_POSIX
#  define FOUNDATION_PLATFORM_POSIX 1

#  define FOUNDATION_PLATFORM_NAME "Android"

// Architecture and detailed description
#  if defined( __arm__ )
#    undef  FOUNDATION_ARCH_ARM
#    define FOUNDATION_ARCH_ARM 1
#    ifdef __ARM_ARCH_7A__
#      undef  FOUNDATION_ARCH_ARM7
#      define FOUNDATION_ARCH_ARM7 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "Android ARMv7"
#    elif defined(__ARM_ARCH_5TE__)
#      undef  FOUNDATION_ARCH_ARM5
#      define FOUNDATION_ARCH_ARM5 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "Android ARMv5"
#    else
#      error Unsupported ARM architecture
#    endif
#  elif defined( __aarch64__ )
#    undef  FOUNDATION_ARCH_ARM
#    define FOUNDATION_ARCH_ARM 1
#    undef  FOUNDATION_ARCH_ARM_64
#    define FOUNDATION_ARCH_ARM_64 1
//Assume ARMv8 for now
//#    if defined( __ARM_ARCH ) && ( __ARM_ARCH == 8 )
#      undef  FOUNDATION_ARCH_ARM8_64
#      define FOUNDATION_ARCH_ARM8_64 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "Android ARM64v8"
//#    else
//#      error Unrecognized AArch64 architecture
//#    endif
#  elif defined( __i386__ )
#    undef  FOUNDATION_ARCH_X86
#    define FOUNDATION_ARCH_X86 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Android x86"
#  elif defined( __x86_64__ )
#    undef  FOUNDATION_ARCH_X86_64
#    define FOUNDATION_ARCH_X86_64 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Android x86-64"
#  elif ( defined( __mips__ ) && defined( __mips64 ) )
#    undef  FOUNDATION_ARCH_MIPS
#    define FOUNDATION_ARCH_MIPS 1
#    undef  FOUNDATION_ARCH_MIPS_64
#    define FOUNDATION_ARCH_MIPS_64 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Android MIPS64"
#    ifndef _MIPS_ISA
#      define _MIPS_ISA 7 /*_MIPS_ISA_MIPS64*/
#    endif
#  elif defined( __mips__ )
#    undef  FOUNDATION_ARCH_MIPS
#    define FOUNDATION_ARCH_MIPS 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Android MIPS"
#    ifndef _MIPS_ISA
#      define _MIPS_ISA 6 /*_MIPS_ISA_MIPS32*/
#    endif
#  else
#    error Unknown architecture
#  endif

// Traits
#  if FOUNDATION_ARCH_MIPS
#    if defined( __MIPSEL__ ) || defined( __MIPSEL ) || defined( _MIPSEL )
#      undef  FOUNDATION_ARCH_ENDIAN_LITTLE
#      define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#    else
#      undef  FOUNDATION_ARCH_ENDIAN_BIG
#      define FOUNDATION_ARCH_ENDIAN_BIG 1
#    endif
#  elif defined( __AARCH64EB__ ) || defined( __ARMEB__ )
#    undef  FOUNDATION_ARCH_ENDIAN_BIG
#    define FOUNDATION_ARCH_ENDIAN_BIG 1
#  else
#    undef  FOUNDATION_ARCH_ENDIAN_LITTLE
#    define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#  endif

#  undef  FOUNDATION_PLATFORM_FAMILY_MOBILE
#  define FOUNDATION_PLATFORM_FAMILY_MOBILE 1

#  undef  FOUNDATION_PLATFORM_FAMILY_CONSOLE
#  define FOUNDATION_PLATFORM_FAMILY_CONSOLE 1

// Tizen
#elif defined( __TIZEN__ )

#  undef  FOUNDATION_PLATFORM_TIZEN
#  define FOUNDATION_PLATFORM_TIZEN 1

// Compatibile platforms
#  undef  FOUNDATION_PLATFORM_POSIX
#  define FOUNDATION_PLATFORM_POSIX 1

#  define FOUNDATION_PLATFORM_NAME "Tizen"

// Architecture and detailed description
#  if defined( __arm__ )
#    undef  FOUNDATION_ARCH_ARM
#    define FOUNDATION_ARCH_ARM 1
#    ifdef __ARM_ARCH_7A__
#      undef  FOUNDATION_ARCH_ARM7
#      define FOUNDATION_ARCH_ARM7 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "Tizen ARMv7"
#    elif defined(__ARM_ARCH_5TE__)
#      undef  FOUNDATION_ARCH_ARM5
#      define FOUNDATION_ARCH_ARM5 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "Tizen ARMv5"
#    else
#      error Unsupported ARM architecture
#    endif
#  elif defined( __i386__ )
#    undef  FOUNDATION_ARCH_X86
#    define FOUNDATION_ARCH_X86 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Tizen x86"
#  elif defined( __x86_64__ )
#    undef  FOUNDATION_ARCH_X86_64
#    define FOUNDATION_ARCH_X86_64 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Tizen x86-64"
#  else
#    error Unknown architecture
#  endif

// Traits
#  if defined( __AARCH64EB__ ) || defined( __ARMEB__ )
#    undef  FOUNDATION_ARCH_ENDIAN_BIG
#    define FOUNDATION_ARCH_ENDIAN_BIG 1
#  else
#    undef  FOUNDATION_ARCH_ENDIAN_LITTLE
#    define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#  endif

#  undef  FOUNDATION_PLATFORM_FAMILY_MOBILE
#  define FOUNDATION_PLATFORM_FAMILY_MOBILE 1

#  undef  FOUNDATION_PLATFORM_FAMILY_CONSOLE
#  define FOUNDATION_PLATFORM_FAMILY_CONSOLE 1

// MacOS X and iOS
#elif ( defined( __APPLE__ ) && __APPLE__ )

#  undef  FOUNDATION_PLATFORM_APPLE
#  define FOUNDATION_PLATFORM_APPLE 1

#  undef  FOUNDATION_PLATFORM_POSIX
#  define FOUNDATION_PLATFORM_POSIX 1

#  include <TargetConditionals.h>

#  if defined( __IPHONE__ ) || ( defined( TARGET_OS_IPHONE ) && TARGET_OS_IPHONE ) || ( defined( TARGET_IPHONE_SIMULATOR ) && TARGET_IPHONE_SIMULATOR )

#    undef  FOUNDATION_PLATFORM_IOS
#    define FOUNDATION_PLATFORM_IOS 1

#    define FOUNDATION_PLATFORM_NAME "iOS"

#    if defined( __arm__ )
#      undef  FOUNDATION_ARCH_ARM
#      define FOUNDATION_ARCH_ARM 1
#      if defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7S__)
#        undef  FOUNDATION_ARCH_ARM7
#        define FOUNDATION_ARCH_ARM7 1
#        define FOUNDATION_PLATFORM_DESCRIPTION "iOS ARMv7"
#        ifndef __ARM_NEON__
#          error Missing ARM NEON support
#        endif
#      elif defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6__)
#        undef  FOUNDATION_ARCH_ARM6
#        define FOUNDATION_ARCH_ARM6 1
#        define FOUNDATION_PLATFORM_DESCRIPTION "iOS ARMv6"
#      else
#        error Unrecognized ARM architecture
#      endif
#    elif defined( __arm64__ )
#      undef  FOUNDATION_ARCH_ARM
#      define FOUNDATION_ARCH_ARM 1
#      undef  FOUNDATION_ARCH_ARM_64
#      define FOUNDATION_ARCH_ARM_64 1
#      if defined( __ARM64_ARCH_8__ )
#        undef  FOUNDATION_ARCH_ARM8_64
#        define FOUNDATION_ARCH_ARM8_64 1
#        define FOUNDATION_PLATFORM_DESCRIPTION "iOS ARM64v8"
#      else
#        error Unrecognized ARM architecture
#      endif
#    elif defined( __i386__ )
#      undef  FOUNDATION_PLATFORM_IOS_SIMULATOR
#      define FOUNDATION_PLATFORM_IOS_SIMULATOR 1
#      undef  FOUNDATION_ARCH_X86
#      define FOUNDATION_ARCH_X86 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "iOS x86 (simulator)"
#    elif defined( __x86_64__ )
#      undef  FOUNDATION_PLATFORM_IOS_SIMULATOR
#      define FOUNDATION_PLATFORM_IOS_SIMULATOR 1
#      undef  FOUNDATION_ARCH_X86_64
#      define FOUNDATION_ARCH_X86_64 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "iOS x86_64 (simulator)"
#    else
#      error Unknown architecture
#    endif

#    undef  FOUNDATION_ARCH_ENDIAN_LITTLE
#    define FOUNDATION_ARCH_ENDIAN_LITTLE 1

#    undef  FOUNDATION_PLATFORM_FAMILY_MOBILE
#    define FOUNDATION_PLATFORM_FAMILY_MOBILE 1

#    undef  FOUNDATION_PLATFORM_FAMILY_CONSOLE
#    define FOUNDATION_PLATFORM_FAMILY_CONSOLE 1

#  elif defined( __MACH__ )

#    undef  FOUNDATION_PLATFORM_MACOSX
#    define FOUNDATION_PLATFORM_MACOSX 1

#    define FOUNDATION_PLATFORM_NAME "MacOSX"

#    if defined( __x86_64__ ) ||  defined( __x86_64 ) || defined( __amd64 )
#      undef  FOUNDATION_ARCH_X86_64
#      define FOUNDATION_ARCH_X86_64 1
#      undef  FOUNDATION_ARCH_ENDIAN_LITTLE
#      define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "MacOSX x86-64"
#    elif defined( __i386__ ) || defined( __intel__ )
#      undef  FOUNDATION_ARCH_X86
#      define FOUNDATION_ARCH_X86 1
#      undef  FOUNDATION_ARCH_ENDIAN_LITTLE
#      define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "MacOSX x86"

#    elif defined( __powerpc64__ ) || defined( __POWERPC64__ )
#      undef  FOUNDATION_ARCH_PPC_64
#      define FOUNDATION_ARCH_PPC_64 1
#      undef  FOUNDATION_ARCH_ENDIAN_BIG
#      define FOUNDATION_ARCH_ENDIAN_BIG 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "MacOSX PPC64"
#    elif defined( __powerpc__ ) || defined( __POWERPC__ )
#      undef  FOUNDATION_ARCH_PPC
#      define FOUNDATION_ARCH_PPC 1
#      undef  FOUNDATION_ARCH_ENDIAN_BIG
#      define FOUNDATION_ARCH_ENDIAN_BIG 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "MacOSX PPC"

#    else
#      error Unknown architecture
#    endif

#    undef  FOUNDATION_PLATFORM_FAMILY_DESKTOP
#    define FOUNDATION_PLATFORM_FAMILY_DESKTOP 1

#  else
#    error Unknown Apple Platform
#  endif

// Linux
#elif ( defined( __linux__ ) || defined( __linux ) )

#  undef  FOUNDATION_PLATFORM_LINUX
#  define FOUNDATION_PLATFORM_LINUX 1

#  undef  FOUNDATION_PLATFORM_POSIX
#  define FOUNDATION_PLATFORM_POSIX 1

#  define FOUNDATION_PLATFORM_NAME "Linux"

#  if defined( __x86_64__ ) || defined( __x86_64 ) || defined( __amd64 )
#    undef  FOUNDATION_ARCH_X86_64
#    define FOUNDATION_ARCH_X86_64 1
#    undef  FOUNDATION_ARCH_ENDIAN_LITTLE
#    define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Linux x86-64"
#  elif defined( __i386__ ) || defined( __intel__ ) || defined( _M_IX86 )
#    undef  FOUNDATION_ARCH_X86
#    define FOUNDATION_ARCH_X86 1
#    undef  FOUNDATION_ARCH_ENDIAN_LITTLE
#    define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Linux x86"

#  elif defined( __powerpc64__ ) || defined( __POWERPC64__ )
#    undef  FOUNDATION_ARCH_PPC_64
#    define FOUNDATION_ARCH_PPC_64 1
#    undef  FOUNDATION_ARCH_ENDIAN_BIG
#    define FOUNDATION_ARCH_ENDIAN_BIG 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Linux PPC64"
#  elif defined( __powerpc__ ) || defined( __POWERPC__ )
#    undef  FOUNDATION_ARCH_PPC
#    define FOUNDATION_ARCH_PPC 1
#    undef  FOUNDATION_ARCH_ENDIAN_BIG
#    define FOUNDATION_ARCH_ENDIAN_BIG 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Linux PPC"
#  elif defined( __arm__ )
#    undef  FOUNDATION_ARCH_ARM
#    define FOUNDATION_ARCH_ARM 1
#    if defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7S__)
#      undef  FOUNDATION_ARCH_ARM7
#      define FOUNDATION_ARCH_ARM7 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "Linux ARMv7"
#      ifndef __ARM_NEON__
#        error Missing ARM NEON support
#      endif
#    elif defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6ZK__)
#      undef  FOUNDATION_ARCH_ARM6
#      define FOUNDATION_ARCH_ARM6 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "Linux ARMv6"
#    else
#      error Unrecognized ARM architecture
#    endif

// Traits
#    if defined( __ARMEB__ )
#      undef  FOUNDATION_ARCH_ENDIAN_BIG
#      define FOUNDATION_ARCH_ENDIAN_BIG 1
#    else
#      undef  FOUNDATION_ARCH_ENDIAN_LITTLE
#      define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#    endif

#  elif defined( __arm64__ ) || defined( __aarch64__ )
#    undef  FOUNDATION_ARCH_ARM
#    define FOUNDATION_ARCH_ARM 1
#    undef  FOUNDATION_ARCH_ARM_64
#    define FOUNDATION_ARCH_ARM_64 1
#    undef  FOUNDATION_ARCH_ENDIAN_LITTLE
#    define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#    if defined( __ARM64_ARCH_8__ )
#      undef  FOUNDATION_ARCH_ARM8_64
#      define FOUNDATION_ARCH_ARM8_64 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "Linux ARM64v8"
#    else
#      error Unrecognized ARM architecture
#    endif

// Traits
#    if defined( __AARCH64EB__ )
#      undef  FOUNDATION_ARCH_ENDIAN_BIG
#      define FOUNDATION_ARCH_ENDIAN_BIG 1
#    else
#      undef  FOUNDATION_ARCH_ENDIAN_LITTLE
#      define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#    endif

#  else
#    error Unknown architecture
#  endif

#  if defined( __raspberrypi__ )
#    undef  FOUNDATION_PLATFORM_LINUX_RASPBERRYPI
#    define FOUNDATION_PLATFORM_LINUX_RASPBERRYPI 1
#  endif

#  undef  FOUNDATION_PLATFORM_FAMILY_DESKTOP
#  define FOUNDATION_PLATFORM_FAMILY_DESKTOP 1

//BSD family
#elif ( defined( __BSD__ ) || defined( __FreeBSD__ ) )

#  undef  FOUNDATION_PLATFORM_BSD
#  define FOUNDATION_PLATFORM_BSD 1

#  undef  FOUNDATION_PLATFORM_POSIX
#  define FOUNDATION_PLATFORM_POSIX 1

#  define FOUNDATION_PLATFORM_NAME "BSD"

#  if defined( __x86_64__ ) || defined( __x86_64 ) || defined( __amd64 )
#    undef  FOUNDATION_ARCH_X86_64
#    define FOUNDATION_ARCH_X86_64 1
#    undef  FOUNDATION_ARCH_ENDIAN_LITTLE
#    define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "BSD x86-64"
#  elif defined( __i386__ ) || defined( __intel__ ) || defined( _M_IX86 )
#    undef  FOUNDATION_ARCH_X86
#    define FOUNDATION_ARCH_X86 1
#    undef  FOUNDATION_ARCH_ENDIAN_LITTLE
#    define FOUNDATION_ARCH_ENDIAN_LITTLE 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "BSD x86"

#  elif defined( __powerpc64__ ) || defined( __POWERPC64__ )
#    undef  FOUNDATION_ARCH_PPC_64
#    define FOUNDATION_ARCH_PPC_64 1
#    undef  FOUNDATION_ARCH_ENDIAN_BIG
#    define FOUNDATION_ARCH_ENDIAN_BIG 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "BSD PPC64"
#  elif defined( __powerpc__ ) || defined( __POWERPC__ )
#    undef  FOUNDATION_ARCH_PPC
#    define FOUNDATION_ARCH_PPC 1
#    undef  FOUNDATION_ARCH_ENDIAN_BIG
#    define FOUNDATION_ARCH_ENDIAN_BIG 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "BSD PPC"

#  else
#    error Unknown architecture
#  endif

#  undef  FOUNDATION_PLATFORM_FAMILY_DESKTOP
#  define FOUNDATION_PLATFORM_FAMILY_DESKTOP 1

// Windows
#elif defined( _WIN32 ) || defined( __WIN32__ ) || defined( _WIN64 )

#  undef  FOUNDATION_PLATFORM_WINDOWS
#  define FOUNDATION_PLATFORM_WINDOWS 1

#  define FOUNDATION_PLATFORM_NAME "Windows"

#  if defined( __x86_64__ ) || defined( _M_AMD64 ) || defined( _AMD64_ )
#    undef  FOUNDATION_ARCH_X86_64
#    define FOUNDATION_ARCH_X86_64 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Windows x86-64"
#  elif defined( __x86__ ) || defined( _M_IX86 ) || defined( _X86_ )
#    undef  FOUNDATION_ARCH_X86
#    define FOUNDATION_ARCH_X86 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Windows x86"
#  elif defined( __ia64__ ) || defined( _M_IA64 ) || defined( _IA64_ )
#    undef  FOUNDATION_ARCH_IA64
#    define FOUNDATION_ARCH_IA64 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Windows IA-64"

#  else
#    error Unknown architecture
#  endif

#  undef  FOUNDATION_ARCH_ENDIAN_LITTLE
#  define FOUNDATION_ARCH_ENDIAN_LITTLE 1

#  undef  FOUNDATION_PLATFORM_FAMILY_DESKTOP
#  define FOUNDATION_PLATFORM_FAMILY_DESKTOP 1

#  if defined( FOUNDATION_COMPILE ) && FOUNDATION_COMPILE && !defined( _CRT_SECURE_NO_WARNINGS )
#    define _CRT_SECURE_NO_WARNINGS 1
#  endif

#else
#  error Unknown platform
#endif

//Utility macros
#define FOUNDATION_PREPROCESSOR_TOSTRING( x )          FOUNDATION_PREPROCESSOR_TOSTRING_WRAP(x)
#define FOUNDATION_PREPROCESSOR_TOSTRING_WRAP( x )     #x

#define FOUNDATION_PREPROCESSOR_JOIN( a, b )           FOUNDATION_PREPROCESSOR_JOIN_WRAP( a, b )
#define FOUNDATION_PREPROCESSOR_JOIN_WRAP( a, b )      FOUNDATION_PREPROCESSOR_JOIN_INTERNAL( a, b )
#define FOUNDATION_PREPROCESSOR_JOIN_INTERNAL( a, b )  a##b

#define FOUNDATION_PREPROCESSOR_NARGS( ... )           FOUNDATION_PREPROCESSOR_NARGS_WRAP( __VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 )
#define FOUNDATION_PREPROCESSOR_NARGS_WRAP( _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _, ... ) _

//Architecture details
#ifdef __SSE2__
#  undef  FOUNDATION_ARCH_SSE2
#  define FOUNDATION_ARCH_SSE2 1
#endif

#ifdef __SSE3__
#  undef  FOUNDATION_ARCH_SSE3
#  define FOUNDATION_ARCH_SSE3 1
#endif

#ifdef __SSE4_1__
#  undef  FOUNDATION_ARCH_SSE4
#  define FOUNDATION_ARCH_SSE4 1
#endif

#ifdef __ARM_NEON__
#  undef  FOUNDATION_ARCH_NEON
#  define FOUNDATION_ARCH_NEON 1
#endif

#ifdef __thumb__
#  undef  FOUNDATION_ARCH_THUMB
#  define FOUNDATION_ARCH_THUMB 1
#endif

//Compilers

#if defined( __clang__ )

#  undef  FOUNDATION_COMPILER_CLANG
#  define FOUNDATION_COMPILER_CLANG 1

#  define FOUNDATION_COMPILER_NAME "clang"
#  define FOUNDATION_COMPILER_DESCRIPTION FOUNDATION_COMPILER_NAME " " FOUNDATION_PREPROCESSOR_TOSTRING( __clang_major__ ) "." FOUNDATION_PREPROCESSOR_TOSTRING( __clang_minor__ )

#  define FOUNDATION_RESTRICT __restrict
#  if FOUNDATION_PLATFORM_WINDOWS
#    define FOUNDATION_THREADLOCAL
#  else
#    define FOUNDATION_THREADLOCAL __thread
#  endif

#  define FOUNDATION_ATTRIBUTE(x) __attribute__((__##x##__))
#  define FOUNDATION_ATTRIBUTE2(x,y) __attribute__((__##x##__(y)))
#  define FOUNDATION_ATTRIBUTE3(x,y,z) __attribute__((__##x##__(y,z)))
#  define FOUNDATION_ATTRIBUTE4(x,y,z,w) __attribute__((__##x##__(y,z,w)))

#  define FOUNDATION_DEPRECATED FOUNDATION_ATTRIBUTE( deprecated )
#  define FOUNDATION_FORCEINLINE inline FOUNDATION_ATTRIBUTE( always_inline )
#  define FOUNDATION_NOINLINE FOUNDATION_ATTRIBUTE( noinline )
#  define FOUNDATION_PURECALL FOUNDATION_ATTRIBUTE( pure )
#  define FOUNDATION_CONSTCALL FOUNDATION_ATTRIBUTE( const )
#  define FOUNDATION_ALIGN( alignment ) FOUNDATION_ATTRIBUTE2( aligned, alignment )
#  define FOUNDATION_ALIGNOF( type ) __alignof__( type )
#  define FOUNDATION_ALIGNED_STRUCT( name, alignment ) struct __attribute__((__aligned__(alignment))) name

#  if FOUNDATION_PLATFORM_WINDOWS
#    define STDCALL
#    ifndef __USE_MINGW_ANSI_STDIO
#      define __USE_MINGW_ANSI_STDIO 1
#    endif
#    ifndef _CRT_SECURE_NO_WARNINGS
#      define _CRT_SECURE_NO_WARNINGS 1
#    endif
#    ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#      define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 0
#    endif
#    ifndef _MSC_VER
#      define _MSC_VER 1300
#    endif
#    define USE_NO_MINGW_SETJMP_TWO_ARGS 1
#  endif

#elif defined( __GNUC__ )

#  undef  FOUNDATION_COMPILER_GCC
#  define FOUNDATION_COMPILER_GCC 1

#  define FOUNDATION_COMPILER_NAME "gcc"
#  define FOUNDATION_COMPILER_DESCRIPTION FOUNDATION_COMPILER_NAME " " FOUNDATION_PREPROCESSOR_TOSTRING( __GNUC__ ) "." FOUNDATION_PREPROCESSOR_TOSTRING( __GNUC_MINOR__ )

#  define FOUNDATIN_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#  define FOUNDATION_RESTRICT __restrict
#  define FOUNDATION_THREADLOCAL __thread

#  define FOUNDATION_ATTRIBUTE(x) __attribute__((__##x##__))
#  define FOUNDATION_ATTRIBUTE2(x,y) __attribute__((__##x##__(y)))
#  define FOUNDATION_ATTRIBUTE3(x,y,z) __attribute__((__##x##__(y,z)))
#  define FOUNDATION_ATTRIBUTE4(x,y,z,w) __attribute__((__##x##__(y,z,w)))

#  define FOUNDATION_DEPRECATED FOUNDATION_ATTRIBUTE( deprecated )
#  define FOUNDATION_FORCEINLINE inline FOUNDATION_ATTRIBUTE( always_inline )
#  define FOUNDATION_NOINLINE FOUNDATION_ATTRIBUTE( noinline )
#  define FOUNDATION_PURECALL FOUNDATION_ATTRIBUTE( pure )
#  define FOUNDATION_CONSTCALL FOUNDATION_ATTRIBUTE( const )
#  define FOUNDATION_ALIGN( alignment ) FOUNDATION_ATTRIBUTE2( aligned, alignment )
#  define FOUNDATION_ALIGNOF( type ) __alignof__( type )
#  define FOUNDATION_ALIGNED_STRUCT( name, alignment ) struct FOUNDATION_ALIGN( alignment ) name

#  if FOUNDATION_PLATFORM_WINDOWS
#    define STDCALL
#    ifndef __USE_MINGW_ANSI_STDIO
#      define __USE_MINGW_ANSI_STDIO 1
#    endif
#    ifndef _CRT_SECURE_NO_WARNINGS
#      define _CRT_SECURE_NO_WARNINGS 1
#    endif
#    ifndef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
#      define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 0
#    endif
#    ifndef _MSC_VER
#      define _MSC_VER 1300
#    endif
#  endif

// Intel
#elif defined( __ICL ) || defined( __ICC ) || defined( __INTEL_COMPILER )

#  undef  FOUNDATION_COMPILER_INTEL
#  define FOUNDATION_COMPILER_INTEL 1

#  define FOUNDATION_COMPILER_NAME "intel"
#  if defined( __ICL )
#    define FOUNDATION_COMPILER_DESCRIPTION FOUNDATION_COMPILER_NAME " " FOUNDATION_PREPROCESSOR_TOSTRING( __ICL )
#  elif defined( __ICC )
#    define FOUNDATION_COMPILER_DESCRIPTION FOUNDATION_COMPILER_NAME " " FOUNDATION_PREPROCESSOR_TOSTRING( __ICC )
#  endif

#  define FOUNDATION_RESTRICT __restrict
#  define FOUNDATION_THREADLOCAL __declspec( thread )

#  define FOUNDATION_ATTRIBUTE(x)
#  define FOUNDATION_ATTRIBUTE2(x,y)
#  define FOUNDATION_ATTRIBUTE3(x,y,z)
#  define FOUNDATION_ATTRIBUTE4(x,y,z,w)

#  define FOUNDATION_DEPRECATED
#  define FOUNDATION_FORCEINLINE __forceinline
#  define FOUNDATION_NOINLINE __declspec( noinline )
#  define FOUNDATION_PURECALL
#  define FOUNDATION_CONSTCALL
#  define FOUNDATION_ALIGN( alignment ) __declspec( align( alignment ) )
#  define FOUNDATION_ALIGNOF( type ) __alignof( type )
#  define FOUNDATION_ALIGNED_STRUCT( name, alignment ) FOUNDATION_ALIGN( alignment ) struct name

#  if FOUNDATION_PLATFORM_WINDOWS
#    define STDCALL __stdcall
#    define va_copy(d,s) ((d)=(s))
#  endif

#  include <intrin.h>

#  define bool _Bool
#  define true 1
#  define false 0
#  define __bool_true_false_are_defined 1

// Microsoft
#elif defined( _MSC_VER )

#  undef  FOUNDATION_COMPILER_MSVC
#  define FOUNDATION_COMPILER_MSVC 1

#  define FOUNDATION_COMPILER_NAME "msvc"
#  define FOUNDATION_COMPILER_DESCRIPTION FOUNDATION_COMPILER_NAME " " FOUNDATION_PREPROCESSOR_TOSTRING( _MSC_VER )

#  define FOUNDATION_ATTRIBUTE(x)
#  define FOUNDATION_ATTRIBUTE2(x,y)
#  define FOUNDATION_ATTRIBUTE3(x,y,z)
#  define FOUNDATION_ATTRIBUTE4(x,y,z,w)

#  define FOUNDATION_RESTRICT __restrict
#  define FOUNDATION_THREADLOCAL __declspec( thread )

#  define FOUNDATION_DEPRECATED __declspec( deprecated )
#  define FOUNDATION_FORCEINLINE __forceinline
#  define FOUNDATION_NOINLINE __declspec( noinline )
#  define FOUNDATION_PURECALL
#  define FOUNDATION_CONSTCALL
#  define FOUNDATION_ALIGN( alignment ) __declspec( align( alignment ) )
#  define FOUNDATION_ALIGNOF( type ) __alignof( type )
#  define FOUNDATION_ALIGNED_STRUCT( name, alignment ) FOUNDATION_ALIGN( alignment ) struct name

#  pragma warning( disable : 4200 )

#  if FOUNDATION_PLATFORM_WINDOWS
#    define STDCALL __stdcall
#  endif

#  ifndef __cplusplus
typedef enum {
  false = 0,
  true  = 1
} bool;
#  endif

#if _MSC_VER < 1800
#  define va_copy(d,s) ((d)=(s))
#endif

#  include <intrin.h>

#else

#  warning Unknown compiler

#  define FOUNDATION_COMPILER_NAME "unknown"
#  define FOUNDATION_COMPILER_DESCRIPTION "unknown"

#  define FOUNDATION_RESTRICT
#  define FOUNDATION_THREADLOCAL

#  define FOUNDATION_DEPRECATED
#  define FOUNDATION_FORCEINLINE
#  define FOUNDATION_NOINLINE
#  define FOUNDATION_PURECALL
#  define FOUNDATION_CONSTCALL
#  define FOUNDATION_ALIGN
#  define FOUNDATION_ALIGNOF
#  define FOUNDATION_ALIGNED_STRUCT( name, alignment ) struct name

typedef enum {
  false = 0,
  true  = 1
} bool;

#endif

#if FOUNDATION_PLATFORM_POSIX

#if FOUNDATION_COMPILER_CLANG
#  pragma clang diagnostic push
#  if __has_warning( "-Wreserved-id-macro" )
#    pragma clang diagnostic ignored "-Wreserved-id-macro"
#  endif
#endif

#ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#endif

#if FOUNDATION_COMPILER_CLANG
#  pragma clang diagnostic pop
#endif

#endif

//Base data types
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <float.h>
#include <limits.h>
#if !FOUNDATION_PLATFORM_WINDOWS
#  include <wchar.h>
#endif
#if FOUNDATION_PLATFORM_PNACL || ( FOUNDATION_PLATFORM_POSIX && !FOUNDATION_PLATFORM_APPLE )
#  include <sys/types.h>
#endif

#define nullptr ((void*)0)

typedef float          float32_t;
typedef double         float64_t;

struct uint128_t {
  uint64_t word[2];
};
typedef struct uint128_t uint128_t;

struct uint256_t {
  uint64_t word[4];
};
typedef struct uint256_t uint256_t;

#define FLOAT32_C(x)   (x##f)
#define FLOAT64_C(x)   (x)

#define FOUNDATION_SIZE_REAL 32U

#if FOUNDATION_SIZE_REAL == 64
typedef   float64_t    real;
#  define REAL_C(x)    FLOAT64_C(x)
#else
typedef   float32_t    real;
#  define REAL_C(x)    FLOAT32_C(x)
#endif

//Pointer size
#if FOUNDATION_ARCH_ARM_64 || FOUNDATION_ARCH_X86_64 || FOUNDATION_ARCH_PPC_64 || FOUNDATION_ARCH_IA64 || FOUNDATION_ARCH_MIPS_64
#  define FOUNDATION_SIZE_POINTER 8U
#else
#  define FOUNDATION_SIZE_POINTER 4U
#endif

//wchar_t size
#if FOUNDATION_PLATFORM_LINUX_RASPBERRYPI
#  define FOUNDATION_SIZE_WCHAR 4U
#else
#  if WCHAR_MAX > 0xffff
#    define FOUNDATION_SIZE_WCHAR 4U
#  else
#    define FOUNDATION_SIZE_WCHAR 2U
#  endif
#endif

//Atomic types
FOUNDATION_ALIGNED_STRUCT(atomic32_t, 4) {
  int32_t nonatomic;
};
typedef struct atomic32_t atomic32_t;

FOUNDATION_ALIGNED_STRUCT(atomic64_t, 8) {
  int64_t nonatomic;
};
typedef struct atomic64_t atomic64_t;

FOUNDATION_ALIGNED_STRUCT(atomicptr_t, FOUNDATION_SIZE_POINTER) {
  void* nonatomic;
};
typedef struct atomicptr_t atomicptr_t;

// Pointer arithmetic
#define pointer_offset( ptr, ofs ) (void*)((char*)(ptr) + (ptrdiff_t)(ofs))
#define pointer_offset_const( ptr, ofs ) (const void*)((const char*)(ptr) + (ptrdiff_t)(ofs))
#define pointer_diff( first, second ) (ptrdiff_t)((const char*)(first) - (const char*)(second))

#include <string.h>

// String argument helpers
#define STRING_CONST( s ) (s), (sizeof((s))-1)
#define STRING_ARGS( s ) (s).str, (s).length
#define STRING_ARGS_CAPACITY( s ) (s).str, (s).length, (s).length+1
#define STRING_FORMAT( s ) (int)(s).length, (s).str

// Misc
#define FOUNDATION_UNUSED(x) ((void)sizeof(x))

#define FOUNDATION_UNUSED_ARGS_1_( arg ) FOUNDATION_UNUSED( arg );
#define FOUNDATION_UNUSED_ARGS_1( ... ) FOUNDATION_UNUSED_ARGS_1_( __VA_ARGS__ )

#define FOUNDATION_UNUSED_ARGS_2_( arg, ... ) FOUNDATION_UNUSED( arg ); FOUNDATION_UNUSED_ARGS_1_( __VA_ARGS__ )
#define FOUNDATION_UNUSED_ARGS_2( ... ) FOUNDATION_UNUSED_ARGS_2_( __VA_ARGS__ )

#define FOUNDATION_UNUSED_ARGS_3_( arg, ... ) FOUNDATION_UNUSED( arg ); FOUNDATION_UNUSED_ARGS_2_( __VA_ARGS__ )
#define FOUNDATION_UNUSED_ARGS_3( ... ) FOUNDATION_UNUSED_ARGS_3_( __VA_ARGS__ )

#define FOUNDATION_UNUSED_ARGS_4_( arg, ... ) FOUNDATION_UNUSED( arg ); FOUNDATION_UNUSED_ARGS_3_( __VA_ARGS__ )
#define FOUNDATION_UNUSED_ARGS_4( ... ) FOUNDATION_UNUSED_ARGS_4_( __VA_ARGS__ )

#define FOUNDATION_UNUSED_ARGS_5_( arg, ... ) FOUNDATION_UNUSED( arg ); FOUNDATION_UNUSED_ARGS_4_( __VA_ARGS__ )
#define FOUNDATION_UNUSED_ARGS_5( ... ) FOUNDATION_UNUSED_ARGS_5_( __VA_ARGS__ )

#define FOUNDATION_UNUSED_ARGS_6_( arg, ... ) FOUNDATION_UNUSED( arg ); FOUNDATION_UNUSED_ARGS_5_( __VA_ARGS__ )
#define FOUNDATION_UNUSED_ARGS_6( ... ) FOUNDATION_UNUSED_ARGS_6_( __VA_ARGS__ )

#define FOUNDATION_UNUSED_VARARGS_1_( arg, ... ) FOUNDATION_UNUSED( arg );
#define FOUNDATION_UNUSED_VARARGS_1( ... ) FOUNDATION_UNUSED_VARARGS_1_( __VA_ARGS__ )

#define FOUNDATION_UNUSED_VARARGS_2_( arg, ... ) FOUNDATION_UNUSED( arg ); FOUNDATION_UNUSED_VARARGS_1_( __VA_ARGS__ )
#define FOUNDATION_UNUSED_VARARGS_2( ... ) FOUNDATION_UNUSED_VARARGS_2_( __VA_ARGS__ )

#define FOUNDATION_UNUSED_VARARGS_3_( arg, ... ) FOUNDATION_UNUSED( arg ); FOUNDATION_UNUSED_VARARGS_2_( __VA_ARGS__ )
#define FOUNDATION_UNUSED_VARARGS_3( ... ) FOUNDATION_UNUSED_VARARGS_3_( __VA_ARGS__ )

#define FOUNDATION_UNUSED_VARARGS_4_( arg, ... ) FOUNDATION_UNUSED( arg ); FOUNDATION_UNUSED_VARARGS_3_( __VA_ARGS__ )
#define FOUNDATION_UNUSED_VARARGS_4( ... ) FOUNDATION_UNUSED_VARARGS_4_( __VA_ARGS__ )

#define FOUNDATION_UNUSED_VARARGS_5_( arg, ... ) FOUNDATION_UNUSED( arg ); FOUNDATION_UNUSED_VARARGS_4_( __VA_ARGS__ )
#define FOUNDATION_UNUSED_VARARGS_5( ... ) FOUNDATION_UNUSED_VARARGS_5_( __VA_ARGS__ )

#define FOUNDATION_UNUSED_VARARGS_6_( arg, ... ) FOUNDATION_UNUSED( arg ); FOUNDATION_UNUSED_VARARGS_5_( __VA_ARGS__ )
#define FOUNDATION_UNUSED_VARARGS_6( ... ) FOUNDATION_UNUSED_VARARGS_6_( __VA_ARGS__ )

// Wrappers for platforms that not yet support thread-local storage declarations
#if FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_ANDROID

// Forward declarations of various system APIs
#if FOUNDATION_PLATFORM_APPLE
typedef __darwin_pthread_key_t _pthread_key_t;
#else
typedef int _pthread_key_t;
#  endif
FOUNDATION_EXTERN int pthread_key_create(_pthread_key_t*, void (*)(void*));
FOUNDATION_EXTERN int pthread_setspecific(_pthread_key_t, const void*);
FOUNDATION_EXTERN void* pthread_getspecific(_pthread_key_t);

FOUNDATION_API void* _allocate_thread_local_block(size_t size);

#define FOUNDATION_DECLARE_THREAD_LOCAL( type, name, init ) \
static _pthread_key_t _##name##_key = 0; \
static FOUNDATION_FORCEINLINE _pthread_key_t get_##name##_key( void ) { if( !_##name##_key ) { pthread_key_create( &_##name##_key, 0 ); pthread_setspecific( _##name##_key, (init) ); } return _##name##_key; } \
static FOUNDATION_FORCEINLINE type get_thread_##name( void ) { void* val = pthread_getspecific( get_##name##_key() ); return (type)((uintptr_t)val); } \
static FOUNDATION_FORCEINLINE void set_thread_##name( type val ) { pthread_setspecific( get_##name##_key(), (const void*)(uintptr_t)val ); }

#define FOUNDATION_DECLARE_THREAD_LOCAL_ARRAY( type, name, arrsize ) \
static _pthread_key_t _##name##_key = 0; \
static FOUNDATION_FORCEINLINE _pthread_key_t get_##name##_key( void ) { if( !_##name##_key ) pthread_key_create( &_##name##_key, 0 ); return _##name##_key; } \
static FOUNDATION_FORCEINLINE type* get_thread_##name( void ) { _pthread_key_t key = get_##name##_key(); type* arr = (type*)pthread_getspecific( key ); if( !arr ) { arr = _allocate_thread_local_block( sizeof( type ) * arrsize ); pthread_setspecific( key, arr ); } return arr; }

#elif FOUNDATION_PLATFORM_WINDOWS && FOUNDATION_COMPILER_CLANG

__declspec(dllimport) unsigned long STDCALL TlsAlloc();
__declspec(dllimport) void* STDCALL TlsGetValue(unsigned long);
__declspec(dllimport) int STDCALL TlsSetValue(unsigned long, void*);

FOUNDATION_API void* _allocate_thread_local_block(size_t size);

#define FOUNDATION_DECLARE_THREAD_LOCAL( type, name, init ) \
static unsigned long _##name##_key = 0; \
static FOUNDATION_FORCEINLINE unsigned long get_##name##_key( void ) { if( !_##name##_key ) { _##name##_key = TlsAlloc(); TlsSetValue( _##name##_key, init ); } return _##name##_key; } \
static FOUNDATION_FORCEINLINE type get_thread_##name( void ) { return (type)((uintptr_t)TlsGetValue( get_##name##_key() )); } \
static FOUNDATION_FORCEINLINE void set_thread_##name( type val ) { TlsSetValue( get_##name##_key(), (void*)((uintptr_t)val) ); }

#define FOUNDATION_DECLARE_THREAD_LOCAL_ARRAY( type, name, arrsize ) \
static unsigned long _##name##_key = 0; \
static FOUNDATION_FORCEINLINE unsigned long get_##name##_key( void ) { if( !_##name##_key ) _##name##_key = TlsAlloc(); return _##name##_key; } \
static FOUNDATION_FORCEINLINE type* get_thread_##name( void ) { unsigned long key = get_##name##_key(); type* arr = (type*)TlsGetValue( key ); if( !arr ) { arr = _allocate_thread_local_block( sizeof( type ) * arrsize ); TlsSetValue( key, arr ); } return arr; }

#else

#define FOUNDATION_DECLARE_THREAD_LOCAL( type, name, init ) \
static FOUNDATION_THREADLOCAL type _thread_##name = init; \
static FOUNDATION_FORCEINLINE void set_thread_##name( type val ) { _thread_##name = val; } \
static FOUNDATION_FORCEINLINE type get_thread_##name( void ) { return _thread_##name; }

#define FOUNDATION_DECLARE_THREAD_LOCAL_ARRAY( type, name, arrsize ) \
static FOUNDATION_THREADLOCAL type _thread_##name [arrsize] = {0}; \
static FOUNDATION_FORCEINLINE type* get_thread_##name( void ) { return _thread_##name; }

#endif

//Utility functions for large integer types
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint128_t
uint128_make(const uint64_t low, const uint64_t high);

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

//Implementations
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint128_t
uint128_make(const uint64_t low, const uint64_t high) {
  uint128_t u = { { low, high } };
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
  uint256_t u = { { w0, w1, w2, w3 } };
  return u;
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL uint256_t
uint256_null(void) {
  return uint256_make(0, 0, 0, 0);
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uint256_equal(const uint256_t u0, const uint256_t u1) {
  return u0.word[0] == u1.word[0] && u0.word[1] == u1.word[1] &&
         u0.word[2] == u1.word[2] && u0.word[3] == u1.word[3];
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL bool
uint256_is_null(const uint256_t u0) {
  return !u0.word[0] && !u0.word[1] && !u0.word[2] && !u0.word[3];
}

//Format specifiers for 64bit and pointers
#if FOUNDATION_COMPILER_MSVC
#  define PRId64       "I64d"
#  define PRIi64       "I64i"
#  define PRIdPTR      "Id"
#  define PRIiPTR      "Ii"
#  define PRIo64       "I64o"
#  define PRIu64       "I64u"
#  define PRIx64       "I64x"
#  define PRIX64       "I64X"
#  define PRIoPTR      "Io"
#  define PRIuPTR      "Iu"
#  define PRIxPTR      "Ix"
#  define PRIXPTR      "IX"
#  define PRIsize      "Iu"
#else
#  include <inttypes.h>
#  define PRIsize      "zu"
#endif

#if FOUNDATION_SIZE_REAL == 64
#  define PRIREAL      "llf"
#else
#  define PRIREAL      "f"
#endif

#if FOUNDATION_PLATFORM_WINDOWS
#  if FOUNDATION_SIZE_POINTER == 8
#    define PRIfixPTR  "016I64X"
#  else
#    define PRIfixPTR  "08IX"
#  endif
#else
#  if FOUNDATION_SIZE_POINTER == 8
#    define PRIfixPTR  "016" PRIXPTR
#  else
#    define PRIfixPTR  "08" PRIXPTR
#  endif
#endif

#include <foundation/build.h>

/*!
\def FOUNDATION_COMPILE
\brief Internal compilation flag
\details Defined to 1 when compiling the foundation library, undefined (or zero) when using the library

\def FOUNDATION_EXTERN
\brief Declare a variable visible to users of the library
\details Declare a variable visible to users of the library

\def FOUNDATION_API
\brief Declare a function visible to users of the library
\details Declare a function visible to users of the library

\def FOUNDATION_PLATFORM_ANDROID
\brief Android platform flag
\details Defined to 1 if compiling for android platforms, 0 otherwise

\def FOUNDATION_PLATFORM_IOS
\brief iOD platform flag
\details Defined to 1 if compiling for iOS platforms (iPhone/iPad and simulators), 0 otherwise

\def FOUNDATION_PLATFORM_IOS_SIMULATOR
\brief iOS simulator platform flag
\details Defined to 1 if compiling for iOS simulator (also has FOUNDATION_PLATFORM_IOS defined to 1), 0 otherwise

\def FOUNDATION_PLATFORM_MACOSX
\brief MacOS X platform flag
\details Defined to 1 if compiling for MacOS X, 0 otherwise

\def FOUNDATION_PLATFORM_LINUX
\brief Linux platform flag
\details Defined to 1 if compiling for Linux, 0 otherwise

\def FOUNDATION_PLATFORM_LINUX_RASPBERRYPI
\brief Raspberry Pi platform flag
\details Defined to 1 if compiling for Raspberry Pi (also has FOUNDATION_PLATFORM_LINUX defined to 1), 0 otherwise

\def FOUNDATION_PLATFORM_BSD
\brief BSD platform flag
\details Defined to 1 if compiling for BSD, 0 otherwise

\def FOUNDATION_PLATFORM_WINDOWS
\brief Windows platform flag
\details Defined to 1 if compiling for Windows, 0 otherwise

\def FOUNDATION_PLATFORM_PNACL
\brief PNaCl platform flag
\details Defined to 1 if compiling for PNaCl, 0 otherwise

\def FOUNDATION_PLATFORM_APPLE
\brief Apple platform group flag
\details Defined to 1 if compiling for Apple platforms (MacOS X, iOS, iOS simulator), 0 otherwise

\def FOUNDATION_PLATFORM_POSIX
\brief Posix platform group flag
\details Defined to 1 if compiling for POSIX platforms (Linux, BSD, MacOS X, iOS, iOS simulator, Android), 0 otherwise

\def FOUNDATION_PLATFORM_FAMILY_MOBILE
\brief Mobile platform family flag
\details Defined to 1 if compiling for mobile platforms (iOS, iOS simulator, Android), 0 otherwise

\def FOUNDATION_PLATFORM_FAMILY_DESKTOP
\brief Desktop platform family flag
\details Defined to 1 if compiling for desktop platforms (Windows, MacOS X, Linux, Raspberry Pi, BSD), 0 otherwise

\def FOUNDATION_PLATFORM_FAMILY_CONSOLE
\brief Console platform family flag
\details Defined to 1 if compiling for console platforms (iOS, iOS simulator, Android), 0 otherwise

\def FOUNDATION_ARCH_ARM
\brief ARM architecture flag
\details Defined to 1 if compiling for ARM architectures, 0 otherwise

\def FOUNDATION_ARCH_ARM5
\brief ARMv5 architecture flag
\details Defined to 1 if compiling for ARMv5 architectures, 0 otherwise

\def FOUNDATION_ARCH_ARM6
\brief ARMv6 architecture flag
\details Defined to 1 if compiling for ARMv6 architectures, 0 otherwise

\def FOUNDATION_ARCH_ARM7
\brief ARMv7 architecture flag
\details Defined to 1 if compiling for ARMv7 architectures, 0 otherwise

\def FOUNDATION_ARCH_ARM8
\brief ARMv8 architecture flag
\details Defined to 1 if compiling for ARMv8 architectures, 0 otherwise

\def FOUNDATION_ARCH_ARM_64
\brief ARM 64-bit architecture flag
\details Defined to 1 if compiling for 64-bit ARM architectures, 0 otherwise

\def FOUNDATION_ARCH_ARM8_64
\brief ARMv8 64-bit architecture flag
\details Defined to 1 if compiling for 64-bit ARMv8 architectures, 0 otherwise

\def FOUNDATION_ARCH_X86
\brief x86 architecture flag
\details Defined to 1 if compiling for x86 architectures, 0 otherwise

\def FOUNDATION_ARCH_X86_64
\brief x86-64 architecture flag
\details Defined to 1 if compiling for x86-64 architectures, 0 otherwise

\def FOUNDATION_ARCH_PPC
\brief PPC architecture flag
\details Defined to 1 if compiling for PPC architectures, 0 otherwise

\def FOUNDATION_ARCH_PPC_64
\brief PPC 64-bit architecture flag
\details Defined to 1 if compiling for 64-bit PPC architectures, 0 otherwise

\def FOUNDATION_ARCH_IA64
\brief IA-64 architecture flag
\details Defined to 1 if compiling for IA64 architectures, 0 otherwise

\def FOUNDATION_ARCH_MIPS
\brief MIPS architecture flag
\details Defined to 1 if compiling for MIPS architectures, 0 otherwise

\def FOUNDATION_ARCH_MIPS_64
\brief MIPS 64-bit architecture flag
\details Defined to 1 if compiling for 64-bit MIPS architectures, 0 otherwise

\def FOUNDATION_ARCH_SSE2
\brief SSE2 architecture capability flag
\details Defined to 1 if compiling with SSE2 instruction set enabled, 0 otherwise

\def FOUNDATION_ARCH_SSE3
\brief SSE3 architecture capability flag
\details Defined to 1 if compiling with SSE3 instruction set enabled, 0 otherwise

\def FOUNDATION_ARCH_SSE4
\brief SSE4 architecture capability flag
\details Defined to 1 if compiling with SSE4 instruction set enabled, 0 otherwise

\def FOUNDATION_ARCH_SSE4_FMA3
\brief SSE4 FMA3 architecture capability flag
\details Defined to 1 if compiling with SSE4 instruction set (including FMA3 instruction) enabled, 0 otherwise

\def FOUNDATION_ARCH_NEON
\brief NEON architecture capability flag
\details Defined to 1 if compiling with NEON instruction set enabled, 0 otherwise

\def FOUNDATION_ARCH_THUMB
\brief THUMB architecture capability flag
\details Defined to 1 if compiling for ARM THUMB instruction set, 0 otherwise

\def FOUNDATION_ARCH_ENDIAN_LITTLE
\brief Little endian architecture flag
\details Defined to 1 if compiling for little endian architectures, 0 otherwise

\def FOUNDATION_ARCH_ENDIAN_BIG
\brief Big endian architecture flag
\details Defined to 1 if compiling for big endian architectures, 0 otherwise

\def FOUNDATION_COMPILER_CLANG
\brief CLang compiler flag
\details Defined to 1 if compiling with clang, 0 otherwise

\def FOUNDATION_COMPILER_GCC
\brief GCC compiler flag
\details Defined to 1 if compiling with GCC, 0 otherwise

\def FOUNDATION_COMPILER_MSVC
\brief Microsoft compiler flag
\details Defined to 1 if compiling with Microsoft compiler, 0 otherwise

\def FOUNDATION_COMPILER_INTEL
\brief Intel compiler flag
\details Defined to 1 if compiling with Intel compiler, 0 otherwise

\def FOUNDATION_COMPILER_NAME
\brief Compiler name
\details A string naming the compiler used

\def FOUNDATION_COMPILER_DESCRIPTION
\brief Compiler description
\details A string with a more detailed description of the compiler used, name and version

\def FOUNDATION_SIZE_REAL
\brief Size of real type
\details Declare the size of a real number, in bytes. Either 4 or 8 bytes for 32 or 64 bit, respectively. Default to 32 bit

\def FOUNDATION_SIZE_POINTER
\brief Size of pointer type
\details Defines the size of a pointer on the current architecture, in bytes. Either 4 or 8 bytes for 32 and 64 bit architectures, respectively

\def FOUNDATION_SIZE_WCHAR
\brief Size of wchar_t type
\details Defines the size of the wchar_t type, in bytes, depending on platform

\def FOUNDATION_MAX_PATHLEN
\brief Maximum path length
\details Defines the maximum length of a path

\def FOUNDATION_DECLARE_THREAD_LOCAL
\brief Declare a thread local variable
\details Declare a thread-local variable of the given type, name and initial value. Only works for types that can be safely cast through a uintptr_t (integers, pointers...).
This will also declare and implement two inlined functions to set and get value of the variable, called get_thread_[name] and set_thread_[name]. For example, to declare
a thread-local integer and use the get/set functions:
<pre>FOUNDATION_DECLARE_THREAD_LOCAL( int, myvar, 0 );
set_thread_myvar( 1 );
int currentval = get_thread_myvar();</pre>
This macro is guaranteed to work across all supported platforms even when TLS is not available. For maximum portability use these macros instead of THREADLOCAL.
\param type    Data type
\param name    Variable name
\param init    Initial value

\def FOUNDATION_DECLARE_THREAD_LOCAL_ARRAY
\brief Declare a thread local array
\details Declare a thread-local array of the given type, name and array size. This will also declare and implement
one inlined function to get the array pointer value of the variable, called get_thread_[name]. For example, to declare
a thread-local integer array and use the get/set functions:
<pre>FOUNDATION_DECLARE_THREAD_LOCAL_ARRAY( int, myvar, 10 );
int* currentarr = get_thread_myvar(); //Get thread-local array storage
currentarr[2] = 10;
int val = currentarr[2];</pre>
\param type    Data type
\param name    Variable name
\param arrsize Size of array in number of elements

\def FOUNDATION_RESTRICT
\brief Restrict attribute
\details Restrict attribute, defined to nothing if compiler does not support restrict

\def FOUNDATION_THREADLOCAL
\brief Thread local attribyte
\details Thread local attribute, defined to nothing if the compiler/platform/architecture does not support thread local variables. For full platform support, use FOUNDATION_DECLARE_THREAD_LOCAL instead

\def FOUNDATION_DEPRECATED
\brief Deprecated attribute
\details Deprecated attribute, marking a function/variable as deprecated

\def FOUNDATION_FORCEINLINE
\brief Force inline attribute
\details Attribute to force function to be inlined

\def FOUNDATION_NOINLINE
\brief Prevent inline attribute
\details Attribute to prevent function from being inlined

\def FOUNDATION_PURECALL
\brief Pure call attribute
\details Attribute declaring function to be pure, meaning it has no effects except the return value and the return value depends only on the parameters and/or global variables

\def FOUNDATION_CONSTCALL
\brief Const call attribute
\details Attribute declaring function to be const, meaning it does not examine any values except the arguments, and has no effects except the return value.
Basically this is just slightly more strict class than the PURECALL attribute, since function is not allowed to read global memory.
Note that a function that has pointer arguments and examines the data pointed to must not be declared const. Likewise, a function that calls a non-const
function usually must not be const. It does not make sense for a const function to return void.

\def FOUNDATION_ALIGN
\brief Align attribute
\details Variable or type attribute declaring the variable/type to have the specified memory alignment
\param alignment Alignment

\def FOUNDATION_ALIGNOF
\brief Get alignment
\details Get the alignment of the given type or variable
\param type Type or variable

\def FOUNDATION_ALIGNED_STRUCT
\brief Declare aligned struct
\details Declare an aligned struct type
\param name Struct name
\param alignment Alignment

\typedef float32_t
\brief 32-bit floating point type
\details Floating point type guaranteed to be 32-bit in size

\typedef float64_t
\brief 64-bit floating point type
\details Floating point type guaranteed to be 64-bit in size

\typedef real
\brief Standard floating point type
\details Floating point type of the size chosen in build config (32 or 64 bit). See FOUNDATION_SIZE_REAL for declaring size used.

\def FLOAT32_C
\brief Declare 32-bit floating point constant
\details Declare a 32-bit floating point constant. Use for automatic suffixing, for example FLOAT32_C(1.0)
\param x      Constant value

\def FLOAT64_C
\brief Declare 64-bit floating point constant
\details Declare a 64-bit floating point constant. Use for automatic suffixing, for example FLOAT64_C(1.0)
\param x      Constant value

\def REAL_C
\brief Declare floating point constant
\details Declare a real constant. Use for automatic suffixing depending on floating point notation used, for example REAL_C(1.0)
\param x      Constant value

\struct uint128_t
\brief 128-bit unsigned integer type
\details 128-bit unsigned integer type

\struct uint256_t
\brief 256-bit unsigned integer type
\details 256-bit unsigned integer type

\struct atomic32_t
\brief 32-bit atomic integer
\details 32-bit atomic integer, use atomic_* functions to load/store values atomically (see atomic.h documentation)

\struct atomic64_t
\brief 64-bit atomic integer
\details 64-bit atomic integer, use atomic_* functions to load/store values atomically (see atomic.h documentation)

\struct atomicptr_t
\brief Atomic pointer
\details Atomic pointer, use atomic_* functions to load/store values atomically (see atomic.h documentation)

\fn uint128_t uint128_make( const uint64_t low, const uint64_t high )
\brief Declare 128-bit value
\details Declare a 128-bit unsigned int value from low and high 64-bit components
\param low     Low 64 bits
\param high    High 64 bits
\return        128-bit integer value

\fn bool uint128_equal( const uint128_t u0, const uint128_t u1 )
\brief Compare 128-bit values
\details Query if two 128-bit unsigned int values are equal
\param u0      First value
\param u1      Second value
\return        true if values are equal, false if not

\fn uint128_t uint128_null( void )
\brief Zero 128-bit value
\details Declare a zero (null) 128-bit unsigned int value
\return        Zero 128-bit value

\fn bool uint128_is_null( const uint128_t u0 )
\brief Compare 128-bit value to zero
\details Query if a 128-bit unsigned int value is zero (null)
\param u0      value
\return        true if value is zero (null), false if not

\fn uint256_t uint256_make( const uint64_t w0, const uint64_t w1, const uint64_t w2, const uint64_t w3 )
\brief Declare 256-bit value
\details Declare a 256-bit unsigned int value from four 64-bit components
\param w0      First (lowest) 64 bits
\param w1      Second 64 bits
\param w2      Third 64 bits
\param w3      Fourth (highest) 64 bits
\return        256-bit integer value

\fn bool uint256_equal( const uint256_t u0, const uint256_t u1 )
\brief Compare 256-bit values
\details Query if two 256-bit unsigned int values are equal
\param u0      First value
\param u1      Second value
\return        true if values are equal, false if not

\fn uint256_t uint256_null( void )
\brief Zero 256-bit value
\details Declare a zero (null) 256-bit unsigned int value
\return        Zero 256-bit value

\fn bool uint256_is_null( const uint256_t u0 )
\brief Compare 256-bit value to zero
\details Query if a 256-bit unsigned int value is zero (null)
\param u0      Value
\return        true if value is zero (null), false if not

\def pointer_offset
\brief Offset pointer
\details Offset a non-const pointer the given number of bytes, disregarding type of pointer
\param ptr     Pointer
\param ofs     Offset in bytes (positive or negative)

\def pointer_offset_const
\brief Offset const pointer
\details Offset a const pointer the given number of bytes, disregarding type of pointer
\param ptr     Pointer
\param ofs     Offset in bytes (positive or negative)

\def pointer_diff
\brief Calculate pointer difference
\details Calculate the offset in bytes between two pointers (from first to second), disregarding type of pointer
\param first   First pointer
\param second  Second pointer

\def PRIREAL
\brief Floating point printf format specifier
\details Printf-style format declaration for a real variable.
Use like other standard PRI* format specifiers, like <code>string_format( "Value: %" PRIREAL, realval );</code>

\def PRIfixPTR
\brief Pointer printf format specifier
\details Printf-style format declaration for a pointer variable producing a fixed-size string (padding with zeroes).
Use like other standard PRI* format specifiers, like <code>string_format( "Value: %" PRIfixPTR, ptr );</code>

*/
