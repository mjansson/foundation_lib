/* main.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#if !BUILD_DYNAMIC_LINK

#if FOUNDATION_PLATFORM_TIZEN
#include <foundation/tizen.h>
#endif

#if FOUNDATION_PLATFORM_WINDOWS

#include <foundation/windows.h>

static BOOL STDCALL
main_console_handler(DWORD control_type) {
	const char* control_name = "UNKNOWN";
	bool post_terminate = false;
	bool handled = true;

	switch (control_type) {
		case CTRL_C_EVENT:
			control_name = "CTRL_C";
			post_terminate = true;
			break;
		case CTRL_BREAK_EVENT:
			control_name = "CTRL_BREAK";
			break;
		case CTRL_CLOSE_EVENT:
			control_name = "CTRL_CLOSE";
			post_terminate = true;
			break;
		case CTRL_LOGOFF_EVENT:
			control_name = "CTRL_LOGOFF";
			break;
		case CTRL_SHUTDOWN_EVENT:
			control_name = "CTRL_SHUTDOWN";
			post_terminate = true;
			break;
		default:
			handled = false;
			break;
	}
	log_infof(0, STRING_CONST("Caught console control: %s (%lu)"), control_name, control_type);
	if (post_terminate) {
		/*lint -e{970} */
		unsigned long level = 0, flags = 0;

		system_post_event(FOUNDATIONEVENT_TERMINATE);

		GetProcessShutdownParameters(&level, &flags);
		SetProcessShutdownParameters(level, SHUTDOWN_NORETRY);

		thread_sleep(1000);
	}
	return handled;
}

/*! Win32 UI application entry point, credits to Microsoft for ignoring yet another standard... */
#if FOUNDATION_COMPILER_MSVC
FOUNDATION_API int APIENTRY
WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
#endif

int STDCALL
WinMain(HINSTANCE instance, HINSTANCE previnst, LPSTR cline, int cmd_show) {
	int ret;

	FOUNDATION_UNUSED(instance);
	FOUNDATION_UNUSED(previnst);
	FOUNDATION_UNUSED(cline);
	FOUNDATION_UNUSED(cmd_show);

	if (main_initialize() < 0)
		return -1;

	SetConsoleCtrlHandler(main_console_handler, TRUE);

	thread_set_main();

	system_post_event(FOUNDATIONEVENT_START);

#if BUILD_DEBUG
	ret = main_run(0);
#else
	{
		string_t name;
		const application_t* app = environment_application();
		string_const_t aname = app->short_name;
		string_const_t vstr = string_from_version_static(app->version);
		name = string_allocate_concat_varg(aname.length ? aname.str : "unknown", aname.length ? aname.length : 7,
		                                   STRING_CONST("-"), STRING_ARGS(vstr), nullptr);

		if (app->exception_handler)
			exception_set_handler(app->exception_handler, STRING_ARGS(name));
		if (app->exception_handler && !system_debugger_attached())
			ret = exception_try(main_run, 0, app->exception_handler, STRING_ARGS(name));
		else
			ret = main_run(0);

		string_deallocate(name.str);
	}
#endif

	main_finalize();

	return ret;
}

#elif FOUNDATION_PLATFORM_ANDROID
#include <foundation/android.h>
#endif

#if FOUNDATION_PLATFORM_POSIX

#include <foundation/posix.h>

#include <signal.h>
#include <stdio.h>

#if FOUNDATION_PLATFORM_APPLE
#include <foundation/main.h>
#include <foundation/delegate.h>
#endif

static void
sighandler(int sig) {
	const char* signame = "UNKNOWN";
	switch (sig) {
		case SIGKILL:
			signame = "SIGKILL";
			break;
		case SIGTERM:
			signame = "SIGTERM";
			break;
		case SIGQUIT:
			signame = "SIGQUIT";
			break;
		case SIGINT:
			signame = "SIGINT";
			break;
		default:
			break;
	}
	log_infof(0, STRING_CONST("Caught signal: %s (%d)"), signame, sig);
	system_post_event(FOUNDATIONEVENT_TERMINATE);
}

#endif

#if FOUNDATION_PLATFORM_ANDROID
/*! Aliased entry point */
static int
real_main(void)
#else
/*! Normal entry point for all platforms, including Windows console applications */
int
main(int argc, char** argv)
#endif
{
	int ret;

#if !FOUNDATION_PLATFORM_ANDROID
	environment_main_args(argc, (const char* const*)argv);
#endif

	ret = main_initialize();
	if (ret < 0)
		return ret;

#if FOUNDATION_PLATFORM_POSIX

	// Set signal handlers
	{
		struct sigaction action;
		memset(&action, 0, sizeof(action));

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif

		// Signals we process globally
		action.sa_handler = sighandler;
		sigaction(SIGKILL, &action, 0);
		sigaction(SIGTERM, &action, 0);
		sigaction(SIGQUIT, &action, 0);
		sigaction(SIGINT, &action, 0);
		sigaction(SIGABRT, &action, 0);

		// Ignore sigpipe
		action.sa_handler = SIG_IGN;
		sigaction(SIGPIPE, &action, 0);

#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic pop
#endif
	}

#endif

#if FOUNDATION_PLATFORM_ANDROID
	if ((ret = android_initialize()) < 0)
		return ret;
#endif

#if FOUNDATION_PLATFORM_TIZEN
	if ((ret = tizen_initialize()) < 0)
		return ret;
#endif

#if FOUNDATION_PLATFORM_WINDOWS

	SetConsoleCtrlHandler(main_console_handler, TRUE);

#endif

	thread_set_main();

#if FOUNDATION_PLATFORM_WINDOWS || FOUNDATION_PLATFORM_LINUX
	system_post_event(FOUNDATIONEVENT_START);
#endif

#if FOUNDATION_PLATFORM_APPLE
#if FOUNDATION_PLATFORM_MACOS
	if (!(environment_application()->flags & (APPLICATION_UTILITY | APPLICATION_DAEMON))) {
		delegate_start_main_ns_thread();

		thread_exit();

		extern int NSApplicationMain(int argc, char* argv[]);
		ret = NSApplicationMain(argc, argv);

#elif FOUNDATION_PLATFORM_IOS
	{
		delegate_start_main_ns_thread();

		thread_exit();

		extern int UIApplicationMain(int argc, char* argv[], void* principalClassName, void* delegateClassName);
		ret = UIApplicationMain(argc, (char**)argv, 0, 0);

#endif
		// NSApplicationMain and UIApplicationMain never returns though
		return ret;
	}
#endif

#if !FOUNDATION_PLATFORM_IOS

#if FOUNDATION_PLATFORM_TIZEN
	tizen_start_main_thread();
	ret = tizen_app_main(argc, argv);
#else
	{
		string_t name;
		const application_t* app = environment_application();
		{
			string_const_t vstr = string_from_version_static(app->version);
			string_const_t aname = app->short_name;
			if (!aname.length)
				aname = string_const(STRING_CONST("unknown"));
			name = string_allocate_format(STRING_CONST("%.*s-%.*s"), (int)aname.length, aname.str, (int)vstr.length,
			                              vstr.str);
		}

		if (app->exception_handler)
			exception_set_handler(app->exception_handler, STRING_ARGS(name));
		if (app->exception_handler && !system_debugger_attached())
			ret = exception_try(main_run, 0, app->exception_handler, STRING_ARGS(name));
		else
			ret = main_run(0);

		string_deallocate(name.str);
	}
#endif

	main_finalize();

#if FOUNDATION_PLATFORM_ANDROID
	android_finalize();
#endif

#if FOUNDATION_PLATFORM_TIZEN
	tizen_finalize();
#endif

	return ret;
#endif
}

#if FOUNDATION_PLATFORM_ANDROID

/*! Android native glue entry point */
void
android_main(struct android_app* app) {
	if (!app)
		return;
	android_entry(app);
	real_main();
}

#endif

#endif
