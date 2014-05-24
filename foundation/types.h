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

/*! Error severity level. The higher the value, the more severe the error. Error level 0
    (ERRORLEVEL_NONE) is used to indicate no error
    \internal Do not change order! */
typedef enum
{
	//! No error
	ERRORLEVEL_NONE    = 0,

	//! Debug level, usually ignored in anything except debug builds. Execution will continue as expected.
	ERRORLEVEL_DEBUG,

	//! Information level, contains generally useful information. Execution will continue as expected.
	ERRORLEVEL_INFO,

	//! Warning level, contains important information. Operation failed, but execution can continue.
	ERRORLEVEL_WARNING,

	//! Error level, contains vital information. Operation failed and execution might be affected.
	ERRORLEVEL_ERROR,

	//! Panic level, contains vital information. Operation failed and execution cannot continue.
	ERRORLEVEL_PANIC
} error_level_t;

/*! Error identifiers. Error 0 (ERROR_NONE) is used to indicate no error.
    \internal Do not change order, only append! */
typedef enum
{
	//! No error
	ERROR_NONE              = 0,

	//! An invalid value was passed to the function
	ERROR_INVALID_VALUE,

	//! The function is unsupported on the current system
	ERROR_UNSUPPORTED,

	//! The function is not yet implemented
	ERROR_NOT_IMPLEMENTED,

	//! The function could not allocate the needed memory and/or resources
	ERROR_OUT_OF_MEMORY,

	//! A memory leak was detected
	ERROR_MEMORY_LEAK,

	//! Memory alignment check failed
	ERROR_MEMORY_ALIGNMENT,

	//! Internal failed, unspecified. The function encountered a state it did not expect or support
	ERROR_INTERNAL_FAILURE,

	//! The function call was not allowed
	ERROR_ACCESS_DENIED,

	//! An exception was thrown
	ERROR_EXCEPTION,

	//! A system call failed
	ERROR_SYSTEM_CALL_FAIL,

	//! The function encountered an unsupported data type for the requested operation
	ERROR_UNKNOWN_TYPE,

	//! The function could not resolve the resource to use in the requested operation
	ERROR_UNKNOWN_RESOURCE,

	//! The function is deprecated and should not be used
	ERROR_DEPRECATED,

	//! An assert triggered
	ERROR_ASSERT,

	//! A script generated an error
	ERROR_SCRIPT,

	//! Marker, last reserved internal error identifier
	ERROR_LAST_BUILTIN  = 0x0fff
} error_t;

/*! Warning classes
    \internal Do not change order, only append! */
typedef enum
{
	//! Performance warning
	WARNING_PERFORMANCE = 0,

	//! Function is deprecated
	WARNING_DEPRECATED,

	//! Bad data passed to/used in function
	WARNING_BAD_DATA,

	//! Memory issues (running low, leaks, ...)
	WARNING_MEMORY,

	//! Unsupported function
	WARNING_UNSUPPORTED,

	//! General warning, function encountered a suspicious state
	WARNING_SUSPICIOUS,

	//! System call failed
	WARNING_SYSTEM_CALL_FAIL,

	//! Potential deadlock encountered
	WARNING_DEADLOCK,

	//! Script generated warning
	WARNING_SCRIPT,

	//! Marker, last reserved inernal warning identifier
	WARNING_LAST_BUILTIN  = 0x0fff
} warning_t;

//! Memory hints
typedef enum
{
	//! Memory is persistent (retained when function returns)
	MEMORY_PERSISTENT          = 0x0000,

	//! Memory is temporary (extremely short lived and generally freed before function returns)
	MEMORY_TEMPORARY           = 0x0001,

	//! Memory is thread local
	MEMORY_THREAD              = 0x0002,

	//! Memory should be allocated in low 32-bit address space
	MEMORY_32BIT_ADDRESS       = 0x0004
} memory_hint_t;

//! Platform identifiers. For compile-time platform selection, use the FOUNDATION_PLATFORM_[...] preprocessor macros
typedef enum
{
	//! Windows
	PLATFORM_WINDOWS   = 1,

	//! Linux
	PLATFORM_LINUX,

	//! MacOS X
	PLATFORM_MACOSX,

	//! iOS (iPhone, iPad)
	PLATFORM_IOS,

	//! Android
	PLATFORM_ANDROID,

	//! Raspberry Pi (linux flavour)
	PLATFORM_RASPBERRYPI
} platform_t;

/*! Architecture identifiers for all architectures the engine supports. For compile-time selection of
    architecture, use the FOUNDATION_ARCH_[...] preprocessor macros. */
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
	ARCHITECTURE_ARM7         = 7,
	
	/*! ARM 8 64bit */
	ARCHITECTURE_ARM8_64      = 8,

	/*! ARM 5 */
	ARCHITECTURE_ARM5         = 9,

	/*! MIPS */
	ARCHITECTURE_MIPS         = 10
} architecture_t;

//! Machine byte order identifiers
typedef enum
{
	//! Little endian
	BYTEORDER_LITTLEENDIAN = 0,

	//! Big endian
	BYTEORDER_BIGENDIAN    = 1
} byteorder_t;

//! Application flags
typedef enum
{
	//! Application is a command line utility and should not have a normal windowing system interaction loop)
	APPLICATION_UTILITY        = 0x0001,

	//! Application is a daemon/service
	APPLICATION_DAEMON         = 0x0002
} application_flag_t;

//! Open modes for streams
typedef enum
{
	//! Readable
	STREAM_IN                  = 0x0001,

	//! Writable
	STREAM_OUT                 = 0x0002,

	//! Truncate on open
	STREAM_TRUNCATE            = 0x0010,

	//! Position at end on open
	STREAM_ATEND               = 0x0020,

	//! Read/write in binary mode
	STREAM_BINARY              = 0x0100,

	//! Force sync on each write
	STREAM_SYNC                = 0x0200
} stream_mode_t;

//! Built-in stream type identifiers
typedef enum
{
	//! Invalid identifier
	STREAMTYPE_INVALID         = 0,

	//! Memory buffer stream
	STREAMTYPE_MEMORY,

	//! File system stream
	STREAMTYPE_FILE,

	//! Socket stream
	STREAMTYPE_SOCKET,

	//! Ring buffer stream
	STREAMTYPE_RINGBUFFER,

	//! Asset stream (Android)
	STREAMTYPE_ASSET,

	//! Pipe stream
	STREAMTYPE_PIPE,

	//! Standard stream (stdin, stderr, stdout)
	STREAMTYPE_STDSTREAM
} stream_type_t;

//! Stream seek directions
typedef enum
{
	//! Seek from start of stream
	STREAM_SEEK_BEGIN          = 0x0000,

	//! Seek from current position of stream
	STREAM_SEEK_CURRENT        = 0x0001,

	//! Seek from end of stream
	STREAM_SEEK_END            = 0x0002
} stream_seek_mode_t;

//! Thread priority
typedef enum
{
	//! Lowest possible priority
	THREAD_PRIORITY_LOW = 0,

	//! Unimportant priority
	THREAD_PRIORITY_BELOWNORMAL,

	//! Normal priority
	THREAD_PRIORITY_NORMAL,

	//! Important priority
	THREAD_PRIORITY_ABOVENORMAL,

	//! High priority
	THREAD_PRIORITY_HIGHEST,

	//! Absolutely critical
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

	//! Create stdout/stdin pipes to process
	PROCESS_STDSTREAMS                        = 0x04,

	//! Windows platform only, use ShellExecute instead of CreateProcess
	PROCESS_WINDOWS_USE_SHELLEXECUTE          = 0x08,
	
	//! MacOSX platform only, use LSOpenApplication instead of fork/execve
	PROCESS_OSX_USE_OPENAPPLICATION           = 0x10
} process_flag_t;

//! Process status
typedef enum
{
	//! Returned when given invalid arguments
	PROCESS_INVALID_ARGS                      = 0x7FFFFFF0,

	//! Returned when process was terminated by signal
	PROCESS_TERMINATED_SIGNAL                 = 0x7FFFFFF1,

	//! Returned when process wait was interrupted
	PROCESS_WAIT_INTERRUPTED                  = 0x7FFFFFF2,

	//! Returned when process wait failed for unknown reasons
	PROCESS_WAIT_FAILED                       = 0x7FFFFFF3,
	
	//! Returned when detached process is still running
	PROCESS_STILL_ACTIVE                      = 0x7FFFFFFF
} process_status_t;

//! Foundadtion library level event identifiers
typedef enum
{
	//! Application has been asked to start
	FOUNDATIONEVENT_START = 1,

	//! Application has been asked to terminate
	FOUNDATIONEVENT_TERMINATE,

	//! Application has been asked to pause
	FOUNDATIONEVENT_PAUSE,

	//! Application has been asked to resume
	FOUNDATIONEVENT_RESUME,

	//! File was created
	FOUNDATIONEVENT_FILE_CREATED,

	//! File was deleted
	FOUNDATIONEVENT_FILE_DELETED,

	//! File was modified
	FOUNDATIONEVENT_FILE_MODIFIED,
	
	//! Low memory warning
	FOUNDATIONEVENT_LOW_MEMORY_WARNING
} foundation_event_id;

//! Event flags
typedef enum
{
	//! Event is delayed and will be delivered at a later timestamp
	EVENTFLAG_DELAY  = 1
} event_flag_t;

//! Blowfish block cipher mode of operation (http://en.wikipedia.org/wiki/Block_cipher_mode_of_operation)
typedef enum
{
	//! Electronic codebook
	BLOWFISH_ECB = 0,

	//! Cipher-block chaining
	BLOWFISH_CBC,

	//! Cipher feedback
	BLOWFISH_CFB,

	//! Output feedback
	BLOWFISH_OFB
} blowfish_mode_t;

//! Radix sort data types
typedef enum
{
	//! 32-bit signed integer
	RADIXSORT_INT32 = 0,

	//! 32-bit unsigned integer
	RADIXSORT_UINT32,

	//! 64-bit signed integer
	RADIXSORT_INT64,

	//! 64-bit unsigned integer
	RADIXSORT_UINT64,

	//! 32-bit floating point
	RADIXSORT_FLOAT32,

	//! 64-bit floating point
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

//! UUID
typedef uint128_t        uuid_t;


//! Error handler callback
typedef int           (* error_callback_fn )( error_level_t level, error_t error );

//! Assert handler callback
typedef int           (* assert_handler_fn )( uint64_t context, const char* condition, const char* file, int line, const char* msg );

//! Log output callback
typedef void          (* log_callback_fn )( uint64_t context, int severity, const char* msg );

//! Subsystem initialization
typedef int           (* system_initialize_fn )( void );

//! Subsystem shutdown
typedef void          (* system_shutdown_fn )( void );

//! Memory system allocation function
typedef void*         (* memory_allocate_fn )( uint64_t context, uint64_t size, unsigned int align, int hint );

//! Memory system allocate-and-zero-out function
typedef void*         (* memory_allocate_zero_fn )( uint64_t context, uint64_t size, unsigned int align, int hint );

//! Memory system reallocation function
typedef void*         (* memory_reallocate_fn )( void* p, uint64_t size, unsigned int align, uint64_t oldsize );

//! Memory system deallocation function
typedef void          (* memory_deallocate_fn )( void* p );

//! Memory tracker tracking function
typedef void          (* memory_track_fn )( void*, uint64_t );

//! Memory tracker untracking function
typedef void          (* memory_untrack_fn )( void* );

//! Callback function for writing profiling data to a stream
typedef void          (* profile_write_fn)( void*, uint64_t );

//! Callback function for reading profiling data from a stream
typedef void          (* profile_read_fn)( void*, uint64_t );

//! Thread execution function
typedef void*         (* thread_fn)( object_t, void* );

//! Guarded function, used with crash_guard
typedef int           (* crash_guard_fn)( void* );

//! Crash callback
typedef void          (* crash_dump_callback_fn)( const char* );

//! Identifier returned from threads and crash guards after a fatal exception (crash) has been caught
#define CRASH_DUMP_GENERATED        0x0badf00dL


// COMPLEX TYPES

//! Memory management callbacks
typedef struct _foundation_memory_system
{
	//! Allocation
	memory_allocate_fn              allocate;

	//! Allocate-and-zero-out
	memory_allocate_zero_fn         allocate_zero;

	//! Reallocation
	memory_reallocate_fn            reallocate;

	//! Deallocation
	memory_deallocate_fn            deallocate;

	//! Initialization of memory system
	system_initialize_fn            initialize;

	//! Shutdown of memory system
	system_shutdown_fn              shutdown;
} memory_system_t;

//! Memory tracking callbacks
typedef struct _foundation_memory_tracker
{
	//! Track a memory allocation
	memory_track_fn                 track;

	//! Untrack a memory allocation
	memory_untrack_fn               untrack;

	//! Initialize tracker
	system_initialize_fn            initialize;

	//! Shutdown tracker
	system_shutdown_fn              shutdown;
} memory_tracker_t;

//! Version identifier
typedef union _foundation_version
{
	//! Compound version identifier
	uint128_t                       version;

	//! Version numbers separated into sections, "major.minor.revision-revision (control)"
	struct
	{
		//! Major version
		uint16_t                    major;

		//! Minor version
		uint16_t                    minor;

		//! Revision number
		uint32_t                    revision;

		//! Build number
		uint32_t                    build;

		//! Source control version/revision/identifier
		uint32_t                    control;
	}                               sub;
} version_t;

//! Application declaration
typedef struct _foundation_application
{
	//! Long descriptive name
	const char*                     name;

	//! Short name, should only contain [a-z][A-Z][-_.]
	const char*                     short_name;

	//! Config directory name
	const char*                     config_dir;

	//! Version declaration
	version_t                       version;

	//! Crash dump callback function
	crash_dump_callback_fn          dump_callback;

	//! Application flags (\see application_flag_t)
	unsigned int                    flags;

	//! Instance UUID, generated by the foundation library on foundation initialization
	uuid_t                          instance;
} application_t;

//! Data for a frame in the error context stack
typedef struct _foundation_error_frame
{
	//! Error description
	const char*                     name;

	//! Data associated with error
	const char*                     data;
} error_frame_t;

//! Error context stack
typedef struct _foundation_error_context
{
	//! Error context stack
	error_frame_t                   frame[BUILD_SIZE_ERROR_CONTEXT_DEPTH];

	//! Current depth of error context stack
	int                             depth;
} error_context_t;

//! Memory context stack
typedef struct _foundation_memory_context
{
	//! Memory context stack
	uint64_t                        context[BUILD_SIZE_MEMORY_CONTEXT_DEPTH];

	//! Current depth of memory context stack
	int                             depth;
} memory_context_t;

//! Object base structure. If changing base object layout, change objectmap_lookup()
#define FOUNDATION_DECLARE_OBJECT               \
	atomic32_t                      ref;        \
	uint32_t                        flags;      \
	object_t                        id

//! Object base structure. All object-based designs must have this layout at the start of the structure
typedef struct _foundation_object_base
{
	/*! \var ref
        Object reference count */
	/*! \var flags
	    Object flags */
	/*! \var id
	    Object ID (self) */
	FOUNDATION_DECLARE_OBJECT;
} object_base_t;

//! Object map
typedef struct ALIGN(16) _foundation_objectmap
{
	//! Current first free slot
	atomic64_t                      free;

	//! Number of slots in map
	uint64_t                        size;

	//! Counter for next available ID
	atomic64_t                      id;

	//! Number of bits needed for slot index
	uint64_t                        size_bits;

	//! Maximum ID (depending on how many bits are used by size)
	uint64_t                        id_max;

	//! Bitmask for slot index
	uint64_t                        mask_index;

	//! Bitmask for ID
	uint64_t                        mask_id;

	//! Slot array
	void*                           map[];
} objectmap_t;

//! Event base structure
#define FOUNDATION_DECLARE_EVENT       \
	uint16_t              id;          \
	uint16_t              flags;       \
	uint16_t              serial;      \
	uint16_t              size;        \
	object_t              object

typedef struct _foundation_event
{
	/*! \var id
        Event ID */
	/*! \var flags
	    Event flags */
	/*! \var serial
	    Event serial number */
	/*! \var size
        Size of event data payload  */
	/*! \var object
        Object associated with event */
	FOUNDATION_DECLARE_EVENT;

	//! Event data payload
	char                  payload[];
} event_t;

//! Semaphore (actual type depending on platform)
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
#  if FOUNDATION_ARCH_X86_64
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

//! MD5 state
typedef struct _foundation_md5              md5_t;

//! Base opaque stream
typedef struct _foundation_stream           stream_t;

//! Mutex
typedef struct _foundation_mutex            mutex_t;

//! Process
typedef struct _foundation_process          process_t;

//! Event block holding a number of events f
typedef struct _foundation_event_block      event_block_t;

//! Event strem
typedef struct _foundation_event_stream     event_stream_t;

//! Ringbuffer
typedef struct _foundation_ringbuffer       ringbuffer_t;

//! Blowfish state
typedef struct _foundation_blowfish         blowfish_t;

//! Radix sorter
typedef struct _foundation_radixsort        radixsort_t;

//! Hashmap
typedef struct _foundation_hashmap          hashmap_t;

//! Hash table (32bit data)
typedef struct _foundation_hashtable32      hashtable32_t;

//! Hash table (64bit data)
typedef struct _foundation_hashtable64      hashtable64_t;


// COMPLEX TYPES

//! Bit buffer for bit based I/O to a memory buffer or stream
typedef struct _foundation_bitbuffer
{
	//! Memory buffer
	uint8_t*            buffer;

	//! End of buffer indicator
	uint8_t*            end;

	//! Stream
	stream_t*           stream;

	//! Swap flag for compatibility between machines with different endian arch
	bool                swap;

	//! Pending data to be read
	unsigned int        pending_read;

	//! Pending data to be written
	unsigned int        pending_write;

	//! Current read offset in bits into pending data
	unsigned int        offset_read;

	//! Current write offset in bits into pending data
	unsigned int        offset_write;

	//! Number of read bits
	unsigned int        count_read;

	//! Number of written bits
	unsigned int        count_write;
} bitbuffer_t;


// UTILITY FUNCTIONS

/*! Pack version definition into a 128-bit integer
    \param major          Major version
    \param minor          Minor version
    \param revision       Revision number
	\param build          Build number
	\param control        Source control version/revision/identifier
    \return               Packed version data */
static FORCEINLINE CONSTCALL version_t      version_make( unsigned int major, unsigned int minor, unsigned int revision, unsigned int build, unsigned int control ) { version_t v; v.sub.major = (uint16_t)major; v.sub.minor = (uint16_t)minor; v.sub.revision = revision, v.sub.build = build; v.sub.control = control; return v; }
