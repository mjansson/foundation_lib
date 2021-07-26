/* regex.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file regex.h
\brief Small regular expression implementation

Small regular expression implementation matching a subset of Perl regular expression syntax.
<pre>    ^        Match beginning of string
    $        Match end of string
    .        Match one character
    ()       Group and capture substring
    []       Match any character from the grouped set
    [^]      Match any character but ones from the grouped set
    \\s       Match whitespace
    \\S       Match non-whitespace
    \\d       Match decimal digits
    \\D       Match non-digits
    \\n       Match newline
    \\r       Match carriage return
    +        Match one or more times (longest possible match)
    +?       Match one or more times (shortest possible match)
    *        Match zero or more times (longest possible match)
    *?       Match zero or more times (shortest possible match)
    ?        Match zero or once
    \\XX      Match byte with hex value 0xXX (must be two hex digits)
    \\meta    Match one of the meta characters ^$()[].*+?|\
</pre> */

#include <foundation/platform.h>

/*! Compile (allocate and parse) a regular expression
\param pattern Pattern string
\param length Length of pattern string
\return Compiled expression, null if error */
FOUNDATION_API regex_t*
regex_compile(const char* pattern, size_t length);

/*! Compile (parse) a regular expression into a predefined expression buffer
\param regex Predefined expression buffer
\param pattern Pattern string
\param length Length of pattern string
\return true if successful, false if not */
FOUNDATION_API bool
regex_parse(regex_t* regex, const char* pattern, size_t length);

/*! Match input string with regular expression with optional captures. Note that captures array
might be modified and contain invalid data even if regex fails. If the regex matches, the
captures array will contain valid data.
\param regex Compiled expression
\param input Input string
\param inlength Length of input string
\param captures Result capture array, null if not wanted
\param maxcaptures Maximum number of captures
\return true if string matches expression, false if not */
FOUNDATION_API bool
regex_match(regex_t* regex, const char* input, size_t inlength, string_const_t* captures, size_t maxcaptures);

/*! Free a compiled expression
\param regex Compiled expression */
FOUNDATION_API void
regex_deallocate(regex_t* regex);
