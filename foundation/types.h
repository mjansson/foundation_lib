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

#include <foundation/platform.h>
#include <foundation/build.h>


// PRIMITIVE TYPES

typedef enum _foundation_error_level
{
	ERRORLEVEL_NONE    = 0,
	ERRORLEVEL_DEBUG,
	ERRORLEVEL_INFO,
	ERRORLEVEL_WARNING,
	ERRORLEVEL_ERROR,
	ERRORLEVEL_PANIC
} error_level_t;

typedef enum
{
	ERROR_NONE              = 0,
	ERROR_INVALID_VALUE,
	ERROR_UNSUPPORTED,
	ERROR_NOT_IMPLEMENTED,
	ERROR_OUT_OF_MEMORY,
	ERROR_MEMORY_LEAK,
	ERROR_MEMORY_ALIGNMENT,
	ERROR_INTERNAL_FAILURE,
	ERROR_ACCESS_DENIED,
	ERROR_EXCEPTION,
	ERROR_SYSTEM_CALL_FAIL,
	ERROR_UNKNOWN_TYPE,
	ERROR_UNKNOWN_RESOURCE,
	ERROR_DEPRECATED,
	ERROR_ASSERT,
	ERROR_SCRIPT,
	ERROR_LAST_BUILTIN  = 0x0fff
} error_t;

typedef enum
{
	WARNING_PERFORMANCE = 0,
	WARNING_DEPRECATED,
	WARNING_BAD_DATA,
	WARNING_MEMORY,
	WARNING_UNSUPPORTED,
	WARNING_SUSPICIOUS,
	WARNING_SYSTEM_CALL_FAIL,
	WARNING_DEADLOCK,
	WARNING_SCRIPT,
	WARNING_LAST_BUILTIN  = 0x0fff
} warning_t;

typedef enum
{
	MEMORY_PERSISTENT          = 0x0000,
	MEMORY_TEMPORARY           = 0x0001,
	MEMORY_THREAD              = 0x0002,
	MEMORY_32BIT_ADDRESS       = 0x0004
} memory_hint_t;

typedef enum
{
	PLATFORM_WINDOWS   = 1,
	PLATFORM_LINUX,
	PLATFORM_MACOSX,
	PLATFORM_IOS,
	PLATFORM_ANDROID,
	PLATFORM_RASPBERRYPI
} platform_t;

typedef enum
{
	ARCHITECTURE_X86          = 0,
	ARCHITECTURE_X86_64       = 1,
	ARCHITECTURE_PPC          = 2,
	ARCHITECTURE_PPC_64       = 3,
	ARCHITECTURE_ARM6         = 6,
	ARCHITECTURE_ARM7         = 7,
	ARCHITECTURE_ARM8_64      = 8,
	ARCHITECTURE_ARM5         = 9,
	ARCHITECTURE_MIPS         = 10
} architecture_t;

typedef enum
{
	BYTEORDER_LITTLEENDIAN = 0,
	BYTEORDER_BIGENDIAN    = 1
} byteorder_t;

typedef enum
{
	APPLICATION_UTILITY        = 0x0001,
	APPLICATION_DAEMON         = 0x0002
} application_flag_t;

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
	STREAMTYPE_ASSET,
	STREAMTYPE_PIPE,
	STREAMTYPE_STDSTREAM
} stream_type_t;

typedef enum
{
	STREAM_SEEK_BEGIN          = 0x0000,
	STREAM_SEEK_CURRENT        = 0x0001,
	STREAM_SEEK_END            = 0x0002
} stream_seek_mode_t;

typedef enum
{
	THREAD_PRIORITY_LOW = 0,
	THREAD_PRIORITY_BELOWNORMAL,
	THREAD_PRIORITY_NORMAL,
	THREAD_PRIORITY_ABOVENORMAL,
	THREAD_PRIORITY_HIGHEST,
	THREAD_PRIORITY_TIMECRITICAL
} thread_priority_t;

typedef enum
{
	PROCESS_ATTACHED                          = 0,
	PROCESS_DETACHED                          = 0x01,
	PROCESS_CONSOLE                           = 0x02,
	PROCESS_STDSTREAMS                        = 0x04,
	PROCESS_WINDOWS_USE_SHELLEXECUTE          = 0x08,
	PROCESS_OSX_USE_OPENAPPLICATION           = 0x10
} process_flag_t;

typedef enum
{
	PROCESS_INVALID_ARGS                      = 0x7FFFFFF0,
	PROCESS_TERMINATED_SIGNAL                 = 0x7FFFFFF1,
	PROCESS_WAIT_INTERRUPTED                  = 0x7FFFFFF2,
	PROCESS_WAIT_FAILED                       = 0x7FFFFFF3,
	PROCESS_STILL_ACTIVE                      = 0x7FFFFFFF
} process_status_t;

typedef enum
{
	FOUNDATIONEVENT_START = 1,
	FOUNDATIONEVENT_TERMINATE,
	FOUNDATIONEVENT_PAUSE,
	FOUNDATIONEVENT_RESUME,
	FOUNDATIONEVENT_FILE_CREATED,
	FOUNDATIONEVENT_FILE_DELETED,
	FOUNDATIONEVENT_FILE_MODIFIED,
	FOUNDATIONEVENT_LOW_MEMORY_WARNING,
	FOUNDATIONEVENT_DEVICE_ORIENTATION
} foundation_event_id;

typedef enum
{
	EVENTFLAG_DELAY  = 1
} event_flag_t;

typedef enum
{
	BLOWFISH_ECB = 0,
	BLOWFISH_CBC,
	BLOWFISH_CFB,
	BLOWFISH_OFB
} blowfish_mode_t;

typedef enum
{
	RADIXSORT_INT32 = 0,
	RADIXSORT_UINT32,
	RADIXSORT_INT64,
	RADIXSORT_UINT64,
	RADIXSORT_FLOAT32,
	RADIXSORT_FLOAT64
} radixsort_data_t;

typedef enum
{
	DEVICEORIENTATION_UNKNOWN = 0,
	DEVICEORIENTATION_PORTRAIT,
	DEVICEORIENTATION_PORTRAIT_FLIPPED,
	DEVICEORIENTATION_LANDSCAPE_CCW,
	DEVICEORIENTATION_LANDSCAPE_CW,
	DEVICEORIENTATION_FACEUP,
	DEVICEORIENTATION_FACEDOWN
} device_orientation_t;

typedef uint64_t         hash_t;
typedef uint64_t         tick_t;
typedef real             deltatime_t;
typedef uint64_t         object_t;
typedef uint16_t         radixsort_index_t;
typedef uint128_t        uuid_t;

typedef int           (* error_callback_fn )( error_level_t level, error_t error );
typedef int           (* assert_handler_fn )( uint64_t context, const char* condition, const char* file, int line, const char* msg );
typedef void          (* log_callback_fn )( uint64_t context, int severity, const char* msg );
typedef int           (* system_initialize_fn )( void );
typedef void          (* system_shutdown_fn )( void );
typedef void*         (* memory_allocate_fn )( uint64_t context, uint64_t size, unsigned int align, int hint );
typedef void*         (* memory_allocate_zero_fn )( uint64_t context, uint64_t size, unsigned int align, int hint );
typedef void*         (* memory_reallocate_fn )( void* p, uint64_t size, unsigned int align, uint64_t oldsize );
typedef void          (* memory_deallocate_fn )( void* p );
typedef void          (* memory_track_fn )( void* p, uint64_t size );
typedef void          (* memory_untrack_fn )( void* p );
typedef void          (* profile_write_fn )( void* data, uint64_t size );
typedef void          (* profile_read_fn )( void* data, uint64_t size );
typedef void*         (* thread_fn )( object_t thread, void* arg );
typedef int           (* crash_guard_fn )( void* arg );
typedef void          (* crash_dump_callback_fn )( const char* file );


//! Identifier returned from threads and crash guards after a fatal exception (crash) has been caught
#define FOUNDATION_CRASH_DUMP_GENERATED        0x0badf00dL


// OPAQUE COMPLEX TYPES

typedef struct _foundation_md5              md5_t;
typedef struct _foundation_stream           stream_t;
typedef struct _foundation_mutex            mutex_t;
typedef struct _foundation_process          process_t;
typedef struct _foundation_event_block      event_block_t;
typedef struct _foundation_event_stream     event_stream_t;
typedef struct _foundation_ringbuffer       ringbuffer_t;
typedef struct _foundation_blowfish         blowfish_t;
typedef struct _foundation_radixsort        radixsort_t;
typedef struct _foundation_hashmap          hashmap_t;
typedef struct _foundation_hashtable32      hashtable32_t;
typedef struct _foundation_hashtable64      hashtable64_t;
typedef struct _foundation_regex            regex_t;


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
typedef struct dispatch_semaphore_s* semaphore_t;
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

//! Regex capture
typedef struct _foundation_regex_capture
{
	const char*                      substring;
	int                              length;
} regex_capture_t;


// UTILITY FUNCTIONS

/*! Pack version definition into a 128-bit integer
    \param major          Major version
    \param minor          Minor version
    \param revision       Revision number
	\param build          Build number
	\param control        Source control version/revision/identifier
    \return               Packed version data */
static FORCEINLINE CONSTCALL version_t      version_make( unsigned int major, unsigned int minor, unsigned int revision, unsigned int build, unsigned int control ) { version_t v; v.sub.major = (uint16_t)major; v.sub.minor = (uint16_t)minor; v.sub.revision = revision, v.sub.build = build; v.sub.control = control; return v; }
