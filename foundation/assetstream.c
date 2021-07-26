/* assetstream.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
 *
 * This library provides a cross-platform foundation library in C11 providing basic support data
 * types and functions to write applications and games in a platform-independent fashion. The latest
 * source code is always available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any
 * restrictions.
 *
 */

#include <foundation/foundation.h>

#if FOUNDATION_PLATFORM_ANDROID

#include <foundation/internal.h>
#include <foundation/android.h>

#include <android/asset_manager.h>

struct stream_asset_t {
	FOUNDATION_DECLARE_STREAM;
	AAsset* asset;
	size_t position;
};

typedef FOUNDATION_ALIGN(8) struct stream_asset_t stream_asset_t;

static stream_vtable_t _asset_stream_vtable;

static size_t
asset_stream_read(stream_t* stream, void* dest, size_t num) {
	stream_asset_t* asset = (stream_asset_t*)stream;

	ssize_t curread = AAsset_read(asset->asset, dest, num);
	if (curread > 0) {
		size_t absread = (size_t)curread;
		asset->position += absread;
		return absread;
	}

	return 0;
}

static size_t
asset_stream_write(stream_t* stream, const void* source, size_t num) {
	FOUNDATION_ASSERT_FAIL("Asset writing not allowed");
	FOUNDATION_UNUSED(stream);
	FOUNDATION_UNUSED(source);
	FOUNDATION_UNUSED(num);
	return 0;
}

static bool
asset_stream_eos(stream_t* stream) {
	stream_asset_t* asset = (stream_asset_t*)stream;
	return !asset || !asset->asset || ((off_t)asset->position >= AAsset_getLength(asset->asset));
}

static void
asset_stream_flush(stream_t* stream) {
	FOUNDATION_UNUSED(stream);
}

static void
asset_stream_truncate(stream_t* stream, size_t size) {
	FOUNDATION_ASSERT_FAIL("Asset truncation not allowed");
	FOUNDATION_UNUSED(stream);
	FOUNDATION_UNUSED(size);
}

static size_t
asset_stream_size(stream_t* stream) {
	stream_asset_t* asset = (stream_asset_t*)stream;
	ssize_t length = (asset && asset->asset ? AAsset_getLength(asset->asset) : 0);
	return (length > 0) ? (size_t)length : 0;
}

static void
asset_stream_seek(stream_t* stream, ssize_t offset, stream_seek_mode_t direction) {
	stream_asset_t* asset = (stream_asset_t*)stream;
	ssize_t newpos = AAsset_seek(asset->asset, offset, direction);
	if (newpos >= 0)
		asset->position = (size_t)newpos;
}

static size_t
asset_stream_tell(stream_t* stream) {
	stream_asset_t* asset = (stream_asset_t*)stream;
	return asset->position;
}

static tick_t
asset_stream_lastmod(const stream_t* stream) {
	FOUNDATION_UNUSED(stream);
	return time_current();
}

static size_t
asset_stream_available_read(stream_t* stream) {
	stream_asset_t* asset = (stream_asset_t*)stream;
	ssize_t length = (asset && asset->asset ? AAsset_getLength(asset->asset) : 0);
	return (length > 0) ? (size_t)length - asset->position : 0;
}

static void
asset_stream_finalize(stream_t* stream) {
	stream_asset_t* asset = (stream_asset_t*)stream;

	if (!asset || (stream->type != STREAMTYPE_ASSET))
		return;

	if (asset->asset)
		AAsset_close(asset->asset);

	asset->asset = 0;
}

static stream_t*
asset_stream_clone(stream_t* stream) {
	return stream ? asset_stream_open(STRING_ARGS(stream->path), stream->mode) : 0;
}

stream_t*
asset_stream_open(const char* path, size_t length, unsigned int mode) {
	string_t finalpath;
	char* pathptr;
	char buffer[BUILD_MAX_PATHLEN];

	if (!path || !length)
		return 0;

	if ((length >= 6) && string_equal(path, 6, STRING_CONST("asset:"))) {
		finalpath = string_copy(buffer, sizeof(buffer), path, length);
		pathptr = finalpath.str + 6;
		if (*pathptr == '/')
			++path;
	} else {
		finalpath = string_concat(buffer, sizeof(buffer), "asset://", path[0] == '/' ? 7 : 8, path, length);
		pathptr = finalpath.str + 8;
	}
	if (*pathptr == '/')
		++path;

	AAsset* assetobj = AAssetManager_open(android_app()->activity->assetManager, pathptr, AASSET_MODE_RANDOM);
	if (!assetobj)
		return 0;

	stream_asset_t* asset =
	    memory_allocate(HASH_STREAM, sizeof(stream_asset_t), 8, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	stream_t* stream = (stream_t*)asset;

	stream_initialize(stream, BUILD_DEFAULT_STREAM_BYTEORDER);

	stream->type = STREAMTYPE_ASSET;
	stream->sequential = 0;
	stream->reliable = 1;
	stream->inorder = 1;
	stream->swap = 0;
	stream->persistent = 1;
	stream->path = string_clone(STRING_ARGS(finalpath));
	stream->mode = (mode & STREAM_BINARY) | STREAM_IN;
	stream->vtable = &_asset_stream_vtable;

	asset->asset = assetobj;
	asset->position = 0;

	return stream;
}

void
_asset_stream_initialize(void) {
	_asset_stream_vtable.read = asset_stream_read;
	_asset_stream_vtable.write = asset_stream_write;
	_asset_stream_vtable.eos = asset_stream_eos;
	_asset_stream_vtable.flush = asset_stream_flush;
	_asset_stream_vtable.truncate = asset_stream_truncate;
	_asset_stream_vtable.size = asset_stream_size;
	_asset_stream_vtable.seek = asset_stream_seek;
	_asset_stream_vtable.tell = asset_stream_tell;
	_asset_stream_vtable.lastmod = asset_stream_lastmod;
	_asset_stream_vtable.available_read = asset_stream_available_read;
	_asset_stream_vtable.finalize = asset_stream_finalize;
	_asset_stream_vtable.clone = asset_stream_clone;
}

#endif
