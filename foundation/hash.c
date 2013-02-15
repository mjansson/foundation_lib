/* hash.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#if FOUNDATION_COMPILER_MSVC
#  pragma intrinsic(_rotl)
#  pragma intrinsic(_rotl64)
#elif FOUNDATION_COMPILER_GCC || FOUNDATION_COMPILER_CLANG
#  define _rotl64(a, bits) (((a) << (bits)) | ((a) >> (64 - (bits))))
#endif

#define HASH_SEED 0xbaadf00d


static FORCEINLINE PURECALL uint64_t getblock ( const uint64_t* RESTRICT p, const unsigned int i );
static FORCEINLINE CONSTCALL uint64_t fmix64( uint64_t k );


//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

static FORCEINLINE uint64_t getblock( const uint64_t* RESTRICT p, const unsigned int i )
{
#if FOUNDATION_PLATFORM_ENDIAN_LITTLE
	return p[i];
#else
	return swap_byteorder64( p[i] );
#endif
}


//----------
// Block mix - combine the key bits with the hash bits and scramble everything
#define bmix64( h1, h2, k1, k2, c1, c2 ) \
	k1 *= c1; \
	k1  = _rotl64(k1,23); \
	k1 *= c2; \
	h1 ^= k1; \
	h1 += h2; \
	h2 = _rotl64(h2,41); \
	k2 *= c2; \
	k2  = _rotl64(k2,23); \
	k2 *= c1; \
	h2 ^= k2; \
	h2 += h1; \
	h1 = h1*3+0x52dce729; \
	h2 = h2*3+0x38495ab5; \
	c1 = c1*5+0x7b7d159c; \
	c2 = c2*5+0x6bce6396;

//----------
// Finalization mix - avalanches all bits to within 0.05% bias
static FORCEINLINE uint64_t fmix64( uint64_t k )
{
	k ^= k >> 33;
	k *= 0xff51afd7ed558ccd;
	k ^= k >> 33;
	k *= 0xc4ceb9fe1a85ec53;
	k ^= k >> 33;

	return k;
}


hash_t hash( const void* key, const unsigned int len )
{
	const unsigned int nblocks = len / 16;
	const uint64_t* blocks;
	unsigned int i;
	const uint8_t* tail;
	uint64_t k1;
	uint64_t k2;

	uint64_t h1 = 0x9368e53c2f6af274ULL ^ HASH_SEED;
	uint64_t h2 = 0x586dcd208f7cd3fdULL ^ HASH_SEED;

    /*lint -esym(438,c1,c2) Last value of c1 and c2 not used */
	uint64_t c1 = 0x87c37b91114253d5ULL;
	uint64_t c2 = 0x4cf5ad432745937fULL;

	//----------
	// body
	FOUNDATION_ASSERT_PLATFORM_ALIGNMENT( key, 8 );

	//Ok to cast to u64, alignment is assured on required platforms at this point
	blocks = (const uint64_t*)key; /*lint !e826 Ok, loop below will not access data outside scope*/

	for( i = 0; i < nblocks; ++i )
	{
		k1 = getblock(blocks,i*2);
		k2 = getblock(blocks,i*2+1);

		bmix64(h1,h2,k1,k2,c1,c2); 
	}

	//----------
	// tail

	tail = pointer_offset_const( key, nblocks * 16 );

	k1 = 0;
	k2 = 0;

	switch(len & 15) /*lint -save -e616 -e825 -e744 */
	{
	case 15: k2 ^= ((uint64_t)tail[14]) << 48;
	case 14: k2 ^= ((uint64_t)tail[13]) << 40;
	case 13: k2 ^= ((uint64_t)tail[12]) << 32;
	case 12: k2 ^= ((uint64_t)tail[11]) << 24;
	case 11: k2 ^= ((uint64_t)tail[10]) << 16;
	case 10: k2 ^= ((uint64_t)tail[ 9]) << 8;
	case  9: k2 ^= ((uint64_t)tail[ 8]);

	case  8: k1 ^= ((uint64_t)tail[ 7]) << 56;
	case  7: k1 ^= ((uint64_t)tail[ 6]) << 48;
	case  6: k1 ^= ((uint64_t)tail[ 5]) << 40;
	case  5: k1 ^= ((uint64_t)tail[ 4]) << 32;
	case  4: k1 ^= ((uint64_t)tail[ 3]) << 24;
	case  3: k1 ^= ((uint64_t)tail[ 2]) << 16;
	case  2: k1 ^= ((uint64_t)tail[ 1]) << 8;
	case  1: k1 ^= ((uint64_t)tail[ 0]);
	         bmix64(h1,h2,k1,k2,c1,c2);
	}; /*lint -restore */

	//----------
	// finalization

	h2 ^= len;

	h1 += h2;
	h2 += h1;

	h1 = fmix64(h1);
	h2 = fmix64(h2);

	h1 += h2;
	/* We only need 64-bit part
	h2 += h1;

	((uint64_t*)out)[0] = h1;
	((uint64_t*)out)[1] = h2;*/
	return h1;
}


#if !BUILD_DEPLOY && FOUNDATION_PLATFORM_FAMILY_DESKTOP

#define HASH_STRING_BUCKETS		17
#define HASH_STRING_KEY_BUCKETS	7

typedef struct ALIGN(16) _hash_string
{
	hash_t              value;
	char*               string;
} hash_string_t;

typedef struct ALIGN(16) _hash_string_bucket
{
	hash_string_t*      key[HASH_STRING_KEY_BUCKETS];
} hash_string_bucket_t;

FOUNDATION_COMPILETIME_ASSERT( ( sizeof( hash_string_t ) % 16 ) == 0, hash_string_align );
FOUNDATION_COMPILETIME_ASSERT( ( sizeof( hash_string_bucket_t ) % 16 ) == 0, hash_string_bucket_align );

static hash_string_bucket_t* _hash_string_bucket[HASH_STRING_BUCKETS] = {0};


void _static_hash_cleanup( void )
{
	int ib, ik, ia, asize;
	for( ib = 0; ib < HASH_STRING_BUCKETS; ++ib )
	{
		hash_string_bucket_t* bucket = _hash_string_bucket[ ib ];
		if( !bucket )
			continue;

		for( ik = 0; ik < HASH_STRING_KEY_BUCKETS; ++ik )
		{
			hash_string_t* key_array = bucket->key[ ik ];
			for( ia = 0, asize = array_size( key_array ); ia < asize; ++ia )
				memory_deallocate( key_array[ia].string );
			array_deallocate( key_array );
		}

		memory_deallocate( bucket );
	}

	memset( _hash_string_bucket, 0, sizeof( hash_string_bucket_t* ) * HASH_STRING_BUCKETS );
}


void _static_hash_store( const void* key, const unsigned int len, const hash_t value )
{
	hash_string_t new_string;
	hash_string_bucket_t* bucket;
	hash_string_t* key_array;
	const char* key_string;
	int ia, asize;

	//TODO: Thread safeness
	bucket = _hash_string_bucket[ value % HASH_STRING_BUCKETS ];
	if( !bucket )
	{
		bucket = memory_allocate_zero( sizeof( hash_string_bucket_t ), 16, MEMORY_PERSISTENT );
		_hash_string_bucket[ value % HASH_STRING_BUCKETS ] = bucket;
	}

	key_string = (const char*)key;
	key_array = bucket->key[ value % HASH_STRING_KEY_BUCKETS ];
	for( ia = 0, asize = array_size( key_array ); ia < asize; ++ia )
	{
		if( key_array[ia].value == value )
		{
			FOUNDATION_ASSERT_MSG( string_equal_substr( key_array[ia].string, key_string, len ), "Static hash collision" );
			FOUNDATION_ASSERT_MSG( string_length( key_array[ia].string ) == len, "Static hash collision" );
			return;
		}
	}

	new_string.value = value;
	new_string.string = string_substr( key_string, 0, len );
	array_push_memcpy( key_array, &new_string );
	bucket->key[ value % HASH_STRING_KEY_BUCKETS ] = key_array;
}


const char* hash_to_string( const hash_t value )
{
	int ia, asize;
	hash_string_t* key_array;
	hash_string_bucket_t* bucket;
	
	bucket = _hash_string_bucket[ value % HASH_STRING_BUCKETS ];
	if( !bucket )
		return 0;
	key_array = bucket->key[ value % HASH_STRING_KEY_BUCKETS ];
	for( ia = 0, asize = array_size( key_array ); ia < asize; ++ia )
	{
		if( key_array[ia].value == value )
			return key_array[ia].string;
	}
	return 0;
}


#endif
