/* objectmap.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#pragma once

/*! \file objectmap.h
    Mapping of object handles to object pointers, thread safe and lock free */

#include <foundation/platform.h>
#include <foundation/types.h>


/*! Allocate storage for new map
    \param size                     Number of slots
	\return                         New object map */
FOUNDATION_API objectmap_t*         objectmap_allocate( unsigned int size );

/*! Free memory. Does not free the stored objects, only map storage.
    \param map                      Object map */
FOUNDATION_API void                 objectmap_deallocate( objectmap_t* map );

/*! Get size of map
    \param map                      Object map
	\return                         Size of map (number of object handles) */
FOUNDATION_API unsigned int         objectmap_size( const objectmap_t* map );

/*! Reserve a slot in the map
    \param map                      Object map
    \return                         New object handle, 0 if none available */
FOUNDATION_API object_t             objectmap_reserve( objectmap_t* map );

/*! Free a slot in the map
    \param map                      Object map
    \param id                       Object handle to free */
FOUNDATION_API void                 objectmap_free( objectmap_t* map, object_t id );

/*! Set object pointer for given slot
    \param map                      Object map
    \param id                       Object handle
	\param object                   Object pointer */
FOUNDATION_API void                 objectmap_set( objectmap_t* map, object_t id, void* object );

/*! Raw lookup of object pointer for map index
    \param map                      Object map
	\param index                    Map index
    \return                         Object pointer */
FOUNDATION_API void*                objectmap_raw_lookup( const objectmap_t* map, unsigned int index );

/*! Map object handle to object pointer. This function is unsafe in the sense that it
    might return an object pointer which points to an invalid (deallocated) object if
    the object reference count was decreased in another thread while this function is
    executing. The object lifetime is also not guaranteed in any code using the returned
    object. For a safe function use objectmap_lookup_ref
    \param map                      Object map
    \param id                       Object handle
	\return                         Object pointer, 0 if invalid/outdated handle */
static FORCEINLINE PURECALL void*   objectmap_lookup( const objectmap_t* map, object_t id );

/*! Map object handle to object pointer and increase ref count. This function is safe in
    the sense that it will only return an object pointer if the ref count was successfully
	increased and object is still valid. Once the caller has finished using the object
	returned, the handle should be released and reference count decreased by a call to the
    appropriate destroy function (for eaxmple, a thread this would be thread_destroy).
    \param map                      Object map
    \param id                       Object handle
	\return                         Object pointer, 0 if invalid/outdated handle */
FOUNDATION_API void*                objectmap_lookup_ref( const objectmap_t* map, object_t id );

/*! Map object handle to object pointer and decrease ref count. If the object reference count
    reaches zero the object is deallocated by a call to the deallocation function. This function
    is safe in the sense that it will work correctly across threads also using the object_lookup_ref
    and object_lookup_unref functions.
    \param map                      Object map
    \param id                       Object handle
    \param deallocate               Deallocation function
	\return                         true if object is still valid, false if it was deallocated */
FOUNDATION_API bool                 objectmap_lookup_unref( const objectmap_t* map, object_t id, object_deallocate_fn deallocate );


static FORCEINLINE void* objectmap_lookup( const objectmap_t* map, object_t id )
{
	void* object = map->map[ id & map->mask_index ];
	return ( object && !( (uintptr_t)object & 1 ) &&
			( ( *( (uint64_t*)object + 1 ) & map->mask_id ) == ( id & map->mask_id ) ) ? //ID in object is offset by 8 bytes
			object : 0 );
}
