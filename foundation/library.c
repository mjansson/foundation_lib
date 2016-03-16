/* library.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#include <foundation/foundation.h>
#include <foundation/internal.h>

#if FOUNDATION_PLATFORM_WINDOWS
#  include <foundation/windows.h>
#  define FOUNDATION_SUPPORT_LIBRARY_LOAD 1
#elif FOUNDATION_PLATFORM_POSIX
#  include <dlfcn.h>
#  define FOUNDATION_SUPPORT_LIBRARY_LOAD 1
#else
#  define FOUNDATION_SUPPORT_LIBRARY_LOAD 0
#endif

#if FOUNDATION_SUPPORT_LIBRARY_LOAD

struct library_t {
	/*lint -e754 */
	FOUNDATION_DECLARE_OBJECT;

	char    name[32];
	hash_t  name_hash;
	size_t  name_length;

#if FOUNDATION_PLATFORM_WINDOWS
	HANDLE  dll;
#elif FOUNDATION_PLATFORM_POSIX
	void*   lib;
#endif
};

typedef FOUNDATION_ALIGN(8) struct library_t library_t;

static objectmap_t* _library_map;

int
_library_initialize(void) {
	_library_map = objectmap_allocate(_foundation_config.library_max);
	if (!_library_map)
		return -1;
	return 0;
}

void
_library_finalize(void) {
	objectmap_deallocate(_library_map);
	_library_map = 0;
}

static void
_library_destroy(object_t id, void* obj) {
	library_t* library = obj;

	objectmap_free(_library_map, id);

#if FOUNDATION_PLATFORM_WINDOWS
	FreeLibrary(library->dll);
#elif FOUNDATION_PLATFORM_POSIX
	dlclose(library->lib);
#endif

	memory_deallocate(library);
}

object_t
library_load(const char* name, size_t length) {
	library_t* library;
	hash_t name_hash;
	size_t i, size;
	object_t id;
	const char* basename;
	size_t last_slash;
	size_t base_length;
#if FOUNDATION_PLATFORM_WINDOWS
	string_t dllname;
	HANDLE dll;
#endif
	char buf[BUILD_MAX_PATHLEN];

#if FOUNDATION_PLATFORM_APPLE
#  define FOUNDATION_LIB_PRE "lib"
#  define FOUNDATION_LIB_EXT ".dylib"
#elif FOUNDATION_PLATFORM_WINDOWS
#  define FOUNDATION_LIB_EXT ".dll"
#else
#  define FOUNDATION_LIB_PRE "lib"
#  define FOUNDATION_LIB_EXT ".so"
#endif

	basename = name;
	base_length = length;
	last_slash = string_rfind(name, length, '/', STRING_NPOS);
#if FOUNDATION_PLATFORM_WINDOWS
	if (last_slash == STRING_NPOS)
		last_slash = string_rfind(name, length, '\\', STRING_NPOS);
#endif
	if (last_slash != STRING_NPOS) {
		basename = name + last_slash + 1;
		base_length = length - last_slash;
	}

	//Locate already loaded library
	library = 0;
	name_hash = string_hash(basename, base_length);
	for (i = 0, size = objectmap_size(_library_map); i < size; ++i) {
		library = objectmap_raw_lookup(_library_map, i);
		if (library && (library->name_hash == name_hash)) {
			FOUNDATION_ASSERT(string_equal(library->name, library->name_length, basename, base_length));
			atomic_incr32(&library->ref);
			return library->id;
		}
	}

	error_context_push(STRING_CONST("loading library"), name, length);

	//Try loading library
#if FOUNDATION_PLATFORM_WINDOWS

	dll = LoadLibraryA(name);
	if (!dll) {
		size_t last_dot = string_rfind(name, length, '/', STRING_NPOS);
		if ((last_dot == STRING_NPOS) || (last_dot < last_slash)) {
			dllname = string_concat(buf, sizeof(buf), name, length, STRING_CONST(FOUNDATION_LIB_EXT));
			dll = LoadLibraryA(dllname.str);
		}
	}
	if (!dll) {
		string_const_t errmsg = system_error_message(0);
		log_warnf(0, WARNING_SUSPICIOUS, STRING_CONST("Unable to load DLL '%.*s': %.*s"),
		          (int)length, name, STRING_FORMAT(errmsg));
		error_context_pop();
		return 0;
	}

#elif FOUNDATION_PLATFORM_POSIX
	void* lib = dlopen(name, RTLD_LAZY);
	if (!lib && !string_ends_with(name, length, STRING_CONST(FOUNDATION_LIB_EXT))) {
		string_t libname;
		if (last_slash == STRING_NPOS) {
			libname = string_format(buf, sizeof(buf), STRING_CONST(FOUNDATION_LIB_PRE "%.*s" FOUNDATION_LIB_EXT),
			                        (int)length, name);
		}
		else {
			string_const_t path = path_directory_name(name, length);
			string_const_t file = path_file_name(name, length);
			libname = string_format(buf, sizeof(buf),
			                        STRING_CONST("%.*s/" FOUNDATION_LIB_PRE "%.*s" FOUNDATION_LIB_EXT),
			                        (int)path.length, path.str, (int)file.length, file.str);
		}
		lib = dlopen(libname.str, RTLD_LAZY);
	}
#if FOUNDATION_PLATFORM_ANDROID
	if (!lib && (last_slash == STRING_NPOS)) {
		string_t libname;
		string_const_t exe_dir = environment_executable_directory();
		if (!string_ends_with(name, length, STRING_CONST(FOUNDATION_LIB_EXT))) {
			libname = string_format(buf, sizeof(buf),
			                        STRING_CONST("%.*s/" FOUNDATION_LIB_PRE "%.*s" FOUNDATION_LIB_EXT),
			                        STRING_FORMAT(exe_dir), (int)length, name);
		}
		else {
			libname = string_format(buf, sizeof(buf), STRING_CONST("%.*s/" FOUNDATION_LIB_PRE "%.*s"),
			                        STRING_FORMAT(exe_dir), (int)length, name);
		}
		lib = dlopen(libname.str, RTLD_LAZY);
	}
#  endif

	if (!lib) {
		log_warnf(0, WARNING_SUSPICIOUS, STRING_CONST("Unable to load dynamic library '%.*s': %s"),
		          (int)length, name, dlerror());
		error_context_pop();
		return 0;
	}

#endif

	id = objectmap_reserve(_library_map);
	if (!id) {
#if FOUNDATION_PLATFORM_WINDOWS
		FreeLibrary(dll);
#elif FOUNDATION_PLATFORM_POSIX
		dlclose(lib);
#endif
		log_errorf(0, ERROR_OUT_OF_MEMORY, STRING_CONST("Unable to allocate new library '%.*s', map full"),
		           (int)length, name);
		error_context_pop();
		return 0;
	}
	library = memory_allocate(0, sizeof(library_t), 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	_object_initialize((object_base_t*)library, id);
	library->name_hash = name_hash;
	library->name_length = string_copy(library->name, sizeof(library->name), basename,
	                                   base_length).length;
#if FOUNDATION_PLATFORM_WINDOWS
	library->dll = dll;
#elif FOUNDATION_PLATFORM_POSIX
	library->lib = lib;
#endif
	objectmap_set(_library_map, id, library);

	error_context_pop();

	return library->id;
}

object_t
library_ref(object_t id) {
	return objectmap_lookup_ref(_library_map, id) ? id : 0;
}

void
library_unload(object_t id) {
	objectmap_lookup_unref(_library_map, id, _library_destroy);
}

void*
library_symbol(object_t id, const char* name, size_t length) {
	library_t* library = objectmap_lookup(_library_map, id);
	FOUNDATION_UNUSED(length);
	if (library) {
#if FOUNDATION_PLATFORM_WINDOWS
		return (void*)GetProcAddress(library->dll, name);
#elif FOUNDATION_PLATFORM_POSIX
		return dlsym(library->lib, name);
#else
		FOUNDATION_UNUSED(name);
		log_errorf(0, ERROR_NOT_IMPLEMENTED,
		           STRING_CONST("Dynamic library symbol lookup implemented for this platform"));
#endif
	}
	return 0;
}

string_const_t
library_name(object_t id) {
	library_t* library = objectmap_lookup(_library_map, id);
	if (library)
		return string_const(library->name, library->name_length);
	return string_null();
}

bool
library_valid(object_t id) {
	return objectmap_lookup(_library_map, id) != 0;
}

#else

int
_library_initialize(void) {
	return 0;
}

void
_library_finalize(void) {
}

object_t
library_load(const char* name, size_t length) {
	FOUNDATION_UNUSED(name);
	FOUNDATION_UNUSED(length);
	log_error(0, ERROR_NOT_IMPLEMENTED,
	          STRING_CONST("Dynamic library loading not implemented for this platform"));
	return 0;
}

object_t
library_ref(object_t id) {
	FOUNDATION_UNUSED(id);
	return 0;
}

void
library_unload(object_t id) {
	FOUNDATION_UNUSED(id);
}

void*
library_symbol(object_t id, const char* name, size_t length) {
	FOUNDATION_UNUSED(id);
	FOUNDATION_UNUSED(name);
	FOUNDATION_UNUSED(length);
	return 0;
}

string_const_t
library_name(object_t id) {
	FOUNDATION_UNUSED(id);
	return string_empty();
}

bool
library_valid(object_t id) {
	FOUNDATION_UNUSED(id);
	return false;
}

#endif
