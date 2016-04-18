/* windows.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#pragma once

/*! \file windows.h
\brief Safe inclusion of windows.h

Safe inclusion of windows.h without collisions with foundation library symbols. */

#include <foundation/platform.h>
#include <foundation/types.h>

#if FOUNDATION_PLATFORM_WINDOWS

#define STREAM_SEEK_END _STREAM_SEEK_END

#undef UUID_DEFINED
#undef UUID

#define UUID_DEFINED 1
#define UUID uint128_t

#define WIN32_LEAN_AND_MEAN

#if FOUNDATION_COMPILER_MSVC
//Work around broken dbghlp.h header
#pragma warning(disable : 4091)
#endif

#include <windows.h>

#undef uuid_t

#include <winsock2.h>
#include <iptypes.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <share.h>
#include <io.h>
#include <shellapi.h>
#include <stdlib.h>
#include <shlobj.h>
#include <dbghelp.h>
#include <crtdbg.h>

#undef min
#undef max
#undef STREAM_SEEK_END
#undef UUID
#undef uuid_t

#if FOUNDATION_COMPILER_CLANG
#  undef WINAPI
#  define WINAPI STDCALL
#endif

#endif
