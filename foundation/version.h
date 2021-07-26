/* version.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file version.h
\brief Version utilities

Version utilities */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Make version identifier from parts, packing into a 128-bit integer
\param major Major version
\param minor Minor version
\param revision Revision number
\param build Build number
\param control Source control identifier
\return Packet version definition */
static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL version_t
version_make(unsigned int major, unsigned int minor, unsigned int revision, unsigned int build, unsigned int control);

// Implementation

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL version_t
version_make(unsigned int major, unsigned int minor, unsigned int revision, unsigned int build, unsigned int control) {
	version_t v;
	v.sub.major = (uint16_t)major;
	v.sub.minor = (uint16_t)minor;
	v.sub.revision = revision;
	v.sub.build = build;
	v.sub.control = control;
	return v;
}
