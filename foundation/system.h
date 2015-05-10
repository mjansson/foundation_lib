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

#include <foundation/platform.h>
#include <foundation/types.h>


FOUNDATION_API int                   system_error( void );
FOUNDATION_API void                  system_error_reset( void );
FOUNDATION_API string_const_t        system_error_message( int code );

FOUNDATION_API platform_t            system_platform( void );
FOUNDATION_API architecture_t        system_architecture( void );
FOUNDATION_API byteorder_t           system_byteorder( void );
FOUNDATION_API size_t                system_hardware_threads( void );
FOUNDATION_API string_t              system_hostname( char* buffer, size_t length );
FOUNDATION_API uint64_t              system_hostid( void );
FOUNDATION_API string_t              system_username( char* buffer, size_t length );

FOUNDATION_API bool                  system_debugger_attached( void );
FOUNDATION_API void                  system_pause( void );

FOUNDATION_API uint16_t              system_language( void );
FOUNDATION_API uint16_t              system_country( void );
FOUNDATION_API uint32_t              system_locale( void );
FOUNDATION_API string_t              system_locale_string( char* buffer, size_t length );

FOUNDATION_API void                  system_set_device_orientation( device_orientation_t orientation );
FOUNDATION_API device_orientation_t  system_device_orientation( void );

FOUNDATION_API void                  system_process_events( void );

FOUNDATION_API bool                  system_message_box( const char* title, size_t title_length, const char* message, size_t message_length, bool cancel_button );

FOUNDATION_API event_stream_t*       system_event_stream( void );
FOUNDATION_API void                  system_post_event( foundation_event_id event );

FOUNDATION_API void                  system_thread_deallocate( void );
