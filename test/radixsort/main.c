/* main.c  -  Foundation radixsort test  -  Public Domain  -  2013 Mattias Jansson
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
test_radixsort_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation radixsort tests"));
	app.short_name = string_const(STRING_CONST("test_radixsort"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_radixsort_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_radixsort_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_radixsort_initialize(void) {
	return 0;
}

static void
test_radixsort_finalize(void) {
}

DECLARE_TEST(radixsort, allocation) {
	radixsort_t* sort_none = radixsort_allocate(RADIXSORT_INT32, 0);
	radixsort_t* sort_small = radixsort_allocate(RADIXSORT_INT32, 128);
	radixsort_t* sort_medium = radixsort_allocate(RADIXSORT_INT32, 0xFFFF);
	radixsort_t* sort_large = radixsort_allocate(RADIXSORT_INT32, 0xFFFFFF);

	EXPECT_NE(sort_none, 0);
	EXPECT_NE(sort_small, 0);
	EXPECT_NE(sort_medium, 0);
	EXPECT_NE(sort_large, 0);

	radixsort_deallocate(sort_none);
	radixsort_deallocate(sort_small);
	radixsort_deallocate(sort_medium);
	radixsort_deallocate(sort_large);

	return 0;
}

DECLARE_TEST(radixsort, sort_int32_index16) {
	int bits = 0;
	int32_t* arr_int;
	uint32_t* arr_uint;
	uint32_t* int_index_count;
	uint32_t* uint_index_count;
	radixsort_t* sort_int;
	radixsort_t* sort_uint;

	sort_int = radixsort_allocate(RADIXSORT_INT32, 0xFFFF);
	sort_uint = radixsort_allocate(RADIXSORT_UINT32, 0xFFFF);

	EXPECT_EQ(sort_int->indextype, RADIXSORT_INDEX16);
	EXPECT_EQ(sort_uint->indextype, RADIXSORT_INDEX16);

	for (bits = 1; bits <= 16; ++bits) {
		uint16_t ival;
		uint16_t num = (uint16_t)((1ULL << (uint64_t)bits) - 1);
		const uint16_t* FOUNDATION_RESTRICT sindex_int;
		const uint16_t* FOUNDATION_RESTRICT sindex_uint;

		arr_int = memory_allocate(0, sizeof(int32_t) * num, 0, MEMORY_PERSISTENT);
		arr_uint = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT);

		for (ival = 0; ival < num; ++ival) {
			arr_int[ival] = (int32_t)random32();
			arr_uint[ival] = random32();
		}

		sindex_int = radixsort_sort(sort_int, arr_int, num);
		sindex_uint = radixsort_sort(sort_uint, arr_uint, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_int[0]];
		++uint_index_count[sindex_uint[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_int[0], 0);
			EXPECT_EQ(sindex_uint[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_int[ival]];
				++uint_index_count[sindex_uint[ival]];

				EXPECT_LE(arr_int[sindex_int[ival - 1]], arr_int[sindex_int[ival]]);
				EXPECT_LE(arr_uint[sindex_uint[ival - 1]], arr_uint[sindex_uint[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_int);
		memory_deallocate(arr_uint);
	}

	for (--bits; bits >= 1; --bits) {
		uint16_t ival;
		uint16_t num = (uint16_t)((1ULL << (uint64_t)bits) - 1);
		const uint16_t* FOUNDATION_RESTRICT sindex_int;
		const uint16_t* FOUNDATION_RESTRICT sindex_uint;

		arr_int = memory_allocate(0, sizeof(int32_t) * num, 0, MEMORY_PERSISTENT);
		arr_uint = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT);

		for (ival = 0; ival < num; ++ival) {
			arr_int[ival] = (int32_t)random32();
			arr_uint[ival] = random32();
		}

		sindex_int = radixsort_sort(sort_int, arr_int, num);
		sindex_uint = radixsort_sort(sort_uint, arr_uint, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_int[0]];
		++uint_index_count[sindex_uint[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_int[0], 0);
			EXPECT_EQ(sindex_uint[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_int[ival]];
				++uint_index_count[sindex_uint[ival]];

				EXPECT_LE(arr_int[sindex_int[ival - 1]], arr_int[sindex_int[ival]]);
				EXPECT_LE(arr_uint[sindex_uint[ival - 1]], arr_uint[sindex_uint[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_int);
		memory_deallocate(arr_uint);
	}

	radixsort_deallocate(sort_int);
	radixsort_deallocate(sort_uint);

	return 0;
}

DECLARE_TEST(radixsort, sort_int32_index32) {
	int bits = 0;
	int32_t* arr_int;
	uint32_t* arr_uint;
	uint32_t* int_index_count;
	uint32_t* uint_index_count;
	radixsort_t* sort_int;
	radixsort_t* sort_uint;

	sort_int = radixsort_allocate(RADIXSORT_INT32, (size_t)((1ULL << 22ULL) - 1));
	sort_uint = radixsort_allocate(RADIXSORT_UINT32, (size_t)((1ULL << 22ULL) - 1));

	EXPECT_EQ(sort_int->indextype, RADIXSORT_INDEX32);
	EXPECT_EQ(sort_uint->indextype, RADIXSORT_INDEX32);

	for (bits = 1; bits <= 22; ++bits) {
		uint32_t ival;
		uint32_t num = (uint32_t)((1ULL << (uint64_t)bits) - 1);
		const uint32_t* FOUNDATION_RESTRICT sindex_int;
		const uint32_t* FOUNDATION_RESTRICT sindex_uint;

		arr_int = memory_allocate(0, sizeof(int32_t) * num, 0, MEMORY_PERSISTENT);
		arr_uint = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT);

		for (ival = 0; ival < num; ++ival) {
			arr_int[ival] = (int32_t)random32();
			arr_uint[ival] = random32();
		}

		sindex_int = radixsort_sort(sort_int, arr_int, num);
		sindex_uint = radixsort_sort(sort_uint, arr_uint, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_int[0]];
		++uint_index_count[sindex_uint[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_int[0], 0);
			EXPECT_EQ(sindex_uint[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_int[ival]];
				++uint_index_count[sindex_uint[ival]];

				EXPECT_LE(arr_int[sindex_int[ival - 1]], arr_int[sindex_int[ival]]);
				EXPECT_LE(arr_uint[sindex_uint[ival - 1]], arr_uint[sindex_uint[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_int);
		memory_deallocate(arr_uint);
	}

	for (--bits; bits >= 1; --bits) {
		uint32_t ival;
		uint32_t num = (uint32_t)((1ULL << (uint64_t)bits) - 1);
		const uint32_t* FOUNDATION_RESTRICT sindex_int;
		const uint32_t* FOUNDATION_RESTRICT sindex_uint;

		arr_int = memory_allocate(0, sizeof(int32_t) * num, 0, MEMORY_PERSISTENT);
		arr_uint = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT);

		for (ival = 0; ival < num; ++ival) {
			arr_int[ival] = (int32_t)random32();
			arr_uint[ival] = random32();
		}

		sindex_int = radixsort_sort(sort_int, arr_int, num);
		sindex_uint = radixsort_sort(sort_uint, arr_uint, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_int[0]];
		++uint_index_count[sindex_uint[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_int[0], 0);
			EXPECT_EQ(sindex_uint[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_int[ival]];
				++uint_index_count[sindex_uint[ival]];

				EXPECT_LE(arr_int[sindex_int[ival - 1]], arr_int[sindex_int[ival]]);
				EXPECT_LE(arr_uint[sindex_uint[ival - 1]], arr_uint[sindex_uint[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_int);
		memory_deallocate(arr_uint);
	}

	radixsort_deallocate(sort_int);
	radixsort_deallocate(sort_uint);

	return 0;
}

DECLARE_TEST(radixsort, sort_int64_index16) {
	int bits = 0;
	int64_t* arr_int;
	uint64_t* arr_uint;
	uint32_t* int_index_count;
	uint32_t* uint_index_count;
	radixsort_t* sort_int;
	radixsort_t* sort_uint;

	sort_int = radixsort_allocate(RADIXSORT_INT64, 0xFFFF);
	sort_uint = radixsort_allocate(RADIXSORT_UINT64, 0xFFFF);

	EXPECT_EQ(sort_int->indextype, RADIXSORT_INDEX16);
	EXPECT_EQ(sort_uint->indextype, RADIXSORT_INDEX16);

	for (bits = 1; bits <= 16; ++bits) {
		uint16_t ival;
		uint16_t num = (uint16_t)((1ULL << (uint64_t)bits) - 1);
		const uint16_t* FOUNDATION_RESTRICT sindex_int;
		const uint16_t* FOUNDATION_RESTRICT sindex_uint;

		arr_int = memory_allocate(0, sizeof(int64_t) * num, 0, MEMORY_PERSISTENT);
		arr_uint = memory_allocate(0, sizeof(uint64_t) * num, 0, MEMORY_PERSISTENT);

		for (ival = 0; ival < num; ++ival) {
			arr_int[ival] = (int64_t)random64();
			arr_uint[ival] = random64();
		}

		sindex_int = radixsort_sort(sort_int, arr_int, num);
		sindex_uint = radixsort_sort(sort_uint, arr_uint, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_int[0]];
		++uint_index_count[sindex_uint[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_int[0], 0);
			EXPECT_EQ(sindex_uint[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_int[ival]];
				++uint_index_count[sindex_uint[ival]];

				EXPECT_LE(arr_int[sindex_int[ival - 1]], arr_int[sindex_int[ival]]);
				EXPECT_LE(arr_uint[sindex_uint[ival - 1]], arr_uint[sindex_uint[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_int);
		memory_deallocate(arr_uint);
	}

	for (--bits; bits >= 1; --bits) {
		uint16_t ival;
		uint16_t num = (uint16_t)((1ULL << (uint64_t)bits) - 1);
		const uint16_t* FOUNDATION_RESTRICT sindex_int;
		const uint16_t* FOUNDATION_RESTRICT sindex_uint;

		arr_int = memory_allocate(0, sizeof(int64_t) * num, 0, MEMORY_PERSISTENT);
		arr_uint = memory_allocate(0, sizeof(uint64_t) * num, 0, MEMORY_PERSISTENT);

		for (ival = 0; ival < num; ++ival) {
			arr_int[ival] = (int64_t)random64();
			arr_uint[ival] = random64();
		}

		sindex_int = radixsort_sort(sort_int, arr_int, num);
		sindex_uint = radixsort_sort(sort_uint, arr_uint, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_int[0]];
		++uint_index_count[sindex_uint[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_int[0], 0);
			EXPECT_EQ(sindex_uint[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_int[ival]];
				++uint_index_count[sindex_uint[ival]];

				EXPECT_LE(arr_int[sindex_int[ival - 1]], arr_int[sindex_int[ival]]);
				EXPECT_LE(arr_uint[sindex_uint[ival - 1]], arr_uint[sindex_uint[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_int);
		memory_deallocate(arr_uint);
	}

	radixsort_deallocate(sort_int);
	radixsort_deallocate(sort_uint);

	return 0;
}

DECLARE_TEST(radixsort, sort_int64_index32) {
	int bits = 0;
	int64_t* arr_int;
	uint64_t* arr_uint;
	uint32_t* int_index_count;
	uint32_t* uint_index_count;
	radixsort_t* sort_int;
	radixsort_t* sort_uint;

	sort_int = radixsort_allocate(RADIXSORT_INT64, (size_t)((1ULL << 22ULL) - 1));
	sort_uint = radixsort_allocate(RADIXSORT_UINT64, (size_t)((1ULL << 22ULL) - 1));

	EXPECT_EQ(sort_int->indextype, RADIXSORT_INDEX32);
	EXPECT_EQ(sort_uint->indextype, RADIXSORT_INDEX32);

	for (bits = 1; bits <= 22; ++bits) {
		uint32_t ival;
		uint32_t num = (uint32_t)((1ULL << (uint64_t)bits) - 1);
		const uint32_t* FOUNDATION_RESTRICT sindex_int;
		const uint32_t* FOUNDATION_RESTRICT sindex_uint;

		arr_int = memory_allocate(0, sizeof(int64_t) * num, 0, MEMORY_PERSISTENT);
		arr_uint = memory_allocate(0, sizeof(uint64_t) * num, 0, MEMORY_PERSISTENT);

		for (ival = 0; ival < num; ++ival) {
			arr_int[ival] = (int64_t)random64();
			arr_uint[ival] = random64();
		}

		sindex_int = radixsort_sort(sort_int, arr_int, num);
		sindex_uint = radixsort_sort(sort_uint, arr_uint, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_int[0]];
		++uint_index_count[sindex_uint[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_int[0], 0);
			EXPECT_EQ(sindex_uint[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_int[ival]];
				++uint_index_count[sindex_uint[ival]];

				EXPECT_LE(arr_int[sindex_int[ival - 1]], arr_int[sindex_int[ival]]);
				EXPECT_LE(arr_uint[sindex_uint[ival - 1]], arr_uint[sindex_uint[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_int);
		memory_deallocate(arr_uint);
	}

	for (--bits; bits >= 1; --bits) {
		uint32_t ival;
		uint32_t num = (uint32_t)((1ULL << (uint64_t)bits) - 1);
		const uint32_t* FOUNDATION_RESTRICT sindex_int;
		const uint32_t* FOUNDATION_RESTRICT sindex_uint;

		arr_int = memory_allocate(0, sizeof(int64_t) * num, 0, MEMORY_PERSISTENT);
		arr_uint = memory_allocate(0, sizeof(uint64_t) * num, 0, MEMORY_PERSISTENT);

		for (ival = 0; ival < num; ++ival) {
			arr_int[ival] = (int64_t)random64();
			arr_uint[ival] = random64();
		}

		sindex_int = radixsort_sort(sort_int, arr_int, num);
		sindex_uint = radixsort_sort(sort_uint, arr_uint, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_int[0]];
		++uint_index_count[sindex_uint[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_int[0], 0);
			EXPECT_EQ(sindex_uint[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_int[ival]];
				++uint_index_count[sindex_uint[ival]];

				EXPECT_LE(arr_int[sindex_int[ival - 1]], arr_int[sindex_int[ival]]);
				EXPECT_LE(arr_uint[sindex_uint[ival - 1]], arr_uint[sindex_uint[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_int);
		memory_deallocate(arr_uint);
	}

	radixsort_deallocate(sort_int);
	radixsort_deallocate(sort_uint);

	return 0;
}

DECLARE_TEST(radixsort, sort_real_index16) {
	int bits = 0;
	float32_t* arr_32;
	float64_t* arr_64;
	uint32_t* int_index_count;
	uint32_t* uint_index_count;
	radixsort_t* sort_32;
	radixsort_t* sort_64;
	real low_range = -(real)(1 << 30);
	real high_range = (real)(1 << 30);

	sort_32 = radixsort_allocate(RADIXSORT_FLOAT32, 0xFFFF);
	sort_64 = radixsort_allocate(RADIXSORT_FLOAT64, 0xFFFF);

	EXPECT_EQ(sort_32->indextype, RADIXSORT_INDEX16);
	EXPECT_EQ(sort_64->indextype, RADIXSORT_INDEX16);

	for (bits = 1; bits <= 16; ++bits) {
		uint16_t ival;
		uint16_t num = (uint16_t)((1ULL << (uint64_t)bits) - 1);
		const uint16_t* FOUNDATION_RESTRICT sindex_32;
		const uint16_t* FOUNDATION_RESTRICT sindex_64;

		arr_32 = memory_allocate(0, sizeof(float32_t) * num, 0, MEMORY_PERSISTENT);
		arr_64 = memory_allocate(0, sizeof(float64_t) * num, 0, MEMORY_PERSISTENT);

		// Mixed neg/pos
		for (ival = 0; ival < num; ++ival) {
			arr_32[ival] = (float32_t)random_range(low_range, high_range);
			arr_64[ival] = random_range(low_range, high_range);
		}

		sindex_32 = radixsort_sort(sort_32, arr_32, num);
		sindex_64 = radixsort_sort(sort_64, arr_64, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_32[0]];
		++uint_index_count[sindex_64[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_32[0], 0);
			EXPECT_EQ(sindex_64[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_32[ival]];
				++uint_index_count[sindex_64[ival]];

				EXPECT_LE(arr_32[sindex_32[ival - 1]], arr_32[sindex_32[ival]]);
				EXPECT_LE(arr_64[sindex_64[ival - 1]], arr_64[sindex_64[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		// Only neg
		for (ival = 0; ival < num; ++ival) {
			arr_32[ival] = (float32_t)random_range(low_range, -1.0f);
			arr_64[ival] = random_range(low_range, -1.0f);
		}

		sindex_32 = radixsort_sort(sort_32, arr_32, num);
		sindex_64 = radixsort_sort(sort_64, arr_64, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_32[0]];
		++uint_index_count[sindex_64[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_32[0], 0);
			EXPECT_EQ(sindex_64[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_32[ival]];
				++uint_index_count[sindex_64[ival]];

				EXPECT_LE(arr_32[sindex_32[ival - 1]], arr_32[sindex_32[ival]]);
				EXPECT_LE(arr_64[sindex_64[ival - 1]], arr_64[sindex_64[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_32);
		memory_deallocate(arr_64);
	}

	for (--bits; bits >= 1; --bits) {
		uint16_t ival;
		uint16_t num = (uint16_t)((1ULL << (uint64_t)bits) - 1);
		const uint16_t* FOUNDATION_RESTRICT sindex_32;
		const uint16_t* FOUNDATION_RESTRICT sindex_64;

		arr_32 = memory_allocate(0, sizeof(float32_t) * num, 0, MEMORY_PERSISTENT);
		arr_64 = memory_allocate(0, sizeof(float64_t) * num, 0, MEMORY_PERSISTENT);

		// Mixed neg/pos
		for (ival = 0; ival < num; ++ival) {
			arr_32[ival] = (float32_t)random_range(low_range, high_range);
			arr_64[ival] = random_range(low_range, high_range);
		}

		sindex_32 = radixsort_sort(sort_32, arr_32, num);
		sindex_64 = radixsort_sort(sort_64, arr_64, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_32[0]];
		++uint_index_count[sindex_64[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_32[0], 0);
			EXPECT_EQ(sindex_64[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_32[ival]];
				++uint_index_count[sindex_64[ival]];

				EXPECT_LE(arr_32[sindex_32[ival - 1]], arr_32[sindex_32[ival]]);
				EXPECT_LE(arr_64[sindex_64[ival - 1]], arr_64[sindex_64[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		// Only neg
		for (ival = 0; ival < num; ++ival) {
			arr_32[ival] = (float32_t)random_range(low_range, -1.0f);
			arr_64[ival] = random_range(low_range, -1.0f);
		}

		sindex_32 = radixsort_sort(sort_32, arr_32, num);
		sindex_64 = radixsort_sort(sort_64, arr_64, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_32[0]];
		++uint_index_count[sindex_64[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_32[0], 0);
			EXPECT_EQ(sindex_64[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_32[ival]];
				++uint_index_count[sindex_64[ival]];

				EXPECT_LE(arr_32[sindex_32[ival - 1]], arr_32[sindex_32[ival]]);
				EXPECT_LE(arr_64[sindex_64[ival - 1]], arr_64[sindex_64[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_32);
		memory_deallocate(arr_64);
	}

	radixsort_deallocate(sort_32);
	radixsort_deallocate(sort_64);

	return 0;
}

DECLARE_TEST(radixsort, sort_real_index32) {
	int bits = 0;
	float32_t* arr_32;
	float64_t* arr_64;
	uint32_t* int_index_count;
	uint32_t* uint_index_count;
	radixsort_t* sort_32;
	radixsort_t* sort_64;
	real low_range = -(real)(1 << 30);
	real high_range = (real)(1 << 30);

	sort_32 = radixsort_allocate(RADIXSORT_FLOAT32, (size_t)((1ULL << 22ULL) - 1));
	sort_64 = radixsort_allocate(RADIXSORT_FLOAT64, (size_t)((1ULL << 22ULL) - 1));

	EXPECT_EQ(sort_32->indextype, RADIXSORT_INDEX32);
	EXPECT_EQ(sort_64->indextype, RADIXSORT_INDEX32);

	for (bits = 1; bits <= 22; ++bits) {
		uint32_t ival;
		uint32_t num = (uint32_t)((1ULL << (uint64_t)bits) - 1);
		const uint32_t* FOUNDATION_RESTRICT sindex_32;
		const uint32_t* FOUNDATION_RESTRICT sindex_64;

		arr_32 = memory_allocate(0, sizeof(float32_t) * num, 0, MEMORY_PERSISTENT);
		arr_64 = memory_allocate(0, sizeof(float64_t) * num, 0, MEMORY_PERSISTENT);

		// Mixed neg/pos
		for (ival = 0; ival < num; ++ival) {
			arr_32[ival] = (float32_t)random_range(low_range, high_range);
			arr_64[ival] = random_range(low_range, high_range);
		}

		sindex_32 = radixsort_sort(sort_32, arr_32, num);
		sindex_64 = radixsort_sort(sort_64, arr_64, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_32[0]];
		++uint_index_count[sindex_64[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_32[0], 0);
			EXPECT_EQ(sindex_64[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_32[ival]];
				++uint_index_count[sindex_64[ival]];

				EXPECT_LE(arr_32[sindex_32[ival - 1]], arr_32[sindex_32[ival]]);
				EXPECT_LE(arr_64[sindex_64[ival - 1]], arr_64[sindex_64[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		// Only neg
		for (ival = 0; ival < num; ++ival) {
			arr_32[ival] = (float32_t)random_range(low_range, -1.0f);
			arr_64[ival] = random_range(low_range, -1.0f);
		}

		sindex_32 = radixsort_sort(sort_32, arr_32, num);
		sindex_64 = radixsort_sort(sort_64, arr_64, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_32[0]];
		++uint_index_count[sindex_64[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_32[0], 0);
			EXPECT_EQ(sindex_64[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_32[ival]];
				++uint_index_count[sindex_64[ival]];

				EXPECT_LE(arr_32[sindex_32[ival - 1]], arr_32[sindex_32[ival]]);
				EXPECT_LE(arr_64[sindex_64[ival - 1]], arr_64[sindex_64[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_32);
		memory_deallocate(arr_64);
	}

	for (--bits; bits >= 1; --bits) {
		uint32_t ival;
		uint32_t num = (uint32_t)((1ULL << (uint64_t)bits) - 1);
		const uint32_t* FOUNDATION_RESTRICT sindex_32;
		const uint32_t* FOUNDATION_RESTRICT sindex_64;

		arr_32 = memory_allocate(0, sizeof(float32_t) * num, 0, MEMORY_PERSISTENT);
		arr_64 = memory_allocate(0, sizeof(float64_t) * num, 0, MEMORY_PERSISTENT);

		// Mixed neg/pos
		for (ival = 0; ival < num; ++ival) {
			arr_32[ival] = (float32_t)random_range(low_range, high_range);
			arr_64[ival] = random_range(low_range, high_range);
		}

		sindex_32 = radixsort_sort(sort_32, arr_32, num);
		sindex_64 = radixsort_sort(sort_64, arr_64, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_32[0]];
		++uint_index_count[sindex_64[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_32[0], 0);
			EXPECT_EQ(sindex_64[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_32[ival]];
				++uint_index_count[sindex_64[ival]];

				EXPECT_LE(arr_32[sindex_32[ival - 1]], arr_32[sindex_32[ival]]);
				EXPECT_LE(arr_64[sindex_64[ival - 1]], arr_64[sindex_64[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		// Only neg
		for (ival = 0; ival < num; ++ival) {
			arr_32[ival] = (float32_t)random_range(low_range, -1.0f);
			arr_64[ival] = random_range(low_range, -1.0f);
		}

		sindex_32 = radixsort_sort(sort_32, arr_32, num);
		sindex_64 = radixsort_sort(sort_64, arr_64, num);

		int_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++int_index_count[sindex_32[0]];
		++uint_index_count[sindex_64[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_32[0], 0);
			EXPECT_EQ(sindex_64[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++int_index_count[sindex_32[ival]];
				++uint_index_count[sindex_64[ival]];

				EXPECT_LE(arr_32[sindex_32[ival - 1]], arr_32[sindex_32[ival]]);
				EXPECT_LE(arr_64[sindex_64[ival - 1]], arr_64[sindex_64[ival]]);
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(int_index_count[ival], 1);
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(int_index_count);
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_32);
		memory_deallocate(arr_64);
	}

	radixsort_deallocate(sort_32);
	radixsort_deallocate(sort_64);

	return 0;
}

DECLARE_TEST(radixsort, sort_custom_index16) {
	int bits = 0;
	uint256_t* arr_uint;
	uint32_t* uint_index_count;
	radixsort_t* sort_uint;

	sort_uint = radixsort_allocate_custom(32, 0xFFFF);

	EXPECT_EQ(sort_uint->indextype, RADIXSORT_INDEX16);

	for (bits = 1; bits <= 16; ++bits) {
		uint32_t ival;
		uint32_t num = (uint32_t)((1ULL << (uint64_t)bits) - 1);
		const uint16_t* FOUNDATION_RESTRICT sindex_uint;

		arr_uint = memory_allocate(0, sizeof(uint256_t) * num, 0, MEMORY_PERSISTENT);

		for (ival = 0; ival < num; ++ival)
			arr_uint[ival] = uint256_make(random64(), random64(), random64(), random64());

		sindex_uint = radixsort_sort(sort_uint, arr_uint, num);

		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++uint_index_count[sindex_uint[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_uint[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++uint_index_count[sindex_uint[ival]];

				for (uint iword = 4; iword > 0; ++iword) {
					bool less_than =
					    (arr_uint[sindex_uint[ival - 1]].word[iword - 1] < arr_uint[sindex_uint[ival]].word[iword - 1]);
					if (less_than)
						break;
					EXPECT_EQ(arr_uint[sindex_uint[ival - 1]].word[iword - 1],
					          arr_uint[sindex_uint[ival]].word[iword - 1]);
				}
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_uint);
	}

	for (--bits; bits >= 1; --bits) {
		uint32_t ival;
		uint32_t num = (uint32_t)((1ULL << (uint64_t)bits) - 1);
		const uint16_t* FOUNDATION_RESTRICT sindex_uint;

		arr_uint = memory_allocate(0, sizeof(uint256_t) * num, 0, MEMORY_PERSISTENT);

		for (ival = 0; ival < num; ++ival)
			arr_uint[ival] = uint256_make(random64(), random64(), random64(), random64());

		sindex_uint = radixsort_sort(sort_uint, arr_uint, num);

		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++uint_index_count[sindex_uint[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_uint[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++uint_index_count[sindex_uint[ival]];

				for (uint iword = 4; iword > 0; ++iword) {
					bool less_than =
					    (arr_uint[sindex_uint[ival - 1]].word[iword - 1] < arr_uint[sindex_uint[ival]].word[iword - 1]);
					if (less_than)
						break;
					EXPECT_EQ(arr_uint[sindex_uint[ival - 1]].word[iword - 1],
					          arr_uint[sindex_uint[ival]].word[iword - 1]);
				}
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_uint);
	}

	radixsort_deallocate(sort_uint);

	return 0;
}

DECLARE_TEST(radixsort, sort_custom_index32) {
	int bits = 0;
	uint256_t* arr_uint;
	uint32_t* uint_index_count;
	radixsort_t* sort_uint;

	sort_uint = radixsort_allocate_custom(32, (size_t)((1ULL << 22ULL) - 1));

	EXPECT_EQ(sort_uint->indextype, RADIXSORT_INDEX32);

	for (bits = 1; bits <= 22; ++bits) {
		uint32_t ival;
		uint32_t num = (uint32_t)((1ULL << (uint64_t)bits) - 1);
		const uint32_t* FOUNDATION_RESTRICT sindex_uint;

		arr_uint = memory_allocate(0, sizeof(uint256_t) * num, 0, MEMORY_PERSISTENT);

		for (ival = 0; ival < num; ++ival)
			arr_uint[ival] = uint256_make(random64(), random64(), random64(), random64());

		sindex_uint = radixsort_sort(sort_uint, arr_uint, num);

		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++uint_index_count[sindex_uint[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_uint[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++uint_index_count[sindex_uint[ival]];

				for (uint iword = 4; iword > 0; ++iword) {
					bool less_than =
					    (arr_uint[sindex_uint[ival - 1]].word[iword - 1] < arr_uint[sindex_uint[ival]].word[iword - 1]);
					if (less_than)
						break;
					EXPECT_EQ(arr_uint[sindex_uint[ival - 1]].word[iword - 1],
					          arr_uint[sindex_uint[ival]].word[iword - 1]);
				}
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_uint);
	}

	for (--bits; bits >= 1; --bits) {
		uint32_t ival;
		uint32_t num = (uint32_t)((1ULL << (uint64_t)bits) - 1);
		const uint32_t* FOUNDATION_RESTRICT sindex_uint;

		arr_uint = memory_allocate(0, sizeof(uint256_t) * num, 0, MEMORY_PERSISTENT);

		for (ival = 0; ival < num; ++ival)
			arr_uint[ival] = uint256_make(random64(), random64(), random64(), random64());

		sindex_uint = radixsort_sort(sort_uint, arr_uint, num);

		uint_index_count = memory_allocate(0, sizeof(uint32_t) * num, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);

		++uint_index_count[sindex_uint[0]];
		if (num == 1) {
			EXPECT_EQ(sindex_uint[0], 0);
		} else {
			for (ival = 1; ival < num; ++ival) {
				++uint_index_count[sindex_uint[ival]];

				for (uint iword = 4; iword > 0; ++iword) {
					bool less_than =
					    (arr_uint[sindex_uint[ival - 1]].word[iword - 1] < arr_uint[sindex_uint[ival]].word[iword - 1]);
					if (less_than)
						break;
					EXPECT_EQ(arr_uint[sindex_uint[ival - 1]].word[iword - 1],
					          arr_uint[sindex_uint[ival]].word[iword - 1]);
				}
			}
		}

		for (ival = 0; ival < num; ++ival) {
			EXPECT_EQ(uint_index_count[ival], 1);
		}
		memory_deallocate(uint_index_count);

		memory_deallocate(arr_uint);
	}

	radixsort_deallocate(sort_uint);

	return 0;
}

static void
test_radixsort_declare(void) {
	ADD_TEST(radixsort, allocation);
	ADD_TEST(radixsort, sort_custom_index16);
	ADD_TEST(radixsort, sort_custom_index32);
	ADD_TEST(radixsort, sort_int32_index16);
	ADD_TEST(radixsort, sort_int32_index32);
	ADD_TEST(radixsort, sort_int64_index16);
	ADD_TEST(radixsort, sort_int64_index32);
	ADD_TEST(radixsort, sort_real_index16);
	ADD_TEST(radixsort, sort_real_index32);
}

static test_suite_t test_radixsort_suite = {test_radixsort_application,
                                            test_radixsort_memory_system,
                                            test_radixsort_config,
                                            test_radixsort_declare,
                                            test_radixsort_initialize,
                                            test_radixsort_finalize,
                                            0};

#if BUILD_MONOLITHIC

int
test_radixsort_run(void);

int
test_radixsort_run(void) {
	test_suite = test_radixsort_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_radixsort_suite;
}

#endif
