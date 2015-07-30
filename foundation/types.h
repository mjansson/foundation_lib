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
\brief Foundation data types
\details Foundation data types, enumerations and typedefs. Provides platform abstractions
of system specific data types and provides the base language used in all libraries built
on this foundation library. */

#include <foundation/platform.h>
#include <foundation/build.h>

#if defined( FOUNDATION_PLATFORM_DOXYGEN )
#  define FOUNDATION_ALIGNED_STRUCT( name, alignment ) struct name
#endif

#if FOUNDATION_COMPILER_CLANG
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wpadded"
#endif

// PRIMITIVE TYPES

/*! \brief Error severity level
Error severity level. The higher the value, the more severe the error. Error level 0
(ERRORLEVEL_NONE) is used to indicate no error
\internal Do not change order! \endinternal */
typedef enum {
	/*! No error */
	ERRORLEVEL_NONE    = 0,
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

/*! \brief Error identifiers
\details Error identifiers. Error 0 (ERROR_NONE) is used to indicate no error.
\internal Do not change order, only append! \endinternal */
typedef enum {
	/*! No error */
	ERROR_NONE              = 0,
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
	/*! Marker, last reserved internal error identifier */
	ERROR_LAST_BUILTIN  = 0x0fff
} error_t;

/*! \brief Warning classes
Warning classes. Indicate which type/group of warning message that was generated
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
	/*! Marker, last reserved inernal warning identifier */
	WARNING_LAST_BUILTIN  = 0x0fff
} warning_t;

/*! \brief Platform identifiers
Platform identifiers. For compile-time platform selection, use the
FOUNDATION_PLATFORM_[...] preprocessor macros
\internal Do not change order, only append! \endinternal */
typedef enum {
	/*! Windows */
	PLATFORM_WINDOWS   = 0,
	/*! Linux */
	PLATFORM_LINUX,
	/*! MacOS X */
	PLATFORM_MACOSX,
	/*! iOS (iPhone, iPad) */
	PLATFORM_IOS,
	/*! Android */
	PLATFORM_ANDROID,
	/*! Raspberry Pi (linux flavour) */
	PLATFORM_RASPBERRYPI,
	/*! PNaCl (unknown host platform) */
	PLATFORM_PNACL,
	/*! BSD */
	PLATFORM_BSD,
	/*! Tizen */
	PLATFORM_TIZEN,
	/*! Invalid platform identifier */
	PLATFORM_INVALID
} platform_t;

/*! \enum architecture_t
\brief Architecture identifiers
Architecture identifiers for all architectures the engine supports. For compile-time
selection of architecture, use the <code>FOUNDATION_ARCH_[...]</code> preprocessor macros.
\internal Do not change order, only append! \endinternal */
typedef enum {
	/*! x86 (Classic 32-bit x86 compatible CPUs) */
	ARCHITECTURE_X86          = 0,
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
	/*! Generic/unknown (for PNaCl) */
	ARCHITECTURE_GENERIC
} architecture_t;

/*! \brief Machine byte order identifiers
Machine byte order identifiers */
typedef enum {
	/*! Little endian */
	BYTEORDER_LITTLEENDIAN = 0,
	/*! Big endian */
	BYTEORDER_BIGENDIAN
} byteorder_t;

/*! \brief Stream type identifiers
Stream type identifiers. Only lists built-in types, application specific types
can be added below the STREAMTYPE_LAST_RESERVED value.
\internal Do not change order, only append! \endinternal */
typedef enum {
	/*! Invalid identifier */
	STREAMTYPE_INVALID         = 0,
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
	/*! Last reserved built-in stream type, not a valid type */
	STREAMTYPE_LAST_RESERVED   = 0x0FFF
} stream_type_t;

/*! \brief Stream seek directions
Stream seek directions */
typedef enum {
	/*! Seek from start of stream */
	STREAM_SEEK_BEGIN,
	/*! Seek from current position of stream */
	STREAM_SEEK_CURRENT,
	/*! Seek from end of stream */
	STREAM_SEEK_END
} stream_seek_mode_t;

/*! \brief Thread priority
Thread priority */
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

/*! \brief Event identifiers
Foundation library level event identifiers. These event identifiers are only
valid in conjunction with foundation event streams. Other event streams will use
their own event identifiers with the same value, and event streams should be treated
as separate "namespaces" for event identifiers. */
typedef enum {
	/*! Application has been asked to start */
	FOUNDATIONEVENT_START = 1,
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
	FOUNDATIONEVENT_DEVICE_ORIENTATION
} foundation_event_id;

/*! \brief Cipher mode
Block cipher mode of operation, see
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

/*! \brief Radix sort data types
Radix sort data types */
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
	RADIXSORT_FLOAT64
} radixsort_data_t;

/*! \brief Device orientation
Device orientation */
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

/*! Memory hint, memory allocationis persistent (retained when function returns) */
#define MEMORY_PERSISTENT       0
/*! Memory hint, memory is temporary (extremely short lived and generally freed
before function returns or scope ends) */
#define MEMORY_TEMPORARY        (1U<<0)
/*! Memory hint, memory allocation is local to the calling thread */
#define MEMORY_THREAD           (1U<<1)
/*! Memory flag, memory should be allocated in low 32-bit address space */
#define MEMORY_32BIT_ADDRESS    (1U<<2)
/*! Memory flag, memory should be initialized to zero during allocation */
#define MEMORY_ZERO_INITIALIZED (1U<<3)

/*! Event flag, event is delayed and will be delivered at a later timestamp */
#define EVENTFLAG_DELAY (1U<<0)

/*! Application flag, application is a command line utility and should not have
a normal windowing system interaction loop */
#define APPLICATION_UTILITY (1U<<0)
/*! Application flag, application is a daemon/service */
#define APPLICATION_DAEMON  (1U<<1)

/*! Stream mode/flag, stream is readable */
#define STREAM_IN       (1U<<0)
/*! Stream mode/flag, stream is writable */
#define STREAM_OUT      (1U<<1)
/*! Stream flag, stream is truncated on open */
#define STREAM_TRUNCATE (1U<<2)
/*! Stream flag, stream target is created if it does not previously exist */
#define STREAM_CREATE   (1U<<3)
/*! Stream flag, stream position is set to end on open */
#define STREAM_ATEND    (1U<<4)
/*! Stream flag/mode, stream I/O is binary (I/O is in ascii if flag not set) */
#define STREAM_BINARY   (1U<<5)
/*! Stream flag, stream is synchronized on each write */
#define STREAM_SYNC     (1U<<6)

/*! Process flag, spawn method will block until process ends and then return
process exit code */
#define PROCESS_ATTACHED                   0
/*! Process flag, spawn method will immediately return with code 0 unless
an error occurs during spawning */
#define PROCESS_DETACHED                   (1U<<0)
/*! Process flag, create a console window for process */
#define PROCESS_CONSOLE                    (1U<<1)
/*! Process flag, create stdout/stdin pipes to process */
#define PROCESS_STDSTREAMS                 (1U<<2)
/*! Process flag, use ShellExecute instead of CreateProcess (Windows platform only) */
#define PROCESS_WINDOWS_USE_SHELLEXECUTE   (1U<<3)
/*! Process flag, use LSOpenApplication instead of fork/execve (MacOSX platform only) */
#define PROCESS_MACOSX_USE_OPENAPPLICATION (1U<<4)

/*! Process exit code, returned when given invalid arguments */
#define PROCESS_INVALID_ARGS      0x7FFFFFF0
/*! Process exit code, returned when process was terminated by signal */
#define PROCESS_TERMINATED_SIGNAL 0x7FFFFFF1
/*! Process exit code, returned when process wait was interrupted */
#define PROCESS_WAIT_INTERRUPTED  0x7FFFFFF2
/*! Process exit code, returned when process wait failed for unknown reasons */
#define PROCESS_WAIT_FAILED       0x7FFFFFF3
/*! Process exit code, returned when detached process is still running */
#define PROCESS_STILL_ACTIVE      0x7FFFFFFF

#if FOUNDATION_PLATFORM_WINDOWS
#  if FOUNDATION_ARCH_X86
#    if FOUDATION_COMPILER_MSVC
typedef _W64 int      ssize_t;
#    else
typedef int           ssize_t;
#    endif
#  else
typedef int64_t       ssize_t;
#  endif
#endif

/*! \brief Hash value
Hash value */
typedef uint64_t      hash_t;
/*! \brief Tick
Tick type used for absolute time measurements or timestamps */
typedef int64_t       tick_t;
/*! \brief Deltatime
Deltatime type used for floating point time differences */
typedef real          deltatime_t;
/*! \brief Object handle
Object handle used for identifying reference counted objects */
typedef uint64_t      object_t;
/*! \brief Index for radix sorter
Default is 16 bit, typedef to 32 bit if need to sort more than 2^16 items in one array */
typedef uint16_t      radixsort_index_t;
/*! \brief UUID
UUID, 128-bit unique identifier */
typedef uint128_t     uuid_t;

/*! \brief Float 32-bit representation
Used to bit manipulate 32-bit floating point values in a alias safe way */
typedef union { int32_t ival; uint32_t uival; float32_t fval; } float32_cast_t;
/*! \brief Float 64-bit representation
Used to bit manipulate 64-bit floating point values in a alias safe way */
typedef union { int64_t ival; uint64_t uival; float64_t fval; } float64_cast_t;
#if FOUNDATION_SIZE_REAL == 64
/*! \brief Real representation
Used to bit manipulate real values in a alias safe way */
typedef union { int64_t ival; uint64_t uival; real rval; } real_cast_t;
#else
/*! \brief Real representation
Used to bit manipulate real values in a alias safe way */
typedef union { int32_t ival; uint32_t uival; real rval; } real_cast_t;
#endif

typedef struct string_t               string_t;
typedef struct string_const_t         string_const_t;
/*! \brief Application declaration
Application declaration and configuration */
typedef struct application_t          application_t;
/*! \brief Bit buffer
Bit buffer instance */
typedef struct bitbuffer_t            bitbuffer_t;
/*! \brief Blowfish
Blowfish cipher instance */
typedef struct blowfish_t             blowfish_t;
/*! \brief Error frame
Error frame holding debug data for an entry in the frame stack in the error context */
typedef struct error_frame_t          error_frame_t;
/*! \brief Error context
Error context holding error frame stack for a thread */
typedef struct error_context_t        error_context_t;
/*! \brief Evenbt
Event base structure */
typedef struct event_t                event_t;
/*! \brief Event block
Event block holding a chunk of events from a single stream */
typedef struct event_block_t          event_block_t;
/*! \brief Event stream
Event stream instance producing event blocks of events */
typedef struct event_stream_t         event_stream_t;
/*! \brief File system event payload
Payload for a file system event */
typedef struct fs_event_payload_t     fs_event_payload_t;
/*! \brief Hash map node
Node in a hash map */
typedef struct hashmap_node_t         hashmap_node_t;
/*! \brief Hash map
Hash map mapping hash value keys to pointer values */
typedef struct hashmap_t              hashmap_t;
/*! \brief Hash table entry
Entry in a 32-bit hash table */
typedef struct hashtable32_entry_t    hashtable32_entry_t;
/*! \brief Hash table entry
Entry in a 64-bit hash table */
typedef struct hashtable64_entry_t    hashtable64_entry_t;
/*! \brief Hash table
Hash table mapping 32-bit keys to 32-bit values */
typedef struct hashtable32_t          hashtable32_t;
/*! \brief Hash table
Hash table mapping 64-bit keys to 64-bit values */
typedef struct hashtable64_t          hashtable64_t;
/*! \brief MD5
MD5 control block */
typedef struct md5_t                  md5_t;
/*! \brief Memory context
Memory context holding the allocation context stack */
typedef struct memory_context_t       memory_context_t;
/*! \brief Memory system
Memory system declaration */
typedef struct memory_system_t        memory_system_t;
/*! \brief Memory tracker
Memory tracker declaration */
typedef struct memory_tracker_t       memory_tracker_t;
/*! \brief Mutex
Platform specific mutex representation */
typedef struct mutex_t                mutex_t;
/*! \brief Object
Base object type all reference counted object types are based on */
typedef struct object_base_t          object_base_t;
/*! \brief Object map
Object map mapping object handles to object instance pointers */
typedef struct objectmap_t            objectmap_t;
/*! \brief Process
Child process control block */
typedef struct process_t              process_t;
/*! \brief Radix sorter
Radix sorter control block */
typedef struct radixsort_t            radixsort_t;
/*! \brief Compiled regex
Compiled regex */
typedef struct regex_t                regex_t;
/*! \brief Ring buffer
Memory ring buffer */
typedef struct ringbuffer_t           ringbuffer_t;
/*! \brief Stream
Base stream type all stream types are based on */
typedef struct stream_t               stream_t;
/*! \brief Memory buffer stream
Memory buffer stream */
typedef struct stream_buffer_t        stream_buffer_t;
/*! \brief Pipe stream
Pipe stream */
typedef struct stream_pipe_t          stream_pipe_t;
/*! \brief Ring buffer stream
Ring buffer stream */
typedef struct stream_ringbuffer_t    stream_ringbuffer_t;
/*! \brief Vtable for streams
Vtable for streams providing stream type specific implementations
of stream operations */
typedef struct stream_vtable_t        stream_vtable_t;
/*! \brief Version
Version declaration */
typedef union  version_t              version_t;
/*! \brief Library configuration
Library configuration block controlling limits, functionality and memory
usage of the library */
typedef struct foundation_config_t    foundation_config_t;

#if FOUNDATION_PLATFORM_WINDOWS
/*! \brief Semaphore
Platform specific representation of a semaphore */
typedef void*                         semaphore_t;
#elif FOUNDATION_PLATFORM_MACOSX
typedef struct OpaqueMPSemaphoreID*   MPSemaphoreID;
typedef struct semaphore_t            semaphore_t;
#elif FOUNDATION_PLATFORM_IOS
typedef struct dispatch_semaphore_s*  semaphore_t;
#elif FOUNDATION_PLATFORM_BSD
#  include <semaphore.h>
typedef sem_t                         semaphore_native_t;
typedef struct semaphore_t            semaphore_t;
#elif FOUNDATION_PLATFORM_POSIX || FOUNDATION_PLATFORM_PNACL
typedef union semaphore_native_t      semaphore_native_t;
typedef struct semaphore_t            semaphore_t;
#endif

/*! \brief Error handler callback
Error handler callback which is passed the error level and reported error. It should return
an implementation specific code which is then returned from the call to error_report
\param level Error level
\param error Error code
\return Implementation specific code which is passed back as return from error_report */
typedef int (* error_callback_fn)(error_level_t level, error_t error);

/*! \brief Assert handler callback
Assert handler callback which is passed assert data and should do impementation specific
processing and return a code indicating if execution can continue or need to be aborted.
\param context Error context
\param condition String expressing the condition that failed
\param cond_length Length of condition string
\param file Source file triggering the assert
\param file_length Length of source file string
\param line Source line triggering the assert
\param msg Assert message, if any
\param msg_length Length of assert message
\return 1 if assert was not handled and execution should break, 0 if assert handled and execution can continue */
typedef int (* assert_handler_fn)(hash_t context, const char* condition, size_t cond_length,
                                  const char* file, size_t file_length, unsigned int line,
                                  const char* msg, size_t msg_length);

/*! \brief Log output callback
Log output callback. Called after each log message processed and output by the log functions.
\param context Log context
\param severity Log severity
\param msg Log message
\param length Length of message */
typedef void (* log_callback_fn)(hash_t context, error_level_t severity, const char* msg,
                                 size_t length);

/*! \brief Subsystem initialization
Subsystem initialization function prototype. Return value should be the success
state of initialization
\return 0 on success, <0 if failure (errors should be reported through log_error
        or error_report) */
typedef int (* system_initialize_fn)(void);

/*! \brief Subsystem finalization
Subsystem finalization function prototype. Will be called for each successfully
initialized subsystem on global finalization */
typedef void (* system_finalize_fn)(void);

/*! \brief Memory allocation
Memory system allocation function prototype. Implementation of a memory system must
provide an implementation with this prototype for allocating memory
\param context Memory context
\param size Requested size
\param align Aligmnent requirement
\param hint Memory hitn (see memory_hint_t)
\return Pointer to allocated memory block if successful, 0 if error */
typedef void* (* memory_allocate_fn)(hash_t context, size_t size, unsigned int align,
                                     unsigned int hint);

/*! \brief Memory reallocation
Memory system reallocation function prototype. Implementation of a memory system must
provide an implementation with this prototype for reallocating memory
\param p Pointer to previous memory block
\param size Requested size
\param align Aligmnent requirement
\param oldsize Size of previous memory block
\return Pointer to allocated memory block if successful, 0 if error */
typedef void* (* memory_reallocate_fn)(void* p, size_t size, unsigned int align, size_t oldsize);

/*! \brief Memory deallocation
Memory system deallocation function prototype. Implementation of a memory system must
provide an implementation with this prototype for deallocating memory
\param p Pointer to memory block */
typedef void (* memory_deallocate_fn)(void* p);

/*! \brief Memory tracking
Memory tracker tracking function prototype. Implementation of a memory tracker must
provide an implementation with this prototype for tracking memory allocations
\param p Pointer to allocated memory block
\param size Size of memory block */
typedef void (* memory_track_fn)(void* p, size_t size);

/*! \brief Memory tracking
Memory tracker untracking function prototype. Implementation of a memory tracker must
provide an implementation with this prototype for untracking memory allocations
\param p Pointer to deallocated memory block */
typedef void (* memory_untrack_fn)(void* p);

/*! \brief Profile data output callback
Callback function for writing profiling data to a stream
\param data Pointer to data block
\param size Size of data block */
typedef void (* profile_write_fn)(void* data, size_t size);

/*! \brief Profile data input callback
Callback function for reading profiling data from a stream
\param data Pointer to data block
\param size Size of data block */
typedef void (* profile_read_fn)(void* data, size_t size);

/*! \brief Thread entry point
Thread entry point function prototype
\param thread Thread object handle
\param arg Argument passed by caller when starting the thread
\return Implementation specific data which can be obtained through thread_result */
typedef void* (* thread_fn)(object_t thread, void* arg);

/*! \brief Function prototype for crash guards
 Any function to be used in conjunction with the crash guard functionality
of the library should have this prototype to allow the crash guard to catch
and handle exceptions correctly
\param arg Implementation specific argument passed to crash_guard
\return Implementation specific return value which is forwarded as return value
        from crash_guard (note that FOUNDATION_CRASH_DUMP_GENERATED is reserved) */
typedef int (* crash_guard_fn)(void* arg);

/*! \brief Crash callback
Crash callback function prototype, used to notify that an exception occurred
and the process state was saved to a dump file
\param file Dump file path
\param length Length of file path */
typedef void (* crash_dump_callback_fn)(const char* file, size_t length);

/*! \brief Object deallocation
Object deallocation function prototype, used to deallocate an object of a specific type
\param id Object handle
\param object Object pointer */
typedef void (* object_deallocate_fn)(object_t id, void* object);

/*! \brief Stream open
Generic function to open a stream with the given path and mode
\param path Path, optionally including protocol
\param length Length of path
\param mode Open mode
\return Newly allocated stream, null if it could not be opened */
typedef stream_t* (* stream_open_fn)(const char* path, size_t length, unsigned int mode);

/*! \brief Stream read
Generic function to read data from a stream
\param stream Stream to read from
\param dst Destination buffer
\param size Number of bytes to read
\return Number of bytes actually read */
typedef size_t (* stream_read_fn)(stream_t* stream, void* dst, size_t size);

/*! \brief Stream write
Generic function to write data to a stream
\param stream Stream to write to
\param src Source buffer
\param size Number of bytes to write
\return Number of bytes actually written */
typedef size_t (* stream_write_fn)(stream_t* stream, const void* src, size_t size);

/*! \brief Query if end of stream
Query if end of stream
\param stream Stream
\return true if stream at end, false if not */
typedef bool (* stream_eos_fn)(stream_t* stream);

/*! \brief Flush stream
Flush stream output buffers
\param stream Stream */
typedef void (* stream_flush_fn)(stream_t* stream);

/*! \brief Truncate stream
Truncate stream size to the given size
\param stream Stream
\param size Size to truncate stream to */
typedef void (* stream_truncate_fn)(stream_t* stream, size_t size);

/*! \brief Get stream size
Get stream size
\param stream Stream
\return Stream size, 0 if invalid stream or unknown (like a network stream) */
typedef size_t (* stream_size_fn)(stream_t* stream);

/*! \brief Seek in stream
Seek in the stream. Only available if stream is seekable and not a sequential stream like
a network stream.
\param stream Stream
\param offset Seek offset
\param mode Seek mode (see #stream_seek_mode_t) */
typedef void (* stream_seek_fn)(stream_t* stream, ssize_t offset, stream_seek_mode_t mode);

/*! \brief Get current stream position
Get current stream position
\param stream Stream
\return Current stream position, 0 if invalid stream or unknown */
typedef size_t (* stream_tell_fn)(stream_t* stream);

/*! \brief Get last modification timestamp
Get timestamp when stream was last modified (written to or attributes/size changed)
\param stream Stream
\return Last modification timestamp, 0 if invalid stream or unknown */
typedef tick_t (* stream_lastmod_fn)(const stream_t* stream);

/*! \brief Get stream digest
Get stream digest. Only available if stream size is known and stream is seekable. Does
not modify the current stream position.
\param stream Stream
\return Digest of stream content, 0 if invalid stream or unknown */
typedef uint128_t (* stream_md5_fn)(stream_t* stream);

/*! \brief Buffer available data
If the stream has available data to be read from an external source (like a socket for
network streams), read and buffer the available data without blocking.
\param stream Stream */
typedef void (* stream_buffer_read_fn)(stream_t* stream);

/*! \brief Query available data
Query how much data can be read from the stream without blocking
\param stream Stream
\return Number of bytes that can be read without blocking */
typedef size_t (* stream_available_read_fn)(stream_t* stream);

/*! \brief Finalize stream object
Finalize a stream object that was previously initialized with a call to a specific
stream initialization function and free any associated resources.
\param stream Stream */
typedef void (* stream_finalize_fn)(stream_t* stream);

/*! \brief Clone stream
Clone stream, allocating a duplicate copy of the stream if the stream type supports it.
\param stream Stream
\return Clone of stream, 0 if not supported or invalid source stream */
typedef stream_t* (* stream_clone_fn)(stream_t* stream);

/*! \brief Crash identifier
Identifier returned from threads and crash guards after a fatal exception (crash) has been caught */
#define FOUNDATION_CRASH_DUMP_GENERATED 0x0badf00dL

// COMPLEX TYPES

/*! \brief Library configuration
Library configuration with runtime controlled configuration parameters */
struct foundation_config_t {
	/*! Maximum number of concurrently allocated threads. Zero for default (128) */
	size_t thread_max;
	/*! Maximum number of concurrently allocated libraries. Zero for default (32) */
	size_t library_max;
	/*! Maximum number of concurrent allocations in memory tracker. Zero for default (32k) */
	size_t memory_tracker_max;
	/*! Maximum number of file system monitors. Zero for default (16) */
	size_t fs_monitor_max;
	/*! Size of temporary memory pool (short lived allocations). Zero for default (512KiB) */
	size_t temporary_memory;
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

/*! \brief String tuple
String tuple holding string data pointer and length. This is used to avoid extra calls
to determine string length in each API call */
struct string_t {
	char* str;
	size_t length;
};

/*! Constant string tuple
Constant string tuple holding unmutable string data pointer and length.
\see string_t */
struct string_const_t {
	const char* str;
	size_t length;
};

/*! \brief MD5 state
MD5 state */
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

/*! \brief Memory management system declaration
Memory management system declaration with function pointers for all memory system entry points. */
struct memory_system_t {
	/*! Memory allocation */
	memory_allocate_fn allocate;
	/*! Memory reallocation */
	memory_reallocate_fn reallocate;
	/*! Memory deallocation */
	memory_deallocate_fn deallocate;
	/*! System initialization */
	system_initialize_fn initialize;
	/*! System finalization */
	system_finalize_fn finalize;
};

struct memory_tracker_t {
	memory_track_fn track;
	memory_untrack_fn untrack;
	system_initialize_fn initialize;
	system_finalize_fn finalize;
};

union version_t {
	uint128_t version;
	struct {
		uint16_t major;
		uint16_t minor;
		uint32_t revision;
		uint32_t build;
		uint32_t control;
	} sub;
};

struct application_t {
	string_const_t name;
	string_const_t short_name;
	string_const_t config_dir;
	version_t version;
	crash_dump_callback_fn dump_callback;
	unsigned int flags;
	uuid_t instance;
};

#define BLOWFISH_SUBKEYS            18U
#define BLOWFISH_SBOXES             4U
#define BLOWFISH_SBOXENTRIES        256U
#define BLOWFISH_MAXKEY             56U

struct blowfish_t {
	uint32_t parray[BLOWFISH_SUBKEYS];
	uint32_t sboxes[BLOWFISH_SBOXES][BLOWFISH_SBOXENTRIES];
};

struct bitbuffer_t {
	uint8_t* buffer;
	uint8_t* end;
	stream_t* stream;
	bool swap;
	uint32_t pending_read;
	uint32_t pending_write;
	unsigned int offset_read;
	unsigned int offset_write;
	uint64_t count_read;
	uint64_t count_write;
};

struct error_frame_t {
	string_const_t name;
	string_const_t data;
};

struct error_context_t {
	unsigned int depth;
	error_frame_t frame[];
};

#define FOUNDATION_DECLARE_EVENT \
	uint16_t id; \
	uint16_t flags; \
	uint16_t serial; \
	uint16_t size; \
	object_t object

struct event_t {
	FOUNDATION_DECLARE_EVENT;
	size_t payload[];
};


struct event_block_t {
	size_t used;
	size_t capacity;
	event_stream_t* stream;
	event_t* events;
};

FOUNDATION_ALIGNED_STRUCT(event_stream_t, 16) {
	atomic32_t write;
	int32_t read;
	event_block_t block[2];
};

struct fs_event_payload_t {
	size_t length;
	const char str[];
};

struct hashmap_node_t {
	hash_t key;
	void* value;
};

struct hashmap_t {
	size_t num_buckets;
	size_t num_nodes;
	hashmap_node_t* bucket[];
};

FOUNDATION_ALIGNED_STRUCT(hashtable32_entry_t, 8) {
	atomic32_t key;
	uint32_t value;
};

FOUNDATION_ALIGNED_STRUCT(hashtable64_entry_t, 8) {
	atomic64_t key;
	uint64_t value;
};

FOUNDATION_ALIGNED_STRUCT(hashtable32_t, 8) {
	size_t capacity;
	hashtable32_entry_t entries[];
};

FOUNDATION_ALIGNED_STRUCT(hashtable64_t, 8) {
	size_t capacity;
	hashtable64_entry_t entries[];
};

struct memory_context_t {
	unsigned int depth;
	hash_t context[];
};

#define FOUNDATION_DECLARE_OBJECT \
	atomic32_t ref; \
	uint32_t flags; \
	object_t id

FOUNDATION_ALIGNED_STRUCT(object_base_t, 8) {
	FOUNDATION_DECLARE_OBJECT;
};

FOUNDATION_ALIGNED_STRUCT(objectmap_t, 8) {
	atomic64_t free;
	atomic64_t id;
	size_t size;
	unsigned int size_bits;
	uint64_t id_max;
	uint64_t mask_index;
	uint64_t mask_id;
	void* map[];
};

struct process_t {
	string_t wd;
	string_t path;
	string_t* args;
	unsigned int flags;
	int code;
	stream_t* pipeout;
	stream_t* pipein;

#if FOUNDATION_PLATFORM_WINDOWS
	string_t  verb;
	void* hp;
	void* ht;
#endif
#if FOUNDATION_PLATFORM_POSIX
	int pid;
#endif
#if FOUNDATION_PLATFORM_MACOSX
	int kq;
#endif
};

struct radixsort_t {
	radixsort_data_t type;
	radixsort_index_t size;
	radixsort_index_t lastused;
	radixsort_index_t* indices[2];
	radixsort_index_t* histogram;
	radixsort_index_t* offset;
};

struct regex_t {
	unsigned int num_captures;
	size_t code_length;
	size_t code_allocated;
	uint8_t code[];
};

#define FOUNDATION_DECLARE_RINGBUFFER \
	uint64_t total_read; \
	uint64_t total_write; \
	size_t  offset_read; \
	size_t offset_write; \
	size_t buffer_size; \
	char buffer[]

struct ringbuffer_t {
	FOUNDATION_DECLARE_RINGBUFFER;
};

#if FOUNDATION_PLATFORM_MACOSX

struct semaphore_t {
	string_t name;
	union {
		MPSemaphoreID unnamed;
		int* named;
	} sem;
};

#elif FOUNDATION_PLATFORM_IOS
#elif FOUNDATION_PLATFORM_BSD

struct semaphore_t {
	string_t name;
	semaphore_native_t* sem;
	semaphore_native_t unnamed;
};

#elif FOUNDATION_PLATFORM_POSIX || FOUNDATION_PLATFORM_PNACL

union semaphore_native_t {
#  if FOUNDATION_PLATFORM_ANDROID
	volatile unsigned int count;
#  elif FOUNDATION_PLATFORM_PNACL
	volatile int count;
	volatile int nwaiters;
#else
#  if FOUNDATION_ARCH_X86_64
	char __size[64];
#  else
	char __size[32];
#  endif
	long int __align;
#endif
};

struct semaphore_t {
	string_t name;
	semaphore_native_t* sem;
	semaphore_native_t unnamed;
};

#endif

#define FOUNDATION_DECLARE_STREAM \
	unsigned int type:16; \
	unsigned int sequential:1; \
	unsigned int reliable:1; \
	unsigned int inorder:1; \
	unsigned int swap:1; \
	unsigned int byteorder:1; \
	unsigned int unused_streamflags:11; \
	unsigned int mode; \
	string_t path; \
	stream_vtable_t* vtable

FOUNDATION_ALIGNED_STRUCT(stream_t, 8) {
	FOUNDATION_DECLARE_STREAM;
};

FOUNDATION_ALIGNED_STRUCT(stream_buffer_t, 8) {
	FOUNDATION_DECLARE_STREAM;
	size_t current;
	size_t size;
	size_t capacity;
	void* buffer;
	bool own;
	bool grow;
	tick_t lastmod;
};

FOUNDATION_ALIGNED_STRUCT(stream_pipe_t, 8) {
	FOUNDATION_DECLARE_STREAM;
	bool eos;
#if FOUNDATION_PLATFORM_WINDOWS
	void* handle_read;
	void* handle_write;
#endif
#if FOUNDATION_PLATFORM_POSIX || FOUNDATION_PLATFORM_PNACL
	int fd_read;
	int fd_write;
#endif
};

FOUNDATION_ALIGNED_STRUCT(stream_ringbuffer_t, 8) {
	FOUNDATION_DECLARE_STREAM;
	semaphore_t signal_read;
	semaphore_t signal_write;
	volatile int pending_read;
	volatile int pending_write;
	size_t total_size;
	FOUNDATION_DECLARE_RINGBUFFER;
};

struct stream_vtable_t {
	stream_read_fn read;
	stream_write_fn write;
	stream_eos_fn eos;
	stream_flush_fn flush;
	stream_truncate_fn truncate;
	stream_size_fn size;
	stream_seek_fn seek;
	stream_tell_fn tell;
	stream_lastmod_fn lastmod;
	stream_md5_fn md5;
	stream_buffer_read_fn buffer_read;
	stream_available_read_fn available_read;
	stream_finalize_fn finalize;
	stream_clone_fn clone;
};

#if FOUNDATION_COMPILER_CLANG
#  pragma clang diagnostic pop
#endif
