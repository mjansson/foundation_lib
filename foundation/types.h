/* types.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file types.h
\brief Foundation data types

Foundation data types, enumerations and typedefs. Provides platform abstractions
of system specific data types and provides the base language used in all libraries built
on this foundation library. */

#include <foundation/platform.h>
#include <foundation/build.h>

#if defined(FOUNDATION_PLATFORM_DOXYGEN)
#define FOUNDATION_ALIGNED_STRUCT(name, alignment) struct name
#endif

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

// PRIMITIVE TYPES

/*! Error severity level. The higher the value, the more severe the error. Error level 0
(ERRORLEVEL_NONE) is used to indicate no error
\internal Do not change order! \endinternal */
typedef enum {
	/*! No error */
	ERRORLEVEL_NONE = 0,
	/*! Debug level, usually ignored in anything except debug builds.
	Execution will continue as expected. */
	ERRORLEVEL_DEBUG,
	/*! Information level, contains generally useful information.
	Execution will continue as expected. */
	ERRORLEVEL_INFO,
	/*! Warning level, contains important information.
	Operation failed, but execution can continue. */
	ERRORLEVEL_WARNING,
	/*! Error level, contains vital information.
	Operation failed and execution might be affected. */
	ERRORLEVEL_ERROR,
	/*! Panic level, contains vital information.
	Operation failed and execution cannot continue. */
	ERRORLEVEL_PANIC
} error_level_t;

/*! Error identifiers. Error 0 (ERROR_NONE) is used to indicate no error.
\internal Do not change order, only append! \endinternal */
typedef enum {
	/*! No error */
	ERROR_NONE = 0,
	/*! An invalid value was passed to the function */
	ERROR_INVALID_VALUE,
	/*! The function is unsupported on the current system */
	ERROR_UNSUPPORTED,
	/*! The function is not yet implemented */
	ERROR_NOT_IMPLEMENTED,
	/*! The function could not allocate the needed memory and/or resources */
	ERROR_OUT_OF_MEMORY,
	/*! A memory leak was detected */
	ERROR_MEMORY_LEAK,
	/*! Memory alignment check failed */
	ERROR_MEMORY_ALIGNMENT,
	/*! Internal failed, unspecified. The function encountered a state it did not
	expect or support */
	ERROR_INTERNAL_FAILURE,
	/*! The function call was not allowed */
	ERROR_ACCESS_DENIED,
	/*! An exception was thrown */
	ERROR_EXCEPTION,
	/*! A system call failed */
	ERROR_SYSTEM_CALL_FAIL,
	/*! The function encountered an unsupported data type for the requested operation */
	ERROR_UNKNOWN_TYPE,
	/*! The function could not resolve the resource to use in the requested operation */
	ERROR_UNKNOWN_RESOURCE,
	/*! The function is deprecated and should not be used */
	ERROR_DEPRECATED,
	/*! An assert triggered */
	ERROR_ASSERT,
	/*! A script generated an error */
	ERROR_SCRIPT,
	/*! Data was corrupted */
	ERROR_CORRUPT_DATA,
	/*! Network or server side error */
	ERROR_NETWORK,
	/*! Marker, last reserved internal error identifier */
	ERROR_LAST_BUILTIN = 0x0fff
} error_t;

/*! Warning classes. Indicate which type/group of warning message that was generated
\internal Do not change order, only append! \endinternal */
typedef enum {
	/*! Performance warning */
	WARNING_PERFORMANCE = 0,
	/*! Function is deprecated */
	WARNING_DEPRECATED,
	/*! An invalid value was passed to the function */
	WARNING_INVALID_VALUE,
	/*! Memory issues (running low, leaks, ...) */
	WARNING_MEMORY,
	/*! Unsupported function */
	WARNING_UNSUPPORTED,
	/*! General warning, function encountered a suspicious state */
	WARNING_SUSPICIOUS,
	/*! System call failed */
	WARNING_SYSTEM_CALL_FAIL,
	/*! Potential deadlock encountered */
	WARNING_DEADLOCK,
	/*! Script generated warning */
	WARNING_SCRIPT,
	/*! Resource was missing or corrupt */
	WARNING_RESOURCE,
	/*! Network issue */
	WARNING_NETWORK,
	/*! Marker, last reserved inernal warning identifier */
	WARNING_LAST_BUILTIN = 0x0fff
} warning_t;

/*! Platform identifiers. For compile-time platform selection, use the
FOUNDATION_PLATFORM_[...] preprocessor macros
\internal Do not change order, only append! \endinternal */
typedef enum {
	/*! Windows */
	PLATFORM_WINDOWS = 0,
	/*! Linux */
	PLATFORM_LINUX,
	/*! macOS */
	PLATFORM_MACOS,
	/*! iOS */
	PLATFORM_IOS,
	/*! Android */
	PLATFORM_ANDROID,
	/*! Raspberry Pi (linux flavour) */
	PLATFORM_RASPBERRYPI,
	/*! BSD */
	PLATFORM_BSD,
	/*! Tizen */
	PLATFORM_TIZEN,
	/*! Invalid platform identifier */
	PLATFORM_INVALID
} platform_t;

/*! Architecture identifiers for all architectures the engine supports. For compile-time
selection of architecture, use the <code>FOUNDATION_ARCH_[...]</code> preprocessor macros.
\internal Do not change order, only append! \endinternal */
typedef enum {
	/*! x86 (Classic 32-bit x86 compatible CPUs) */
	ARCHITECTURE_X86 = 0,
	/*! x86-64 (x86 with 64-bit extensions) */
	ARCHITECTURE_X86_64,
	/*! PowerPC 32-bit */
	ARCHITECTURE_PPC,
	/*! PowerPC 64-bit */
	ARCHITECTURE_PPC_64,
	/*! ARM 5 */
	ARCHITECTURE_ARM5,
	/*! ARM 6 */
	ARCHITECTURE_ARM6,
	/*! ARM 7 */
	ARCHITECTURE_ARM7,
	/*! ARM 8 */
	ARCHITECTURE_ARM8,
	/*! ARM 8 64-bit */
	ARCHITECTURE_ARM8_64,
	/*! MIPS */
	ARCHITECTURE_MIPS,
	/*! MIPS 64-bit */
	ARCHITECTURE_MIPS_64,
	/*! Generic/unknown */
	ARCHITECTURE_GENERIC
} architecture_t;

/*! Machine byte order identifiers */
typedef enum {
	/*! Little endian */
	BYTEORDER_LITTLEENDIAN = 0,
	/*! Big endian */
	BYTEORDER_BIGENDIAN
} byteorder_t;

/*! Stream type identifiers. Only lists built-in types, application specific types
can be added below the STREAMTYPE_LAST_RESERVED value.
\internal Do not change order, only append! \endinternal */
typedef enum {
	/*! Invalid identifier */
	STREAMTYPE_INVALID = 0,
	/*! Memory buffer stream */
	STREAMTYPE_MEMORY,
	/*! File system stream */
	STREAMTYPE_FILE,
	/*! Network socket stream */
	STREAMTYPE_SOCKET,
	/*! Memory ring buffer stream */
	STREAMTYPE_RINGBUFFER,
	/*! Asset stream (Android only) */
	STREAMTYPE_ASSET,
	/*! Pipe stream */
	STREAMTYPE_PIPE,
	/*! Standard stream (stdin, stderr, stdout) */
	STREAMTYPE_STDSTREAM,
	/*! Custom unknown stream type */
	STREAMTYPE_CUSTOM,
	/*! Last reserved built-in stream type, not a valid type */
	STREAMTYPE_LAST_RESERVED = 0x0FFF
} stream_type_t;

/*! Stream seek directions */
typedef enum {
	/*! Seek from start of stream */
	STREAM_SEEK_BEGIN = 0,
	/*! Seek from current position of stream */
	STREAM_SEEK_CURRENT,
	/*! Seek from end of stream */
	STREAM_SEEK_END
} stream_seek_mode_t;

/*! Thread priority */
typedef enum {
	/*! Lowest possible priority */
	THREAD_PRIORITY_LOW = 0,
	/*! Unimportant priority */
	THREAD_PRIORITY_BELOWNORMAL,
	/*! Normal priority */
	THREAD_PRIORITY_NORMAL,
	/*! Important priority */
	THREAD_PRIORITY_ABOVENORMAL,
	/*! Highest priority */
	THREAD_PRIORITY_HIGHEST,
	/*! Absolutely critical */
	THREAD_PRIORITY_TIMECRITICAL
} thread_priority_t;

/*! Foundation library level event identifiers. These event identifiers are only
valid in conjunction with foundation event streams. Other event streams will use
their own event identifiers with the same value, and event streams should be treated
as separate "namespaces" for event identifiers. */
typedef enum {
	/*! Not an event, used a sentinel to denote no event */
	FOUNDATIONEVENT_NOEVENT = 0,
	/*! Application has been asked to start */
	FOUNDATIONEVENT_START,
	/*! Application has been asked to terminate */
	FOUNDATIONEVENT_TERMINATE,
	/*! Application has been asked to pause */
	FOUNDATIONEVENT_PAUSE,
	/*! Application has been asked to resume */
	FOUNDATIONEVENT_RESUME,
	/*! Application gained focus */
	FOUNDATIONEVENT_FOCUS_GAIN,
	/*! Application lost focus */
	FOUNDATIONEVENT_FOCUS_LOST,
	/*! File was created */
	FOUNDATIONEVENT_FILE_CREATED,
	/*! File was deleted */
	FOUNDATIONEVENT_FILE_DELETED,
	/*! File was modified */
	FOUNDATIONEVENT_FILE_MODIFIED,
	/*! Low memory warning */
	FOUNDATIONEVENT_LOW_MEMORY_WARNING,
	/*! Device orientation changed */
	FOUNDATIONEVENT_DEVICE_ORIENTATION,
	/*! Last reserved event id */
	FOUNDATIONEVENT_LAST_RESERVED = 32
} foundation_event_id;

/*! Block cipher mode of operation, see
http://en.wikipedia.org/wiki/Block_cipher_mode_of_operation */
typedef enum {
	/*! Electronic codebook */
	BLOCKCIPHER_ECB = 0,
	/*! Cipher-block chaining */
	BLOCKCIPHER_CBC,
	/*! Cipher feedback */
	BLOCKCIPHER_CFB,
	/*! Output feedback */
	BLOCKCIPHER_OFB
} blockcipher_mode_t;

/*! Radix sort data types */
typedef enum {
	/*! 32-bit signed integer */
	RADIXSORT_INT32 = 0,
	/*! 32-bit unsigned integer */
	RADIXSORT_UINT32,
	/*! 64-bit signed integer */
	RADIXSORT_INT64,
	/*! 64-bit unsigned integer */
	RADIXSORT_UINT64,
	/*! 32-bit floating point */
	RADIXSORT_FLOAT32,
	/*! 64-bit floating point */
	RADIXSORT_FLOAT64,
	/*! Custom opaque data type */
	RADIXSORT_CUSTOM,
} radixsort_data_t;

/*! Radix sort index types */
typedef enum {
	/*! 16-bit indices */
	RADIXSORT_INDEX16 = 2,
	/*! 32-bit indices */
	RADIXSORT_INDEX32 = 4
} radixsort_indextype_t;

/*! Device orientation */
typedef enum {
	/*! Orientation not known or not supported */
	DEVICEORIENTATION_UNKNOWN = 0,
	/*! Device oriented vertically, bottom of device down */
	DEVICEORIENTATION_PORTRAIT,
	/*! Device oriented vertically, bottom of device up */
	DEVICEORIENTATION_PORTRAIT_FLIPPED,
	/*! Device oriented horizontally, bottom of device to the right,
	rotated counter-clockwise from portrait mode */
	DEVICEORIENTATION_LANDSCAPE_CCW,
	/*! Device oriented horizontally, bottom of device to the left,
	rotated clockwise from portrait mode */
	DEVICEORIENTATION_LANDSCAPE_CW,
	/*! Device oriented flat, face up */
	DEVICEORIENTATION_FACEUP,
	/*! Device oriented flat, face down */
	DEVICEORIENTATION_FACEDOWN
} device_orientation_t;

/*! JSON token type */
typedef enum {
	/*! Invalid type */
	JSON_UNDEFINED = 0,
	/*! Object */
	JSON_OBJECT,
	/*! Array */
	JSON_ARRAY,
	/*! String */
	JSON_STRING,
	/*! Primitive */
	JSON_PRIMITIVE
} json_type_t;

/*! Memory hint, memory allocationis persistent (retained when function returns) */
#define MEMORY_PERSISTENT 0
/*! Memory hint, memory is temporary (extremely short lived and generally freed
before function returns or scope ends) */
#define MEMORY_TEMPORARY 1U
/*! Memory hint, memory allocation is local to the calling thread */
#define MEMORY_THREAD (1U << 1)
/*! Memory flag, memory should be initialized to zero during allocation */
#define MEMORY_ZERO_INITIALIZED (1U << 3)
/*! Memory flag, memory content does not have to be preserved during reallocation */
#define MEMORY_NO_PRESERVE (1U << 4)

/*! Event flag, event is delayed and will be delivered at a later timestamp */
#define EVENTFLAG_DELAY 1U

/*! Application flag, application is a command line utility and should not have
a normal windowing system interaction loop */
#define APPLICATION_UTILITY (1U << 0)
/*! Application flag, application is a daemon/service */
#define APPLICATION_DAEMON (1U << 1)
/*! Application flag, application is unattended */
#define APPLICATION_UNATTENDED (1U << 2)

/*! Stream mode/flag, stream is readable */
#define STREAM_IN 1U
/*! Stream mode/flag, stream is writable */
#define STREAM_OUT (1U << 1)
/*! Stream flag, stream is truncated on open if writable */
#define STREAM_TRUNCATE (1U << 2)
/*! Stream flag, stream target is created if it does not previously exist */
#define STREAM_CREATE (1U << 3)
/*! Stream flag, stream position is set to end on open */
#define STREAM_ATEND (1U << 4)
/*! Stream flag/mode, stream I/O is binary (I/O is in ascii if flag not set) */
#define STREAM_BINARY (1U << 5)
/*! Stream flag, stream is synchronized on each write */
#define STREAM_SYNC (1U << 6)
/*! Stream flag, create exclusively, fail if file already exists */
#define STREAM_CREATE_EXCLUSIVE (STREAM_CREATE | (1U << 7))

/*! Process flag, spawn method will block until process ends and then return
process exit code */
#define PROCESS_ATTACHED 0
/*! Process flag, spawn method will immediately return with code 0 unless
an error occurs during spawning */
#define PROCESS_DETACHED (1U << 0)
/*! Process flag, create a console window for process */
#define PROCESS_CONSOLE (1U << 1)
/*! Process flag, create stdout/stdin pipes to process */
#define PROCESS_STDSTREAMS (1U << 2)
/*! Process flag, use ShellExecute instead of CreateProcess (Windows platform only) */
#define PROCESS_WINDOWS_USE_SHELLEXECUTE (1U << 3)
/*! Process flag, use LSOpenApplication instead of fork/execve (macOS platform only) */
#define PROCESS_MACOS_USE_OPENAPPLICATION (1U << 4)

/*! Process exit code, returned when given invalid arguments */
#define PROCESS_INVALID_ARGS 0x7FFFFFF0
/*! Process exit code, returned when process was terminated by signal */
#define PROCESS_TERMINATED_SIGNAL 0x7FFFFFF1
/*! Process exit code, returned when process wait was interrupted */
#define PROCESS_WAIT_INTERRUPTED 0x7FFFFFF2
/*! Process exit code, returned when process wait failed for unknown reasons */
#define PROCESS_WAIT_FAILED 0x7FFFFFF3
/*! Process exit code, returned when spawn system calls failed */
#define PROCESS_SYSTEM_CALL_FAILED 0x7FFFFFF4
/*! Process exit code, returned when detached process is still running */
#define PROCESS_STILL_ACTIVE 0x7FFFFFFF
/*! Process exit code, generic failure */
#define PROCESS_EXIT_FAILURE EXIT_FAILURE
/*! Process exit code, generic success */
#define PROCESS_EXIT_SUCCESS EXIT_SUCCESS

/*! Virtual array flag for normal memory allocated storage */
#define VIRTUALARRAY_MEMORY_ALLOCATED 1

#if FOUNDATION_PLATFORM_WINDOWS
#if FOUNDATION_ARCH_X86
typedef int ssize_t;
#else
typedef int64_t ssize_t;
#endif
#endif

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

/*! Hash value */
typedef uint64_t hash_t;
/*! Tick type used for absolute time measurements or timestamps */
typedef int64_t tick_t;
/*! Deltatime type used for floating point time differences */
typedef real deltatime_t;
/*! Object handle used for identifying reference counted objects */
typedef uint32_t object_t;
/*! UUID, 128-bit unique identifier */
typedef uint128_t uuid_t;

/*! Used to bit manipulate 32-bit floating point values in a alias safe way */
typedef union float32_cast_t {
	/*! Signed integer representation */
	int32_t ival;
	/*! Unsigned integer representation */
	uint32_t uival;
	/*! Floating point representation */
	float32_t fval;
} float32_cast_t;

/*! Used to bit manipulate 64-bit floating point values in a alias safe way */
typedef union float64_cast_t {
	/*! Signed integer representation */
	int64_t ival;
	/*! Unsigned integer representation */
	uint64_t uival;
	/*! Floating point representation */
	float64_t fval;
} float64_cast_t;

#if FOUNDATION_SIZE_REAL == 8
typedef union float64_cast_t real_cast_t;
#else
typedef union float32_cast_t real_cast_t;
#endif

/*! String */
typedef struct string_t string_t;
/*! Constant immutable string */
typedef struct string_const_t string_const_t;
/*! Application declaration and configuration */
typedef struct application_t application_t;
/*! Beacon for waiting */
typedef struct beacon_t beacon_t;
/*! Bit buffer instance */
typedef struct bitbuffer_t bitbuffer_t;
/*! Blowfish cipher instance */
typedef struct blowfish_t blowfish_t;
/*! Bucketized array for POD types */
typedef struct bucketarray_t bucketarray_t;
/*! Virtualized array for POD types */
typedef struct virtualarray_t virtualarray_t;
/*! Error frame holding debug data for an entry in the frame stack in the error context */
typedef struct error_frame_t error_frame_t;
/*! Error context holding error frame stack for a thread */
typedef struct error_context_t error_context_t;
/*! Event base structure */
typedef struct event_t event_t;
/*! Event block holding a chunk of events from a single stream */
typedef struct event_block_t event_block_t;
/*! Event stream instance producing event blocks of events */
typedef struct event_stream_t event_stream_t;
/*! Stats for a filesystem node */
typedef struct fs_stat_t fs_stat_t;
/*! Payload for a file system event */
typedef struct fs_event_payload_t fs_event_payload_t;
/*! Node in a hash map */
typedef struct hashmap_node_t hashmap_node_t;
/*! Hash map mapping hash value keys to pointer values */
typedef struct hashmap_t hashmap_t;
/*! Hash map of fixed size */
typedef struct hashmap_fixed_t hashmap_fixed_t;
/*! Node in a uuid hash map */
typedef struct uuidmap_node_t uuidmap_node_t;
/*! Hash map mapping uuid value keys to pointer values */
typedef struct uuidmap_t uuidmap_t;
/*! Hash map of fixed size for uuids */
typedef struct uuidmap_fixed_t uuidmap_fixed_t;
/*! Entry in a 32-bit hash table */
typedef struct hashtable32_entry_t hashtable32_entry_t;
/*! Entry in a 64-bit hash table */
typedef struct hashtable64_entry_t hashtable64_entry_t;
/*! Hash table mapping 32-bit keys to 32-bit values */
typedef struct hashtable32_t hashtable32_t;
/*! Hash table mapping 64-bit keys to 64-bit values */
typedef struct hashtable64_t hashtable64_t;
/*! MD5 control block */
typedef struct md5_t md5_t;
/*! Memory context holding the allocation context stack */
typedef struct memory_context_t memory_context_t;
/*! Memory system declaration */
typedef struct memory_system_t memory_system_t;
/*! Memory tracker declaration */
typedef struct memory_tracker_t memory_tracker_t;
/*! Memory statistics */
typedef struct memory_statistics_t memory_statistics_t;
/*! Platform specific mutex representation, opaque data type */
typedef struct mutex_t mutex_t;
/*! Object map mapping object handles to object instance pointers */
typedef struct objectmap_t objectmap_t;
/*! Object map entry mapping object handles to object instance pointers */
typedef struct objectmap_entry_t objectmap_entry_t;
/*! Child process control block */
typedef struct process_t process_t;
/*! Radix sorter control block */
typedef struct radixsort_t radixsort_t;
/*! Compiled regex */
typedef struct regex_t regex_t;
/*! Memory ring buffer */
typedef struct ringbuffer_t ringbuffer_t;
/*! SHA-256 control block */
typedef struct sha256_t sha256_t;
/*! SHA-512 control block */
typedef struct sha512_t sha512_t;
/*! Base stream type all stream types are based on */
typedef struct stream_t stream_t;
/*! Memory buffer stream */
typedef struct stream_buffer_t stream_buffer_t;
/*! Pipe stream */
typedef struct stream_pipe_t stream_pipe_t;
/*! Ring buffer stream */
typedef struct stream_ringbuffer_t stream_ringbuffer_t;
/*! Vtable for streams providing stream type specific implementations
of stream operations */
typedef struct stream_vtable_t stream_vtable_t;
/*! Thread */
typedef struct thread_t thread_t;
/*! JSON token */
typedef struct json_token_t json_token_t;
/*! Version declaration */
typedef union version_t version_t;
/*! Library configuration block controlling limits, functionality and memory
usage of the library */
typedef struct foundation_config_t foundation_config_t;

#if FOUNDATION_PLATFORM_WINDOWS
/*! Platform specific representation of a semaphore */
typedef void* semaphore_t;
#elif FOUNDATION_PLATFORM_APPLE
#include <sys/semaphore.h>
typedef struct semaphore_t semaphore_t;
#elif FOUNDATION_PLATFORM_BSD || FOUNDATION_PLATFORM_POSIX
#include <semaphore.h>
typedef struct semaphore_t semaphore_t;
#endif

/*! Error handler which is passed the error level and reported error. It should return
an implementation specific code which is then returned from the call to error_report
\param level Error level
\param error Error code
\return Implementation specific code which is passed back as return from error_report */
typedef int (*error_handler_fn)(error_level_t level, error_t error);

/*! Assert handler which is passed assert data and should do impementation specific
processing and return a code indicating if execution can continue or need to be aborted.
\param context Error context
\param condition String expressing the condition that failed
\param cond_length Length of condition string
\param file Source file triggering the assert
\param file_length Length of source file string
\param line Source line triggering the assert
\param msg Assert message, if any
\param msg_length Length of assert message
\return 1 if assert was not handled and execution should break, 0 if assert handled and
        execution can continue */
typedef int (*assert_handler_fn)(hash_t context, const char* condition, size_t cond_length, const char* file,
                                 size_t file_length, unsigned int line, const char* msg, size_t msg_length);

/*! Log output handler. Called after each log message processed and output by
the log functions.
\param context Log context
\param severity Log severity
\param msg Log message
\param length Length of message */
typedef void (*log_handler_fn)(hash_t context, error_level_t severity, const char* msg, size_t length);

/* JSON parsing handler
\param path Path of data being parsed
\param path_size Length of path string
\param buffer Data buffer
\param size Size of data buffer
\param tokens Tokens array
\param token_count Number of tokens */
typedef void (*json_handler_fn)(const char* path, size_t path_size, const char* buffer, size_t size,
                                const json_token_t* tokens, size_t token_count);

/*! Memory tracker dump handler
\param addr Address of allocated region
\param size Size of allocation
\param trace Stack trace of allocation (if any, otherwise null)
\param depth Depth of stack trace
\return 0 to continue dumping allocations, non-zero to stop dump */
typedef int (*memory_tracker_handler_fn)(const void* addr, size_t size, void* const* trace, size_t depth);

/*! Subsystem initialization function prototype. Return value should be the success
state of initialization
\return 0 on success, <0 if failure (errors should be reported through log_error
        or error_report) */
typedef int (*system_initialize_fn)(void);

/*! Subsystem finalization function prototype. Will be called for each successfully
initialized subsystem on global finalization */
typedef void (*system_finalize_fn)(void);

/*! Memory system allocation function prototype. Implementation of a memory system must
provide an implementation with this prototype for allocating memory
\param context Memory context
\param size Requested size
\param align Aligmnent requirement
\param hint Memory hints
\return Pointer to allocated memory block if successful, 0 if error */
typedef void* (*memory_allocate_fn)(hash_t context, size_t size, unsigned int align, unsigned int hint);

/*! Memory system reallocation function prototype. Implementation of a memory system must
provide an implementation with this prototype for reallocating memory
\param p Pointer to previous memory block
\param size Requested size
\param align Aligmnent requirement
\param oldsize Size of previous memory block
\param hint Memory hints
\return Pointer to allocated memory block if successful, 0 if error */
typedef void* (*memory_reallocate_fn)(void* p, size_t size, unsigned int align, size_t oldsize, unsigned int hint);

/*! Memory system deallocation function prototype. Implementation of a memory system must
provide an implementation with this prototype for deallocating memory
\param p Pointer to memory block */
typedef void (*memory_deallocate_fn)(void* p);

/*! Memory system block size query function prototype. Implementation of a memory system must
provide an implementation with this prototype for querying the size of a memory block
\param p Pointer to memory block
\return Size of memory block in bytes */
typedef size_t (*memory_usable_size_fn)(const void* p);

/*! Memory system integrity verification function prototype. Implementation of a memory system could
provide an implementation with this prototype for verifying the integrity of the memory block
\param p Pointer to a memory block
\return true if integrity checks passed, false if compromised */
typedef bool (*memory_verify_fn)(const void* p);

/*! Memory thread initialization function prototype. Implementation of a memory system can
optionally provide an implementation with this prototype for initialization at thread start */
typedef void (*memory_thread_initialize_fn)(void);

/*! Memory thread finalization function prototype. Implementation of a memory system can
optionally provide an implementation with this prototype for finalization at thread exit */
typedef void (*memory_thread_finalize_fn)(void);

/*! Memory tracker tracking function prototype. Implementation of a memory tracker must
provide an implementation with this prototype for tracking memory allocations
\param p Pointer to allocated memory block
\param size Size of memory block */
typedef void (*memory_track_fn)(void* p, size_t size);

/*! Memory tracker untracking function prototype. Implementation of a memory tracker must
provide an implementation with this prototype for untracking memory allocations
\param p Pointer to deallocated memory block */
typedef void (*memory_untrack_fn)(void* p);

/*! Memory tracker statistics function prototype. Implementation of a memory tracker must
provide an implementation with this prototype for memory statistics
\return Memory statistics */
typedef memory_statistics_t (*memory_statistics_fn)(void);

/*! Memory tracker dump function prototype. Implementation of a memory tracker can
provide an implementation of this prototype.
\param handler Dump handler function */
typedef void (*memory_tracker_dump_fn)(memory_tracker_handler_fn handler);

/*! Callback function for writing profiling data to a stream
\param data Pointer to data block
\param size Size of data block */
typedef void (*profile_write_fn)(void* data, size_t size);

/*! Callback function for reading profiling data from a stream
\param data Pointer to data block
\param size Size of data block */
typedef void (*profile_read_fn)(void* data, size_t size);

/*! Thread entry point function prototype
\param arg Argument passed by caller when starting the thread
\return Implementation specific data which can be obtained through thread_result */
typedef void* (*thread_fn)(void* arg);

/*! Any function to be used in conjunction with the exception handling
in the library should have this prototype
\param arg Implementation specific argument passed to exception_try
\return Implementation specific return value which is forwarded as return value
        from exception_try (note that FOUNDATION_EXCEPTION_CAUGHT is reserved) */
typedef int (*exception_try_fn)(void* arg);

/*! Exception handler function prototype, used to notify that an exception occurred
and the process state was saved to a dump file
\param file Dump file path
\param length Length of file path */
typedef void (*exception_handler_fn)(const char* file, size_t length);

/*! Object deallocation function prototype, used to deallocate an object of a specific type
\param object Object pointer */
typedef void (*object_deallocate_fn)(void* object);

/*! Generic function to open a stream with the given path and mode
\param path Path, optionally including protocol
\param length Length of path
\param mode Open mode
\return Newly allocated stream, null if it could not be opened */
typedef stream_t* (*stream_open_fn)(const char* path, size_t length, unsigned int mode);

/*! Generic function to read data from a stream
\param stream Stream to read from
\param dst Destination buffer
\param size Number of bytes to read
\return Number of bytes actually read */
typedef size_t (*stream_read_fn)(stream_t* stream, void* dst, size_t size);

/*! Generic function to write data to a stream
\param stream Stream to write to
\param src Source buffer
\param size Number of bytes to write
\return Number of bytes actually written */
typedef size_t (*stream_write_fn)(stream_t* stream, const void* src, size_t size);

/*! Query if end of stream
\param stream Stream
\return true if stream at end, false if not */
typedef bool (*stream_eos_fn)(stream_t* stream);

/*! Flush stream output buffers
\param stream Stream */
typedef void (*stream_flush_fn)(stream_t* stream);

/*! Truncate stream size to the given size
\param stream Stream
\param size Size to truncate stream to */
typedef void (*stream_truncate_fn)(stream_t* stream, size_t size);

/*! Get stream size
\param stream Stream
\return Stream size, 0 if invalid stream or unknown (like a network stream) */
typedef size_t (*stream_size_fn)(stream_t* stream);

/*! Seek in the stream. Only available if stream is seekable and not a sequential stream like
a network stream.
\param stream Stream
\param offset Seek offset
\param mode Seek mode (see #stream_seek_mode_t) */
typedef void (*stream_seek_fn)(stream_t* stream, ssize_t offset, stream_seek_mode_t mode);

/*! Get current stream position
\param stream Stream
\return Current stream position, 0 if invalid stream or unknown */
typedef size_t (*stream_tell_fn)(stream_t* stream);

/*! Get timestamp when stream was last modified (written to or attributes/size changed)
\param stream Stream
\return Last modification timestamp, 0 if invalid stream or unknown */
typedef tick_t (*stream_lastmod_fn)(const stream_t* stream);

/*! Get stream digest. Only available if stream size is known and stream is seekable. Does
not modify the current stream position.
\param stream Stream
\return Digest of stream content, 0 if invalid stream or unknown */
typedef uint128_t (*stream_md5_fn)(stream_t* stream);

/*! Get stream digest. Only available if stream size is known and stream is seekable. Does
not modify the current stream position.
\param stream Stream
\return Digest of stream content, 0 if invalid stream or unknown */
typedef uint256_t (*stream_sha256_fn)(stream_t* stream);

/*! Get stream digest. Only available if stream size is known and stream is seekable. Does
not modify the current stream position.
\param stream Stream
\return Digest of stream content, 0 if invalid stream or unknown */
typedef uint512_t (*stream_sha512_fn)(stream_t* stream);

/*! If the stream has available data to be read from an external source (like a socket for
network streams), read and buffer the available data without blocking.
\param stream Stream */
typedef void (*stream_buffer_read_fn)(stream_t* stream);

/*! Query how much data can be read from the stream without blocking
\param stream Stream
\return Number of bytes that can be read without blocking */
typedef size_t (*stream_available_read_fn)(stream_t* stream);

/*! Finalize a stream object that was previously initialized with a call to a specific
stream initialization function and free any associated resources.
\param stream Stream */
typedef void (*stream_finalize_fn)(stream_t* stream);

/*! Clone stream, allocating a duplicate copy of the stream if the stream type supports it.
\param stream Stream
\return Clone of stream, 0 if not supported or invalid source stream */
typedef stream_t* (*stream_clone_fn)(stream_t* stream);

/*! Identifier returned from threads and exception_try after an exception
has been caught (and optionally a dump generated) */
#define FOUNDATION_EXCEPTION_CAUGHT 0x0badf00dL

// COMPLEX TYPES

/*! Library configuration with runtime controlled configuration parameters */
struct foundation_config_t {
	/*! Maximum number of concurrently allocated libraries. Zero for default (32) */
	size_t library_max;
	/*! Maximum number of file system monitors. Zero for default (16) */
	size_t fs_monitor_max;
	/*! Maximum depth of an error context. Zero for default (32) */
	size_t error_context_depth;
	/*! Maximum depth of a memory context. Zero for default (32) */
	size_t memory_context_depth;
	/*! Maximum depth of a stack trace. Zero for default (32) */
	size_t stacktrace_depth;
	/*! Maximum number of hash values stored in reverse lookup. Zero for default (0) */
	size_t hash_store_size;
	/*! Default size of an event block. Zero for default (8KiB) */
	size_t event_block_chunk;
	/*! Maximum size of an event block. Zero for default (512KiB) */
	size_t event_block_limit;
	/*! Default thread stack size. Zero for default (32KiB) */
	size_t thread_stack_size;
	/*! Number of random state blocks to preallocate on thread startup. Zero for default (0) */
	size_t random_state_prealloc;
};

/*! String tuple holding string data pointer and length. This is used to avoid extra calls
to determine string length in each API call */
struct string_t {
	/*! String buffer */
	char* str;
	/*! Length of string, not including any (optional) zero terminator */
	size_t length;
};

/*! Constant string tuple holding unmutable string data pointer and length.
\see string_t */
struct string_const_t {
	/*! String buffer */
	const char* str;
	/*! Length of string, not including any (optional) zero terminator */
	size_t length;
};

/*! MD5 state */
struct md5_t {
	/*! Flag indicating the md5 state has been initialized and ready for digestion of data */
	bool init;
	/*! Internal state during data digestion */
	uint32_t state[4];
	/*! Internal counters during data digestion */
	uint32_t count[2];
	/*! Internal buffer during data digestion */
	unsigned char buffer[64];
	/*! Internal digest data buffer */
	unsigned char digest[16];
};

/*! SHA-256 state */
struct sha256_t {
	/*! Flag indicating the sha state has been initialized and ready for digestion of data */
	bool init;
	/*! Number of bytes currently buffered */
	size_t current;
	/*! Number of bits digested in total */
	size_t length;
	/*! Internal state during data digestion */
	uint32_t state[8];
	/*! Buffered data */
	unsigned char buffer[64];
};

/*! SHA-512 state */
struct sha512_t {
	/*! Flag indicating the sha state has been initialized and ready for digestion of data */
	bool init;
	/*! Number of bytes currently buffered */
	size_t current;
	/*! Number of bits digested in total */
	size_t length;
	/*! Internal state during data digestion */
	uint64_t state[8];
	/*! Buffered data */
	unsigned char buffer[128];
};

/*! Memory management system declaration with function pointers for all memory system
entry points. */
struct memory_system_t {
	/*! Memory allocation */
	memory_allocate_fn allocate;
	/*! Memory reallocation */
	memory_reallocate_fn reallocate;
	/*! Memory deallocation */
	memory_deallocate_fn deallocate;
	/*! Memory block size query */
	memory_usable_size_fn usable_size;
	/*! Memory integrity verification */
	memory_verify_fn verify;
	/*! Thread initialization */
	memory_thread_initialize_fn thread_initialize;
	/*! Thread finalization */
	memory_thread_finalize_fn thread_finalize;
	/*! System initialization */
	system_initialize_fn initialize;
	/*! System finalization */
	system_finalize_fn finalize;
};

/*! Memory tracking system declarations with function pointers for all memory tracking
entry points */
struct memory_tracker_t {
	/*! Track a memory allocation */
	memory_track_fn track;
	/*! Untrack a memory allocation */
	memory_untrack_fn untrack;
	/*! Statistics */
	memory_statistics_fn statistics;
	/*! Dump */
	memory_tracker_dump_fn dump;
	/*! Initialize memory tracker */
	system_initialize_fn initialize;
	/*! Abort memory tracker */
	system_finalize_fn abort;
	/*! Finalize memory tracker */
	system_finalize_fn finalize;
};

/*! Memory statistics */
struct memory_statistics_t {
	/*! Number of allocations in total, running counter */
	uint64_t allocations_total;
	/*! Number fo allocations, current */
	uint64_t allocations_current;
	/*! Number of allocated bytes in total, running counter */
	uint64_t allocated_total;
	/*! Number of allocated bytes, current */
	uint64_t allocated_current;
};

/*! Version identifier expressed as an 128-bit integer with major, minor,
revision, build and control version number components */
union version_t {
	/*! Compound version identifier */
	uint128_t version;
	/*! Version numbers separated into sections, when serialized into strig form
	representing "major.minor.revision-revision (control)" */
	struct {
		/*! Major version */
		uint16_t major;
		/*! Minor version */
		uint16_t minor;
		/*! Revision number */
		uint32_t revision;
		/*! Build number */
		uint32_t build;
		/*! Source control version/revision/identifier */
		uint32_t control;
	} sub;
};

/*! Application declaration. String pointers passed in this struct must be
constant and valid for the entire lifetime and execution of the application. */
struct application_t {
	/*! Long descriptive name */
	string_const_t name;
	/*! Short name, must only contain [a-z][A-Z][-_.] */
	string_const_t short_name;
	/*! Optional company name, must only contain characters valid in a file name */
	string_const_t company;
	/*! Version declaration */
	version_t version;
	/*! Optional exception handler */
	exception_handler_fn exception_handler;
	/*! Application flags, see APPLICATION_[*] definitions */
	unsigned int flags;
	/*! Instance UUID, generated by the foundation library on foundation initialization */
	uuid_t instance;
};

#define BLOWFISH_SUBKEYS 18U
#define BLOWFISH_SBOXES 4U
#define BLOWFISH_SBOXENTRIES 256U
#define BLOWFISH_MAXKEY 56U

/*! State for a blowfish encryption block */
struct blowfish_t {
	/*! Blowfish P-array */
	uint32_t parray[BLOWFISH_SUBKEYS];
	/*! Blowfish S-boxes */
	uint32_t sboxes[BLOWFISH_SBOXES][BLOWFISH_SBOXENTRIES];
};

/*! Bit buffer for bit based I/O to a memory buffer or stream */
struct bitbuffer_t {
	/*! Memory buffer for buffer based I/O */
	uint8_t* buffer;
	/*! End of buffer indicator */
	uint8_t* end;
	/*! Stream for stream based I/O */
	stream_t* stream;
	/*! Swap flag for compatibility between machines with different endian architectures */
	bool swap;
	/*! Pending data to be read */
	uint32_t pending_read;
	/*! Pending data to be written */
	uint32_t pending_write;
	/*! Current read offset in bits into pending data */
	unsigned int offset_read;
	/*! Current write offset in bits into pending data */
	unsigned int offset_write;
	/*! Total number of read bits */
	uint64_t count_read;
	/*! Total number of written bits */
	uint64_t count_write;
};

/*! Bucketized array for POD types that are safe to memcpy/memmove */
struct bucketarray_t {
	//! Array of buckets
	void** bucket;
	//! Size of data type stored in array
	size_t element_size;
	//! Mask for intra-bucket index
	size_t bucket_mask;
	//! Bits to shift to get bucket index
	size_t bucket_shift;
	//! Number of currently allocated buckets
	size_t bucket_count;
	//! Number of elements stored in array
	size_t count;
};

/*! Virtualized array for POD types that are safe to memcpy/memmove */
struct virtualarray_t {
	//! Current number of elements stored
	size_t count;
	//! Capacity of array in number of elements
	size_t capacity;
	//! Flags
	uint flags;
	//! Element size
	uint element_size;
	//! Storage
	void* storage;
};

/*! Data for a frame in the error context stack */
struct error_frame_t {
	/*! Frame description */
	string_const_t name;
	/*! Data associated with frame */
	string_const_t data;
};

/*! Error context stack */
struct error_context_t {
	/*! Current depth of error context stack */
	unsigned int depth;
	/*! Error context stack */
	error_frame_t frame[FOUNDATION_FLEXIBLE_ARRAY];
};

/*! Declares the base event data layout. Event structures should use the macro as first
declaration in an event struct to place the base data in the correct place:
<code>typedef struct
{
  FOUNDATION_DECLARE_EVENT;
  int some_other_data;
  //[...]
} my_event_t;</code> */
#define FOUNDATION_DECLARE_EVENT \
	uint16_t id;                 \
	uint16_t flags;              \
	uint16_t serial;             \
	uint16_t size;               \
	object_t object

/*! Event base structure. All event structures must have this layout at the start of
the structure. See #FOUNDATION_DECLARE_EVENT for a macro to declare the base layout
in a structure. */
struct event_t {
	FOUNDATION_DECLARE_EVENT;
	/*!
	\var event_t::id
	Event ID

	\var event_t::flags
	Event flags

	\var event_t::serial
	Event serial number

	\var event_t::size
	Size of event data payload

	\var event_t::object
	Object associated with event
	*/
	/*! Event data payload */
	size_t payload[FOUNDATION_FLEXIBLE_ARRAY];
};

/*! Event block holding a number of events for processing. Block can be of a maximum
size specified in the #foundation_config_t at library initialization */
struct event_block_t {
	/*! Number of events pending */
	size_t used;
	/*! Capacity of event store buffer */
	size_t capacity;
	/*! Event stream owning this event block */
	event_stream_t* stream;
	/*! Memory buffer holding event data */
	event_t* events;
	/*! Fired state */
	bool fired;
};

/*! Event stream from a single module. Event streams produce event blocks for processing */
FOUNDATION_ALIGNED_STRUCT(event_stream_t, 16) {
	/*! Write block index */
	atomic32_t write;
	/*! Read block index */
	int32_t read;
	/*! Event blocks, double buffered for concurrent read/write access */
	event_block_t block[2];
	/*! Optional beacon */
	beacon_t* beacon;
};

/*! Stat for filesystem node */
struct fs_stat_t {
	/*! Size of file */
	uint64_t size;
	/*! Last modified timestamp */
	uint64_t last_modified;
	/*! Protect mode */
	uint32_t mode;
	/*! Exists */
	bool is_valid;
	/*! File flag */
	bool is_file;
	/*! Directory flag */
	bool is_directory;
};

/*! Payload layout for a file system event */
struct fs_event_payload_t {
	/*! Length of path string */
	size_t length;
	/*! Path string */
	const char str[FOUNDATION_FLEXIBLE_ARRAY];
};

/*! Single node in a hash map, mapping a single key to a single data value (pointer). */
struct hashmap_node_t {
	/*! Key for the hash map node */
	hash_t key;
	/*! Value for the hash map node */
	void* value;
};

/*! Declare an inlined hashmap of given size */
#define FOUNDATION_DECLARE_HASHMAP(size) \
	size_t bucket_count;                 \
	size_t node_count;                   \
	hashmap_node_t* bucket[size]

/*! Hash map container, mapping hash values to data pointers */
struct hashmap_t {
	/*!
	\var bucket_count
	Number of buckets in the hash map

	\var node_count
	Total number of nodes in the hash map across all buckets

	\var bucket
	Bucket array, represented as an array of hashmap_node_t arrays, which will be
	dynamically allocated and reallocated to the required sizes.
	*/
	FOUNDATION_DECLARE_HASHMAP(FOUNDATION_FLEXIBLE_ARRAY);
};

/*! Hashmap of default size. Initialize with a call to
<code>hashmap_fixed_t map;
hashmap_initialize((hashmap_t*)&map, sizeof(map.bucket)/sizeof(map.bucket[0]), bucketsize)</code> */
struct hashmap_fixed_t {
	FOUNDATION_DECLARE_HASHMAP(13);
};

/*! Single node in a uuid hash map, mapping a single key to a single data value (pointer). */
struct uuidmap_node_t {
	/*! Key for the uuid map node */
	uuid_t key;
	/*! Value for the hash map node */
	void* value;
};

/*! Declare an inlined uuidmap of given size */
#define FOUNDATION_DECLARE_UUIDMAP(size) \
	size_t bucket_count;                 \
	size_t node_count;                   \
	uuidmap_node_t* bucket[size]

/*! UUID hash map container, mapping uuid values to data pointers */
struct uuidmap_t {
	/*!
	\var bucket_count
	Number of buckets in the uuid hash map

	\var node_count
	Total number of nodes in the uuid hash map across all buckets

	\var bucket
	Bucket array, represented as an array of uuidmap_node_t arrays, which will be
	dynamically allocated and reallocated to the required sizes.
	*/
	FOUNDATION_DECLARE_UUIDMAP(FOUNDATION_FLEXIBLE_ARRAY);
};

/*! UUID hash map of default size. Initialize with a call to
<code>uuidhmap_fixed_t map;
uuidmap_initialize((uuidmap_t*)&map, sizeof(map.bucket)/sizeof(map.bucket[0]), bucketsize)</code> */
struct uuidmap_fixed_t {
	FOUNDATION_DECLARE_UUIDMAP(13);
};

/*! Node in 32-bit hash table holding key and value for a single node. */
FOUNDATION_ALIGNED_STRUCT(hashtable32_entry_t, 8) {
	/*! Hash key for node in hash table */
	atomic32_t key;
	/*! Value for the corresponding hash key. If the value is zero the node is
	    considered unused/erased. */
	uint32_t value;
};

/*! Node in 64-bit hash table holding key and value for a single node. */
FOUNDATION_ALIGNED_STRUCT(hashtable64_entry_t, 8) {
	/*! Hash key for node in hash table. */
	atomic64_t key;
	/*! Value for the corresponding hash key. If the value is zero the node is
	    considered unused/erased. */
	uint64_t value;
};

/*! Declare an inlined 32-bit hashtable of given size */
#define FOUNDATION_DECLARE_HASHTABLE32(size) \
	size_t capacity;                         \
	hashtable32_entry_t entries[size]

/*! Declare an inlined 64-bit hashtable of given size */
#define FOUNDATION_DECLARE_HASHTABLE64(size) \
	size_t capacity;                         \
	hashtable64_entry_t entries[size]

/*! Hash table, a lock free mapping of 32-but values to 32 bit integer data. */
FOUNDATION_ALIGNED_STRUCT(hashtable32_t, 8) {
	/*!
	\var capacity
	Number of nodes in the table, i.e maximum number of key-value pairs that can be stored.

	\var entries
	Hash table storage as array of nodes where each node is a key-value pair.
	*/
	FOUNDATION_DECLARE_HASHTABLE32(FOUNDATION_FLEXIBLE_ARRAY);
};

/*! Hash table, a lock free mapping of 64-bit values to 64-bit integer data. */
FOUNDATION_ALIGNED_STRUCT(hashtable64_t, 8) {
	/*!
	\var capacity
	Number of nodes in the table, i.e maximum number of key-value pairs that can be stored.

	\var entries
	Hash table storage as array of nodes where each node is a key-value pair.
	*/
	FOUNDATION_DECLARE_HASHTABLE64(FOUNDATION_FLEXIBLE_ARRAY);
};

/*! Memory context stack */
struct memory_context_t {
	/*! Current depth of memory context stack */
	unsigned int depth;
	/*! Memory context stack */
	hash_t context[FOUNDATION_FLEXIBLE_ARRAY];
};

/*! State for a child process */
struct process_t {
	/*! Working directory */
	string_t wd;
	/*! Executable path */
	string_t path;
	/*! Array of argument strings */
	string_t* args;
	/*! Flags, see PROCESS_[*] flags */
	unsigned int flags;
	/*! Exit code for the process */
	int code;
	/*! Pipe stream for stdout */
	stream_t* pipeout;
	/*! Pipe stream for stderr */
	stream_t* pipeerr;
	/*! Pipe stream for stdin */
	stream_t* pipein;
#if FOUNDATION_PLATFORM_WINDOWS
	/*! Windows only, shell verb used when launching process with ShellExecute */
	string_t verb;
	/*! Windows only, process handle */
	void* hp;
	/*! Windows only, thread handle */
	void* ht;
#endif
#if FOUNDATION_PLATFORM_POSIX
	/*! Posix only, process identifier */
	int pid;
#endif
#if FOUNDATION_PLATFORM_MACOS
	/*! MaxOS X only, kqueue for watching for process termination when launching
	    process with LSOpenApplication */
	int kq;
#endif
};

/*! State for a radix sorter for a defined data type. */
struct radixsort_t {
	/*! Data type being sorted */
	radixsort_data_t type;
	/*! Index size */
	radixsort_indextype_t indextype;
	/*! Custom data size */
	size_t custom_data_size;
	/*! Maximum number of elements that can be sorted */
	size_t size;
	/*! Number of elements in last call to #radixsort_sort */
	size_t lastused;
	/*! Index buffers holding sorted result */
	void* indices[2];
	/*! Buffer for histogram data */
	void* histogram;
	/*! Offset table */
	void* offset;
};

/*! Compiled regular expression */
struct regex_t {
	/*! Counter during regex matching keeping number of currently captured substrings */
	unsigned int capture_count;
	/*! Length of the compiled code array */
	size_t code_length;
	/*! Capacity of the code array (number of bytes) */
	size_t code_allocated;
	/*! Compiled regex code */
	uint8_t code[FOUNDATION_FLEXIBLE_ARRAY];
};

/*! Declares the base ring buffer data layout. Use the macro as first declaration in
a ring buffer struct:
<code>typedef struct
{
  FOUNDATION_DECLARE_RINGBUFFER;
  int       some_other_data;
  //[...]
} my_ringbuffer_t;</code> */
#define FOUNDATION_DECLARE_RINGBUFFER(buffersize) \
	uint64_t total_read;                          \
	uint64_t total_write;                         \
	size_t offset_read;                           \
	size_t offset_write;                          \
	size_t buffer_size;                           \
	char buffer[buffersize]

/*! Ring buffer, a shared memory area wrapped to a circular buffer with one read
and one get pointer. */
struct ringbuffer_t {
	/*!
	\var ringbuffer_t::total_read
	Total number of bytes read from ring buffer

	\var ringbuffer_t::total_write
	Total number of bytes written to ring buffer

	\var ringbuffer_t::offset_read
	Current read offset

	\var ringbuffer_t::offset_write
	Current write offset

	\var ringbuffer_t::buffer_size
	Size of buffer in bytes

	\var ringbuffer_t::buffer
	Memory buffer
	*/
	FOUNDATION_DECLARE_RINGBUFFER(FOUNDATION_FLEXIBLE_ARRAY);
};

#if FOUNDATION_PLATFORM_APPLE

/*! Semaphore for thread synchronization and communication. Actual type specifics depend
on platform, the semaphore_t type should be treated as an opaque data struct. */
struct semaphore_t {
	string_t name;
	union {
		struct dispatch_semaphore_s* unnamed;
		sem_t* named;
	} sem;
};

#elif FOUNDATION_PLATFORM_POSIX

FOUNDATION_ALIGNED_STRUCT(semaphore_t, 8) {
	sem_t unnamed;
	sem_t* sem;
	string_t name;
};

#endif

/*! Beacon representation. Linked events are platform dependent. */
struct beacon_t {
	/*! Linked event count */
	size_t count;
#if FOUNDATION_PLATFORM_WINDOWS
	/*! Beacon event */
	void* event;
	/*! Linked events */
	void* all[8];
	/*! Linked events flags */
	unsigned int flags[8];
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
	/*! Beacon file descriptor */
	int fd;
	/*! Beacon poll descriptor */
	int poll;
	/*! Linked events (file descriptors) */
	int all[8];
	/*! Fired flag */
	atomic32_t fired;
#elif FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD
	/*! Beacon kqueue */
	int kq;
	/*! Beacon file descriptor */
	int writefd;
	/*! Linked events (file descriptors) */
	int all[8];
	/*! Fired flag */
	atomic32_t fired;
#else
	/*! Beacon mutex and event */
	mutex_t* mutex;
#endif
};

/*! Thread representation */
struct thread_t {
	/*! OS specific ID */
	uint64_t osid;
	/*! Thread priority */
	thread_priority_t priority;
	/*! Stack size */
	unsigned int stacksize;
	/*! Thread execution function */
	thread_fn fn;
	/*! Argument given to thread execution function */
	void* arg;
	/*! Result code from thread execution function */
	void* result;
	/*! Thread state */
	atomic32_t state;
	/*! Notification beacon */
	beacon_t beacon;

#if FOUNDATION_PLATFORM_WINDOWS
	/*! OS handle */
	uintptr_t handle;
#endif
#if FOUNDATION_PLATFORM_POSIX
	/*! OS handle */
	uintptr_t handle;
#endif
	/*! Name string */
	string_const_t name;
	/*! Buffer for name string */
	char namebuffer[32];
};

/*! Entry in objec map */
struct objectmap_entry_t {
	//! Object pointer
	void* ptr;
	//! Reference count
	atomic32_t ref;
};

#define FOUNDATION_DECLARE_OBJECTMAP(mapsize) \
	uint32_t free;                            \
	uint32_t tag;                             \
	uint32_t size;                            \
	uint32_t autolink;                        \
	semaphore_t write;                        \
	objectmap_entry_t map[mapsize]

/*! Object map which maps object handles to object pointers. As object lifetime is managed
by reference counting, objects that are deallocated will invalidate the handle in the
corresponding object map. */
struct objectmap_t {
	/*!
	\var free
	Current first free slot

	\var id
	Counter for next available ID

	\var size
	Number of slots in map

	\var index_bits
	Number of bits needed for slot index

	\var id_max
	Maximum ID depending on how many bits are used by size

	\var mask_index
	Bitmask for slot index

	\var mask_id
	Bitmask for ID

	\var write
	Write access semaphore

	\var map
	Slot array
	*/
	FOUNDATION_DECLARE_OBJECTMAP(FOUNDATION_FLEXIBLE_ARRAY);
};

/*! Declares the base stream data layout. Stream structures should be 8-byte align for
platform compatibility. Use the macro as first declaration in a stream struct:
<code>typedef FOUNDATION_ALIGNED_STRUCT(my_stream_t, 8)
{
  FOUNDATION_DECLARE_STREAM;
  int       some_other_data;
  //[...]
};</code> */
#define FOUNDATION_DECLARE_STREAM        \
	unsigned int type : 16;              \
	unsigned int sequential : 1;         \
	unsigned int reliable : 1;           \
	unsigned int inorder : 1;            \
	unsigned int swap : 1;               \
	unsigned int byteorder : 1;          \
	unsigned int persistent : 1;         \
	unsigned int compressed : 1;         \
	unsigned int unused_streamflags : 9; \
	unsigned int mode;                   \
	string_t path;                       \
	string_const_t mime_type;            \
	stream_vtable_t* vtable

/*! Base stream type from which all streams are derived. All stream-based designs
must have this layout at the start of the structure. See #FOUNDATION_DECLARE_STREAM
for a macro to declare the base layout in a structure. */
FOUNDATION_ALIGNED_STRUCT(stream_t, 8) {
	/*!
	\var stream_t::type
	\details Stream type identifier

	\var stream_t::sequential
	Flag indicating stream is sequential, meaning it can only read/write sequentially
	and only seek forward from current position. Generally stream size is unavailable
	for sequential streams.

	\var stream_t::reliable
	Flag indicating stream is reliable, meaning data written is guaranteed to be readable.

	\var stream_t::inorder
	Flag indicating stream is in-order, meaning data written is guaranteed to be read in
	the same order as it was written.

	\var stream_t::swap
	Flag indicating stream is byte order swapped, meaning multi-byte data will be byte
	order swapped on read/write.

	\var stream_t::byteorder
	Stream byte order indicator (0 = little endian, 1 = big endian)

	\var stream_t::persistent
	Flag indicating the stream is located on persistent storage, meaning the stream
	can most likely be re-opened and/or cloned.

	\var stream_t::mode
	Stream open mode (see STREAM_* modes/flags)

	\var stream_t::path
	Stream path

	\var stream_t::vtable
	Stream virtual table with stream type specific implementation function pointers
	*/
	FOUNDATION_DECLARE_STREAM;
};

/*! Stream interface for read/write to a memory buffer. This struct is also a stream_t
(stream struct type declared at start of struct) and can be used in all functions
operating on a stream_t. The memory buffer is allocated separately from the stream buffer. */
FOUNDATION_ALIGNED_STRUCT(stream_buffer_t, 8) {
	FOUNDATION_DECLARE_STREAM;
	/*! Current read/write offset */
	size_t current;
	/*! Current size of buffer (always less or equal than capacity) */
	size_t size;
	/*! Current allocated capacity of buffer */
	size_t capacity;
	/*! Memory buffer */
	void* buffer;
	/*! If this flag is set the memory buffer is owned internally by the stream buffer
	and will be deallocated together with the stream buffer. If not set, the ownership
	of the memory buffer is handled externally. */
	bool own;
	/*! If this flag is set (and if the memory buffer is owned by the stream buffer),
	the memory buffer can grow by reallocating memory if write requests increase size
	beyond the capacity of the memory buffer. If not set, any write requests beyond
	the capacity will fail. */
	bool grow;
	/*! Timestamp of last modification */
	tick_t lastmod;
};

/*! Stream interface for read/write to a pipe. This struct is also a stream_t
(stream struct type declared at start of struct) and can be used in all functions
operating on a stream_t. Pipe streams are sequential. */
FOUNDATION_ALIGNED_STRUCT(stream_pipe_t, 8) {
	FOUNDATION_DECLARE_STREAM;
	/*! End of stream flag indicating the pipe has reached end of stream and
	is considered closed. */
	bool eos;
	/*! File descriptor for read end of the pipe. */
	int fd_read;
	/*! File descriptor for write end of the pipe. */
	int fd_write;
};

/*! Stream interface for read/write to a ring buffer. This struct is also a stream_t
(stream struct type declared at start of struct) and can be used in all functions
operating on a stream_t. Read and write operation can be concurrent (one single
thread reading, one single thread writing) as availability of data is flagged with
semaphores. Multiple readers and/or writers are not supported. Stream is sequential. */
FOUNDATION_ALIGNED_STRUCT(stream_ringbuffer_t, 8) {
	FOUNDATION_DECLARE_STREAM;
	/*! Semaphore signalling availability of data for reading */
	semaphore_t signal_read;
	/*! Semaphore signalling availability of data for writing */
	semaphore_t signal_write;
	/*! Number of bytes pending for reading */
	volatile int pending_read;
	/*! Number of bytes pending for writing */
	volatile int pending_write;
	/*! Number of bytes written (total size of stream) */
	size_t total_size;

	FOUNDATION_DECLARE_RINGBUFFER(FOUNDATION_FLEXIBLE_ARRAY);
};

/*! Virtual function table for stream implementations. Each stream type must provide
implementation for the basic stream operations in this struct or set the entry to null
to indicate that the functionality is not supported by the stream type. */
struct stream_vtable_t {
	/*! Function to read data from stream. */
	stream_read_fn read;
	/*! Function to write data to stream. */
	stream_write_fn write;
	/*! Function to query if current position is at end of stream. */
	stream_eos_fn eos;
	/*! Function to flush any pending data to stream. */
	stream_flush_fn flush;
	/*! Function to truncate stream to current position. */
	stream_truncate_fn truncate;
	/*! Function to query size of stream */
	stream_size_fn size;
	/*! Function to seek in stream. */
	stream_seek_fn seek;
	/*! Function to query the current position in the stream. */
	stream_tell_fn tell;
	/*! Function to query the last modification timestamp of the stream. */
	stream_lastmod_fn lastmod;
	/*! Function to query the MD5 of the stream content */
	stream_md5_fn md5;
	/*! Function to query the SHA-256 of the stream content */
	stream_sha256_fn sha256;
	/*! Function to query the SHA-512 of the stream content */
	stream_sha512_fn sha512;
	/*! Function to buffer any incoming data for the stream. */
	stream_buffer_read_fn buffer_read;
	/*! Function to query the amount of data that can be read without blocking. */
	stream_available_read_fn available_read;
	/*! Function to finalize the stream, free memory and release any resources associated
	with the stream. */
	stream_finalize_fn finalize;
	/*! Function to clone the stream. */
	stream_clone_fn clone;
};

/*! JSON token. The token points into the parsed data buffer using absolute offsets
from start of buffer */
struct json_token_t {
	/*! Token type */
	json_type_t type;
	/*! Identifier string offset */
	unsigned int id;
	/*! Length of identifier string. 0 if no identifier string */
	unsigned int id_length;
	/*! Value string offset */
	unsigned int value;
	/*! Length of value string for objects and primitive values, 0 if no or empty value string. For
	 * array values the number of elements in the array. */
	unsigned int value_length;
	/*! Child token index in token array. 0 if no child token */
	unsigned int child;
	/*! Sibling token index in token array. 0 if no sibling token */
	unsigned int sibling;
};

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic pop
#endif
