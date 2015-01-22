/* pnacl.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#if FOUNDATION_PLATFORM_PNACL

#include <foundation/string.h>
#include <foundation/log.h>
#include <foundation/hashstrings.h>
#include <foundation/thread.h>
#include <foundation/environment.h>
#include <foundation/main.h>
#include <foundation/pnacl.h>

#include <ppapi/c/pp_instance.h>
#include <ppapi/c/pp_module.h>
#include <ppapi/c/pp_bool.h>
#include <ppapi/c/pp_rect.h>
#include <ppapi/c/pp_resource.h>
#include <ppapi/c/pp_errors.h>

#include <ppapi/c/ppp_instance.h>

extern int real_main( PP_Instance instance );


static PP_Module _pnacl_module = 0;
static PPB_GetInterface _pnacl_browser = 0;


static void* pnacl_instance_main_thread( object_t thread, void* arg )
{
	char* name = 0;
	const application_t* app = environment_application();
	{
		const char* aname = app->short_name;
		name = string_clone( aname ? aname : "unknown" );
		name = string_append( name, "-" );
		name = string_append( name, string_from_version_static( app->version ) );
	}

	if( app->dump_callback )
		crash_guard_set( app->dump_callback, name );

	int ret = crash_guard( main_run, 0, app->dump_callback, name );

	string_deallocate( name );

	return (void*)(uintptr_t)ret;
}


static PP_Bool pnacl_instance_create( PP_Instance instance, uint32_t argc, const char* argn[], const char* argv[] )
{
	log_debugf( HASH_PNACL, "PNaCL instance create: %d %d", instance, argc );
	for( unsigned int iarg = 0; iarg < argc; ++iarg )
		log_debugf( HASH_PNACL, "  %s = %s", argn[iarg], argv[iarg] );

	real_main( instance );
}


int pnacl_instance_main( PP_Instance instance )
{
	object_t instance_thread = thread_create( pnacl_instance_main_thread, "main", THREAD_PRIORITY_NORMAL, BUILD_SIZE_DEFAULT_THREAD_STACK );
	thread_start( instance_thread, (void*)(uintptr_t)instance );
	return 0;
}


static void pnacl_instance_destroy( PP_Instance instance )
{
}


static void pnacl_instance_change_view( PP_Instance instance, const struct PP_Rect* position, const struct PP_Rect* clip )
{
}


static void pnacl_instance_change_focus( PP_Instance instance, PP_Bool has_focus )
{
}


static PP_Bool pnacl_instance_document_load( PP_Instance instance, PP_Resource url_loader )
{
	return PP_TRUE;
}


int pnacl_module_initialize( PP_Module module_id, PPB_GetInterface browser )
{
	_pnacl_module = module_id;
	_pnacl_browser = browser;
	return PP_OK;
}


const void* pnacl_module_interface( const char* interface_name )
{
	if( string_equal( interface_name, PPP_INSTANCE_INTERFACE ) == 0 )
	{
		static PPP_Instance instance_interface = {
			&pnacl_instance_create,
			&pnacl_instance_destroy,
			&pnacl_instance_change_view,
			&pnacl_instance_change_focus,
			&pnacl_instance_document_load
		};
		return &instance_interface;
	}
	return 0;
}


void pnacl_module_shutdown()
{
}


#endif
