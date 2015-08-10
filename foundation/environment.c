/* environment.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
#include <foundation/internal.h>

static string_t _environment_executable_name;
static string_t _environment_executable_dir;
static string_t _environment_executable_path;
static string_t _environment_initial_working_dir;
static string_t _environment_current_working_dir;
static string_t _environment_home_dir;
static string_t _environment_temp_dir;
static bool _environment_temp_dir_local;

#if FOUNDATION_PLATFORM_WINDOWS
static string_t _environment_var;
#  include <foundation/windows.h>
#elif FOUNDATION_PLATFORM_POSIX
#  include <foundation/posix.h>
#endif

#if FOUNDATION_PLATFORM_ANDROID
#  include <foundation/android.h>
#endif

#if FOUNDATION_PLATFORM_APPLE
#  include <foundation/apple.h>

extern void
_environment_ns_command_line(string_t** argv);

extern string_t
_environment_ns_home_directory(char*, size_t);

extern string_t
_environment_ns_temporary_directory(char*, size_t);

#endif

#if FOUNDATION_PLATFORM_BSD
#  include <sys/types.h>
#  include <sys/sysctl.h>
#endif

static application_t _environment_app;
static string_t* _environment_argv;
static int _environment_main_argc;
static const char* const* _environment_main_argv;

static void
_environment_clean_temporary_directory(bool recreate);

void
_environment_main_args(int argc, const char* const* argv) {
	_environment_main_argc = argc;
	_environment_main_argv = argv;
}

#if !FOUNDATION_PLATFORM_PNACL

static void
_environment_set_executable_paths(char* executable_path, size_t length) {
	size_t last_path = string_rfind(executable_path, length, '/', STRING_NPOS);
	if (last_path != STRING_NPOS) {
		_environment_executable_dir = string_clone(executable_path, last_path);
		_environment_executable_name = string_clone(executable_path + last_path + 1,
		                                            length - (last_path + 1));
	}
	else {
		_environment_executable_dir = (string_t) {0, 0};
		_environment_executable_name = string_clone(executable_path, length);
	}
#if FOUNDATION_PLATFORM_WINDOWS
	if ((_environment_executable_name.length > 4) &&
	    (string_ends_with(_environment_executable_name.str, _environment_executable_name.length,
	                      STRING_CONST(".exe")) ||
	     string_ends_with(_environment_executable_name.str, _environment_executable_name.length,
	                      STRING_CONST(".EXE")))) {
		_environment_executable_name.str[_environment_executable_name.length - 4] = 0;
		_environment_executable_name.length -= 4;
	}
#endif
	_environment_executable_path = string_clone(executable_path, length);
}

#endif

int
_environment_initialize(const application_t application) {
	string_const_t working_dir;
#if !FOUNDATION_PLATFORM_PNACL
	char buffer[BUILD_MAX_PATHLEN];
#endif

#if FOUNDATION_PLATFORM_WINDOWS
	int ia;
	int num_args = 0;
	DWORD ret = 0;
	wchar_t module_filename[BUILD_MAX_PATHLEN];
	LPWSTR* arg_list = CommandLineToArgvW(GetCommandLineW(), &num_args);
	if (!arg_list)
		return -1;

	for (ia = 0; ia < num_args; ++ia) {
		array_push(_environment_argv, 
		           string_allocate_from_wstring(arg_list[ia], wstring_length(arg_list[ia])));
	}

	LocalFree(arg_list);

	if (GetModuleFileNameW(0, module_filename, BUILD_MAX_PATHLEN)) {
		string_t exe_path = string_convert_utf16(buffer, sizeof(buffer), module_filename,
		                                         wstring_length(module_filename));
		exe_path = path_absolute(exe_path.str, exe_path.length, BUILD_MAX_PATHLEN);

		_environment_set_executable_paths(exe_path.str, exe_path.length);
	}
	else {
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to get module filename"));
		return -1;
	}

#elif FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_IOS

	_environment_ns_command_line(&_environment_argv);

	//TODO: Read executable name from system, not command line (might be set to anything)
	string_t exe_path = string_copy(buffer, sizeof(buffer), _environment_argv[0].str,
	                                _environment_argv[0].length);
	exe_path = path_absolute(exe_path.str, exe_path.length, sizeof(buffer));
	_environment_set_executable_paths(exe_path.str, exe_path.length);

#elif FOUNDATION_PLATFORM_ANDROID

	string_const_t dir_name;
	string_t exe_path;
	char exelink[BUILD_MAX_PATHLEN] = {0};
	ssize_t exelength;
	string_const_t exe_name;

	stream_t* cmdline = fs_open_file(STRING_CONST("/proc/self/cmdline"), STREAM_IN | STREAM_BINARY);
	if (!cmdline) {
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to read /proc/self/cmdline"));
		return -1;
	}

	while (true) {
		string_t arg = stream_read_string(cmdline);
		if (!arg.length) {
			string_deallocate(arg.str);
			break;
		}
		array_push(_environment_argv, arg);
	}
	stream_deallocate(cmdline);

	dir_name = path_directory_name(android_app()->activity->internalDataPath,
	                               string_length(android_app()->activity->internalDataPath));
	exe_path = path_concat(buffer, sizeof(buffer), STRING_ARGS(dir_name), STRING_CONST("lib"));

	// This will return something like "app_process" since we're just a dynamic
	// library that gets invoked by a launcher process
	exelength = readlink("/proc/self/exe", exelink, sizeof(exelink));
	if (exelength < 0) {
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to read /proc/self/exe link"));
		return -1;
	}

	exe_name = path_file_name(exelink, (size_t)exelength);
	exe_path = path_append(exe_path.str, exe_path.length, sizeof(exelink), exe_name.str,
	                       exe_name.length);

	_environment_set_executable_paths(exe_path.str, exe_path.length);

#elif FOUNDATION_PLATFORM_BSD

	for (int ia = 0; ia < _environment_main_argc; ++ia)
		array_push(_environment_argv, string_clone(_environment_main_argv[ia],
		                                           string_length(_environment_main_argv[ia])));

	int callarg[4];
	size_t size = sizeof(buffer);
	callarg[0] = CTL_KERN;
	callarg[1] = KERN_PROC;
	callarg[2] = KERN_PROC_PATHNAME;
	callarg[3] = -1;
	sysctl(callarg, 4, buffer, &size, 0, 0);

	string_t exe_path = path_clean(buffer, size, BUILD_MAX_PATHLEN);
	exe_path = path_absolute(exe_path.str, exe_path.length, BUILD_MAX_PATHLEN);

	_environment_set_executable_paths(exe_path.str, exe_path.length);

#elif FOUNDATION_PLATFORM_POSIX

	stream_t* cmdline = fs_open_file(STRING_CONST("/proc/self/cmdline"), STREAM_IN | STREAM_BINARY);
	if (!cmdline) {
		log_error(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to read /proc/self/cmdline"));
		return -1;
	}

	while (true) {
		string_t arg = stream_read_string(cmdline);
		if (!arg.length) {
			string_deallocate(arg.str);
			break;
		}
		array_push(_environment_argv, arg);
	}
	stream_deallocate(cmdline);

	ssize_t exelength = readlink("/proc/self/exe", buffer, sizeof(buffer));
	if (exelength < 0) {
#if BUILD_ENABLE_LOG
		int err = errno;
		string_const_t errmsg = system_error_message(err);
#endif
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to read /proc/self/exe link: %*s (%d)"),
		           STRING_FORMAT(errmsg), err);
		return -1;
	}

	string_t exe_path = path_clean(buffer, (size_t)exelength, sizeof(buffer));
	exe_path = path_absolute(STRING_ARGS(exe_path), sizeof(buffer));

	_environment_set_executable_paths(exe_path.str, exe_path.length);

#elif FOUNDATION_PLATFORM_PNACL

	for (int ia = 0; ia < _environment_main_argc; ++ia)
		array_push(_environment_argv, string_clone(_environment_main_argv[ia],
		                                           string_length(_environment_main_argv[ia])));

	_environment_executable_dir = string_clone(STRING_CONST("/cache"));
	_environment_current_working_dir = string_clone(STRING_CONST("/cache"));
	_environment_home_dir = string_clone(STRING_CONST("/persistent"));
	_environment_temp_dir = string_clone(STRING_CONST("/tmp"));
	_environment_executable_path = string_clone(STRING_ARGS(application.short_name));

#else
#  error Not implemented
	/*if( array_size( _environment_argv ) > 0 )
	{
		char* exe_path = path_clean( string_clone( _environment_argv[0] ), path_is_absolute( _environment_argv[0] ) );
		char* dir_path = path_make_absolute( exe_path );

		_environment_set_executable_paths( dir_path );

		string_deallocate( dir_path );
		string_deallocate( exe_path );
	}
	else if( !string_length( _environment_executable_dir ) )
	   	string_copy( _environment_executable_dir, environment_current_working_directory(), FOUNDATION_MAX_PATHLEN ); */
#endif

	_environment_app = application;

	if (uuid_is_null(_environment_app.instance))
		_environment_app.instance = uuid_generate_random();

	working_dir = environment_current_working_directory();
	_environment_initial_working_dir = string_clone(working_dir.str, working_dir.length);

	_environment_clean_temporary_directory(true);

	return 0;
}

void
_environment_finalize(void) {
	_environment_clean_temporary_directory(false);

	string_array_deallocate(_environment_argv);

#if FOUNDATION_PLATFORM_WINDOWS
	string_deallocate(_environment_var.str);
#endif
	string_deallocate(_environment_executable_name.str);
	string_deallocate(_environment_executable_dir.str);
	string_deallocate(_environment_executable_path.str);
	string_deallocate(_environment_initial_working_dir.str);
	string_deallocate(_environment_current_working_dir.str);
	string_deallocate(_environment_home_dir.str);
	string_deallocate(_environment_temp_dir.str);

	_environment_executable_name =
	_environment_executable_dir =
	_environment_executable_path =
	_environment_initial_working_dir =
	_environment_current_working_dir =
	_environment_home_dir =
	_environment_temp_dir = (string_t) {0, 0};
}

const string_const_t*
environment_command_line(void) {
	return (const string_const_t*)_environment_argv;
}

string_const_t
environment_executable_name(void) {
	return string_to_const(_environment_executable_name);
}

string_const_t
environment_executable_directory(void) {
	return string_to_const(_environment_executable_dir);
}

string_const_t
environment_executable_path(void) {
	return string_to_const(_environment_executable_path);
}

string_const_t
environment_initial_working_directory(void) {
	return string_to_const(_environment_initial_working_dir);
}

string_const_t
environment_current_working_directory(void) {
	if (_environment_current_working_dir.str)
		return string_to_const(_environment_current_working_dir);
#if FOUNDATION_PLATFORM_WINDOWS
	{
		string_t localpath;
		wchar_t wdbuffer[BUILD_MAX_PATHLEN];
		wchar_t* wd = wdbuffer;
		DWORD ret = GetCurrentDirectoryW(BUILD_MAX_PATHLEN, wd);
		if (ret > BUILD_MAX_PATHLEN) {
			wd = memory_allocate(0, ret * sizeof(wchar_t), 0, MEMORY_TEMPORARY);
			ret = GetCurrentDirectoryW(BUILD_MAX_PATHLEN, wd);
		}
		localpath = string_allocate_from_wstring(wd, ret);
		if (wd != wdbuffer)
			memory_deallocate(wd);
		_environment_current_working_dir = path_clean(STRING_ARGS_CAPACITY(localpath));
	}
#elif FOUNDATION_PLATFORM_POSIX
	string_t localpath = string_thread_buffer();
	if (!getcwd(localpath.str, localpath.length)) {
#if BUILD_ENABLE_LOG
		int err = errno;
		string_const_t errmsg = system_error_message(err);
#endif
		log_errorf(0, ERROR_SYSTEM_CALL_FAIL, STRING_CONST("Unable to get cwd: %*s (%d)"),
		           STRING_FORMAT(errmsg), err);
		return string_const(0, 0);
	}
	localpath = path_clean(localpath.str, string_length(localpath.str), localpath.length);
	if ((localpath.length > 1) && (localpath.str[ localpath.length - 1 ] == '/'))
		localpath.str[ --localpath.length ] = 0;
	_environment_current_working_dir = string_clone(STRING_ARGS(localpath));
#elif FOUNDATION_PLATFORM_PNACL
	_environment_current_working_dir = string_clone(STRING_CONST("/persistent"));
#else
#  error Not implemented
#endif
	return string_to_const(_environment_current_working_dir);
}

void
environment_set_current_working_directory(const char* path, size_t length) {
#if FOUNDATION_PLATFORM_POSIX
	string_t buffer, pathstr;
#endif
	log_debugf(0, STRING_CONST("Setting current working directory to: %*s"), 0, (int)length, path);
#if FOUNDATION_PLATFORM_WINDOWS
	{
		wchar_t* wpath = wstring_allocate_from_string(path, length);
		if (!SetCurrentDirectoryW(wpath))
			log_warnf(0, WARNING_SUSPICIOUS, STRING_CONST("Unable to set working directory: %ls"), wpath);
		wstring_deallocate(wpath);
	}
	string_deallocate(_environment_current_working_dir.str);
	_environment_current_working_dir = (string_t) { 0, 0 };
#elif FOUNDATION_PLATFORM_POSIX
	buffer = string_thread_buffer();
	pathstr = string_copy(STRING_ARGS(buffer), path, length);
	if (chdir(pathstr.str) < 0) {
#if BUILD_ENABLE_LOG
		int err = errno;
		string_const_t errmsg = system_error_message(err);
#endif
		log_warnf(0, WARNING_SYSTEM_CALL_FAIL,
		          STRING_CONST("Unable to set working directory to %*s: %*s (%d)"),
		          (int)length, path, STRING_FORMAT(errmsg), err);
	}
	string_deallocate(_environment_current_working_dir.str);
	_environment_current_working_dir = (string_t) { 0, 0 };
#elif FOUNDATION_PLATFORM_PNACL
	//Allow anything
	char buffer[BUILD_MAX_PATHLEN];
	string_deallocate(_environment_current_working_dir.str);
	_environment_current_working_dir = string_copy(buffer, sizeof(buffer), path, length);
	_environment_current_working_dir = path_absolute(_environment_current_working_dir.str,
	                                                 _environment_current_working_dir.length, sizeof(buffer));
	_environment_current_working_dir = string_clone(STRING_ARGS(_environment_current_working_dir));
#else
#  error Not implemented
#endif
}

string_const_t
environment_home_directory(void) {
	if (_environment_home_dir.str)
		return string_to_const(_environment_home_dir);
#if FOUNDATION_PLATFORM_WINDOWS
	{
		wchar_t wpath[BUILD_MAX_PATHLEN];
		SHGetFolderPathW(0, CSIDL_LOCAL_APPDATA, 0, 0, wpath);
		_environment_home_dir = string_allocate_from_wstring(wpath, wstring_length(wpath));
		_environment_home_dir = path_clean(STRING_ARGS_CAPACITY(_environment_home_dir));
	}
#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_BSD || FOUNDATION_PLATFORM_TIZEN
	string_const_t env_home = environment_variable(STRING_CONST("HOME"));
	if (!env_home.length) {
		struct passwd* pw = getpwuid(getuid());
		env_home.str = pw->pw_dir;
		env_home.length = string_length(env_home.str);
	}
	_environment_home_dir = string_clone(env_home.str, env_home.length);
#elif FOUNDATION_PLATFORM_IOS || FOUNDATION_PLATFORM_MACOSX
	string_t pathstr = string_allocate(0, BUILD_MAX_PATHLEN);
	pathstr = _environment_ns_home_directory(pathstr.str, BUILD_MAX_PATHLEN);
#  if FOUNDATION_PLATFORM_MACOSX
	if (!(environment_application()->flags & APPLICATION_UTILITY)) {
		char bundle_identifier[256];
		string_t bundle = environment_bundle_identifier(bundle_identifier, 256);
		pathstr = path_append_varg(STRING_ARGS(pathstr), BUILD_MAX_PATHLEN,
		                           STRING_CONST("Library/Application Support"),
		                           STRING_ARGS(bundle), nullptr);
	}
#  endif
	_environment_home_dir = string_clone(pathstr.str, pathstr.length);
	string_deallocate(pathstr.str);
#elif FOUNDATION_PLATFORM_ANDROID
	_environment_home_dir = string_clone(android_app()->activity->internalDataPath,
	                                     string_length(android_app()->activity->internalDataPath));
#elif FOUNDATION_PLATFORM_PNACL
#else
#  error Not implemented
#endif
	return string_to_const(_environment_home_dir);
}

string_const_t
environment_temporary_directory(void) {
	if (_environment_temp_dir.str)
		return string_to_const(_environment_temp_dir);
#if FOUNDATION_PLATFORM_WINDOWS
	{
		wchar_t wpath[BUILD_MAX_PATHLEN];
		GetTempPathW(BUILD_MAX_PATHLEN, wpath);
		_environment_temp_dir = string_allocate_from_wstring(wpath, wstring_length(wpath));
		_environment_temp_dir = path_clean(STRING_ARGS_CAPACITY(_environment_temp_dir));
		_environment_temp_dir = path_absolute(STRING_ARGS_CAPACITY(_environment_temp_dir));
	}
#elif FOUNDATION_PLATFORM_ANDROID
	//Use application internal data path, or if that fails, external data path
	struct android_app* app = android_app();
	const char* test_path[] = { app&& app->activity ? app->activity->internalDataPath : 0, app&& app->activity ? app->activity->externalDataPath : 0 };
	char path[BUILD_MAX_PATHLEN];
	for (int itest = 0; !_environment_temp_dir.str && (itest < 2); ++itest) {
		if (test_path[itest] && test_path[itest][0]) {
			string_t pathstr = string_copy(path, sizeof(path), test_path[itest],
			                               string_length(test_path[itest]));
			pathstr = path_clean(STRING_ARGS(pathstr), sizeof(path));

			fs_make_directory(STRING_ARGS(pathstr));

			string_t temp_path = path_append(STRING_ARGS(pathstr), sizeof(path), STRING_CONST(".tmp-"));
			string_const_t uuidstr = string_from_uuid_static(uuid_generate_random());
			temp_path = string_append(STRING_ARGS(temp_path), sizeof(path), STRING_ARGS(uuidstr));

			stream_t* temp_stream = fs_open_file(STRING_ARGS(temp_path),
			                                     STREAM_CREATE | STREAM_OUT | STREAM_BINARY);
			if (temp_stream) {
				stream_deallocate(temp_stream);

				_environment_temp_dir = path_append(STRING_ARGS(pathstr), sizeof(path), STRING_CONST(".tmp"));
				_environment_temp_dir = string_clone(STRING_ARGS(_environment_temp_dir));

				_environment_temp_dir_local = true;
			}
		}
	}
#else
#  if FOUNDATION_PLATFORM_APPLE
	char* buffer = memory_allocate(HASH_STRING, BUILD_MAX_PATHLEN, 0, MEMORY_PERSISTENT);
	_environment_temp_dir = _environment_ns_temporary_directory(buffer, BUILD_MAX_PATHLEN);
	_environment_temp_dir = string_clone(STRING_ARGS(_environment_temp_dir));
#    if FOUNDATION_PLATFORM_IOS
	_environment_temp_dir_local = true;
#    endif
#  elif FOUNDATION_PLATFORM_POSIX
	_environment_temp_dir = string_clone(P_tmpdir, string_length(P_tmpdir));
#  endif
#endif

#if !FOUNDATION_PLATFORM_ANDROID && !FOUNDATION_PLATFORM_IOS
	if (_environment_app.config_dir.length) {
		size_t curlen = _environment_temp_dir.length;
		size_t cfglen = _environment_app.config_dir.length;
		size_t totallen = curlen + cfglen + 40;
		if (totallen < BUILD_MAX_PATHLEN) {
			string_t modpath = string_allocate(0, totallen - 1);
			string_const_t uuidstr = string_from_uuid_static(_environment_app.instance);
			modpath = string_copy(modpath.str, totallen, STRING_ARGS(_environment_temp_dir));
			modpath = path_append_varg(STRING_ARGS(modpath), totallen, STRING_ARGS(_environment_app.config_dir),
			                           STRING_ARGS(uuidstr), nullptr);
			string_deallocate(_environment_temp_dir.str);
			_environment_temp_dir = modpath;
			_environment_temp_dir_local = true;
		}
	}
#endif
#  if FOUNDATION_PLATFORM_APPLE
	if (_environment_temp_dir.str == buffer)
		_environment_temp_dir = string_clone(STRING_ARGS(_environment_temp_dir));
	string_deallocate(buffer);
#endif
	if ((_environment_temp_dir.length > 1) &&
	    (_environment_temp_dir.str[ _environment_temp_dir.length - 1 ] == '/')) {
		--_environment_temp_dir.length;
		_environment_temp_dir.str[ _environment_temp_dir.length ] = 0;
	}
	return string_to_const(_environment_temp_dir);
}

static void
_environment_clean_temporary_directory(bool recreate) {
	string_const_t path = environment_temporary_directory();

	if (_environment_temp_dir_local && fs_is_directory(path.str, path.length)) {
		fs_remove_directory(path.str, path.length);
		if (recreate)
			fs_make_directory(path.str, path.length);
	}
}

string_const_t
environment_variable(const char* var, size_t length) {
#if !FOUNDATION_PLATFORM_PNACL
	string_t buffer = string_thread_buffer();
	string_t varstr = string_copy(STRING_ARGS(buffer), var, length);
#endif
#if FOUNDATION_PLATFORM_WINDOWS
	unsigned int required;
	wchar_t* key = wstring_allocate_from_string(STRING_ARGS(varstr));
	wchar_t val[BUILD_MAX_PATHLEN]; val[0] = 0;
	if ((required = GetEnvironmentVariableW(key, val, BUILD_MAX_PATHLEN)) > BUILD_MAX_PATHLEN) {
		wchar_t* val_local = memory_allocate(0, sizeof(wchar_t) * (required + 2), 0, MEMORY_TEMPORARY);
		val_local[0] = 0;
		required = GetEnvironmentVariableW(key, val_local, required + 1);
		if (_environment_var.str)
			string_deallocate(_environment_var.str);
		_environment_var = string_allocate_from_wstring(val_local, required);
		memory_deallocate(val_local);
	}
	else {
		if (_environment_var.str)
			string_deallocate(_environment_var.str);
		_environment_var = string_allocate_from_wstring(val, required);
	}
	wstring_deallocate(key);
	return string_to_const(_environment_var);
#elif FOUNDATION_PLATFORM_POSIX
	const char* value = getenv(varstr.str);
	return string_const(value, value ? string_length(value) : 0);
#elif FOUNDATION_PLATFORM_PNACL
	FOUNDATION_UNUSED(var);
	FOUNDATION_UNUSED(length);
	return string_const(0, 0);   //No env vars on PNaCl
#else
#  error Not implemented
#endif
}

const application_t*
environment_application(void) {
	return &_environment_app;
}
