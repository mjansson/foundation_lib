/* main.c  -  Foundation example application  -  Public Domain  -  2013 Mattias Jansson
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#include <foundation/foundation.h>

/* Global trigger beacon */
static beacon_t example_trigger;

/* Global control flag */
static bool example_terminate;

/* Event handler loop run in a separate thread */
static void*
event_loop(void* arg) {
	event_stream_t* stream = arg;
	event_block_t* block;
	event_t* event = 0;

	// Set the system event stream to fire this thread beacon when an event
	// is posted. The beacon will thus wake this thread from the blocking
	// thread_wait() call
	event_stream_set_beacon(stream, &thread_self()->beacon);

	// Run this loop until a FOUNDATIONEVENT_TERMINATE event is posted
	while (!example_terminate) {
		// Process all pending events in the event stream
		block = event_stream_process(stream);
		event = 0;
		while ((event = event_next(block, event))) {
			switch (event->id) {
				case FOUNDATIONEVENT_START:
					break;

				case FOUNDATIONEVENT_TERMINATE:
					// Trigger flag and beacon
					example_terminate = true;
					beacon_fire(&example_trigger);
					break;

				case FOUNDATIONEVENT_FOCUS_GAIN:
				case FOUNDATIONEVENT_FOCUS_LOST:
					break;

				default:
					break;
			}
		}

		// Block and wait for events unless terminating
		if (!example_terminate)
			thread_wait();
	}

	return 0;
}

/* Exception handler, just post a log message and terminate */
static void
example_exception_handler(const char* dump_file, size_t length) {
	FOUNDATION_UNUSED(dump_file);
	FOUNDATION_UNUSED(length);
	log_error(HASH_TEST, ERROR_EXCEPTION, STRING_CONST("Test raised exception"));
	process_exit(-1);
}

/* Find the base directory where execution should occur */
static void
set_suitable_working_directory(void) {
	char buffer[BUILD_MAX_PATHLEN];
	bool found = false;

	// Example executable/config directory layout is
	//  [base_path]/bin/<platform>/<buildtype>/<arch>
	//  [base_path]/config
	// We are looking for the base_path as current working directory.
	// Simply walk the path of the executable until we find the config subdir.
	string_const_t last_dir;
	string_t config_dir;
	string_const_t working_dir = environment_executable_directory();
	do {
		last_dir = working_dir;
		config_dir =
		    path_concat(buffer, sizeof(buffer), STRING_ARGS(working_dir), STRING_CONST("config"));
		if (fs_is_directory(STRING_ARGS(config_dir))) {
			found = true;
			break;
		}

		working_dir = path_directory_name(STRING_ARGS(working_dir));
	} while (!string_equal(STRING_ARGS(working_dir), STRING_ARGS(last_dir)));

	// Set found working directory
	if (found)
		environment_set_current_working_directory(STRING_ARGS(working_dir));
}

/* Initialization entry point */
int
main_initialize(void) {
	foundation_config_t config;
	application_t application;
	int ret;

#if !BUILD_DEPLOY
	// Only track memory in non-deploy builds
	memory_set_tracker(memory_tracker_local());
#endif

	// Use default values for foundation config
	memset(&config, 0, sizeof(config));

	// Declare the application
	memset(&application, 0, sizeof(application));
	application.name = string_const(STRING_CONST("Foundation example application"));
	application.short_name = string_const(STRING_CONST("example"));
	application.company = string_const(STRING_CONST("Foo Bar Inc"));
	application.version = foundation_version();
	application.flags = APPLICATION_UTILITY;
	application.exception_handler = example_exception_handler;

	// Suppress debug messages
	log_set_suppress(0, ERRORLEVEL_DEBUG);

	// Initialize library
	ret = foundation_initialize(memory_system_malloc(), application, config);
	if (ret)
		return ret;

	// Set working directory
	set_suitable_working_directory();

	return ret;
}

/* Phony configuration parser */
static void
do_parse_config(const char* buffer, size_t size, json_token_t* tokens, size_t numtokens) {
	FOUNDATION_UNUSED(size);

	for (size_t tok = numtokens ? tokens[0].child : 0; tok && tok < numtokens;
	     tok = tokens[tok].sibling) {
		string_const_t id = json_token_identifier(buffer, tokens + tok);
		if ((tokens[tok].type == JSON_OBJECT) &&
		    string_equal(STRING_ARGS(id), STRING_CONST("example_object"))) {
			for (size_t restok = tokens[tok].child; restok && (restok < numtokens);
			     restok = tokens[restok].sibling) {
				string_const_t resid = json_token_identifier(buffer, tokens + restok);
				if (tokens[restok].type == JSON_STRING) {
					hash_t idhash = hash(STRING_ARGS(resid));
					hash_t wantedhash = hash(STRING_CONST("sub_variable"));

					if (idhash == wantedhash) {
						// Do something with value here
						// string_const_t value = json_token_value(buffer, tokens + restok);
					}
				}
			}
		}
	}
}

/* Read configuration files in "config" subdirectory */
static void
do_read_config(void) {
	sjson_parse_path(STRING_CONST("config"), do_parse_config);
}

/* Phony execution function */
static void
do_compute_and_magic(void) {
	// Just wait for teminate flag
	while (!example_terminate) {
		// Process system events
		system_process_events();

		// Post a log message
		log_info(0, STRING_CONST("Going to sleep for a bit..."));

		// Wait for trigger, loop and process system events
		// every 500 milliseconds
		beacon_try_wait(&example_trigger, 500);
	}
}

/* Main execution entry point */
int
main_run(void* main_arg) {
	thread_t event_thread;

	// Initialize trigger beacon
	beacon_initialize(&example_trigger);

	// Start event thread
	thread_initialize(&event_thread, event_loop, system_event_stream(),
	                  STRING_CONST("event_thread"), THREAD_PRIORITY_NORMAL, 0);
	thread_start(&event_thread);

	// Do configuration
	do_read_config();

	// Do execution
	do_compute_and_magic();

	// Finalize event thread
	thread_finalize(&event_thread);

	// Finalize beacon
	beacon_finalize(&example_trigger);

	return 0;
}

/* Finalization entry point */
void
main_finalize(void) {
	// Finalize the library
	foundation_finalize();
}
