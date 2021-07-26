/* main.c  -  Foundation bitbuffer test  -  Public Domain  -  2013 Mattias Jansson
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
#include <test/test.h>

static application_t
test_bitbuffer_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation bitbuffer tests"));
	app.short_name = string_const(STRING_CONST("test_bitbuffer"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_bitbuffer_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_bitbuffer_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_bitbuffer_initialize(void) {
	return 0;
}

static void
test_bitbuffer_finalize(void) {
}

DECLARE_TEST(bitbuffer, basics) {
	uint32_t buffer[1024];
	uint32_t read;
	bitbuffer_t bitbuffer;

	memset(buffer, 0xFF, 1024 * 4);

	bitbuffer_initialize_buffer(&bitbuffer, buffer, 1024 * 4, false);

	bitbuffer_write32(&bitbuffer, 0x12345678, 32);
	bitbuffer_align_write(&bitbuffer, false);
	bitbuffer_discard_write(&bitbuffer);

	EXPECT_EQ(buffer[0], 0x12345678);
	EXPECT_EQ(buffer[1], 0xFFFFFFFF);

	bitbuffer_align_write(&bitbuffer, true);

	EXPECT_EQ(buffer[0], 0x12345678);
	EXPECT_EQ(buffer[1], 0x00000000);
	EXPECT_EQ(buffer[2], 0xFFFFFFFF);

	bitbuffer_write32(&bitbuffer, 0x12345678, 8);

	EXPECT_EQ(buffer[0], 0x12345678);
	EXPECT_EQ(buffer[1], 0x00000000);
	EXPECT_EQ(buffer[2], 0xFFFFFFFF);

	bitbuffer_align_write(&bitbuffer, false);

	EXPECT_EQ(buffer[0], 0x12345678);
	EXPECT_EQ(buffer[1], 0x00000000);
	EXPECT_EQ(buffer[2], 0x00000078);
	EXPECT_EQ(buffer[3], 0xFFFFFFFF);

	bitbuffer_write32(&bitbuffer, 0x12345678, 27);
	bitbuffer_write32(&bitbuffer, 0x12345678, 4);
	bitbuffer_discard_write(&bitbuffer);
	bitbuffer_write32(&bitbuffer, 0x12345678, 12);

	EXPECT_EQ(buffer[0], 0x12345678);
	EXPECT_EQ(buffer[1], 0x00000000);
	EXPECT_EQ(buffer[2], 0x00000078);
	EXPECT_EQ(buffer[3], 0xFFFFFFFF);

	bitbuffer_align_write(&bitbuffer, true);
	bitbuffer_discard_write(&bitbuffer);

	EXPECT_EQ(buffer[0], 0x12345678);
	EXPECT_EQ(buffer[1], 0x00000000);
	EXPECT_EQ(buffer[2], 0x00000078);
	EXPECT_EQ(buffer[3], 0x00000678);
	EXPECT_EQ(buffer[4], 0xFFFFFFFF);

	bitbuffer_initialize_buffer(&bitbuffer, buffer, 5 * 4, false);

	bitbuffer_discard_read(&bitbuffer);
	read = bitbuffer_read32(&bitbuffer, 32);
	bitbuffer_align_read(&bitbuffer, false);

	EXPECT_EQ(read, 0x12345678);

	read = bitbuffer_read32(&bitbuffer, 32);

	EXPECT_EQ(read, 0x00000000);

	// Should skip buffer[2] (0x00000078)
	bitbuffer_align_read(&bitbuffer, true);
	read = bitbuffer_read32(&bitbuffer, 12);

	EXPECT_EQ(read, 0x00000678);

	// Re-read
	bitbuffer_discard_read(&bitbuffer);
	read = bitbuffer_read32(&bitbuffer, 12);
	bitbuffer_align_read(&bitbuffer, false);

	EXPECT_EQ(read, 0x00000678);

	read = bitbuffer_read32(&bitbuffer, 31);

	EXPECT_EQ(read, 0x7FFFFFFF);

	// Read past end
	read = bitbuffer_read32(&bitbuffer, 31);

	EXPECT_UINTEQ(read, 1);

	return 0;
}

DECLARE_TEST(bitbuffer, readwrite) {
	int ipass, ival;
	uint32_t buffer[1024];
	bitbuffer_t bitbuffer;
	uint128_t readval128, writeval128;
	float32_cast_t convread32, convwant32;
	float64_cast_t convread64, convwant64;

	uint32_t val32[4] = {random32(), random32(), random32(), random32()};
	uint64_t val64[4] = {random64(), random64(), random64(), random64()};
	uint128_t val128[4];
	float32_t valf32[4] = {(float32_t)random_normalized(), (float32_t)random_normalized(),
	                       (float32_t)random_normalized(), (float32_t)random_normalized()};
	float64_t valf64[4] = {(float64_t)random_normalized(), (float64_t)random_normalized(),
	                       (float64_t)random_normalized(), (float64_t)random_normalized()};

	unsigned int bits32[20] = {0};
	unsigned int bits64[20] = {0};
	unsigned int bits128[20] = {0};

	for (ival = 0; ival < 4; ++ival)
		val128[ival] = uint128_make(random64(), random64());

	for (ipass = 0; ipass < 8192; ++ipass) {
		for (ival = 0; ival < 20; ++ival) {
			bits32[ival] = random32_range(0, 33);
			bits64[ival] = random32_range(0, 65);
			bits128[ival] = random32_range(0, 129);
		}

		// Phase 1 - write data
		bitbuffer_initialize_buffer(&bitbuffer, buffer, 1024 * 4, false);

		for (ival = 0; ival < 16; ++ival) {
			bitbuffer_write32(&bitbuffer, val32[0], bits32[ival]);
			bitbuffer_write32(&bitbuffer, val32[1], bits32[ival + 1]);
			bitbuffer_write32(&bitbuffer, val32[2], bits32[ival + 2]);
			bitbuffer_write32(&bitbuffer, val32[3], bits32[ival + 3]);

			if (!(ipass % 13))
				bitbuffer_align_write(&bitbuffer, false);

			bitbuffer_write64(&bitbuffer, val64[0], bits64[ival]);
			bitbuffer_write64(&bitbuffer, val64[1], bits64[ival + 1]);
			bitbuffer_write64(&bitbuffer, val64[2], bits64[ival + 2]);
			bitbuffer_write64(&bitbuffer, val64[3], bits64[ival + 3]);

			bitbuffer_write128(&bitbuffer, val128[0], bits128[ival]);
			bitbuffer_write128(&bitbuffer, val128[1], bits128[ival + 1]);
			bitbuffer_write128(&bitbuffer, val128[2], bits128[ival + 2]);
			bitbuffer_write128(&bitbuffer, val128[3], bits128[ival + 3]);

			if (!(ipass % 29))
				bitbuffer_align_write(&bitbuffer, true);

			bitbuffer_write_float32(&bitbuffer, valf32[0]);
			bitbuffer_write_float32(&bitbuffer, valf32[1]);
			bitbuffer_write_float32(&bitbuffer, valf32[2]);
			bitbuffer_write_float32(&bitbuffer, valf32[3]);

			bitbuffer_write_float64(&bitbuffer, valf64[0]);
			bitbuffer_write_float64(&bitbuffer, valf64[1]);
			bitbuffer_write_float64(&bitbuffer, valf64[2]);
			bitbuffer_write_float64(&bitbuffer, valf64[3]);
		}

		bitbuffer_align_write(&bitbuffer, false);
		bitbuffer_write64(&bitbuffer, 0, 63);

		// Phase 2 - read and verify data
		bitbuffer_initialize_buffer(&bitbuffer, buffer, 1024 * 4, false);

		for (ival = 0; ival < 16; ++ival) {
			EXPECT_EQ(bitbuffer_read32(&bitbuffer, bits32[ival]),
			          val32[0] & (bits32[ival] == 32 ? (uint32_t)-1 : (bits32[ival] ? (1U << bits32[ival]) - 1 : 0U)));
			EXPECT_EQ(bitbuffer_read32(&bitbuffer, bits32[ival + 1]),
			          val32[1] & (bits32[ival + 1] == 32 ? (uint32_t)-1 :
			                                               (bits32[ival + 1] ? (1U << bits32[ival + 1]) - 1 : 0U)));
			EXPECT_EQ(bitbuffer_read32(&bitbuffer, bits32[ival + 2]),
			          val32[2] & (bits32[ival + 2] == 32 ? (uint32_t)-1 :
			                                               (bits32[ival + 2] ? (1U << bits32[ival + 2]) - 1 : 0U)));
			EXPECT_EQ(bitbuffer_read32(&bitbuffer, bits32[ival + 3]),
			          val32[3] & (bits32[ival + 3] == 32 ? (uint32_t)-1 :
			                                               (bits32[ival + 3] ? (1U << bits32[ival + 3]) - 1 : 0U)));

			if (!(ipass % 13))
				bitbuffer_align_read(&bitbuffer, false);

			EXPECT_EQ(
			    bitbuffer_read64(&bitbuffer, bits64[ival]),
			    val64[0] & (bits64[ival] == 64 ? (uint64_t)-1 : (bits64[ival] ? (1ULL << bits64[ival]) - 1 : 0ULL)));
			EXPECT_EQ(bitbuffer_read64(&bitbuffer, bits64[ival + 1]),
			          val64[1] & (bits64[ival + 1] == 64 ? (uint64_t)-1 :
			                                               (bits64[ival + 1] ? (1ULL << bits64[ival + 1]) - 1 : 0ULL)));
			EXPECT_EQ(bitbuffer_read64(&bitbuffer, bits64[ival + 2]),
			          val64[2] & (bits64[ival + 2] == 64 ? (uint64_t)-1 :
			                                               (bits64[ival + 2] ? (1ULL << bits64[ival + 2]) - 1 : 0ULL)));
			EXPECT_EQ(bitbuffer_read64(&bitbuffer, bits64[ival + 3]),
			          val64[3] & (bits64[ival + 3] == 64 ? (uint64_t)-1 :
			                                               (bits64[ival + 3] ? (1ULL << bits64[ival + 3]) - 1 : 0ULL)));

			writeval128 = val128[0];
			writeval128.word[0] &=
			    (bits128[ival] >= 64 ? (uint64_t)-1 : (bits128[ival] ? (1ULL << bits128[ival]) - 1 : 0));
			writeval128.word[1] &=
			    (bits128[ival] == 128 ? (uint64_t)-1 : (bits128[ival] > 64 ? (1ULL << (bits128[ival] - 64)) - 1 : 0));
			readval128 = bitbuffer_read128(&bitbuffer, bits128[ival]);
			EXPECT_TRUE(uint128_equal(readval128, writeval128));
			writeval128 = val128[1];
			writeval128.word[0] &=
			    (bits128[ival + 1] >= 64 ? (uint64_t)-1 : (bits128[ival + 1] ? (1ULL << bits128[ival + 1]) - 1 : 0));
			writeval128.word[1] &=
			    (bits128[ival + 1] == 128 ? (uint64_t)-1 :
			                                (bits128[ival + 1] > 64 ? (1ULL << (bits128[ival + 1] - 64)) - 1 : 0));
			readval128 = bitbuffer_read128(&bitbuffer, bits128[ival + 1]);
			EXPECT_TRUE(uint128_equal(readval128, writeval128));
			writeval128 = val128[2];
			writeval128.word[0] &=
			    (bits128[ival + 2] >= 64 ? (uint64_t)-1 : (bits128[ival + 2] ? (1ULL << bits128[ival + 2]) - 1 : 0));
			writeval128.word[1] &=
			    (bits128[ival + 2] == 128 ? (uint64_t)-1 :
			                                (bits128[ival + 2] > 64 ? (1ULL << (bits128[ival + 2] - 64)) - 1 : 0));
			readval128 = bitbuffer_read128(&bitbuffer, bits128[ival + 2]);
			EXPECT_TRUE(uint128_equal(readval128, writeval128));
			writeval128 = val128[3];
			writeval128.word[0] &=
			    (bits128[ival + 3] >= 64 ? (uint64_t)-1 : (bits128[ival + 3] ? (1ULL << bits128[ival + 3]) - 1 : 0));
			writeval128.word[1] &=
			    (bits128[ival + 3] == 128 ? (uint64_t)-1 :
			                                (bits128[ival + 3] > 64 ? (1ULL << (bits128[ival + 3] - 64)) - 1 : 0));
			readval128 = bitbuffer_read128(&bitbuffer, bits128[ival + 3]);
			EXPECT_TRUE(uint128_equal(readval128, writeval128));

			if (!(ipass % 29))
				bitbuffer_align_read(&bitbuffer, true);

			convread32.fval = bitbuffer_read_float32(&bitbuffer);
			convwant32.fval = valf32[0];
			EXPECT_EQ(convread32.uival, convwant32.uival);

			convread32.fval = bitbuffer_read_float32(&bitbuffer);
			convwant32.fval = valf32[1];
			EXPECT_EQ(convread32.uival, convwant32.uival);

			convread32.fval = bitbuffer_read_float32(&bitbuffer);
			convwant32.fval = valf32[2];
			EXPECT_EQ(convread32.uival, convwant32.uival);

			convread32.fval = bitbuffer_read_float32(&bitbuffer);
			convwant32.fval = valf32[3];
			EXPECT_EQ(convread32.uival, convwant32.uival);

			convread64.fval = bitbuffer_read_float64(&bitbuffer);
			convwant64.fval = valf64[0];
			EXPECT_EQ(convread64.uival, convwant64.uival);

			convread64.fval = bitbuffer_read_float64(&bitbuffer);
			convwant64.fval = valf64[1];
			EXPECT_EQ(convread64.uival, convwant64.uival);

			convread64.fval = bitbuffer_read_float64(&bitbuffer);
			convwant64.fval = valf64[2];
			EXPECT_EQ(convread64.uival, convwant64.uival);

			convread64.fval = bitbuffer_read_float64(&bitbuffer);
			convwant64.fval = valf64[3];
			EXPECT_EQ(convread64.uival, convwant64.uival);
		}
	}

	return 0;
}

DECLARE_TEST(bitbuffer, readwriteswap) {
	int ipass, ival;
	uint32_t buffer[1024];
	bitbuffer_t* bitbuffer;
	uint128_t readval128, writeval128;
	float32_cast_t convread32, convwant32;
	float64_cast_t convread64, convwant64;

	uint32_t val32[4] = {random32(), random32(), random32(), random32()};
	uint64_t val64[4] = {random64(), random64(), random64(), random64()};
	uint128_t val128[4];
	float32_t valf32[4] = {(float32_t)random_normalized(), (float32_t)random_normalized(),
	                       (float32_t)random_normalized(), (float32_t)random_normalized()};
	float64_t valf64[4] = {(float64_t)random_normalized(), (float64_t)random_normalized(),
	                       (float64_t)random_normalized(), (float64_t)random_normalized()};

	unsigned int bits32[20] = {0};
	unsigned int bits64[20] = {0};
	unsigned int bits128[20] = {0};

	for (ival = 0; ival < 4; ++ival)
		val128[ival] = uint128_make(random64(), random64());

	for (ipass = 0; ipass < 8192; ++ipass) {
		for (ival = 0; ival < 20; ++ival) {
			bits32[ival] = random32_range(0, 33);
			bits64[ival] = random32_range(0, 65);
			bits128[ival] = random32_range(0, 129);
		}

		// Phase 1 - write data
		bitbuffer = bitbuffer_allocate_buffer(buffer, 1024 * 4, true);

		for (ival = 0; ival < 16; ++ival) {
			bitbuffer_write32(bitbuffer, val32[0], bits32[ival]);
			bitbuffer_write32(bitbuffer, val32[1], bits32[ival + 1]);
			bitbuffer_write32(bitbuffer, val32[2], bits32[ival + 2]);
			bitbuffer_write32(bitbuffer, val32[3], bits32[ival + 3]);

			bitbuffer_write64(bitbuffer, val64[0], bits64[ival]);
			bitbuffer_write64(bitbuffer, val64[1], bits64[ival + 1]);
			bitbuffer_write64(bitbuffer, val64[2], bits64[ival + 2]);
			bitbuffer_write64(bitbuffer, val64[3], bits64[ival + 3]);

			bitbuffer_write128(bitbuffer, val128[0], bits128[ival]);
			bitbuffer_write128(bitbuffer, val128[1], bits128[ival + 1]);
			bitbuffer_write128(bitbuffer, val128[2], bits128[ival + 2]);
			bitbuffer_write128(bitbuffer, val128[3], bits128[ival + 3]);

			bitbuffer_write_float32(bitbuffer, valf32[0]);
			bitbuffer_write_float32(bitbuffer, valf32[1]);
			bitbuffer_write_float32(bitbuffer, valf32[2]);
			bitbuffer_write_float32(bitbuffer, valf32[3]);

			bitbuffer_write_float64(bitbuffer, valf64[0]);
			bitbuffer_write_float64(bitbuffer, valf64[1]);
			bitbuffer_write_float64(bitbuffer, valf64[2]);
			bitbuffer_write_float64(bitbuffer, valf64[3]);
		}

		bitbuffer_align_write(bitbuffer, false);

		// Validate oversize writes get clamped
		bitbuffer_write32(bitbuffer, val32[0], 64);
		bitbuffer_write64(bitbuffer, val64[0], 128);
		bitbuffer_write128(bitbuffer, val128[0], 256);

		bitbuffer_finalize(bitbuffer);

		// Phase 2 - read and verify data
		bitbuffer_initialize_buffer(bitbuffer, buffer, 1024 * 4, true);

		for (ival = 0; ival < 16; ++ival) {
			EXPECT_EQ(bitbuffer_read32(bitbuffer, bits32[ival]),
			          val32[0] & (bits32[ival] == 32 ? (uint32_t)-1 : (bits32[ival] ? (1U << bits32[ival]) - 1 : 0U)));
			EXPECT_EQ(bitbuffer_read32(bitbuffer, bits32[ival + 1]),
			          val32[1] & (bits32[ival + 1] == 32 ? (uint32_t)-1 :
			                                               (bits32[ival + 1] ? (1U << bits32[ival + 1]) - 1 : 0U)));
			EXPECT_EQ(bitbuffer_read32(bitbuffer, bits32[ival + 2]),
			          val32[2] & (bits32[ival + 2] == 32 ? (uint32_t)-1 :
			                                               (bits32[ival + 2] ? (1U << bits32[ival + 2]) - 1 : 0U)));
			EXPECT_EQ(bitbuffer_read32(bitbuffer, bits32[ival + 3]),
			          val32[3] & (bits32[ival + 3] == 32 ? (uint32_t)-1 :
			                                               (bits32[ival + 3] ? (1U << bits32[ival + 3]) - 1 : 0U)));

			EXPECT_EQ(
			    bitbuffer_read64(bitbuffer, bits64[ival]),
			    val64[0] & (bits64[ival] == 64 ? (uint64_t)-1 : (bits64[ival] ? (1ULL << bits64[ival]) - 1 : 0ULL)));
			EXPECT_EQ(bitbuffer_read64(bitbuffer, bits64[ival + 1]),
			          val64[1] & (bits64[ival + 1] == 64 ? (uint64_t)-1 :
			                                               (bits64[ival + 1] ? (1ULL << bits64[ival + 1]) - 1 : 0ULL)));
			EXPECT_EQ(bitbuffer_read64(bitbuffer, bits64[ival + 2]),
			          val64[2] & (bits64[ival + 2] == 64 ? (uint64_t)-1 :
			                                               (bits64[ival + 2] ? (1ULL << bits64[ival + 2]) - 1 : 0ULL)));
			EXPECT_EQ(bitbuffer_read64(bitbuffer, bits64[ival + 3]),
			          val64[3] & (bits64[ival + 3] == 64 ? (uint64_t)-1 :
			                                               (bits64[ival + 3] ? (1ULL << bits64[ival + 3]) - 1 : 0ULL)));

			writeval128 = val128[0];
			writeval128.word[0] &=
			    (bits128[ival] >= 64 ? (uint64_t)-1 : (bits128[ival] ? (1ULL << bits128[ival]) - 1 : 0));
			writeval128.word[1] &=
			    (bits128[ival] == 128 ? (uint64_t)-1 : (bits128[ival] > 64 ? (1ULL << (bits128[ival] - 64)) - 1 : 0));
			readval128 = bitbuffer_read128(bitbuffer, bits128[ival]);
			EXPECT_TRUE(uint128_equal(readval128, writeval128));
			writeval128 = val128[1];
			writeval128.word[0] &=
			    (bits128[ival + 1] >= 64 ? (uint64_t)-1 : (bits128[ival + 1] ? (1ULL << bits128[ival + 1]) - 1 : 0));
			writeval128.word[1] &=
			    (bits128[ival + 1] == 128 ? (uint64_t)-1 :
			                                (bits128[ival + 1] > 64 ? (1ULL << (bits128[ival + 1] - 64)) - 1 : 0));
			readval128 = bitbuffer_read128(bitbuffer, bits128[ival + 1]);
			EXPECT_TRUE(uint128_equal(readval128, writeval128));
			writeval128 = val128[2];
			writeval128.word[0] &=
			    (bits128[ival + 2] >= 64 ? (uint64_t)-1 : (bits128[ival + 2] ? (1ULL << bits128[ival + 2]) - 1 : 0));
			writeval128.word[1] &=
			    (bits128[ival + 2] == 128 ? (uint64_t)-1 :
			                                (bits128[ival + 2] > 64 ? (1ULL << (bits128[ival + 2] - 64)) - 1 : 0));
			readval128 = bitbuffer_read128(bitbuffer, bits128[ival + 2]);
			EXPECT_TRUE(uint128_equal(readval128, writeval128));
			writeval128 = val128[3];
			writeval128.word[0] &=
			    (bits128[ival + 3] >= 64 ? (uint64_t)-1 : (bits128[ival + 3] ? (1ULL << bits128[ival + 3]) - 1 : 0));
			writeval128.word[1] &=
			    (bits128[ival + 3] == 128 ? (uint64_t)-1 :
			                                (bits128[ival + 3] > 64 ? (1ULL << (bits128[ival + 3] - 64)) - 1 : 0));
			readval128 = bitbuffer_read128(bitbuffer, bits128[ival + 3]);
			EXPECT_TRUE(uint128_equal(readval128, writeval128));

			convread32.fval = bitbuffer_read_float32(bitbuffer);
			convwant32.fval = valf32[0];
			EXPECT_EQ(convread32.uival, convwant32.uival);

			convread32.fval = bitbuffer_read_float32(bitbuffer);
			convwant32.fval = valf32[1];
			EXPECT_EQ(convread32.uival, convwant32.uival);

			convread32.fval = bitbuffer_read_float32(bitbuffer);
			convwant32.fval = valf32[2];
			EXPECT_EQ(convread32.uival, convwant32.uival);

			convread32.fval = bitbuffer_read_float32(bitbuffer);
			convwant32.fval = valf32[3];
			EXPECT_EQ(convread32.uival, convwant32.uival);

			convread64.fval = bitbuffer_read_float64(bitbuffer);
			convwant64.fval = valf64[0];
			EXPECT_EQ(convread64.uival, convwant64.uival);

			convread64.fval = bitbuffer_read_float64(bitbuffer);
			convwant64.fval = valf64[1];
			EXPECT_EQ(convread64.uival, convwant64.uival);

			convread64.fval = bitbuffer_read_float64(bitbuffer);
			convwant64.fval = valf64[2];
			EXPECT_EQ(convread64.uival, convwant64.uival);

			convread64.fval = bitbuffer_read_float64(bitbuffer);
			convwant64.fval = valf64[3];
			EXPECT_EQ(convread64.uival, convwant64.uival);
		}

		bitbuffer_align_read(bitbuffer, false);

		// Validate oversize writes get clamped
		EXPECT_EQ(bitbuffer_read32(bitbuffer, 64), val32[0]);
		EXPECT_EQ(bitbuffer_read64(bitbuffer, 128), val64[0]);
		EXPECT_TRUE(uint128_equal(bitbuffer_read128(bitbuffer, 256), val128[0]));

		bitbuffer_deallocate(bitbuffer);
	}

	return 0;
}

DECLARE_TEST(bitbuffer, stream) {
	int ipass, ival;
	bitbuffer_t* bitbuffer;
	stream_t* stream;
	uint128_t readval128, writeval128;
	float32_cast_t convread32, convwant32;
	float64_cast_t convread64, convwant64;

	uint32_t val32[4] = {random32(), random32(), random32(), random32()};
	uint64_t val64[4] = {random64(), random64(), random64(), random64()};
	uint128_t val128[4];
	float32_t valf32[4] = {(float32_t)random_normalized(), (float32_t)random_normalized(),
	                       (float32_t)random_normalized(), (float32_t)random_normalized()};
	float64_t valf64[4] = {(float64_t)random_normalized(), (float64_t)random_normalized(),
	                       (float64_t)random_normalized(), (float64_t)random_normalized()};

	unsigned int bits32[20] = {0};
	unsigned int bits64[20] = {0};
	unsigned int bits128[20] = {0};

	string_t tmp_path;

	for (ival = 0; ival < 4; ++ival)
		val128[ival] = uint128_make(random64(), random64());

	for (ipass = 0; ipass < 1024; ++ipass) {
		for (ival = 0; ival < 20; ++ival) {
			bits32[ival] = random32_range(0, 33);
			bits64[ival] = random32_range(0, 65);
			bits128[ival] = random32_range(0, 129);
		}

		// Phase 1 - write data
		stream = fs_temporary_file();
		EXPECT_NE(stream, 0);

		tmp_path = string_clone(STRING_ARGS(stream_path(stream)));

		bitbuffer = bitbuffer_allocate_stream(stream);

		for (ival = 0; ival < 16; ++ival) {
			bitbuffer_write32(bitbuffer, val32[0], bits32[ival]);
			bitbuffer_write32(bitbuffer, val32[1], bits32[ival + 1]);
			bitbuffer_write32(bitbuffer, val32[2], bits32[ival + 2]);
			bitbuffer_write32(bitbuffer, val32[3], bits32[ival + 3]);

			bitbuffer_write64(bitbuffer, val64[0], bits64[ival]);
			bitbuffer_write64(bitbuffer, val64[1], bits64[ival + 1]);
			bitbuffer_write64(bitbuffer, val64[2], bits64[ival + 2]);
			bitbuffer_write64(bitbuffer, val64[3], bits64[ival + 3]);

			bitbuffer_write128(bitbuffer, val128[0], bits128[ival]);
			bitbuffer_write128(bitbuffer, val128[1], bits128[ival + 1]);
			bitbuffer_write128(bitbuffer, val128[2], bits128[ival + 2]);
			bitbuffer_write128(bitbuffer, val128[3], bits128[ival + 3]);

			bitbuffer_write_float32(bitbuffer, valf32[0]);
			bitbuffer_write_float32(bitbuffer, valf32[1]);
			bitbuffer_write_float32(bitbuffer, valf32[2]);
			bitbuffer_write_float32(bitbuffer, valf32[3]);

			bitbuffer_write_float64(bitbuffer, valf64[0]);
			bitbuffer_write_float64(bitbuffer, valf64[1]);
			bitbuffer_write_float64(bitbuffer, valf64[2]);
			bitbuffer_write_float64(bitbuffer, valf64[3]);
		}

		bitbuffer_align_write(bitbuffer, false);
		bitbuffer_write64(bitbuffer, 0, 63);

		bitbuffer_finalize(bitbuffer);

		// Phase 2 - read and verify data
		stream_seek(stream, 0, STREAM_SEEK_BEGIN);
		bitbuffer_initialize_stream(bitbuffer, stream);

		for (ival = 0; ival < 16; ++ival) {
			EXPECT_EQ(bitbuffer_read32(bitbuffer, bits32[ival]),
			          val32[0] & (bits32[ival] == 32 ? (uint32_t)-1 : (bits32[ival] ? (1U << bits32[ival]) - 1 : 0U)));
			EXPECT_EQ(bitbuffer_read32(bitbuffer, bits32[ival + 1]),
			          val32[1] & (bits32[ival + 1] == 32 ? (uint32_t)-1 :
			                                               (bits32[ival + 1] ? (1U << bits32[ival + 1]) - 1 : 0U)));
			EXPECT_EQ(bitbuffer_read32(bitbuffer, bits32[ival + 2]),
			          val32[2] & (bits32[ival + 2] == 32 ? (uint32_t)-1 :
			                                               (bits32[ival + 2] ? (1U << bits32[ival + 2]) - 1 : 0U)));
			EXPECT_EQ(bitbuffer_read32(bitbuffer, bits32[ival + 3]),
			          val32[3] & (bits32[ival + 3] == 32 ? (uint32_t)-1 :
			                                               (bits32[ival + 3] ? (1U << bits32[ival + 3]) - 1 : 0U)));

			EXPECT_EQ(bitbuffer_read64(bitbuffer, bits64[ival]),
			          val64[0] & (bits64[ival] == 64 ? (uint64_t)-1 : (bits64[ival] ? (1ULL << bits64[ival]) - 1 : 0)));
			EXPECT_EQ(bitbuffer_read64(bitbuffer, bits64[ival + 1]),
			          val64[1] & (bits64[ival + 1] == 64 ? (uint64_t)-1 :
			                                               (bits64[ival + 1] ? (1ULL << bits64[ival + 1]) - 1 : 0)));
			EXPECT_EQ(bitbuffer_read64(bitbuffer, bits64[ival + 2]),
			          val64[2] & (bits64[ival + 2] == 64 ? (uint64_t)-1 :
			                                               (bits64[ival + 2] ? (1ULL << bits64[ival + 2]) - 1 : 0)));
			EXPECT_EQ(bitbuffer_read64(bitbuffer, bits64[ival + 3]),
			          val64[3] & (bits64[ival + 3] == 64 ? (uint64_t)-1 :
			                                               (bits64[ival + 3] ? (1ULL << bits64[ival + 3]) - 1 : 0)));

			writeval128 = val128[0];
			writeval128.word[0] &=
			    (bits128[ival] >= 64 ? (uint64_t)-1 : (bits128[ival] ? (1ULL << bits128[ival]) - 1 : 0));
			writeval128.word[1] &=
			    (bits128[ival] == 128 ? (uint64_t)-1 : (bits128[ival] > 64 ? (1ULL << (bits128[ival] - 64)) - 1 : 0));
			readval128 = bitbuffer_read128(bitbuffer, bits128[ival]);
			EXPECT_TRUE(uint128_equal(readval128, writeval128));
			writeval128 = val128[1];
			writeval128.word[0] &=
			    (bits128[ival + 1] >= 64 ? (uint64_t)-1 : (bits128[ival + 1] ? (1ULL << bits128[ival + 1]) - 1 : 0));
			writeval128.word[1] &=
			    (bits128[ival + 1] == 128 ? (uint64_t)-1 :
			                                (bits128[ival + 1] > 64 ? (1ULL << (bits128[ival + 1] - 64)) - 1 : 0));
			readval128 = bitbuffer_read128(bitbuffer, bits128[ival + 1]);
			EXPECT_TRUE(uint128_equal(readval128, writeval128));
			writeval128 = val128[2];
			writeval128.word[0] &=
			    (bits128[ival + 2] >= 64 ? (uint64_t)-1 : (bits128[ival + 2] ? (1ULL << bits128[ival + 2]) - 1 : 0));
			writeval128.word[1] &=
			    (bits128[ival + 2] == 128 ? (uint64_t)-1 :
			                                (bits128[ival + 2] > 64 ? (1ULL << (bits128[ival + 2] - 64)) - 1 : 0));
			readval128 = bitbuffer_read128(bitbuffer, bits128[ival + 2]);
			EXPECT_TRUE(uint128_equal(readval128, writeval128));
			writeval128 = val128[3];
			writeval128.word[0] &=
			    (bits128[ival + 3] >= 64 ? (uint64_t)-1 : (bits128[ival + 3] ? (1ULL << bits128[ival + 3]) - 1 : 0));
			writeval128.word[1] &=
			    (bits128[ival + 3] == 128 ? (uint64_t)-1 :
			                                (bits128[ival + 3] > 64 ? (1ULL << (bits128[ival + 3] - 64)) - 1 : 0));
			readval128 = bitbuffer_read128(bitbuffer, bits128[ival + 3]);
			EXPECT_TRUE(uint128_equal(readval128, writeval128));

			convread32.fval = bitbuffer_read_float32(bitbuffer);
			convwant32.fval = valf32[0];
			EXPECT_EQ(convread32.uival, convwant32.uival);

			convread32.fval = bitbuffer_read_float32(bitbuffer);
			convwant32.fval = valf32[1];
			EXPECT_EQ(convread32.uival, convwant32.uival);

			convread32.fval = bitbuffer_read_float32(bitbuffer);
			convwant32.fval = valf32[2];
			EXPECT_EQ(convread32.uival, convwant32.uival);

			convread32.fval = bitbuffer_read_float32(bitbuffer);
			convwant32.fval = valf32[3];
			EXPECT_EQ(convread32.uival, convwant32.uival);

			convread64.fval = bitbuffer_read_float64(bitbuffer);
			convwant64.fval = valf64[0];
			EXPECT_EQ(convread64.uival, convwant64.uival);

			convread64.fval = bitbuffer_read_float64(bitbuffer);
			convwant64.fval = valf64[1];
			EXPECT_EQ(convread64.uival, convwant64.uival);

			convread64.fval = bitbuffer_read_float64(bitbuffer);
			convwant64.fval = valf64[2];
			EXPECT_EQ(convread64.uival, convwant64.uival);

			convread64.fval = bitbuffer_read_float64(bitbuffer);
			convwant64.fval = valf64[3];
			EXPECT_EQ(convread64.uival, convwant64.uival);
		}

		bitbuffer_deallocate(bitbuffer);
		stream_deallocate(stream);
		fs_remove_file(STRING_ARGS(tmp_path));
		string_deallocate(tmp_path.str);
	}

	return 0;
}

static void
test_bitbuffer_declare(void) {
	ADD_TEST(bitbuffer, basics);
	ADD_TEST(bitbuffer, readwrite);
	ADD_TEST(bitbuffer, readwriteswap);
	ADD_TEST(bitbuffer, stream);
}

static test_suite_t test_bitbuffer_suite = {test_bitbuffer_application,
                                            test_bitbuffer_memory_system,
                                            test_bitbuffer_config,
                                            test_bitbuffer_declare,
                                            test_bitbuffer_initialize,
                                            test_bitbuffer_finalize,
                                            0};

#if BUILD_MONOLITHIC

int
test_bitbuffer_run(void);

int
test_bitbuffer_run(void) {
	test_suite = test_bitbuffer_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_bitbuffer_suite;
}

#endif
