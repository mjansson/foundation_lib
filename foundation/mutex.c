/* mutex.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#include <foundation/foundation.h>

#if FOUNDATION_PLATFORM_WINDOWS
#include <foundation/windows.h>
#elif FOUNDATION_PLATFORM_POSIX
#include <foundation/posix.h>
#endif

struct FOUNDATION_ALIGN(16) mutex_t {
#if FOUNDATION_PLATFORM_WINDOWS
	CRITICAL_SECTION csection;
	void* event;
	atomic32_t waiting;
#elif FOUNDATION_PLATFORM_POSIX
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	volatile bool pending;
#else
#error Not implemented
#endif
	volatile int lockcount;
	uint64_t lockedthread;
	char name_buffer[32];
	string_const_t name;
};

static void
mutex_initialize(mutex_t* mutex, const char* name, size_t length) {
	mutex->name = string_to_const(string_copy(mutex->name_buffer, 32, name, length));

#if FOUNDATION_PLATFORM_WINDOWS
	InitializeCriticalSectionAndSpinCount(&mutex->csection, 4000);
	mutex->event = CreateEvent(0, TRUE, FALSE, 0);
	atomic_store32(&mutex->waiting, 0, memory_order_release);
#elif FOUNDATION_PLATFORM_POSIX
	mutex->pending = false;

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

	pthread_cond_init(&mutex->cond, 0);
	pthread_mutex_init(&mutex->mutex, &attr);

	pthread_mutexattr_destroy(&attr);
#else
#error not implemented
#endif

	mutex->lockcount = 0;
	mutex->lockedthread = 0;
}

static void
mutex_finalize(mutex_t* mutex) {
	FOUNDATION_ASSERT(!mutex->lockcount);
#if FOUNDATION_PLATFORM_WINDOWS
	CloseHandle(mutex->event);
	DeleteCriticalSection(&mutex->csection);
#elif FOUNDATION_PLATFORM_POSIX
	pthread_mutex_destroy(&mutex->mutex);
	pthread_cond_destroy(&mutex->cond);
#else
#error not implemented
#endif
}

mutex_t*
mutex_allocate(const char* name, size_t length) {
	mutex_t* mutex = memory_allocate(0, sizeof(mutex_t), 16, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	mutex_initialize(mutex, name, length);
	return mutex;
}

void
mutex_deallocate(mutex_t* mutex) {
	if (!mutex)
		return;
	mutex_finalize(mutex);
	memory_deallocate(mutex);
}

string_const_t
mutex_name(mutex_t* mutex) {
	return mutex->name;
}

bool
mutex_try_lock(mutex_t* mutex) {
	bool was_locked;

#if !BUILD_DEPLOY
	profile_trylock(mutex->name.str, mutex->name.length);
#endif

#if FOUNDATION_PLATFORM_WINDOWS
	was_locked = TryEnterCriticalSection(&mutex->csection);
#elif FOUNDATION_PLATFORM_POSIX
	was_locked = (pthread_mutex_trylock(&mutex->mutex) == 0);
#else
#error mutex_try_lock not implemented
	was_locked = false;
#endif
#if !BUILD_DEPLOY
	if (was_locked)
		profile_lock(mutex->name.str, mutex->name.length);
#endif
	if (was_locked) {
		FOUNDATION_ASSERT(!mutex->lockcount || (thread_id() == mutex->lockedthread));
		if (!mutex->lockcount)
			mutex->lockedthread = thread_id();
		++mutex->lockcount;
	}
	return was_locked;
}

bool
mutex_lock(mutex_t* mutex) {
#if !BUILD_DEPLOY
	profile_trylock(mutex->name.str, mutex->name.length);
#endif

#if FOUNDATION_PLATFORM_WINDOWS
	EnterCriticalSection(&mutex->csection);
#elif FOUNDATION_PLATFORM_POSIX
	if (pthread_mutex_lock(&mutex->mutex) != 0) {
		FOUNDATION_ASSERT_FAILFORMAT("unable to lock mutex %s", mutex->name.str);
		return false;
	}
#else
#error mutex_lock not implemented
#endif
#if !BUILD_DEPLOY
	profile_lock(mutex->name.str, mutex->name.length);
#endif

	FOUNDATION_ASSERT_MSGFORMAT(!mutex->lockcount || (thread_id() == mutex->lockedthread),
	                            "Mutex lock acquired with lockcount > 0 (%d) and locked thread not self (%" PRIx64
	                            " != %" PRIx64 ")",
	                            mutex->lockcount, mutex->lockedthread, thread_id());
	if (!mutex->lockcount)
		mutex->lockedthread = thread_id();
	++mutex->lockcount;

	return true;
}

bool
mutex_unlock(mutex_t* mutex) {
	if (!mutex->lockcount) {
		log_warnf(0, WARNING_SUSPICIOUS, STRING_CONST("Unable to unlock unlocked mutex %.*s"), (int)mutex->name.length,
		          mutex->name.str);
		return false;
	}

	FOUNDATION_ASSERT(mutex->lockedthread == thread_id());
	--mutex->lockcount;

#if !BUILD_DEPLOY
	profile_unlock(mutex->name.str, mutex->name.length);
#endif

#if FOUNDATION_PLATFORM_WINDOWS
	LeaveCriticalSection(&mutex->csection);
#elif FOUNDATION_PLATFORM_POSIX
	if (pthread_mutex_unlock(&mutex->mutex) != 0) {
		FOUNDATION_ASSERT_FAILFORMAT("unable to unlock mutex %s", mutex->name.str);
		return false;
	}
#else
#error mutex_unlock not implemented
#endif
	return true;
}

bool
mutex_wait(mutex_t* mutex) {
#if FOUNDATION_PLATFORM_WINDOWS
	return mutex_try_wait(mutex, INFINITE);
#else
	return mutex_try_wait(mutex, 0xFFFFFFFF);
#endif
}

bool
mutex_try_wait(mutex_t* mutex, unsigned int milliseconds) {
#if FOUNDATION_PLATFORM_WINDOWS
	DWORD ret;
#elif FOUNDATION_PLATFORM_POSIX
	struct timeval now;
	struct timespec then;
#endif
#if FOUNDATION_PLATFORM_WINDOWS

#if !BUILD_DEPLOY
	profile_wait(STRING_ARGS(mutex->name));
#endif

	atomic_incr32(&mutex->waiting, memory_order_acq_rel);

	ret = WaitForSingleObject(mutex->event, milliseconds);

	if (ret == WAIT_OBJECT_0)
		mutex_lock(mutex);

	if (atomic_decr32(&mutex->waiting, memory_order_acq_rel) == 0)
		ResetEvent(mutex->event);

	return ret == WAIT_OBJECT_0;

#elif FOUNDATION_PLATFORM_POSIX

	mutex_lock(mutex);

	if (mutex->pending) {
		mutex->pending = false;
		return true;
	}
	if (!milliseconds) {
		mutex_unlock(mutex);
		return false;
	}

	--mutex->lockcount;

	bool was_signal = false;
	if (milliseconds == 0xFFFFFFFF) {
		int ret = pthread_cond_wait(&mutex->cond, &mutex->mutex);
		if (ret == 0) {
			was_signal = true;
		} else {
			string_const_t errmsg = system_error_message(ret);
			log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to wait on mutex '%.*s': %.*s (%d)"),
			           STRING_FORMAT(mutex->name), STRING_FORMAT(errmsg), ret);
		}
	} else {
		int ret;
		gettimeofday(&now, 0);
		then.tv_sec = now.tv_sec + (time_t)(milliseconds / 1000);
		then.tv_nsec = (now.tv_usec * 1000) + (long)(milliseconds % 1000) * 1000000L;
		while (then.tv_nsec >= 1000000000L) {
			++then.tv_sec;
			then.tv_nsec -= 1000000000L;
		}
		ret = pthread_cond_timedwait(&mutex->cond, &mutex->mutex, &then);
		if (ret == 0) {
			was_signal = true;
		} else if (ret != ETIMEDOUT) {
			string_const_t errmsg = system_error_message(ret);
			log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to wait (timed) on mutex '%.*s': %.*s (%d)"),
			           STRING_FORMAT(mutex->name), STRING_FORMAT(errmsg), ret);
		}
	}

	++mutex->lockcount;
	mutex->lockedthread = thread_id();

	if (was_signal)
		mutex->pending = false;
	else
		mutex_unlock(mutex);

	return was_signal;

#else
#error mutex_wait not implemented
#endif
}

void
mutex_signal(mutex_t* mutex) {
#if !BUILD_DEPLOY
	profile_signal(mutex->name.str, mutex->name.length);
#endif

#if FOUNDATION_PLATFORM_WINDOWS

	SetEvent(mutex->event);

#elif FOUNDATION_PLATFORM_POSIX

	mutex_lock(mutex);
	mutex->pending = true;

	int ret = pthread_cond_broadcast(&mutex->cond);
	if (ret != 0) {
		string_const_t errmsg = system_error_message(ret);
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to signal mutex '%.*s': %.*s (%d)"),
		           STRING_FORMAT(mutex->name), STRING_FORMAT(errmsg), ret);
	}

	mutex_unlock(mutex);

#else
#error mutex_signal not implemented
#endif
}

#if FOUNDATION_PLATFORM_WINDOWS

void*
mutex_event_handle(mutex_t* mutex) {
	return mutex->event;
}

#endif
