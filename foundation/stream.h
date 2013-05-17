/* stream.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file stream.h
    Base abstraction of I/O streams */

#include <foundation/platform.h>
#include <foundation/types.h>


/*! Open stream
    \param path                  Path */
FOUNDATION_API stream_t*         stream_open( const char* path, unsigned int mode );

/*! Clone a stream (if supported by stream type)
    \param stream                Stream
    \return                      Cloned stream */
FOUNDATION_API stream_t*         stream_clone( stream_t* stream );

/*! Deallocate stream */
FOUNDATION_API void              stream_deallocate( stream_t* stream );

/*! Get current stream position
    \return                      Position in stream, -1 if error (invalid stream) */
FOUNDATION_API int64_t           stream_tell( stream_t* stream );

/*! Reposition the stream position according to offset and direction.
	Sequential streams can only be repositioned with a positive offset
	and CURRENT direction.
	\param offset                Offset in bytes
	\param direction             Seek direction */
FOUNDATION_API void              stream_seek( stream_t* stream, int64_t offset, stream_seek_mode_t direction );

/*! Query if the stream is at the end
	\return                      true if at end of stream, false if not */
FOUNDATION_API bool              stream_eos( stream_t* stream );

/*! Set byte order of stream data. Only used in binary mode I/O.
	\param byteorder             New byte order */
FOUNDATION_API void              stream_set_byteorder( stream_t* stream, byteorder_t byteorder );

/*! Set binary mode of stream
	\param binary                Binary if true, ASCII if false */
FOUNDATION_API void              stream_set_binary( stream_t* stream, bool binary );

/*! Query if stream is in binary mode
	\return                      True if binary, false if ASCII */
FOUNDATION_API bool              stream_is_binary( const stream_t* stream );

/*! Query if stream is sequential (only forward seeking possible). Network streams are
	generally sequential.
	\return                      true if stream is sequential, false if random access capable */
FOUNDATION_API bool              stream_is_sequential( const stream_t* stream );

/*! Query if stream is reliable (data written guaranteed to be readable). Network streams
    over an UDP socket are unreliable.
    \return                      true if stream is reliable, false if unreliable */
FOUNDATION_API bool              stream_is_reliable( const stream_t* stream );

/*! Query if stream is in-order (data written guaranteed to be read back in the same order). Network streams
    over an UDP socket are out-of-order.
    \return                      true if stream is reliable, false if unreliable */
FOUNDATION_API bool              stream_is_inorder( const stream_t* stream );

/*! Query if bytes must be swapped when reading from stream (byte order of file does not match byte order of system
	\return                      true if bytes must be swapped, false if not */
FOUNDATION_API bool              stream_is_swapped( const stream_t* stream );
	
/*! \return                      Current byte order of stream data */
FOUNDATION_API byteorder_t       stream_byteorder( const stream_t* stream );

/*! \return                      Stream path */
FOUNDATION_API const char*       stream_path( const stream_t* stream );

/*! \return                      Last modified timestamp, 0 if not open or date not meaningful for stream type */
FOUNDATION_API uint64_t          stream_last_modified( const stream_t* stream );


/*! Get raw data from stream, disregarding byte order.
    \param buffer                Destination buffer
    \param num_bytes             Number of bytes to read
    \return                      Number of bytes read */
FOUNDATION_API uint64_t          stream_read( stream_t* stream, void* buffer, uint64_t num_bytes );
	
/*! Get line of up to count characters, consuming but discarding delimiter, reading into buffer
    \param dest                  Destination buffer, null if input is to be ignored
    \param count                 Maximum number of characters to extract
    \param delimiter             Delimiter
    \return                      Number of bytes read (including delimiter) */
FOUNDATION_API uint64_t          stream_read_line_buffer( stream_t* stream, char* dest, unsigned int count, char delimiter );

/*! Get line as string, discarding delimiter
    \param delimiter             Delimiter, default '\n'
    \return                      String, must be freed with a call to string_deallocte() */
FOUNDATION_API char*             stream_read_line( stream_t* stream, char delimiter );

/*! Get the size (length) of the stream data in bytes, if available. If the size can't be determined (sequential streams, like network sockets) the method will return 0.
	\return                      Stream size (length), or 0 if unknown */
FOUNDATION_API uint64_t          stream_size( stream_t* stream );

/*! Try to determine if stream is in binary or ASCII mode by reading the given number of bytes and checking if all bytes are in printable ASCII range (byte values [0x20,0x7e] and linefeed/tab). The method will restore the stream position with a call to seek, so using this on a sequential stream is not supported
	\param num                   Number of bytes to read */
FOUNDATION_API void              stream_determine_binary_mode( stream_t* stream, unsigned int num );

/*! Get boolean value from stream
	\return                      Boolean value read, false if error */
FOUNDATION_API bool              stream_read_bool( stream_t* stream );

/*! Get char value from stream
    \return                      Character read, 0 if error */
FOUNDATION_API int8_t            stream_read_int8( stream_t* stream );

/*! Get byte from stream
    \return                      Byte read, 0 if error */
FOUNDATION_API uint8_t           stream_read_uint8( stream_t* stream );

/*! Get short from stream
    \return                      Value read, 0 if error */
FOUNDATION_API int16_t           stream_read_int16( stream_t* stream );

/*! Get unsigned short from stream
	\return                      Value read, 0 if error */
FOUNDATION_API uint16_t          stream_read_uint16( stream_t* stream );

/*! Get integer from stream
    \return                      Value read, 0 if error */
FOUNDATION_API int32_t           stream_read_int32( stream_t* stream );

/*! Get unsigned integer from stream
	\return                      Value read, 0 if error */
FOUNDATION_API uint32_t          stream_read_uint32( stream_t* stream );

/*! Get integer from stream
	\return                      Value read, 0 if error */
FOUNDATION_API int64_t           stream_read_int64( stream_t* stream );

/*! Get unsigned integer from stream
	\return                      Value read, 0 if error */
FOUNDATION_API uint64_t          stream_read_uint64( stream_t* stream );

/*! Get float from stream
	\return                      Value read, 0 if error */
FOUNDATION_API float32_t         stream_read_float32( stream_t* stream );

/*! Get float from stream
	\return                      Value read, 0 if error */
FOUNDATION_API float64_t         stream_read_float64( stream_t* stream );

/*! Get string from stream. Must be freed with a call to string_deallocate()
	\return                      String, 0 if error */
FOUNDATION_API char*             stream_read_string( stream_t* stream );

/*! Get string from stream
    \param buffer                Buffer
    \param size                  Size of buffer, including terminating zero
	\return                      String, 0 if error */
FOUNDATION_API uint64_t          stream_read_string_buffer( stream_t* stream, char* buffer, uint64_t size );

/*! Buffer any pending incoming data (network streams)
    \param stream                Stream */
FOUNDATION_API void              stream_buffer_read( stream_t* stream );

/*! Check number of bytes available (network streams)
    \param stream                Stream
	\return                      Number of bytes that can be read without blocking */
FOUNDATION_API unsigned int      stream_available_read( stream_t* stream );

/*! Get stream md5 digest
    \param stream                Stream
    \return                      md5 digest, 0 if not available for stream type or invalid stream */
FOUNDATION_API uint128_t         stream_md5( stream_t* stream );

/*! Put raw data to stream
    \param p_buffer              Buffer of data to write
    \param numBytes              Number of bytes to write
    \return                      Number of bytes written */
FOUNDATION_API uint64_t          stream_write( stream_t* stream, const void* p_buffer, uint64_t num_bytes );

/*! Put boolean to stream
    \param data                  Boolean to write */
FOUNDATION_API void              stream_write_bool( stream_t* stream, bool data );

/*! Put char to stream
    \param data                  Char to write  */
FOUNDATION_API void              stream_write_int8( stream_t* stream, int8_t data );

/*! Put byte to stream
    \param data                  Byte to write */
FOUNDATION_API void              stream_write_uint8( stream_t* stream, uint8_t data );

/*! Put short to stream
    \param data                  Short to write */
FOUNDATION_API void              stream_write_int16( stream_t* stream, int16_t data );

/*! Put unsigned short to stream
    \param data                  Unsigned short to write */
FOUNDATION_API void              stream_write_uint16( stream_t* stream, uint16_t data );

/*! Put integer to stream
    \param data                  Integer to write */
FOUNDATION_API void              stream_write_int32( stream_t* stream, int32_t data );

/*! Put unsigned integer to stream
    \param data                  Unsigned integer to write */
FOUNDATION_API void              stream_write_uint32( stream_t* stream, uint32_t data );

/*! Put integer to stream
    \param data                  Integer to write */
FOUNDATION_API void              stream_write_int64( stream_t* stream, int64_t data );

/*! Put unsigned integer to stream
    \param data                  Unsigned integer to write */
FOUNDATION_API void              stream_write_uint64( stream_t* stream, uint64_t data );

/*! Put float to stream
    \param data                  Float to write */
FOUNDATION_API void              stream_write_float32( stream_t* stream, float32_t data );

/*! Put float to stream
    \param data                  Float to write */
FOUNDATION_API void              stream_write_float64( stream_t* stream, float64_t data );

/*! Put string to stream
    \param data                  String to write */
FOUNDATION_API void              stream_write_string( stream_t* stream, const char* data );

/*! Put an endline character to the stream and flush
    \param stream                Stream */
FOUNDATION_API void              stream_write_endl( stream_t* stream );

/*! Write a formatted string to stream
    \param stream                Stream */
FOUNDATION_API void              stream_write_format( stream_t* stream, const char* format, ... );

/*! Truncate stream to given size if it is larger, do nothing if smaller or equal in size
    \param length                New length of stream */
FOUNDATION_API void              stream_truncate( stream_t* stream, uint64_t length );

/*! Flush the stream */
FOUNDATION_API void              stream_flush( stream_t* stream );

