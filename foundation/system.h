/* system.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#pragma once

/*! \file system.h
    System queries */

#include <foundation/platform.h>
#include <foundation/types.h>


/*! Translate given error code into error message. System errno (unix variants) or last error code (Windows)
    will be used if the given code is 0.
    \param code                   Error code
    \return                       Error message (static buffer), empty string if no error */
FOUNDATION_API const char*        system_error_message( int code );

/*! Get current platform identifier
    \return                       Platform identifier */
FOUNDATION_API platform_t         system_platform( void );

/*! Get current architecture identifier
    \return                       Architecture identifier */
FOUNDATION_API architecture_t     system_architecture( void );

/*! Get current byte order identifier
    \return                       Byte order identifier */
FOUNDATION_API byteorder_t        system_byteorder( void );

/*! Get number of hardware execution threads the process can utilize
    \return                       Number of hardware threads */
FOUNDATION_API unsigned int       system_hardware_threads( void );

/*! Get current host name of system
    \return                       Host name (static buffer) */
FOUNDATION_API const char*        system_hostname( void );

/*! Get current host id (MAC address)
    \return                       Host id */
FOUNDATION_API uint64_t           system_hostid( void );

/*! Get user name of user executing process
    \return                       User name (static buffer) */
FOUNDATION_API const char*        system_username( void );

/*! Query if debugger is attached
    \return                       true if debugger is attached, false if not */
FOUNDATION_API bool               system_debugger_attached( void );

FOUNDATION_API void               system_pause( void );

/*! Get the currently set language
    \return                       Currently set language code */
FOUNDATION_API uint16_t           system_language( void );

/*! Get the currently set country
    \return                       Currently set country code */
FOUNDATION_API uint16_t           system_country( void );

/*! Get the currently set locale (country & language)
    \return                       Currently set locale identifier (country & language) */
FOUNDATION_API uint32_t           system_locale( void );

/*! Get the currently set locale name
    \return                       Currently set locale as a string identifier */
FOUNDATION_API const char*        system_locale_string( void );

/*! Get the current device orientation
    \return                       Device orientation */
FOUNDATION_API device_orientation_t system_device_orientation( void );

/*! Process system events. You should call this once every loop iteration in your main loop */
FOUNDATION_API void               system_process_events( void );

/*! Popup a system message box to the user
    \param title                  Title of the message box
    \param message                Message displayed in the box
    \param cancel_button          Flag to trigger a cancel button as well as the default ok button
    \return                       true if ok button pressed, false if cancel button pressed */
FOUNDATION_API bool               system_message_box( const char* title, const char* message, bool cancel_button );

/*! Get system event stream
    \return                       System event stream */
FOUNDATION_API event_stream_t*    system_event_stream( void );

/*! Post a system event to the system event stream
    \param event                  Event identifier */
FOUNDATION_API void               system_post_event( foundation_event_id event );
