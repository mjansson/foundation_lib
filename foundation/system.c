/* system.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

#if FOUNDATION_PLATFORM_POSIX
#include <foundation/posix.h>
#include <sys/socket.h>
#include <net/if.h>
#endif

#if FOUNDATION_PLATFORM_ANDROID
#include <foundation/android.h>
#include <cpu-features.h>
#endif

#if FOUNDATION_PLATFORM_BSD
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#if FOUNDATION_PLATFORM_APPLE
#include <sys/sysctl.h>

extern unsigned int
system_process_info_processor_count(void);

extern int
system_show_alert(const char*, size_t, const char*, size_t, int);

#endif

#define SYSTEM_BUFFER_SIZE 511
FOUNDATION_DECLARE_THREAD_LOCAL(char*, system_buffer, 0)

static device_orientation_t system_device_orientation_current = DEVICEORIENTATION_UNKNOWN;
static event_stream_t* system_event_stream_current;

struct platform_info_t {
	platform_t platform;
	architecture_t architecture;
	byteorder_t byteorder;
};

typedef struct platform_info_t platform_info_t;

static const platform_info_t platform_info = {

#if FOUNDATION_PLATFORM_WINDOWS
    PLATFORM_WINDOWS,
#elif FOUNDATION_PLATFORM_ANDROID
    PLATFORM_ANDROID,
#elif FOUNDATION_PLATFORM_LINUX_RASPBERRYPI
    PLATFORM_RASPBERRYPI,
#elif FOUNDATION_PLATFORM_LINUX
    PLATFORM_LINUX,
#elif FOUNDATION_PLATFORM_MACOS
    PLATFORM_MACOS,
#elif FOUNDATION_PLATFORM_IOS
    PLATFORM_IOS,
#elif FOUNDATION_PLATFORM_BSD
    PLATFORM_BSD,
#elif FOUNDATION_PLATFORM_TIZEN
    PLATFORM_TIZEN,
#else
#error Unknown platform
#endif

#if FOUNDATION_ARCH_X86_64
    ARCHITECTURE_X86_64,
#elif FOUNDATION_ARCH_X86
    ARCHITECTURE_X86,
#elif FOUNDATION_ARCH_PPC_64
    ARCHITECTURE_PPC_64,
#elif FOUNDATION_ARCH_PPC
    ARCHITECTURE_PPC,
#elif FOUNDATION_ARCH_ARM8_64
    ARCHITECTURE_ARM8_64,
#elif FOUNDATION_ARCH_ARM8
    ARCHITECTURE_ARM8,
#elif FOUNDATION_ARCH_ARM7
    ARCHITECTURE_ARM7,
#elif FOUNDATION_ARCH_ARM6
    ARCHITECTURE_ARM6,
#elif FOUNDATION_ARCH_ARM5
    ARCHITECTURE_ARM5,
#elif FOUNDATION_ARCH_MIPS_64
    ARCHITECTURE_MIPS_64,
#elif FOUNDATION_ARCH_MIPS
    ARCHITECTURE_MIPS,
#elif FOUNDATION_ARCH_GENERIC
    ARCHITECTURE_GENERIC,
#else
#error Unknown architecture
#endif

#if FOUNDATION_ARCH_ENDIAN_LITTLE
    BYTEORDER_LITTLEENDIAN
#else
    BYTEORDER_BIGENDIAN
#endif
};

static char*
system_buffer(void) {
	char* buffer = get_thread_system_buffer();
	if (!buffer) {
		buffer = memory_allocate(0, SYSTEM_BUFFER_SIZE + 1, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		set_thread_system_buffer(buffer);
	}
	return buffer;
}

platform_t
system_platform(void) {
	return platform_info.platform;
}

architecture_t
system_architecture(void) {
	return platform_info.architecture;
}

byteorder_t
system_byteorder(void) {
	return platform_info.byteorder;
}

#if FOUNDATION_PLATFORM_WINDOWS

#include <foundation/windows.h>

static object_t system_library_iphlpapi;

int
internal_system_initialize(void) {
	system_event_stream_current = event_stream_allocate(128);
	return 0;
}

void
internal_system_finalize(void) {
	if (system_library_iphlpapi)
		library_release(system_library_iphlpapi);
	system_library_iphlpapi = 0;

	event_stream_deallocate(system_event_stream_current);
	system_event_stream_current = 0;
}

int
system_error(void) {
	return (int)GetLastError();
}

void
system_error_reset(void) {
	SetLastError(0);
}

string_const_t
system_error_message(int code) {
	char* errmsg;
	if (!code)
		code = system_error();
	if (!code)
		return string_const(STRING_CONST("<no error>"));

	errmsg = system_buffer();
	errmsg[0] = 0;
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 0, (unsigned int)code & 0xBFFFFFFF,
	               0 /*LANG_SYSTEM_DEFAULT*/ /*MAKELANGID( LANG_ENGLISH, SUBLANG_DEFAULT )*/, errmsg,
	               SYSTEM_BUFFER_SIZE, 0);
	return string_strip(errmsg, string_length(errmsg), STRING_CONST(STRING_WHITESPACE));
}

string_t
system_hostname(char* buffer, size_t capacity) {
	DWORD size = (DWORD)capacity;
	if (!GetComputerNameA(buffer, &size))
		return string_copy(buffer, capacity, STRING_CONST("unknown"));
	return (string_t){buffer, size};
}

uint64_t
system_hostid(void) {
	/*lint --e{970} */
	union {
		unsigned char hostid[8];
		uint64_t val;
	} hostid;
	IP_ADAPTER_INFO adapter_info[16];
	unsigned int status, i, j;
	unsigned long buffer_length;
	DWORD(STDCALL * get_adapters_info)(PIP_ADAPTER_INFO, PULONG) = 0;

	if (!system_library_iphlpapi)
		system_library_iphlpapi = library_load(STRING_CONST("iphlpapi"));
	if (system_library_iphlpapi) {
		/*lint -e{611} */
		get_adapters_info = (DWORD(STDCALL*)(PIP_ADAPTER_INFO, PULONG))library_symbol(system_library_iphlpapi,
		                                                                              STRING_CONST("GetAdaptersInfo"));
	}
	if (!get_adapters_info)
		return 0;

	hostid.val = 0;
	buffer_length = sizeof(adapter_info);  // Save memory size of buffer
	memset(adapter_info, 0, sizeof(adapter_info));
	status = get_adapters_info(adapter_info, &buffer_length);
	if (status == ERROR_SUCCESS)
		for (i = 0; i < 16; ++i) {
			if (adapter_info[i].Type == MIB_IF_TYPE_ETHERNET) {
				for (j = 0; j < 6; ++j)
					hostid.hostid[5 - j] = adapter_info[i].Address[j];
				break;
			}
		}
	return hostid.val;
}

string_t
system_username(char* buffer, size_t capacity) {
	DWORD size = (DWORD)capacity;
	if (!GetUserNameA(buffer, &size))
		return string_copy(buffer, capacity, STRING_CONST("unknown"));
	return (string_t){buffer, size};
}

static size_t processor_total_count;
extern size_t processor_group_count;
size_t processor_group_count;

size_t
system_hardware_threads(void) {
	if (processor_total_count)
		return processor_total_count;

	size_t processor_count = 0;
	size_t group_count = 1;

	object_t kernel_lib = library_load(STRING_CONST("kernel32"));
	if (kernel_lib) {
		WORD(STDCALL * get_group_count)
		(void) = (WORD(STDCALL*)(void))library_symbol(kernel_lib, STRING_CONST("GetActiveProcessorGroupCount"));
		DWORD(STDCALL * get_processor_count)
		(WORD) = (DWORD(STDCALL*)(WORD))library_symbol(kernel_lib, STRING_CONST("GetActiveProcessorCount"));

		if (get_group_count)
			group_count = (size_t)get_group_count();
		if (get_processor_count)
			processor_count = (size_t)get_processor_count(ALL_PROCESSOR_GROUPS);

		if (group_count > 1) {
			// Force process affinity to span all groups
			for (size_t igroup = group_count; igroup > 0; --igroup) {
				GROUP_AFFINITY affinity = {0};
				affinity.Group = (WORD)(igroup - 1);
				affinity.Mask = (ULONG_PTR)-1;
				SetThreadGroupAffinity(GetCurrentThread(), &affinity, 0);
			}
		}

		library_release(kernel_lib);
	}
	if (!processor_count) {
		SYSTEM_INFO system_info;
		GetSystemInfo(&system_info);
		processor_count = (size_t)system_info.dwNumberOfProcessors;
	}

	processor_total_count = processor_count;
	processor_group_count = group_count;

	return processor_total_count;
}

void
system_process_events(void) {
}

bool
system_debugger_attached(void) {
	return IsDebuggerPresent();
}

void
system_pause(void) {
	system("pause");
}

static uint32_t
system_default_locale(void) {
	return LOCALE_DEFAULT;
}

typedef int(STDCALL* fnGetLocaleInfoEx)(LPCWSTR, LCTYPE, LPWSTR, int);

static uint32_t
system_user_locale(void) {
	fnGetLocaleInfoEx get_locale_info =
	    (fnGetLocaleInfoEx)GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetLocaleInfoEx");
	if (get_locale_info) {
		wchar_t locale_sname[128] = {0};
		union {
			char buffer[8];
			uint32_t value;
		} locale_data;
		string_t locale_string;
		if (get_locale_info(0 /*LOCALE_NAME_USER_DEFAULT*/, 0x0000005c /*LOCALE_SNAME*/, locale_sname, 32) > 0) {
			locale_string = string_convert_utf16(locale_data.buffer, sizeof(locale_data.buffer),
			                                     (uint16_t*)locale_sname, wstring_length(locale_sname));
			if (string_match_pattern(STRING_ARGS(locale_string), STRING_CONST("?"
			                                                                  "?"
			                                                                  "-"
			                                                                  "?"
			                                                                  "?"))) {
				locale_data.buffer[2] = locale_data.buffer[3];
				locale_data.buffer[3] = locale_data.buffer[4];
				locale_data.buffer[4] = 0;
				return locale_data.value;
			}
		}
	}

	return system_default_locale();
}

#elif FOUNDATION_PLATFORM_POSIX
#if !FOUNDATION_PLATFORM_ANDROID
#include <ifaddrs.h>
#endif

int
internal_system_initialize(void) {
	system_event_stream_current = event_stream_allocate(128);
	return 0;
}

void
internal_system_finalize(void) {
	event_stream_deallocate(system_event_stream_current);
	system_event_stream_current = 0;
}

int
system_error(void) {
	return errno;
}

void
system_error_reset(void) {
	errno = 0;
}

string_const_t
system_error_message(int code) {
	if (!code)
		code = system_error();
	if (!code)
		return string_const(STRING_CONST("<no error>"));
	char* buffer = system_buffer();
#if FOUNDATION_PLATFORM_LINUX && defined(_GNU_SOURCE)
	if ((buffer = strerror_r(code, buffer, SYSTEM_BUFFER_SIZE)) != nullptr)
		return string_const(buffer, string_length(buffer));
#else
	if (strerror_r(code, buffer, SYSTEM_BUFFER_SIZE) == 0)
		return string_const(buffer, string_length(buffer));
#endif
	return string_const(STRING_CONST("<no error string>"));
}

string_t
system_hostname(char* buffer, size_t size) {
	int ret = gethostname(buffer, size);
	if ((ret < 0) || !size || !*buffer)
		return string_copy(buffer, size, STRING_CONST("unknown"));
	return (string_t){buffer, string_length(buffer)};
}

string_t
system_username(char* buffer, size_t size) {
	struct passwd passwd;
	struct passwd* result;

#if FOUNDATION_PLATFORM_ANDROID && (__ANDROID_API__ <= 19)
	result = 0;
#else
	if (getpwuid_r(getuid(), &passwd, buffer, size, &result) != 0)
		result = 0;
#endif
	if (!result || !result->pw_name || !*result->pw_name) {
#if FOUNDATION_PLATFORM_ANDROID
		const char* login = getlogin();
		if (!login || !*login)
			return string_copy(buffer, size, STRING_CONST("unknown"));
		return string_copy(buffer, size, login, string_length(login));
#elif FOUNDATION_PLATFORM_BSD
		if (getlogin_r(buffer, (int)size) != 0)
			return string_copy(buffer, size, STRING_CONST("unknown"));
		return (string_t){buffer, string_length(buffer)};
#else
		if (getlogin_r(buffer, size) != 0)
			return string_copy(buffer, size, STRING_CONST("unknown"));
		return (string_t){buffer, string_length(buffer)};
#endif
	}
	return (string_t){result->pw_name, string_length(result->pw_name)};
}

#if FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD

#include <net/if_dl.h>

static uint64_t
system_hostid_lookup(struct ifaddrs* ifaddr) {
	unsigned int j;
	union {
		uint64_t id;
		uint8_t FOUNDATION_ALIGN(8) buffer[8];
	} hostid;

	if (ifaddr->ifa_addr && (ifaddr->ifa_addr->sa_family == AF_LINK)) {
		struct sockaddr_dl addr_dl;
		memcpy(&addr_dl, ifaddr->ifa_addr, sizeof(addr_dl));
		if (addr_dl.sdl_alen == 6) {
			hostid.id = 0;
			for (j = 0; j < 6; ++j)
				hostid.buffer[5 - j] = (uint8_t)LLADDR(&addr_dl)[j];
			return hostid.id;
		}
	}

	return 0;
}

#elif FOUNDATION_PLATFORM_POSIX

static uint64_t
system_hostid_lookup(int sock, struct ifreq* ifr) {
	unsigned int j;
	union {
		uint64_t id;
		uint8_t FOUNDATION_ALIGN(8) buffer[8];
	} hostid;

	if (ioctl(sock, SIOCGIFHWADDR, ifr) < 0) {
		log_warnf(0, WARNING_SYSTEM_CALL_FAIL, STRING_CONST("Unable to get hw address for interface %s"),
		          ifr->ifr_name);
		return 0;
	}

	hostid.id = 0;
	for (j = 0; j < 6; ++j)
		hostid.buffer[5 - j] = (uint8_t)ifr->ifr_hwaddr.sa_data[j];

	return hostid.id;
}

#endif

uint64_t
system_hostid(void) {
	uint64_t hostid = 0;

#if FOUNDATION_PLATFORM_ANDROID

	struct ifconf ifc;
	struct ifreq* ifrarr;
	int sock = socket(PF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		log_warn(0, WARNING_SYSTEM_CALL_FAIL, STRING_CONST("Unable to lookup system hostid (no socket)"));
		return 0;
	}

	memset(&ifc, 0, sizeof(ifc));
	if (ioctl(sock, SIOCGIFCONF, &ifc) == 0) {
		ifrarr = memory_allocate(0, (unsigned int)ifc.ifc_len, 0, MEMORY_TEMPORARY);
		ifc.ifc_ifcu.ifcu_req = ifrarr;
		if (ioctl(sock, SIOCGIFCONF, &ifc) == 0) {
			int numif = ifc.ifc_len / (int)sizeof(struct ifreq);
			int iif;
			for (iif = 0; (iif < numif) && !hostid; ++iif) {
				struct ifreq* ifr = &ifrarr[iif];
				if (ifr->ifr_name && string_equal(ifr->ifr_name, 2, "lo", 2))
					continue;
				hostid = system_hostid_lookup(sock, ifr);
			}
		} else {
			log_warn(0, WARNING_SYSTEM_CALL_FAIL, STRING_CONST("Unable to lookup system hostid (query ioctl failed)"));
		}
		memory_deallocate(ifrarr);
	} else {
		log_warn(0, WARNING_SYSTEM_CALL_FAIL, STRING_CONST("Unable to lookup system hostid (ioctl failed)"));
	}

	close(sock);

#elif FOUNDATION_PLATFORM_APPLE || FOUNDATION_PLATFORM_BSD

	struct ifaddrs* ifaddr;
	struct ifaddrs* ifa;

	if (getifaddrs(&ifaddr) == 0) {
		for (ifa = ifaddr; ifa && !hostid; ifa = ifa->ifa_next) {
			if (memcmp(ifa->ifa_name, "lo", 2) == 0)
				continue;

			hostid = system_hostid_lookup(ifa);
		}
		freeifaddrs(ifaddr);
	}

#else

	struct ifaddrs* ifaddr;
	struct ifaddrs* ifa;
	struct ifreq ifr;
	int sock = socket(PF_INET, SOCK_DGRAM, 0);

	if (getifaddrs(&ifaddr) == 0) {
		for (ifa = ifaddr; ifa && !hostid; ifa = ifa->ifa_next) {
			size_t ifa_length = string_length(ifa->ifa_name);
			if (string_equal(ifa->ifa_name, ifa_length, "lo", 2))
				continue;

			memset(&ifr, 0, sizeof(ifr));
			string_copy(ifr.ifr_name, sizeof(ifr.ifr_name), ifa->ifa_name, ifa_length);

			hostid = system_hostid_lookup(sock, &ifr);
		}
		freeifaddrs(ifaddr);
	} else {
		memset(&ifr, 0, sizeof(ifr));
		string_copy(ifr.ifr_name, sizeof(ifr.ifr_name), "eth0", 4);

		hostid = system_hostid_lookup(sock, &ifr);
	}

	close(sock);

#endif

	return hostid;
}

size_t
system_hardware_threads(void) {
#if FOUNDATION_PLATFORM_APPLE
	return system_process_info_processor_count();
#elif FOUNDATION_PLATFORM_ANDROID
	return (size_t)android_getCpuCount();
#elif FOUNDATION_PLATFORM_BSD
	int ctlarg[2], ncpu;
	size_t len;

	ctlarg[0] = CTL_HW;
	ctlarg[1] = HW_NCPU;
	len = sizeof(ncpu);
	if (sysctl(ctlarg, 2, &ncpu, &len, 0, 0) == 0)
		return (size_t)ncpu;
	return 1;
#else
	cpu_set_t prevmask, testmask;
	CPU_ZERO(&prevmask);
	CPU_ZERO(&testmask);
	sched_getaffinity(0, sizeof(prevmask), &prevmask);  // Get current mask
	sched_setaffinity(0, sizeof(testmask), &testmask);  // Set zero mask
	sched_getaffinity(0, sizeof(testmask), &testmask);  // Get mask for all CPUs
	sched_setaffinity(0, sizeof(prevmask), &prevmask);  // Reset current mask
	int num = CPU_COUNT(&testmask);
	return (size_t)(num > 1 ? num : 1);
#endif
}

void
system_process_events(void) {
#if FOUNDATION_PLATFORM_ANDROID
	profile_begin_block(STRING_CONST("system events"));

	int ident = 0;
	int events = 0;
	int nummsg = 0;
	struct android_poll_source* source = 0;
	struct android_app* app = android_app();

	while ((ident = ALooper_pollOnce(0, 0, &events, (void**)&source)) >= 0) {
		// Process this event.
		if (source)
			source->process(app, source);
		++nummsg;
	}

	profile_end_block();
#endif
}

bool
system_debugger_attached(void) {
#if FOUNDATION_PLATFORM_APPLE

	int mib[4];
	struct kinfo_proc info;
	size_t size;

	memset(&info, 0, sizeof(info));
	info.kp_proc.p_flag = 0;

	mib[0] = CTL_KERN;
	mib[1] = KERN_PROC;
	mib[2] = KERN_PROC_PID;
	mib[3] = getpid();

	size = sizeof(info);
	sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, 0, 0);

	return ((info.kp_proc.p_flag & P_TRACED) != 0);

#elif FOUNDATION_PLATFORM_LINUX || FOUNDATION_PLATFORM_ANDROID

	int fd, ib, ofs, partial = 0;
	bool read_pid = false;
	ssize_t nread;
	static const char tracer_pid[] = "TracerPid:";

	fd = open("/proc/self/status", O_RDONLY);
	if (fd < 0)
		return false;

	do {
		char buffer[128];
		nread = read(fd, buffer, sizeof(buffer));
		if (nread > 0) {
			for (ib = 0; (ib < nread); ++ib) {
				if (read_pid) {
					if ((buffer[ib] >= '1') && (buffer[ib] <= '9'))
						return true;
					if ((buffer[ib] != ' ') && (buffer[ib] != '\t'))
						return false;
				} else {
					for (ofs = 0; (ib + ofs < nread) && tracer_pid[partial]; ++partial, ++ofs) {
						if (buffer[ib + ofs] != tracer_pid[partial])
							break;
					}
					if (!tracer_pid[partial]) {
						ib += ofs;
						read_pid = true;
					} else if (ib + partial < nread)
						partial = 0;
				}
			}
		}
	} while (nread > 0);

	return false;

#else
	return false;
#endif
}

void
system_pause(void) {
}

static uint32_t
system_default_locale(void) {
	return LOCALE_DEFAULT;
}

static uint32_t
system_user_locale(void) {
	return system_default_locale();
}

#endif

static uint32_t system_locale_current;

uint32_t
system_locale(void) {
	return system_locale_current ? system_locale_current : system_user_locale();
}

string_t
system_locale_string(char* buffer, size_t capacity) {
	uint32_t locale = system_locale();
	size_t length = (capacity > 4) ? 4 : capacity;
	memcpy(buffer, &locale, length);
	if (capacity > 4)
		buffer[4] = 0;
	return (string_t){buffer, length};
}

void
system_set_locale(uint32_t locale) {
	system_locale_current = locale;
}

uint16_t
system_language(void) {
	return (uint16_t)(system_locale() & 0xFFFF);
}

uint16_t
system_country(void) {
	return (uint16_t)((system_locale() >> 16) & 0xFFFF);
}

void
system_set_device_orientation(device_orientation_t orientation) {
	if (system_device_orientation_current == orientation)
		return;

	system_device_orientation_current = orientation;
	system_post_event(FOUNDATIONEVENT_DEVICE_ORIENTATION);
}

device_orientation_t
system_device_orientation(void) {
	return system_device_orientation_current;
}

event_stream_t*
system_event_stream(void) {
	return system_event_stream_current;
}

void
system_post_event(foundation_event_id event) {
	if (system_event_stream_current)
		event_post(system_event_stream_current, (int)event, 0, 0, 0, 0);
}

bool
system_message_box(const char* title, size_t title_length, const char* message, size_t message_length,
                   bool cancel_button) {
	if (environment_application()->flags & APPLICATION_UNATTENDED)
		return true;

#if FOUNDATION_PLATFORM_WINDOWS
	FOUNDATION_UNUSED(message_length);
	FOUNDATION_UNUSED(title_length);
	return (MessageBoxA(0, message, title, cancel_button ? MB_OKCANCEL : MB_OK) == IDOK);
#elif FOUNDATION_PLATFORM_APPLE
	return system_show_alert(title, title_length, message, message_length, cancel_button ? 1 : 0) > 0;
#elif 0  // FOUNDATION_PLATFORM_LINUX
	char* buf = string_format("%s\n\n%s\n", title, message);
	pid_t pid = fork();

	switch (pid) {
		case -1:
			// error
			string_deallocate(buf);
			break;

		case 0:
			execlp("xmessage", "xmessage", "-buttons", cancel_button ? "OK:101,Cancel:102" : "OK:101", "-default", "OK",
			       "-center", buf, (char*)0);
			_exit(-1);
			break;

		default:
			string_deallocate(buf);
			int status;
			waitpid(pid, &status, 0);
			if ((!WIFEXITED(status)) || (WEXITSTATUS(status) != 101))
				return false;
			return true;
	}

	return false;
#else
	// Not implemented
	FOUNDATION_UNUSED(message);
	FOUNDATION_UNUSED(message_length);
	FOUNDATION_UNUSED(title);
	FOUNDATION_UNUSED(title_length);
	FOUNDATION_UNUSED(cancel_button);
	return false;
#endif
}

void
system_thread_finalize(void) {
	char* buffer = get_thread_system_buffer();
	if (!buffer)
		return;

	memory_deallocate(buffer);
	set_thread_system_buffer(0);
}
