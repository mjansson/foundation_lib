/* semaphore.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
#elif FOUNDATION_PLATFORM_MACOSX 
#  include <sys/fcntl.h>
#  include <sys/semaphore.h>
#  include <errno.h>
extern int MPCreateSemaphore( unsigned long, unsigned long, MPSemaphoreID* );
extern int MPDeleteSemaphore( MPSemaphoreID );
extern int MPSignalSemaphore( MPSemaphoreID );
extern int MPWaitOnSemaphore( MPSemaphoreID, int );
#elif FOUNDATION_PLATFORM_IOS
#  include <dispatch/dispatch.h>
#  include <errno.h>
#elif FOUNDATION_PLATFORM_POSIX
#  include "/usr/include/time.h"
#  include "/usr/include/semaphore.h"
#  include <sys/fcntl.h>
#  define native_sem_t sem_t
#endif


#if FOUNDATION_PLATFORM_WINDOWS


void semaphore_initialize( semaphore_t* semaphore, int value )
{
	*semaphore = CreateSemaphoreA( 0, value, 0xFFFF, 0 );
}


void semaphore_initialize_named( semaphore_t* semaphore, const char* name, int value )
{
	FOUNDATION_ASSERT( name );
	*semaphore = CreateSemaphoreA( 0, value, 0xFFFF, name );
}


void semaphore_destroy( semaphore_t* semaphore )
{
	CloseHandle( (HANDLE)*semaphore );
}


void semaphore_wait( semaphore_t* semaphore )
{
	WaitForSingleObject( (HANDLE)*semaphore, INFINITE );
}


bool semaphore_try_wait( semaphore_t* semaphore, int milliseconds )
{
	DWORD res = WaitForSingleObject( (HANDLE)*semaphore, milliseconds > 0 ? milliseconds : 0 );
	return ( res == WAIT_OBJECT_0 );
}


void semaphore_post( semaphore_t* semaphore )
{
	ReleaseSemaphore( (HANDLE)*semaphore, 1, 0 );
}


#elif FOUNDATION_PLATFORM_MACOSX

//OSX:
//unnamed - MPCreateSemaphore (for wait until), should be ported to dispatch_semaphore_t if 10.6+
//named - sem_open

void semaphore_initialize( semaphore_t* semaphore, int value )
{
	semaphore->name = 0;

	int ret = MPCreateSemaphore( 0xFFFF, value, &semaphore->sem.unnamed );
	if( ret < 0 )
	{
		log_errorf( ERRORLEVEL_ERROR, ERROR_SYSTEM_CALL_FAIL, "Unable to initialize unnamed semaphore: %s", system_error_message( 0 ) );
		FOUNDATION_ASSERT_FAIL( "Unable to initialize unnamed semaphore" );
		return;
	}
}


void semaphore_initialize_named( semaphore_t* semaphore, const char* name, int value )
{
	semaphore->name = string_clone( name );

	sem_t* sem = SEM_FAILED;
	
	sem = sem_open( name, O_CREAT, 0666, value );
		
	if( sem == SEM_FAILED )
	{
		log_errorf( ERRORLEVEL_ERROR, ERROR_SYSTEM_CALL_FAIL, "Unable to initialize named semaphore (sem_open '%s'): %s", name, system_error_message( 0 ) );
		FOUNDATION_ASSERT_FAIL( "Unable to initialize semaphore (sem_open)" );
	}

	semaphore->sem.named = sem;
}


void semaphore_destroy( semaphore_t* semaphore )
{
	if( !semaphore->name )
	{
		MPDeleteSemaphore( semaphore->sem.unnamed );
	}
	else
	{
		sem_destroy( semaphore->sem.named );
		sem_unlink( semaphore->name );
		string_deallocate( semaphore->name );
	}
}


void semaphore_wait( semaphore_t* semaphore )
{
	if( !semaphore->name )
	{
		MPWaitOnSemaphore( semaphore->sem.unnamed, 0x7FFFFFFF/*kDurationForever*/ );
	}
	else
	{
		int ret = sem_wait( semaphore->sem.named );
#if !BUILD_DEPLOY
		if( ret != 0 )
		{
			//Don't report error if interrupted
			if( errno != EINTR )
				log_errorf( ERRORLEVEL_ERROR, ERROR_SYSTEM_CALL_FAIL, "Unable to wait for semaphore: %s (%d)", system_error_message( 0 ) );
			else
			{
				info_logf( "Semaphore wait interrupted by signal" );
				ret = 0;
			}
		}
#endif
		FOUNDATION_ASSERT( ( ret == 0 ) || ( errno == EINTR ) );
	}
}


bool semaphore_try_wait( semaphore_t* semaphore, int milliseconds )
{
	if( !semaphore->name )
	{
		int duration = 0/*kDurationImmediate*/;
		if( milliseconds > 0 )
			duration = 1/*kDurationMillisecond*/ * milliseconds;
		int ret = MPWaitOnSemaphore( semaphore->sem.unnamed, duration );
		if( ret < 0 )
			return false;
		return true;
	}
	else
	{
		//TODO: Proper implementation (sem_timedwait not supported)
		if( milliseconds > 0 )
		{
			time_t wakeup = timer_current() + ( ( (uint64_t)milliseconds * timer_ticks_per_second() ) / 1000ULL );
			do
			{
				if( sem_trywait( semaphore->sem.named ) == 0 )
					return true;
				thread_yield();
			} while( time_current() < wakeup );
			return false;
		}
		return sem_trywait( semaphore->sem.named ) == 0;
	}
}


void semaphore_post( semaphore_t* semaphore )
{
	if( !semaphore->name )
	{
		MPSignalSemaphore( semaphore->sem.unnamed );
	}
	else
	{
		sem_post( semaphore->sem.named );
	}
}


#elif FOUNDATION_PLATFORM_IOS

//IOS:
//unnamed - dispatch_semaphore_t
//named - UNSUPPORTED

void semaphore_initialize( semaphore_t* semaphore, int value )
{
	*semaphore = dispatch_semaphore_create( value );
	if( !*semaphore )
	{
		FOUNDATION_ASSERT_FAIL( "Unable to initialize semaphore" );
		log_errorf( ERRORLEVEL_ERROR, ERROR_SYSTEM_CALL_FAIL, "Unable to initialize semaphore: %s", system_error_message( 0 ) );
	}
}


void semaphore_initialize_named( semaphore_t* semaphore, const char* name, int value )
{
	 FOUNDATION_ASSERT_FAIL( "Named semaphores not supported on this platform" );
}


void semaphore_destroy( semaphore_t* semaphore )
{
	if( *semaphore )
		dispatch_release( *semaphore );
}


void semaphore_wait( semaphore_t* semaphore )
{
	dispatch_semaphore_wait( *semaphore, DISPATCH_TIME_FOREVER );
}


bool semaphore_try_wait( semaphore_t* semaphore, int milliseconds )
{
	long result = dispatch_semaphore_wait( *semaphore, ( milliseconds > 0 ) ? dispatch_time( DISPATCH_TIME_NOW, 1000000LL * (int64_t)milliseconds ) : DISPATCH_TIME_NOW );
	return ( result == 0 );
}


void semaphore_post( semaphore_t* semaphore )
{
	dispatch_semaphore_signal( *semaphore );
}


#elif FOUNDATION_PLATFORM_POSIX


void semaphore_initialize( semaphore_t* semaphore, int value )
{
	semaphore->name = 0;

	if( sem_init( (native_sem_t*)&semaphore->unnamed, 0, value ) )
	{
		FOUNDATION_ASSERT_FAIL( "Unable to initialize semaphore" );
		log_errorf( ERRORLEVEL_ERROR, ERROR_SYSTEM_CALL_FAIL, "Unable to initialize semaphore: %s", system_error_message( 0 ) );
	}

	semaphore->sem = &semaphore->unnamed;
}


void semaphore_initialize_named( semaphore_t* semaphore, const char* name, int value )
{
	semaphore->name = string_clone( name );

	native_sem_t* sem = SEM_FAILED;
	
	sem = sem_open( name, O_CREAT, 0666, value );
		
	if( sem == SEM_FAILED )
	{
		log_errorf( ERRORLEVEL_ERROR, ERROR_SYSTEM_CALL_FAIL, "Unable to initialize named semaphore (sem_open '%s'): %s", name, system_error_message( 0 ) );
		FOUNDATION_ASSERT_FAIL( "Unable to initialize semaphore (sem_open)" );
	}

	semaphore->sem = (semaphore_native_t*)sem;
}


void semaphore_destroy( semaphore_t* semaphore )
{
	sem_destroy( (native_sem_t*)semaphore->sem );

	if( semaphore->name )
	{
		sem_unlink( semaphore->name );
		string_deallocate( semaphore->name );
	}
}


void semaphore_wait( semaphore_t* semaphore )
{
	sem_wait( (native_sem_t*)semaphore->sem );
}


bool semaphore_try_wait( semaphore_t* semaphore, int milliseconds )
{
	if( milliseconds > 0 )
	{
		struct timespec ts;
		ts.tv_sec = milliseconds / 1000;
		ts.tv_nsec = (long)( milliseconds % 1000 ) * (long)1000000;
		return sem_timedwait( (native_sem_t*)semaphore->sem, &ts ) == 0;
	}
	return sem_trywait( (native_sem_t*)semaphore->sem ) == 0;
}


void semaphore_post( semaphore_t* semaphore )
{
	sem_post( (native_sem_t*)semaphore->sem );
}


#else
#  error Not implemented
#endif

