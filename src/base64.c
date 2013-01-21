/* base64.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include <foundation.h>


/*lint -e{840}  We use null character in string literal deliberately here*/
static const char _base64_decode[] = "|\0\0\0}rstuvwxyz{\0\0\0\0\0\0\0>?@ABCDEFGHIJKLMNOPQRSTUVW\0\0\0\0\0\0XYZ[\\]^_`abcdefghijklmnopq";
static const char _base64_code[]   = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


char* encode_base64( const void* arr, unsigned int size )
{
	char* buffer, *ptr;
	const unsigned char* carr;
	unsigned char bits;
	unsigned int len = ( size / 3 ) * 4;
	if( size % 3 )
		len += 4;
	carr = (const unsigned char*)arr;
	buffer = memory_allocate( (uint64_t)len + 1U, 0, MEMORY_PERSISTENT );
	ptr = buffer;
	while( size > 2 )
	{
		bits = ( *carr >> 2 ) & 0x3F; *ptr++ = _base64_code[bits];
		bits = (unsigned char)( ( *carr & 0x3 ) << 4 ) | ( ( *( carr + 1 ) >> 4 ) & 0xF ); *ptr++ = _base64_code[bits];
		bits = (unsigned char)( ( *( carr + 1 ) & 0xF ) << 2 ) | ( ( *( carr + 2 ) >> 6 ) & 0x3 ); *ptr++ = _base64_code[bits];
		bits = *( carr + 2 ) & 0x3F; *ptr++ = _base64_code[bits];
		size -= 3;
		carr += 3;
	}
	if( size == 2 )
	{
		bits = ( *carr >> 2 ) & 0x3F; *ptr++ = _base64_code[bits];
		bits = (unsigned char)( ( *carr & 0x3 ) << 4 ) | ( ( *( carr + 1 ) >> 4 ) & 0xF ); *ptr++ = _base64_code[bits];
		bits = (unsigned char)( ( *( carr + 1 ) & 0xF ) << 2 ); *ptr++ = _base64_code[bits];
		*ptr++ = '=';
	}
	else if( size == 1 )
	{
		bits = ( *carr >> 2 ) & 0x3F; *ptr++ = _base64_code[bits];
		bits = (unsigned char)( ( *carr & 0x3 ) << 4 ); *ptr++ = _base64_code[bits];
		*ptr++ = '=';
		*ptr++ = '=';
	}
	buffer[len] = 0;
	return buffer;
}


#define _decodeblock_base64( in, out ) \
    out[ 0 ] = (char)( in[0] << 2 | in[1] >> 4 ); \
    out[ 1 ] = (char)( in[1] << 4 | in[2] >> 2 ); \
    out[ 2 ] = (char)( ( ( in[2] << 6 ) & 0xc0 ) | in[3] );

void decode_base64( const char* data, void* arr, unsigned int* size )
{
	unsigned int i, length, blocksize;
	char* dst = (char*)arr;
	if( size )
		*size = 0;
	if( !data || !arr )
		return;
	length = (unsigned int)string_length( data );
	while( length )
	{
		unsigned char in[4] = { 0, 0, 0, 0 }; //Always build blocks of 4 bytes to decode, pad with 0
		blocksize = 0;
		for( i = 0; length && ( i < 4 ); i++ )
		{
			char v = 0;
			while( length && !v ) //Consume one valid byte from input, discarding invalid data
			{
				v = *data++;
				v = ( ( v < 43 || v > 122 ) ? 0 : _base64_decode[ v - 43 ] );
				if( v )
				{
					in[i] = (unsigned char)( v - 62 );
					blocksize++;
				}
				--length;
			}
		}
		if( blocksize )
		{
			char out[3];
			_decodeblock_base64( in, out );
			for( i = 0; i < blocksize - 1; ++i )
				*dst++ = out[i];
		}
	}
	if( size )
		*size = (unsigned int)( dst - (char*)arr );
}
