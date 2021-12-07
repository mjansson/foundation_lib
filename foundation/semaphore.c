/* semaphore.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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
#elif FOUNDATION_PLATFORM_APPLE
#include <foundation/apple.h>
#include <dispatch/dispatch.h>
#include <errno.h>
#elif FOUNDATION_PLATFORM_ANDROID
#include <time.h>
#include <asm/fcntl.h>
#elif FOUNDATION_PLATFORM_POSIX
#include <time.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#endif

#if FOUNDATION_PLATFORM_WINDOWS

bool
semaphore_initialize(semaphore_t* semaphore, uint value) {
	FOUNDATION_ASSERT(value <= 0xFFFF);
	*semaphore = CreateSemaphoreA(0, (long)value, 0xFFFF, 0);  // lint !e970
	return (*semaphore) != 0 ? true : false;
}

bool
semaphore_initialize_named(semaphore_t* semaphore, const char* name, size_t length, uint value) {
	FOUNDATION_ASSERT(name);
	FOUNDATION_ASSERT(value <= 0xFFFF);
	char buffer[128];
	string_t namestr = string_copy(buffer, sizeof(buffer), name, length);
	*semaphore = CreateSemaphoreA(0, (long)value, 0xFFFF, namestr.str);  // lint !e970
	return (*semaphore) != 0 ? true : false;
}

void
semaphore_finalize(semaphore_t* semaphore) {
	CloseHandle((HANDLE)*semaphore);
}

bool
semaphore_wait(semaphore_t* semaphore) {
	DWORD res = WaitForSingleObject((HANDLE)*semaphore, INFINITE);
	return (res == WAIT_OBJECT_0);
}

bool
semaphore_try_wait(semaphore_t* semaphore, uint milliseconds) {
	DWORD res = WaitForSingleObject((HANDLE)*semaphore, milliseconds);
	return (res == WAIT_OBJECT_0);
}

void
semaphore_post(semaphore_t* semaphore) {
	ReleaseSemaphore((HANDLE)*semaphore, 1, 0);
}

void
semaphore_post_multiple(semaphore_t* semaphore, uint count) {
	ReleaseSemaphore((HANDLE)*semaphore, count, 0);
}

void*
semaphore_event_handle(semaphore_t* semaphore) {
	return *semaphore;
}

#elif FOUNDATION_PLATFORM_APPLE

// macOS:
// unnamed - dispatch_semaphore_create
// named - sem_open

// iOS:
// unnamed - dispatch_semaphore_create
// named - UNSUPPORTED

bool
semaphore_initialize(semaphore_t* semaphore, uint value) {
	FOUNDATION_ASSERT(value <= 0xFFFF);

	semaphore->name = (string_t){0, 0};
	semaphore->sem.unnamed = dispatch_semaphore_create((long)value);

	if (!semaphore->sem.unnamed) {
		string_const_t errmsg = system_error_message(0);
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to initialize unnamed semaphore: %.*s"),
		           STRING_FORMAT(errmsg));
		return false;
	}
	return true;
}

bool
semaphore_initialize_named(semaphore_t* semaphore, const char* name, size_t length, uint value) {
	FOUNDATION_ASSERT(name);
	FOUNDATION_ASSERT(value <= 0xFFFF);
#if FOUNDATION_PLATFORM_MACOS
	semaphore->name = string_clone(name, length);
	semaphore->sem.named = 0;

	sem_t* sem = sem_open(semaphore->name.str, O_CREAT, 0666, value);
	if (sem == SEM_FAILED) {
		string_const_t errmsg = system_error_message(0);
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL,
		           STRING_CONST("Unable to initialize named semaphore (sem_open '%.*s'): %.*s"),
		           STRING_FORMAT(semaphore->name), STRING_FORMAT(errmsg));
		return false;
	}

	semaphore->sem.named = sem;
	return true;
#else
	FOUNDATION_ASSERT_FAIL("Named semaphores not supported on iOS");
	FOUNDATION_UNUSED(semaphore);
	FOUNDATION_UNUSED(length);
	return false;
#endif
}

void
semaphore_finalize(semaphore_t* semaphore) {
	if (!semaphore->name.length) {
		if (semaphore->sem.unnamed)
			dispatch_release(semaphore->sem.unnamed);
	}
#if FOUNDATION_PLATFORM_MACOS
	else {
		sem_unlink(semaphore->name.str);
		if (semaphore->sem.named)
			sem_close(semaphore->sem.named);
		string_deallocate(semaphore->name.str);
	}
#endif
}

bool
semaphore_wait(semaphore_t* semaphore) {
	if (!semaphore->name.length) {
		long result = dispatch_semaphore_wait(semaphore->sem.unnamed, DISPATCH_TIME_FOREVER);
		return (result == 0);
	}
#if FOUNDATION_PLATFORM_MACOS
	int ret = sem_wait(semaphore->sem.named);
	if (ret != 0) {
		// Don't report error if interrupted
		if (errno != EINTR) {
			int err = errno;
			string_const_t errmsg = system_error_message(err);
			log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to wait for named semaphore '%.*s': %.*s (%d)"),
			           STRING_FORMAT(semaphore->name), STRING_FORMAT(errmsg), err);
		} else {
			log_info(0, STRING_CONST("Semaphore wait interrupted by signal"));
		}
		return false;
	}
	return true;
#else
	return false;
#endif
}

bool
semaphore_try_wait(semaphore_t* semaphore, uint milliseconds) {
	if (!semaphore->name.length) {
		long result = dispatch_semaphore_wait(semaphore->sem.unnamed,
		                                      (milliseconds > 0) ?
		                                          dispatch_time(DISPATCH_TIME_NOW, 1000000LL * (int64_t)milliseconds) :
		                                          DISPATCH_TIME_NOW);
		return (result == 0);
	}
#if FOUNDATION_PLATFORM_MACOS
	// TODO: Proper implementation (sem_timedwait not supported)
	if (milliseconds > 0) {
		tick_t wakeup = time_current() + (((tick_t)milliseconds * time_ticks_per_second()) / 1000LL);
		do {
			if (sem_trywait(semaphore->sem.named) == 0)
				return true;
			thread_yield();
		} while (time_current() < wakeup);
		return false;
	}
	return sem_trywait(semaphore->sem.named) == 0;
#else
	return false;
#endif
}

void
semaphore_post(semaphore_t* semaphore) {
	if (!semaphore->name.length) {
		dispatch_semaphore_signal(semaphore->sem.unnamed);
	}
#if FOUNDATION_PLATFORM_MACOS
	else {
		sem_post(semaphore->sem.named);
	}
#endif
}

void
semaphore_post_multiple(semaphore_t* semaphore, uint count) {
	if (!semaphore->name.length) {
		for (uint item = 0; item < count; ++item)
			dispatch_semaphore_signal(semaphore->sem.unnamed);
	}
#if FOUNDATION_PLATFORM_MACOS
	else {
		for (uint item = 0; item < count; ++item)
			sem_post(semaphore->sem.named);
	}
#endif
}

#elif FOUNDATION_PLATFORM_POSIX

bool
semaphore_initialize(semaphore_t* semaphore, uint value) {
	FOUNDATION_ASSERT(value <= 0xFFFF);

	semaphore->name = (string_t){0, 0};
	semaphore->sem = 0;

#if FOUNDATION_PLATFORM_ANDROID
#pragma clang diagnostic ignored "-Wcast-qual"
	// atomic_init((atomic_uint*)&semaphore->unnamed.count, value << 1);
	semaphore->unnamed.count = (value << 1);
	atomic_thread_fence_release();
#else
	if (sem_init(&semaphore->unnamed, 0, value)) {
		int err = system_error();
		string_const_t errmsg = system_error_message(err);
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to initialize semaphore: %.*s (%d)"),
		           STRING_FORMAT(errmsg), err);
		return false;
	}
#endif
	semaphore->sem = &semaphore->unnamed;

	return true;
}

bool
semaphore_initialize_named(semaphore_t* semaphore, const char* name, size_t length, uint value) {
	FOUNDATION_ASSERT(name);
	FOUNDATION_ASSERT(value <= 0xFFFF);

	if (name && (length > 0) && (name[0] != '/'))
		semaphore->name = string_allocate_format(STRING_CONST("/%.*s"), (int)length, name);
	else
		semaphore->name = string_clone(name, length);
	semaphore->sem = 0;

	sem_t* sem = sem_open(semaphore->name.str, O_CREAT, (mode_t)0666, value);
	if (sem == SEM_FAILED) {
		int err = system_error();
		string_const_t errmsg = system_error_message(err);
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL,
		           STRING_CONST("Unable to initialize named semaphore (sem_open '%.*s'): %.*s (%d)"),
		           STRING_FORMAT(semaphore->name), STRING_FORMAT(errmsg), err);
		return false;
	}

	semaphore->sem = sem;
	return true;
}

void
semaphore_finalize(semaphore_t* semaphore) {
	if (!semaphore->name.length) {
		if (semaphore->sem)
			sem_destroy(semaphore->sem);
	} else {
		sem_unlink(semaphore->name.str);
		if (semaphore->sem)
			sem_close(semaphore->sem);
		string_deallocate(semaphore->name.str);
	}
}

bool
semaphore_wait(semaphore_t* semaphore) {
	return sem_wait(semaphore->sem) == 0;
}

bool
semaphore_try_wait(semaphore_t* semaphore, uint milliseconds) {
	if (milliseconds > 0) {
		struct timeval now;
		struct timespec then;
		gettimeofday(&now, 0);
		then.tv_sec = now.tv_sec + (time_t)(milliseconds / 1000);
		then.tv_nsec = (now.tv_usec * 1000) + (long)(milliseconds % 1000) * 1000000L;
		while (then.tv_nsec >= 1000000000L) {
			++then.tv_sec;
			then.tv_nsec -= 1000000000L;
		}
		return sem_timedwait(semaphore->sem, &then) == 0;
	}
	return sem_trywait(semaphore->sem) == 0;
}

void
semaphore_post(semaphore_t* semaphore) {
	sem_post(semaphore->sem);
}

void
semaphore_post_multiple(semaphore_t* semaphore, uint count) {
	for (uint item = 0; item < count; ++item)
		sem_post(semaphore->sem);
}

#else
#error Not implemented
#endif
