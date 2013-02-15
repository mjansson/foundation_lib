/* mutex.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
#elif FOUNDATION_PLATFORM_POSIX
#  include <time.h>
#  include <pthread.h>
#  include <sys/time.h>
#endif


struct ALIGN(16) _foundation_mutex
{
#if !BUILD_DEPLOY
	char                   name[32];
#endif

#if FOUNDATION_PLATFORM_WINDOWS

	//! Critical section
	unsigned char          csection[32];

	//! Event handle
	void*                  event;

	//! Wait count
	volatile int           waiting;

#elif FOUNDATION_PLATFORM_POSIX

	//! Mutex object
	pthread_mutex_t        mutex;

	//! Condition object
	pthread_cond_t         cond;

	//! Pending signal
	volatile bool          pending;

#else
#  error Not implemented	
#endif		
};


static void _mutex_initialize( mutex_t* mutex, const char* name )
{
#if !BUILD_DEPLOY
	unsigned int namelen = string_length( name );
	if( namelen > 31 )
		namelen = 31;
	string_copy( mutex->name, name, 32 );
#endif

#if FOUNDATION_PLATFORM_WINDOWS
	InitializeCriticalSectionAndSpinCount( (CRITICAL_SECTION*)mutex->csection, 4000 );
	mutex->event = CreateEvent( 0, TRUE, FALSE, 0 );
	mutex->waiting = 0;
#elif FOUNDATION_PLATFORM_POSIX
	mutex->pending = false;
	
	pthread_mutexattr_t attr;
	pthread_mutexattr_init( &attr );
	pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
	
	pthread_cond_init( &mutex->cond, 0 );
	pthread_mutex_init( &mutex->mutex, &attr );
	
	pthread_mutexattr_destroy( &attr );
#else
#  error _mutex_initialize not implemented
#endif
}


static void _mutex_shutdown( mutex_t* mutex )
{
#if FOUNDATION_PLATFORM_WINDOWS
	CloseHandle( mutex->event );
	DeleteCriticalSection( (CRITICAL_SECTION*)mutex->csection );
#elif FOUNDATION_PLATFORM_POSIX
	pthread_mutex_destroy( &mutex->mutex );
	pthread_cond_destroy( &mutex->cond );
#else
#  error mutex_deallocate not implemented
#endif
}


mutex_t* mutex_allocate( const char* name )
{
	mutex_t* mutex = memory_allocate_zero( sizeof( mutex_t ), 16, MEMORY_PERSISTENT );

	_mutex_initialize( mutex, name );

	return mutex;
}


void mutex_deallocate( mutex_t* mutex )
{
	if( !mutex )
		return;

	_mutex_shutdown( mutex );

	memory_deallocate( mutex );
}


const char* mutex_name( mutex_t* mutex )
{
	FOUNDATION_ASSERT( mutex );
#if !BUILD_DEPLOY
	return mutex->name;
#else
	return "";
#endif
}


bool mutex_try_lock( mutex_t* mutex )
{
	bool was_locked = false;
	FOUNDATION_ASSERT( mutex );
#if FOUNDATION_PLATFORM_WINDOWS
	was_locked = TryEnterCriticalSection( (CRITICAL_SECTION*)mutex->csection );
#elif FOUNDATION_PLATFORM_POSIX
	was_locked = ( pthread_mutex_trylock( &mutex->mutex ) == 0 );
#else
#  error mutex_try_lock not implemented
#endif
#if !BUILD_DEPLOY
	if( was_locked )
		profile_lock( mutex->name );
#endif
	return was_locked;
}


bool mutex_lock( mutex_t* mutex )
{
	FOUNDATION_ASSERT( mutex );
#if !BUILD_DEPLOY
	profile_trylock( mutex->name );
#endif
#if FOUNDATION_PLATFORM_WINDOWS
	EnterCriticalSection( (CRITICAL_SECTION*)mutex->csection );
#elif FOUNDATION_PLATFORM_POSIX
	if( pthread_mutex_lock( &mutex->mutex ) < 0 )
	{
		FOUNDATION_ASSERT_FAILFORMAT( "unable to lock mutex %s", mutex->name );
		return false;
	}
#else
#  error mutex_lock not implemented
#endif
#if !BUILD_DEPLOY
	profile_lock( mutex->name );
#endif
	return true;
}


bool mutex_unlock( mutex_t* mutex )
{
	FOUNDATION_ASSERT( mutex );
#if !BUILD_DEPLOY
	profile_unlock( mutex->name );
#endif
#if FOUNDATION_PLATFORM_WINDOWS
	LeaveCriticalSection( (CRITICAL_SECTION*)mutex->csection );
#elif FOUNDATION_PLATFORM_POSIX
	if( pthread_mutex_unlock( &mutex->mutex ) < 0 )
	{
		FOUNDATION_ASSERT_FAILFORMAT( "unable to unlock mutex %s", mutex->name );
		return false;
	}
#else
#  error mutex_unlock not implemented
#endif
	return true;
}


bool mutex_wait( mutex_t* mutex, unsigned int timeout )
{
#if FOUNDATION_PLATFORM_WINDOWS
	DWORD ret;
#elif FOUNDATION_PLATFORM_POSIX
	struct timeval now;
	struct timespec then;
#endif	
	FOUNDATION_ASSERT( mutex );
#if FOUNDATION_PLATFORM_WINDOWS

	{
		//MutexLock mlock( *this );
		atomic_incr32( &mutex->waiting );
	}
	ret = WaitForSingleObject( mutex->event, ( timeout == 0 ) ? INFINITE : timeout );
	{
		//MutexLock lock( *this );
		if( atomic_decr32( &mutex->waiting ) == 0 ) //TODO: not safe, could potentially be thread incrementing and calling SetEvent between return of atomic_decr32 and ResetEvent
			ResetEvent( mutex->event );
	}

	return ret == WAIT_OBJECT_0;

#elif FOUNDATION_PLATFORM_POSIX
	
	mutex_lock( mutex );
	
	if( mutex->pending )
	{
		mutex->pending = false;
		mutex_unlock( mutex );
		return true;
	}
	
	bool was_signal = false;
	if( !timeout )
	{
		if( !pthread_cond_wait( &mutex->cond, &mutex->mutex ) )
			was_signal = true;
	}
	else
	{
		gettimeofday( &now, 0 );
		then.tv_sec  = now.tv_sec + ( timeout / 1000 );
		then.tv_nsec = ( now.tv_usec * 1000 ) + (long)( timeout % 1000 ) * 1000000L;
		while( then.tv_nsec > 999999999 )
		{
			++then.tv_sec;
			then.tv_nsec -= 1000000000L;
		}
		if( !pthread_cond_timedwait( &mutex->cond, &mutex->mutex, &then ) )
			was_signal = true;
	}
	
	mutex->pending = false;
	mutex_unlock( mutex );
	
	return was_signal;

#else
#  error mutex_wait not implemented
#endif
}


void mutex_signal( mutex_t* mutex )
{
	FOUNDATION_ASSERT( mutex );
#if FOUNDATION_PLATFORM_WINDOWS
	SetEvent( mutex->event );
#elif FOUNDATION_PLATFORM_POSIX
	mutex_lock( mutex );
	mutex->pending = true;
	pthread_cond_broadcast( &mutex->cond );
	mutex_unlock( mutex );
#else
#  error mutex_signal not implemented
#endif
}


#if FOUNDATION_PLATFORM_WINDOWS

void* mutex_event_object( mutex_t* mutex )
{
	FOUNDATION_ASSERT( mutex );
	return mutex->event;
}

#endif

