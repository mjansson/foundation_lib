/* uuid.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

typedef struct {
	uint32_t data1;
	uint16_t data2;
	uint16_t data3;
	uint8_t data4[8];
} uuid_raw_t;

typedef struct {
	uint32_t time_low;
	uint16_t time_mid;
	uint16_t time_hi_and_version;
	uint8_t clock_seq_hi_and_reserved;
	uint8_t clock_seq_low;
	uint8_t node[6];
} uuid_time_t;

typedef union {
	uuid_t uuid;
	uuid_raw_t raw;
	uint128_t rnd;
} uuid_random_t;

typedef union {
	uuid_raw_t raw;
	uuid_time_t time;
	uuid_t uuid;
} uuid_convert_t;

static atomic32_t uuid_last_counter;
static atomic64_t uuid_last_hostid;

// 6ba7b810-9dad-11d1-80b4-00c04fd430c8
#if FOUNDATION_ARCH_ENDIAN_LITTLE
const uuid_t UUID_DNS = {{0x11d19dad6ba7b810ULL, 0xc830d44fc000b480ULL}};
#else
const uuid_t UUID_DNS = {{0x6ba7b8109dad11d1ULL, 0x80b400c04fd430c8ULL}};
#endif

uuid_t
uuid_generate_random(void) {
	uuid_random_t random_uuid;
	random_uuid.rnd = uint128_make(random64(), random64());

	// Add variant and version
	random_uuid.raw.data3 &= 0x0FFF;
	random_uuid.raw.data3 |= 0x4000;
	random_uuid.raw.data4[0] &= 0x3F;
	random_uuid.raw.data4[0] |= 0x80;

	return random_uuid.uuid;
}

uuid_t
uuid_generate_time(void) {
	uuid_time_t time_uuid;
	uuid_convert_t convert;
	tick_t current_time;
	int32_t current_counter;
	tick_t current_tick;
	uint32_t clock_seq;
	uint64_t host_id, in;

	// Allows creation of 10000 unique timestamps per millisecond
	current_time = time_system();
	current_counter = atomic_incr32(&uuid_last_counter, memory_order_relaxed) % 10000;

	current_tick =
	    (current_time * 10000LL) + current_counter + 0x01B21DD213814000LL;  // Convert to 100ns since UUID UTC base
	                                                                        // time, October 15 1582, and add counter

	// We have no state so clock sequence is random
	clock_seq = random32();
	/*lint --e{704} */
	time_uuid.time_low = (uint32_t)(current_tick & 0xFFFFFFFFLL);
	time_uuid.time_mid = (uint16_t)((current_tick >> 32LL) & 0xFFFFLL);
	time_uuid.time_hi_and_version = (uint16_t)((current_tick >> 48LL) & 0xFFFFLL);
	time_uuid.clock_seq_low = (clock_seq & 0xFF);
	time_uuid.clock_seq_hi_and_reserved = ((clock_seq & 0x3F00) >> 8);

	// If hardware node ID is null, use random and set identifier (multicast) bit
	host_id = (uint64_t)atomic_load64(&uuid_last_hostid, memory_order_relaxed);
	if (!host_id) {
		host_id = system_hostid();
		atomic_store64(&uuid_last_hostid, (int64_t)host_id, memory_order_relaxed);
	}
	if (host_id) {
		for (in = 0; in < 6; ++in) {
			uint64_t byteval = (host_id >> (8ULL * in)) & 0xFFULL;
			time_uuid.node[5 - in] = (uint8_t)byteval;
		}
	} else {
		for (in = 0; in < 6; ++in)
			time_uuid.node[in] = (uint8_t)(random32() & 0xFF);
		time_uuid.node[0] |= 0x01;
	}

	// Add variant and version
	time_uuid.time_hi_and_version &= 0x0FFF;
	time_uuid.time_hi_and_version |= (1 << 12);
	time_uuid.clock_seq_hi_and_reserved &= 0x3F;
	time_uuid.clock_seq_hi_and_reserved |= 0x80;

	convert.time = time_uuid;
	return convert.uuid;
}

uuid_t
uuid_generate_name(const uuid_t ns, const char* name, size_t length) {
	// v3 uuid, namespace and md5
	md5_t md5;
	uuid_raw_t namespace_id;
	uuid_raw_t gen_uuid;
	uuid_convert_t convert;
	uint128_t digest;

	// Namespace in network byte order
	convert.uuid = ns;
	namespace_id = convert.raw;
	namespace_id.data1 = byteorder_bigendian32(namespace_id.data1);
	namespace_id.data2 = byteorder_bigendian16(namespace_id.data2);
	namespace_id.data3 = byteorder_bigendian16(namespace_id.data3);

	md5_initialize(&md5);
	md5_digest(&md5, &namespace_id, sizeof(namespace_id));
	md5_digest(&md5, name, length);
	md5_digest_finalize(&md5);

	// Convert to host order
	digest = md5_get_digest_raw(&md5);
	memcpy(&gen_uuid, &digest, sizeof(uuid_raw_t));
	gen_uuid.data1 = byteorder_bigendian32(gen_uuid.data1);
	gen_uuid.data2 = byteorder_bigendian16(gen_uuid.data2);
	gen_uuid.data3 = byteorder_bigendian16(gen_uuid.data3);

	// Add variant and version
	gen_uuid.data3 &= 0x0FFF;
	gen_uuid.data3 |= (3 << 12);  // Variant 3 for MD5
	gen_uuid.data4[0] &= 0x3F;
	gen_uuid.data4[0] |= 0x80;

	md5_finalize(&md5);

	convert.raw = gen_uuid;
	return convert.uuid;
}

#include <stdio.h>

#if FOUNDATION_PLATFORM_WINDOWS
#if FOUNDATION_COMPILER_MSVC || FOUNDATION_COMPILER_INTEL || FOUNDATION_COMPILER_CLANG
#define snprintf(p, s, ...) _snprintf_s(p, s, _TRUNCATE, __VA_ARGS__)
#define sscanf sscanf_s
#endif
#endif

string_t
string_from_uuid(char* buffer, size_t size, const uuid_t val) {
	int len;
	uuid_convert_t convert;
	if (!size)
		return (string_t){buffer, 0};
	convert.uuid = val;
	len = snprintf(buffer, size, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", convert.raw.data1,
	               convert.raw.data2, convert.raw.data3, convert.raw.data4[0], convert.raw.data4[1],
	               convert.raw.data4[2], convert.raw.data4[3], convert.raw.data4[4], convert.raw.data4[5],
	               convert.raw.data4[6], convert.raw.data4[7]);
	if ((len < 0) || ((unsigned int)len > size)) {
		buffer[size - 1] = 0;
		return (string_t){buffer, size - 1};
	}
	return (string_t){buffer, (unsigned int)len};
}

uuid_t
string_to_uuid(const char* str, size_t length) {
	uuid_convert_t convert;
	unsigned int data[10];
	memset(data, 0, sizeof(data));
	convert.raw.data1 = 0;
	if (length &&
	    (sscanf(str, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", &convert.raw.data1, &data[0], &data[1],
	            &data[2], &data[3], &data[4], &data[5], &data[6], &data[7], &data[8], &data[9]) == 11)) {
		convert.raw.data2 = (uint16_t)data[0];
		convert.raw.data3 = (uint16_t)data[1];
		convert.raw.data4[0] = (uint8_t)data[2];
		convert.raw.data4[1] = (uint8_t)data[3];
		convert.raw.data4[2] = (uint8_t)data[4];
		convert.raw.data4[3] = (uint8_t)data[5];
		convert.raw.data4[4] = (uint8_t)data[6];
		convert.raw.data4[5] = (uint8_t)data[7];
		convert.raw.data4[6] = (uint8_t)data[8];
		convert.raw.data4[7] = (uint8_t)data[9];
		return convert.uuid;
	}
	return uuid_null();
}
