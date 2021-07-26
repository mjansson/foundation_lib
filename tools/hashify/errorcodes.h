/* errorcodes.h  -  Foundation library tools  -  Public Domain  -  2013 Mattias Jansson
 *
 * This library provides a cross-platform foundation library in C11 providing basic support data types and
 * functions to write applications and games in a platform-independent fashion. The latest source code is
 * always available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

// Error codes returned by hashify tool
#define HASHIFY_RESULT_OK 0
#define HASHIFY_RESULT_MISSING_INPUT_FILE -1
#define HASHIFY_RESULT_MISSING_OUTPUT_FILE -2
#define HASHIFY_RESULT_OUTPUT_FILE_OUT_OF_DATE -3
#define HASHIFY_RESULT_HASH_STRING_MISMATCH -4
#define HASHIFY_RESULT_HASH_MISMATCH -5
#define HASHIFY_RESULT_HASH_MISSING -6
#define HASHIFY_RESULT_HASH_COLLISION -7
#define HASHIFY_RESULT_STRING_COLLISION -8
#define HASHIFY_RESULT_EXTRA_STRING -9
#define HASHIFY_RESULT_OUTPUT_FILE_WRITE_FAIL -10
