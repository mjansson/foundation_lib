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
#include <foundation/pnacl.h>

#if FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD
#  include <sys/event.h>
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
	beacon->fd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
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
#else
	beacon->mutex = mutex_allocate(STRING_CONST("beacon"));
#endif
}

void
beacon_finalize(beacon_t* beacon) {
#if FOUNDATION_PLATFORM_WINDOWS
	CloseHandle(beacon->event);
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
	close(beacon->poll);
	close(beacon->fd);
#elif FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD
	close(beacon->kq);
	close(beacon->all[0]);
	close(beacon->writefd);
#else
	mutex_deallocate(beacon->mutex);
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
#else
	return beacon_try_wait(beacon, (unsigned int)-1);
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
	struct epoll_event event;
	int ret = epoll_wait(beacon->poll, &event, 1, (int)milliseconds);
	if (ret > 0)
		slot = event.data.fd;
	if (slot == 0) {
		eventfd_t value = 0;
		if (eventfd_read(beacon->fd, &value) < 0) 
			slot = -1;
	}
#elif FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD
	int ret;
	struct timespec tspec;
	struct timespec* timeout = nullptr;
	struct kevent event;
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
	else if ((ret < 0) && timeout)
		thread_sleep(milliseconds);
	if (slot == 0) {
		char data[16];
		bool got_data = false;
		while (read(beacon->all[0], data, sizeof(data)) > 0)
			got_data = true;
		if (!got_data)
			slot = -1;
	}
#else
	bool got = false;
	if (milliseconds != (unsigned int)-1)
		got = mutex_try_wait(beacon->mutex, milliseconds);
	else
		got = mutex_wait(beacon->mutex);
	if (got) {
		slot = 0;
		mutex_unlock(beacon->mutex);
	}
#endif
	return slot;
}

void
beacon_fire(beacon_t* beacon) {
#if FOUNDATION_PLATFORM_WINDOWS
	SetEvent(beacon->event);
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID
	eventfd_t value = 1;
	eventfd_write(beacon->fd, value);
#elif FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD
	char data = 0;
	write(beacon->writefd, &data, 1);
#else
	mutex_signal(beacon->mutex);
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
