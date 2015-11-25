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
#include <foundation/posix.h>
#include <foundation/apple.h>

#if FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD
#  include <sys/event.h>
#endif

#define BEACON_FIRE_NONE    0
#define BEACON_FIRE_PENDING 1
#define BEACON_FIRE_DONE    2
#define BEACON_FIRE_READING 3

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
	beacon->poll = epoll_create(sizeof(beacon->all)/sizeof(beacon->all[0]));
	beacon->count = 1;
	beacon->all[0] = beacon->fd;
	struct epoll_event event;
	event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
	event.data.fd = 0;
	epoll_ctl(beacon->poll, EPOLL_CTL_ADD, beacon->fd, &event);
#elif FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD
	int pipefd[2];
	beacon->kq = kqueue();
	pipe(pipefd);
	fcntl(pipefd[0], F_SETFL, O_NONBLOCK);
	beacon->all[0] = pipefd[0];
	beacon->writefd = pipefd[1];
	struct kevent changes;
	EV_SET(&changes, beacon->all[0], EVFILT_READ, EV_ADD, 0, 0, nullptr);
	kevent(beacon->kq, &changes, 1, 0, 0, 0);
	beacon->count = 1;
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
	close(beacon->all[0]);
	close(beacon->writefd);
#endif
}

void
beacon_deallocate(beacon_t* beacon) {
	if (!beacon)
		return;
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
#else
	return -1;
#endif
}

int
beacon_try_wait(beacon_t* beacon, unsigned int milliseconds) {
	int slot = -1;
#if FOUNDATION_PLATFORM_WINDOWS
	unsigned int count = (unsigned int)beacon->count;
	unsigned int wait_status = WaitForMultipleObjects(count, (HANDLE*)beacon->all,
	                                                  FALSE, milliseconds);
	//WAIT_OBJECT_0 value is 0, so this checks range [WAIT_OBJECT_0, WAIT_OBJECT_0+count)
	if (wait_status < count) {
		//Same behaviour as linux/bsd implementations, where auxiliary beacons added
		//will remain fired after a beacon has seen it
		if (wait_status > 0)
			SetEvent(beacon->all[wait_status]);
		return (int)wait_status;
	}
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
	if (atomic_cas32(&beacon->fired, BEACON_FIRE_READING, BEACON_FIRE_DONE)) {
		eventfd_t value = 0;
		eventfd_read(beacon->fd, &value);
		atomic_cas32(&beacon->fired, BEACON_FIRE_NONE, BEACON_FIRE_READING);
		return (value > 0)
	}
	struct epoll_event event;
	int ret = epoll_wait(beacon->poll, &event, 1, (int)milliseconds);
	if (ret > 0)
		slot = event.data.fd;
	if (slot == 0) {
		if (atomic_cas32(&beacon->fired, BEACON_FIRE_READING, BEACON_FIRE_DONE)) {
			eventfd_t value = 0;
			eventfd_read(beacon->fd, &value);
			atomic_cas32(&beacon->fired, BEACON_FIRE_NONE, BEACON_FIRE_READING);
			return (value > 0);
		}
	}
#elif FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD
	int ret;
	struct timespec tspec;
	struct timespec* timeout = nullptr;
	struct kevent event;
	if (atomic_cas32(&beacon->fired, BEACON_FIRE_READING, BEACON_FIRE_DONE)) {
		char data[8];
		while (read(beacon->all[0], data, 8) <= 0)
			thread_yield();
		while (read(beacon->all[0], data, 8) > 0);
		atomic_cas32(&beacon->fired, BEACON_FIRE_NONE, BEACON_FIRE_READING);
		return 0;
	}
	if (milliseconds != (unsigned int)-1) {
		tspec.tv_sec  = (time_t)(milliseconds / 1000);
		tspec.tv_nsec = (long)(milliseconds % 1000) * 1000000L;
		while (tspec.tv_nsec >= 1000000000L) {
			++tspec.tv_sec;
			tspec.tv_nsec -= 1000000000L;
		}
		timeout = &tspec;
	}
	ret = kevent(beacon->kq, nullptr, 0, &event, 1, timeout);
	if (ret > 0)
		slot = (int)(uintptr_t)event.udata;
	if (slot == 0) {
		if (atomic_cas32(&beacon->fired, BEACON_FIRE_READING, BEACON_FIRE_DONE)) {
			char data[8];
			while (read(beacon->all[0], data, 8) <= 0)
				thread_yield();
			while (read(beacon->all[0], data, 8) > 0);
			atomic_cas32(&beacon->fired, BEACON_FIRE_NONE, BEACON_FIRE_READING);
			return 0;
		}
		slot = -1;
	}
#endif
	return slot;
}

void
beacon_fire(beacon_t* beacon) {
#if FOUNDATION_PLATFORM_WINDOWS
	SetEvent(beacon->event);
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
	if (atomic_cas32(&beacon->fired, BEACON_FIRE_PENDING, BEACON_FIRE_NONE)) {
		eventfd_t value = 1;
		eventfd_write(beacon->fd, value);
		atomic_cas32(&beacon->fired, BEACON_FIRE_DONE, BEACON_FIRE_PENDING);
	}
#elif FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD
	if (atomic_cas32(&beacon->fired, BEACON_FIRE_PENDING, BEACON_FIRE_NONE)) {
		char data = 0;
		write(beacon->writefd, &data, 1);
		atomic_cas32(&beacon->fired, BEACON_FIRE_DONE, BEACON_FIRE_PENDING);
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
		struct epoll_event event;
		event.events = EPOLLIN | EPOLLERR | EPOLLHUP;
		event.data.fd = (int)beacon->count;
		epoll_ctl(beacon->poll, EPOLL_CTL_ADD, fd, &event);
		return (int)beacon->count++;
	}
	return -1;
}

void
beacon_remove(beacon_t* beacon, int fd) {
	size_t islot;
	for (islot = 1; islot < beacon->count; ++islot) {
		if (beacon->all[islot] == fd) {
			struct epoll_event event;
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
	size_t numslots = sizeof(beacon->all)/sizeof(beacon->all[0]);
	if (beacon->count < numslots) {
		beacon->all[beacon->count] = fd;
		struct kevent changes;
		EV_SET(&changes, fd, EVFILT_READ, EV_ADD, 0, 0, (void*)(uintptr_t)beacon->count);
		kevent(beacon->kq, &changes, 1, 0, 0, 0);
		return (int)beacon->count++;
	}
	return -1;
}

void
beacon_remove(beacon_t* beacon, int fd) {
	size_t islot;
	for (islot = 1; islot < beacon->count; ++islot) {
		if (beacon->all[islot] == fd) {
			struct kevent changes;
			EV_SET(&changes, fd, EVFILT_READ, EV_DELETE, 0, 0, nullptr);
			kevent(beacon->kq, &changes, 1, 0, 0, 0);
			--beacon->count;
			if (islot < beacon->count) {
				beacon->all[islot] = beacon->all[beacon->count];
				//Re-add acts as mosify
				EV_SET(&changes, beacon->all[islot], EVFILT_READ, EV_ADD, 0, 0, (void*)(uintptr_t)islot);
				kevent(beacon->kq, &changes, 1, 0, 0, 0);
			}
		}
	}
}

int
beacon_event_handle(beacon_t* beacon) {
	return beacon->all[0];
}

#endif
