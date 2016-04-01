/* config.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

config_node_t*
config_allocate(void) {
	config_node_t* node = memory_allocate(0, sizeof(config_node_t), 0, MEMORY_PERSISTENT);
	config_initialize(node);
	return node;
}

void
config_initialize(config_node_t* node) {
	memset(node, 0, sizeof(config_node_t));
}

void
config_finalize(config_node_t* node) {
	if (node->expanded.str != node->sval.str)
		string_deallocate(node->expanded.str);
	if ((node->type != CONFIGVALUE_STRING_CONST) && (node->type != CONFIGVALUE_STRING_CONST_VAR))
		string_deallocate(node->sval.str);
	if (node && (node->type == CONFIGVALUE_NODE)) {
		size_t inode, nsize;
		for (inode = 0, nsize = array_size(node->nodes); inode < nsize; ++inode)
			config_finalize(node->nodes + inode);
		array_deallocate(node->nodes);
	}
	node->expanded = (string_t) {0, 0};
	node->sval = (string_t) {0, 0};
	node->nodes = nullptr;
}

void
config_deallocate(config_node_t* node) {
	config_finalize(node);
	memory_deallocate(node);
}

static config_node_t*
_config_subnode(config_node_t* node, hash_t key, bool create) {
	size_t inode, nsize;
	if (node->type == CONFIGVALUE_NODE) {
		for (inode = 0, nsize = array_size(node->nodes); inode < nsize; ++inode) {
			if (node->nodes[inode].name == key)
				return node->nodes + inode;
		}
	}
	if (create) {
		config_node_t newnode;
		if (node->type != CONFIGVALUE_NODE) {
			node->type = CONFIGVALUE_NODE;
			node->nodes = nullptr;
		}
		memset(&newnode, 0, sizeof(newnode));
		newnode.name = key;
		array_push(node->nodes, newnode);
		return node->nodes + (array_size(node->nodes) - 1);
	}
	return nullptr;
}

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
	else if (key == HASH_APPLICATION_DIRECTORY)
		return environment_application_directory();
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
_expand_string(config_node_t* root, config_node_t* parent, string_t str) {
	string_const_t variable, value;
	size_t var_pos, var_end_pos, separator, var_offset, capacity, newlength;
	string_t expanded;
	hash_t node_key, key;

	capacity = 0;
	newlength = str.length + 1;
	expanded = str;
	var_pos = string_find_string(STRING_ARGS(expanded), STRING_CONST("$("), 0);

	while (var_pos != STRING_NPOS) {
		var_end_pos = string_find(STRING_ARGS(expanded), ')', var_pos + 2);
		FOUNDATION_ASSERT_MSG(var_end_pos != STRING_NPOS, "Malformed config variable statement");
		variable = string_substr(STRING_ARGS(expanded), var_pos,
		                         (var_end_pos != STRING_NPOS) ? (1 + var_end_pos - var_pos) : STRING_NPOS);

		separator = string_find(STRING_ARGS(variable), ':', 0);
		config_node_t* node = root;
		if (separator != STRING_NPOS) {
			size_t start = 2;
			size_t subpath = 0;
			do {
				string_const_t keystr = string_substr(STRING_ARGS(variable), start, separator - start);
				start = separator + 1;
				separator = string_find(STRING_ARGS(variable), ':', start);
				if (!keystr.length)
					continue;
				node_key = hash(STRING_ARGS(keystr));
				if (!subpath && (node_key == HASH_ENVIRONMENT))
					break;

				node = _config_subnode(node, node_key, false);
				node_key = 0;
				++subpath;
			}
			while (separator != STRING_NPOS);

			var_offset = start;
		}
		else {
			var_offset = 2;
			node = parent;
			node_key = 0;
		}

		string_const_t varstr = string_substr(STRING_ARGS(variable), var_offset,
		                                      variable.length - (var_offset + (variable.str[variable.length - 1] == ')' ? 1 : 0)));
		key = hash(STRING_ARGS(varstr));
		if (node_key == HASH_ENVIRONMENT)
			value = _expand_environment(key, varstr);
		else
			value = config_string(node, key, HASH_NULL);

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
		log_debugf(HASH_CONFIG, STRING_CONST("Expanded config value \"%.*s\" to \"%.*s\""),
		           STRING_FORMAT(str), STRING_FORMAT(expanded));
#endif

	return expanded;
}

static FOUNDATION_NOINLINE void
_expand_string_val(config_node_t* root, config_node_t* parent, config_node_t* node) {
	bool is_true;
	FOUNDATION_ASSERT(node->sval.str);
	if (node->expanded.str != node->sval.str)
		string_deallocate(node->expanded.str);
	node->expanded = _expand_string(root, parent, node->sval);

	is_true = string_equal(STRING_ARGS(node->expanded), STRING_CONST("true"));
	node->bval = (string_equal(STRING_ARGS(node->expanded), STRING_CONST("false")) ||
	              string_equal(STRING_ARGS(node->expanded), STRING_CONST("0")) ||
	              !node->expanded.length) ? false : true;
	node->ival = is_true ? 1 : _config_string_to_int(string_to_const(node->expanded));
	node->rval = is_true ? REAL_C(1.0) : _config_string_to_real(string_to_const(node->expanded));
}

static const string_const_t platformsuffix =
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
	string_const_t archsuffix =
#if FOUNDATION_ARCH_ARM8_64
	(string_const_t) { STRING_CONST("/arm64") };
#elif FOUNDATION_ARCH_ARM_64
	(string_const_t) { STRING_CONST("/arm64") };
#elif FOUNDATION_ARCH_ARM5
	(string_const_t) { STRING_CONST("/arm5") };
#elif FOUNDATION_ARCH_ARM6
	(string_const_t) { STRING_CONST("/arm6") };
#elif FOUNDATION_ARCH_ARM7
	(string_const_t) { STRING_CONST("/arm7") };
#elif FOUNDATION_ARCH_ARM8
	(string_const_t) { STRING_CONST("/arm8") };
#elif FOUNDATION_ARCH_X86_64
	(string_const_t) { STRING_CONST("/x86-64") };
#elif FOUNDATION_ARCH_X86
	(string_const_t) { STRING_CONST("/x86") };
#elif FOUNDATION_ARCH_PPC_64
	(string_const_t) { STRING_CONST("/ppc64") };
#elif FOUNDATION_ARCH_PPC
	(string_const_t) { STRING_CONST("/ppc") };
#elif FOUNDATION_ARCH_IA64
	(string_const_t) { STRING_CONST("/ia64") };
#elif FOUNDATION_ARCH_MIPS_64
	(string_const_t) { STRING_CONST("/mips64") };
#elif FOUNDATION_ARCH_MIPS
	(string_const_t) { STRING_CONST("/mips") };
#else
	(string_const_t) { STRING_CONST("/generic") };
#endif
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

	if (string_ends_with(STRING_ARGS(path), STRING_ARGS(archsuffix))) {
		path.length = path.length - archsuffix.length;
		path.str[ path.length ] = 0;
	}
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
		env_dir = environment_application_directory();
		return string_concat_varg(buffer, capacity, STRING_ARGS(env_dir),
		                          STRING_CONST("/."), STRING_ARGS(environment_application()->config_dir), nullptr);
#else
		break;
#endif

	default:
		break;
	}
	return (string_t) { 0, 0 };
}

FOUNDATION_NOINLINE void
config_load(config_node_t* root, const char* name, size_t length,
            bool built_in, bool overwrite) {
	char buffer[BUILD_MAX_PATHLEN];
	string_t pathname;
	string_t filename;
	stream_t* istream;
	int start_path = 0;
	int end_path = 6;
	int ipath;

	if (!built_in) {
#if FOUNDATION_PLATFORM_WINDOWS || FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_MACOSX || FOUNDATION_PLATFORM_BSD
		start_path = 6;
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
		                         STRING_CONST("/%.*s.ini"), (int)length, name);
		filename.str = pathname.str;
		filename.length += pathname.length;
		istream = stream_open(STRING_ARGS(filename), STREAM_IN);
		if (istream) {
			config_parse(root, istream, overwrite);
			stream_deallocate(istream);
		}

		if (built_in) {
			filename = string_format(pathname.str + pathname.length, sizeof(buffer) - pathname.length,
			                         STRING_CONST("%.*s/%.*s.ini"), STRING_FORMAT(platformsuffix), (int)length, name);
			filename.str = pathname.str;
			filename.length += pathname.length;
			istream = stream_open(STRING_ARGS(filename), STREAM_IN);
			if (istream) {
				config_parse(root, istream, overwrite);
				stream_deallocate(istream);
			}
		}
	}
}

static FOUNDATION_NOINLINE config_node_t*
_config_resolve_node(config_node_t** root, va_list path, bool create) {
	hash_t key;
	config_node_t* node = *root;
	va_list cpath;
	/*lint -e{838} */
	va_copy(cpath, path);
	key = va_arg(cpath, hash_t);
	while (node && key) {
		*root = node;
		node = _config_subnode(*root, key, create);
		key = va_arg(cpath, hash_t);
	}
	va_end(cpath);

	return node;
}

#define RESOLVE_NODE(arg, create) \
	config_node_t* parent = root; \
	config_node_t* node; \
	va_list list; \
	va_start(list, arg); \
	node = _config_resolve_node(&parent, list, create); \
	va_end(list)

bool
config_bool(config_node_t* root, ...) {
	RESOLVE_NODE(root, false);
	if (node && (node->type >= CONFIGVALUE_STRING_VAR))
		_expand_string_val(root, parent, node);
	return node ? node->bval : false;
}

int64_t
config_int(config_node_t* root, ...) {
	RESOLVE_NODE(root, false);
	if (node && (node->type >= CONFIGVALUE_STRING_VAR))
		_expand_string_val(root, parent, node);
	return node ? node->ival : 0;
}

real
config_real(config_node_t* root, ...) {
	RESOLVE_NODE(root, false);
	if (node && (node->type >= CONFIGVALUE_STRING_VAR))
		_expand_string_val(root, parent, node);
	return node ? node->rval : 0;
}

static string_const_t
_config_string(config_node_t* root, config_node_t* parent, config_node_t* node) {
	//Convert to string
	/*lint --e{788} We use default for remaining enums */
	switch (node->type) {
	case CONFIGVALUE_NODE:
		return string_null();

	case CONFIGVALUE_BOOL:
		if (node->bval)
			return (string_const_t) {STRING_CONST("true")};
		return (string_const_t) {STRING_CONST("false")};

	case CONFIGVALUE_INT:
		if (!node->sval.str)
			node->sval = string_clone_string(string_from_int_static(node->ival, 0, 0));
		return string_to_const(node->sval);

	case CONFIGVALUE_REAL:
		if (!node->sval.str)
			node->sval = string_clone_string(string_from_real_static(node->rval, 4, 0, '0'));
		return string_to_const(node->sval);

	case CONFIGVALUE_STRING:
	case CONFIGVALUE_STRING_CONST:
	case CONFIGVALUE_STRING_VAR:
	case CONFIGVALUE_STRING_CONST_VAR:
		break;
	}
	//String value of some form
	if (!node->sval.str)
		return string_const("", 0);
	if (node->type >= CONFIGVALUE_STRING_VAR) {
		_expand_string_val(root, parent, node);
		return string_to_const(node->expanded);
	}
	return string_to_const(node->sval);
}

string_const_t
config_string(config_node_t* root, ...) {
	RESOLVE_NODE(root, false);
	if (!node)
		return string_const("", 0);
	return _config_string(root, parent, node);
}

hash_t
config_hash(config_node_t* root, ...) {
	string_const_t value;
	RESOLVE_NODE(root, false);
	if (!node)
		return HASH_EMPTY_STRING;
	value = _config_string(root, parent, node);
	return value.length ? hash(STRING_ARGS(value)) : HASH_EMPTY_STRING;
}

config_node_t*
config_node(config_node_t* root, ...) {
	RESOLVE_NODE(root, false);
	return node;
}

void
config_set_bool(config_node_t* root, bool value, ...) {
	RESOLVE_NODE(value, true);
	if (!FOUNDATION_VALIDATE(node)) return;
	config_finalize(node);
	node->bval = value;
	node->ival = (value ? 1 : 0);
	node->rval = (value ? REAL_C(1.0) : REAL_C(0.0));
	node->type = CONFIGVALUE_BOOL;
}

void
config_set_int(config_node_t* root, int64_t value, ...) {
	RESOLVE_NODE(value, true);
	if (!FOUNDATION_VALIDATE(node)) return;
	config_finalize(node);
	node->bval = value ? true : false;
	node->ival = value;
	node->rval = (real)value;
	node->type = CONFIGVALUE_INT;
}

void
config_set_real(config_node_t* root, real value, ...) {
	RESOLVE_NODE(value, true);
	if (!FOUNDATION_VALIDATE(node)) return;
	config_finalize(node);
	node->bval = !math_real_is_zero(value);
	node->ival = (int64_t)value;
	node->rval = value;
	node->type = CONFIGVALUE_REAL;
}

void
config_set_string(config_node_t* root, const char* value, size_t length, ...) {
	RESOLVE_NODE(length, true);
	if (!FOUNDATION_VALIDATE(node)) return;
	config_finalize(node);

	node->sval = string_clone(value, length);
	node->type = ((string_find_string(node->sval.str, node->sval.length,
	                                  STRING_CONST("$("), 0) != STRING_NPOS) ?
	              CONFIGVALUE_STRING_VAR : CONFIGVALUE_STRING);

	if (node->type == CONFIGVALUE_STRING) {
		bool is_true = string_equal(STRING_ARGS(node->sval), STRING_CONST("true"));
		node->bval = (string_equal(STRING_ARGS(node->sval), STRING_CONST("false")) ||
		              string_equal(STRING_ARGS(node->sval), STRING_CONST("0")) ||
		              !node->sval.length) ? false : true;
		node->ival = is_true ? 1 : _config_string_to_int(string_to_const(node->sval));
		node->rval = is_true ? REAL_C(1.0) : _config_string_to_real(string_to_const(node->sval));
	}
}

void
config_set_string_constant(config_node_t* root, const char* value, size_t length, ...) {
	RESOLVE_NODE(length, true);
	if (!FOUNDATION_VALIDATE(node)) return;
	config_finalize(node);

	//node->sval = (char*)value;
	//Yeah yeah, we're storing a const pointer in a non-const var
	memcpy(&node->sval.str, &value, sizeof(char*));
	node->sval.length = length;
	node->type = ((string_find_string(STRING_ARGS(node->sval),
	                                  STRING_CONST("$("), 0) != STRING_NPOS) ?
	              CONFIGVALUE_STRING_CONST_VAR : CONFIGVALUE_STRING_CONST);

	if (node->type == CONFIGVALUE_STRING_CONST) {
		bool is_true = string_equal(STRING_ARGS(node->sval), STRING_CONST("true"));
		node->bval = (string_equal(STRING_ARGS(node->sval), STRING_CONST("false")) ||
		              string_equal(STRING_ARGS(node->sval), STRING_CONST("0")) ||
		              !node->sval.length) ? false : true;
		node->ival = is_true ? 1 : _config_string_to_int(string_to_const(node->sval));
		node->rval = is_true ? REAL_C(1.0) : _config_string_to_real(string_to_const(node->sval));
	}
}

static void
_config_parse_token(config_node_t* node, json_token_t* tokens, unsigned int current, char* buffer, bool overwrite) {
	do {
		json_token_t* token = tokens + current;
		config_node_t* subnode;
		string_const_t identifier = json_token_identifier(buffer, tokens + current);
		string_const_t value = json_token_value(buffer, tokens + current);
		hash_t id = hash(STRING_ARGS(identifier));
		switch (token->type) {
		case JSON_OBJECT:
			subnode = config_node(node, id, HASH_NULL);
			if (!subnode && overwrite) {
				config_set_bool(node, false, id, HASH_NULL);
				subnode = config_node(node, id, HASH_NULL);
			}
			if (subnode && token->child)
				_config_parse_token(subnode, tokens, token->child, buffer, overwrite);
			break;
		case JSON_ARRAY:
			break;
		case JSON_PRIMITIVE:
			if (overwrite || !config_node(node, id, HASH_NULL)) {
				if (value.length && (value.str[0] == 't'))
					config_set_bool(node, true, id, HASH_NULL);
				else if (value.length && (value.str[0] == 'f'))
					config_set_bool(node, false, id, HASH_NULL);
				else if (string_find(STRING_ARGS(value), '.', 0) == STRING_NPOS)
					config_set_int(node, string_to_int64(STRING_ARGS(value)), id, HASH_NULL);
				else
					config_set_real(node, string_to_real(STRING_ARGS(value)), id, HASH_NULL);
			}
			break;
		case JSON_STRING:
			if (overwrite || !config_node(node, id, HASH_NULL))
				config_set_string(node, STRING_ARGS(value), id, HASH_NULL);
			break;
		}
		current = token->sibling;
	} while (current);
}

bool
config_parse(config_node_t* root, stream_t* stream, bool overwrite) {
	size_t size = stream_size(stream);
	char* buffer = memory_allocate(0, size, 0, MEMORY_PERSISTENT);
	json_token_t store[64];
	size_t capacity = sizeof(store) / sizeof(store[0]);
	json_token_t* tokens = store;
	size_t num;
	stream_read(stream, buffer, size);
	num = sjson_parse(buffer, size, tokens, capacity);
	if (num > capacity) {
		tokens = memory_allocate(0, sizeof(json_token_t) * num, 0, MEMORY_PERSISTENT);
		num = sjson_parse(buffer, size, tokens, capacity);
	}
	if (num && (tokens[0].type == JSON_OBJECT))
		_config_parse_token(root, tokens, 1, buffer, overwrite);
	if (store != tokens)
		memory_deallocate(tokens);
	memory_deallocate(buffer);
	return num > 0;
}

void
config_parse_commandline(config_node_t* root, const string_const_t* cmdline, size_t num) {
	size_t arg, var_offset;
	hash_t key;
	for (arg = 0; arg < num; ++arg) {
		if ((cmdline[arg].length > 4) && (cmdline[arg].str[0] == '-') && (cmdline[arg].str[1] == '-')) {
			size_t eq_sep = string_find(STRING_ARGS(cmdline[arg]), '=', 0);
			if (eq_sep == STRING_NPOS)
				continue;
			string_const_t variable = string_substr(STRING_ARGS(cmdline[arg]), 2, eq_sep - 2);
			size_t separator = string_find(STRING_ARGS(variable), ':', 0);
			config_node_t* node = root;
			string_const_t varstr, value;
			if (separator != STRING_NPOS) {
				size_t start = 0;
				do {
					string_const_t keystr = string_substr(STRING_ARGS(variable), start, separator - start);
					start = separator + 1;
					separator = string_find(STRING_ARGS(variable), ':', start);
					if (!keystr.length)
						continue;
					key = hash(STRING_ARGS(keystr));
					node = _config_subnode(node, key, true);
				}
				while (separator != STRING_NPOS);

				var_offset = start;
			}
			else {
				var_offset = 0;
				node = root;
			}

			varstr = string_substr(STRING_ARGS(variable), var_offset, variable.length - var_offset);
			key = hash(STRING_ARGS(varstr));

			value = string_substr(STRING_ARGS(cmdline[arg]), eq_sep + 1, STRING_NPOS);

			if (!value.length)
				config_set_string(node, "", 0, key, HASH_NULL);
			else if (string_equal(STRING_ARGS(value), STRING_CONST("false")))
				config_set_bool(node, false, key, HASH_NULL);
			else if (string_equal(STRING_ARGS(value), STRING_CONST("true")))
				config_set_bool(node, true, key, HASH_NULL);
			else if ((string_find(STRING_ARGS(value), '.', 0) != STRING_NPOS) &&
			         (string_find_first_not_of(STRING_ARGS(value), STRING_CONST("0123456789."), 0) == STRING_NPOS) &&
			         (string_find(STRING_ARGS(value), '.', string_find(STRING_ARGS(value), '.', 0) + 1) == STRING_NPOS))
				config_set_real(node, string_to_real(STRING_ARGS(value)), key, HASH_NULL);
			else if (string_find_first_not_of(STRING_ARGS(value), STRING_CONST("0123456789"), 0) == STRING_NPOS)
				config_set_int(node, string_to_int64(STRING_ARGS(value)), key, HASH_NULL);
			else {
				if ((value.length > 1) && (value.str[0] == '"') && (value.str[ value.length - 1 ] == '"')) {
					value.str++;
					value.length -= 2;
					config_set_string(node, STRING_ARGS(value), key, HASH_NULL);
				}
				else {
					config_set_string(node, STRING_ARGS(value), key, HASH_NULL);
				}
			}

			log_infof(HASH_CONFIG, STRING_CONST("Config value from command line: %.*s = %.*s"),
			          STRING_FORMAT(variable), STRING_FORMAT(value));
		}
	}
}

void
config_write(config_node_t* root, stream_t* stream, string_const_t (*map)(hash_t)) {
	size_t inode, nsize;

	if (!root || (root->type != CONFIGVALUE_NODE))
		return;

	stream_set_binary(stream, false);

	for (inode = 0, nsize = array_size(root->nodes); inode < nsize; ++inode) {
		config_node_t* node = root->nodes + inode;
		string_const_t name = map(node->name);

		stream_write_format(stream, STRING_CONST("%.*s = "), STRING_FORMAT(name));
		switch (node->type) {
		case CONFIGVALUE_BOOL:
			stream_write_bool(stream, node->bval);
			break;

		case CONFIGVALUE_INT:
			stream_write_int64(stream, node->ival);
			break;

		case CONFIGVALUE_REAL:
#if FOUNDATION_SIZE_REAL == 8
			stream_write_float64(stream, node->rval);
#else
			stream_write_float32(stream, node->rval);
#endif
			break;

		case CONFIGVALUE_STRING:
		case CONFIGVALUE_STRING_CONST:
		case CONFIGVALUE_STRING_VAR:
		case CONFIGVALUE_STRING_CONST_VAR:
			if (string_find_first_of(STRING_ARGS(node->sval), 
				STRING_CONST(STRING_WHITESPACE "=:[]{}\""), 0) != STRING_NPOS) {
				if (string_find(STRING_ARGS(node->sval), '"', 0) != STRING_NPOS) {

				}
				else {
					stream_write(stream, "\"", 1);
					stream_write_string(stream, STRING_ARGS(node->sval));
					stream_write(stream, "\"", 1);
				}
			}
			else {
				stream_write_string(stream, STRING_ARGS(node->sval));
			}
			break;

		case CONFIGVALUE_NODE:
			stream_write_string(stream, STRING_CONST("{"));
			stream_write_endl(stream);
			config_write(node, stream, map);
			stream_write_string(stream, STRING_CONST("}"));
			break;
		}
		stream_write_endl(stream);
	}
}
