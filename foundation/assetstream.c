/* config.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


#if FOUNDATION_PLATFORM_ANDROID

#include "app/android/android_native_app_glue.h"
#include <android/asset_manager.h>


typedef struct ALIGN(8) _foundation_stream_asset
{
	FOUNDATION_DECLARE_STREAM;
	AAsset*                  asset;
	int64_t                  position;
} stream_asset_t;

static stream_vtable_t __asset_vtable;


uint64_t asset_stream_read( stream_t* stream, void* dest, uint64_t num )
{
	stream_asset_t* asset = (stream_asset_t*)stream;

	int curread = AAsset_read( asset->asset, dest, num );
	if( curread > 0 )
		asset->position += curread;

	return curread;
}


uint64_t asset_stream_write( stream_t* stream, const void* source, uint64_t num )
{
	NEO_ASSERT_FAIL( "Asset writing not allowed" );
	return 0;
}


bool asset_stream_isopen( const stream_t* stream )
{
	const stream_asset_t* asset = (const stream_asset_t*)stream;
	return asset && asset->asset ? true : false;
}


bool asset_stream_eos( stream_t* stream )
{
	stream_asset_t* asset = (stream_asset_t*)stream;
	return !asset || !asset->asset || ( asset->position >= (int64_t)AAsset_getLength( asset->asset ) );
}


void asset_stream_flush( stream_t* stream )
{
}


void asset_stream_truncate( stream_t* stream, uint64_t size )
{
	NEO_ASSERT_FAIL( "Asset truncation not allowed" );
}


uint64_t asset_stream_size( stream_t* stream )
{
	stream_asset_t* asset = (stream_asset_t*)stream;
	return ( asset && asset->asset ? (int64_t)AAsset_getLength( asset->asset ) : 0 );
}


void asset_stream_seek( stream_t* stream, int64_t offset, seek_mode_t direction )
{
	stream_asset_t* asset = (stream_asset_t*)stream;
	off_t newpos = AAsset_seek( asset->asset, offset, direction );
	if( newpos >= 0 )
		asset->position = newpos;
}


int64_t asset_stream_tell( stream_t* stream )
{
	stream_asset_t* asset = (stream_asset_t*)stream;
	return asset->position;
}


uint64_t asset_stream_lastmod( stream_t* stream )
{
	return time_current();
}


uint64_t asset_stream_available_read( stream_t* stream )
{
	stream_asset_t* asset = (stream_asset_t*)stream;
	return AAsset_getLength( asset->asset ) - asset->position;
}


void asset_stream_deallocate( stream_t* stream )
{
	stream_asset_t* asset = (stream_asset_t*)stream;
	if( asset && asset->asset )
		AAsset_close( asset->asset );
}


stream_t* asset_stream_clone( stream_t* stream )
{
	return asset_stream_open( stream->name, stream->mode );
}


stream_t* asset_stream_open( const char* path, unsigned int mode )
{
	if( !path || !path[0] )
		return 0;

	//Setup global vtable
	if( __asset_vtable.read != asset_stream_read )
	{
		__asset_vtable.read = asset_stream_read;
		__asset_vtable.write = asset_stream_write;
		__asset_vtable.isopen = asset_stream_isopen;
		__asset_vtable.eos = asset_stream_eos;
		__asset_vtable.flush = asset_stream_flush;
		__asset_vtable.truncate = asset_stream_truncate;
		__asset_vtable.size = asset_stream_size;
		__asset_vtable.seek = asset_stream_seek;
		__asset_vtable.tell = asset_stream_tell;
		__asset_vtable.lastmod = asset_stream_lastmod;
		__asset_vtable.available_read = asset_stream_available_read;
		__asset_vtable.deallocate = asset_stream_deallocate;
		__asset_vtable.clone = asset_stream_clone;
	}

	if( string_equal_substr( path, "asset://", 8 ) )
		path += 8;
	if( *path == '/' )
		++path;

	AAsset* assetobj = AAssetManager_open( _global_app->activity->assetManager, path, AASSET_MODE_RANDOM );
	if( !assetobj )
	{
		//warn_logf( WARNING_SYSTEM_CALL_FAIL, "Unable to open asset: asset://%s", path );
		return 0;
	}

	stream_asset_t* asset = allocate_zero( file_allocator(), sizeof( stream_asset_t ), 0 );
	stream_t* stream = (stream_t*)asset;

	_initialize_stream( stream );

	stream->type = STREAMTYPE_ASSET;
	stream->sequential = 0;
	stream->name = string_format( "asset://%s", path );
	stream->mode = ( mode & STREAM_BINARY ) | STREAM_IN;
	stream->vtable = &__asset_vtable;

	asset->asset = assetobj;
	asset->position = 0;

	return stream;
}


#endif
