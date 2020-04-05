/* system.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file system.h
\brief System queries

System queries and utility functions. All functions returning a string (const char*)
are thread safe and return a thread-local buffer that is valid until the next call to a system
function returning a string or the termination of the thread. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Get the current thread local system error code (errno in Posix, GetLastError on Windows)
\return Error code */
FOUNDATION_API int
system_error(void);

/*! Reset the current thread local system error code to indicate no error */
FOUNDATION_API void
system_error_reset(void);

/*! Translate given error code into error message. System errno (unix variants) or last
error code (Windows) will be used if the given code is 0.
\param code Error code
\return Error message (static buffer), empty string if no error */
FOUNDATION_API string_const_t
system_error_message(int code);

/*! Get current platform.
\return Platform identifier */
FOUNDATION_API platform_t
system_platform(void);

/*! Get current architecture.
\return Architecture identifier */
FOUNDATION_API architecture_t
system_architecture(void);

/*! Get current byte order.
\return Byte order identifier */
FOUNDATION_API byteorder_t
system_byteorder(void);

/*! Get number of hardware execution threads the process can utilize.
\return Number of hardware threads */
FOUNDATION_API size_t
system_hardware_threads(void);

/*! Get current host name of system in the given buffer
\param buffer Buffer
\param capacity Capacity of buffer
\return Host name string */
FOUNDATION_API string_t
system_hostname(char* buffer, size_t capacity);

/*! Get current unique host id
\return Host id */
FOUNDATION_API uint64_t
system_hostid(void);

/*! Get user name of user owning the executing process in the given buffer
\param buffer Buffer
\param capacity Capacity of buffer
\return User name string */
FOUNDATION_API string_t
system_username(char* buffer, size_t capacity);

/*! Query if debugger is attached.
\return true if debugger is attached, false if not */
FOUNDATION_API bool
system_debugger_attached(void);

/*! Windows only, execute pause operating system command */
FOUNDATION_API void
system_pause(void);

/*! Get the currently set language.
\return Currently set language code */
FOUNDATION_API uint16_t
system_language(void);

/*! Get the currently set country.
\return Currently set country code */
FOUNDATION_API uint16_t
system_country(void);

/*! Get the currently set locale (country & language).
\return Currently set locale identifier (country & language) */
FOUNDATION_API uint32_t
system_locale(void);

/*! Get the currently set locale (country & language).
\return Currently set locale identifier (country & language) */
FOUNDATION_API uint32_t
system_locale(void);

/*! Get the currently set locale name in the given buffer
\param buffer Buffer
\param capacity Capacity of buffer
\return Currently set locale as a string identifier */
FOUNDATION_API string_t
system_locale_string(char* buffer, size_t capacity);

/*! Set the current locale (country & language).
\param locale Locale identifier (country & language) */
FOUNDATION_API void
system_set_locale(uint32_t locale);

/*! Set the current device orientation. Usually only called internally by system event listeners.
\param orientation New device orientation */
FOUNDATION_API void
system_set_device_orientation(device_orientation_t orientation);

/*! Get the current device orientation.
\return Device orientation */
FOUNDATION_API device_orientation_t
system_device_orientation(void);

/*! Process system events. You should call this once every loop iteration in your main loop. */
FOUNDATION_API void
system_process_events(void);

/*! Popup a system message box to the user.
\param title Title of the message box
\param title_length Length of title string
\param message Message displayed in the box
\param message_length Length of message string
\param cancel_button Flag to trigger a cancel button as well as the default ok button
\return true if ok button pressed, false if cancel button pressed */
FOUNDATION_API bool
system_message_box(const char* title, size_t title_length, const char* message, size_t message_length,
                   bool cancel_button);

/*! Get system event stream.
\return System event stream */
FOUNDATION_API event_stream_t*
system_event_stream(void);

/*! Post a system event to the system event stream.
\param event Event identifier */
FOUNDATION_API void
system_post_event(foundation_event_id event);

/*! Free thread memory used by system buffers. Will be called automatically
on thread exit for foundation threads. */
FOUNDATION_API void
system_thread_finalize(void);
