/* types.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file types.h
    Foundation types */

#include <foundation/platform.h>
#include <foundation/build.h>


// PRIMITIVE TYPES

//! Error severity level
typedef enum
{
	ERRORLEVEL_NONE    = 0,
	ERRORLEVEL_DEBUG,
	ERRORLEVEL_INFO,
	ERRORLEVEL_WARNING,
	ERRORLEVEL_ERROR,
	ERRORLEVEL_PANIC
} error_level_t;

//! Error identifiers
typedef enum
{
	ERROR_NONE              = 0,
	ERROR_INVALID_VALUE,
	ERROR_UNSUPPORTED,
	ERROR_NOT_IMPLEMENTED,
	ERROR_OUT_OF_MEMORY,
	ERROR_INTERNAL_FAILURE,
	ERROR_MALLOC_FAILURE,
	ERROR_MEMORY_LEAK,
	ERROR_ACCESS_DENIED,
	ERROR_EXCEPTION,
	ERROR_SYSTEM_CALL_FAIL,
	ERROR_SCRIPT,
	ERROR_UNKNOWN_TYPE,
	ERROR_UNKNOWN_RESOURCE,
	ERROR_MEMORY_ALIGNMENT,
	ERROR_DEPRECATED
} error_t;

//! Warning classes
typedef enum
{
	WARNING_PERFORMANCE,
	WARNING_DEPRECATED,
	WARNING_BAD_DATA,
	WARNING_MEMORY,
	WARNING_UNSUPPORTED,
	WARNING_SUSPICIOUS,
	WARNING_SCRIPT,
	WARNING_SYSTEM_CALL_FAIL,
	WARNING_DEADLOCK
} warning_class_t;

//! Memory hints
typedef enum
{
	MEMORY_TEMPORARY,
	MEMORY_PERSISTENT
} memory_hint_t;

//! \Platform identifiers. For compile-time platform selection, use the FOUNDATION_PLATFORM_[...] preprocessor macros
typedef enum
{
	PLATFORM_WINDOWS   = 1,
	PLATFORM_LINUX,
	PLATFORM_MACOSX,
	PLATFORM_IOS,
	PLATFORM_ANDROID,
	PLATFORM_RASPBERRYPI
} platform_t;

/*! Architecture identifiers for all architectures the engine supports. For compile-time selection of
    architecture, use the FOUNDATION_PLATFORM_ARCH_[...] preprocessor macros. */
typedef enum
{
	/*! x86 (Classic 32-bit x86 compatible CPUs) */
	ARCHITECTURE_X86          = 0,

	/*! x86-64 (x86 with 64-bit extensions) */
	ARCHITECTURE_X86_64       = 1,

	/*! PowerPC 32-bit */
	ARCHITECTURE_PPC          = 2,
	
	/*! PowerPC 64-bit */
	ARCHITECTURE_PPC_64       = 3,

	/*! ARM 6 */
	ARCHITECTURE_ARM6         = 6,

	/*! ARM 7 */
	ARCHITECTURE_ARM7         = 7
} architecture_t;

//! Machine byte order identifiers
typedef enum
{
	BYTEORDER_LITTLEENDIAN = 0,
	BYTEORDER_BIGENDIAN    = 1
} byteorder_t;

//! Open modes for streams
typedef enum
{
	STREAM_IN                  = 0x0001,
	STREAM_OUT                 = 0x0002,
	STREAM_TRUNCATE            = 0x0010,
	STREAM_ATEND               = 0x0020,
	STREAM_BINARY              = 0x0100,
	STREAM_SYNC                = 0x0200
} stream_mode_t;

typedef enum
{
	STREAMTYPE_INVALID         = 0,
	STREAMTYPE_MEMORY,
	STREAMTYPE_FILE,
	STREAMTYPE_SOCKET,
	STREAMTYPE_RINGBUFFER,
	STREAMTYPE_ASSET
} stream_type_t;

//! Stream seek directions
typedef enum
{
	STREAM_SEEK_BEGIN          = 0x0000,
	STREAM_SEEK_CURRENT        = 0x0001,
	STREAM_SEEK_END            = 0x0002
} stream_seek_mode_t;

//! Thread priority
typedef enum
{
	THREAD_PRIORITY_LOW = 0,
	THREAD_PRIORITY_BELOWNORMAL,
	THREAD_PRIORITY_NORMAL,
	THREAD_PRIORITY_ABOVENORMAL,
	THREAD_PRIORITY_HIGHEST,
	THREAD_PRIORITY_TIMECRITICAL
} thread_priority_t;

//! Process flags
typedef enum
{
	//! Attached, spawn method will block until process ends and then return process exit code
	PROCESS_ATTACHED                          = 0,

	//! Detached, spawn method will immediately return with code 0
	PROCESS_DETACHED                          = 0x01,

	//! Create a console window for process
	PROCESS_CONSOLE                           = 0x02,

	//! Windows platform only, use ShellExecute instead of CreateProcess
	PROCESS_WINDOWS_USE_SHELLEXECUTE          = 0x04,
	
	//! MacOSX platform only, use LSOpenApplication instead of fork/execve
	PROCESS_OSX_USE_OPENAPPLICATION           = 0x08
} process_flag_t;

//! Process status
typedef enum
{
	//! Returned when given invalid arguments
	PROCESS_INVALID_ARGS                      = 0x7FFFFFF0,

	//! Returned when process was terminated by signal
	PROCESS_TERMINATED_SIGNAL                 = 0x7FFFFFF1,
	
	//! Returned when detached process is still running
	PROCESS_STILL_ACTIVE                      = 0x7FFFFFFF
} process_status_t;

typedef enum
{
	//! System identifier for foundation
	SYSTEM_FOUNDATION = 1
} system_t;

typedef enum
{
	//! Application has been asked to terminate
	FOUNDATIONEVENT_TERMINATE = 1,

	//! File was created
	FOUNDATIONEVENT_FILE_CREATED,

	//! File was deleted
	FOUNDATIONEVENT_FILE_DELETED,

	//! File was modified
	FOUNDATIONEVENT_FILE_MODIFIED
} foundation_event_id;

typedef enum
{
	BLOWFISH_ECB,
	BLOWFISH_CBC,
	BLOWFISH_CFB,
	BLOWFISH_OFB
} blowfish_mode_t;

typedef enum
{
	RADIXSORT_INT32,
	RADIXSORT_UINT32,
	RADIXSORT_INT64,
	RADIXSORT_UINT64,
	RADIXSORT_FLOAT32,
	RADIXSORT_FLOAT64
} radixsort_data_t;

//! Hash value
typedef uint64_t         hash_t;

//! Tick type
typedef uint64_t         tick_t;

//! Deltatime type
typedef real             deltatime_t;

//! Object handle
typedef uint64_t         object_t;

//! Index for radix sorter (define to 32bit if need to sort more than 2^16 items in one array)
typedef uint16_t         radixsort_index_t;
//typedef uint32_t       radixsort_index_t;


//! Error handler callback
typedef int           (* error_callback_fn )( error_level_t level, error_t error );

//! Assert handler callback
typedef int           (* assert_handler_fn )( const char* condition, const char* file, int line, const char* msg );

//! Log output callback
typedef void          (* log_callback_fn )( int severity, const char* msg );

typedef void*         (* memory_allocate_fn )( uint64_t size, unsigned int align, memory_hint_t hint );
typedef void*         (* memory_allocate_zero_fn )( uint64_t size, unsigned int align, memory_hint_t hint );
typedef void*         (* memory_reallocate_fn )( void* p, uint64_t size, unsigned int align );
typedef void          (* memory_deallocate_fn )( void* p );

//! Callback function for writing profiling data to a stream
typedef void          (* profile_write_fn)( void*, uint64_t );

//! Thread execution function
typedef void*         (* thread_fn)( object_t, void* );

//! Guarded function, used with crash_guard
typedef int           (* crash_guard_fn)( void* );

//! Crash callback
typedef void          (* crash_dump_callback_fn)( const char* );

#define CRASH_DUMP_GENERATED        0x0badf00dL


// COMPLEX TYPES

//! Memory management callbacks
typedef struct _foundation_memory_system
{
	memory_allocate_fn              allocate;
	memory_allocate_zero_fn         allocate_zero;
	memory_reallocate_fn            reallocate;
	memory_deallocate_fn            deallocate;
} memory_system_t;

//! Version identifier
typedef union _foundation_version
{
	uint128_t                       version;
	struct
	{
		uint16_t                    major;
		uint16_t                    minor;
		uint32_t                    revision;
		uint32_t                    build;
		uint32_t                    control;
	}                               sub;
} version_t;

//! Application declaration
typedef struct _foundation_application
{
	const char*                     name;
	const char*                     short_name;
	const char*                     config_dir;
	version_t                       version;
	crash_dump_callback_fn          dump_callback;
} application_t;

typedef struct _foundation_error_frame
{
	const char*                     name;
	const char*                     data;
} error_frame_t;

typedef struct _foundation_error_context
{
	error_frame_t                   frame[BUILD_SIZE_ERROR_CONTEXT_DEPTH];
	int                             depth;
} error_context_t;

//! Object base structure. If changing base object layout, change objectmap_lookup()
#define FOUNDATION_DECLARE_OBJECT               \
	ALIGN(16) volatile int32_t      ref;        \
	int32_t                         objecttype; \
	object_t                        id

typedef struct _foundation_object_base
{
	FOUNDATION_DECLARE_OBJECT;
} object_base_t;

//! Object map
typedef struct _foundation_objectmap
{
	ALIGN(16) volatile uint64_t     free;
	uint64_t                        size;
	ALIGN(16) volatile uint64_t     id;
	uint64_t                        size_bits;
	uint64_t                        id_max;
	uint64_t                        mask_index;
	uint64_t                        mask_id;
	void*                           map[];
} objectmap_t;

//! Event base structure
#define FOUNDATION_DECLARE_EVENT       \
	uint8_t               system;      \
	uint8_t               id;          \
	uint16_t              serial;      \
	uint16_t              size;        \
	object_t              object

typedef struct _foundation_event
{
	FOUNDATION_DECLARE_EVENT;
	char                  payload[];
} event_t;

//! Semaphore
#if FOUNDATION_PLATFORM_WINDOWS
typedef void*                        semaphore_t;
#elif FOUNDATION_PLATFORM_MACOSX
typedef struct OpaqueMPSemaphoreID*  MPSemaphoreID;
typedef struct _foundation_semaphore
{
	char*                            name;
	union
	{
		MPSemaphoreID                unnamed;
		int*                         named;
	} sem;
} semaphore_t;
#elif FOUNDATION_PLATFORM_IOS
typedef struct dispatch_semaphore_s* dispatch_semaphore_t;
typedef dispatch_semaphore_t         semaphore_t;
#elif FOUNDATION_PLATFORM_POSIX
typedef union
{
#  if FOUNDATION_PLATFORM_ANDROID
	volatile unsigned int  count;
#else
#  if FOUNDATION_PLATFORM_ARCH_X86_64
	char __size[64];
#  else
	char __size[32];
#  endif
	long int __align;
#endif
} semaphore_native_t;
typedef struct _foundation_semaphore
{
	char*                            name;
	semaphore_native_t*              sem;
	semaphore_native_t               unnamed;
} semaphore_t;
#else
#  error Semaphore not implemented yet on this platform
#endif


// OPAQUE COMPLEX TYPES

typedef struct _foundation_md5              md5_t;

typedef struct _foundation_stream           stream_t;
typedef struct _foundation_directory        directory_t;

typedef struct _foundation_mutex            mutex_t;
typedef struct _foundation_process          process_t;

typedef struct _foundation_event_block      event_block_t;
typedef struct _foundation_event_stream     event_stream_t;

typedef struct _foundation_ringbuffer       ringbuffer_t;

typedef struct _foundation_blowfish         blowfish_t;

typedef struct _foundation_radixsort        radixsort_t;


// UTILITY FUNCTIONS

//! Pack version definition into a 128-bit integer
static FORCEINLINE CONSTCALL version_t      version_make( unsigned int major, unsigned int minor, unsigned int revision, unsigned int build, unsigned int control ) { version_t v; v.sub.major = (uint16_t)major; v.sub.minor = (uint16_t)minor; v.sub.revision = revision, v.sub.build = build; v.sub.control = control; return v; }
