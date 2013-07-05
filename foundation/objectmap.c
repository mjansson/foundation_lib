/* objectmap.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


objectmap_t* objectmap_allocate( unsigned int size )
{
	uint64_t bits;
	unsigned int ip;
	uintptr_t next_indexshift;
	objectmap_t* map;
	void** slot;

	FOUNDATION_ASSERT_MSG( size > 2, "Invalid objectmap size" );
	if( size <= 2 )
		size = 2;
	bits = math_round( math_log2( (real)size ) ); //Number of bits needed
	FOUNDATION_ASSERT_MSGFORMAT( bits < 50, "Invalid objectmap size %d", size );

	map = memory_allocate_zero( sizeof( objectmap_t ) + ( sizeof( void* ) * size ), 16, MEMORY_PERSISTENT );
	map->size_bits   = bits;
	map->id_max      = ((1ULL<<(62ULL-bits))-1);
	map->size        = size;
	map->mask_index  = ((1ULL<<bits)-1ULL);
	map->mask_id     = ( 0x3FFFFFFFFFFFFFFFULL & ~map->mask_index );
	map->free        = 0;
	map->id          = 1;

	slot = map->map;
	for( ip = 0, next_indexshift = 3; ip < ( size - 1 ); ++ip, next_indexshift += 2, ++slot )
		*slot = (void*)next_indexshift;
	*slot = (void*)((uintptr_t)-1);
	
	return map;
}


void objectmap_deallocate( objectmap_t* map )
{
	uint64_t i;

	if( !map )
		return;

	for( i = 0; i < map->size; ++i )
	{
		bool is_object = !( (uintptr_t)map->map[i] & 1 );
		if( is_object )
		{
			log_errorf( ERROR_MEMORY_LEAK, "Object still stored in objectmap when map deallocated" );
			break;
		}
	}
	
	memory_deallocate( map );
}


unsigned int objectmap_size( const objectmap_t* map )
{
	FOUNDATION_ASSERT( map );
	return (unsigned int)map->size;
}


void* objectmap_raw_lookup( const objectmap_t* map, unsigned int idx )
{
	uintptr_t ptr;

	/*lint --e{613} Performance path (no ptr checks)*/
	FOUNDATION_ASSERT( map );
	FOUNDATION_ASSERT( idx < map->size );
	ptr = (uintptr_t)map->map[idx];
	return ( ptr & 1 ) ? 0 : (void*)ptr;
}


object_t objectmap_reserve( objectmap_t* map )
{
	uint64_t idx, next, id;

	FOUNDATION_ASSERT( map ); /*lint -esym(613,pool) */
	
	//Reserve spot in array
	//TODO: Look into double-ended implementation with allocation from tail and free push to head
	do
	{
		idx = map->free;
		if( idx >= map->size )
		{
			log_errorf( ERROR_OUT_OF_MEMORY, "Pool full, unable to reserve id" );
			return 0;
		}
		next = ((uintptr_t)map->map[idx]) >> 1;
	} while( !atomic_cas64( (volatile int64_t*)&map->free, next, idx ) );
	
	//Sanity check that slot isn't taken
	FOUNDATION_ASSERT_MSG( (intptr_t)(map->map[idx]) & 1, "Map failed sanity check, slot taken after reserve" );
	map->map[idx] = 0;
	
	//Allocate ID
	id = 0;
	do
	{
		id = atomic_incr64( (volatile int64_t*)&map->id ) & map->id_max; //Wrap-around handled by masking
	} while( !id );

	//Make sure id stays within correct bits (if fails, check objectmap allocation and the mask setup there)
	FOUNDATION_ASSERT( ( ( id << map->size_bits ) & map->mask_id ) == ( id << map->size_bits ) );
	
	return ( id << map->size_bits ) | idx; /*lint +esym(613,pool) */
}


void objectmap_free( objectmap_t* map, object_t id )
{
	uint64_t idx, last;

	FOUNDATION_ASSERT( map ); /*lint -esym(613,pool) */
	
	idx = (intptr_t)( id & map->mask_index );
	if( (uintptr_t)map->map[idx] & 1 )
		return; //Already free

	do
	{
		last = (uint64_t)map->free;
		map->map[idx] = (void*)((uintptr_t)(last<<1)|1);
	} while( !atomic_cas64( (volatile int64_t*)&map->free, idx, last ) ); /*lint +esym(613,pool) */
}


void objectmap_set( objectmap_t* map, object_t id, void* object )
{
	uint64_t idx;

	FOUNDATION_ASSERT( map ); /*lint -esym(613,pool) */
	
	idx = (int)( id & map->mask_index );
	//Sanity check, can't set free slot, and non-free slot should be initialized to 0 in reserve function
	FOUNDATION_ASSERT( !(((uintptr_t)map->map[idx]) & 1 ) );
	FOUNDATION_ASSERT( !((uintptr_t)map->map[idx]) );
	if( !map->map[idx] )
		map->map[idx] = object;
	/*lint +esym(613,pool) */
}

