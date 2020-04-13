/* main.c  -  Foundation library tools  -  Public Domain  -  2013 Mattias Jansson
 *
 * This library provides a cross-platform foundation library in C11 providing basic support data types and
 * functions to write applications and games in a platform-independent fashion. The latest source code is
 * always available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#include <foundation/foundation.h>

#include "errorcodes.h"

typedef enum { METHOD_RANDOM = 0, METHOD_TIME, METHOD_NAMESPACE_MD5 } uuid_method_t;

typedef struct {
	uuid_method_t method;

	// For random and time based
	unsigned int num;

	// For name based
	uuid_t namespace;
	string_t name;
} uuid_instance_t;

typedef struct {
	bool display_help;
	bool output_lowercase;
	bool output_binary;
	string_t output_file;
	uuid_instance_t* generate;
} uuidgen_input_t;

typedef struct {
	uint32_t data1;
	uint16_t data2;
	uint16_t data3;
	uint8_t data4[8];
} uuid_raw_t;

typedef union {
	uuid_raw_t raw;
	uuid_t uuid;
} uuid_convert_t;

static uuidgen_input_t
uuidgen_parse_command_line(const string_const_t* cmdline);

static int
uuidgen_generate(uuid_t** uuid, const uuid_instance_t input);

static int
uuidgen_output(uuid_t* uuid, string_t output, bool binary, bool lowercase);

static void
uuidgen_print_usage(void);

int
main_initialize(void) {
	int ret = 0;
	application_t application;
	foundation_config_t config;

	memset(&config, 0, sizeof(config));

	memset(&application, 0, sizeof(application));
	application.name = string_const(STRING_CONST("uuidgen"));
	application.short_name = string_const(STRING_CONST("uuidgen"));
	application.flags = APPLICATION_UTILITY;

	log_enable_prefix(false);
	log_set_suppress(0, ERRORLEVEL_ERROR);

	if ((ret = foundation_initialize(memory_system_malloc(), application, config)) < 0)
		return ret;

	return 0;
}

int
main_run(void* main_arg) {
	int result = 0;
	size_t iinst, instance_count;
	uuid_t* output = 0;
	uuidgen_input_t input = uuidgen_parse_command_line(environment_command_line());
	FOUNDATION_UNUSED(main_arg);

	for (iinst = 0, instance_count = array_size(input.generate); iinst < instance_count; ++iinst) {
		result = uuidgen_generate(&output, input.generate[iinst]);
		if (result < 0)
			goto exit;
	}

	result = uuidgen_output(output, input.output_file, input.output_binary, input.output_lowercase);
	if (result < 0)
		goto exit;

exit:

	if (input.display_help)
		uuidgen_print_usage();

	for (iinst = 0, instance_count = array_size(input.generate); iinst < instance_count; ++iinst)
		string_deallocate(input.generate[iinst].name.str);
	array_deallocate(input.generate);
	array_deallocate(output);
	string_deallocate(input.output_file.str);

	return result;
}

void
main_finalize(void) {
	foundation_finalize();
}

uuidgen_input_t
uuidgen_parse_command_line(const string_const_t* cmdline) {
	uuidgen_input_t input;
	size_t arg, asize;

	memset(&input, 0, sizeof(input));

	error_context_push(STRING_CONST("parsing command line"), STRING_CONST(""));
	for (arg = 1, asize = array_size(cmdline); arg < asize; ++arg) {
		if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--help")))
			input.display_help = true;
		else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--output"))) {
			if (arg < asize - 1) {
				++arg;
				string_deallocate(input.output_file.str);
				input.output_file = string_clone(cmdline[arg].str, cmdline[arg].length);
			}
		} else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--binary"))) {
			input.output_binary = true;
		} else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--lowercase"))) {
			input.output_lowercase = true;
		} else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--uppercase"))) {
			input.output_lowercase = false;
		} else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--random"))) {
			uuid_instance_t instance;
			memset(&instance, 0, sizeof(instance));
			instance.method = METHOD_RANDOM;
			instance.num = 1;
			if ((arg < (asize - 1)) && (cmdline[arg + 1].str[0] != '-')) {
				++arg;
				instance.num = string_to_uint(STRING_ARGS(cmdline[arg]), false);
			}
			if (instance.num < 1)
				instance.num = 1;
			array_push_memcpy(input.generate, &instance);
		} else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--time"))) {
			uuid_instance_t instance;
			memset(&instance, 0, sizeof(instance));
			instance.method = METHOD_TIME;
			instance.num = 1;
			if ((arg < (asize - 1)) && (cmdline[arg + 1].str[0] != '-')) {
				++arg;
				instance.num = string_to_uint(STRING_ARGS(cmdline[arg]), false);
			}
			if (instance.num < 1)
				instance.num = 1;
			array_push_memcpy(input.generate, &instance);
		} else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--md5"))) {
			uuid_instance_t instance;
			memset(&instance, 0, sizeof(instance));
			instance.method = METHOD_NAMESPACE_MD5;
			instance.num = 1;
			if (arg < (asize - 1)) {
				++arg;
				if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("dns")) ||
				    string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("DNS")))
					instance.namespace = UUID_DNS;
				else
					instance.namespace = string_to_uuid(cmdline[arg].str, cmdline[arg].length);
			}
			if (arg < (asize - 1)) {
				++arg;
				instance.name = string_clone(cmdline[arg].str, cmdline[arg].length);
			}
			array_push_memcpy(input.generate, &instance);
		} else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--debug"))) {
			log_set_suppress(0, ERRORLEVEL_NONE);
		} else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--")))
			break;  // Stop parsing cmdline options
		else {
			// Unknown argument, display help
			input.display_help = true;
		}
	}
	error_context_pop();

	if (!array_size(input.generate) && !input.display_help) {
		// Default to one random-based UUID
		uuid_instance_t instance;
		memset(&instance, 0, sizeof(instance));
		instance.method = METHOD_RANDOM;
		instance.num = 1;
		array_push_memcpy(input.generate, &instance);
	}

	return input;
}

int
uuidgen_generate(uuid_t** uuid, const uuid_instance_t input) {
	int result = UUIDGEN_RESULT_OK;
	switch (input.method) {
		case METHOD_RANDOM: {
			size_t ii = 0;
			for (; ii < input.num; ++ii)
				array_push(*uuid, uuid_generate_random());
			break;
		}

		case METHOD_TIME: {
			size_t ii = 0;
			for (; ii < input.num; ++ii)
				array_push(*uuid, uuid_generate_time());
			break;
		}

		case METHOD_NAMESPACE_MD5: {
			array_push(*uuid, uuid_generate_name(input.namespace, input.name.str, input.name.length));
			break;
		}
	}

	return result;
}

int
uuidgen_output(uuid_t* uuid, string_t output, bool binary, bool lowercase) {
	if (output.length) {
		size_t i, uuidsize;
		stream_t* stream = stream_open(output.str, output.length, STREAM_OUT | (binary ? STREAM_BINARY : 0));
		if (!stream)
			return UUIDGEN_RESULT_UNABLE_TO_OPEN_OUTPUT_FILE;
		for (i = 0, uuidsize = array_size(uuid); i < uuidsize; ++i) {
			if (binary)
				stream_write(stream, uuid + i, sizeof(uuid[i]));
			else {
				uuid_convert_t convert;
				convert.uuid = uuid[i];
				stream_write_format(stream,
				                    STRING_CONST(lowercase ? "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x" :
				                                             "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
				                    convert.raw.data1, convert.raw.data2, convert.raw.data3, convert.raw.data4[0],
				                    convert.raw.data4[1], convert.raw.data4[2], convert.raw.data4[3],
				                    convert.raw.data4[4], convert.raw.data4[5], convert.raw.data4[6],
				                    convert.raw.data4[7]);
				stream_write_endl(stream);
			}
		}
		stream_deallocate(stream);
	} else {
		size_t i, uuidsize;
		log_set_suppress(0, ERRORLEVEL_DEBUG);
		for (i = 0, uuidsize = array_size(uuid); i < uuidsize; ++i) {
			uuid_convert_t convert;
			convert.uuid = uuid[i];
			log_infof(0,
			          STRING_CONST(lowercase ? "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x" :
			                                   "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
			          convert.raw.data1, convert.raw.data2, convert.raw.data3, convert.raw.data4[0],
			          convert.raw.data4[1], convert.raw.data4[2], convert.raw.data4[3], convert.raw.data4[4],
			          convert.raw.data4[5], convert.raw.data4[6], convert.raw.data4[7]);
		}
	}
	return UUIDGEN_RESULT_OK;
}

static void
uuidgen_print_usage(void) {
	const error_level_t saved_level = log_suppress(0);
	log_set_suppress(0, ERRORLEVEL_DEBUG);
	log_info(0, STRING_CONST(
	                "uuidgen usage:\n"
	                "  uuidgen [--time n] [--random n] [--md5 <namespace> <name>] [--output <filename>] [--debug] "
	                "[--help] [--]\n"
	                "    If no arguments are given, one random-based UUID is output to stdout\n"
	                "    Optional arguments:\n"
	                "      --time n                     Generate n time-based UUIDs\n"
	                "      --random n                   Generate n random-based UUIDs\n"
	                "      --md5 <namespace> <name>     Generate a name-based UUID using the namespace UUID specified\n"
	                "                                   in <namespace> and a name string specified in <name>\n"
	                "      --output <filename>          Output to <filename> instead of stdout\n"
	                "      --binary                     Output binary data instead of ASCII (stdout is always ASCII)\n"
	                "      --lowercase                  Output UUID in lowercase hex\n"
	                "      --uppercase                  Output UUID in uppercase hex (default)\n"
	                "      --debug                      Enable debug output\n"
	                "      --help                       Display this help message\n"
	                "      --                           Stop processing command line arguments"));
	log_set_suppress(0, saved_level);
}
