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

// Use fixed-size buffers since the data format is very restricted
#define HASHIFY_LINEBUFFER_LENGTH 512

typedef struct {
	bool check_only;
	string_t* strings;
	string_t* files;
} hashify_input_t;

typedef struct {
	string_t string;
	hash_t hash;
} hashify_string_t;

static hashify_string_t* all_hashes;

static hashify_input_t
hashify_parse_command_line(const string_const_t* cmdline);

static void
hashify_print_usage(void);

static int
hashify_process_strings(string_t* strings);

static int
hashify_process_files(string_t* files, bool check_only);

static int
hashify_process_file(stream_t* input_file, stream_t* output_file, string_t output_filename, bool check_only,
                     hashify_string_t** history);

static int
hashify_generate_preamble(stream_t* output_file, string_t output_filename);

static int
hashify_read_hashes(stream_t* file, hashify_string_t** hashes);

static int
hashify_write_file(stream_t* generated_file, string_t output_filename);

static int
hashify_check_local_consistency(string_const_t string, hash_t hash_value, const hashify_string_t* local_hashes);

static int
hashify_check_collisions(string_const_t string, hash_t hash_value, const hashify_string_t* history);

static int
hashify_check_match(const hashify_string_t* hashes, const hashify_string_t* generated);

int
main_initialize(void) {
	int ret = 0;
	application_t application;
	foundation_config_t config;

	memset(&config, 0, sizeof(config));

	memset(&application, 0, sizeof(application));
	application.name = string_const(STRING_CONST("hashify"));
	application.short_name = string_const(STRING_CONST("hashify"));
	application.flags = APPLICATION_UTILITY;

	log_enable_prefix(false);
	log_set_suppress(0, ERRORLEVEL_WARNING);

	if ((ret = foundation_initialize(memory_system_malloc(), application, config)) < 0)
		return ret;

	return 0;
}

int
main_run(void* main_arg) {
	int result = 0;
	hashify_input_t input = hashify_parse_command_line(environment_command_line());
	FOUNDATION_UNUSED(main_arg);

	result = hashify_process_strings(input.strings);
	if (result < 0)
		goto exit;

	result = hashify_process_files(input.files, input.check_only);
	if (result < 0)
		goto exit;

exit:

	for (unsigned int ihash = 0, hsize = array_size(all_hashes); ihash < hsize; ++ihash)
		string_deallocate(all_hashes[ihash].string.str);
	array_deallocate(all_hashes);

	string_array_deallocate(input.strings);
	string_array_deallocate(input.files);

	return result;
}

void
main_finalize(void) {
	foundation_finalize();
}

hashify_input_t
hashify_parse_command_line(const string_const_t* cmdline) {
	hashify_input_t input;
	size_t arg, asize;

	memset(&input, 0, sizeof(input));
	input.check_only = false;

	error_context_push(STRING_CONST("parsing command line"), STRING_CONST(""));
	for (arg = 1, asize = array_size(cmdline); arg < asize; ++arg) {
		if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--help"))) {
			hashify_print_usage();
			continue;
		} else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--validate"))) {
			input.check_only = true;
			continue;
		} else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--generate-string"))) {
			if (arg < asize - 1) {
				++arg;
				array_push(input.strings, string_clone(STRING_ARGS(cmdline[arg])));
			}
			continue;
		} else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--debug"))) {
			log_set_suppress(0, ERRORLEVEL_NONE);
			continue;
		} else if (string_equal(STRING_ARGS(cmdline[arg]), STRING_CONST("--")))
			break;  // Stop parsing cmdline options
		else if ((cmdline[arg].length > 2) && string_equal(cmdline[arg].str, 2, STRING_CONST("--")))
			continue;  // Cmdline argument not parsed here

		array_push(input.files, string_clone(STRING_ARGS(cmdline[arg])));
	}
	error_context_pop();

	if (array_size(cmdline) <= 1)
		hashify_print_usage();

	return input;
}

int
hashify_process_strings(string_t* strings) {
	size_t istr, strings_size;
	for (istr = 0, strings_size = array_size(strings); istr < strings_size; ++istr) {
		uint64_t hash_value = hash(STRING_ARGS(strings[istr]));
		log_infof(0, STRING_CONST("String '%.*s' hash: 0x%" PRIx64), STRING_FORMAT(strings[istr]), hash_value);
	}
	return 0;
}

int
hashify_process_files(string_t* files, bool check_only) {
	int result = HASHIFY_RESULT_OK;
	hashify_string_t* history = 0;
	size_t ifile, files_size;
	for (ifile = 0, files_size = array_size(files); (result == HASHIFY_RESULT_OK) && (ifile < files_size); ++ifile) {
		string_t input_filename;
		string_t output_filename;
		string_const_t base_filename;

		stream_t* input_file;
		stream_t* output_file;

		input_filename = string_allocate(0, BUILD_MAX_PATHLEN);
		input_filename = string_copy(input_filename.str, BUILD_MAX_PATHLEN, STRING_ARGS(files[ifile]));
		input_filename = path_clean(STRING_ARGS(input_filename), BUILD_MAX_PATHLEN);
		error_context_push(STRING_CONST("parsing file"), STRING_ARGS(input_filename));

		base_filename = path_base_file_name_with_directory(STRING_ARGS(input_filename));
		output_filename = string_allocate_format(STRING_CONST("%.*s.h"), STRING_FORMAT(base_filename));

		log_infof(0, STRING_CONST("Hashifying %.*s -> %.*s"), STRING_FORMAT(input_filename),
		          STRING_FORMAT(output_filename));

		input_file = stream_open(STRING_ARGS(input_filename), STREAM_IN);

		// If only validating, open the final output file. If generating, make a memory buffer as intermediate storage
		if (check_only)
			output_file = stream_open(STRING_ARGS(output_filename), STREAM_IN);
		else
			output_file = buffer_stream_allocate(memory_allocate(0, 65536, 0, MEMORY_PERSISTENT),
			                                     STREAM_IN | STREAM_OUT, 0, 65536, true, true);

		if (!input_file) {
			log_warnf(0, WARNING_INVALID_VALUE, STRING_CONST("Unable to open input file: %.*s"),
			          STRING_FORMAT(input_filename));
			result = HASHIFY_RESULT_MISSING_INPUT_FILE;
		} else if (!output_file) {
			log_warnf(0, WARNING_INVALID_VALUE, STRING_CONST("Unable to open output file: %.*s"),
			          STRING_FORMAT(output_filename));
			result = HASHIFY_RESULT_MISSING_OUTPUT_FILE;
		}

		if (input_file && output_file) {
			result = hashify_process_file(input_file, output_file, output_filename, check_only, &history);
			if ((result == HASHIFY_RESULT_OK) && !check_only)
				result = hashify_write_file(output_file, output_filename);
		}

		stream_deallocate(input_file);
		stream_deallocate(output_file);

		error_context_pop();

		string_deallocate(input_filename.str);
		string_deallocate(output_filename.str);
	}

	if ((result == HASHIFY_RESULT_OK) && (files_size > 0)) {
		if (check_only)
			log_info(0, STRING_CONST("All hashes validated"));
		else
			log_info(0, STRING_CONST("All hashes generated"));
	}

	array_deallocate(history);

	return result;
}

int
hashify_process_file(stream_t* input_file, stream_t* output_file, string_t output_filename, bool check_only,
                     hashify_string_t** history) {
	int result = HASHIFY_RESULT_OK;
	char line_buffer[HASHIFY_LINEBUFFER_LENGTH];
	hashify_string_t* local_hashes = 0;
	hashify_string_t* local_generated = 0;

	if (check_only)
		result = hashify_read_hashes(output_file, &local_hashes);
	else
		result = hashify_generate_preamble(output_file, output_filename);

	memset(line_buffer, 0, sizeof(line_buffer));
	while (!stream_eos(input_file) && (result == HASHIFY_RESULT_OK)) {
		string_t line_string;
		string_const_t def_string;
		string_const_t value_string;

		line_string = stream_read_line_buffer(input_file, line_buffer, sizeof(line_buffer), '\n');
		string_split(STRING_ARGS(line_string), STRING_CONST(" \t"), &def_string, &value_string, false);

		def_string = string_strip(STRING_ARGS(def_string), STRING_CONST(STRING_WHITESPACE));
		value_string = string_strip(STRING_ARGS(value_string), STRING_CONST(STRING_WHITESPACE));

		if (value_string.length && (value_string.str[0] == '"') && (value_string.str[value_string.length - 1] == '"')) {
			++value_string.str;
			value_string.length -= 2;
		}

		if (def_string.length) {
			hash_t hash_value = hash(STRING_ARGS(value_string));

			log_infof(0, STRING_CONST("  %.*s: %.*s -> 0x%" PRIx64), STRING_FORMAT(def_string),
			          STRING_FORMAT(value_string), hash_value);

			if (check_only) {
				// Check local consistency
				result = hashify_check_local_consistency(value_string, hash_value, local_hashes);
			} else {
				stream_write_format(
				    output_file,
				    STRING_CONST("#define %.*s static_hash_string(\"%.*s\", %" PRIsize ", 0x%" PRIx64 "ULL)\n"),
				    STRING_FORMAT(def_string), STRING_FORMAT(value_string), value_string.length, hash_value);
			}

			if (result == HASHIFY_RESULT_OK) {
				hashify_string_t hash_string;

				// Check history
				result = hashify_check_collisions(value_string, hash_value, *history);

				// Add to history
				hash_string.string = string_clone(STRING_ARGS(value_string));
				hash_string.hash = hash_value;
				array_push_memcpy(*history, &hash_string);
				array_push_memcpy(local_generated, &hash_string);
				array_push_memcpy(all_hashes, &hash_string);
			}
		}
	}

	if (check_only) {
		// Check local consistency
		result = hashify_check_match(local_hashes, local_generated);
	}

	array_deallocate(local_hashes);
	array_deallocate(local_generated);

	return result;
}

int
hashify_generate_preamble(stream_t* output_file, string_t output_filename) {
	// Read and preserve everything before #pragma once in case it contains header comments to be preserved
	char line_buffer[HASHIFY_LINEBUFFER_LENGTH];
	size_t capacity = 1024;
	string_t preamble = string_allocate(0, capacity);
	stream_t* prev_file = stream_open(STRING_ARGS(output_filename), STREAM_IN);

	memset(line_buffer, 0, sizeof(line_buffer));
	while (prev_file && !stream_eos(prev_file)) {
		string_t line;
		string_const_t stripped_line;

		line = stream_read_line_buffer(prev_file, line_buffer, sizeof(line_buffer), '\n');
		stripped_line = string_strip(STRING_ARGS(line), STRING_CONST("\n\r"));

		if ((string_find_string(STRING_ARGS(stripped_line), STRING_CONST("pragma"), 0) != STRING_NPOS) &&
		    (string_find_string(STRING_ARGS(stripped_line), STRING_CONST("once"), 0) != STRING_NPOS))
			break;

		if (preamble.length + stripped_line.length + 1 >= capacity) {
			size_t newcapacity = capacity + 1024 + stripped_line.length;
			preamble.str = memory_reallocate(preamble.str, newcapacity, 0, capacity, 0);
			capacity = newcapacity;
		}

		preamble = string_append(STRING_ARGS(preamble), capacity, STRING_ARGS(stripped_line));
		if (line.length < sizeof(line_buffer))
			preamble = string_append(STRING_ARGS(preamble), capacity, STRING_CONST(STRING_NEWLINE));
	}
	stream_deallocate(prev_file);

	stream_seek(output_file, 0, STREAM_SEEK_BEGIN);
	if (preamble.length)
		stream_write_string(output_file, STRING_ARGS(preamble));
	stream_write_string(output_file, STRING_CONST("#pragma once\n\n"
	                                              "#include <foundation/hash.h>\n\n"
	                                              "/* ****** AUTOMATICALLY GENERATED, DO NOT EDIT ******\n"
	                                              "    Edit corresponding definitions file and rerun\n"
	                                              "    the foundation hashify tool to update this file */\n\n"));

	string_deallocate(preamble.str);

	return 0;
}

int
hashify_read_hashes(stream_t* file, hashify_string_t** hashes) {
	// Read in hashes in file
	char line_buffer[HASHIFY_LINEBUFFER_LENGTH];
	string_const_t tokens[32];

	memset(line_buffer, 0, sizeof(line_buffer));
	do {
		string_t line = stream_read_line_buffer(file, line_buffer, sizeof(line_buffer), '\n');
		string_const_t stripped_line = string_strip(STRING_ARGS(line), STRING_CONST("\n\r"));
		if ((string_find_string(STRING_ARGS(stripped_line), STRING_CONST("define"), 0) != STRING_NPOS) &&
		    (string_find_string(STRING_ARGS(stripped_line), STRING_CONST("static_hash"), 0) != STRING_NPOS)) {
			// Format is: #define HASH_<hashstring> static_hash_string( "<string>", 0x<hashvalue>ULL )
			size_t tokens_count = string_explode(STRING_ARGS(stripped_line), STRING_CONST(" \t"), tokens, 32, false);

			if (tokens_count >= 6) {
				hashify_string_t hash_string;
				string_const_t stripped = string_strip(STRING_ARGS(tokens[3]), STRING_CONST(","));
				stripped = string_strip(STRING_ARGS(stripped), STRING_CONST("\""));
				hash_string.string = string_clone(STRING_ARGS(stripped));
				hash_string.hash = string_to_uint64(STRING_ARGS(tokens[4]), true);

				if (hash(STRING_ARGS(hash_string.string)) != hash_string.hash) {
					log_errorf(0, ERROR_INVALID_VALUE,
					           STRING_CONST("  hash output file is out of date, %.*s is set to 0x%" PRIx64
					                        " but should be 0x%" PRIx64),
					           STRING_FORMAT(hash_string.string), hash_string.hash,
					           hash(STRING_ARGS(hash_string.string)));
					return HASHIFY_RESULT_OUTPUT_FILE_OUT_OF_DATE;
				}

				array_push_memcpy(*hashes, &hash_string);
				array_push_memcpy(all_hashes, &hash_string);
			}
		}
	} while (!stream_eos(file));

	return 0;
}

int
hashify_write_file(stream_t* generated_file, string_t output_filename) {
	bool need_update = false;
	stream_t* output_file = 0;
	int result = HASHIFY_RESULT_OK;

	output_file = stream_open(STRING_ARGS(output_filename), STREAM_OUT | STREAM_IN);
	if (!output_file) {
		need_update = true;
		output_file = stream_open(STRING_ARGS(output_filename), STREAM_OUT | STREAM_CREATE);
		if (!output_file) {
			log_warnf(0, WARNING_INVALID_VALUE, STRING_CONST("Unable to open output file: %.*s"),
			          STRING_FORMAT(output_filename));
			return HASHIFY_RESULT_MISSING_OUTPUT_FILE;
		}
	}

	if (!need_update)
		need_update = !uint128_equal(stream_md5(generated_file), stream_md5(output_file));

	if (need_update) {
		char local_buffer[1024];
		size_t read = 0;
		size_t written = 0;
		uint64_t total_written = 0;

		stream_seek(generated_file, 0, STREAM_SEEK_BEGIN);
		stream_seek(output_file, 0, STREAM_SEEK_BEGIN);

		while (!stream_eos(generated_file)) {
			read = stream_read(generated_file, local_buffer, 1024);
			if (!read)
				break;

			written = stream_write(output_file, local_buffer, read);
			total_written += written;

			if (written != read) {
				log_errorf(
				    0, ERROR_SYSTEM_CALL_FAIL,
				    STRING_CONST("Unable to write to output file '%.*s': %" PRIsize " of %" PRIsize " bytes written"),
				    STRING_FORMAT(output_filename), written, read);
				result = HASHIFY_RESULT_OUTPUT_FILE_WRITE_FAIL;
				break;
			}
		}

		if (result == HASHIFY_RESULT_OK) {
			stream_truncate(output_file, (size_t)total_written);
			log_infof(0, STRING_CONST("  wrote %.*s : %" PRIu64 " bytes"), STRING_FORMAT(output_filename),
			          total_written);
		}
	} else {
		log_info(0, STRING_CONST("  hash file already up to date"));
	}

	stream_deallocate(output_file);

	return result;
}

int
hashify_check_local_consistency(string_const_t string, hash_t hash_value, const hashify_string_t* local_hashes) {
	size_t ilocal, localsize;
	for (ilocal = 0, localsize = array_size(local_hashes); ilocal < localsize; ++ilocal) {
		if (local_hashes[ilocal].hash == hash_value) {
			if (!string_equal(local_hashes[ilocal].string.str, local_hashes[ilocal].string.length, string.str,
			                  string.length)) {
				log_errorf(0, ERROR_INVALID_VALUE,
				           STRING_CONST("  hash string mismatch, \"%.*s\" with hash 0x%" PRIx64
				                        " stored in output file, read \"%.*s\" from input file"),
				           STRING_FORMAT(local_hashes[ilocal].string), local_hashes[ilocal].hash,
				           STRING_FORMAT(string));
				return HASHIFY_RESULT_HASH_STRING_MISMATCH;
			}
			break;
		} else if (string_equal(local_hashes[ilocal].string.str, local_hashes[ilocal].string.length, string.str,
		                        string.length)) {
			log_errorf(0, ERROR_INVALID_VALUE,
			           STRING_CONST("  hash mismatch, \"%.*s\" with hash 0x%" PRIx64
			                        " stored in output file, read \"%.*s\" with hash 0x%" PRIx64 " from input file"),
			           STRING_FORMAT(local_hashes[ilocal].string), local_hashes[ilocal].hash, STRING_FORMAT(string),
			           hash_value);
			return HASHIFY_RESULT_HASH_MISMATCH;
		}
	}
	if (ilocal == localsize) {
		log_errorf(0, ERROR_INVALID_VALUE, STRING_CONST("  hash missing in output file, \"%.*s\" with hash 0x%" PRIx64),
		           STRING_FORMAT(string), hash_value);
		return HASHIFY_RESULT_HASH_MISSING;
	}

	return HASHIFY_RESULT_OK;
}

int
hashify_check_collisions(string_const_t string, hash_t hash_value, const hashify_string_t* history) {
	size_t ihist, history_size;
	for (ihist = 0, history_size = array_size(history); ihist < history_size; ++ihist) {
		if (history[ihist].hash == hash_value) {
			if (string_equal(history[ihist].string.str, history[ihist].string.length, string.str, string.length)) {
				log_errorf(0, ERROR_INVALID_VALUE, STRING_CONST("  global string duplication, \"%.*s\""),
				           STRING_FORMAT(string));
				return HASHIFY_RESULT_STRING_COLLISION;
			} else {
				log_errorf(0, ERROR_INVALID_VALUE,
				           STRING_CONST("  global hash collision, 0x%" PRIx64 " between: \"%.*s\" and \"%.*s\" "),
				           hash_value, STRING_FORMAT(string), STRING_FORMAT(history[ihist].string));
				return HASHIFY_RESULT_HASH_COLLISION;
			}
		}
	}

	return 0;
}

int
hashify_check_match(const hashify_string_t* hashes, const hashify_string_t* generated) {
	// From hashify_check_local_consistency we know that generated array already is a subset of hashes
	// This test checks that hashes is a subset of generated, i.e sets are equal
	size_t ihash, hashes_size, igen, generated_size;
	for (ihash = 0, hashes_size = array_size(hashes); ihash < hashes_size; ++ihash) {
		for (igen = 0, generated_size = array_size(generated); igen < generated_size; ++igen) {
			if (hashes[ihash].hash == generated[igen].hash) {
				if (!string_equal(hashes[ihash].string.str, hashes[ihash].string.length, generated[igen].string.str,
				                  generated[igen].string.length)) {
					log_errorf(0, ERROR_INVALID_VALUE,
					           STRING_CONST("  hash string mismatch, \"%.*s\" with hash 0x%" PRIx64
					                        " stored in output file, generated by \"%.*s\" from input file"),
					           STRING_FORMAT(hashes[ihash].string), hashes[ihash].hash,
					           STRING_FORMAT(generated[igen].string));
					return HASHIFY_RESULT_HASH_STRING_MISMATCH;
				}
				break;
			} else if (string_equal(hashes[ihash].string.str, hashes[ihash].string.length, generated[igen].string.str,
			                        generated[igen].string.length)) {
				log_errorf(
				    0, ERROR_INVALID_VALUE,
				    STRING_CONST("  hash mismatch, \"%.*s\" with hash 0x%" PRIx64
				                 " stored in output file, \"%.*s\" generated hash 0x%" PRIx64 " from input file"),
				    STRING_FORMAT(hashes[ihash].string), hashes[ihash].hash, STRING_FORMAT(generated[igen].string),
				    generated[igen].hash);
				return HASHIFY_RESULT_HASH_MISMATCH;
			}
		}
		if (igen == generated_size) {
			log_errorf(0, ERROR_INVALID_VALUE,
			           STRING_CONST("  extra hash \"%.*s\" with hash 0x%" PRIx64 " not found in input file"),
			           STRING_FORMAT(hashes[ihash].string), hashes[ihash].hash);
			return HASHIFY_RESULT_EXTRA_STRING;
		}
	}
	return HASHIFY_RESULT_OK;
}

static void
hashify_print_usage(void) {
	const error_level_t saved_level = log_suppress(0);
	log_set_suppress(0, ERRORLEVEL_DEBUG);
	log_info(
	    0,
	    STRING_CONST(
	        "hashify usage:\n"
	        "  hashify [--validate] [--generate-string <string>] [<filename> <filename> ...] [--debug] [--help] [--]\n"
	        "    Generated files have the same file name as the input file, with the extension replaced by .h\n"
	        "    Optional arguments:\n"
	        "      --validate                   Suppress output and only validate existing hashes\n"
	        "      --generate-string <string>   Generate hash of the given string\n"
	        "      <filename> <filename> ...    Any number of input files\n"
	        "      --debug                      Enable debug output\n"
	        "      --help                       Display this help message\n"
	        "      --                           Stop processing command line arguments"));
	log_set_suppress(0, saved_level);
}
