/* path.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


FOUNDATION_API string_const_t   path_base_file_name( const char* path, size_t length );
FOUNDATION_API string_const_t   path_base_file_name_with_directory( const char* path, size_t length );
FOUNDATION_API string_const_t   path_file_extension( const char* path, size_t length );
FOUNDATION_API string_const_t   path_file_name( const char* path, size_t length );
FOUNDATION_API string_const_t   path_directory_name( const char* path, size_t length );
FOUNDATION_API string_const_t   path_subdirectory_name( const char* path, size_t length, const char* root, size_t root_length );
FOUNDATION_API string_const_t   path_protocol( const char* uri, size_t length );

FOUNDATION_API string_t         path_merge( char* dest, size_t capacity, bool reallocate, const char* first, size_t first_length, const char* second, size_t second_length );
FOUNDATION_API string_t         path_merge_varg( char* dest, size_t capacity, bool reallocate, const char* first, size_t first_length, ... ) FOUNDATION_ATTRIBUTE( sentinel );
FOUNDATION_API string_t         path_merge_vlist( char* dest, size_t capacity, bool reallocate, const char* first, size_t first_length, va_list list );
FOUNDATION_API string_t         path_append( char* base, size_t base_length, size_t base_capacity, bool reallocate, const char* tail, size_t tail_length );
FOUNDATION_API string_t         path_append_varg( char* base, size_t base_length, size_t base_capacity, bool reallocate, const char* tail, size_t tail_length, ... ) FOUNDATION_ATTRIBUTE( sentinel );
FOUNDATION_API string_t         path_append_vlist( char* base, size_t base_length, size_t base_capacity, bool reallocate, const char* tail, size_t tail_length, va_list list );
FOUNDATION_API string_t         path_prepend( char* tail, size_t tail_length, size_t tail_capacity, bool reallocate, const char* base, size_t base_length );
FOUNDATION_API string_t         path_prepend_varg( char* tail, size_t tail_length, size_t tail_capacity, bool reallocate, const char* base, size_t base_length, ... ) FOUNDATION_ATTRIBUTE( sentinel );
FOUNDATION_API string_t         path_prepend_vlist( char* tail, size_t tail_length, size_t tail_capacity, bool reallocate, const char* base, size_t base_length, va_list list );
FOUNDATION_API string_t         path_absolute( char* path, size_t length, size_t capacity, bool reallocate );
FOUNDATION_API string_t         path_clean( char* path, size_t length, size_t capacity, bool reallocate );

FOUNDATION_API bool             path_is_absolute( const char* path, size_t length );

FOUNDATION_API string_t         path_make_temporary( char* buffer, size_t capacity, bool reallocate );

