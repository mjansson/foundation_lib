/* main.c  -  Foundation md5 test  -  Public Domain  -  2013 Mattias Jansson
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
test_md5_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation md5 tests"));
	app.short_name = string_const(STRING_CONST("test_md5"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_md5_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_md5_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_md5_initialize(void) {
	return 0;
}

static void
test_md5_finalize(void) {
}

DECLARE_TEST(md5, empty) {
	md5_t* md5;
	char md5str[33];
	string_t digest;

	md5 = md5_allocate();
	md5_digest_finalize(md5);
	digest = md5_get_digest(md5, md5str, sizeof(md5str));

	EXPECT_STRINGEQ(digest, string_const(STRING_CONST("d41d8cd98f00b204e9800998ecf8427e")));

	md5_initialize(md5);
	md5_digest_finalize(md5);
	digest = md5_get_digest(md5, md5str, sizeof(md5str));

	EXPECT_STRINGEQ(digest, string_const(STRING_CONST("d41d8cd98f00b204e9800998ecf8427e")));

	md5_deallocate(md5);

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

DECLARE_TEST(md5, reference) {
	md5_t* md5;
	char md5str[33];
	string_t digest;

	md5 = md5_allocate();
	md5_digest_finalize(md5);

	md5_digest(md5, "testing md5 implementation", 26);
	md5_digest_finalize(md5);

	digest = md5_get_digest(md5, md5str, sizeof(md5str));

	EXPECT_STRINGEQ(digest, string_const(STRING_CONST("4e24e37e5e06f23210fa1518e97a50c4")));

	md5_digest(md5, "testing md5 implementation", 26);
	md5_digest(md5, "", 0);
	md5_digest(md5, "further testing md5 implementation with long buffer > 32 bytes", 62);
	md5_digest_finalize(md5);
	digest = md5_get_digest(md5, md5str, sizeof(md5str));

	EXPECT_STRINGEQ(digest, string_const(STRING_CONST("bd870884942ea7b32a9cb2547b02b871")));

	md5_digest(md5, digest_test_string, 2000);
	md5_digest_finalize(md5);
	digest = md5_get_digest(md5, md5str, sizeof(md5str));

	EXPECT_STRINGEQ(digest, string_const(STRING_CONST("137d3c94230a0e230c4ddfc97eacccd2")));

	md5_deallocate(md5);

	return 0;
}

DECLARE_TEST(md5, streams) {
	stream_t* test_stream;
	stream_t* unix_stream;
	stream_t* windows_stream;
	uint128_t digest, unix_digest, windows_digest;
	size_t ichunks;

	char unix_buffer[] =
	    "tvqy0C9TO2MI7uyUqr\n\n"
	    "QVcrmjDKDEcB3e7dpM\n\n"
	    "bMpRv6uH0LWPjvNcNp\n"
	    "jpkje9KYXhJjSA2TBy\n"
	    "bTqnOPhOOZ5aWnUdgO\n"
	    "fLapbWSZInasn1SJlk\n"
	    "ytNFKkASDln05zw39X\n"
	    "L8McmojqPmn41Y6CRN\n\n"
	    "wZbKo0PZofDpbAMr1u\n"
	    "u8GJIocbEBFzsyKo62\n"
	    "FlQh1pjm9jBeoEJIHL\n"
	    "v4ixQn77l4M7zbRHgJ\n"
	    "DIVy0vvpNEzxFRyD3Z\n"
	    "5OrJvr\n";

	char windows_buffer[] =
	    "tvqy0C9TO2MI7uyUqr\r\n\r\n"
	    "QVcrmjDKDEcB3e7dpM\r\n\r\n"
	    "bMpRv6uH0LWPjvNcNp\r\n"
	    "jpkje9KYXhJjSA2TBy\r\n"
	    "bTqnOPhOOZ5aWnUdgO\r\n"
	    "fLapbWSZInasn1SJlk\r\n"
	    "ytNFKkASDln05zw39X\r\n"
	    "L8McmojqPmn41Y6CRN\r\n\r\n"
	    "wZbKo0PZofDpbAMr1u\r\n"
	    "u8GJIocbEBFzsyKo62\r\n"
	    "FlQh1pjm9jBeoEJIHL\r\n"
	    "v4ixQn77l4M7zbRHgJ\r\n"
	    "DIVy0vvpNEzxFRyD3Z\r\n"
	    "5OrJvr\r\n";

	test_stream = buffer_stream_allocate(0, STREAM_IN | STREAM_OUT | STREAM_BINARY, 0, 0, true, true);
	stream_write(test_stream, digest_test_string, 2000);

	stream_seek(test_stream, 0, STREAM_SEEK_BEGIN);
	digest = stream_md5(test_stream);
	stream_deallocate(test_stream);

	EXPECT_TRUE(uint128_equal(digest, uint128_make(byteorder_bigendian64(0x230e0a23943c7d13ULL),
	                                               byteorder_bigendian64(0xd2ccac7ec9df4d0cULL))));

	unix_stream = buffer_stream_allocate((void*)unix_buffer, STREAM_IN, string_length(unix_buffer),
	                                     string_length(unix_buffer), false, false);
	windows_stream = buffer_stream_allocate((void*)windows_buffer, STREAM_IN, string_length(windows_buffer),
	                                        string_length(windows_buffer), false, false);

	stream_set_binary(unix_stream, false);
	stream_set_binary(windows_stream, false);

	unix_digest = stream_md5(unix_stream);
	windows_digest = stream_md5(windows_stream);

	EXPECT_TRUE(uint128_equal(unix_digest, uint128_make(byteorder_bigendian64(0xcf32b789c7e77197ULL),
	                                                    byteorder_bigendian64(0x2bff28c36c601093ULL))));
	EXPECT_TRUE(uint128_equal(unix_digest, windows_digest));

	stream_set_binary(unix_stream, true);
	stream_set_binary(windows_stream, true);

	stream_seek(unix_stream, 0, STREAM_SEEK_BEGIN);
	stream_seek(windows_stream, 0, STREAM_SEEK_BEGIN);

	unix_digest = stream_md5(unix_stream);
	windows_digest = stream_md5(windows_stream);

	stream_deallocate(unix_stream);
	stream_deallocate(windows_stream);

	EXPECT_TRUE(uint128_equal(unix_digest, uint128_make(byteorder_bigendian64(0xcf32b789c7e77197ULL),
	                                                    byteorder_bigendian64(0x2bff28c36c601093ULL))));
	EXPECT_TRUE(uint128_equal(windows_digest, uint128_make(byteorder_bigendian64(0xf77d63bbe1df9334ULL),
	                                                       byteorder_bigendian64(0x24d5cb05cd503e44ULL))));
	EXPECT_TRUE(!uint128_equal(unix_digest, windows_digest));

	unix_stream = buffer_stream_allocate(nullptr, STREAM_IN | STREAM_OUT, 128 * 1024, 128 * 1024, true, true);
	windows_stream = buffer_stream_allocate(nullptr, STREAM_IN | STREAM_OUT, 128 * 1024, 128 * 1024, true, true);

	ichunks = 0;
	while ((ichunks + 1) * 3 < 128 * 1024) {
		stream_write(unix_stream, STRING_CONST("a\n"));
		stream_write(windows_stream, STRING_CONST("a\r\n"));
		++ichunks;
	}

	stream_seek(unix_stream, 0, STREAM_SEEK_BEGIN);
	stream_seek(windows_stream, 0, STREAM_SEEK_BEGIN);

	unix_digest = stream_md5(unix_stream);
	windows_digest = stream_md5(windows_stream);

	stream_deallocate(unix_stream);
	stream_deallocate(windows_stream);

	EXPECT_TRUE(uint128_equal(unix_digest, windows_digest));

	return 0;
}

static void
test_md5_declare(void) {
	ADD_TEST(md5, empty);
	ADD_TEST(md5, reference);
	ADD_TEST(md5, streams);
}

static test_suite_t test_md5_suite = {test_md5_application,
                                      test_md5_memory_system,
                                      test_md5_config,
                                      test_md5_declare,
                                      test_md5_initialize,
                                      test_md5_finalize,
                                      0};

#if BUILD_MONOLITHIC

int
test_md5_run(void);

int
test_md5_run(void) {
	test_suite = test_md5_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_md5_suite;
}

#endif
