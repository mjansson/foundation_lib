/* main.c  -  Foundation bin2hex tool  -  Public Domain  -  2013 Mattias Jansson
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

typedef struct {
	string_t* input_files;
	string_t* output_files;
	size_t columns;
	bool display_help;
} bin2hex_input_t;

static bin2hex_input_t
bin2hex_parse_command_line(const string_const_t* cmdline);

static int
bin2hex_process_files(string_t* input, string_t* output, size_t columns);

static int
bin2hex_process_file(stream_t* input, stream_t* output, size_t columns);

static void
bin2hex_print_usage(void);

int
main_initialize(void) {
	int ret = 0;
	application_t application;
	foundation_config_t config;

	memset(&config, 0, sizeof(config));

	memset(&application, 0, sizeof(application));
	application.name = string_const(STRING_CONST("bin2hex"));
	application.short_name = string_const(STRING_CONST("bin2hex"));
	application.flags = APPLICATION_UTILITY;

	log_enable_prefix(false);
	log_set_suppress(0, ERRORLEVEL_ERROR);

	if ((ret = foundation_initialize(memory_system_malloc(), application, config)) < 0)
		return ret;

	return 0;
}

int
main_run(void* main_arg) {
	int result = BIN2HEX_RESULT_OK;
	bin2hex_input_t input = bin2hex_parse_command_line(environment_command_line());
	FOUNDATION_UNUSED(main_arg);

	if (input.display_help)
		bin2hex_print_usage();
	else {
		result = bin2hex_process_files(input.input_files, input.output_files, input.columns);
		if (result < 0)
			goto exit;
	}

exit:

	string_array_deallocate(input.input_files);
	string_array_deallocate(input.output_files);

	return result;
}

void
main_finalize(void) {
	foundation_finalize();
}

bin2hex_input_t
bin2hex_parse_command_line(const string_const_t* cmdline) {
	bin2hex_input_t input;
	size_t arg, asize;

	memset(&input, 0, sizeof(input));

	error_context_push(STRING_CONST("parsing command line"), STRING_CONST(""));
	for (arg = 1, asize = array_size(cmdline); arg < asize; ++arg) {
		if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--help"))) {
			input.display_help = true;
		} else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--columns"))) {
			if (arg < (asize - 1)) {
				++arg;
				input.columns = string_to_uint(cmdline[arg].str, cmdline[arg].length, false);
			}
		} else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--debug"))) {
			log_set_suppress(0, ERRORLEVEL_NONE);
		} else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--")))
			break;  // Stop parsing cmdline options
		else if ((cmdline[arg].length > 2) && string_equal(cmdline[arg].str, 2, STRING_CONST("--")))
			continue;  // Cmdline argument not parsed here
		else {
			array_push(input.input_files, string_clone(cmdline[arg].str, cmdline[arg].length));
			array_push(input.output_files,
			           string_allocate_format(STRING_CONST("%.*s.hex"), (int)cmdline[arg].length, cmdline[arg].str));
		}
	}
	error_context_pop();

	if (array_size(cmdline) <= 1)
		input.display_help = true;

	return input;
}

int
bin2hex_process_files(string_t* input, string_t* output, size_t columns) {
	int result = BIN2HEX_RESULT_OK;
	size_t ifile, files_size;
	for (ifile = 0, files_size = array_size(input); (result == BIN2HEX_RESULT_OK) && (ifile < files_size); ++ifile) {
		string_t input_filename;
		string_t output_filename;

		stream_t* input_file = 0;
		stream_t* output_file = 0;

		input_filename = string_allocate(0, BUILD_MAX_PATHLEN);
		input_filename = string_copy(input_filename.str, BUILD_MAX_PATHLEN, STRING_ARGS(input[ifile]));
		input_filename = path_clean(STRING_ARGS(input_filename), BUILD_MAX_PATHLEN);
		error_context_push(STRING_CONST("parsing file"), STRING_ARGS(input_filename));

		output_filename = string_allocate(0, BUILD_MAX_PATHLEN);
		output_filename = string_copy(output_filename.str, BUILD_MAX_PATHLEN, STRING_ARGS(output[ifile]));
		output_filename = path_clean(STRING_ARGS(output_filename), BUILD_MAX_PATHLEN);

		log_infof(0, STRING_CONST("bin2hex %.*s -> %.*s"), STRING_FORMAT(input_filename),
		          STRING_FORMAT(output_filename));

		input_file = stream_open(STRING_ARGS(input_filename), STREAM_IN | STREAM_BINARY);

		if (!input_file) {
			log_warnf(0, WARNING_INVALID_VALUE, STRING_CONST("Unable to open input file: %.*s"),
			          STRING_FORMAT(input_filename));
			result = BIN2HEX_RESULT_MISSING_INPUT_FILE;
		} else {
			output_file = stream_open(STRING_ARGS(output_filename), STREAM_OUT);
			if (!output_file) {
				log_warnf(0, WARNING_INVALID_VALUE, STRING_CONST("Unable to open output file: %.*s"),
				          STRING_FORMAT(output_filename));
				result = BIN2HEX_RESULT_UNABLE_TO_OPEN_OUTPUT_FILE;
			}
		}

		if (input_file && output_file)
			result = bin2hex_process_file(input_file, output_file, columns);

		stream_deallocate(input_file);
		stream_deallocate(output_file);

		string_deallocate(output_filename.str);

		error_context_pop();
		string_deallocate(input_filename.str);
	}

	if ((result == BIN2HEX_RESULT_OK) && (files_size > 0))
		log_info(0, STRING_CONST("All files generated"));

	return result;
}

int
bin2hex_process_file(stream_t* input, stream_t* output, size_t columns) {
	uint8_t block[512];
	size_t read, byte;

	if (!columns)
		columns = 32;
	if (columns > 512)
		columns = 512;

	while (!stream_eos(input)) {
		read = stream_read(input, block, columns);
		if (!read)
			break;

		for (byte = 0; byte < read; ++byte)
			stream_write_format(output, STRING_CONST("0x%02x, "), (unsigned int)block[byte]);
		stream_write_endl(output);
	}
	return BIN2HEX_RESULT_OK;
}

void
bin2hex_print_usage(void) {
	const error_level_t saved_level = log_suppress(0);
	log_set_suppress(0, ERRORLEVEL_DEBUG);
	log_info(0, STRING_CONST("bin2hex usage:\n"
	                         "  bin2hex [--columns n] [--debug] [--help] <file> <file> <file> <...> [--]\n"
	                         "    Required arguments:\n"
	                         "      <file>                       Input filename (any number of input files allowed). "
	                         "Output will be named \"<file>.hex\"\n"
	                         "    Optional arguments:\n"
	                         "      --columns n                  Print n bytes in each column (default is 32)\n"
	                         "      --debug                      Enable debug output\n"
	                         "      --help                       Display this help message\n"
	                         "      --                           Stop processing command line arguments"));
	log_set_suppress(0, saved_level);
}
