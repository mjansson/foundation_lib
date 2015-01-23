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
#include <foundation/system.h>
#include <foundation/pnacl.h>

#include <ppapi/c/pp_instance.h>
#include <ppapi/c/pp_module.h>
#include <ppapi/c/pp_bool.h>
#include <ppapi/c/pp_rect.h>
#include <ppapi/c/pp_resource.h>
#include <ppapi/c/pp_errors.h>
#include <ppapi/c/pp_file_info.h>
#include <ppapi/c/pp_completion_callback.h>

#include <ppapi/c/ppp_instance.h>

extern int real_main( PP_Instance instance );


static PP_Module _pnacl_module = 0;
static PPB_GetInterface _pnacl_browser_interface = 0;
static PP_Instance _pnacl_instance = 0;


const char* pnacl_error_message( int err )
{
	switch( err )
	{
		case PP_OK: return "ok";
		case PP_OK_COMPLETIONPENDING: return "completion pending";
		case PP_ERROR_FAILED: return "failed";
		case PP_ERROR_ABORTED: return "aborted";
		case PP_ERROR_BADARGUMENT: return "bad argument";
		case PP_ERROR_BADRESOURCE: return "bad resource";
		case PP_ERROR_NOINTERFACE: return "no interface";
		case PP_ERROR_NOACCESS: return "no access";
		case PP_ERROR_NOMEMORY: return "no memory";
		case PP_ERROR_NOSPACE: return "no space";
		case PP_ERROR_NOQUOTA: return "no quota";
		case PP_ERROR_INPROGRESS: return "in progress";
		case PP_ERROR_NOTSUPPORTED: return "not supported";
		case PP_ERROR_BLOCKS_MAIN_THREAD: return "blocks main thread";
		case PP_ERROR_MALFORMED_INPUT: return "malformed input";
		case PP_ERROR_RESOURCE_FAILED: return "resource failed";
		case PP_ERROR_FILENOTFOUND: return "file not found";
		case PP_ERROR_FILEEXISTS: return "file exists";
		case PP_ERROR_FILETOOBIG: return "file too big";
		case PP_ERROR_FILECHANGED: return "file changed";
		case PP_ERROR_NOTAFILE: return "not a file";
		case PP_ERROR_TIMEDOUT: return "timeout";
		case PP_ERROR_USERCANCEL: return "user cancel";
		case PP_ERROR_NO_USER_GESTURE: return "no user gesture";
		case PP_ERROR_CONTEXT_LOST: return "context lost";
		case PP_ERROR_NO_MESSAGE_LOOP: return "no message loop";
		case PP_ERROR_WRONG_THREAD: return "wrong thread";
		case PP_ERROR_WOULD_BLOCK_THREAD: return "would block thread";
		case PP_ERROR_CONNECTION_CLOSED: return "connection closed";
		case PP_ERROR_CONNECTION_RESET: return "connection reset";
		case PP_ERROR_CONNECTION_REFUSED: return "connection refused";
		case PP_ERROR_CONNECTION_ABORTED: return "connection aborted";
		case PP_ERROR_CONNECTION_FAILED: return "connection failed";
		case PP_ERROR_CONNECTION_TIMEDOUT: return "connection timeout";
		case PP_ERROR_ADDRESS_INVALID: return "address invalid";
		case PP_ERROR_ADDRESS_UNREACHABLE: return "address unreachable";
		case PP_ERROR_ADDRESS_IN_USE: return "address in use";
		case PP_ERROR_MESSAGE_TOO_BIG: return "message too big";
		case PP_ERROR_NAME_NOT_RESOLVED: return "name not resolved";
		default: break;
	}
	return "UNKNOWN";
}


const void* pnacl_interface( const char* interface )
{
	return _pnacl_browser_interface ? _pnacl_browser_interface( interface ) : 0;
}


PP_Instance pnacl_instance( void )
{
	//TODO: Make this non-static
	return _pnacl_instance;
}


static void* pnacl_instance_main_thread( object_t thread, void* arg )
{
	PP_Instance instance = (PP_Instance)(uintptr_t)arg;
	int ret = real_main( instance );

	return (void*)(uintptr_t)ret;
}


static int pnacl_instance_initialize( PP_Instance instance )
{
	//TODO: Make this non-static
	_pnacl_instance = instance;
	return 0;
}


static PP_Bool pnacl_instance_create( PP_Instance instance, uint32_t argc, const char* argn[], const char* argv[] )
{
	log_debugf( HASH_PNACL, "PNaCL instance create: %d %d", instance, argc );
	for( unsigned int iarg = 0; iarg < argc; ++iarg )
		log_debugf( HASH_PNACL, "  %s = %s", argn[iarg], argv[iarg] );

	if( pnacl_instance_initialize( instance ) < 0 )
		return PP_FALSE;

	pthread_t main_thread;
	int err = pthread_create( &main_thread, 0, pnacl_instance_main_thread, (void*)(uintptr_t)instance );
	if( err )
	{
		log_errorf( 0, ERROR_SYSTEM_CALL_FAIL, "Unable to create main thread: pthread_create failed: %s", system_error_message( err ) );
		return PP_FALSE;
	}

	return PP_TRUE;
}


static void pnacl_instance_destroy( PP_Instance instance )
{
	//TODO: Deostry main thread
}


static void pnacl_instance_change_view( PP_Instance instance, PP_Resource view )
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
	_pnacl_browser_interface = browser;
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
