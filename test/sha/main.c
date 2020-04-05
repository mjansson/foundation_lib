/* main.c  -  Foundation SHA-2 test  -  Public Domain  -  2013 Mattias Jansson
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
test_sha_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation SHA-2 tests"));
	app.short_name = string_const(STRING_CONST("test_sha"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_sha_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_sha_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_sha_initialize(void) {
	return 0;
}

static void
test_sha_finalize(void) {
}

DECLARE_TEST(sha, empty) {
	sha256_t* sha256;
	sha512_t* sha512;
	char shastr[129];
	string_t digest;

	sha256 = sha256_allocate();
	sha256_digest_finalize(sha256);
	digest = sha256_get_digest(sha256, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")));

	sha256_initialize(sha256);
	sha256_digest_finalize(sha256);
	digest = sha256_get_digest(sha256, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")));

	sha256_deallocate(sha256);

	sha512 = sha512_allocate();
	sha512_digest_finalize(sha512);
	digest = sha512_get_digest(sha512, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c"
	                                          "5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e")));

	sha512_initialize(sha512);
	sha512_digest_finalize(sha512);
	digest = sha512_get_digest(sha512, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c"
	                                          "5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e")));

	sha512_deallocate(sha512);

	return 0;
}

static const char* digest_test_string =
    "tvqy0C9TO2MI7uyUqrQVcrmjDKDEcB3e7dpMbMpRv6uH0LWPjvNcNpjpkje9KYXhJjSA2TBybTqnOPhOOZ5aWnUdgOfLapbWSZInasn1SJlkytNFKk"
    "ASDln0"
    "5zw39XL8McmojqPmn41Y6CRNwZbKo0PZofDpbAMr1uu8GJIocbEBFzsyKo62FlQh1pjm9jBeoEJIHLv4ixQn77l4M7zbRHgJDIVy0vvpNEzxFRyD3Z"
    "5OrJvr"
    "sUpwyXzcX3cYoezA2ob0ECsSk9lki8SDaCRpMxC1x0vKLCdKBHIV0IOgSZcMQCz11enNRDsCxuG45EsFQfiw2gmfcg7nI4tfEAW6DdExDWcpE3yDD2"
    "GaFs9S"
    "1q90mhlzTrlUZSir0Grc0qRetsmqwxh90SG6nEawLgL8B0G7xX2vHswwrF4pubz3vFwJkTczMsSsJI1UzEoMk61hwWzNwmFmEo8joMK832IZFOsSfm"
    "8cPA88"
    "FLj0yFMEkjdaidPV87f7uLFgjUYUrHDqdaOKuic7H1KTavXV6u61RxXYWwUxtXuZTza61Q8T1wK4UAo2i9upbkymYeCPD9BP7DsZDtB3qEBRDIOxqM"
    "GCNf3n"
    "FORbM5AeuTIqzFfHTAVhxXkJ8OYO6SOFfneaysmbMO4P7IukWr6ixML7XVON6th8YKv0bSReXpzuG4addOWT8OISopcOWYdhe5kKUETueVLPEYZy5N"
    "K7SXjk"
    "uMNwOsG9tdQFtsf3IewcAIKNFwkgO93uqANFfzg2f2gdjiYczmwv63gXjuYEGuBAcZELYmwkItqtNoacvIfJERfIpY6Uuh2VR0CxQzM0jEsG2FdsTI"
    "Ykq4Jq"
    "cm5lsAnA25cWtb5GGTyNOuRUpHZKgefrkUrAjV2KvEVMEe8Fk3p8xKny8mEUNPhjrhp7HJpOoNTrHB4dOG2RXCS5vQE3GwoI24amSSGQ24OluuDfVb"
    "q4mWbv"
    "1R6Q90UKkMUhfWYI46sHo975SlG2svsFuvzAusHKdwnj7U1Q9V0NS4no8jBFcTQ6pj1VJsuC1whEbaHRcRdbdLtOf8bMlf93sTfE8EKX2yE9YYTgWd"
    "BoNlde"
    "3yAERyj4M0W7nxACUmbApU4MZJq9c24cJwoLwyDiqwClAQm2KBtUvunraZCDK9095QlE00MjeiVthceVe3XlyoKEpZqOMqpntf90H7awqkG54Nq3G1"
    "8UYd4l"
    "zpAcJpLilhlxvvNy1Y81buCaakocAYal3gnvCYuw2BaXn1fWq5jgR2dfgryYatgcdAk1fmbCgE4oMq0B8EM1tDIG6xVL1iKs1IcRp767NtsOjAAQuP"
    "TpHII4"
    "quYpacd1BCbvg65lVIFEVkRINy0Bkj4VthWMuEeuPN84Ndg8GVu7zGROhuZZz1WTp9dkUhYip6AfAiX1N2VKsC2JTPIcZKjB8rCTqhLe7ykxF8qfYx"
    "4I87ek"
    "12RIGfldhteCORpOqLVC4qtIBLADIavjgWpum3evffwtDjbZFPY0yIzUV0hsLX8HuRqyg0KlkZ0aCfOArN9V24Ql97Iwy5L7FTu6PKVEGFmwNcFKUg"
    "KruRKt"
    "GS12sUmTo5AgZBCEI0Z2rBbhHO1xwrb2k3fqjOSFoUhYe5SUMc0copW1cRzJwOCLscJSfOf8iCKlszCgiK3AHsS7jxNIEi89gBAOPbeGnN8DmqFIAg"
    "4MCMQx"
    "hnNzsTlvy8tDzg36k8SFzn9mqRvLvbPzX2jr52wBSEgKDXJoXCeTFOTLWKProh9WWkYlohtmGTtNoK8dm7G2qbZ6X3UpIBHJajF6ZPbTVGR4Q9Fm0b"
    "ck2dR7"
    "0uW4zk9cK7xBaqlRE6ifUPUEFvcIu8Ltar1TrxtVvpX6yF6drz9nJEViYSmbafZYiQnKRib0RhN1zRzBYQlQUrAvFx7T2mS65jbp8jIVlNrVnG2g5j"
    "47hgL4"
    "QLd6uVg78HZXW68Yf6ZJp8EKN7eRsPcUZDNTOsJXp96CXORuhKOw1ZsCmXZVgmj9AUUAriR9YCVbmSPm";

DECLARE_TEST(sha, reference) {
	sha256_t* sha256;
	sha512_t* sha512;
	char shastr[129];
	string_t digest;

	sha256 = sha256_allocate();
	sha256_digest_finalize(sha256);

	sha256_digest(sha256, STRING_CONST(""));
	sha256_digest_finalize(sha256);

	digest = sha256_get_digest(sha256, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855")));

	sha256_digest(sha256, STRING_CONST("t"));
	sha256_digest_finalize(sha256);

	digest = sha256_get_digest(sha256, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("e3b98a4da31a127d4bde6e43033f66ba274cab0eb7eb1c70ec41402bf6273dd8")));

	sha256_digest(sha256, STRING_CONST("testing sha implementation"));
	sha256_digest_finalize(sha256);

	digest = sha256_get_digest(sha256, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("996b42ec51535716079323ae099876d25c5d34a13c529bab5cf0e69613d92163")));

	sha256_digest(sha256, STRING_CONST("testing sha implementation"));
	sha256_digest(sha256, STRING_CONST(""));
	sha256_digest(sha256, STRING_CONST("further testing sha implementation with long buffer > 32 bytes"));
	sha256_digest_finalize(sha256);
	digest = sha256_get_digest(sha256, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("1c282ea87da44b562e7d737ce67f6286f10431ae23c96114ffea6f108617bdac")));

	sha256_digest(sha256, STRING_CONST("testing sha implementation with a buffer that is 64 + 58 bytes l"));
	sha256_digest(sha256, STRING_CONST(""));
	sha256_digest(sha256, STRING_CONST("ong, which should trigger the edge case of buffer padding."));
	sha256_digest_finalize(sha256);
	digest = sha256_get_digest(sha256, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("546cb96d98f9e20fb6cbbcbf2ca215d793ecb1f98bc19d458e8d5460e249f769")));

	sha256_digest(sha256, digest_test_string, 2000);
	sha256_digest_finalize(sha256);
	digest = sha256_get_digest(sha256, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("7c0b7113a3e5a156d60e858da92a11a1fcdb11e7626482179f0a3ded5ef33a75")));

	sha256_deallocate(sha256);

	sha512 = sha512_allocate();
	sha512_digest_finalize(sha512);

	sha512_digest(sha512, STRING_CONST(""));
	sha512_digest_finalize(sha512);

	digest = sha512_get_digest(sha512, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c"
	                                          "5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e")));

	sha512_digest(sha512, STRING_CONST("t"));
	sha512_digest_finalize(sha512);

	digest = sha512_get_digest(sha512, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("99f97d455d5d62b24f3a942a1abc3fa8863fc0ce2037f52f09bd785b22b800d4f2e7b2b6"
	                                          "14cb600ffc2a4fe24679845b24886d69bb776fcfa46e54d188889c6f")));

	sha512_digest(sha512, STRING_CONST("testing sha implementation"));
	sha512_digest_finalize(sha512);

	digest = sha512_get_digest(sha512, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("8a6b756216f93dd17352b64984c1ea25d6d4be8ffd284c679d7f2f04c956c7e015f0c770"
	                                          "260a607a4ba94ef46ef7f68a3e47c79d400d1bf0d9ce3e1862663805")));

	sha512_digest(sha512, STRING_CONST("testing sha implementation"));
	sha512_digest(sha512, STRING_CONST(""));
	sha512_digest(sha512, STRING_CONST("further testing sha implementation with long buffer > 32 bytes"));
	sha512_digest_finalize(sha512);
	digest = sha512_get_digest(sha512, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("a150934b8be284a8a417196e3ddb6402fcdcecedcbeedd7ca5eb1a7603b170e98f4b0878"
	                                          "6967f50aa5912b0cdbc64dc78a39ddc9f2bc52c438f1570601c8e73d")));

	sha512_digest(sha512, STRING_CONST("testing sha implementation with a buffer that is 128 + 114 bytes l"));
	sha512_digest(sha512, STRING_CONST(""));
	sha512_digest(sha512,
	              STRING_CONST("ong, which should trigger the edge case of buffer padding. Fill with nonsense. "));
	sha512_digest(
	    sha512,
	    STRING_CONST(
	        "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut lab"));
	sha512_digest_finalize(sha512);
	digest = sha512_get_digest(sha512, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("c752654931621ac2eaf8aef597000403b6f6d40c3cc4f18ad579bea7a586857381935657"
	                                          "fe6470b7bf729b36af4581207d096851d7bec7d972cd61dfef78eb7a")));

	sha512_digest(sha512, digest_test_string, 2000);
	sha512_digest_finalize(sha512);
	digest = sha512_get_digest(sha512, shastr, sizeof(shastr));

	EXPECT_STRINGEQ(digest,
	                string_const(STRING_CONST("4f85e6e923f5b173002a0edbd99f0bdafa8ddbe8653c19323b087ac772b36d1c61da24ad"
	                                          "f5bf91f771057e039fbefc63814d14bc4ad6ad4acd996ccdfa6f954d")));

	sha512_deallocate(sha512);

	return 0;
}

static void
test_sha_declare(void) {
	ADD_TEST(sha, empty);
	ADD_TEST(sha, reference);
}

static test_suite_t test_sha_suite = {test_sha_application,
                                      test_sha_memory_system,
                                      test_sha_config,
                                      test_sha_declare,
                                      test_sha_initialize,
                                      test_sha_finalize,
                                      0};

#if BUILD_MONOLITHIC

int
test_sha_run(void);

int
test_sha_run(void) {
	test_suite = test_sha_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_sha_suite;
}

#endif
