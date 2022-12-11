/* stream.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

/*! \file stream.h
\brief Stream I/O

Base abstraction of I/O streams. */

#include <foundation/platform.h>
#include <foundation/types.h>

/*! Open stream with the given path, which may include a protocol specifier.
\param path Path
\param length Length of path
\param mode Open mode (read, write, ...), see STREAM_[*] mode/flags
\return New stream, null if error or invalid path/mode */
FOUNDATION_API stream_t*
stream_open(const char* path, size_t length, unsigned int mode);

/*! Clone a stream, if supported by stream type
\param stream Stream
\return Cloned stream, null if not clonable */
FOUNDATION_API stream_t*
stream_clone(stream_t* stream);

/*! Copy a stream, if supported by stream type. Copy is binary (no conversion
of data is done).
\param source Source stream
\param destination Destination stream
\return true if success, false if error or unsupported */
FOUNDATION_API bool
stream_copy(stream_t* source, stream_t* destination);

/*! Deallocate stream previously allocated by any stream specific allocation function
or from a call to #stream_open.
\param stream Stream */
FOUNDATION_API void
stream_deallocate(stream_t* stream);

/*! Initialize stream structure with default values
\param stream Stream
\param order Byte order of stream */
FOUNDATION_API void
stream_initialize(stream_t* stream, byteorder_t order);

/*! Finalize stream previously initialized by any stream specific initialization function.
\param stream Stream */
FOUNDATION_API void
stream_finalize(stream_t* stream);

/*! Get current stream position.
\param stream Stream
\return Position in stream, 0 if error or invalid stream */
FOUNDATION_API size_t
stream_tell(stream_t* stream);

/*! Reposition the stream position according to offset and direction.
Sequential streams can only be repositioned with a positive offset
and #STREAM_SEEK_CURRENT direction.
\param stream Stream
\param offset Offset in bytes
\param direction Seek direction, see #stream_seek_mode_t */
FOUNDATION_API void
stream_seek(stream_t* stream, ssize_t offset, stream_seek_mode_t direction);

/*! Query if the stream is at the end
\param stream Stream
\return true if at end of stream, false if not */
FOUNDATION_API bool
stream_eos(stream_t* stream);

/*! Get the size (length) of the stream data in bytes, if available. If the size can't be
determined (sequential streams, like network sockets) the method will return 0.
\param stream Stream
\return Stream size (length), or 0 if unknown */
FOUNDATION_API size_t
stream_size(stream_t* stream);

/*! Set byte order of stream data. Only used in binary mode I/O.
\param stream Stream
\param byteorder New byte order */
FOUNDATION_API void
stream_set_byteorder(stream_t* stream, byteorder_t byteorder);

/*! Set binary mode of stream
\param stream Stream
\param binary Binary if true, ASCII if false */
FOUNDATION_API void
stream_set_binary(stream_t* stream, bool binary);

/*! Try to determine if stream is in binary or ASCII mode by reading the given number of bytes and
checking if all bytes are in printable ASCII range (byte values [0x20,0x7e] and linefeed/tab).
The method will restore the stream position with a call to #stream_seek, so using this on a
sequential stream is not supported.
\param stream Stream
\param size Number of bytes to read */
FOUNDATION_API void
stream_determine_binary_mode(stream_t* stream, size_t size);

/*! Query if stream is in binary mode
\param stream Stream
\return True if binary, false if ASCII */
FOUNDATION_API bool
stream_is_binary(const stream_t* stream);

/*! Query if stream is sequential (only forward seeking possible). Network streams are
generally sequential.
\param stream Stream
\return true if stream is sequential, false if random access capable */
FOUNDATION_API bool
stream_is_sequential(const stream_t* stream);

/*! Query if stream is reliable (data written guaranteed to be readable). Network streams
over an UDP socket are unreliable.
\param stream Stream
\return true if stream is reliable, false if unreliable */
FOUNDATION_API bool
stream_is_reliable(const stream_t* stream);

/*! Query if stream is in-order (data written guaranteed to be read back in the same order).
Network streams over an UDP socket are out-of-order.
\param stream Stream
\return true if stream is reliable, false if unreliable */
FOUNDATION_API bool
stream_is_inorder(const stream_t* stream);

/*! Query if bytes must be swapped when reading from stream (byte order of file does not
match byte order of system).
\param stream Stream
\return true if bytes must be swapped, false if not */
FOUNDATION_API bool
stream_is_swapped(const stream_t* stream);

/*! Get stream byte order.
\param stream Stream
\return Current byte order of stream data */
FOUNDATION_API byteorder_t
stream_byteorder(const stream_t* stream);

/*! Get stream full path including protocol
\param stream Stream
\return Stream path */
FOUNDATION_API string_const_t
stream_path(const stream_t* stream);

/*! Get last modification timestamp.
\param stream Stream
\return Last modified timestamp, 0 if not open or date not meaningful for stream type */
FOUNDATION_API tick_t
stream_last_modified(const stream_t* stream);

/*! Read raw data from stream, disregarding byte order.
\param stream Stream
\param buffer Destination buffer
\param size Number of bytes to read
\return Number of bytes read */
FOUNDATION_API size_t
stream_read(stream_t* stream, void* buffer, size_t size);

/*! Read line of up to count characters, consuming but discarding delimiter, reading into buffer.
\param stream Stream
\param dest Destination buffer, null if input is to be ignored
\param size Maximum number of characters to extract (destination buffer must be large enough)
\param delimiter Delimiter
\return Number of bytes read (including delimiter) */
FOUNDATION_API string_t
stream_read_line_buffer(stream_t* stream, char* dest, size_t size, char delimiter);

/*! Read line as string, discarding delimiter. Allocates a new memory buffer for the string, must
be freed with a call to #string_deallocate
\param stream Stream
\param delimiter Delimiter, default '\n'
\return String read, null string if error or if no bytes could be read from stream */
FOUNDATION_API string_t
stream_read_line(stream_t* stream, char delimiter);

/*! Read boolean value from stream.
\param stream Stream
\return Boolean value read, false if error */
FOUNDATION_API bool
stream_read_bool(stream_t* stream);

/*! Read char value from stream.
\param stream Stream
\return Character read, 0 if error */
FOUNDATION_API int8_t
stream_read_int8(stream_t* stream);

/*! Read byte value from stream.
\param stream Stream
\return Byte read, 0 if error */
FOUNDATION_API uint8_t
stream_read_uint8(stream_t* stream);

/*! Read short value from stream.
\param stream Stream
\return Value read, 0 if error */
FOUNDATION_API int16_t
stream_read_int16(stream_t* stream);

/*! Read unsigned short from stream
\param stream Stream
\return Value read, 0 if error */
FOUNDATION_API uint16_t
stream_read_uint16(stream_t* stream);

/*! Read integer from stream.
\param stream Stream
\return Value read, 0 if error */
FOUNDATION_API int32_t
stream_read_int32(stream_t* stream);

/*! Read unsigned integer from stream.
\param stream Stream
\return Value read, 0 if error */
FOUNDATION_API uint32_t
stream_read_uint32(stream_t* stream);

/*! Read integer from stream.
\param stream Stream
\return Value read, 0 if error */
FOUNDATION_API int64_t
stream_read_int64(stream_t* stream);

/*! Read unsigned integer from stream.
\param stream Stream
\return Value read, 0 if error */
FOUNDATION_API uint64_t
stream_read_uint64(stream_t* stream);

/*! Read 128-bit unsigned integer from stream.
\param stream Stream
\return Value read, 0 if error */
FOUNDATION_API uint128_t
stream_read_uint128(stream_t* stream);

/*! Read 256-bit unsigned integer from stream.
\param stream Stream
\return Value read, 0 if error */
FOUNDATION_API uint256_t
stream_read_uint256(stream_t* stream);

/*! Read 512-bit unsigned integer from stream.
\param stream Stream
\return Value read, 0 if error */
FOUNDATION_API uint512_t
stream_read_uint512(stream_t* stream);

/*! Read UUID from stream.
\param stream Stream
\return Value read, 0 if error */
FOUNDATION_API uuid_t
stream_read_uuid(stream_t* stream);

/*! Read 32-bit float from stream
\param stream Stream
\return Value read, 0 if error */
FOUNDATION_API float32_t
stream_read_float32(stream_t* stream);

/*! Read 64-bit float from stream
\param stream Stream
\return Value read, 0 if error */
FOUNDATION_API float64_t
stream_read_float64(stream_t* stream);

/*! Read string from stream. Must be freed with a call to #string_deallocate
\param stream Stream
\return String, null string if error or if no bytes (or in binary mode, an empty string)
        could be read from stream */
FOUNDATION_API string_t
stream_read_string(stream_t* stream);

/*! Read string from stream into pre-allocated buffer.
\param stream Stream
\param buffer Buffer
\param capacity Size of buffer, including terminating zero
\return String, null string if error */
FOUNDATION_API string_t
stream_read_string_buffer(stream_t* stream, char* buffer, size_t capacity);

/*! If stream is in text mode, skip whitespace characters.
If stream is in binary mode, ignore.
\param stream Stream
\return Number of bytes skipped */
FOUNDATION_API size_t
stream_skip_whitespace(stream_t* stream);

/*! Write raw data to stream.
\param stream Stream
\param buffer Buffer of data to write
\param size Number of bytes to write
\return Number of bytes written */
FOUNDATION_API size_t
stream_write(stream_t* stream, const void* buffer, size_t size);

/*! Write boolean to stream.
\param stream Stream
\param data Boolean to write */
FOUNDATION_API void
stream_write_bool(stream_t* stream, bool data);

/*! Write byte to stream.
\param stream Stream
\param data Char to write */
FOUNDATION_API void
stream_write_int8(stream_t* stream, int8_t data);

/*! Write unsigned byte to stream.
\param stream Stream
\param data Byte to write */
FOUNDATION_API void
stream_write_uint8(stream_t* stream, uint8_t data);

/*! Write short to stream.
\param stream Stream
\param data Short to write */
FOUNDATION_API void
stream_write_int16(stream_t* stream, int16_t data);

/*! Write unsigned short to stream.
\param stream Stream
\param data Unsigned short to write */
FOUNDATION_API void
stream_write_uint16(stream_t* stream, uint16_t data);

/*! Write integer to stream.
\param stream Stream
\param data Integer to write */
FOUNDATION_API void
stream_write_int32(stream_t* stream, int32_t data);

/*! Write unsigned integer to stream.
\param stream Stream
\param data Unsigned integer to write */
FOUNDATION_API void
stream_write_uint32(stream_t* stream, uint32_t data);

/*! Write integer to stream.
\param stream Stream
\param data Integer to write */
FOUNDATION_API void
stream_write_int64(stream_t* stream, int64_t data);

/*! Write unsigned integer to stream.
\param stream Stream
\param data Unsigned integer to write */
FOUNDATION_API void
stream_write_uint64(stream_t* stream, uint64_t data);

/*! Write 128-bit unsigned integer to stream.
\param stream Stream
\param data Unsigned integer to write */
FOUNDATION_API void
stream_write_uint128(stream_t* stream, uint128_t data);

/*! Write 256-bit unsigned integer to stream.
\param stream Stream
\param data Unsigned integer to write */
FOUNDATION_API void
stream_write_uint256(stream_t* stream, uint256_t data);

/*! Write 512-bit unsigned integer to stream.
\param stream Stream
\param data Unsigned integer to write */
FOUNDATION_API void
stream_write_uint512(stream_t* stream, uint512_t data);

/*! Write UUID to stream.
\param stream Stream
\param data UUID to write */
FOUNDATION_API void
stream_write_uuid(stream_t* stream, uuid_t data);

/*! Write 32-bit float to stream.
\param stream Stream
\param data Float to write */
FOUNDATION_API void
stream_write_float32(stream_t* stream, float32_t data);

/*! Write 64-bit float to stream.
\param stream Stream
\param data Float to write */
FOUNDATION_API void
stream_write_float64(stream_t* stream, float64_t data);

/*! Write string to stream.
\param stream Stream
\param data String to write
\param length Length of string (NOT including optional zero terminator) */
FOUNDATION_API void
stream_write_string(stream_t* stream, const char* data, size_t length);

/*! If the stream is in text mode, write a separator character (whitespace) to stream
If the stream is in binary mode, ignore.
\param stream Stream */
FOUNDATION_API void
stream_write_separator(stream_t* stream);

/*! If the stream is in text mode, write an endline character to the stream and flush.
If the stream is in binary mode, only flush.
\param stream Stream */
FOUNDATION_API void
stream_write_endl(stream_t* stream);

/*! Write a formatted string to stream.
\param stream Stream
\param format Format declaration string
\param format_length Length of format declaration string */
FOUNDATION_API void
stream_write_format(stream_t* stream, const char* format, size_t format_length, ...) FOUNDATION_PRINTFCALL(2, 4);

/*! \fn void stream_buffer_read(stream_t* stream)
Buffer any pending incoming data (network streams).
\param stream Stream */
FOUNDATION_API void
stream_buffer_read(stream_t* stream);

/*! Check number of bytes available (network streams).
\param stream Stream
\return Number of bytes that can be read without blocking */
FOUNDATION_API size_t
stream_available_read(stream_t* stream);

/*! Read stream md5 digest. Line ending will be unified and digested as a
UNIX style LF if the stream is in ascii mode.
\param stream Stream
\return md5 digest, 0 if not available for stream type or invalid stream */
FOUNDATION_API uint128_t
stream_md5(stream_t* stream);

/*! Read stream SHA-256 digest. Line ending will be unified and digested as a
UNIX style LF if the stream is in ascii mode.
\param stream Stream
\return SHA-256 digest, 0 if not available for stream type or invalid stream */
FOUNDATION_API uint256_t
stream_sha256(stream_t* stream);

/*! Read stream SHA-512 digest. Line ending will be unified and digested as a
UNIX style LF if the stream is in ascii mode.
\param stream Stream
\return SHA-512 digest, 0 if not available for stream type or invalid stream */
FOUNDATION_API uint512_t
stream_sha512(stream_t* stream);

/*! Truncate stream to given size if it is larger, do nothing if smaller or equal in size.
\param stream Stream
\param length New length of stream */
FOUNDATION_API void
stream_truncate(stream_t* stream, size_t length);

/*! Flush the stream
\param stream Stream */
FOUNDATION_API void
stream_flush(stream_t* stream);

/*! Allocate a stream for stdout
\return Stream wrapping stdout */
FOUNDATION_API stream_t*
stream_open_stdout(void);

/*! Allocate a stream for stderr
\return Stream wrapping stderr */
FOUNDATION_API stream_t*
stream_open_stderr(void);

/*! Allocate a stream for stdin
\return Stream wrapping stdin */
FOUNDATION_API stream_t*
stream_open_stdin(void);

/*! Set function to handle opening streams for the given protocol
\param protocol Protocol
\param length Length of protocol
\param fn Handler function */
FOUNDATION_API void
stream_set_protocol_handler(const char* protocol, size_t length, stream_open_fn fn);

/*! Get the assigned handler for the given protocol
\param protocol Procotol string
\param length Length of protocol string
\return Handler function, 0 if none registered */
FOUNDATION_API stream_open_fn
stream_protocol_handler(const char* protocol, size_t length);

/*! Read and digest the stream content. Will do line ending unification for ascii
mode streams.
\param stream Stream to digest
\param digester Digestion function
\param data User data pointer passed
\return true if digestion succeeded, false if not */
FOUNDATION_API bool
stream_digest(stream_t* stream, void* (*digester)(void*, const void*, size_t), void* data);
