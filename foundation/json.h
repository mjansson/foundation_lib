/* json.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file json.h
\brief JSON/SJSON parser

Small in-place JSON parser without any allocation. Entry points for both
standard JSON and simplified JSON data parsing. All character data must be
in UTF-8 format.

Strings are not automatically unescaped. Use json_unescape/json_escape to
perform unescaping and espacing of strings. Unescaping can be done in-place
to avoid memory allocations.

Simplified JSON as parsed by this library has the following differences
from standard JSON:
- The equal sign = is used to define key-value pairs instead of the colon :
- Quotes around string keys in key-value pairs are optional, unless you need
the key to contain either whitespace or the equal sign =
- Commas are optional in object and array definitions
- Each SJSON file is always interpreted as a definition for a single object.
You can think of this as an implicit set of curly quotes { ... } that surround
the contents of the file

Kudos to Niklas Gray for SJSON syntax,
http://bitsquid.blogspot.se/2009/10/simplified-json-notation.html
*/

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
json_token_identifier(const char* buffer, const json_token_t* token);

/*! Convenience function to get value string. Note that value string is
in escaped form, use json_unesapce to get translated string.
\param buffer Data buffer
\param token JSON token
\return Value string */
static FOUNDATION_FORCEINLINE string_const_t
json_token_value(const char* buffer, const json_token_t* token);

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

/*! Utility function to load a stream into a temporary buffer, parse
the read data and call a handler with the found tokens.
\param path Stream path
\param length Length of stream path
\param handler Handler function
\return Number of tokens processed */
FOUNDATION_API size_t
sjson_parse_path(const char* path, size_t length, json_handler_fn handler);

// Implementations

static FOUNDATION_FORCEINLINE string_const_t
json_token_identifier(const char* buffer, const json_token_t* token) {
	return string_const(buffer + token->id, token->id_length);
}

static FOUNDATION_FORCEINLINE string_const_t
json_token_value(const char* buffer, const json_token_t* token) {
	return string_const(buffer + token->value, token->value_length);
}
