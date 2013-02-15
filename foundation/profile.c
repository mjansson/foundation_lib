/* profile.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


#if BUILD_ENABLE_PROFILE

typedef struct _profile_block_data   profile_block_data;
typedef struct _profile_block        profile_block;

#define MAX_MESSAGE_LENGTH 23

struct _profile_block_data
{
	uint32_t            id;
	uint32_t            parentid;
	uint32_t            processor;
	uint32_t            thread;
	uint64_t            start;
	uint64_t            end;
	char                name[ MAX_MESSAGE_LENGTH + 1 ];
}; //sizeof( profile_block_data ) == 56

struct _profile_block
{
	profile_block_data data;
	uint16_t           previous;
	uint16_t           sibling;
	uint32_t           child;
}; //sizeof( profile_block ) == 64

#define PROFILE_ID_SYSTEMINFO       0
#define PROFILE_ID_LOGMESSAGE       1
#define PROFILE_ID_LOGCONTINUE      2
#define PROFILE_ID_ENDFRAME         3
#define PROFILE_ID_TRYLOCK          4
#define PROFILE_ID_TRYLOCKCONTINUE  5
#define PROFILE_ID_LOCK             6
#define PROFILE_ID_LOCKCONTINUE     7
#define PROFILE_ID_UNLOCK           8
#define PROFILE_ID_UNLOCKCONTINUE   9

#define GET_BLOCK( index )    ( _profile_blocks + (index) )
#define BLOCK_INDEX( block )  (uint16_t)((uintptr_t)( (block) - _profile_blocks ))

static uint32_t             _profile_counter = 0;
static uint32_t             _profile_free = 0;
static profile_block*       _profile_root = 0;
static profile_block*       _profile_blocks = 0;
static uint64_t             _profile_ground_time = 0;
static int                  _profile_enable = 0;
static profile_write_fn     _profile_write = 0;
static int                  _profile_terminate_io = 0;

FOUNDATION_DECLARE_THREAD_LOCAL( uint32_t, profile_block, 0 )


static profile_block* _allocate_profile_block( void )
{
	//Grab block from free list
	profile_block* block;
	uint32_t free_block, next_block;
	do
	{
		free_block = _profile_free;
		next_block = GET_BLOCK( free_block )->child;
	} while( !atomic_cas32( (int32_t*)&_profile_free, next_block, free_block ) );
	block = GET_BLOCK( free_block );
	memset( block, 0, sizeof( profile_block ) );
	return block;
}


static void _free_profile_block( uint32_t block, uint32_t leaf )
{
	uint32_t last;
	do
	{
		last = _profile_free;
		GET_BLOCK( leaf )->child = last;
	} while( !atomic_cas32( (int32_t*)&_profile_free, block, last ) );
}


static void _profile_put_root_block( uint32_t block )
{
	profile_block* self = GET_BLOCK( block );
	do
	{
		self->sibling = _profile_root->child;
	} while( !atomic_cas32( (int32_t*)&_profile_root->child, block, self->sibling ) );
}


static void _profile_put_simple_block( uint32_t block )
{
	//Add to current block, or if no current add to array
	uint32_t parent_block = get_thread_profile_block();
	if( parent_block )
	{
		profile_block* self = GET_BLOCK( block );
		profile_block* parent = GET_BLOCK( parent_block );
		uint32_t next_block = parent->child;
		self->previous = (uint16_t)parent_block;
		self->sibling = (uint16_t)next_block;
		if( next_block )
			GET_BLOCK( next_block )->previous = (uint16_t)block;
		parent->child = block;
	}
	else
	{
		_profile_put_root_block( block );
	}
}


static void _profile_put_message_block( uint32_t id, const char* message )
{
	profile_block* subblock = 0;
	int len = (int)string_length( message );

	//Allocate new master block
	profile_block* block = _allocate_profile_block();
	block->data.id = id;
	block->data.processor = thread_hardware();
	block->data.thread = (uint32_t)thread_id();
	block->data.start  = time_current() - _profile_ground_time;
	block->data.end = atomic_add32( (int32_t*)&_profile_counter, 1 );
	string_copy( block->data.name, message, ( len > MAX_MESSAGE_LENGTH ) ? MAX_MESSAGE_LENGTH : len );

	len -= MAX_MESSAGE_LENGTH;
	message += MAX_MESSAGE_LENGTH;
	subblock = block;

	while( len > 0 )
	{
		//add subblock
		profile_block* cblock = _allocate_profile_block();
		uint16_t cblock_index = BLOCK_INDEX( cblock );
		cblock->data.id = id + 1;
		cblock->data.parentid = (uint32_t)subblock->data.end;
		cblock->data.processor = block->data.processor;
		cblock->data.thread = block->data.thread;
		cblock->data.start  = block->data.start;
		cblock->data.end    = atomic_add32( (int32_t*)&_profile_counter, 1 );
		string_copy( cblock->data.name, message, ( len > MAX_MESSAGE_LENGTH ) ? MAX_MESSAGE_LENGTH : len );

		cblock->sibling = subblock->child;
		if( subblock->child )
			GET_BLOCK( subblock->child )->previous = cblock_index;
		subblock->child = cblock_index;
		cblock->previous = BLOCK_INDEX( subblock );
		subblock = cblock;

		len -= MAX_MESSAGE_LENGTH;
		message += MAX_MESSAGE_LENGTH;
	}

	_profile_put_simple_block( BLOCK_INDEX( block ) );
}


//Pass each block once, writing it to stream and adjusting child/sibling pointers to form a single-linked list through child pointer
static profile_block* _process_profile_block( profile_block* block )
{
	profile_block* leaf = block;

	if( _profile_write )
		_profile_write( block, sizeof( profile_block ) );

	if( block->child )
		leaf = _process_profile_block( GET_BLOCK( block->child ) );
	if( block->sibling && !block->child )
	{
		block->child = block->sibling;
		block->sibling = 0;
		leaf = _process_profile_block( GET_BLOCK( block->child ) );
	}
	if( block->sibling )
	{
		profile_block* subleaf = _process_profile_block( GET_BLOCK( block->sibling ) );
		subleaf->child = block->child;
		block->child = block->sibling;
	}
	return leaf;
}


void* _profile_io( void* arg )
{
	unsigned int system_info_counter = 0;
	profile_block system_info;
	memset( &system_info, 0, sizeof( profile_block ) );
	system_info.data.id = PROFILE_ID_SYSTEMINFO;
	system_info.data.start = time_ticks_per_second();
	string_copy( system_info.data.name, "sysinfo", 7 );

	while( !_profile_terminate_io )
	{
		profile_begin_block( "profile_io" );
		
		if( _profile_root->child )
		{
			uint32_t block;
			profile_begin_block( "process" );
			
			do
			{
				block = _profile_root->child;
			} while( !atomic_cas32( (int32_t*)&_profile_root->child, 0, block ) );

			do
			{
				profile_block* current = GET_BLOCK( block );
				profile_block* leaf;
				uint32_t next = current->sibling;

				current->sibling = 0;
				leaf = _process_profile_block( current );
				_free_profile_block( block, BLOCK_INDEX( leaf ) );

				block = next;
			} while( block );

			profile_end_block();
		}

		if( _profile_enable )
		{
			if( system_info_counter++ > 10 )
			{
				if( _profile_write )
					_profile_write( &system_info, sizeof( profile_block ) );
				system_info_counter = 0;
			}
		}

		profile_end_block();
		
		thread_sleep( 100 );
	}

	return 0;
}


void profile_initialize( char* identifier, void* buffer, uint64_t size )
{
	profile_block* block = buffer;
	uint64_t num_blocks = size / sizeof( profile_block );
	uint32_t i;
	
	_profile_root = block++;
	for( i = 1; i < ( num_blocks - 1 ); ++i, ++block )
	{
		block->child = ( i + 1 );
		block->sibling = 0;
	}
	block->child = 0;
	block->sibling = 0;
	_profile_root->child = 0;
	
	_profile_blocks = _profile_root;
	_profile_free = 1;
	_profile_counter = 128;
	_profile_ground_time = time_current();
	set_thread_profile_block( 0 );
}


void profile_shutdown( void )
{
	_profile_free = 0;
}



void profile_output( profile_write_fn writer )
{
	_profile_write = writer;
}


void profile_enable( int enable )
{
	_profile_enable = enable;
}


void profile_end_frame( uint64_t counter )
{
	profile_block* block;
	if( !_profile_enable )
		return;
	
	//Allocate new master block
	block = _allocate_profile_block();
	block->data.id = PROFILE_ID_ENDFRAME;
	block->data.processor = thread_hardware();
	block->data.thread = (uint32_t)thread_id();
	block->data.start  = time_current() - _profile_ground_time;
	block->data.end = counter;

	_profile_put_simple_block( BLOCK_INDEX( block ) );
}


void profile_begin_block( const char* message )
{
	uint32_t parent;
	profile_block* block;
	if( !_profile_enable )
		return;
	
	parent = get_thread_profile_block();
	if( !parent )
	{
		//Allocate new master block
		block = _allocate_profile_block();
		block->data.id = atomic_add32( (int32_t*)&_profile_counter, 1 );
		string_copy( block->data.name, message, MAX_MESSAGE_LENGTH );
		block->data.processor = thread_hardware();
		block->data.thread = (uint32_t)thread_id();
		block->data.start  = time_current() - _profile_ground_time;
		set_thread_profile_block( BLOCK_INDEX( block ) );
	}
	else
	{
		//Allocate new child block
		profile_block* subblock = _allocate_profile_block();
		uint32_t subindex = BLOCK_INDEX( subblock );
		block = GET_BLOCK( parent );
		subblock->data.id = atomic_add32( (int32_t*)&_profile_counter, 1 );
		subblock->data.parentid = block->data.id;
		string_copy( subblock->data.name, message, MAX_MESSAGE_LENGTH );
		subblock->data.processor = thread_hardware();
		subblock->data.thread = (uint32_t)thread_id();
		subblock->data.start  = time_current() - _profile_ground_time;
		subblock->previous = parent;
		subblock->sibling = block->child;
		if( block->child )
			GET_BLOCK( block->child )->previous = subindex;
		block->child = subindex;
		set_thread_profile_block( subindex );
	}
}


void profile_update_block( void )
{
	char* message;
	unsigned int processor;
	uint32_t block_index = get_thread_profile_block();
	profile_block* block;
	if( !_profile_enable && !block_index )
		return;
	
	block = GET_BLOCK( block_index );
	message = block->data.name;
	processor = thread_hardware();
	if( block->data.processor == processor )
		return;
	
	//Thread migrated to another core, split into new block
	profile_end_block();
	profile_begin_block( message );
}


void profile_end_block( void )
{
	uint32_t block_index = get_thread_profile_block();
	profile_block* block;
	if( !_profile_enable && !block_index )
		return;
	
	block = GET_BLOCK( block_index );
	block->data.end = time_current() - _profile_ground_time;
	
	if( block->previous )
	{
		unsigned int processor;
		profile_block* previous = GET_BLOCK( block->previous );
		while( previous->child != block_index )
		{
			block_index = block->previous; //Walk sibling list backwards
			block = GET_BLOCK( block_index );
			previous = GET_BLOCK( block->previous );
		}
		block_index = block->previous; //Previous now points to parent
		block = GET_BLOCK( block_index );
		set_thread_profile_block( block_index );

		processor = thread_hardware();
		if( block->data.processor != processor )
		{
			const char* message = block->data.name;
			//Thread migrated, split into new block
			profile_end_block();
			profile_begin_block( message );
		}
	}
	else
	{
		_profile_put_root_block( block_index );
		set_thread_profile_block( 0 );
	}
}


void profile_log( const char* message )
{
	if( !_profile_enable )
		return;

	_profile_put_message_block( PROFILE_ID_LOGMESSAGE, message );
}


void profile_trylock( const char* name )
{
	if( !_profile_enable )
		return;

	_profile_put_message_block( PROFILE_ID_TRYLOCK, name );
}


void profile_lock( const char* name )
{
	if( !_profile_enable )
		return;

	_profile_put_message_block( PROFILE_ID_LOCK, name );
}


void profile_unlock( const char* name )
{
	if( !_profile_enable )
		return;

	_profile_put_message_block( PROFILE_ID_UNLOCK, name );
}


#endif
