/* thread.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#pragma once

/*! \file thread.h
\brief Threads

Thread management. If you use the foundation library in threads created by other
means than this thread interface you must remember to call #thread_exit on thread
termination to avoid memory leaks.

Normal thread usage is to use the following flow:
<code>thread = thread_allocate(...);
thread_start(thread);
//...do other things...
thread_join(thread);
thread_deallocate(thread);</code> */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Allocate a new thread.
\param fn Thread execution function
\param data Argument sent to execution function
\param name Thread name
\param length Length of thread name
\param priority Thread priority
\param stacksize Thread stack size
\return New thread  */
FOUNDATION_API thread_t*
thread_allocate(thread_fn fn, void* data, const char* name, size_t length, thread_priority_t priority,
                unsigned int stacksize);

/*! Initialize a new thread.
\param thread Thread
\param fn Thread execution function
\param data Argument sent to execution function
\param name Thread name
\param length Length of thread name
\param priority Thread priority
\param stacksize Thread stack size */
FOUNDATION_API void
thread_initialize(thread_t* thread, thread_fn fn, void* data, const char* name, size_t length,
                  thread_priority_t priority, unsigned int stacksize);

/* Finalize a thread
\param thread Thread */
FOUNDATION_API void
thread_finalize(thread_t* thread);

/* Deallocate a thread
\param thread Thread */
FOUNDATION_API void
thread_deallocate(thread_t* thread);

/*! Start execution of a thread. Must be paired with a corresponding call
to #thread_join
\param thread Thread
\return true if successful, false if failure */
FOUNDATION_API bool
thread_start(thread_t* thread);

/*! Join an execution thread and free system resources. Must be paired with a
corresponding call to #thread_start
\param thread Thread
\return Thread exit code */
FOUNDATION_API void*
thread_join(thread_t* thread);

/*! Query if thread has started execution
\param thread Thread
\return true if started, false if not */
FOUNDATION_API bool
thread_is_started(const thread_t* thread);

/*! Query if thread is running
\param thread Thread
\return true if running, false if not */
FOUNDATION_API bool
thread_is_running(const thread_t* thread);

/*! Query if thread has completed execution and safe to join
\param thread Thread
\return true if finished, false if not */
FOUNDATION_API bool
thread_is_finished(const thread_t* thread);

/*! Signal thread (post thread semaphore)
\param thread Thread */
FOUNDATION_API void
thread_signal(thread_t* thread);

/*! Query if calling thread is signalled
\return true if signalled, false if not */
FOUNDATION_API bool
thread_wait(void);

/*! Wait for calling thread to be signalled for the given amount of time
\param milliseconds Time in milliseconds to wait, 0 means no wait
\return true if signalled, false if not */
FOUNDATION_API bool
thread_try_wait(unsigned int milliseconds);

/*! Query if calling thread is the main thread.
\return true if main thread, false if not */
FOUNDATION_API bool
thread_is_main(void);

/*! Mark the calling thread as the main thread */
FOUNDATION_API void
thread_set_main(void);

/*! Set name for calling thread.
\param name Thread name
\param length Length of thread name */
FOUNDATION_API void
thread_set_name(const char* name, size_t length);

/*! Set thread CPU core affinity for the calling thread. The affinity is
represented by a bitfield, where a set bit indicates affinity for the
corresponding core. For example, to set affinity to core 2 and 3, use
(1<<2)|(1<<3) as mask.
\param group Processor group
\param mask CPU core mask for which the thread is allowed to execute on */
FOUNDATION_API void
thread_set_hardware(uint group, uint64_t mask);

/*! Get name for calling thread.
\return Thread name */
FOUNDATION_API string_const_t
thread_name(void);

/*! Get calling thread system id.
\return Thread id */
FOUNDATION_API uint64_t
thread_id(void);

/*! Get the hardware thread the calling thread is running on.
\return Hardware thread id */
FOUNDATION_API unsigned int
thread_hardware(void);

/*! Sleep calling thread for specified time.
\param milliseconds Milliseconds to sleep */
FOUNDATION_API void
thread_sleep(unsigned int milliseconds);

/*! Yield calling thread remaining timeslice to other threads. */
FOUNDATION_API void
thread_yield(void);

/*! Get thread control block for the calling thread, 0 if not a foundation thread
\return Thread */
FOUNDATION_API thread_t*
thread_self(void);

/*! Initialize on thread start and aqcuire thread local resources. */
FOUNDATION_API void
thread_enter(void);

/*! Finalize on thread exit and free thread local resources. */
FOUNDATION_API void
thread_exit(void);

/*! Query if thread has been entered (thread_enter called)
\return true if thread_enter has been called, false if not */
FOUNDATION_API bool
thread_is_entered(void);

#if FOUNDATION_PLATFORM_ANDROID

/*! Android only. Attach the thread to the JVM to be able to do JVM calls through JNI.
Returns the JNI environment specific to the calling thread.
\return JNI environment (const struct JNINativeInterface**) for the calling thread,
        null if error */
FOUNDATION_API void*
thread_attach_jvm(void);

/*! Android only. Detach the thread from the JVM. No more calls to the JVM through JNI
can be made from this thread after a call to this function. */
FOUNDATION_API void
thread_detach_jvm(void);

#endif
