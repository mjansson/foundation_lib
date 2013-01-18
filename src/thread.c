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

#include <foundation.h>

#if FOUNDATION_PLATFORM_WINDOWS
#  include <safewindows.h>
typedef DWORD (WINAPI* GetCurrentProcessorNumberFn)(VOID);
DWORD WINAPI GetCurrentProcessorNumberFallback(VOID) { return 0; }
GetCurrentProcessorNumberFn _fnGetCurrentProcessorNumber = GetCurrentProcessorNumberFallback;
#endif

#if FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_ANDROID

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
	
	warn_logf( WARNING_MEMORY, "Unable to locate thread local memory block slot, will leak %d bytes", size );
	return block;
}

#endif


int _thread_initialize( void )
{
#if FOUNDATION_PLATFORM_WINDOWS
	//TODO: look into GetCurrentProcessorNumberEx for 64+ core support
	GetCurrentProcessorNumberFn getprocidfn = (GetCurrentProcessorNumberFn)GetProcAddress( GetModuleHandleA( "kernel32" ), "GetCurrentProcessorNumber" );
	if( getprocidfn )
		_fnGetCurrentProcessorNumber = getprocidfn;
#endif
	return 0;
}


void _thread_shutdown( void )
{
#if FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS
	for( int i = 0; i < 1024; ++i )
	{
		if( _thread_local_blocks[i].block )
			memory_deallocate( _thread_local_blocks[i].block );
	}
#endif

	thread_cleanup();
}


void thread_sleep( int milliseconds )
{
#if FOUNDATION_PLATFORM_WINDOWS
	SleepEx( milliseconds, 1 );
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_ANDROID || FOUNDATION_PLATFORM_IOS
	struct timespec t;
	t.tv_sec  = milliseconds / 1000;
	t.tv_nsec = (long)( milliseconds % 1000 ) * 1000000L;
	nanosleep( &t, 0 );
#else
#  error Not implemented
#endif
}


void thread_yield( void )
{
#if FOUNDATION_PLATFORM_WINDOWS
	Sleep(0);
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_ANDROID || FOUNDATION_PLATFORM_IOS
	sched_yield();
#else
#  error Not implemented
#endif
}


uint64_t thread_id( void )
{
#if FOUNDATION_PLATFORM_WINDOWS
	return GetCurrentThreadId();
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
	return pthread_self();
#elif FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS
	return pthread_mach_thread_np( pthread_self() );
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


void thread_cleanup( void )
{
#if FOUNDATION_PLATFORM_ANDROID
	thread_detach_jvm();
#endif

	error_context_thread_deallocate();
	
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
	jint result = (*_global_app->activity->vm)->AttachCurrentThread( _global_app->activity->vm, &_global_app->activity->env, &attach_args );
	if( result < 0 )
		warn_logf( WARNING_SYSTEM_CALL_FAIL, "Unable to attach thread to Java VM (%d)", result );
	else
		set_thread_jvm_attached( true );
}


void thread_detach_jvm( void )
{
	if( get_thread_jvm_attached() )
	{
		JavaVM* java_vm = _global_app->activity->vm;
		(*java_vm)->DetachCurrentThread( java_vm );

		set_thread_jvm_attached( false );
	}
}

#endif
