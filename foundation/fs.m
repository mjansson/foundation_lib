/* fs.m  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#if FOUNDATION_PLATFORM_MACOSX

#include <foundation/apple.h>

#include <dispatch/queue.h>
#include <sys/stat.h>

void* _fs_event_stream_create( const char* path );
void  _fs_event_stream_destroy( void* stream );
void  _fs_event_stream_flush( void* stream );


//This implementation is not optimal in any way, but will do for now


typedef struct _file_node file_node_t;

struct _file_node
{
	char*          name;
	file_node_t**  subdirs;
	char**         files;
	uint64_t*      last_modified;
};


static void _fs_node_deallocate( file_node_t* node )
{
	string_deallocate( node->name );
	string_array_deallocate( node->files );
	for( int isub = 0, subsize = array_size( node->subdirs ); isub < subsize; ++isub )
		_fs_node_deallocate( node->subdirs[isub] );
}


static void _fs_node_populate( file_node_t* node, const char* fullpath )
{
	for( int isub = 0, subsize = array_size( node->subdirs ); isub < subsize; ++isub )
		_fs_node_deallocate( node->subdirs[isub] );
	array_deallocate( node->subdirs );
	string_array_deallocate( node->files );

	char** subdirs = fs_subdirs( fullpath );
	for( int isub = 0, subsize = array_size( subdirs ); isub < subsize; ++isub )
	{
		file_node_t* child = memory_allocate_zero( sizeof( file_node_t ), 0, 0 );
		child->name = subdirs[isub];
		array_push( node->subdirs, child );
	}
	array_deallocate( subdirs ); //Only array, strings are kept in nodes

	node->files = fs_files( fullpath );
	for( int isub = 0, subsize = array_size( node->files ); isub < subsize; ++isub )
	{
		char* filepath = path_merge( fullpath, node->files[isub] );
		uint64_t last_modified = fs_last_modified( filepath );
		array_push( node->last_modified, last_modified );
		string_deallocate( filepath );
	}
	
	for( int isub = 0, subsize = array_size( node->subdirs ); isub < subsize; ++isub )
	{
		char* subpath = path_merge( fullpath, node->subdirs[isub]->name );
		_fs_node_populate( node->subdirs[isub], subpath );
		string_deallocate( subpath );
	}
}


static file_node_t* _fs_node_find( file_node_t* root, const char* path )
{
	unsigned int pathlen = string_length( path );
	if( !pathlen || string_equal( path, "/" ) )
		return root;
	
	file_node_t* node = root;
	do
	{
		file_node_t* next = 0;
		unsigned int separator = string_find( path, '/', 0 );
		for( int isub = 0, subsize = array_size( node->subdirs ); isub < subsize; ++isub )
		{
			if( string_equal_substr( node->subdirs[isub]->name, path, separator != STRING_NPOS ? separator : string_length( path ) ) )
			{
				next = node->subdirs[isub];
				path = path + separator + 1;
				break;
			}
		}
		node = next;
		if( !*path )
			return node;
	} while ( node );
	
	return 0;
}


static void _fs_node_send_deletions( file_node_t* node, const char* path )
{
	for( int ifile = 0, fsize = array_size( node->files ); ifile < fsize; ++ifile )
	{
		char* filepath = path_merge( path, node->files[ifile] );
		log_infof( HASH_FOUNDATION, "  deleted %s", filepath );
		fs_post_event( FOUNDATIONEVENT_FILE_DELETED, filepath, 0 );
		string_deallocate( filepath );
	}

	for( int isub = 0, subsize = array_size( node->subdirs ); isub < subsize; ++isub )
	{
		char* subpath = path_merge( path, node->subdirs[isub]->name );
		_fs_node_send_deletions( node->subdirs[isub], subpath );
		string_deallocate( subpath );
	}
}


static void _fs_node_send_creations( file_node_t* node, const char* path )
{
	for( int ifile = 0, fsize = array_size( node->files ); ifile < fsize; ++ifile )
	{
		char* filepath = path_merge( path, node->files[ifile] );
		log_infof( HASH_FOUNDATION, "  created %s", filepath );
		fs_post_event( FOUNDATIONEVENT_FILE_CREATED, filepath, 0 );
		string_deallocate( filepath );
	}
	
	for( int isub = 0, subsize = array_size( node->subdirs ); isub < subsize; ++isub )
	{
		char* subpath = path_merge( path, node->subdirs[isub]->name );
		_fs_node_send_creations( node->subdirs[isub], subpath );
		string_deallocate( subpath );
	}
}


static void _fs_event_stream_callback( ConstFSEventStreamRef stream_ref, void* user_data, size_t num_events, const char *const event_paths[], const FSEventStreamEventFlags event_flags[], const FSEventStreamEventId event_ids[] )
{
	file_node_t* root_node = user_data;
	unsigned int root_path_len = string_length( root_node->name );
	
	@autoreleasepool
	{
		for( size_t i = 0; i < num_events; ++i )
		{
			const char* path = event_paths[i];
            
			FSEventStreamEventFlags flags = event_flags[i];
			FSEventStreamEventId identifier = event_ids[i];
			
			/* Store path and recurse flag in paths-to-process,
			   then keep state and rescan for changes in fs monitor thread*/
			if( ( flags & kFSEventStreamEventFlagMustScanSubDirs ) != 0 )
			{
				//TODO: Implement
				log_warnf( 0, WARNING_UNSUPPORTED, "Got kFSEventStreamEventFlagMustScanSubDirs: %s (0x%x 0x%x)", path, (unsigned int)flags, (unsigned int)identifier );
			}
			else
			{
				//log_debugf( HASH_FOUNDATION, "Got event for: %s (0x%x 0x%x)", path, (unsigned int)flags, (unsigned int)identifier );

				unsigned int root_ofs = string_find_string( path, root_node->name, 0 );
				if( root_ofs == STRING_NPOS )
					continue;
				
				const char* subpath = path + root_ofs + root_path_len + 1;
				
				file_node_t* node = _fs_node_find( root_node, subpath );
				if( !node )
				{
					log_warnf( 0, WARNING_SUSPICIOUS, "No node found when processing FS event for %s", path );
				}
				else
				{
					char** files = fs_files( path );

					//Check if file have been added, removed or modified
					for( int isub = 0, subsize = array_size( node->files ); isub < subsize; )
					{
						int ifile;
						char* filepath = path_merge( path + root_ofs, node->files[isub] );
						
						if( ( ifile = string_array_find( (const char* const*)files, node->files[isub], array_size( files ) ) ) == -1 )
						{
							fs_post_event( FOUNDATIONEVENT_FILE_DELETED, filepath, 0 );
							string_deallocate( node->files[isub] );
							array_erase_memcpy( node->files, isub );
							array_erase_memcpy( node->last_modified, isub );
							--subsize;
						}
						else
						{
							if( fs_last_modified( filepath ) > node->last_modified[ifile] )
								fs_post_event( FOUNDATIONEVENT_FILE_MODIFIED, filepath, 0 );
							++isub;
						}
						string_deallocate( filepath );
					}
					for( int isub = 0, subsize = array_size( files ); isub < subsize; ++isub )
					{
						if( string_array_find( (const char* const*)node->files, files[isub], array_size( node->files ) ) == -1 )
						{
							char* filepath = path_merge( path + root_ofs, files[isub] );
							fs_post_event( FOUNDATIONEVENT_FILE_CREATED, filepath, 0 );

							array_push( node->last_modified, fs_last_modified( filepath ) );
							array_push( node->files, files[isub] );
							files[isub] = 0;
							string_deallocate( filepath );
						}
					}
					
					string_array_deallocate( files );

					//Check for subdir additions/removals
					char** subdirs = fs_subdirs( path );
					for( int iexist = 0, existsize = array_size( node->subdirs ); iexist < existsize; )
					{
						bool found = false;
						for( int isub = 0, subsize = array_size( subdirs ); isub < subsize; ++isub )
						{
							if( string_equal( node->subdirs[iexist]->name, subdirs[isub] ) )
							{
								found = true;
								break;
							}
						}
						
						if( !found )
						{
							//Recurse and send out file deletion events
							char* fullpath = path_merge( path + root_ofs, node->subdirs[iexist]->name );
							log_infof( HASH_FOUNDATION, "Send file deletion events for old subdir: %s", fullpath );
							_fs_node_send_deletions( node, fullpath );
							string_deallocate( fullpath );
							
							_fs_node_deallocate( node->subdirs[iexist] );
							array_erase_memcpy( node->subdirs, iexist );
							--existsize;
						}
						else
						{
							++iexist;
						}
					}

					for( int isub = 0, subsize = array_size( subdirs ); isub < subsize; ++isub )
					{
						bool found = false;
						for( int iexist = 0, existsize = array_size( node->subdirs ); iexist < existsize; ++existsize )
						{
							if( string_equal( node->subdirs[iexist]->name, subdirs[isub] ) )
							{
								found = true;
								break;
							}
						}
						
						if( !found )
						{
							file_node_t* child = memory_allocate_zero( sizeof( file_node_t ), 0, MEMORY_PERSISTENT );
							
							child->name = subdirs[isub];
							subdirs[isub] = 0;

							array_push( node->subdirs, child );
							
							char* fullpath = path_merge( path, subdirs[isub] );
							_fs_node_populate( child, fullpath );
							string_deallocate( fullpath );

							fullpath = path_merge( path + root_ofs, child->name );
							log_infof( HASH_FOUNDATION, "Send file creation events for new subdir: %s", fullpath );
							_fs_node_send_creations( child, fullpath );
							string_deallocate( fullpath );
						}
					}
					
					string_array_deallocate( subdirs );
				}
			}
        }
	}
}


void _fs_event_stream_flush( void* stream )
{
	FSEventStreamFlushAsync( stream );
}


static const void* _fs_event_stream_retain( const void* info )
{
	return info;
}


static void _fs_event_stream_release( const void* info )
{
	if( info )
		_fs_node_deallocate( (file_node_t*)info );
}


void* _fs_event_stream_create( const char* path )
{
	@autoreleasepool
	{
		file_node_t* node = memory_allocate_zero( sizeof( file_node_t ), 0, MEMORY_PERSISTENT );
		node->name = string_clone( path );
		
		_fs_node_populate( node, path );
		
		NSString* nspath = [[NSString alloc] initWithUTF8String:path];
		NSArray* patharr = [NSArray arrayWithObject:nspath];
		FSEventStreamContext context = { 0, node, _fs_event_stream_retain, _fs_event_stream_release, 0 };
		NSTimeInterval latency = 1.0;
		
		//TODO: Implement allocator based on foundation memory allocation subsystem
		void* stream = FSEventStreamCreate( 0, (FSEventStreamCallback)&_fs_event_stream_callback, &context, (__bridge CFArrayRef)patharr, kFSEventStreamEventIdSinceNow, (CFAbsoluteTime)latency, kFSEventStreamCreateFlagNone );
		if( stream )
		{
			FSEventStreamSetDispatchQueue( stream, dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_DEFAULT, 0 ) );
			if( NO == FSEventStreamStart( stream ) )
			{
				log_errorf( 0, ERROR_SYSTEM_CALL_FAIL, "Unable to start FS event stream" );
			}
		}
		else
		{
			log_errorf( 0, ERROR_SYSTEM_CALL_FAIL, "Unable to create FS event stream" );
		}

		log_debugf( 0, "Started FS event stream for: %s", path );
		
		return stream;
	}	
}


void _fs_event_stream_destroy( void* stream )
{
	if( !stream )
		return;
	
	@autoreleasepool
	{	
		FSEventStreamStop( stream );
		FSEventStreamInvalidate( stream );
		FSEventStreamRelease( stream );
	}
}

#endif

