/* library.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file library.h
\brief Dynamic library loader

Dynamic library loader for platforms supporting this concept. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Load library. Where libraries are searched is system dependent. If the
same library was already previously loaded, a new reference to the same library
object will be returned, otherwise a new library object will be created and
referenced.

Use #library_unload to release a reference to a library object.

If the given name does not contain the platform specific dynamic library prefix
and extension, the function will also try loading the library with this prefix and
extension added. If the name contains a path, it should be on the standard format
as returned by #path_clean

On platforms where dynamic library loading is not supported a not implemented
error will be reported.
\param name Dynamic library name (or optional path)
\param length Length of name/path
\return Null if not found or error loading, library object reference if loaded successfully */
FOUNDATION_API object_t
library_load(const char* name, size_t length);

/*! Reference library, explicitly increase the reference count of the library object.
Use #library_unload to release a reference to a library object.
\param library Library object
\return Object handle if library object is still valid, 0 if library object is no longer valid */
FOUNDATION_API object_t
library_ref(object_t library);

/*! Release a reference to the library object. Once the library object
has no more references, the underlying structure will be deallocated and the system resources
associated will be released.
\param library Library object */
FOUNDATION_API void
library_release(object_t library);

/*! Lookup a symbol by name in the library.
\param library Library object
\param name Symbol name
\param length Length of symbol name
\return Address of symbol, 0 if not found */
FOUNDATION_API void*
library_symbol(object_t library, const char* name, size_t length);

/*! Get library name
\param library Library object
\return Library name, empty string if not a valid library */
FOUNDATION_API string_const_t
library_name(object_t library);

/*! Query if valid library
\param library Library object
\return true if loaded and valid, false if not */
FOUNDATION_API bool
library_valid(object_t library);
