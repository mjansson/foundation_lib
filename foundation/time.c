/* time.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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
#include <foundation/internal.h>

#if FOUNDATION_PLATFORM_WINDOWS
#include <foundation/windows.h>
#elif FOUNDATION_PLATFORM_APPLE
#include <foundation/apple.h>
#include <mach/mach_time.h>
#elif FOUNDATION_PLATFORM_POSIX
#include <foundation/posix.h>
#else
#error Not implemented on this platform!
#endif

static tick_t time_freq;
static double time_oofreq;
static tick_t time_start;

#if FOUNDATION_PLATFORM_APPLE

static mach_timebase_info_data_t time_info;

static void
absolutetime_to_nanoseconds(uint64_t mach_time, tick_t* clock) {
	*clock = (tick_t)(mach_time * time_info.numer / time_info.denom);
}

#endif

int
internal_time_initialize(void) {
#if FOUNDATION_PLATFORM_WINDOWS
	tick_t unused;
	if (!QueryPerformanceFrequency((LARGE_INTEGER*)&time_freq) || !QueryPerformanceCounter((LARGE_INTEGER*)&unused))
		return -1;
#elif FOUNDATION_PLATFORM_APPLE
	if (mach_timebase_info(&time_info))
		return -1;
	time_freq = 1000000000LL;
#elif FOUNDATION_PLATFORM_POSIX
	struct timespec ts = {.tv_sec = 0, .tv_nsec = 0};
	if (clock_gettime(CLOCK_MONOTONIC, &ts))
		return -1;
	time_freq = 1000000000LL;
#else
#error Not implemented
#endif

	time_oofreq = 1.0 / (double)time_freq;
	time_start = time_current();

	return 0;
}

void
internal_time_finalize(void) {
}

tick_t
time_current(void) {
#if FOUNDATION_PLATFORM_WINDOWS

	tick_t curclock;
	QueryPerformanceCounter((LARGE_INTEGER*)&curclock);
	return curclock;

#elif FOUNDATION_PLATFORM_APPLE

	tick_t curclock = 0;
	absolutetime_to_nanoseconds(mach_absolute_time(), &curclock);
	return curclock;

#elif FOUNDATION_PLATFORM_POSIX

	struct timespec ts = {.tv_sec = 0, .tv_nsec = 0};
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ((tick_t)ts.tv_sec * 1000000000LL) + (tick_t)ts.tv_nsec;

#else
#error Not implemented
#endif
}

tick_t
time_startup(void) {
	return time_start;
}

tick_t
time_ticks_per_second(void) {
	return time_freq;
}

tick_t
time_diff(const tick_t from, const tick_t to) {
	return (to - from);
}

deltatime_t
time_elapsed(const tick_t t) {
	tick_t ticks = time_elapsed_ticks(t);
	return (deltatime_t)((double)ticks * time_oofreq);
}

tick_t
time_elapsed_ticks(const tick_t t) {
	tick_t dt;

#if FOUNDATION_PLATFORM_WINDOWS

	tick_t curclock = t;
	QueryPerformanceCounter((LARGE_INTEGER*)&curclock);
	dt = curclock - t;

#elif FOUNDATION_PLATFORM_APPLE

	tick_t curclock = t;
	absolutetime_to_nanoseconds(mach_absolute_time(), &curclock);
	dt = curclock - t;

#elif FOUNDATION_PLATFORM_POSIX

	tick_t curclock;
	struct timespec ts = {.tv_sec = 0, .tv_nsec = 0};
	clock_gettime(CLOCK_MONOTONIC, &ts);

	curclock = ((tick_t)ts.tv_sec * 1000000000LL) + ts.tv_nsec;
	dt = curclock - t;

#else
#error Not implemented
	dt = 0;
#endif

	return dt;
}

deltatime_t
time_ticks_to_seconds(const tick_t dt) {
	return (deltatime_t)((double)dt * time_oofreq);
}

deltatime_t
time_ticks_to_milliseconds(const tick_t dt) {
	return (deltatime_t)(((double)dt * 1000.0) * time_oofreq);
}

#if FOUNDATION_PLATFORM_WINDOWS
#include <sys/timeb.h>
#if FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
#define _ftime64_s _ftime64
#endif
#endif
#if FOUNDATION_PLATFORM_APPLE
#include <sys/time.h>
#endif

tick_t
time_system(void) {
#if FOUNDATION_PLATFORM_WINDOWS

	/*lint --e{754,534} */
	struct __timeb64 tb;
	_ftime64_s(&tb);
	return ((tick_t)tb.time * 1000LL) + (tick_t)tb.millitm;

#elif FOUNDATION_PLATFORM_APPLE

	struct timeval now = {0, 0};
	gettimeofday(&now, 0);
	return ((int64_t)now.tv_sec * 1000LL) + (now.tv_usec / 1000LL);

#elif FOUNDATION_PLATFORM_POSIX

	struct timespec ts = {.tv_sec = 0, .tv_nsec = 0};
	clock_gettime(CLOCK_REALTIME, &ts);
	return ((int64_t)ts.tv_sec * 1000LL) + (ts.tv_nsec / 1000000LL);

#else
#error Not implemented
#endif
}
