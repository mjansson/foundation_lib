/* main.c  -  Foundation blowfish tests  -  Public Domain  -  2013 Mattias Jansson
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

#define VARIABLEKEYTESTS_COUNT 34
#define SETKEYTESTS_COUNT 24

static const uint32_t test_plaintext_left[VARIABLEKEYTESTS_COUNT + SETKEYTESTS_COUNT] = {
    0x00000000L, 0xFFFFFFFFL, 0x10000000L, 0x11111111L, 0x11111111L, 0x01234567L, 0x00000000L, 0x01234567L, 0x01A1D6D0L,
    0x5CD54CA8L, 0x0248D438L, 0x51454B58L, 0x42FD4430L, 0x059B5E08L, 0x0756D8E0L, 0x762514B8L, 0x3BDD1190L, 0x26955F68L,
    0x164D5E40L, 0x6B056E18L, 0x004BD6EFL, 0x480D3900L, 0x437540C8L, 0x072D43A0L, 0x02FE5577L, 0x1D9D5C50L, 0x30553228L,
    0x01234567L, 0x01234567L, 0x01234567L, 0xFFFFFFFFL, 0x00000000L, 0x00000000L, 0xFFFFFFFFL, 0xFEDCBA98L, 0xFEDCBA98L,
    0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L,
    0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L,
    0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L, 0xFEDCBA98L};

static const uint32_t test_plaintext_right[VARIABLEKEYTESTS_COUNT + SETKEYTESTS_COUNT] = {
    0x00000000L, 0xFFFFFFFFL, 0x00000001L, 0x11111111L, 0x11111111L, 0x89ABCDEFL, 0x00000000L, 0x89ABCDEFL, 0x39776742L,
    0x3DEF57DAL, 0x06F67172L, 0x2DDF440AL, 0x59577FA2L, 0x51CF143AL, 0x774761D2L, 0x29BF486AL, 0x49372802L, 0x35AF609AL,
    0x4F275232L, 0x759F5CCAL, 0x09176062L, 0x6EE762F2L, 0x698F3CFAL, 0x77075292L, 0x8117F12AL, 0x18F728C2L, 0x6D6F295AL,
    0x89ABCDEFL, 0x89ABCDEFL, 0x89ABCDEFL, 0xFFFFFFFFL, 0x00000000L, 0x00000000L, 0xFFFFFFFFL, 0x76543210L, 0x76543210L,
    0x76543210L, 0x76543210L, 0x76543210L, 0x76543210L, 0x76543210L, 0x76543210L, 0x76543210L, 0x76543210L, 0x76543210L,
    0x76543210L, 0x76543210L, 0x76543210L, 0x76543210L, 0x76543210L, 0x76543210L, 0x76543210L, 0x76543210L, 0x76543210L,
    0x76543210L, 0x76543210L, 0x76543210L, 0x76543210L};

static const unsigned char test_key_variable[VARIABLEKEYTESTS_COUNT][8] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11},
    {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF}, {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, {0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10},
    {0x7C, 0xA1, 0x10, 0x45, 0x4A, 0x1A, 0x6E, 0x57}, {0x01, 0x31, 0xD9, 0x61, 0x9D, 0xC1, 0x37, 0x6E},
    {0x07, 0xA1, 0x13, 0x3E, 0x4A, 0x0B, 0x26, 0x86}, {0x38, 0x49, 0x67, 0x4C, 0x26, 0x02, 0x31, 0x9E},
    {0x04, 0xB9, 0x15, 0xBA, 0x43, 0xFE, 0xB5, 0xB6}, {0x01, 0x13, 0xB9, 0x70, 0xFD, 0x34, 0xF2, 0xCE},
    {0x01, 0x70, 0xF1, 0x75, 0x46, 0x8F, 0xB5, 0xE6}, {0x43, 0x29, 0x7F, 0xAD, 0x38, 0xE3, 0x73, 0xFE},
    {0x07, 0xA7, 0x13, 0x70, 0x45, 0xDA, 0x2A, 0x16}, {0x04, 0x68, 0x91, 0x04, 0xC2, 0xFD, 0x3B, 0x2F},
    {0x37, 0xD0, 0x6B, 0xB5, 0x16, 0xCB, 0x75, 0x46}, {0x1F, 0x08, 0x26, 0x0D, 0x1A, 0xC2, 0x46, 0x5E},
    {0x58, 0x40, 0x23, 0x64, 0x1A, 0xBA, 0x61, 0x76}, {0x02, 0x58, 0x16, 0x16, 0x46, 0x29, 0xB0, 0x07},
    {0x49, 0x79, 0x3E, 0xBC, 0x79, 0xB3, 0x25, 0x8F}, {0x4F, 0xB0, 0x5E, 0x15, 0x15, 0xAB, 0x73, 0xA7},
    {0x49, 0xE9, 0x5D, 0x6D, 0x4C, 0xA2, 0x29, 0xBF}, {0x01, 0x83, 0x10, 0xDC, 0x40, 0x9B, 0x26, 0xD6},
    {0x1C, 0x58, 0x7F, 0x1C, 0x13, 0x92, 0x4F, 0xEF}, {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},
    {0x1F, 0x1F, 0x1F, 0x1F, 0x0E, 0x0E, 0x0E, 0x0E}, {0xE0, 0xFE, 0xE0, 0xFE, 0xF1, 0xFE, 0xF1, 0xFE},
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF}, {0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10}};

static const unsigned char test_key_set[24] = {0xF0, 0xE1, 0xD2, 0xC3, 0xB4, 0xA5, 0x96, 0x87, 0x78, 0x69, 0x5A, 0x4B,
                                               0x3C, 0x2D, 0x1E, 0x0F, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};

static const uint32_t test_ciphertext_left[VARIABLEKEYTESTS_COUNT + SETKEYTESTS_COUNT] = {
    0x4EF99745L, 0x51866FD5L, 0x7D856F9AL, 0x2466DD87L, 0x61F9C380L, 0x7D0CC630L, 0x4EF99745L, 0x0ACEAB0FL, 0x59C68245L,
    0xB1B8CC0BL, 0x1730E577L, 0xA25E7856L, 0x353882B1L, 0x48F4D088L, 0x432193B7L, 0x13F04154L, 0x2EEDDA93L, 0xD887E039L,
    0x5F99D04FL, 0x4A057A3BL, 0x452031C1L, 0x7555AE39L, 0x53C55F9CL, 0x7A8E7BFAL, 0xCF9C5D7AL, 0xD1ABB290L, 0x55CB3774L,
    0xFA34EC48L, 0xA7907951L, 0xC39E072DL, 0x014933E0L, 0xF21E9A77L, 0x24594688L, 0x6B5C5A9CL, 0xF9AD597CL, 0xE91D21C1L,
    0xE9C2B70AL, 0xBE1E6394L, 0xB39E4448L, 0x9457AA83L, 0x8BB77032L, 0xE87A244EL, 0x15750E7AL, 0x122BA70BL, 0x3A833C9AL,
    0x9409DA87L, 0x884F8062L, 0x1F85031CL, 0x79D9373AL, 0x93142887L, 0x03429E83L, 0xA4299E27L, 0xAFD5AED1L, 0x10851C0EL,
    0xE6F51ED7L, 0x64A6E14AL, 0x80C7D7D4L, 0x05044B62L};

static const uint32_t test_ciphertext_right[VARIABLEKEYTESTS_COUNT + SETKEYTESTS_COUNT] = {
    0x6198DD78L, 0xB85ECB8AL, 0x613063F2L, 0x8B963C9DL, 0x2281B096L, 0xAFDA1EC7L, 0x6198DD78L, 0xC6A0A28DL, 0xEB05282BL,
    0x250F09A0L, 0x8BEA1DA4L, 0xCF2651EBL, 0x09CE8F1AL, 0x4C379918L, 0x8951FC98L, 0xD69D1AE5L, 0xFFD39C79L, 0x3C2DA6E3L,
    0x5B163969L, 0x24D3977BL, 0xE4FADA8EL, 0xF59B87BDL, 0xB49FC019L, 0x937E89A3L, 0x4986ADB5L, 0x658BC778L, 0xD13EF201L,
    0x47B268B2L, 0x08EA3CAEL, 0x9FAC631DL, 0xCDAFF6E4L, 0xB71C49BCL, 0x5754369AL, 0x5D9E0A5AL, 0x49DB005EL, 0xD961A6D6L,
    0x1BC65CF3L, 0x08640F05L, 0x1BDB1E6EL, 0xB1928C0DL, 0xF960629DL, 0x2CC85E82L, 0x4F4EC577L, 0x3AB64AE0L, 0xFFC537F6L,
    0xA90F6BF2L, 0x5060B8B4L, 0x19E11968L, 0x714CA34FL, 0xEE3BE15CL, 0x8CE2D14BL, 0x469FF67BL, 0xC1BC96A8L, 0x3858DA9FL,
    0x9B9DB21FL, 0xFD36B46FL, 0x5A5479ADL, 0xFA52D080L};

static application_t
test_blowfish_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation blowfish tests"));
	app.short_name = string_const(STRING_CONST("test_blowfish"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_blowfish_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_blowfish_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_blowfish_initialize(void) {
	return 0;
}

static void
test_blowfish_finalize(void) {
}

// Internal functions
extern void
blowfish_encrypt_words(const blowfish_t* blowfish, uint32_t* lvalres, uint32_t* hvalres);

extern void
blowfish_decrypt_words(const blowfish_t* blowfish, uint32_t* lvalres, uint32_t* hvalres);

DECLARE_TEST(blowfish, initialize) {
	blowfish_t* blowfish;
	unsigned int i, j;
	uint32_t left, right;

	blowfish = blowfish_allocate();

	for (i = 0; i < VARIABLEKEYTESTS_COUNT; ++i) {
		blowfish_initialize(blowfish, test_key_variable[i], 8);

		left = test_plaintext_left[i];
		right = test_plaintext_right[i];
		blowfish_encrypt_words(blowfish, &left, &right);

		EXPECT_EQ(memcmp(&left, &test_ciphertext_left[i], 4), 0);
		EXPECT_EQ(memcmp(&right, &test_ciphertext_right[i], 4), 0);

		blowfish_decrypt_words(blowfish, &left, &right);

		EXPECT_EQ(memcmp(&left, &test_plaintext_left[i], 4), 0);
		EXPECT_EQ(memcmp(&right, &test_plaintext_right[i], 4), 0);
	}

	for (j = 1, i = VARIABLEKEYTESTS_COUNT; i < (VARIABLEKEYTESTS_COUNT + SETKEYTESTS_COUNT); ++i) {
		blowfish_initialize(blowfish, test_key_set, j++);

		left = test_plaintext_left[i];
		right = test_plaintext_right[i];
		blowfish_encrypt_words(blowfish, &left, &right);

		EXPECT_EQ(memcmp(&left, &test_ciphertext_left[i], 4), 0);
		EXPECT_EQ(memcmp(&right, &test_ciphertext_right[i], 4), 0);

		blowfish_decrypt_words(blowfish, &left, &right);

		EXPECT_EQ(memcmp(&left, &test_plaintext_left[i], 4), 0);
		EXPECT_EQ(memcmp(&right, &test_plaintext_right[i], 4), 0);
	}

	blowfish_deallocate(blowfish);

	return 0;
}

DECLARE_TEST(blowfish, known_data) {
	unsigned char plaintext[2][VARIABLEKEYTESTS_COUNT * 8];
	unsigned int i;
	blowfish_t* blowfish;
	uint64_t init_vector = 0x54A23F87BE3147C3;

	blowfish = blowfish_allocate();

	for (i = 0; i < VARIABLEKEYTESTS_COUNT; ++i) {
		memcpy(&plaintext[0][(8 * i) + 0], &test_plaintext_left[i], 4);
		memcpy(&plaintext[0][(8 * i) + 4], &test_plaintext_right[i], 4);
	}

	memcpy(plaintext[1], plaintext[0], VARIABLEKEYTESTS_COUNT * 8);

	for (i = 0; i < VARIABLEKEYTESTS_COUNT; ++i) {
		blowfish_initialize(blowfish, test_key_variable[i], 8);

		// Add some unalignment to size of buffer to test re-alignment in implementation
		blowfish_encrypt(blowfish, plaintext[0], 1 + VARIABLEKEYTESTS_COUNT * 8, BLOCKCIPHER_ECB, init_vector);
		blowfish_decrypt(blowfish, plaintext[0], 2 + VARIABLEKEYTESTS_COUNT * 8, BLOCKCIPHER_ECB, init_vector);
		EXPECT_EQ(memcmp(plaintext[0], plaintext[1], VARIABLEKEYTESTS_COUNT * 8), 0);

		blowfish_encrypt(blowfish, plaintext[0], 3 + VARIABLEKEYTESTS_COUNT * 8, BLOCKCIPHER_CBC, init_vector);
		blowfish_decrypt(blowfish, plaintext[0], 4 + VARIABLEKEYTESTS_COUNT * 8, BLOCKCIPHER_CBC, init_vector);
		EXPECT_EQ(memcmp(plaintext[0], plaintext[1], VARIABLEKEYTESTS_COUNT * 8), 0);

		blowfish_encrypt(blowfish, plaintext[0], 5 + VARIABLEKEYTESTS_COUNT * 8, BLOCKCIPHER_CFB, init_vector);
		blowfish_decrypt(blowfish, plaintext[0], 6 + VARIABLEKEYTESTS_COUNT * 8, BLOCKCIPHER_CFB, init_vector);
		EXPECT_EQ(memcmp(plaintext[0], plaintext[1], VARIABLEKEYTESTS_COUNT * 8), 0);

		blowfish_encrypt(blowfish, plaintext[0], 7 + VARIABLEKEYTESTS_COUNT * 8, BLOCKCIPHER_OFB, init_vector);
		blowfish_decrypt(blowfish, plaintext[0], VARIABLEKEYTESTS_COUNT * 8, BLOCKCIPHER_OFB, init_vector);
		EXPECT_EQ(memcmp(plaintext[0], plaintext[1], VARIABLEKEYTESTS_COUNT * 8), 0);

		init_vector *= (uintptr_t)blowfish;
	}

	blowfish_deallocate(blowfish);

	return 0;
}

DECLARE_TEST(blowfish, random_data) {
	uint64_t plaintext[2][1024];
	uint64_t keytext[32];
	unsigned int i, j;
	blowfish_t* blowfish;
	uint64_t init_vector;

	blowfish = blowfish_allocate();

	for (i = 0; i < 1024; ++i) {
		for (j = 0; j < 32; ++j)
			keytext[j] = random64();

		for (j = 0; j < 1024; ++j) {
			plaintext[0][j] = random64();
			plaintext[1][j] = plaintext[0][j];
		}

		init_vector = random64();

		blowfish_initialize(blowfish, keytext, random32_range(1, 32 * 8));

		blowfish_encrypt(blowfish, plaintext[0], 1024 * 8, BLOCKCIPHER_ECB, init_vector);
		blowfish_decrypt(blowfish, plaintext[0], 1024 * 8, BLOCKCIPHER_ECB, init_vector);
		EXPECT_EQ(memcmp(plaintext[0], plaintext[1], 1024 * 8), 0);

		blowfish_encrypt(blowfish, plaintext[0], 1024 * 8, BLOCKCIPHER_CBC, init_vector);
		blowfish_decrypt(blowfish, plaintext[0], 1024 * 8, BLOCKCIPHER_CBC, init_vector);
		EXPECT_EQ(memcmp(plaintext[0], plaintext[1], 1024 * 8), 0);

		blowfish_encrypt(blowfish, plaintext[0], 1024 * 8, BLOCKCIPHER_CFB, init_vector);
		blowfish_decrypt(blowfish, plaintext[0], 1024 * 8, BLOCKCIPHER_CFB, init_vector);
		EXPECT_EQ(memcmp(plaintext[0], plaintext[1], 1024 * 8), 0);

		blowfish_encrypt(blowfish, plaintext[0], 1024 * 8, BLOCKCIPHER_OFB, init_vector);
		blowfish_decrypt(blowfish, plaintext[0], 1024 * 8, BLOCKCIPHER_OFB, init_vector);
		EXPECT_EQ(memcmp(plaintext[0], plaintext[1], 1024 * 8), 0);
	}

	blowfish_deallocate(blowfish);

	return 0;
}

static void
test_blowfish_declare(void) {
	ADD_TEST(blowfish, initialize);
	ADD_TEST(blowfish, known_data);
	ADD_TEST(blowfish, random_data);
}

static test_suite_t test_blowfish_suite = {test_blowfish_application,
                                           test_blowfish_memory_system,
                                           test_blowfish_config,
                                           test_blowfish_declare,
                                           test_blowfish_initialize,
                                           test_blowfish_finalize,
                                           0};

#if BUILD_MONOLITHIC

int
test_blowfish_run(void);

int
test_blowfish_run(void) {
	test_suite = test_blowfish_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_blowfish_suite;
}

#endif
