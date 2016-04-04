/* json.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file json.h
\brief JSON parser

Small in-place JSON parser without any allocation. Entry points for both
standard JSON and simplified JSON data parsing. All character data must be
in UTF-8 format.

Simplified JSON as parsed by this library has the following differences
from standard JSON:
- The equal sign = is used to define key-value pairs instead of the colon :
- Quotes around string keys in key-value pairs are optional, unless you need
the key to contain either spaces or the equal sign =
- Commas are optional in object and array definitions
- Each SJSON file is always interpreted as a definition for a single object.
You can think of this as an implicit set of curly quotes { ... } that surround
the contents of the file */

#include <foundation/platform.h>
#include <foundation/types.h>
#include <foundation/string.h>

/*! Parse a JSON blob. Number of parsed tokens can be greater than the supplied
capacity to indicate the need for additional capacity for a full parse. Note that
string identifiers and values are in escaped form.
\param buffer Data buffer
\param size Size of data buffer
\param tokens Token array
\param capacity Capacity of token array (number of tokens)
\return Number of parsed tokens, 0 if error */
FOUNDATION_API size_t
json_parse(const char* buffer, size_t size, json_token_t* tokens, size_t capacity);

/*! Parse a simplified JSON blob. Number of parsed tokens can be greater than the supplied
capacity to indicate the need for additional capacity for a full parse. Not that
string identifiers and values are in escaped form.
\param buffer Data buffer
\param size Size of data buffer
\param tokens Token array
\param capacity Capacity of token array (number of tokens)
\return Number of parsed tokens, 0 if error */
FOUNDATION_API size_t
sjson_parse(const char* buffer, size_t size, json_token_t* tokens, size_t capacity);

/*! Convenience function to get identifier string. Not that identifier string
is in escaped form, use json_unescape to get translated string.
\param buffer Data buffer
\param token JSON token
\return Identifier string */
static FOUNDATION_FORCEINLINE string_const_t
json_token_identifier(const char* buffer, json_token_t* token) {
	return string_const(buffer + token->id, token->id_length);
}

/*! Convenience function to get value string. Note that value string is
in escaped form, use json_unesapce to get translated string.
\param buffer Data buffer
\param token JSON token
\return Value string */
static FOUNDATION_FORCEINLINE string_const_t
json_token_value(const char* buffer, json_token_t* token) {
	return string_const(buffer + token->value, token->value_length);
}

/*! Function to unescape a JSON identifier or value string. Buffer can be
pointing to same memory area as string (in-place unescaping).
\param buffer Output buffer
\param capacity Capacity of output buffer
\param string Input string identifier or value
\param length Length of input string
\return Unescaped string in buffer */
FOUNDATION_API string_t
json_unescape(char* buffer, size_t capacity, const char* string, size_t length);

/*! Function to escape a JSON identifier or value string
\param buffer Output buffer
\param capacity Capacity of output buffer
\param string Input string identifier or value
\param length Length of input string
\return Escaped string in buffer */
FOUNDATION_API string_t
json_escape(char* buffer, size_t capacity, const char* string, size_t length);
