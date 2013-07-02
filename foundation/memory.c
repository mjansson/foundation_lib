/* memory.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#if FOUNDATION_PLATFORM_WINDOWS
#  include <foundation/windows.h>
#endif
#if FOUNDATION_PLATFORM_POSIX
#  include <foundation/posix.h>
#  include <malloc.h>
#endif


static memory_system_t _memsys = {0};


typedef ALIGN(8) struct
{
	void*               storage;
	void*               end;
	void*               head;
	void*               tail;
	uint64_t            size;
	uint64_t            maxchunk;
} atomic_linear_memory_t;

static atomic_linear_memory_t _memory_temporary = {0};


#if BUILD_ENABLE_MEMORY_TRACKER
static memory_tracker_t _memory_tracker = {0};
static void _memory_track( void* addr, uint64_t size );
static void _memory_untrack( void* addr );
static void _memory_report( void );
#else
#define _memory_track( addr, size ) do { (void)sizeof( (addr) ); (void)sizeof( (size) ); } while(0)
#define _memory_untrack( addr ) do { (void)sizeof( (addr) ); } while(0)
#define _memory_report()
#endif


static void _atomic_allocate_initialize( uint64_t storagesize )
{
	if( storagesize < 1024 )
		storagesize = BUILD_SIZE_TEMPORARY_MEMORY;
	_memory_temporary.storage   = memory_allocate( storagesize, FOUNDATION_PLATFORM_POINTER_SIZE, MEMORY_PERSISTENT );
	_memory_temporary.end       = pointer_offset( _memory_temporary.storage, storagesize );
	_memory_temporary.head      = _memory_temporary.storage;
	_memory_temporary.size      = storagesize;
	_memory_temporary.maxchunk  = ( storagesize / 8 );
}


static void _atomic_allocate_shutdown( void )
{
	if( _memory_temporary.storage )
		memory_deallocate( _memory_temporary.storage );
	memset( &_memory_temporary, 0, sizeof( _memory_temporary ) );
}


static void* _atomic_allocate_linear( uint64_t chunksize )
{
	void* old_head;
	void* new_head;
	void* return_pointer = 0;

	do
	{
		old_head = _memory_temporary.head;
		new_head = pointer_offset( old_head, chunksize );

		return_pointer = old_head;

		if( new_head > _memory_temporary.end )
		{
			new_head = pointer_offset( _memory_temporary.storage, chunksize );
			return_pointer = _memory_temporary.storage;
		}
	} while( !atomic_cas_ptr( &_memory_temporary.head, new_head, old_head ) );
	
	return return_pointer;
}


static CONSTCALL FORCEINLINE unsigned int _memory_get_align( unsigned int align )
{
	if( align < FOUNDATION_PLATFORM_POINTER_SIZE )
	{
#if FOUNDATION_PLATFORM_ANDROID
		return sizeof( uint64_t );
#else
		return align ? FOUNDATION_PLATFORM_POINTER_SIZE : 0;
#endif
	}
	align = math_align_poweroftwo( align );
	return ( align < ( FOUNDATION_PLATFORM_POINTER_SIZE * 4 ) ) ? align : ( FOUNDATION_PLATFORM_POINTER_SIZE * 4 );
}


static CONSTCALL void* _memory_align_pointer( void* p, unsigned int align )
{
	uintptr_t address;
	if( !p || !align )
		return p;

	address = (uintptr_t)p;
	if( address % align )
	{
		address = address + ( align - ( address % align ) );
		p = (void*)address;
	}

	return p;
}	


int _memory_initialize( const memory_system_t memory )
{
	_memsys = memory;
	return _memsys.initialize();
}


void _memory_preallocate( void )
{
	hash_t tracker;
	
	if( !_memory_temporary.storage )
		_atomic_allocate_initialize( config_int( HASH_FOUNDATION, HASH_TEMPORARY_MEMORY ) );

	tracker = config_string_hash( HASH_FOUNDATION, HASH_MEMORY_TRACKER );
	if( tracker == HASH_LOCAL )
		memory_set_tracker( memory_tracker_local() );
}


void _memory_shutdown( void )
{
	memory_tracker_t no_tracker = {0};
	memory_set_tracker( no_tracker );
	
	_atomic_allocate_shutdown();
	_memsys.shutdown();
}


void* memory_allocate( uint64_t size, unsigned int align, memory_hint_t hint )
{
	void* p;
	if( ( hint == MEMORY_TEMPORARY ) && _memory_temporary.storage && ( size + align < _memory_temporary.maxchunk ) )
	{
		align = _memory_get_align( align );
		p = _memory_align_pointer( _atomic_allocate_linear( size + align ), align );
	}
	else
	{		
		p = _memsys.allocate( memory_context(), size, align, hint );
	}
	_memory_track( p, size );
	return p;
}


void* memory_allocate_zero( uint64_t size, unsigned int align, memory_hint_t hint )
{
	void* p;
	if( ( hint == MEMORY_TEMPORARY ) && _memory_temporary.storage && ( size + align < _memory_temporary.maxchunk ) )
	{
		align = _memory_get_align( align );
		p = _memory_align_pointer( _atomic_allocate_linear( size + align ), align );
		memset( p, 0, (size_t)size );
	}
	else
	{
		p = _memsys.allocate_zero( memory_context(), size, align, hint );
	}
	_memory_track( p, size );
	return p;
}


void* memory_allocate_context( uint16_t context, uint64_t size, unsigned int align, memory_hint_t hint )
{
	void* p = _memsys.allocate( context, size, align, hint );
	_memory_track( p, size );
	return p;
}


void* memory_allocate_zero_context( uint16_t context, uint64_t size, unsigned int align, memory_hint_t hint )
{
	void* p = _memsys.allocate_zero( context, size, align, hint );
	_memory_track( p, size );
	return p;
}


void* memory_reallocate( void* p, uint64_t size, unsigned int align )
{
	FOUNDATION_ASSERT_MSG( ( p < _memory_temporary.storage ) || ( p >= _memory_temporary.end ), "Trying to reallocate temporary memory" );
	_memory_untrack( p );
	p = _memsys.reallocate( p, size, align );
	_memory_track( p, size );
	return p;
}


void memory_deallocate( void* p )
{
	if( ( p < _memory_temporary.storage ) || ( p >=_memory_temporary.end ) )
		_memsys.deallocate( p );
	_memory_untrack( p );
}


#if BUILD_ENABLE_MEMORY_CONTEXT


FOUNDATION_DECLARE_THREAD_LOCAL( memory_context_t*, memory_context, 0 )

void memory_context_push( uint16_t context_id )
{
	memory_context_t* context = get_thread_memory_context();
	if( !context )
	{
		context = memory_allocate_zero( sizeof( memory_context_t ), 0, MEMORY_PERSISTENT );
		set_thread_memory_context( context );
	}
	FOUNDATION_ASSERT_MSG( context->depth < BUILD_SIZE_MEMORY_CONTEXT_DEPTH, "Memory context stack overflow" );
	context->context[ context->depth ] = context_id;
	if( context->depth < BUILD_SIZE_MEMORY_CONTEXT_DEPTH-1 )
		++context->depth;
}


void memory_context_pop( void )
{
	memory_context_t* context = get_thread_memory_context();
	if( context )
	{
		FOUNDATION_ASSERT_MSG( context->depth, "Memory context stack underflow" );
		--context->depth;
	}
}


uint16_t memory_context( void )
{
	memory_context_t* context = get_thread_memory_context();
	return ( context && ( context->depth > 0 ) ) ? context->context[ context->depth - 1 ] : MEMORYCONTEXT_GLOBAL;
}


void memory_context_thread_deallocate( void )
{
	memory_context_t* context = get_thread_memory_context();
	if( context )
		memory_deallocate( context );
	set_thread_memory_context( 0 );
}


#endif


static void* _memory_allocate_malloc( uint16_t context, uint64_t size, unsigned int align, memory_hint_t hint )
{
	align = _memory_get_align( align );
#if FOUNDATION_PLATFORM_WINDOWS
	return _aligned_malloc( (size_t)size, align );
#elif FOUNDATION_PLATFORM_POSIX && !FOUNDATION_PLATFORM_ANDROID
	void* memory = 0;
	if( !align )
		return malloc( (size_t)size );
	/*int result = posix_memalign( &memory, align, (size_t)size );	
	if( result || !memory )
		log_panicf( ERROR_OUT_OF_MEMORY, "Unable to allocate memory: %s", system_error_message( 0 ) );
	return ( result == 0 ) ? memory : 0;*/
	memory = aligned_alloc( align, (size_t)size );
	if( !memory )
		log_panicf( ERROR_OUT_OF_MEMORY, "Unable to allocate memory: %s", system_error_message( 0 ) );
	return memory;
#else
	void* memory = malloc( size + align );
	memory = _memory_align_pointer( memory, align );
	return memory;
#endif
}


static void* _memory_allocate_zero_malloc( uint16_t context, uint64_t size, unsigned int align, memory_hint_t hint )
{
	void* memory = _memory_allocate_malloc( context, size, align, hint );
	if( memory )
		memset( memory, 0, (size_t)size );
	return memory;
}


static void* _memory_reallocate_malloc( void* p, uint64_t size, unsigned int align )
{
	align = _memory_get_align( align );
#if FOUNDATION_PLATFORM_WINDOWS
	return _aligned_realloc( p, (size_t)size, align );
#else
	if( align )
	{
		//No realloc aligned available
		void* memory = aligned_alloc( align, (size_t)size );
		if( !memory )
		{
			log_panicf( ERROR_OUT_OF_MEMORY, "Unable to reallocate memory: %s", system_error_message( 0 ) );
			return 0;
		}
		if( p )
		{
			size_t prev_size = malloc_usable_size( p );
			memcpy( memory, p, ( size < prev_size ) ? size : prev_size );
		}
		return memory;
	}
	return realloc( p, (size_t)size );
#endif
}


static void _memory_deallocate_malloc( void* p )
{
#if FOUNDATION_PLATFORM_WINDOWS
	if( p )
		_aligned_free( p );
#elif FOUNDATION_PLATFORM_POSIX
	free( p );
#else
#  error Not implemented
#endif
}


static int _memory_initialize_malloc( void )
{
	return 0;
}


static void _memory_shutdown_malloc( void )
{
}


memory_system_t memory_system_malloc( void )
{
	memory_system_t memsystem;
	memsystem.allocate = _memory_allocate_malloc;
	memsystem.allocate_zero = _memory_allocate_zero_malloc;
	memsystem.reallocate = _memory_reallocate_malloc;
	memsystem.deallocate = _memory_deallocate_malloc;
	memsystem.initialize = _memory_initialize_malloc;
	memsystem.shutdown = _memory_shutdown_malloc;
	return memsystem;
}


#if BUILD_ENABLE_MEMORY_TRACKER


void memory_set_tracker( memory_tracker_t tracker )
{
	memory_tracker_t no_tracker = {0};
	memory_tracker_t old_tracker = _memory_tracker;

	if( ( old_tracker.track == tracker.track ) && ( old_tracker.untrack == tracker.untrack ) )
		return;
	
	_memory_tracker = no_tracker;
	
	if( old_tracker.shutdown )
		old_tracker.shutdown();
	
	if( tracker.initialize )
		tracker.initialize();

	_memory_tracker = tracker;
}


static void _memory_track( void* addr, uint64_t size )
{
	if( _memory_tracker.track )
		_memory_tracker.track( addr, size );
}


static void _memory_untrack( void* addr )
{
	if( _memory_tracker.untrack )
		_memory_tracker.untrack( addr );
}


static void _memory_report( void )
{
}


typedef struct ALIGN(8) _foundation_memory_tag
{
	void*         address;
	uintptr_t     size;
	void*         trace[14];
} memory_tag_t;


hashtable_t*       _memory_table = 0;
memory_tag_t*      _memory_tags = 0;
ALIGN(8) int32_t   _memory_tag_next = 0;

#define MAX_CONCURRENT_ALLOCATIONS 32 * 1024


static int _memory_tracker_initialize( void )
{
	log_debug( "Initializing local memory tracker" );
	if( !_memory_tags )
		_memory_tags = memory_allocate_zero( sizeof( memory_tag_t ) * MAX_CONCURRENT_ALLOCATIONS, 16, MEMORY_PERSISTENT );
	if( !_memory_table )
		_memory_table = hashtable_allocate( MAX_CONCURRENT_ALLOCATIONS );
	return 0;
}


static void _memory_tracker_shutdown( void )
{
	if( _memory_table )
		hashtable_deallocate( _memory_table );
	if( _memory_tags )
	{
		bool got_leaks = false;

		log_debug( "Checking for memory leaks" );
		for( unsigned int it = 0; it < MAX_CONCURRENT_ALLOCATIONS; ++it )
		{
			memory_tag_t* tag = _memory_tags + it;
			if( tag->address )
			{
				char* trace = stacktrace_resolve( tag->trace, 14, 0 );
				log_warnf( WARNING_MEMORY, "Memory leak: %d bytes @ " STRING_FORMAT_POINTER " : tag %d\n%s", (unsigned int)tag->size, tag->address, it, trace );
				string_deallocate( trace );
				got_leaks = true;
			}
		}
		memory_deallocate( _memory_tags );

		if( !got_leaks )
			log_debug( "No memory leaks detected" );
	}
}


static void _memory_tracker_track( void* addr, uint64_t size )
{
	if( addr ) do
	{
		int32_t tag = atomic_exchange_and_add32( &_memory_tag_next, 1 );
		if( tag >= MAX_CONCURRENT_ALLOCATIONS )
		{
			int32_t newtag = tag % MAX_CONCURRENT_ALLOCATIONS;
			atomic_cas32( &_memory_tag_next, newtag, tag + 1 );
			tag = newtag;
		}
		if( !_memory_tags[ tag ].address && atomic_cas_ptr( &_memory_tags[ tag ].address, addr, 0 ) )
		{
			_memory_tags[ tag ].size = (uintptr_t)size;
			stacktrace_capture( _memory_tags[ tag ].trace, 14, 3 );
			hashtable_set( _memory_table, (uintptr_t)addr, (uintptr_t)( tag + 1 ) );
			return;
		}
	} while( true );
}


static void _memory_tracker_untrack( void* addr )
{
	uintptr_t tag = addr ? hashtable_get( _memory_table, (uintptr_t)addr ) : 0;
	if( tag )
	{
		--tag;
		_memory_tags[ tag ].address = 0;
	}
	//else if( addr )
	//	log_warnf( WARNING_SUSPICIOUS, "Untracked deallocation: " STRING_FORMAT_POINTER, addr );
}


#endif


memory_tracker_t memory_tracker_local( void )
{
	memory_tracker_t tracker = {0};
#if BUILD_ENABLE_MEMORY_TRACKER
	tracker.track = _memory_tracker_track;
	tracker.untrack = _memory_tracker_untrack;
	tracker.initialize = _memory_tracker_initialize;
	tracker.shutdown = _memory_tracker_shutdown;
#endif
	return tracker;
}
