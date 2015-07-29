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
#include <foundation/internal.h>


#define CONFIG_SECTION_BUCKETS 7
#define CONFIG_KEY_BUCKETS     11

enum config_type_t {
	CONFIGVALUE_BOOL = 0,
	CONFIGVALUE_INT,
	CONFIGVALUE_REAL,
	CONFIGVALUE_STRING,
	CONFIGVALUE_STRING_CONST,
	CONFIGVALUE_STRING_VAR,
	CONFIGVALUE_STRING_CONST_VAR
};

struct config_key_t {
	hash_t name;
	enum config_type_t type;
	bool bval;
	int64_t ival;
	string_t sval;
	string_t expanded;
	real rval;
};

struct config_section_t {
	hash_t name;
	struct config_key_t* key[CONFIG_KEY_BUCKETS];
};

typedef enum config_type_t config_type_t;
typedef FOUNDATION_ALIGN(8) struct config_key_t config_key_t;
typedef FOUNDATION_ALIGN(8) struct config_section_t config_section_t;

FOUNDATION_STATIC_ASSERT(FOUNDATION_ALIGNOF(config_key_t) == 8, "config_key_t alignment");
FOUNDATION_STATIC_ASSERT(FOUNDATION_ALIGNOF(config_section_t) == 8, "config_section_t alignment");

//Global config store
static config_section_t* _config_section[CONFIG_SECTION_BUCKETS];

static int64_t
_config_string_to_int(string_const_t str) {
	size_t first_nonnumeric;
	size_t dot_position;
	if (str.length < 2)
		return string_to_int64(STRING_ARGS(str));

	first_nonnumeric = string_find_first_not_of(STRING_ARGS(str), STRING_CONST("0123456789."), 0);
	if ((first_nonnumeric == (str.length - 1)) && ((str.str[ first_nonnumeric ] == 'm') ||
	                                               (str.str[ first_nonnumeric ] == 'M'))) {
		dot_position = string_find(STRING_ARGS(str), '.', 0);
		if (dot_position != STRING_NPOS) {
			if (string_find(STRING_ARGS(str), '.', dot_position + 1) == STRING_NPOS)
				return (int64_t)(string_to_float64(STRING_ARGS(str)) * (1024.0 * 1024.0));
			return string_to_int64(STRING_ARGS(str));     //More than one dot
		}
		return string_to_int64(STRING_ARGS(str)) * (1024LL * 1024LL);
	}
	if ((first_nonnumeric == (str.length - 1)) && ((str.str[ first_nonnumeric ] == 'k') ||
	                                               (str.str[ first_nonnumeric ] == 'K'))) {
		dot_position = string_find(STRING_ARGS(str), '.', 0);
		if (dot_position != STRING_NPOS) {
			if (string_find(STRING_ARGS(str), '.', dot_position + 1) == STRING_NPOS)
				return (int64_t)(string_to_float64(STRING_ARGS(str)) * 1024.0);
			return string_to_int64(STRING_ARGS(str));     //More than one dot
		}
		return string_to_int64(STRING_ARGS(str)) * 1024LL;
	}

	return string_to_int64(STRING_ARGS(str));
}

static real
_config_string_to_real(string_const_t str) {
	size_t first_nonnumeric;
	size_t dot_position;
	if (str.length < 2)
		return string_to_real(STRING_ARGS(str));

	first_nonnumeric = string_find_first_not_of(STRING_ARGS(str), STRING_CONST("0123456789."), 0);
	if ((first_nonnumeric == (str.length - 1)) && ((str.str[ first_nonnumeric ] == 'm') ||
	                                               (str.str[ first_nonnumeric ] == 'M'))) {
		dot_position = string_find(STRING_ARGS(str), '.', 0);
		if (dot_position != STRING_NPOS) {
			if (string_find(STRING_ARGS(str), '.', dot_position + 1) != STRING_NPOS)
				return string_to_real(STRING_ARGS(str));     //More than one dot
		}
		return string_to_real(STRING_ARGS(str)) * (REAL_C(1024.0) * REAL_C(1024.0));
	}
	if ((first_nonnumeric == (str.length - 1)) && ((str.str[ first_nonnumeric ] == 'k') ||
	                                               (str.str[ first_nonnumeric ] == 'K'))) {
		dot_position = string_find(STRING_ARGS(str), '.', 0);
		if (dot_position != STRING_NPOS) {
			if (string_find(STRING_ARGS(str), '.', dot_position + 1) != STRING_NPOS)
				return string_to_real(STRING_ARGS(str));     //More than one dot
		}
		return string_to_real(STRING_ARGS(str)) * REAL_C(1024.0);
	}

	return string_to_real(STRING_ARGS(str));
}

static FOUNDATION_NOINLINE string_const_t
_expand_environment(hash_t key, string_const_t var) {
	if (key == HASH_EXECUTABLE_NAME)
		return environment_executable_name();
	else if (key == HASH_EXECUTABLE_DIRECTORY)
		return environment_executable_directory();
	else if (key == HASH_EXECUTABLE_PATH)
		return environment_executable_path();
	else if (key == HASH_INITIAL_WORKING_DIRECTORY)
		return environment_initial_working_directory();
	else if (key == HASH_CURRENT_WORKING_DIRECTORY)
		return environment_current_working_directory();
	else if (key == HASH_HOME_DIRECTORY)
		return environment_home_directory();
	else if (key == HASH_TEMPORARY_DIRECTORY)
		return environment_temporary_directory();
	else if ((var.length > 9) && //variable[varname] - Environment variable named "varname"
	         string_equal(var.str, 9, STRING_CONST("variable["))) {
		string_const_t substr = string_substr(STRING_ARGS(var), 9, var.length - 10);
		return environment_variable(STRING_ARGS(substr));
	}
	return string_null();
}

static FOUNDATION_NOINLINE string_t
_expand_string(hash_t section_current, string_t str) {
	string_const_t variable, value;
	size_t var_pos, var_end_pos, separator, var_offset, capacity, newlength;
	hash_t section, key;
	string_t expanded;

	capacity = 0;
	newlength = str.length + 1;
	expanded = str;
	var_pos = string_find_string(STRING_ARGS(expanded), STRING_CONST("$("), 0);

	while (var_pos != STRING_NPOS) {
		var_end_pos = string_find(STRING_ARGS(expanded), ')', var_pos + 2);
		FOUNDATION_ASSERT_MSG(var_end_pos != STRING_NPOS, "Malformed config variable statement");
		variable = string_substr(STRING_ARGS(expanded), var_pos,
		                         (var_end_pos != STRING_NPOS) ? (1 + var_end_pos - var_pos) : STRING_NPOS);

		section = section_current;
		key = 0;
		separator = string_find(STRING_ARGS(variable), ':', 0);
		if (separator != STRING_NPOS) {
			if (separator != 2)
				section = hash(variable.str + 2, separator - 2);
			var_offset = separator + 1;
		}
		else {
			var_offset = 2;
		}
		key = hash(variable.str + var_offset,
		           variable.length - (var_offset + (variable.str[ variable.length - 1 ] == ')' ? 1 : 0)));

		if (section != HASH_ENVIRONMENT)
			value = config_string(section, key);
		else
			value = _expand_environment(key, string_substr(STRING_ARGS(variable), var_offset,
			                                               variable.length - var_offset - 1));

		newlength += (value.length > variable.length) ? value.length - variable.length : 0;
		if (newlength >= capacity) {
			size_t allocsize = newlength + 32;
			expanded.str = capacity ? memory_reallocate(expanded.str, allocsize, 0, capacity) :
			               memory_allocate(HASH_STRING, allocsize, 0, MEMORY_PERSISTENT);
			if (!capacity)
				string_copy(expanded.str, allocsize, STRING_ARGS(str));
			capacity = allocsize;
		}

		expanded = string_replace(STRING_ARGS(expanded), capacity, STRING_ARGS(variable),
		                          STRING_ARGS(value), false);

		var_pos = string_find_string(STRING_ARGS(expanded), STRING_CONST("$("), var_pos);
	}
#if BUILD_ENABLE_CONFIG_DEBUG
	if (str.str != expanded.str)
		log_debugf(HASH_CONFIG, STRING_CONST("Expanded config value \"%*s\" to \"%*s\""),
		           STRING_FORMAT(str), STRING_FORMAT(expanded));
#endif

	return expanded;
}

static FOUNDATION_NOINLINE void
_expand_string_val(hash_t section, config_key_t* key) {
	bool is_true;
	FOUNDATION_ASSERT(key->sval.str);
	if (key->expanded.str != key->sval.str)
		string_deallocate(key->expanded.str);
	key->expanded = _expand_string(section, key->sval);

	is_true = string_equal(STRING_ARGS(key->expanded), STRING_CONST("true"));
	key->bval = (string_equal(STRING_ARGS(key->expanded), STRING_CONST("false")) ||
	             string_equal(STRING_ARGS(key->expanded), STRING_CONST("0")) ||
	             !key->expanded.length) ? false : true;
	key->ival = is_true ? 1 : _config_string_to_int(string_to_const(key->expanded));
	key->rval = is_true ? REAL_C(1.0) : _config_string_to_real(string_to_const(key->expanded));
}

int _config_initialize(void) {
	config_load(STRING_CONST("foundation"), 0ULL, true, false);
	config_load(STRING_CONST("application"), 0ULL, true, false);

	//Load per-user config
	config_load(STRING_CONST("user"), HASH_USER, false, true);

	return 0;
}

void
_config_finalize(void) {
	size_t isb, is, ikb, ik, ssize, ksize;
	config_section_t* section;
	config_key_t* key;
	for (isb = 0; isb < CONFIG_SECTION_BUCKETS; ++isb) {
		section = _config_section[isb];
		for (is = 0, ssize = array_size(section); is < ssize; ++is) {
			for (ikb = 0; ikb < CONFIG_KEY_BUCKETS; ++ikb) {
				/*lint -e{613} array_size( section ) in loop condition does the null pointer guard */
				key = section[is].key[ikb];
				for (ik = 0, ksize = array_size(key); ik < ksize; ++ik) {
					/*lint --e{613} array_size( key ) in loop condition does the null pointer guard */
					if (key[ik].expanded.str != key[ik].sval.str)
						string_deallocate(key[ik].expanded.str);
					if ((key[ik].type != CONFIGVALUE_STRING_CONST) && (key[ik].type != CONFIGVALUE_STRING_CONST_VAR))
						string_deallocate(key[ik].sval.str);
				}
				array_deallocate(key);
			}
		}
		array_deallocate(section);
	}
}

static string_const_t platformsuffix =
#if FOUNDATION_PLATFORM_WINDOWS
{ STRING_CONST("/windows") };
#elif FOUNDATION_PLATFORM_MACOSX
  { STRING_CONST("/macosx") };
#elif FOUNDATION_PLATFORM_IOS
  { STRING_CONST("/ios") };
#elif FOUNDATION_PLATFORM_ANDROID
  { STRING_CONST("/android") };
#elif FOUNDATION_PLATFORM_LINUX_RASPBERRYPI
  { STRING_CONST("/raspberrypi") };
#elif FOUNDATION_PLATFORM_LINUX
  { STRING_CONST("/linux") };
#elif FOUNDATION_PLATFORM_PNACL
  { STRING_CONST("/pnacl") };
#elif FOUNDATION_PLATFORM_BSD
  { STRING_CONST("/bsd") };
#elif FOUNDATION_PLATFORM_TIZEN
  { STRING_CONST("/tizen") };
#else
  { STRING_CONST("/unknown") };
#endif

#if !FOUNDATION_PLATFORM_PNACL

static string_t
config_unsuffix_path(string_t path) {
	string_const_t buildsuffix =
#if BUILD_DEBUG
	(string_const_t) { STRING_CONST("/debug") };
#elif BUILD_RELEASE
	(string_const_t) { STRING_CONST("/release") };
#elif BUILD_PROFILE
	(string_const_t) { STRING_CONST("/profile") };
#else
	(string_const_t) { STRING_CONST("/deploy") };
#endif
	string_const_t binsuffix =
	(string_const_t) { STRING_CONST("/bin") };

	if (string_ends_with(STRING_ARGS(path), STRING_ARGS(buildsuffix))) {
		path.length = path.length - buildsuffix.length;
		path.str[ path.length ] = 0;
	}
	if (string_ends_with(STRING_ARGS(path), STRING_ARGS(platformsuffix))) {
		path.length = path.length - platformsuffix.length;
		path.str[ path.length ] = 0;
	}
	if (string_ends_with(STRING_ARGS(path), STRING_ARGS(binsuffix))) {
		path.length = path.length - binsuffix.length;
		path.str[ path.length ] = 0;
	}
	return path;
}

#endif

static string_t
config_make_path(int path, char* buffer, size_t capacity) {
#if !FOUNDATION_PLATFORM_PNACL
	string_t result;
#endif
	string_const_t env_dir;
#if FOUNDATION_PLATFORM_FAMILY_DESKTOP && !BUILD_DEPLOY
	const string_const_t* cmd_line;
	size_t icl, clsize;
#endif
	switch (path) {
	case 0:
		env_dir = environment_executable_directory();
		return string_copy(buffer, capacity, STRING_ARGS(env_dir));

	case 1:
#if !FOUNDATION_PLATFORM_PNACL
		env_dir = environment_executable_directory();
		return path_concat(buffer, capacity, STRING_ARGS(env_dir), STRING_CONST("config"));
#else
		break;
#endif

	case 2:
#if !FOUNDATION_PLATFORM_PNACL
		env_dir = environment_executable_directory();
		result = string_copy(buffer, capacity, STRING_ARGS(env_dir));
		result = config_unsuffix_path(result);
		if (result.length == env_dir.length)
			return (string_t) {0, 0};
		return path_append(STRING_ARGS(result), capacity, STRING_CONST("config"));
#else
		break;
#endif

	case 3:
#if FOUNDATION_PLATFORM_FAMILY_DESKTOP && !BUILD_DEPLOY
		env_dir = environment_initial_working_directory();
		return string_copy(buffer, capacity, STRING_ARGS(env_dir));
#else
		break;
#endif

	case 4:
#if FOUNDATION_PLATFORM_MACOSX
		env_dir = environment_executable_directory();
		result = string_copy(buffer, capacity, STRING_ARGS(env_dir));
		result = path_append(STRING_ARGS(result), capacity, STRING_CONST("../Resources/config"));
		return path_clean(STRING_ARGS(result), capacity);
#elif FOUNDATION_PLATFORM_ANDROID
		return string_copy(buffer, capacity, STRING_CONST("asset://config"));
#else
		break;
#endif

	case 5:
#if FOUNDATION_PLATFORM_FAMILY_DESKTOP
		env_dir = environment_current_working_directory();
		return string_copy(buffer, capacity, STRING_ARGS(env_dir));
#else
		break;
#endif

	case 6:
#if FOUNDATION_PLATFORM_FAMILY_DESKTOP
		env_dir = environment_current_working_directory();
		result = string_copy(buffer, capacity, STRING_ARGS(env_dir));
		return path_append(STRING_ARGS(result), capacity, STRING_CONST("config"));
#else
		break;
#endif

	case 7:
#if FOUNDATION_PLATFORM_FAMILY_DESKTOP && !BUILD_DEPLOY
		cmd_line = environment_command_line();
		env_dir = string_null();
		/*lint -e{850} We modify loop var to skip extra arg */
		for (icl = 0, clsize = array_size(cmd_line); icl < clsize; ++icl) {
			/*lint -e{613} array_size( cmd_line ) in loop condition does the null pointer guard */
			if (string_equal(STRING_ARGS(cmd_line[icl]), STRING_CONST("--configdir"))) {
				if (string_equal(STRING_ARGS(cmd_line[icl]), STRING_CONST("--configdir="))) {
					env_dir = string_substr(STRING_ARGS(cmd_line[icl]), 12, STRING_NPOS);
					break;
				}
				else if (icl < (clsize - 1)) {
					env_dir = cmd_line[++icl];
					break;
				}
			}
		}
		if (env_dir.length)
			return string_copy(buffer, capacity, STRING_ARGS(env_dir));
#endif
		break;

	case 8:
#if FOUNDATION_PLATFORM_FAMILY_DESKTOP
		env_dir = environment_home_directory();
		return string_concat_varg(buffer, capacity, STRING_ARGS(env_dir),
		                          STRING_CONST("/."), STRING_ARGS(environment_application()->config_dir), nullptr);
#else
		break;
#endif
	}
	return (string_t) { 0, 0 };
}

void
config_load(const char* name, size_t length, hash_t filter_section, bool built_in, bool overwrite) {
	char buffer[BUILD_MAX_PATHLEN];
	string_t pathname;
	string_t filename;
	stream_t* istream;
	int start_path = 0;
	int end_path = 8;
	int ipath;

	if (!built_in) {
#if FOUNDATION_PLATFORM_WINDOWS || FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_BSD
		start_path = 8;
		end_path = 9;
#else
		return;
#endif
	}

	for (ipath = start_path; ipath < end_path; ++ipath) {
		pathname = config_make_path(ipath, buffer, sizeof(buffer));
		if (!pathname.length)
			continue;

		//TODO: Support loading configs from virtual file system (i.e in zip/other packages)
		filename = string_format(pathname.str + pathname.length, sizeof(buffer) - pathname.length,
		                         STRING_CONST("/%*s.ini"), (int)length, name);
		filename.str = pathname.str;
		filename.length += pathname.length;
		istream = stream_open(STRING_ARGS(filename), STREAM_IN);
		if (istream) {
			config_parse(istream, filter_section, overwrite);
			stream_deallocate(istream);
		}

		if (built_in) {
			filename = string_format(pathname.str + pathname.length, sizeof(buffer) - pathname.length,
			                         STRING_CONST("%*s/%*s.ini"), STRING_FORMAT(platformsuffix), (int)length, name);
			filename.str = pathname.str;
			filename.length += pathname.length;
			istream = stream_open(STRING_ARGS(filename), STREAM_IN);
			if (istream) {
				config_parse(istream, filter_section, overwrite);
				stream_deallocate(istream);
			}
		}
	}
}

static FOUNDATION_NOINLINE config_section_t*
config_section(hash_t section, bool create) {
	config_section_t* bucket;
	size_t ib, bsize;

	bucket = _config_section[ section % CONFIG_SECTION_BUCKETS ];
	for (ib = 0, bsize = array_size(bucket); ib < bsize; ++ib) {
		/*lint --e{613} array_size( bucket ) in loop condition does the null pointer guard */
		if (bucket[ib].name == section)
			return bucket + ib;
	}

	if (!create)
		return 0;

	//TODO: Thread safeness
	{
		config_section_t new_section;
		memset(&new_section, 0, sizeof(new_section));
		new_section.name = section;

		array_push_memcpy(bucket, &new_section);
		_config_section[ section % CONFIG_SECTION_BUCKETS ] = bucket;
	}

	return bucket + bsize;
}

static FOUNDATION_NOINLINE config_key_t*
config_key(hash_t section, hash_t key, bool create) {
	config_key_t new_key;
	config_section_t* csection;
	config_key_t* bucket;
	size_t ib, bsize;

	csection = config_section(section, create);
	if (!csection) {
		FOUNDATION_ASSERT(!create);
		return 0;
	}
	bucket = csection->key[ key % CONFIG_KEY_BUCKETS ];
	for (ib = 0, bsize = array_size(bucket); ib < bsize; ++ib) {
		/*lint --e{613} array_size( bucket ) in loop condition does the null pointer guard */
		if (bucket[ib].name == key)
			return bucket + ib;
	}

	if (!create)
		return 0;

	memset(&new_key, 0, sizeof(new_key));
	new_key.name = key;

	//TODO: Thread safeness
	array_push_memcpy(bucket, &new_key);
	csection->key[ key % CONFIG_KEY_BUCKETS ] = bucket;

	return bucket + bsize;
}

bool
config_bool(hash_t section, hash_t key) {
	config_key_t* key_val = config_key(section, key, false);
	if (key_val && (key_val->type >= CONFIGVALUE_STRING_VAR))
		_expand_string_val(section, key_val);
	return key_val ? key_val->bval : false;
}

int64_t
config_int(hash_t section, hash_t key) {
	config_key_t* key_val = config_key(section, key, false);
	if (key_val && (key_val->type >= CONFIGVALUE_STRING_VAR))
		_expand_string_val(section, key_val);
	return key_val ? key_val->ival : 0;
}

real
config_real(hash_t section, hash_t key) {
	config_key_t* key_val = config_key(section, key, false);
	if (key_val && (key_val->type >= CONFIGVALUE_STRING_VAR))
		_expand_string_val(section, key_val);
	return key_val ? key_val->rval : 0;
}

string_const_t
config_string(hash_t section, hash_t key) {
	config_key_t* key_val = config_key(section, key, false);
	if (!key_val)
		return string_const("", 0);
	//Convert to string
	/*lint --e{788} We use default for remaining enums */
	switch (key_val->type) {
	case CONFIGVALUE_BOOL:
		return key_val->bval ? (string_const_t) {STRING_CONST("true")} :
		                       (string_const_t) {STRING_CONST("false")};

	case CONFIGVALUE_INT:
		if (!key_val->sval.str)
			key_val->sval = string_clone_string(string_from_int_static(key_val->ival, 0, 0));
		return string_to_const(key_val->sval);

	case CONFIGVALUE_REAL:
		if (!key_val->sval.str)
			key_val->sval = string_clone_string(string_from_real_static(key_val->rval, 4, 0, '0'));
		return string_to_const(key_val->sval);

	case CONFIGVALUE_STRING:
	case CONFIGVALUE_STRING_CONST:
	case CONFIGVALUE_STRING_VAR:
	case CONFIGVALUE_STRING_CONST_VAR:
		break;
	}
	//String value of some form
	if (!key_val->sval.str)
		return string_const("", 0);
	if (key_val->type >= CONFIGVALUE_STRING_VAR) {
		_expand_string_val(section, key_val);
		return string_to_const(key_val->expanded);
	}
	return string_to_const(key_val->sval);
}

hash_t
config_hash(hash_t section, hash_t key) {
	string_const_t value = config_string(section, key);
	return value.length ? hash(STRING_ARGS(value)) : HASH_EMPTY_STRING;
}

#define CLEAR_KEY_STRINGS( key_val ) \
	if( key_val->expanded.str != key_val->sval.str ) \
		string_deallocate( key_val->expanded.str ); \
	if( ( key_val->type != CONFIGVALUE_STRING_CONST ) && ( key_val->type != CONFIGVALUE_STRING_CONST_VAR ) ) \
		string_deallocate( key_val->sval.str ); \
	key_val->expanded = key_val->sval = (string_t){ 0, 0 }

void
config_set_bool(hash_t section, hash_t key, bool value) {
	config_key_t* key_val = config_key(section, key, true);
	if (!FOUNDATION_VALIDATE(key_val)) return;
	key_val->bval = value;
	key_val->ival = (value ? 1 : 0);
	key_val->rval = (value ? REAL_C(1.0) : REAL_C(0.0));
	CLEAR_KEY_STRINGS(key_val);
	key_val->type = CONFIGVALUE_BOOL;
}

void
config_set_int(hash_t section, hash_t key, int64_t value) {
	config_key_t* key_val = config_key(section, key, true);
	if (!FOUNDATION_VALIDATE(key_val)) return;
	key_val->bval = value ? true : false;
	key_val->ival = value;
	key_val->rval = (real)value;
	CLEAR_KEY_STRINGS(key_val);
	key_val->type = CONFIGVALUE_INT;
}

void
config_set_real(hash_t section, hash_t key, real value) {
	config_key_t* key_val = config_key(section, key, true);
	if (!FOUNDATION_VALIDATE(key_val)) return;
	key_val->bval = !math_real_is_zero(value);
	key_val->ival = (int64_t)value;
	key_val->rval = value;
	CLEAR_KEY_STRINGS(key_val);
	key_val->type = CONFIGVALUE_REAL;
}

void
config_set_string(hash_t section, hash_t key, const char* value, size_t length) {
	config_key_t* key_val = config_key(section, key, true);
	if (!FOUNDATION_VALIDATE(key_val)) return;
	CLEAR_KEY_STRINGS(key_val);

	key_val->sval = string_clone(value, length);
	key_val->type = ((string_find_string(key_val->sval.str, key_val->sval.length, STRING_CONST("$("),
	                                     0) != STRING_NPOS) ? CONFIGVALUE_STRING_VAR : CONFIGVALUE_STRING);

	if (key_val->type == CONFIGVALUE_STRING) {
		bool is_true = string_equal(STRING_ARGS(key_val->sval), STRING_CONST("true"));
		key_val->bval = (string_equal(STRING_ARGS(key_val->sval), STRING_CONST("false")) ||
		                 string_equal(STRING_ARGS(key_val->sval), STRING_CONST("0")) ||
		                 !key_val->sval.length) ? false : true;
		key_val->ival = is_true ? 1 : _config_string_to_int(string_to_const(key_val->sval));
		key_val->rval = is_true ? REAL_C(1.0) : _config_string_to_real(string_to_const(key_val->sval));
	}
}

void
config_set_string_constant(hash_t section, hash_t key, const char* value, size_t length) {
	config_key_t* key_val = config_key(section, key, true);
	if (!FOUNDATION_VALIDATE(key_val)) return;
	if (!FOUNDATION_VALIDATE(value)) return;
	CLEAR_KEY_STRINGS(key_val);

	//key_val->sval = (char*)value;
	memcpy(&key_val->sval.str, &value,
	       sizeof(char*));     //Yeah yeah, we're storing a const pointer in a non-const var
	key_val->sval.length = length;
	key_val->type = ((string_find_string(STRING_ARGS(key_val->sval), STRING_CONST("$("),
	                                     0) != STRING_NPOS) ? CONFIGVALUE_STRING_CONST_VAR : CONFIGVALUE_STRING_CONST);

	if (key_val->type == CONFIGVALUE_STRING_CONST) {
		bool is_true = string_equal(STRING_ARGS(key_val->sval), STRING_CONST("true"));
		key_val->bval = (string_equal(STRING_ARGS(key_val->sval), STRING_CONST("false")) ||
		                 string_equal(STRING_ARGS(key_val->sval), STRING_CONST("0")) ||
		                 !key_val->sval.length) ? false : true;
		key_val->ival = is_true ? 1 : _config_string_to_int(string_to_const(key_val->sval));
		key_val->rval = is_true ? REAL_C(1.0) : _config_string_to_real(string_to_const(key_val->sval));
	}
}

void
config_parse(stream_t* stream, hash_t filter_section, bool overwrite) {
	string_t buffer;
	string_const_t stripped;
	hash_t section = 0;
	hash_t key = 0;
	unsigned int line = 0;
	size_t buffer_size;

#if BUILD_ENABLE_LOG || BUILD_ENABLE_CONFIG_DEBUG
	string_const_t path = stream_path(stream);
#endif

#if BUILD_ENABLE_CONFIG_DEBUG
	log_debugf(HASH_CONFIG, STRING_CONST("Parsing config stream: %*s"), STRING_FORMAT(path));
#endif
	buffer.length = buffer_size = 1024;
	buffer.str = memory_allocate(0, buffer.length, 0, MEMORY_TEMPORARY | MEMORY_ZERO_INITIALIZED);
	while (!stream_eos(stream)) {
		++line;
		buffer = stream_read_line_buffer(stream, buffer.str, buffer_size, '\n');
		stripped = string_strip(STRING_ARGS(buffer), STRING_CONST(" \t\n\r"));
		if (!stripped.length || (stripped.str[0] == ';') || (stripped.str[0] == '#'))
			continue;
		if (stripped.str[0] == '[') {
			//Section declaration
			size_t endpos = string_rfind(STRING_ARGS(stripped), ']', STRING_NPOS);
			if (endpos < 1) {
				log_warnf(HASH_CONFIG, WARNING_INVALID_VALUE,
				          STRING_CONST("Invalid section declaration on line %u in config stream '%*s'"), line,
				          STRING_FORMAT(path));
				continue;
			}
			section = hash(stripped.str + 1, endpos - 1);
#if BUILD_ENABLE_CONFIG_DEBUG
			log_debugf(HASH_CONFIG, STRING_CONST("  config: section set to '%*s' (0x%" PRIx64 ")"),
			           (int)endpos - 1, stripped.str + 1, section);
#endif
		}
		else if (!filter_section || (filter_section == section)) {
			//name=value declaration
			string_const_t name;
			string_const_t value;
			size_t separator = string_find(STRING_ARGS(stripped), '=', 0);
			if (separator == STRING_NPOS) {
				log_warnf(HASH_CONFIG, WARNING_INVALID_VALUE,
				          STRING_CONST("Invalid value declaration on line %u in config stream '%*s', missing assignment operator '=': %*s"),
				          line, STRING_FORMAT(path), STRING_FORMAT(stripped));
				continue;
			}

			name = string_strip(stripped.str, separator, STRING_CONST(" \t"));
			value = string_strip(stripped.str + separator + 1, stripped.length - separator - 1,
			                     STRING_CONST(" \t"));
			if (!name.length) {
				log_warnf(HASH_CONFIG, WARNING_INVALID_VALUE,
				          STRING_CONST("Invalid value declaration on line %d in config stream '%*s', empty name string: %*s"),
				          line, STRING_FORMAT(path), STRING_FORMAT(stripped));
				continue;
			}

			key = hash(STRING_ARGS(name));

			if (overwrite || !config_key(section, key, false)) {
#if BUILD_ENABLE_CONFIG_DEBUG
				log_debugf(HASH_CONFIG, STRING_CONST("  config: %*s (0x%llx) = %s"), name, key, value);
#endif

				if (!value.length)
					config_set_string(section, key, "", 0);
				else if (string_equal(STRING_ARGS(value), STRING_CONST("false")))
					config_set_bool(section, key, false);
				else if (string_equal(STRING_ARGS(value), STRING_CONST("true")))
					config_set_bool(section, key, true);
				else if ((string_find(STRING_ARGS(value), '.', 0) != STRING_NPOS) &&
				         (string_find_first_not_of(STRING_ARGS(value), STRING_CONST("0123456789."), 0) == STRING_NPOS) &&
				         (string_find(STRING_ARGS(value), '.', string_find(STRING_ARGS(value), '.', 0) + 1) == STRING_NPOS))
					config_set_real(section, key, string_to_real(STRING_ARGS(value))); //Exactly one "."
				else if (string_find_first_not_of(STRING_ARGS(value), STRING_CONST("0123456789"), 0) == STRING_NPOS)
					config_set_int(section, key, string_to_int64(STRING_ARGS(value)));
				else
					config_set_string(section, key, STRING_ARGS(value));
			}
		}
	}
	memory_deallocate(buffer.str);
}


void
config_parse_commandline(const string_const_t* cmdline, size_t num) {
	size_t arg;
	for (arg = 0; arg < num; ++arg) {
		if (string_match_pattern(STRING_ARGS(cmdline[arg]), STRING_CONST("--*:*=*"))) {
			size_t first_sep = string_find(STRING_ARGS(cmdline[arg]), ':', 0);
			size_t second_sep = string_find(STRING_ARGS(cmdline[arg]), '=', 0);
			if ((first_sep != STRING_NPOS) && (second_sep != STRING_NPOS) && (first_sep < second_sep)) {
				size_t section_length = first_sep - 2;
				size_t end_pos = first_sep + 1;
				size_t key_length = second_sep - end_pos;

				const char* section_str = cmdline[arg].str + 2;
				const char* key_str = pointer_offset_const(cmdline[arg].str, end_pos);

				hash_t section = hash(section_str, section_length);
				hash_t key = hash(key_str, key_length);

				string_const_t value = string_substr(STRING_ARGS(cmdline[arg]), second_sep + 1, STRING_NPOS);

				if (!value.length)
					config_set_string(section, key, "", 0);
				else if (string_equal(STRING_ARGS(value), STRING_CONST("false")))
					config_set_bool(section, key, false);
				else if (string_equal(STRING_ARGS(value), STRING_CONST("true")))
					config_set_bool(section, key, true);
				else if ((string_find(STRING_ARGS(value), '.', 0) != STRING_NPOS) &&
				         (string_find_first_not_of(STRING_ARGS(value), STRING_CONST("0123456789."), 0) == STRING_NPOS) &&
				         (string_find(STRING_ARGS(value), '.', string_find(STRING_ARGS(value), '.',
				                                                           0) + 1) == STRING_NPOS))            //Exactly one "."
					config_set_real(section, key, string_to_real(STRING_ARGS(value)));
				else if (string_find_first_not_of(STRING_ARGS(value), STRING_CONST("0123456789"), 0) == STRING_NPOS)
					config_set_int(section, key, string_to_int64(STRING_ARGS(value)));
				else {
					if ((value.length > 1) && (value.str[0] == '"') && (value.str[ value.length - 1 ] == '"')) {
						value.str++;
						value.length -= 2;
						config_set_string(section, key, STRING_ARGS(value));
					}
					else {
						config_set_string(section, key, STRING_ARGS(value));
					}
				}

				log_infof(HASH_CONFIG, STRING_CONST("Config value from command line: %*s:%*s = %*s"),
				          (int)section_length, section_str, (int)key_length, key_str, STRING_FORMAT(value));
			}
		}
	}
}


void
config_write(stream_t* stream, hash_t filter_section, string_const_t (*string_mapper)(hash_t)) {
	config_section_t* csection;
	config_key_t* bucket;
	size_t key, ib, bsize;

	stream_set_binary(stream, false);

	//TODO: If random access stream, update section if available, else append at end of stream
	//if( stream_is_sequential( stream ) )
	{
		string_const_t section = string_mapper(filter_section);
		stream_write_format(stream, STRING_CONST("[%*s]"), STRING_FORMAT(section));
		stream_write_endl(stream);

		csection = config_section(filter_section, false);
		if (csection) for (key = 0; key < CONFIG_KEY_BUCKETS; ++key) {
				bucket = csection->key[ key ];
				if (bucket) for (ib = 0, bsize = array_size(bucket); ib < bsize; ++ib) {
						string_const_t bucketstr = string_mapper(bucket[ib].name);
						stream_write_format(stream, STRING_CONST("\t%*s\t\t\t\t= "), STRING_FORMAT(bucketstr));
						switch (bucket[ib].type) {
						case CONFIGVALUE_BOOL:
							stream_write_bool(stream, bucket[ib].bval);
							break;

						case CONFIGVALUE_INT:
							stream_write_int64(stream, bucket[ib].ival);
							break;

						case CONFIGVALUE_REAL:
#if FOUNDATION_SIZE_REAL == 64
							stream_write_float64(stream, bucket[ib].rval);
#else
							stream_write_float32(stream, bucket[ib].rval);
#endif
							break;

						case CONFIGVALUE_STRING:
						case CONFIGVALUE_STRING_CONST:
						case CONFIGVALUE_STRING_VAR:
						case CONFIGVALUE_STRING_CONST_VAR:
							stream_write_string(stream, STRING_ARGS(bucket[ib].sval));
							break;
						}
						stream_write_endl(stream);
					}
			}
	}
}
