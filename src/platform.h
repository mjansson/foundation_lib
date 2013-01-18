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
    Platform abstractions
	
	This file tries to unify platform definitions and data types across supported platforms and compilers.
	If you need to override platform or architecture, predefine one of FOUNDATION_PLATFORM_[...] and FOUNDATION_PLATFORM_ARCH_[...] to 1

	All preprocessor macros are designed to be always defined and used by value, i.e a check should be performed like
	"#if FOUNDATION_PLATFORM_[...]" rather than "#ifdef FOUNDATION_PLATFORM_[...]", since this solves the potential problem of typos in
	preprocessor checks (the #if test will most likely catch the typo with an not defined error, whereas the
	#ifdef macro will just resolve to false)

	This header also defines a bunch of preprocessor macros:
	FOUNDATION_PLATFORM_[...]
	FOUNDATION_PLATFORM_ARCH_[...]
	FOUNDATION_COMPILER_[...]
	FOUNDATION_EXTERN
	FOUNDATION_API
	FOUNDATION_PREPROCESSOR_TOSTRING
	FOUNDATION_PREPROCESSOR_JOIN
	FOUNDATION_DECLARE_THREAD_LOCAL
	FOUNDATION_DECLARE_THREAD_LOCAL_ARRAY

	And some short-form type/function attributes as well:
	RESTRICT
	THREADLOCAL
	DEPRECATED
	FORCEINLINE
	NOINLINE
	PURE

	*/

#if defined( FOUNDATION_COMPILE ) && FOUNDATION_COMPILE
#  if __cplusplus
#  define FOUNDATION_EXTERN extern "C"
#  else
#  define FOUNDATION_EXTERN extern
#  endif
#  define FOUNDATION_API
#else
#  if __cplusplus
#  define FOUNDATION_EXTERN extern "C"
#  define FOUNDATION_API extern "C"
#  else
#  define FOUNDATION_EXTERN extern
#  define FOUNDATION_API extern
#  endif
#endif

//Platforms
#ifndef FOUNDATION_PLATFORM_ANDROID
#  define FOUNDATION_PLATFORM_ANDROID 0
#endif
#ifndef FOUNDATION_PLATFORM_IOS
#  define FOUNDATION_PLATFORM_IOS 0
#endif
#ifndef FOUNDATION_PLATFORM_IOS_SIMULATOR
#  define FOUNDATION_PLATFORM_IOS_SIMULATOR 0
#endif
#ifndef FOUNDATION_PLATFORM_MACOSX
#  define FOUNDATION_PLATFORM_MACOSX 0
#endif
#ifndef FOUNDATION_PLATFORM_LINUX
# define FOUNDATION_PLATFORM_LINUX 0
#endif
#ifndef FOUNDATION_PLATFORM_BSD
#  define FOUNDATION_PLATFORM_BSD 0
#endif
#ifndef FOUNDATION_PLATFORM_WINDOWS
#  define FOUNDATION_PLATFORM_WINDOWS 0
#endif

//Architectures
#ifndef  FOUNDATION_PLATFORM_ARCH_ARM
#  define FOUNDATION_PLATFORM_ARCH_ARM 0
#  define FOUNDATION_PLATFORM_ARCH_ARM6 0
#  define FOUNDATION_PLATFORM_ARCH_ARM7 0
#  define FOUNDATION_PLATFORM_ARCH_ARM8 0
#endif
#ifndef  FOUNDATION_PLATFORM_ARCH_X86
#  define FOUNDATION_PLATFORM_ARCH_X86 0
#endif
#ifndef  FOUNDATION_PLATFORM_ARCH_X86_64
#  define FOUNDATION_PLATFORM_ARCH_X86_64 0
#endif
#ifndef  FOUNDATION_PLATFORM_ARCH_PPC
#  define FOUNDATION_PLATFORM_ARCH_PPC 0
#endif
#ifndef  FOUNDATION_PLATFORM_ARCH_PPC_64
#  define FOUNDATION_PLATFORM_ARCH_PPC_64 0
#endif
#ifndef FOUNDATION_PLATFORM_ARCH_IA64
#  define FOUNDATION_PLATFORM_ARCH_IA64 0
#endif

//Platform traits
#define FOUNDATION_PLATFORM_APPLE 0
#define FOUNDATION_PLATFORM_POSIX 0

#define FOUNDATION_PLATFORM_ENDIAN_LITTLE 0
#define FOUNDATION_PLATFORM_ENDIAN_BIG 0

#define FOUNDATION_PLATFORM_FAMILY_MOBILE 0
#define FOUNDATION_PLATFORM_FAMILY_DESKTOP 0
#define FOUNDATION_PLATFORM_FAMILY_CONSOLE 0

//Compilers
#define FOUNDATION_COMPILER_CLANG 0
#define FOUNDATION_COMPILER_GCC 0
#define FOUNDATION_COMPILER_MSVC 0
#define FOUNDATION_COMPILER_INTEL 0


//First, platforms and architectures

// Android
#if defined( __ANDROID__ ) || FOUNDATION_PLATFORM_ANDROID

#  undef  FOUNDATION_PLATFORM_ANDROID
#  define FOUNDATION_PLATFORM_ANDROID 1

// Compatibile platforms
#  undef  FOUNDATION_PLATFORM_LINUX
#  define FOUNDATION_PLATFORM_LINUX 1
#  undef  FOUNDATION_PLATFORM_POSIX
#  define FOUNDATION_PLATFORM_POSIX 1

#  define FOUNDATION_PLATFORM_NAME "Android"

// Architecture and detailed description
#  if defined( __arm__ ) || FOUNDATION_PLATFORM_ARCH_ARM
#    undef  FOUNDATION_PLATFORM_ARCH_ARM
#    define FOUNDATION_PLATFORM_ARCH_ARM 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Android ARM"
#  elif defined( __i386__ ) || FOUNDATION_PLATFORM_ARCH_X86
#    undef  FOUNDATION_PLATFORM_ARCH_X86
#    define FOUNDATION_PLATFORM_ARCH_X86 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Android x86"
#  elif defined( __x86_64__ ) || FOUNDATION_PLATFORM_ARCH_X86_64
#    undef  FOUNDATION_PLATFORM_ARCH_X86_64
#    define FOUNDATION_PLATFORM_ARCH_X86_64 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Android x86-64"
#  else
#    error Unknown architecture
#  endif

// Traits
#  undef  FOUNDATION_PLATFORM_ENDIAN_LITTLE
#  define FOUNDATION_PLATFORM_ENDIAN_LITTLE 1

#  undef  FOUNDATION_PLATFORM_FAMILY_MOBILE
#  define FOUNDATION_PLATFORM_FAMILY_MOBILE 1

#  undef  FOUNDATION_PLATFORM_FAMILY_CONSOLE
#  define FOUNDATION_PLATFORM_FAMILY_CONSOLE 1


// MacOS X and iOS
#elif ( defined( __APPLE__ ) && __APPLE__ ) || FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_MACOSX

#define FOUNDATION_PLATFORM_APPLE 1
#define FOUNDATION_PLATFORM_POSIX 1

#  if defined( __IPHONE__ ) || ( defined( TARGET_OS_IPHONE ) && TARGET_OS_IPHONE ) || ( defined( TARGET_IPHONE_SIMULATOR ) && TARGET_IPHONE_SIMULATOR ) || FOUNDATION_PLATFORM_IOS

#    undef  FOUNDATION_PLATFORM_IOS
#    define FOUNDATION_PLATFORM_IOS 1

#    define FOUNDATION_PLATFORM_NAME "iOS"

#    if defined( __arm__ ) || FOUNDATION_PLATFORM_ARCH_ARM
#      undef  FOUNDATION_PLATFORM_ARCH_ARM
#      define FOUNDATION_PLATFORM_ARCH_ARM 1
#      if defined( __ARM_ARCH_8__ )
#        undef  FOUNDATION_PLATFORM_ARCH_ARM8
#        define FOUNDATION_PLATFORM_ARCH_ARM8 1
#        define FOUNDATION_PLATFORM_DESCRIPTION "iOS ARMv8"
#        error ARMv8 not yet supported
#      elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7S__)
#        undef  FOUNDATION_PLATFORM_ARCH_ARM7
#        define FOUNDATION_PLATFORM_ARCH_ARM7 1
#        define FOUNDATION_PLATFORM_DESCRIPTION "iOS ARMv7"
#        ifndef __ARM_NEON__
#          error Missing ARM NEON support
#        endif
#      elif defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6__)
#        undef  FOUNDATION_PLATFORM_ARCH_ARM6
#        define FOUNDATION_PLATFORM_ARCH_ARM6 1
#        define FOUNDATION_PLATFORM_DESCRIPTION "iOS ARMv6"
#      else
#        error Unrecognized ARM architecture
#      endif
#    elif defined( __i386__ ) || FOUNDATION_PLATFORM_ARCH_X86
#      undef  FOUNDATION_PLATFORM_IOS_SIMULATOR
#      define FOUNDATION_PLATFORM_IOS_SIMULATOR 1
#      undef  FOUNDATION_PLATFORM_ARCH_X86
#      define FOUNDATION_PLATFORM_ARCH_X86 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "iOS x86 (simulator)"
#    else
#      error Unknown architecture
#    endif

#    undef  FOUNDATION_PLATFORM_ENDIAN_LITTLE
#    define FOUNDATION_PLATFORM_ENDIAN_LITTLE 1

#    undef  FOUNDATION_PLATFORM_FAMILY_MOBILE
#    define FOUNDATION_PLATFORM_FAMILY_MOBILE 1

#    undef  FOUNDATION_PLATFORM_FAMILY_CONSOLE
#    define FOUNDATION_PLATFORM_FAMILY_CONSOLE 1

#  elif defined( __MACH__ ) || FOUNDATION_PLATFORM_MACOSX

#    undef  FOUNDATION_PLATFORM_MACOSX
#    define FOUNDATION_PLATFORM_MACOSX 1

#    undef  FOUNDATION_PLATFORM_POSIX
#    define FOUNDATION_PLATFORM_POSIX 1

#    define FOUNDATION_PLATFORM_NAME "MacOSX"

#    if defined( __x86_64__ ) ||  defined( __x86_64 ) || defined( __amd64 ) || FOUNDATION_PLATFORM_ARCH_X86_64
#      undef  FOUNDATION_PLATFORM_ARCH_X86_64
#      define FOUNDATION_PLATFORM_ARCH_X86_64 1
#      undef  FOUNDATION_PLATFORM_ENDIAN_LITTLE
#      define FOUNDATION_PLATFORM_ENDIAN_LITTLE 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "MacOSX x86-64"
#    elif defined( __i386__ ) || defined( __intel__ ) || FOUNDATION_PLATFORM_ARCH_X86
#      undef  FOUNDATION_PLATFORM_ARCH_X86
#      define FOUNDATION_PLATFORM_ARCH_X86 1
#      undef  FOUNDATION_PLATFORM_ENDIAN_LITTLE
#      define FOUNDATION_PLATFORM_ENDIAN_LITTLE 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "MacOSX x86"

#    elif defined( __powerpc64__ ) || defined( __POWERPC64__ ) || FOUNDATION_PLATFORM_ARCH_PPC_64
#      undef  FOUNDATION_PLATFORM_ARCH_PPC_64
#      define FOUNDATION_PLATFORM_ARCH_PPC_64 1
#      undef  FOUNDATION_PLATFORM_ENDIAN_BIG
#      define FOUNDATION_PLATFORM_ENDIAN_BIG 1
#      define FOUNDATION_PLATFORM_DESCRIPTION "MacOSX PPC64"
#    elif defined( __powerpc__ ) || defined( __POWERPC__ ) || FOUNDATION_PLATFORM_ARCH_PPC
#      undef  FOUNDATION_PLATFORM_ARCH_PPC
#      define FOUNDATION_PLATFORM_ARCH_PPC 1
#      undef  FOUNDATION_PLATFORM_ENDIAN_BIG
#      define FOUNDATION_PLATFORM_ENDIAN_BIG 1
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
#elif ( defined( __linux__ ) || defined( __linux ) ) || FOUNDATION_PLATFORM_LINUX

#  undef  FOUNDATION_PLATFORM_LINUX
#  define FOUNDATION_PLATFORM_LINUX 1

#  undef  FOUNDATION_PLATFORM_POSIX
#  define FOUNDATION_PLATFORM_POSIX 1

#  define FOUNDATION_PLATFORM_NAME "Linux"

#  if defined( __x86_64__ ) || defined( __x86_64 ) || defined( __amd64 ) || FOUNDATION_PLATFORM_ARCH_X86_64
#    undef  FOUNDATION_PLATFORM_ARCH_X86_64
#    define FOUNDATION_PLATFORM_ARCH_X86_64 1
#    undef  FOUNDATION_PLATFORM_ENDIAN_LITTLE
#    define FOUNDATION_PLATFORM_ENDIAN_LITTLE 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Linux x86-64"
#  elif defined( __i386__ ) || defined( __intel__ ) || defined( _M_IX86 ) || FOUNDATION_PLATFORM_ARCH_X86
#    undef  FOUNDATION_PLATFORM_ARCH_X86
#    define FOUNDATION_PLATFORM_ARCH_X86 1
#    undef  FOUNDATION_PLATFORM_ENDIAN_LITTLE
#    define FOUNDATION_PLATFORM_ENDIAN_LITTLE 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Linux x86"

#  elif defined( __powerpc64__ ) || defined( __POWERPC64__ ) || FOUNDATION_PLATFORM_ARCH_PPC_64
#    undef  FOUNDATION_PLATFORM_ARCH_PPC_64
#    define FOUNDATION_PLATFORM_ARCH_PPC_64 1
#    undef  FOUNDATION_PLATFORM_ENDIAN_BIG
#    define FOUNDATION_PLATFORM_ENDIAN_BIG 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Linux PPC64"
#  elif defined( __powerpc__ ) || defined( __POWERPC__ ) || FOUNDATION_PLATFORM_ARCH_PPC
#    undef  FOUNDATION_PLATFORM_ARCH_PPC
#    define FOUNDATION_PLATFORM_ARCH_PPC 1
#    undef  FOUNDATION_PLATFORM_ENDIAN_BIG
#    define FOUNDATION_PLATFORM_ENDIAN_BIG 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Linux PPC"

#  else
#    error Unknown architecture
#  endif

#  undef  FOUNDATION_PLATFORM_FAMILY_DESKTOP
#  define FOUNDATION_PLATFORM_FAMILY_DESKTOP 1

//BSD family
#elif ( defined( __BSD__ ) || defined( __FreeBSD__ ) ) || FOUNDATION_PLATFORM_BSD

#  undef  FOUNDATION_PLATFORM_BSD
#  define FOUNDATION_PLATFORM_BSD 1

#  undef  FOUNDATION_PLATFORM_POSIX
#  define FOUNDATION_PLATFORM_POSIX 1

#  define FOUNDATION_PLATFORM_NAME "BSD"

#  if defined( __x86_64__ ) || defined( __x86_64 ) || defined( __amd64 ) || FOUNDATION_PLATFORM_ARCH_X86_64
#    undef  FOUNDATION_PLATFORM_ARCH_X86_64
#    define FOUNDATION_PLATFORM_ARCH_X86_64 1
#    undef  FOUNDATION_PLATFORM_ENDIAN_LITTLE
#    define FOUNDATION_PLATFORM_ENDIAN_LITTLE 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "BSD x86-64"
#  elif defined( __i386__ ) || defined( __intel__ ) || defined( _M_IX86 ) || FOUNDATION_PLATFORM_ARCH_X86
#    undef  FOUNDATION_PLATFORM_ARCH_X86
#    define FOUNDATION_PLATFORM_ARCH_X86 1
#    undef  FOUNDATION_PLATFORM_ENDIAN_LITTLE
#    define FOUNDATION_PLATFORM_ENDIAN_LITTLE 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "BSD x86"

#  elif defined( __powerpc64__ ) || defined( __POWERPC64__ ) || FOUNDATION_PLATFORM_ARCH_PPC_64
#    undef  FOUNDATION_PLATFORM_ARCH_PPC_64
#    define FOUNDATION_PLATFORM_ARCH_PPC_64 1
#    undef  FOUNDATION_PLATFORM_ENDIAN_BIG
#    define FOUNDATION_PLATFORM_ENDIAN_BIG 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "BSD PPC64"
#  elif defined( __powerpc__ ) || defined( __POWERPC__ ) || FOUNDATION_PLATFORM_ARCH_PPC
#    undef  FOUNDATION_PLATFORM_ARCH_PPC
#    define FOUNDATION_PLATFORM_ARCH_PPC 1
#    undef  FOUNDATION_PLATFORM_ENDIAN_BIG
#    define FOUNDATION_PLATFORM_ENDIAN_BIG 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "BSD PPC"

#  else
#    error Unknown architecture
#  endif

#  undef  FOUNDATION_PLATFORM_FAMILY_DESKTOP
#  define FOUNDATION_PLATFORM_FAMILY_DESKTOP 1

// Windows
#elif defined( _WIN32 ) || defined( __WIN32__ ) || defined( _WIN64 ) || FOUNDATION_PLATFORM_WINDOWS

#  undef  FOUNDATION_PLATFORM_WINDOWS
#  define FOUNDATION_PLATFORM_WINDOWS 1

#  define FOUNDATION_PLATFORM_NAME "Windows"

#  if defined( __x86_64__ ) || defined( _M_AMD64 ) || defined( _AMD64_ ) || FOUNDATION_PLATFORM_ARCH_X86_64
#    undef  FOUNDATION_PLATFORM_ARCH_X86_64
#    define FOUNDATION_PLATFORM_ARCH_X86_64 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Windows x86-64"
#  elif defined( __x86__ ) || defined( _M_IX86 ) || defined( _X86_ ) || FOUNDATION_PLATFORM_ARCH_X86
#    undef  FOUNDATION_PLATFORM_ARCH_X86
#    define FOUNDATION_PLATFORM_ARCH_X86 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Windows x86"
#  elif defined( __ia64__ ) || defined( _M_IA64 ) || defined( _IA64_ ) || FOUNDATION_PLATFORM_ARCH_IA64
#    undef  FOUNDATION_PLATFORM_ARCH_IA64
#    define FOUNDATION_PLATFORM_ARCH_IA64 1
#    define FOUNDATION_PLATFORM_DESCRIPTION "Windows IA-64"

#  else
#    error Unknown architecture
#  endif

#  undef  FOUNDATION_PLATFORM_ENDIAN_LITTLE
#  define FOUNDATION_PLATFORM_ENDIAN_LITTLE 1

#  undef  FOUNDATION_PLATFORM_FAMILY_DESKTOP
#  define FOUNDATION_PLATFORM_FAMILY_DESKTOP 1

#  if defined( FOUNDATION_COMPILE ) && FOUNDATION_COMPILE && !defined( _CRT_SECURE_NO_WARNINGS )
#    define _CRT_SECURE_NO_WARNINGS 1
#  endif

#else
#  error Unknown platform
#endif


//Utility macros
#define FOUNDATION_PREPROCESSOR_TOSTRING( x )          _FOUNDATION_PREPROCESSOR_TOSTRING(x)
#define _FOUNDATION_PREPROCESSOR_TOSTRING( x )         #x

#define FOUNDATION_PREPROCESSOR_JOIN( a, b )           _FOUNDATION_PREPROCESSOR_JOIN( a, b )
#define _FOUNDATION_PREPROCESSOR_JOIN( a, b )          _FOUNDATION_PREPROCESSOR_JOIN_INTERNAL( a, b )
#define _FOUNDATION_PREPROCESSOR_JOIN_INTERNAL( a, b ) a##b


//Compilers

// CLang
#if defined( __clang__ )

#  undef  FOUNDATION_COMPILER_CLANG
#  define FOUNDATION_COMPILER_CLANG 1

#  define FOUNDATION_COMPILER_NAME "clang"
#  define FOUNDATION_COMPILER_DESCRIPTION FOUNDATION_COMPILER_NAME " " __clang_version__

#  define RESTRICT restrict
#  define THREADLOCAL __thread

#  define ATTRIBUTE(x) __attribute__((__##x##__))
#  define ATTRIBUTE2(x,y) __attribute__((__##x##__(y)))
#  define ATTRIBUTE3(x,y,z) __attribute__((__##x##__(y,z)))

#  define DEPRECATED ATTRIBUTE(deprecated)
#  define FORCEINLINE inline ATTRIBUTE(always_inline)
#  define NOINLINE ATTRIBUTE(noinline)
#  define PURECALL ATTRIBUTE(pure)
#  define CONSTCALL ATTRIBUTE(const)
#  define ALIGN(x) ATTRIBUTE2(aligned,x)

// GCC
#elif defined( __GNUC__ )

#  undef  FOUNDATION_COMPILER_GCC
#  define FOUNDATION_COMPILER_GCC 1

#  define FOUNDATION_COMPILER_NAME "gcc"
#  define FOUNDATION_COMPILER_DESCRIPTION FOUNDATION_COMPILER_NAME " v" FOUNDATION_PREPROCESSOR_TOSTRING( __GNUC__ ) "." FOUNDATION_PREPROCESSOR_TOSTRING( __GNUC_MINOR__ )

#  define RESTRICT restrict
#  define THREADLOCAL __thread

#  define ATTRIBUTE(x) __attribute__((__##x##__))
#  define ATTRIBUTE2(x,y) __attribute__((__##x##__(y)))
#  define ATTRIBUTE3(x,y,z) __attribute__((__##x##__(y,z)))

#  define DEPRECATED ATTRIBUTE(deprecated)
#  define FORCEINLINE inline ATTRIBUTE(always_inline)
#  define NOINLINE ATTRIBUTE(noinline)
#  define PURECALL ATTRIBUTE(pure)
#  define CONSTCALL ATTRIBUTE(const)
#  define ALIGN(x) ATTRIBUTE2(aligned,x)

// Intel
#elif defined( __ICL ) || defined( __ICC )

#  undef  FOUNDATION_COMPILER_INTEL
#  define FOUNDATION_COMPILER_INTEL 1

#  define FOUNDATION_COMPILER_NAME "intel"
#  if defined( __ICL )
#    define FOUNDATION_COMPILER_DESCRIPTION FOUNDATION_COMPILER_NAME " v" FOUNDATION_PREPROCESSOR_TOSTRING( __ICL )
#  elif defined( __ICC )
#    define FOUNDATION_COMPILER_DESCRIPTION FOUNDATION_COMPILER_NAME " v" FOUNDATION_PREPROCESSOR_TOSTRING( __ICC )
#  endif

#  define RESTRICT restrict
#  define THREADLOCAL __thread

#  define ATTRIBUTE(x) __attribute__((__##x##__))
#  define ATTRIBUTE2(x,y) __attribute__((__##x##__(y)))
#  define ATTRIBUTE3(x,y,z) __attribute__((__##x##__(y,z)))

#  define DEPRECATED ATTRIBUTE(deprecated)
#  define FORCEINLINE inline ATTRIBUTE(always_inline)
#  define NOINLINE ATTRIBUTE(noinline)
#  define PURECALL ATTRIBUTE(pure)
#  define CONSTCALL ATTRIBUTE(const)
#  define ALIGN(x) ATTRIBUTE2(aligned,x)

// Microsoft
#elif defined( _MSC_VER )

#  undef  FOUNDATION_COMPILER_MSVC
#  define FOUNDATION_COMPILER_MSVC 1

#  define FOUNDATION_COMPILER_NAME "msvc"
#  define FOUNDATION_COMPILER_DESCRIPTION FOUNDATION_COMPILER_NAME " v" FOUNDATION_PREPROCESSOR_TOSTRING( _MSC_VER )

#  define RESTRICT __restrict
#  define THREADLOCAL __declspec(thread)

#  define DEPRECATED __declspec(deprecated)
#  define FORCEINLINE __forceinline
#  define NOINLINE __declspec(noinline)
#  define PURECALL
#  define CONSTCALL
#  define ALIGN(x) __declspec(align(x))

#  ifndef __cplusplus
typedef enum
{
	false = 0,
	true  = 1
} bool;
#endif

#else
#  error Unknown compiler
#endif


//Pointer size
#if FOUNDATION_PLATFORM_ARCH_X86_64 || FOUNDATION_PLATFORM_ARCH_PPC_64 || FOUNDATION_PLATFORM_ARCH_IA64
#  define FOUNDATION_PLATFORM_POINTER_SIZE 8
#else
#  define FOUNDATION_PLATFORM_POINTER_SIZE 4
#endif


//Base data types
#include <stdint.h>   //Standard types like int32_t, uintptr_t
#include <float.h>

typedef float          float32_t;
typedef double         float64_t;

typedef struct {
	uint64_t word[2];
} uint128_t;

typedef struct {
	uint64_t word[4];
} uint256_t;

#define FLOAT32_C(x)   (x##f)
#define FLOAT64_C(x)   (x)

//Configurable standard floating point precision type
#if !defined( FLT_EVAL_METHOD ) && ( defined(__FLT_EVAL_METHOD__ ) || defined( _FEVAL ) )
#  ifdef __FLT_EVAL_METHOD__
#    define FLT_EVAL_METHOD __FLT_EVAL_METHOD__
#else
#    define FLT_EVAL_METHOD _FEVAL
#  endif
#endif

#if ( defined( FLT_EVAL_METHOD ) && ( FLT_EVAL_METHOD >= 1 ) ) || ( defined( FOUNDATION_PLATFORM_REALSIZE ) && ( FOUNDATION_PLATFORM_REALSIZE > 32 ) )
typedef   float64_t         real;
#  define REAL_C(x)         FLOAT64_C(x)
#  undef  FOUNDATION_PLATFORM_REALSIZE
#  define FOUNDATION_PLATFORM_REALSIZE 64
#else
typedef   float32_t         real;
#  define REAL_C(x)         FLOAT32_C(x)
#  undef  FOUNDATION_PLATFORM_REALSIZE
#  define FOUNDATION_PLATFORM_REALSIZE 32
#endif

static FORCEINLINE CONSTCALL uint128_t uint128_make( const uint64_t w0, const uint64_t w1 ) { uint128_t u = { w0, w1 }; return u; }
static FORCEINLINE CONSTCALL bool      uint128_equal( const uint128_t u0, const uint128_t u1 ) { return u0.word[0] == u1.word[0] && u0.word[1] == u1.word[1]; }
static FORCEINLINE CONSTCALL uint128_t uint128_null( void ) { return uint128_make( 0, 0 ); }
static FORCEINLINE CONSTCALL bool      uint128_is_null( const uint128_t u0 ) { return !u0.word[0] && !u0.word[1]; }

static FORCEINLINE CONSTCALL uint256_t uint256_make( const uint64_t w0, const uint64_t w1, const uint64_t w2, const uint64_t w3 ) { uint256_t u = { w0, w1, w2, w3 }; return u; }
static FORCEINLINE CONSTCALL bool      uint256_equal( const uint256_t u0, const uint256_t u1 ) { return u0.word[0] == u1.word[0] && u0.word[1] == u1.word[1] && u0.word[2] == u1.word[2] && u0.word[3] == u1.word[3]; }
static FORCEINLINE CONSTCALL uint256_t uint256_null( void ) { return uint256_make( 0, 0, 0, 0 ); }
static FORCEINLINE CONSTCALL bool      uint256_is_null( const uint256_t u0 ) { return !u0.word[0] && !u0.word[1] && !u0.word[2] && !u0.word[3]; }


// Base limits

#define FOUNDATION_MAX_PATHLEN    512


// Wrappers for platforms that not yet support thread-local storage declarations
// For maximum portability use wrapper macros FOUNDATION_DECLARE_THREAD_LOCAL / FOUNDATION_DECLARE_THREAD_LOCAL_ARRAY
// Only works for types that can be safely cast through a uintptr_t (integers, pointers...)
//   FOUNDATION_DECLARE_THREAD_LOCAL( int, profile_block, 0 );
//   FOUNDATION_DECLARE_THREAD_LOCAL_ARRAY( void*, profile_free, 32 );
//   set_thread_profile_block( 1 ); // Assigns 1 to thread-local variable "profile_block"
//   get_thread_profile_free()[0] = some_ptr; // Assigns some_ptr to slot 0 in thread-local array "profile_free"
#if FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID

// Forward declarations of various system APIs
#  if FOUNDATION_PLATFORM_ANDROID
typedef int _pthread_key_t;
#  else
typedef __darwin_pthread_key_t _pthread_key_t;
#  endif
FOUNDATION_EXTERN int pthread_key_create( _pthread_key_t*, void (*)(void*) );
FOUNDATION_EXTERN int pthread_setspecific( _pthread_key_t, const void* );
FOUNDATION_EXTERN void* pthread_getspecific( _pthread_key_t );

FOUNDATION_API void* _allocate_thread_local_block( unsigned int size );

#define FOUNDATION_DECLARE_THREAD_LOCAL( type, name, init ) \
static _pthread_key_t _##name##_key = 0; \
static FORCEINLINE _pthread_key_t get_##name##_key( void ) { if( !_##name##_key ) pthread_key_create( &_##name##_key, init ); return _##name##_key; } \
static FORCEINLINE type get_thread_##name( void ) { return (type)((uintptr_t)pthread_getspecific( get_##name##_key() )); } \
static FORCEINLINE void set_thread_##name( type val ) { pthread_setspecific( get_##name##_key(), (void*)val ); }

#define FOUNDATION_DECLARE_THREAD_LOCAL_ARRAY( type, name, arrsize ) \
static _pthread_key_t _##name##_key = 0; \
static FORCEINLINE _pthread_key_t get_##name##_key( void ) { if( !_##name##_key ) pthread_key_create( &_##name##_key, 0 ); return _##name##_key; } \
static FORCEINLINE type* get_thread_##name( void ) { _pthread_key_t key = get_##name##_key(); type* arr = (type*)pthread_getspecific( key ); if( !arr ) { arr = _allocate_thread_local_block( sizeof( type ) * arrsize ); pthread_setspecific( key, arr ); } return arr; }

#else

#define FOUNDATION_DECLARE_THREAD_LOCAL( type, name, init ) \
static THREADLOCAL type _thread_##name = init; \
static FORCEINLINE void set_thread_##name( type val ) { _thread_##name = val; } \
static FORCEINLINE type get_thread_##name( void ) { return _thread_##name; }

#define FOUNDATION_DECLARE_THREAD_LOCAL_ARRAY( type, name, arrsize ) \
static THREADLOCAL type _thread_##name [arrsize] = {0}; \
static FORCEINLINE type* get_thread_##name( void ) { return _thread_##name; }

#endif
