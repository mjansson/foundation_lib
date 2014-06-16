/* regex.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file regex.h
    Small regular expression implementation */

#include <foundation/platform.h>


/*! Precompile a regular expression
    \param pattern                   Pattern string
    \return                          Compiled expression, null if error */
FOUNDATION_API regex_t*              regex_compile( const char* pattern );

/*! Match input string with regular expression
    \param regex                     Compiled expression
    \param input                     Input string
    \param inlength                  Input length (0 for input string length)
	\param captures                  Result capture array, null if not wanted
    \param maxcaptures               Maximum number of captures
    \return                          true if string matches expression, false if not */
FOUNDATION_API bool                  regex_match( regex_t* regex, const char* input, int inlength, regex_capture_t* captures, int maxcaptures );

/*! Free a compiled expression
    \param regex                     Compiled expression */
FOUNDATION_API void                  regex_free( regex_t* regex );

