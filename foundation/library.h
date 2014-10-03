/* library.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file library.h
    Dynamic library loader */

#include <foundation/platform.h>
#include <foundation/types.h>


/*! Load library. Where libraries are searched is system dependent.
    \param name              Dynamic library name (or optional path)
    \return                  Null if not found or error loading, library object reference if loaded successfully */
FOUNDATION_API object_t      library_load( const char* name );

/*! Reference library, explicitly increase the reference count of the library object
    \param library           Library object
    \return                  Object handle if library object is still valid, 0 if library object is no longer valid */
FOUNDATION_API object_t      library_ref( object_t library );

/*! Unload library
    \param library           Library object */
FOUNDATION_API void          library_unload( object_t library );

/*! Symbol lookup
    \param library           Library object
    \param name              Symbol name
    \return                  Address of symbol, 0 if not found */
FOUNDATION_API void*         library_symbol( object_t library, const char* name );

/*! Get library name
    \param library           Library object
    \return                  Library name, 0 if not a valid library */
FOUNDATION_API const char*   library_name( object_t library );

/*! Query if valid library
    \param library           Library object
    \return                  true if loaded and valid, false if not */
FOUNDATION_API bool          library_valid( object_t library );
