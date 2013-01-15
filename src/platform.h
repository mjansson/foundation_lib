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
	If you need to override platform or architecture, predefine one of PLATFORM_[...] and PLATFORM_ARCH_[...] to 1

	All preprocessor macros are designed to be always defined and used by value, i.e a check should be performed like
	"#if PLATFORM_[...]" rather than "#ifdef PLATFORM_[...]", since this solves the potential problem of typos in
	preprocessor checks (the #if test will most likely catch the typo with an not defined error, whereas the
	#ifdef macro will just resolve to false)

	This header also defines a bunch of other preprocessor macros:

	COMPILER_[...]
	PREPROCESSOR_TOSTRING
	PREPROCESSOR_JOIN
	RESTRICT
	THREADLOCAL
	DEPRECATED
	FORCEINLINE
	NOINLINE
	PURE
	DECLARE_THREAD_LOCAL
	DECLARE_THREAD_LOCAL_ARRAY

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
#ifndef PLATFORM_ANDROID
#  define PLATFORM_ANDROID 0
#endif
#ifndef PLATFORM_IOS
#  define PLATFORM_IOS 0
#endif
#ifndef PLATFORM_IOS_SIMULATOR
#  define PLATFORM_IOS_SIMULATOR 0
#endif
#ifndef PLATFORM_MACOSX
#  define PLATFORM_MACOSX 0
#endif
#ifndef PLATFORM_LINUX
# define PLATFORM_LINUX 0
#endif
#ifndef PLATFORM_BSD
#  define PLATFORM_BSD 0
#endif
#ifndef PLATFORM_WINDOWS
#  define PLATFORM_WINDOWS 0
#endif

//Architectures
#ifndef  PLATFORM_ARCH_ARM
#  define PLATFORM_ARCH_ARM 0
#endif
#ifndef  PLATFORM_ARCH_X86
#  define PLATFORM_ARCH_X86 0
#endif
#ifndef  PLATFORM_ARCH_X86_64
#  define PLATFORM_ARCH_X86_64 0
#endif
#ifndef  PLATFORM_ARCH_PPC
#  define PLATFORM_ARCH_PPC 0
#endif
#ifndef  PLATFORM_ARCH_PPC_64
#  define PLATFORM_ARCH_PPC_64 0
#endif
#ifndef PLATFORM_ARCH_IA64
#  define PLATFORM_ARCH_IA64 0
#endif

//Platform traits
#define PLATFORM_APPLE 0
#define PLATFORM_POSIX 0

#define PLATFORM_ENDIAN_LITTLE 0
#define PLATFORM_ENDIAN_BIG 0

#define PLATFORM_FAMILY_MOBILE 0
#define PLATFORM_FAMILY_DESKTOP 0

//Compilers
#define COMPILER_CLANG 0
#define COMPILER_GCC 0
#define COMPILER_MSVC 0
#define COMPILER_INTEL 0


//First, platforms and architectures

// Android
#if defined( __ANDROID__ ) || PLATFORM_ANDROID

#  undef  PLATFORM_ANDROID
#  define PLATFORM_ANDROID 1

// Compatibile platforms
#  undef  PLATFORM_LINUX
#  define PLATFORM_LINUX 1
#  undef  PLATFORM_POSIX
#  define PLATFORM_POSIX 1

#  define PLATFORM_NAME "Android"

// Architecture and detailed description
#  if defined( __arm__ ) || PLATFORM_ARCH_ARM
#    undef  PLATFORM_ARCH_ARM
#    define PLATFORM_ARCH_ARM 1
#    define PLATFORM_DESCRIPTION "Android ARM"
#  elif defined( __i386__ ) || PLATFORM_ARCH_X86
#    undef  PLATFORM_ARCH_X86
#    define PLATFORM_ARCH_X86 1
#    define PLATFORM_DESCRIPTION "Android x86"
#  elif defined( __x86_64__ ) || PLATFORM_ARCH_X86_64
#    undef  PLATFORM_ARCH_X86_64
#    define PLATFORM_ARCH_X86_64 1
#    define PLATFORM_DESCRIPTION "Android x86-64"
#  else
#    error Unknown architecture
#  endif

// Traits
#  undef  PLATFORM_ENDIAN_LITTLE
#  define PLATFORM_ENDIAN_LITTLE 1
#  undef  PLATFORM_FAMILY_MOBILE
#  define PLATFORM_FAMILY_MOBILE 1


// MacOS X and iOS
#elif ( defined( __APPLE__ ) && __APPLE__ ) || PLATFORM_IOS || PLATFORM_MACOSX

#define PLATFORM_APPLE 1
#define PLATFORM_POSIX 1

#  if defined( __IPHONE__ ) || ( defined( TARGET_OS_IPHONE ) && TARGET_OS_IPHONE ) || ( defined( TARGET_IPHONE_SIMULATOR ) && TARGET_IPHONE_SIMULATOR ) || PLATFORM_IOS

#    undef  PLATFORM_IOS
#    define PLATFORM_IOS 1

#    define PLATFORM_NAME "iOS"

#    if defined( __arm__ ) || PLATFORM_ARCH_ARM
#      undef  PLATFORM_ARCH_ARM
#      define PLATFORM_ARCH_ARM 1
#      define PLATFORM_DESCRIPTION "iOS ARM"
#    elif defined( __i386__ ) || PLATFORM_ARCH_X86
#      undef  PLATFORM_IOS_SIMULATOR
#      define PLATFORM_IOS_SIMULATOR 1
#      undef  PLATFORM_ARCH_X86
#      define PLATFORM_ARCH_X86 1
#      define PLATFORM_DESCRIPTION "iOS x86 (simulator)"
#    else
#      error Unknown architecture
#    endif

#    undef  PLATFORM_ENDIAN_LITTLE
#    define PLATFORM_ENDIAN_LITTLE 1
#    undef  PLATFORM_FAMILY_MOBILE
#    define PLATFORM_FAMILY_MOBILE 1

#  elif defined( __MACH__ ) || PLATFORM_MACOSX

#    undef  PLATFORM_MACOSX
#    define PLATFORM_MACOSX 1

#    undef  PLATFORM_POSIX
#    define PLATFORM_POSIX 1

#    define PLATFORM_NAME "MacOSX"

#    if defined( __x86_64__ ) ||  defined( __x86_64 ) || defined( __amd64 ) || PLATFORM_ARCH_X86_64
#      undef  PLATFORM_ARCH_X86_64
#      define PLATFORM_ARCH_X86_64 1
#      undef  PLATFORM_ENDIAN_LITTLE
#      define PLATFORM_ENDIAN_LITTLE 1
#      define PLATFORM_DESCRIPTION "MacOSX x86-64"
#    elif defined( __i386__ ) || defined( __intel__ ) || PLATFORM_ARCH_X86
#      undef  PLATFORM_ARCH_X86
#      define PLATFORM_ARCH_X86 1
#      undef  PLATFORM_ENDIAN_LITTLE
#      define PLATFORM_ENDIAN_LITTLE 1
#      define PLATFORM_DESCRIPTION "MacOSX x86"

#    elif defined( __powerpc64__ ) || defined( __POWERPC64__ ) || PLATFORM_ARCH_PPC_64
#      undef  PLATFORM_ARCH_PPC_64
#      define PLATFORM_ARCH_PPC_64 1
#      undef  PLATFORM_ENDIAN_BIG
#      define PLATFORM_ENDIAN_BIG 1
#      define PLATFORM_DESCRIPTION "MacOSX PPC64"
#    elif defined( __powerpc__ ) || defined( __POWERPC__ ) || PLATFORM_ARCH_PPC
#      undef  PLATFORM_ARCH_PPC
#      define PLATFORM_ARCH_PPC 1
#      undef  PLATFORM_ENDIAN_BIG
#      define PLATFORM_ENDIAN_BIG 1
#      define PLATFORM_DESCRIPTION "MacOSX PPC"

#    else
#      error Unknown architecture
#    endif

#    undef  PLATFORM_FAMILY_DESKTOP
#    define PLATFORM_FAMILY_DESKTOP 1

#  else
#    error Unknown Apple Platform
#  endif

// Linux
#elif ( defined( __linux__ ) || defined( __linux ) ) || PLATFORM_LINUX

#  undef  PLATFORM_LINUX
#  define PLATFORM_LINUX 1

#  undef  PLATFORM_POSIX
#  define PLATFORM_POSIX 1

#  define PLATFORM_NAME "Linux"

#  if defined( __x86_64__ ) || defined( __x86_64 ) || defined( __amd64 ) || PLATFORM_ARCH_X86_64
#    undef  PLATFORM_ARCH_X86_64
#    define PLATFORM_ARCH_X86_64 1
#    undef  PLATFORM_ENDIAN_LITTLE
#    define PLATFORM_ENDIAN_LITTLE 1
#    define PLATFORM_DESCRIPTION "Linux x86-64"
#  elif defined( __i386__ ) || defined( __intel__ ) || defined( _M_IX86 ) || PLATFORM_ARCH_X86
#    undef  PLATFORM_ARCH_X86
#    define PLATFORM_ARCH_X86 1
#    undef  PLATFORM_ENDIAN_LITTLE
#    define PLATFORM_ENDIAN_LITTLE 1
#    define PLATFORM_DESCRIPTION "Linux x86"

#  elif defined( __powerpc64__ ) || defined( __POWERPC64__ ) || PLATFORM_ARCH_PPC_64
#    undef  PLATFORM_ARCH_PPC_64
#    define PLATFORM_ARCH_PPC_64 1
#    undef  PLATFORM_ENDIAN_BIG
#    define PLATFORM_ENDIAN_BIG 1
#    define PLATFORM_DESCRIPTION "Linux PPC64"
#  elif defined( __powerpc__ ) || defined( __POWERPC__ ) || PLATFORM_ARCH_PPC
#    undef  PLATFORM_ARCH_PPC
#    define PLATFORM_ARCH_PPC 1
#    undef  PLATFORM_ENDIAN_BIG
#    define PLATFORM_ENDIAN_BIG 1
#    define PLATFORM_DESCRIPTION "Linux PPC"

#  else
#    error Unknown architecture
#  endif

#  undef  PLATFORM_FAMILY_DESKTOP
#  define PLATFORM_FAMILY_DESKTOP 1

//BSD family
#elif ( defined( __BSD__ ) || defined( __FreeBSD__ ) ) || PLATFORM_BSD

#  undef  PLATFORM_BSD
#  define PLATFORM_BSD 1

#  undef  PLATFORM_POSIX
#  define PLATFORM_POSIX 1

#  define PLATFORM_NAME "BSD"

#  if defined( __x86_64__ ) || defined( __x86_64 ) || defined( __amd64 ) || PLATFORM_ARCH_X86_64
#    undef  PLATFORM_ARCH_X86_64
#    define PLATFORM_ARCH_X86_64 1
#    undef  PLATFORM_ENDIAN_LITTLE
#    define PLATFORM_ENDIAN_LITTLE 1
#    define PLATFORM_DESCRIPTION "BSD x86-64"
#  elif defined( __i386__ ) || defined( __intel__ ) || defined( _M_IX86 ) || PLATFORM_ARCH_X86
#    undef  PLATFORM_ARCH_X86
#    define PLATFORM_ARCH_X86 1
#    undef  PLATFORM_ENDIAN_LITTLE
#    define PLATFORM_ENDIAN_LITTLE 1
#    define PLATFORM_DESCRIPTION "BSD x86"

#  elif defined( __powerpc64__ ) || defined( __POWERPC64__ ) || PLATFORM_ARCH_PPC_64
#    undef  PLATFORM_ARCH_PPC_64
#    define PLATFORM_ARCH_PPC_64 1
#    undef  PLATFORM_ENDIAN_BIG
#    define PLATFORM_ENDIAN_BIG 1
#    define PLATFORM_DESCRIPTION "BSD PPC64"
#  elif defined( __powerpc__ ) || defined( __POWERPC__ ) || PLATFORM_ARCH_PPC
#    undef  PLATFORM_ARCH_PPC
#    define PLATFORM_ARCH_PPC 1
#    undef  PLATFORM_ENDIAN_BIG
#    define PLATFORM_ENDIAN_BIG 1
#    define PLATFORM_DESCRIPTION "BSD PPC"

#  else
#    error Unknown architecture
#  endif

#  undef  PLATFORM_FAMILY_DESKTOP
#  define PLATFORM_FAMILY_DESKTOP 1

// Windows
#elif defined( _WIN32 ) || defined( __WIN32__ ) || defined( _WIN64 ) || PLATFORM_WINDOWS

#  undef  PLATFORM_WINDOWS
#  define PLATFORM_WINDOWS 1

#  define PLATFORM_NAME "Windows"

#  if defined( __x86_64__ ) || defined( _M_AMD64 ) || defined( _AMD64_ ) || PLATFORM_ARCH_X86_64
#    undef  PLATFORM_ARCH_X86_64
#    define PLATFORM_ARCH_X86_64 1
#    define PLATFORM_DESCRIPTION "Windows x86-64"
#  elif defined( __x86__ ) || defined( _M_IX86 ) || defined( _X86_ ) || PLATFORM_ARCH_X86
#    undef  PLATFORM_ARCH_X86
#    define PLATFORM_ARCH_X86 1
#    define PLATFORM_DESCRIPTION "Windows x86"
#  elif defined( __ia64__ ) || defined( _M_IA64 ) || defined( _IA64_ ) || PLATFORM_ARCH_IA64
#    undef  PLATFORM_ARCH_IA64
#    define PLATFORM_ARCH_IA64 1
#    define PLATFORM_DESCRIPTION "Windows IA-64"

#  else
#    error Unknown architecture
#  endif

#  undef  PLATFORM_ENDIAN_LITTLE
#  define PLATFORM_ENDIAN_LITTLE 1

#  undef  PLATFORM_FAMILY_DESKTOP
#  define PLATFORM_FAMILY_DESKTOP 1

#  if defined( FOUNDATION_COMPILE ) && FOUNDATION_COMPILE && !defined( _CRT_SECURE_NO_WARNINGS )
#    define _CRT_SECURE_NO_WARNINGS 1
#  endif

#else
#  error Unknown platform
#endif


//Utility macros
#define PREPROCESSOR_TOSTRING( x )    _PREPROCESSOR_TOSTRING(x)
#define _PREPROCESSOR_TOSTRING( x )   #x

#define PREPROCESSOR_JOIN( a, b )           _PREPROCESSOR_JOIN( a, b )
#define _PREPROCESSOR_JOIN( a, b )          _PREPROCESSOR_JOIN_INTERNAL( a, b )
#define _PREPROCESSOR_JOIN_INTERNAL( a, b ) a##b


//Compilers

// CLang
#if defined( __clang__ )

#  undef  COMPILER_CLANG
#  define COMPILER_CLANG 1

#  define COMPILER_NAME "clang"
#  define COMPILER_DESCRIPTION COMPILER_NAME " " __clang_version__

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

#  undef  COMPILER_GCC
#  define COMPILER_GCC 1

#  define COMPILER_NAME "gcc"
#  define COMPILER_DESCRIPTION COMPILER_NAME " v" PREPROCESSOR_TOSTRING( __GNUC__ ) "." PREPROCESSOR_TOSTRING( __GNUC_MINOR__ )

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

#  undef  COMPILER_INTEL
#  define COMPILER_INTEL 1

#  define COMPILER_NAME "intel"
#  if defined( __ICL )
#    define COMPILER_DESCRIPTION COMPILER_NAME " v" PREPROCESSOR_TOSTRING( __ICL )
#  elif defined( __ICC )
#    define COMPILER_DESCRIPTION COMPILER_NAME " v" PREPROCESSOR_TOSTRING( __ICC )
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

#  undef  COMPILER_MSVC
#  define COMPILER_MSVC 1

#  define COMPILER_NAME "msvc"
#  define COMPILER_DESCRIPTION COMPILER_NAME " v" PREPROCESSOR_TOSTRING( _MSC_VER )

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
#if PLATFORM_ARCH_X86_64 || PLATFORM_ARCH_PPC_64 || PLATFORM_ARCH_IA64
#  define PLATFORM_POINTER_SIZE 8
#else
#  define PLATFORM_POINTER_SIZE 4
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

#if ( defined( FLT_EVAL_METHOD ) && ( FLT_EVAL_METHOD >= 1 ) ) || ( defined( PLATFORM_REALSIZE ) && ( PLATFORM_REALSIZE > 32 ) )
typedef   float64_t         real;
#  define REAL_C(x)         FLOAT64_C(x)
#  undef  PLATFORM_REALSIZE
#  define PLATFORM_REALSIZE 64
#else
typedef   float32_t         real;
#  define REAL_C(x)         FLOAT32_C(x)
#  undef  PLATFORM_REALSIZE
#  define PLATFORM_REALSIZE 32
#endif

static FORCEINLINE CONSTCALL uint128_t uint128_make( const uint64_t w0, const uint64_t w1 ) { uint128_t u = { w0, w1 }; return u; }
static FORCEINLINE CONSTCALL bool      uint128_equal( const uint128_t u0, const uint128_t u1 ) { return u0.word[0] == u1.word[0] && u0.word[1] == u1.word[1]; }
static FORCEINLINE CONSTCALL uint128_t uint128_null( void ) { return uint128_make( 0, 0 ); }
static FORCEINLINE CONSTCALL bool      uint128_is_null( const uint128_t u0 ) { return !u0.word[0] && !u0.word[1]; }

static FORCEINLINE CONSTCALL uint256_t uint256_make( const uint64_t w0, const uint64_t w1, const uint64_t w2, const uint64_t w3 ) { uint256_t u = { w0, w1, w2, w3 }; return u; }
static FORCEINLINE CONSTCALL bool      uint256_equal( const uint256_t u0, const uint256_t u1 ) { return u0.word[0] == u1.word[0] && u0.word[1] == u1.word[1] && u0.word[2] == u1.word[2] && u0.word[3] == u1.word[3]; }
static FORCEINLINE CONSTCALL uint256_t uint256_null( void ) { return uint256_make( 0, 0, 0, 0 ); }
static FORCEINLINE CONSTCALL bool      uint256_is_null( const uint256_t u0 ) { return !u0.word[0] && !u0.word[1] && !u0.word[2] && !u0.word[3]; }


// Wrappers for platforms that not yet support thread-local storage declarations
// For maximum portability use wrapper macros DECLARE_THREAD_LOCAL / DECLARE_THREAD_LOCAL_ARRAY
// Only works for types that can be safely cast through a uintptr_t (integers, pointers...)
//   DECLARE_THREAD_LOCAL( int, profile_block, 0 );
//   DECLARE_THREAD_LOCAL_ARRAY( void*, profile_free, 32 );
//   set_thread_profile_block( 1 ); // Assigns 1 to thread-local variable "profile_block"
//   get_thread_profile_free()[0] = some_ptr; // Assigns some_ptr to slot 0 in thread-local array "profile_free"
#if PLATFORM_MACOSX || PLATFORM_IOS || PLATFORM_ANDROID

// Forward declarations of various system APIs
#  if PLATFORM_ANDROID
typedef int _pthread_key_t;
#  else
typedef __darwin_pthread_key_t _pthread_key_t;
#  endif
FOUNDATION_EXTERN int pthread_key_create( _pthread_key_t*, void (*)(void*) );
FOUNDATION_EXTERN int pthread_setspecific( _pthread_key_t, const void* );
FOUNDATION_EXTERN void* pthread_getspecific( _pthread_key_t );

FOUNDATION_API void* _allocate_thread_local_block( unsigned int size );

#define DECLARE_THREAD_LOCAL( type, name, init ) \
static _pthread_key_t _##name##_key = 0; \
static FORCEINLINE _pthread_key_t get_##name##_key( void ) { if( !_##name##_key ) pthread_key_create( &_##name##_key, init ); return _##name##_key; } \
static FORCEINLINE type get_thread_##name( void ) { return (type)((uintptr_t)pthread_getspecific( get_##name##_key() )); } \
static FORCEINLINE void set_thread_##name( type val ) { pthread_setspecific( get_##name##_key(), (void*)val ); }

#define DECLARE_THREAD_LOCAL_ARRAY( type, name, arrsize ) \
static _pthread_key_t _##name##_key = 0; \
static FORCEINLINE _pthread_key_t get_##name##_key( void ) { if( !_##name##_key ) pthread_key_create( &_##name##_key, 0 ); return _##name##_key; } \
static FORCEINLINE type* get_thread_##name( void ) { _pthread_key_t key = get_##name##_key(); type* arr = (type*)pthread_getspecific( key ); if( !arr ) { arr = _allocate_thread_local_block( sizeof( type ) * arrsize ); pthread_setspecific( key, arr ); } return arr; }

#else

#define DECLARE_THREAD_LOCAL( type, name, init ) \
static THREADLOCAL type _thread_##name = init; \
static FORCEINLINE void set_thread_##name( type val ) { _thread_##name = val; } \
static FORCEINLINE type get_thread_##name( void ) { return _thread_##name; }

#define DECLARE_THREAD_LOCAL_ARRAY( type, name, arrsize ) \
static THREADLOCAL type _thread_##name [arrsize] = {0}; \
static FORCEINLINE type* get_thread_##name( void ) { return _thread_##name; }

#endif
