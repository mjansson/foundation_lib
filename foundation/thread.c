/* thread.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <foundation/foundation.h>

#if FOUNDATION_PLATFORM_WINDOWS
#  include <foundation/windows.h>
typedef DWORD (WINAPI* GetCurrentProcessorNumberFn)(VOID);
DWORD WINAPI GetCurrentProcessorNumberFallback(VOID) { return 0; }
GetCurrentProcessorNumberFn _fnGetCurrentProcessorNumber = GetCurrentProcessorNumberFallback;
#elif FOUNDATION_PLATFORM_POSIX
#  if !FOUNDATION_PLATFORM_APPLE
#    include <sys/prctl.h>
#  endif
#  include <pthread.h>
#endif

#if FOUNDATION_PLATFORM_ANDROID
#  include <foundation/android.h>
#endif

#if FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_ANDROID || ( FOUNDATION_PLATFORM_WINDOWS && FOUNDATION_COMPILER_CLANG )

typedef struct _foundation_thread_local_block
{
	uint64_t     thread;
	void*        block;
} thread_local_block_t;

//TODO: Ugly hack, improve this shit
static thread_local_block_t _thread_local_blocks[1024] = {{0}};

void* _allocate_thread_local_block( unsigned int size )
{
	void* block = memory_allocate_zero( size, 0, MEMORY_PERSISTENT );
	
	for( int i = 0; i < 1024; ++i )
	{
		if( !_thread_local_blocks[i].block )
		{
			if( atomic_cas_ptr( &_thread_local_blocks[i].block, block, 0 ) )
			{
				_thread_local_blocks[i].thread = thread_id();
				return block;
			}
		}
	}
	
	log_warnf( WARNING_MEMORY, "Unable to locate thread local memory block slot, will leak %d bytes", size );
	return block;
}

#endif


typedef struct ALIGN(16) _foundation_thread
{
	FOUNDATION_DECLARE_OBJECT;

	volatile int32_t      started; //Aligned to 16 bytes for atomic
	volatile int32_t      running;
	volatile int32_t      terminate;
	uint32_t              stacksize;
	thread_fn             fn;
	char                  name[32];
	thread_priority_t     priority;
	void*                 arg;
	void*                 result;
	uint64_t              osid;

#if FOUNDATION_PLATFORM_WINDOWS
	HANDLE                handle;
#elif FOUNDATION_PLATFORM_POSIX
	pthread_t             thread;
#else
#  error Not implemented
#endif
} thread_t;

static uint64_t     _thread_main_id = 0;
static objectmap_t* _thread_map = 0;

#define GET_THREAD( obj ) objectmap_lookup( _thread_map, obj )

FOUNDATION_DECLARE_THREAD_LOCAL( const char*, name, 0 )
FOUNDATION_DECLARE_THREAD_LOCAL( thread_t*, self, 0 )


int _thread_initialize( void )
{
#if FOUNDATION_PLATFORM_WINDOWS
	//TODO: look into GetCurrentProcessorNumberEx for 64+ core support
	GetCurrentProcessorNumberFn getprocidfn = (GetCurrentProcessorNumberFn)GetProcAddress( GetModuleHandleA( "kernel32" ), "GetCurrentProcessorNumber" );
	if( getprocidfn )
		_fnGetCurrentProcessorNumber = getprocidfn;
#endif

	_thread_map = objectmap_allocate( BUILD_SIZE_THREAD_MAP );

	return 0;
}


void _thread_shutdown( void )
{
	objectmap_deallocate( _thread_map );

#if FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS
	for( int i = 0; i < 1024; ++i )
	{
		if( _thread_local_blocks[i].block )
			memory_deallocate( _thread_local_blocks[i].block );
	}
#endif

	thread_cleanup();
}


void _thread_destroy( void* thread_raw )
{
	thread_t* thread = thread_raw;

	if( thread_is_running( thread->id ) )
	{
		unsigned int spin_count = 0;
		FOUNDATION_ASSERT_FAIL( "Destroying running thread" );
		thread_terminate( thread->id );
		while( thread_is_running( thread->id ) && ( ++spin_count < 50 ) )
			thread_yield();
	}
	objectmap_free( _thread_map, thread->id );
	memory_deallocate( thread );
}


static FORCEINLINE void _thread_dec_ref( thread_t* thread )
{
	if( atomic_decr32( &thread->ref ) <= 0 )
		_thread_destroy( thread );
}


static int _thread_guard_wrapper( void* data )
{
	thread_t* thread = data;
	FOUNDATION_ASSERT( thread->running == 1 );
	thread->result = thread->fn( thread->id, thread->arg );
	return 0;
}


object_t thread_create( thread_fn fn, const char* name, thread_priority_t priority, unsigned int stacksize )
{
	thread_t* thread;
	uint64_t id = objectmap_reserve( _thread_map );
	if( !id )
	{
		log_errorf( ERROR_OUT_OF_MEMORY, "Unable to allocate new thread, map full" );	
		return 0;
	}
	thread = memory_allocate_zero( sizeof( thread_t ), 0, MEMORY_PERSISTENT );
	thread->id = id;
	thread->fn = fn;
	string_copy( thread->name, name, 32 );
	thread->running = 0;
	thread->priority = priority;
	thread->stacksize = stacksize;
	thread->ref = 1;
	objectmap_set( _thread_map, id, thread );
	return thread->id;
}


void thread_destroy( object_t id )
{
	thread_t* thread = GET_THREAD( id );
	if( thread )
		_thread_dec_ref( thread );
}


bool thread_is_started( object_t id )
{
	thread_t* thread = GET_THREAD( id );
	return ( thread && ( thread->started > 0 ) );
}


bool thread_is_running( object_t id )
{
	thread_t* thread = GET_THREAD( id );
	return ( thread && ( thread->running > 0 ) );
}


bool thread_is_thread( object_t id )
{
	thread_t* thread = GET_THREAD( id );
	return ( thread != 0 );
}


void* thread_result( object_t id )
{
	thread_t* thread = GET_THREAD( id );
	return ( !thread || !thread->started || thread->running ) ? 0 : thread->result;
}


const char* thread_name( void )
{
	return get_thread_name();
}


#if FOUNDATION_PLATFORM_WINDOWS
const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
void _set_thread_name( uint64_t threadid, const char* threadname )
{
   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName = threadname;
   info.dwThreadID = (DWORD)threadid;
   info.dwFlags = 0;
   __try
   {
      RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
   }
   __except(EXCEPTION_CONTINUE_EXECUTION) //EXCEPTION_EXECUTE_HANDLER seems to require a debugger present...
   {
   }
}
#endif

void thread_set_name( const char* name )
{
	thread_t* self;

#if !BUILD_DEPLOY
#  if FOUNDATION_PLATFORM_WINDOWS
	_set_thread_name( thread_id(), name );
#  elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
	if( !thread_is_main() ) //Don't set main thread (i.e process) name
		prctl( PR_SET_NAME, name, 0, 0, 0 );
#  elif FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS
	if( !thread_is_main() ) //Don't set main thread (i.e process) name
        pthread_setname_np( name );
#  endif
#endif

	set_thread_name( name );

	self = get_thread_self();
	if( self )
	{
#if !BUILD_DEPLOY
		thread_t* check_self = GET_THREAD( self->id );
		FOUNDATION_ASSERT( self == check_self );
#endif
		string_copy( self->name, name, 32 );
	}
}


object_t thread_self( void )
{
	thread_t* self = get_thread_self();
	return self ? self->id : 0;
}


#if FOUNDATION_PLATFORM_WINDOWS

typedef DWORD thread_return_t;
typedef void* thread_arg_t;
#define FOUNDATION_THREADCALL WINAPI
#define GET_THREAD_PTR(x) ((thread_t*)(x))

#elif FOUNDATION_PLATFORM_POSIX

typedef void* thread_return_t;
typedef void* thread_arg_t;
#define FOUNDATION_THREADCALL
#define GET_THREAD_PTR(x) ((thread_t*)(x))

#else
#  error Not implemented
#endif

thread_return_t FOUNDATION_THREADCALL _thread_entry( thread_arg_t data )
{
	uint64_t thr_osid;
	uint64_t thr_id;
	thread_t* thread = GET_THREAD_PTR( data );

	atomic_incr32( &thread->ref );
	atomic_cas32( &thread->started, 1, 0 );
	if( !atomic_cas32( &thread->running, 1, 0 ) )
	{
		log_warnf( WARNING_SUSPICIOUS, "Unable to enter thread %llx, already running", thread->id );
		_thread_dec_ref( thread );
		return 0;
	}

#if FOUNDATION_PLATFORM_WINDOWS
	thread->osid = GetCurrentThreadId();
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
	pthread_t curid = pthread_self();
	thread->osid = curid;
#if !BUILD_DEPLOY
	prctl( PR_SET_NAME, thread->name, 0, 0, 0 );
#endif
#elif FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS
	mach_port_t curid = pthread_mach_thread_np( pthread_self() );
	thread->osid = curid;
#else
	#error Not implemented
#endif
	thread->terminate = 0;

	set_thread_self( thread );

	FOUNDATION_ASSERT( thread->running == 1 );

	log_debugf( "Started thread '%s' (%llx) ID %llx", thread->name, thread->osid, thread->id );

	if( crash_guard_callback() )
	{
		int crash_result = crash_guard( _thread_guard_wrapper, thread, crash_guard_callback(), crash_guard_name() );
		if( crash_result == CRASH_DUMP_GENERATED )
		{
			thread->result = (void*)((uintptr_t)CRASH_DUMP_GENERATED);
			log_warnf( WARNING_SUSPICIOUS, "Thread '%s' (%llx) ID %llx crashed", thread->name, thread->osid, thread->id );
		}
	}
	else
	{
		thread->result = thread->fn( thread->id, thread->arg );
	}

	thr_osid = thread->osid;
	thr_id = thread->id;
	log_debugf( "Terminated thread '%s' (%llx) ID %llx with %d refs", thread->name, thr_osid, thr_id, thread->ref );

	thread->osid  = 0;

	set_thread_self( 0 );
	thread_cleanup();

	if( !atomic_cas32( &thread->running, 0, 1 ) )
	{
		FOUNDATION_ASSERT_FAIL( "Unable to reset running flag" );
		thread->running = 0;
	}

	log_debugf( "Exiting thread '%s' (%llx) ID %llx with %d refs", thread->name, thr_osid, thr_id, thread->ref );

	_thread_dec_ref( thread );

	return 0;
}


bool thread_start( object_t id, void* data )
{
#if FOUNDATION_PLATFORM_WINDOWS
	unsigned long osid = 0;
#endif
	thread_t* thread = GET_THREAD( id );
	if( !thread )
	{
		log_errorf( ERROR_INVALID_VALUE, "Unable to start thread %llx, invalid id", id );
		return false; //Old/invalid id
	}

	if( thread->running > 0 )
	{
		log_warnf( WARNING_SUSPICIOUS, "Unable to start thread %llx, already running", id );
		return false; //Thread already running
	}

	atomic_cas32( &thread->started, 0, 1 );

	thread->arg = data;

	if( !thread->stacksize )
		thread->stacksize = BUILD_SIZE_DEFAULT_THREAD_STACK;

#if FOUNDATION_PLATFORM_WINDOWS
	thread->handle = CreateThread( 0, thread->stacksize, _thread_entry, thread, 0, &osid );
	if( !thread->handle )
	{
		log_errorf( ERROR_OUT_OF_MEMORY, "Unable to create thread: CreateThread failed: %s", system_error_message( GetLastError() ) );
		return false;
	}
#if !BUILD_DEPLOY
	_set_thread_name( osid, thread->name );
#endif
#elif FOUNDATION_PLATFORM_POSIX
	int err = pthread_create( &thread->thread, 0, _thread_entry, thread );
	if( err )
	{
		log_errorf( ERROR_OUT_OF_MEMORY, "Unable to create thread: pthread_create failed: %s", system_error_message( err ) );
		return false;
	}
#else
#  error Not implemented
#endif

	return true;
}


void thread_terminate( object_t id )
{
	thread_t* thread = GET_THREAD( id );
	if( thread )
		thread->terminate = 1;
}


bool thread_should_terminate( object_t id )
{
	thread_t* thread = GET_THREAD( id );
	if( thread )
		return thread->terminate > 0;
	return true;
}


void thread_sleep( int milliseconds )
{
#if FOUNDATION_PLATFORM_WINDOWS
	SleepEx( milliseconds, 1 );
#elif FOUNDATION_PLATFORM_POSIX
	struct timespec ts;
	ts.tv_sec  = milliseconds / 1000;
	ts.tv_nsec = (long)( milliseconds % 1000 ) * 1000000L;
	nanosleep( &ts, 0 );
#else
#  error Not implemented
#endif
}


void thread_yield( void )
{
#if FOUNDATION_PLATFORM_WINDOWS
	Sleep(0);
#elif FOUNDATION_PLATFORM_POSIX
	sched_yield();
#else
#  error Not implemented
#endif
}


uint64_t thread_id( void )
{
#if FOUNDATION_PLATFORM_WINDOWS
	return GetCurrentThreadId();
#elif FOUNDATION_PLATFORM_APPLE
	return pthread_mach_thread_np( pthread_self() );
#elif FOUNDATION_PLATFORM_POSIX
	return pthread_self();
#else
#  error Not implemented
#endif
}


unsigned int thread_hardware( void )
{
#if FOUNDATION_PLATFORM_WINDOWS
	return _fnGetCurrentProcessorNumber();
#elif FOUNDATION_PLATFORM_LINUX
	return sched_getcpu();
#else
	//TODO: Implement for other platforms
	return 0;
#endif
}


void thread_set_hardware( unsigned int hw_thread )
{
}


void thread_set_main( void )
{
	_thread_main_id = thread_id();
}


bool thread_is_main( void )
{
	return thread_id() == _thread_main_id;
}


void thread_cleanup( void )
{
	random_thread_deallocate();

#if FOUNDATION_PLATFORM_ANDROID
	thread_detach_jvm();
#endif

	error_context_thread_deallocate();
	memory_context_thread_deallocate();

#if FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS
	uint64_t curid = thread_id();
	for( int i = 0; i < 1024; ++i )
	{
		if( ( _thread_local_blocks[i].thread == curid ) && _thread_local_blocks[i].block )
		{
			memory_deallocate( _thread_local_blocks[i].block );
			_thread_local_blocks[i].thread = 0;
			_thread_local_blocks[i].block = 0;
		}
	}
#endif	
}


#if FOUNDATION_PLATFORM_ANDROID

#include <android_native_app_glue.h>

#include <android/native_activity.h>

FOUNDATION_DECLARE_THREAD_LOCAL( bool, jvm_attached, false )


void thread_attach_jvm( void )
{
	if( get_thread_jvm_attached() )
		return;

	JavaVMAttachArgs attach_args;
	attach_args.version = JNI_VERSION_1_6;
	attach_args.name = "NativeThread";
	attach_args.group = 0;

	// Attaches the current thread to the JVM
	// TODO: According to the native activity, the java env can only be used in the main thread (calling ANativeActivityCallbacks)
	struct android_app* app = android_app();
	jint result = (*app->activity->vm)->AttachCurrentThread( app->activity->vm, &app->activity->env, &attach_args );
	if( result < 0 )
		log_warnf( WARNING_SYSTEM_CALL_FAIL, "Unable to attach thread to Java VM (%d)", result );
	else
		set_thread_jvm_attached( true );
}


void thread_detach_jvm( void )
{
	if( get_thread_jvm_attached() )
	{
		JavaVM* java_vm = android_app()->activity->vm;
		(*java_vm)->DetachCurrentThread( java_vm );

		set_thread_jvm_attached( false );
	}
}

#endif
