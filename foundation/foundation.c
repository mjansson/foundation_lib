/* foundation.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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
#include <foundation/internal.h>

// Make artificial reference to main entry point
#if FOUNDATION_PLATFORM_ANDROID

struct android_app;

extern void
android_main(struct android_app*);

#elif !BUILD_DYNAMIC_LINK

extern int
main(int, char**);

#endif

static foundation_config_t foundation_cfg;
static bool foundation_initialized;

static void
foundation_initialize_config(const foundation_config_t config) {
	foundation_cfg.library_max = (config.library_max ? config.library_max : 32);
	foundation_cfg.fs_monitor_max = (config.fs_monitor_max ? config.fs_monitor_max : 16);
	foundation_cfg.error_context_depth = (config.error_context_depth ? config.error_context_depth : 32);
	foundation_cfg.memory_context_depth = (config.memory_context_depth ? config.memory_context_depth : 32);
	foundation_cfg.stacktrace_depth = (config.stacktrace_depth ? config.stacktrace_depth : 32);
	foundation_cfg.event_block_chunk = (config.event_block_chunk ? config.event_block_chunk : (8 * 1024));
	foundation_cfg.event_block_limit = (config.event_block_limit ? config.event_block_limit : (512 * 1024));
	foundation_cfg.thread_stack_size = (config.thread_stack_size ? config.thread_stack_size : 0x10000);
	foundation_cfg.hash_store_size = config.hash_store_size;
	foundation_cfg.random_state_prealloc = config.random_state_prealloc;
}

#define SUBSYSTEM_INIT(system) \
	if (ret == 0)              \
	ret = internal_##system##_initialize()
#define SUBSYSTEM_INIT_ARGS(system, ...) \
	if (ret == 0)                        \
	ret = internal_##system##_initialize(__VA_ARGS__)

int
foundation_initialize(const memory_system_t memory, const application_t application, const foundation_config_t config) {
	int ret = 0;

	if (foundation_initialized)
		return 0;

	process_set_exit_code(PROCESS_EXIT_SUCCESS);

	foundation_initialize_config(config);

	/*lint -e774 */
	SUBSYSTEM_INIT(atomic);
	SUBSYSTEM_INIT_ARGS(memory, memory);
	SUBSYSTEM_INIT(static_hash);
	SUBSYSTEM_INIT(assert);
	SUBSYSTEM_INIT(library);
	SUBSYSTEM_INIT(log);
	SUBSYSTEM_INIT(time);
	SUBSYSTEM_INIT(thread);
	SUBSYSTEM_INIT(random);
	SUBSYSTEM_INIT(stream);
	SUBSYSTEM_INIT(fs);
	SUBSYSTEM_INIT(stacktrace);
	SUBSYSTEM_INIT(exception);
	SUBSYSTEM_INIT_ARGS(environment, application);
	SUBSYSTEM_INIT(system);

	if (ret)
		return ret;

	// Parse built-in command line options
	{
		/*lint --e{613} */
		const string_const_t* cmdline = environment_command_line();
		size_t iarg, argsize;
		for (iarg = 0, argsize = array_size(cmdline); iarg < argsize; ++iarg) {
			string_const_t arg = cmdline[iarg];
			if (string_equal(arg.str, arg.length, STRING_CONST("--log-debug")))
				log_set_suppress(0, ERRORLEVEL_NONE);
			else if (string_equal(arg.str, arg.length, STRING_CONST("--log-info")))
				log_set_suppress(0, ERRORLEVEL_DEBUG);
			else if (string_equal(arg.str, arg.length, STRING_CONST("--log-warning")))
				log_set_suppress(0, ERRORLEVEL_WARNING);
			else if (string_equal(arg.str, arg.length, STRING_CONST("--log-error")))
				log_set_suppress(0, ERRORLEVEL_ERROR);
		}
	}

#if !BUILD_DYNAMIC_LINK
	// Artificial references
	/*lint -e506 */
#if FOUNDATION_PLATFORM_ANDROID
	android_main(0);
#else
	if ((uintptr_t)main < 1)
		return -1;
#endif
#endif

	foundation_initialized = true;

	return 0;
}

void
foundation_finalize(void) {
	if (!foundation_initialized)
		return;
	foundation_initialized = false;

	profile_finalize();

	internal_fs_finalize();
	internal_stream_finalize();
	internal_system_finalize();
	internal_library_finalize();
	internal_environment_finalize();
	internal_random_finalize();
	internal_thread_finalize();
	internal_time_finalize();
	internal_log_finalize();
	internal_assert_finalize();
	internal_exception_finalize();
	internal_stacktrace_finalize();
	internal_static_hash_finalize();
	internal_memory_finalize();
	internal_atomic_finalize();
}

bool
foundation_is_initialized(void) {
	return foundation_initialized;
}

foundation_config_t
foundation_config(void) {
	return foundation_cfg;
}
