/* beacon.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#include <foundation/foundation.h>
#include <foundation/windows.h>

#if FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD
static atomic32_t _user_id;
#endif

beacon_t*
beacon_allocate(void) {
	beacon_t* beacon = memory_allocate(0, sizeof(beacon_t), 0, MEMORY_PERSISTENT);
	beacon_initialize(beacon);
	return beacon;
}

void
beacon_initialize(beacon_t* beacon) {
	memset(beacon, 0, sizeof(beacon_t));
#if FOUNDATION_PLATFORM_WINDOWS
	beacon->event = CreateEventA(nullptr, FALSE, FALSE, nullptr);
	beacon->all[0] = beacon->event;
	beacon->count = 1;
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
	beacon->fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK | EFD_SEMAPHORE);
	beacon->poll = epoll_create(BEACON_MAX_FD);
	beacon->count = 1;
	beacon->all[0] = beacon->fd;
	struct epoll_event event;
	event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
	event.data.fd = 0;
	epoll_ctl(beacon->poll, EPOLL_CTL_ADD, beacon->fd, &event);
#elif FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD
	beacon->kq = kqueue();
	beacon->event = atomic_incr32(&_user_id);
	struct kevent changes;
	EV_SET(&changes, beacon->event, EVFILT_USER, EV_ADD, NOTE_FFCOPY, 0, nullptr);
	kevent(beacon->kq, &changes, 1, &changes, 1, 0);
#endif
}

void
beacon_finalize(beacon_t* beacon) {
#if FOUNDATION_PLATFORM_WINDOWS
	CloseHandle(beacon->event);
#elif FOUNDATION_PLATFORM_LINUX
	close(beacon->poll);
	close(beacon->fd);
#elif FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD
	close(beacon->kq);
#endif
}

void
beacon_deallocate(beacon_t* beacon) {
	beacon_finalize(beacon);
	memory_deallocate(beacon);
}

int
beacon_wait(beacon_t* beacon) {
#if FOUNDATION_PLATFORM_WINDOWS
	return beacon_try_wait(beacon, INFINITE);
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
	return beacon_try_wait(beacon, (unsigned int)-1);
#elif FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD
	return beacon_try_wait(beacon, (unsigned int)-1);
#endif
	return -1;
}

int
beacon_try_wait(beacon_t* beacon, unsigned int milliseconds) {
#if FOUNDATION_PLATFORM_WINDOWS
	unsigned int count = (unsigned int)beacon->count;
	unsigned int wait_status = WaitForMultipleObjects(count, (HANDLE*)beacon->all,
	                                                  FALSE, milliseconds);
	if ((wait_status >= WAIT_OBJECT_0) && (wait_status < (WAIT_OBJECT_0 + count)))
		return (int)(wait_status - WAIT_OBJECT_0);
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
	if (atomic_cas32(&beacon->fired, 0, 1)) {
		eventfd_t value = 0;
		eventfd_read(beacon->fd, &value);
		if (value > 0)
			return 0;
	}
	struct epoll_event event = {0};
	int ret = epoll_wait(beacon->poll, &event, 1, (int)milliseconds);
	if (ret < 0)
		return -1;
	int slot = event.data.fd;
	if (slot == 0) {
		if (atomic_cas32(&beacon->fired, 0, 1)) {
			eventfd_t value = 0;
			eventfd_read(beacon->fd, &value);
			return (value > 0);
		}
	}
	return slot;
#elif FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD
	int slot = -1;
	struct kevent event = {0};
	struct timespec tspec = {0};
	struct timespec* timeout = nullptr;
	if (atomic_cas32(&beacon->fired, 0, 1)) {
		EV_SET(&event, beacon->event, EVFILT_USER, EV_DISABLE, EV_CLEAR | NOTE_FFCOPY, 0, nullptr);
		kevent(beacon->kq, &event, 1, &event, 1, 0);
		return 0;
	}
	if (milliseconds != (unsigned int)-1) {
		struct timeval now = {0};
		gettimeofday(&now, 0);
		tspec.tv_sec  = now.tv_sec + (time_t)(milliseconds / 1000);
		tspec.tv_nsec = (now.tv_usec * 1000) + (long)(milliseconds % 1000) * 1000000L;
		while (tspec.tv_nsec >= 1000000000L) {
			++tspec.tv_sec;
			tspec.tv_nsec -= 1000000000L;
		}
		timeout = tspec;
	}
	if (kevent(beacon->kq, 0, 0, &event, 1, timeout) > 0) {
		//...
	}
	if (slot == 0) {
		if (atomic_cas32(&beacon->fired, 0, 1)) {
			EV_SET(&event, beacon->event, EVFILT_USER, EV_DISABLE, EV_CLEAR | NOTE_FFCOPY, 0, nullptr);
			kevent(beacon->kq, &event, 1, &event, 1, 0);
			return 0;
		}
	}
#endif
	return -1;
}

void
beacon_fire(beacon_t* beacon) {
#if FOUNDATION_PLATFORM_WINDOWS
	SetEvent(beacon->event);
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
	if (atomic_cas32(&beacon->fired, 1, 0)) {
		eventfd_t value = 1;
		eventfd_write(beacon->fd, &value);
	}
#elif FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD
	if (atomic_cas32(&beacon->fired, 1, 0)) {
		struct kevent changes;
		EV_SET(&changes, 0, EVFILT_USER, EV_ENABLE, NOTE_FFCOPY | NOTE_TRIGGER | 0x1, 0, nullptr);
		kevent(beacon->kq, &changes, 1, &changes, 1, 0);
	}
#endif
}

#if FOUNDATION_PLATFORM_WINDOWS

int
beacon_add(beacon_t* beacon, void* handle) {
	size_t numslots = sizeof(beacon->all)/sizeof(beacon->all[0]);
	if (beacon->count < numslots) {
		beacon->all[beacon->count] = handle;
		return (int)beacon->count++;
	}
	return -1;
}

void
beacon_remove(beacon_t* beacon, void* handle) {
	size_t islot;
	for (islot = 1; islot < beacon->count; ++islot) {
		if (beacon->all[islot] == handle)
			beacon->all[islot] = beacon->all[--beacon->count];
	}
}

void*
beacon_event_handle(beacon_t* beacon) {
	return beacon->event;
}

#endif

#if FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID

int
beacon_add(beacon_t* beacon, int fd) {
	size_t numslots = sizeof(beacon->all)/sizeof(beacon->all[0]);
	if (beacon->count < numslots) {
		beacon->all[beacon->count] = fd;
		struct epoll_event event = {0};
		event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
		event.data.fd = beacon->count;
		epoll_ctl(beacon->poll, EPOLL_CTL_ADD, fd, &event);
		return (int)beacon->count++;
	}
	return -1;
}

void
beacon_remove(beacon_t* beacon, int fd) {
	size_t islot;
	for (islot = 1; islot < beacon->count; ++islot) {
		if (beacon->all[islot] == handle) {
			struct epoll_event event = {0};
			epoll_ctl(beacon->poll, EPOLL_CTL_DEL, fd, &event);
			--beacon->count;
			if (islot < beacon->count) {
				beacon->all[islot] = beacon->all[beacon->count];
				event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
				event.data.fd = (int)islot;
				epoll_ctl(beacon->poll, EPOLL_CTL_MOD, beacon->all[islot], &event);
			}
		}
	}
}

int
beacon_event_handle(beacon_t* beacon) {
	return beacon->fd;
}

#endif

#if FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD

int
beacon_add(beacon_t* beacon, int fd) {
	//...
	return -1;
}

void
beacon_remove(beacon_t* beacon, int fd) {
	//...
}

int
beacon_event_handle(beacon_t* beacon) {
	return beacon->event;
}

#endif
