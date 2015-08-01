/* pnacl.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

/*! \file pnacl.h
\brief Safe inclusion of PNaCl headers

Safe inclusion of PNaCl headers, and PNaCl specific entry points. */

#include <foundation/platform.h>
#include <foundation/types.h>

#if FOUNDATION_PLATFORM_PNACL

#if FOUNDATION_COMPILER_CLANG
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wdocumentation"
#endif

#define __error_t_defined 1

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sched.h>
#include <pwd.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/time.h>

#include <ppapi/c/ppp.h>
#include <ppapi/c/pp_instance.h>
#include <ppapi/c/pp_resource.h>
#include <ppapi/c/pp_errors.h>

/*! PNaCl array wrapper, keeping track of current element count and a pointer
to the array memory */
typedef struct pnacl_array_t {
  /*! Array memory */
  void*         data;
  /*! Current element count */
  unsigned int  count;
} pnacl_array_t;

/*! Initialize PNaCl module instance
\param module_id Module ID
\param get_browser Interface to browser
\return PP_OK if successful, error code if not */
FOUNDATION_API int
pnacl_module_initialize(PP_Module module_id, PPB_GetInterface get_browser);

/*! Get module interface for given interface name
\param interface Interface name string
\param length Length of interface name string
\return Interface, null if not found or error occurred */
FOUNDATION_API const void*
pnacl_module_interface(const char* interface, size_t length);

/*! Finalize PNaCl module instance */
FOUNDATION_API void
pnacl_module_finalize(void);

/*! Get error description string for error code
\param err Error code
\return Error description string */
FOUNDATION_API string_const_t
pnacl_error_message(int err);

/*! Get current PNaCl instance
\return Instance */
FOUNDATION_API PP_Instance
pnacl_instance(void);

/*! Get instance interface for given interface name
\param interface Interface name string
\param length Length of interface name string
\return Interface, null if not found or error occurred */
FOUNDATION_API const void*
pnacl_interface(const char* interface, size_t length);

/*! Array output wrapper, handling allocation of array storage
\param arr Array object pointer
\param count Number of elements
\param size Size of an element
\return Array data pointer */
FOUNDATION_API void*
pnacl_array_output(void* arr, uint32_t count, uint32_t size);

/*! Post log message to JavaScript as a JSON object with "type" set to "log".
Other attributes are "context" holding the log context, "severity" holding the
error level severity and "msg" holding the log message.
\param context Log context
\param severity Error level severity
\param msg Log message
\param msglen Length of log message */
FOUNDATION_API void
pnacl_post_log(hash_t context, error_level_t severity, const char* msg, size_t msglen);

/*! Main entry point for PNaCl instance
\param instance Instance
\return Exit code */
FOUNDATION_API int
pnacl_main(PP_Instance instance);

#if FOUNDATION_COMPILER_CLANG
#  pragma clang diagnostic pop
#endif

#endif
