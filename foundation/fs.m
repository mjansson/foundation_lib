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

#include <foundation/platform.h>
#include <foundation/atomic.h>
#include <foundation/log.h>
#include <foundation/fs.h>

#include <foundation/apple.h>

#include <dispatch/queue.h>
#include <sys/stat.h>

void* _fs_event_stream_create( const char* path );
void  _fs_event_stream_destroy( void* stream );
void  _fs_event_stream_flush( void* stream );


void _fs_event_stream_callback( ConstFSEventStreamRef stream_ref, void* user_data, size_t num_events, const char *const event_paths[], const FSEventStreamEventFlags event_flags[], const FSEventStreamEventId event_ids[] )
{
	@autoreleasepool
	{
		for( size_t i = 0; i < num_events; ++i )
		{
			const char* path = event_paths[i];
            
			FSEventStreamEventFlags flags = event_flags[i];
			FSEventStreamEventId identifier = event_ids[i];
			
			/* Store path and recurse flag in paths-to-process,
			   then keep state and rescan for changes in fs monitor thread
			if( ( flags & kFSEventStreamEventFlagMustScanSubDirs ) != 0 )
			{
				//TODO: Implement
				log_warnf( WARNING_UNSUPPORTED, "Got kFSEventStreamEventFlagMustScanSubDirs: %s (%u %u)", path, (unsigned int)flags, (unsigned int)identifier );
			}
			else
			{
				struct stat st; memset( &st, 0, sizeof( st ) );
				if( stat( path, &st ) < 0 )
				{
					fs_post_event( FOUNDATIONEVENT_FILE_DELETED, path, 0 );
					continue;
				}
				
				fs_post_event( FOUNDATIONEVENT_FILE_MODIFIED, path, 0 );
				
				//return (uint64_t)st.st_mtime * 1000ULL;
				
				//fs_post_event( FOUNDATIONEVENT_FILE_CREATED, curpath, 0 );
				//fs_post_event( FOUNDATIONEVENT_FILE_MODIFIED, curpath, 0 );
			}*/
        }
	}
}


void _fs_event_stream_flush( void* stream )
{
	FSEventStreamFlushAsync( stream );
}


void* _fs_event_stream_create( const char* path )
{
	@autoreleasepool
	{
		NSString* nspath = [[NSString alloc] initWithUTF8String:path];
		NSArray* patharr = [NSArray arrayWithObject:nspath];
		FSEventStreamContext context = { 0, 0, 0, 0, 0 };
		NSTimeInterval latency = 1.0;
		
		//TODO: Implement allocator based on foundation memory allocation subsystem
		void* stream = FSEventStreamCreate( 0, (FSEventStreamCallback)&_fs_event_stream_callback, &context, (__bridge CFArrayRef)patharr, kFSEventStreamEventIdSinceNow, (CFAbsoluteTime)latency, kFSEventStreamCreateFlagNone );
		if( stream )
		{
			FSEventStreamSetDispatchQueue( stream, dispatch_get_global_queue( DISPATCH_QUEUE_PRIORITY_DEFAULT, 0 ) );
			if( NO == FSEventStreamStart( stream ) )
			{
				log_errorf( ERROR_SYSTEM_CALL_FAIL, "Unable to start FS event stream" );
			}
		}
		else
		{
			log_errorf( ERROR_SYSTEM_CALL_FAIL, "Unable to create FS event stream" );
		}
		
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
