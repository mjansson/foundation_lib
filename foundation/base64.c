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

#include <foundation/foundation.h>


/*lint -e{840}  We use null character in string literal deliberately here*/
static const char _base64_decode[] = "|\0\0\0}rstuvwxyz{\0\0\0\0\0\0\0>?@ABCDEFGHIJKLMNOPQRSTUVW\0\0\0\0\0\0XYZ[\\]^_`abcdefghijklmnopq";
static const char _base64_encode[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


size_t base64_encode( const void* src, size_t srcsize, char* dst, size_t dstsize )
{
	char* ptr;
	const unsigned char* carr;
	unsigned char bits;

	if( dstsize > 0 )
	{
		size_t maxsrcsize = ( ( dstsize - 1 ) / 4 ) * 3;
		if( maxsrcsize < srcsize )
			srcsize = maxsrcsize;
	}
	else
	{
		return 0;
	}

	carr = (const unsigned char*)src;
	ptr = dst;
	while( srcsize > 2 )
	{
		bits = ( *carr >> 2 ) & 0x3F; *ptr++ = _base64_encode[bits];
		bits = (unsigned char)( ( *carr & 0x3 ) << 4 ) | ( ( *( carr + 1 ) >> 4 ) & 0xF ); *ptr++ = _base64_encode[bits];
		bits = (unsigned char)( ( *( carr + 1 ) & 0xF ) << 2 ) | ( ( *( carr + 2 ) >> 6 ) & 0x3 ); *ptr++ = _base64_encode[bits];
		bits = *( carr + 2 ) & 0x3F; *ptr++ = _base64_encode[bits];
		srcsize -= 3;
		carr += 3;
	}
	if( srcsize == 2 )
	{
		bits = ( *carr >> 2 ) & 0x3F; *ptr++ = _base64_encode[bits];
		bits = (unsigned char)( ( *carr & 0x3 ) << 4 ) | ( ( *( carr + 1 ) >> 4 ) & 0xF ); *ptr++ = _base64_encode[bits];
		bits = (unsigned char)( ( *( carr + 1 ) & 0xF ) << 2 ); *ptr++ = _base64_encode[bits];
		*ptr++ = '=';
	}
	else if( srcsize == 1 )
	{
		bits = ( *carr >> 2 ) & 0x3F; *ptr++ = _base64_encode[bits];
		bits = (unsigned char)( ( *carr & 0x3 ) << 4 ); *ptr++ = _base64_encode[bits];
		*ptr++ = '=';
		*ptr++ = '=';
	}

	*ptr++ = 0;

	return (size_t)pointer_diff( ptr, dst );
}


#define _base64_decodeblock( in, out ) \
    out[ 0 ] = (char)( in[0] << 2 | in[1] >> 4 ); \
    out[ 1 ] = (char)( in[1] << 4 | in[2] >> 2 ); \
    out[ 2 ] = (char)( ( ( in[2] << 6 ) & 0xc0 ) | in[3] );

size_t base64_decode( const char* src, size_t srcsize, void* dst, size_t dstsize )
{
	size_t i, length, blocksize;
	char* cdst = (char*)dst;
	char* cdstend = cdst + dstsize;
	length = srcsize ? srcsize : string_length( src );
	while( length )
	{
		unsigned char in[4] = { 0, 0, 0, 0 }; //Always build blocks of 4 bytes to decode, pad with 0
		blocksize = 0;
		for( i = 0; length && ( i < 4 ); i++ )
		{
			char v = 0;
			while( length && !v ) //Consume one valid byte from input, discarding invalid data
			{
				v = *src++;
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
			_base64_decodeblock( in, out );
			for( i = 0; ( i < blocksize - 1 ) && ( cdst < cdstend ); ++i )
				*cdst++ = out[i];
		}
	}

	return (size_t)pointer_diff( cdst, dst );
}
