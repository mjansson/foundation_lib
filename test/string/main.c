/* main.c  -  Foundation string tests  -  Public Domain  -  2013 Mattias Jansson
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

#include <locale.h>

static application_t
test_string_application(void) {
	application_t app;
	memset(&app, 0, sizeof(app));
	app.name = string_const(STRING_CONST("Foundation string tests"));
	app.short_name = string_const(STRING_CONST("test_string"));
	app.company = string_const(STRING_CONST(""));
	app.flags = APPLICATION_UTILITY;
	app.exception_handler = test_exception_handler;
	return app;
}

static memory_system_t
test_string_memory_system(void) {
	return memory_system_malloc();
}

static foundation_config_t
test_string_config(void) {
	foundation_config_t config;
	memset(&config, 0, sizeof(config));
	return config;
}

static int
test_string_initialize(void) {
	return 0;
}

static void
test_string_finalize(void) {
}

DECLARE_TEST(string, allocate) {
	{
		string_t nullstr1 = string_allocate(0, 0);
		string_t nullstr2 = string_allocate(0, 1);
		string_t nullstr3 = string_allocate(0, 32);
		string_t nullstr4 = string_allocate(1, 2);
		string_t nullstr5 = string_allocate(10, 32);

		EXPECT_EQ(0, nullstr1.length);
		EXPECT_EQ(0, nullstr2.length);
		EXPECT_EQ(0, nullstr3.length);
		EXPECT_EQ(1, nullstr4.length);
		EXPECT_EQ(10, nullstr5.length);
		EXPECT_EQ(0, nullstr1.str);
		EXPECT_NE(0, nullstr2.str);
		EXPECT_EQ(0, strcmp(nullstr2.str, ""));
		EXPECT_EQ(0, strcmp(nullstr3.str, ""));
		EXPECT_EQ(0, strcmp(nullstr4.str, ""));
		EXPECT_EQ(0, strcmp(nullstr5.str, ""));

		string_deallocate(nullstr1.str);
		string_deallocate(nullstr2.str);
		string_deallocate(nullstr3.str);
		string_deallocate(nullstr4.str);
		string_deallocate(nullstr5.str);
	}
	{
		char teststr1[] = "test";
		char teststr2[] = "testing long string with more than 16 characters";
		char teststr3[] =
		    "\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x90"
		    "\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x90"
		    "\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x09";
		string_const_t substr;
		string_t str1, str2, str3, str4, str5, str6, str7, str8, str9, str10, str11, str12, str13, str14, str15, str16,
		    str17;

		str1 = string_clone(STRING_CONST(teststr1));
		substr = string_substr(STRING_CONST(teststr1), 0, 3);
		str2 = string_clone(STRING_ARGS(substr));
		substr = string_substr(STRING_CONST(teststr1), 0, 4);
		str3 = string_clone(STRING_ARGS(substr));
		substr = string_substr(STRING_CONST(teststr1), 0, 32);
		str4 = string_clone(STRING_ARGS(substr));
		substr = string_substr(STRING_CONST(teststr1), 0, STRING_NPOS);
		str5 = string_clone(STRING_ARGS(substr));
		substr = string_substr(STRING_CONST(teststr1), 5, STRING_NPOS);
		str6 = string_clone(STRING_ARGS(substr));

		str7 = string_clone(STRING_CONST(teststr2));
		substr = string_substr(STRING_CONST(teststr2), 0, 3);
		str8 = string_clone(STRING_ARGS(substr));
		substr = string_substr(STRING_CONST(teststr2), 0, 20);
		str9 = string_clone(STRING_ARGS(substr));
		substr = string_substr(STRING_CONST(teststr2), 0, STRING_NPOS);
		str10 = string_clone(STRING_ARGS(substr));
		substr = string_substr(STRING_CONST(teststr2), 256, STRING_NPOS);
		str11 = string_clone(STRING_ARGS(substr));

		str12 = string_clone(STRING_CONST(teststr3));
		substr = string_substr(STRING_CONST(teststr3), 0, 3);
		str13 = string_clone_string(substr);
		substr = string_substr(STRING_CONST(teststr3), 0, 20);
		str14 = string_clone_string(substr);
		substr = string_substr(STRING_CONST(teststr3), 0, STRING_NPOS);
		str15 = string_clone(STRING_ARGS(substr));
		substr = string_substr(STRING_CONST(teststr3), STRING_NPOS, STRING_NPOS);
		str16 = string_clone(STRING_ARGS(substr));
		str17 = string_clone(nullptr, 0);

		EXPECT_EQ(0, strcmp(str1.str, teststr1));
		EXPECT_EQ(0, strcmp(str2.str, "tes"));
		EXPECT_EQ(0, strcmp(str3.str, teststr1));
		EXPECT_EQ(0, strcmp(str4.str, teststr1));
		EXPECT_EQ(0, strcmp(str5.str, teststr1));
		EXPECT_EQ(0, strcmp(str6.str, ""));

		EXPECT_EQ(0, strcmp(str7.str, teststr2));
		EXPECT_EQ(0, strcmp(str8.str, "tes"));
		EXPECT_EQ(0, strcmp(str9.str, "testing long string "));
		EXPECT_EQ(0, strcmp(str10.str, teststr2));
		EXPECT_EQ(0, strcmp(str11.str, ""));

		EXPECT_EQ(0, strcmp(str12.str, teststr3));
		EXPECT_EQ(0, strcmp(str13.str, "\x01\x02\x03"));
		EXPECT_EQ(
		    0, strcmp(str14.str, "\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02"));
		EXPECT_EQ(0, strcmp(str15.str, teststr3));
		EXPECT_EQ(0, strcmp(str16.str, ""));
		EXPECT_EQ(0, str17.length);

		string_deallocate(str1.str);
		string_deallocate(str2.str);
		string_deallocate(str3.str);
		string_deallocate(str4.str);
		string_deallocate(str5.str);
		string_deallocate(str6.str);
		string_deallocate(str7.str);
		string_deallocate(str8.str);
		string_deallocate(str9.str);
		string_deallocate(str10.str);
		string_deallocate(str11.str);
		string_deallocate(str12.str);
		string_deallocate(str13.str);
		string_deallocate(str14.str);
		string_deallocate(str15.str);
		string_deallocate(str16.str);
		string_deallocate(str17.str);
	}
	{
		char conststr[] = "foobar barfoo foobar barfoo foobar barfoo foobar barfoo foobar barfoo foobar barfoo";
		EXPECT_EQ(nullptr, string_null().str);
		EXPECT_EQ(0, string_null().length);

		EXPECT_NE(nullptr, string_empty().str);
		EXPECT_EQ(0, string_empty().str[0]);
		EXPECT_EQ(0, string_empty().length);

		EXPECT_EQ(conststr, string_const(conststr, sizeof(conststr)).str);
		EXPECT_EQ(sizeof(conststr), string_const(conststr, sizeof(conststr)).length);
		EXPECT_EQ(nullptr, string_const(nullptr, sizeof(conststr)).str);
		EXPECT_EQ(0, string_const(conststr, 0).length);

		EXPECT_EQ(conststr, string_to_const(string(conststr, sizeof(conststr))).str);
		EXPECT_EQ(sizeof(conststr), string_to_const(string(conststr, sizeof(conststr))).length);
		EXPECT_EQ(nullptr, string_to_const(string(nullptr, sizeof(conststr))).str);
		EXPECT_EQ(0, string_to_const(string(conststr, 0)).length);
	}
	{
		wchar_t teststr1[] = L"test";
		wchar_t teststr2[] = L"testing long string with more than 16 characters";
		wchar_t teststr3[] =
		    L"0\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003\x0004\x0005\x0006\x0007"
		    L"\x0008\x0009\x0030\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003\x0004"
		    L"\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001"
		    L"\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009";
		wchar_t teststr4[] =
		    L"Wide-char string with some strange characters: åäöÅÄÖ_________ (test utf-8 conversions with long "
		    L"strings)";
		// char utfteststr4[] = "Wide-char string with some strange characters:
		// \xC3\xA5\xC3\xA4\xC3\xB6\xC3\x85\xC3\x84\xC3\x96\xEF\xB7\xB2\xDA\x81\xED\x80\x83\xEC\x99\xA7\xEB\x96\xAF\xE7\xBF\xB3\xE5\xA1\x8C\xE1\xA7\xBC\xE1\xA7\xAD
		// (test utf-8 conversions with long strings)";

		string_t str1, str2, str3, str5, str6, str7, str8, str9, str10, str11, str12, str13, str14, str15, str16, str17;
		wchar_t *wstr1, *wstr2, *wstr3, *wstr5, *wstr6, *wstr7, *wstr8, *wstr9, *wstr10, *wstr11, *wstr12, *wstr13,
		    *wstr14, *wstr15, *wstr16, *wstr17;
		wchar_t wbuffer[512];
		wchar_t tmpchar;

		teststr4[53] = (wchar_t)0x602f;  // L'ﷲ';
		teststr4[54] = (wchar_t)0xf045;  // L'ځ';
		teststr4[55] = (wchar_t)0x1d30;  // L'퀃';
		teststr4[56] = (wchar_t)0x0378;  // L'왧';
		teststr4[57] = (wchar_t)0x0ffe;  // L'떯';
		teststr4[58] = (wchar_t)0xffe0;  // L'翳';
		teststr4[59] = (wchar_t)0x1234;  // L'塌';
		teststr4[60] = (wchar_t)0x4321;  // L'᧼';
		teststr4[61] = (wchar_t)0x0f0f;  // L'᧭';

		str1 = string_allocate_from_wstring(teststr1, 0);
		str2 = string_allocate_from_wstring(teststr1, 3);
		str3 = string_allocate_from_wstring(teststr1, 4);
		str5 = string_allocate_from_wstring(teststr1, (sizeof(teststr1) / sizeof(teststr1[0])) - 1);

		str6 = string_allocate_from_wstring(teststr2, 0);
		str7 = string_allocate_from_wstring(teststr2, 3);
		str8 = string_allocate_from_wstring(teststr2, 20);
		str9 = string_allocate_from_wstring(teststr2, (sizeof(teststr2) / sizeof(teststr2[0])) - 1);

		str10 = string_allocate_from_wstring(teststr3, 0);
		str11 = string_allocate_from_wstring(teststr3, 3);
		str12 = string_allocate_from_wstring(teststr3, 20);
		str13 = string_allocate_from_wstring(teststr3, (sizeof(teststr3) / sizeof(teststr3[0])) - 1);

		str14 = string_allocate_from_wstring(teststr4, 0);
		str15 = string_allocate_from_wstring(teststr4, 3);
		str16 = string_allocate_from_wstring(teststr4, 63);
		str17 = string_allocate_from_wstring(teststr4, (sizeof(teststr4) / sizeof(teststr4[0])) - 1);

		wstr1 = wstring_allocate_from_string(str1.str, str1.length);
		wstr2 = wstring_allocate_from_string(str2.str, 2);
		wstr3 = wstring_allocate_from_string(str3.str, str3.length);
		wstr5 = wstring_allocate_from_string(str5.str, str5.length);

		wstr6 = wstring_allocate_from_string(str6.str, str6.length);
		wstr7 = wstring_allocate_from_string(str7.str, str7.length);
		wstr8 = wstring_allocate_from_string(str8.str, str8.length);
		wstr9 = wstring_allocate_from_string(str9.str, str9.length);

		wstr10 = wstring_allocate_from_string(str10.str, str10.length);
		wstr11 = wstring_allocate_from_string(str11.str, str11.length);
		wstr12 = wstring_allocate_from_string(str12.str, str12.length);
		wstr13 = wstring_allocate_from_string(str13.str, str13.length);

		wstr14 = wstring_allocate_from_string(str14.str, str14.length);
		wstr15 = wstring_allocate_from_string(str15.str, str15.length);
		wstr16 = wstring_allocate_from_string(str16.str, str16.length);
		wstr17 = wstring_allocate_from_string(str17.str, str17.length);

		EXPECT_TRUE(wstring_equal(wstr1, L""));
		EXPECT_TRUE(wstring_equal(wstr2, L"te"));
		EXPECT_TRUE(wstring_equal(wstr3, teststr1));
		EXPECT_TRUE(wstring_equal(wstr5, teststr1));

		EXPECT_TRUE(wstring_equal(wstr6, L""));
		EXPECT_TRUE(wstring_equal(wstr7, L"tes"));
		EXPECT_TRUE(wstring_equal(wstr8, L"testing long string "));
		EXPECT_TRUE(wstring_equal(wstr9, teststr2));

		EXPECT_TRUE(wstring_equal(wstr10, L""));
		EXPECT_TRUE(wstring_equal(wstr11, L"0\x0001\x0002"));
		EXPECT_TRUE(wstring_equal(wstr12,
		                          L"0\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003"
		                          L"\x0004\x0005\x0006\x0007\x0008\x0009"));
		EXPECT_TRUE(wstring_equal(wstr13, teststr3));

		EXPECT_TRUE(wstring_equal(wstr14, L""));
		EXPECT_TRUE(wstring_equal(wstr15, L"Wid"));
		EXPECT_TRUE(wstring_equal(wstr17, teststr4));
		tmpchar = teststr4[63];
		teststr4[63] = 0;
		EXPECT_TRUE(wstring_equal(wstr16, teststr4));
		teststr4[63] = tmpchar;

		{
			wchar_t wteststr[] = {0x0100, 0x078f, 0x1234, 0xFF03, 0xD854, 0xDC53, 0x0032, 0};
			string_t utf8_teststr = string_allocate_from_wstring(wteststr, sizeof(wteststr) / sizeof(wteststr[0]));
			wchar_t* wchar_teststr = wstring_allocate_from_string(STRING_ARGS(utf8_teststr));
			EXPECT_TRUE(wstring_equal(wteststr, wchar_teststr));
			wstring_deallocate(wchar_teststr);
			string_deallocate(utf8_teststr.str);
		}

		{
			uint16_t wteststr[] = {0xFEFF, 0x0100, 0x078f, 0x1234, 0xFF03, 0xDB02,
			                       0xDC54, 0x0032, 0xFFFE, 0x1234, 0xFF03, 0};
#if FOUNDATION_SIZE_WCHAR == 4
			wchar_t wtestcmpstr[] = {0x0100, 0x078f, 0x1234, 0xFF03, 0x000D0854, 0x0032, 0x3412, 0x03FF, 0};
#else
			wchar_t wtestcmpstr[] = {0x0100, 0x078f, 0x1234, 0xFF03, 0xDB02, 0xDC54, 0x0032, 0x3412, 0x03FF, 0};
#endif
			string_t utf8_teststr = string_allocate_from_utf16(wteststr, sizeof(wteststr) / sizeof(wteststr[0]));
			wchar_t* wchar_teststr = wstring_allocate_from_string(STRING_ARGS(utf8_teststr));
			EXPECT_TRUE(wstring_equal(wtestcmpstr, wchar_teststr));
			wstring_deallocate(wchar_teststr);
			string_deallocate(utf8_teststr.str);
		}

		{
			uint32_t wteststr[] = {0x0000FEFF, 0x00000100, 0x0000078f, 0x00001234, 0x0000FF03, 0x000D0854,
			                       0x000D0C53, 0x00000032, 0xFFFE0000, 0x12340000, 0xFF030000, 0};
#if FOUNDATION_SIZE_WCHAR == 4
			wchar_t wtestcmpstr[] = {0x0100, 0x078f, 0x1234, 0xFF03, 0x000D0854, 0x000D0C53, 0x0032, 0x3412, 0x03FF, 0};
#else
			wchar_t wtestcmpstr[] = {0x0100, 0x078f, 0x1234, 0xFF03, 0xDB02, 0xDC54,
			                         0xDB03, 0xDC53, 0x0032, 0x3412, 0x03FF, 0};
#endif
			string_t utf8_teststr = string_allocate_from_utf32(wteststr, sizeof(wteststr) / sizeof(wteststr[0]));
			wchar_t* wchar_teststr = wstring_allocate_from_string(STRING_ARGS(utf8_teststr));
			EXPECT_TRUE(wstring_equal(wtestcmpstr, wchar_teststr));
			wstring_deallocate(wchar_teststr);
			string_deallocate(utf8_teststr.str);
		}

		wstring_deallocate(wstr1);
		wstring_deallocate(wstr2);
		wstring_deallocate(wstr3);
		wstring_deallocate(wstr5);
		wstring_deallocate(wstr6);
		wstring_deallocate(wstr7);
		wstring_deallocate(wstr8);
		wstring_deallocate(wstr9);
		wstring_deallocate(wstr10);
		wstring_deallocate(wstr11);
		wstring_deallocate(wstr12);
		wstring_deallocate(wstr13);
		wstring_deallocate(wstr14);
		wstring_deallocate(wstr15);
		wstring_deallocate(wstr16);
		wstring_deallocate(wstr17);

		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str1.str, str1.length);
		EXPECT_TRUE(wstring_equal(wbuffer, L""));
		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str2.str, 2);
		EXPECT_TRUE(wstring_equal(wbuffer, L"te"));
		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str3.str, str3.length);
		EXPECT_TRUE(wstring_equal(wbuffer, teststr1));
		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str5.str, str5.length);
		EXPECT_TRUE(wstring_equal(wbuffer, teststr1));

		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str6.str, str6.length);
		EXPECT_TRUE(wstring_equal(wbuffer, L""));
		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str7.str, str7.length);
		EXPECT_TRUE(wstring_equal(wbuffer, L"tes"));
		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str8.str, str8.length);
		EXPECT_TRUE(wstring_equal(wbuffer, L"testing long string "));
		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str9.str, str9.length);
		EXPECT_TRUE(wstring_equal(wbuffer, teststr2));

		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str10.str, str10.length);
		EXPECT_TRUE(wstring_equal(wbuffer, L""));
		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str11.str, str11.length);
		EXPECT_TRUE(wstring_equal(wbuffer, L"0\x0001\x0002"));
		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str12.str, str12.length);
		EXPECT_TRUE(wstring_equal(wbuffer,
		                          L"0\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003"
		                          L"\x0004\x0005\x0006\x0007\x0008\x0009"));
		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str13.str, str13.length);
		EXPECT_TRUE(wstring_equal(wbuffer, teststr3));

		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str14.str, str14.length);
		EXPECT_TRUE(wstring_equal(wbuffer, L""));
		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str15.str, str15.length);
		EXPECT_TRUE(wstring_equal(wbuffer, L"Wid"));
		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str17.str, str17.length);
		EXPECT_TRUE(wstring_equal(wbuffer, teststr4));
		wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), str16.str, str16.length);
		teststr4[63] = 0;
		EXPECT_TRUE(wstring_equal(wbuffer, teststr4));

		{
			wchar_t wteststr[] = {0x0100, 0x078f, 0x1234, 0xFF03, 0xD854, 0xDC53, 0x0032, 0};
			string_t utf8_teststr = string_allocate_from_wstring(wteststr, sizeof(wteststr) / sizeof(wteststr[0]));
			wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), STRING_ARGS(utf8_teststr));
			EXPECT_TRUE(wstring_equal(wteststr, wbuffer));
			string_deallocate(utf8_teststr.str);
		}

		{
			uint16_t wteststr[] = {0xFEFF, 0x0100, 0x078f, 0x1234, 0xFF03, 0xDB02,
			                       0xDC54, 0x0032, 0xFFFE, 0x1234, 0xFF03, 0};
#if FOUNDATION_SIZE_WCHAR == 4
			wchar_t wtestcmpstr[] = {0x0100, 0x078f, 0x1234, 0xFF03, 0x000D0854, 0x0032, 0x3412, 0x03FF, 0};
#else
			wchar_t wtestcmpstr[] = {0x0100, 0x078f, 0x1234, 0xFF03, 0xDB02, 0xDC54, 0x0032, 0x3412, 0x03FF, 0};
#endif
			string_t utf8_teststr = string_allocate_from_utf16(wteststr, sizeof(wteststr) / sizeof(wteststr[0]));
			wstring_from_string(wbuffer, sizeof(wbuffer) / sizeof(wbuffer[0]), STRING_ARGS(utf8_teststr));
			EXPECT_TRUE(wstring_equal(wtestcmpstr, wbuffer));
			EXPECT_EQ(wstring_length(wbuffer), (sizeof(wtestcmpstr) / sizeof(wtestcmpstr[0])) - 1);
			string_deallocate(utf8_teststr.str);
		}

		{
			size_t expected_length;
			uint32_t wteststr[] = {0x0000FEFF, 0x00000100, 0x0000078f, 0x00001234, 0x0000FF03, 0x000D0854,
			                       0x000D0C53, 0x00000032, 0xFFFE0000, 0x12340000, 0xFF030000, 0};
#if FOUNDATION_SIZE_WCHAR == 4
			wchar_t wtestcmpstr[] = {0x0100, 0x078f, 0x1234, 0xFF03, 0x000D0854, 0x000D0C53, 0x0032, 0x3412, 0x03FF, 0};
#else
			wchar_t wtestcmpstr[] = {0x0100, 0x078f, 0x1234, 0xFF03, 0xDB02, 0xDC54,
			                         0xDB03, 0xDC53, 0x0032, 0x3412, 0x03FF, 0};
#endif
			string_t utf8_teststr = string_allocate_from_utf32(wteststr, sizeof(wteststr) / sizeof(wteststr[0]));
			wstring_from_string(wbuffer, 6, STRING_ARGS(utf8_teststr));
#if FOUNDATION_SIZE_WCHAR == 4
			expected_length = 5;
#else
			// Final glyph > 0x10FFFF will not be output since it requires
			// two 16-bit wide characters which will not fit in given buffer
			expected_length = 4;
			EXPECT_EQ(wbuffer[4], 0);
#endif
			wtestcmpstr[expected_length] = 0;
			EXPECT_TRUE(wstring_equal(wtestcmpstr, wbuffer));
			EXPECT_EQ(wstring_length(wbuffer), expected_length);
			string_deallocate(utf8_teststr.str);
		}

		EXPECT_EQ(wstring_length(0), 0);

		wbuffer[0] = 1;
		wstring_from_string(wbuffer, 0, str16.str, str16.length);
		EXPECT_EQ(wbuffer[0], 1);

		wbuffer[0] = 1;
		wstring_from_string(wbuffer, 1, str16.str, str16.length);
		EXPECT_EQ(wbuffer[0], 0);

		string_deallocate(str1.str);
		string_deallocate(str2.str);
		string_deallocate(str3.str);
		string_deallocate(str5.str);
		string_deallocate(str6.str);
		string_deallocate(str7.str);
		string_deallocate(str8.str);
		string_deallocate(str9.str);
		string_deallocate(str10.str);
		string_deallocate(str11.str);
		string_deallocate(str12.str);
		string_deallocate(str13.str);
		string_deallocate(str14.str);
		string_deallocate(str15.str);
		string_deallocate(str16.str);
		string_deallocate(str17.str);
	}
	{
		char* buffer;
		string_t result;

		result = string_resize(nullptr, 0, 0, 8, ' ');
		EXPECT_STRINGEQ(result, string_const(STRING_CONST("        ")));
		EXPECT_EQ(result.str[result.length], 0);
		string_deallocate(result.str);

		buffer = memory_allocate(0, 64, 0, MEMORY_PERSISTENT);
		result = string_resize(buffer, 0, 64, 8, ' ');
		EXPECT_STRINGEQ(result, string_const(STRING_CONST("        ")));
		EXPECT_EQ(result.str[result.length], 0);
		memory_deallocate(buffer);

		buffer = memory_allocate(0, 8, 0, MEMORY_PERSISTENT);
		result = string_resize(buffer, 0, 8, 8, ' ');
		EXPECT_STRINGEQ(result, string_const(STRING_CONST("        ")));
		EXPECT_EQ(result.str[result.length], 0);
		string_deallocate(result.str);

		buffer = memory_allocate(0, 8, 0, MEMORY_PERSISTENT);
		string_copy(buffer, 8, STRING_CONST("        "));
		result = string_resize(buffer, 7, 8, 8, ' ');
		EXPECT_STRINGEQ(result, string_const(STRING_CONST("        ")));
		EXPECT_EQ(result.str[result.length], 0);
		string_deallocate(result.str);

		buffer = memory_allocate(0, 8, 0, MEMORY_PERSISTENT);
		memset(buffer, ' ', 8);
		result = string_resize(buffer, 8, 8, 12, 'a');
		EXPECT_STRINGEQ(result, string_const(STRING_CONST("        aaaa")));
		EXPECT_EQ(result.str[result.length], 0);
		string_deallocate(result.str);

		buffer = memory_allocate(0, 8, 0, MEMORY_PERSISTENT);
		memset(buffer, ' ', 8);
		result = string_resize(buffer, 8, 8, 8, 'a');
		EXPECT_STRINGEQ(result, string_const(STRING_CONST("        ")));
		EXPECT_EQ(result.str[result.length], 0);
		string_deallocate(result.str);

		buffer = memory_allocate(0, 32, 0, MEMORY_PERSISTENT);
		string_copy(buffer, 32, STRING_CONST("       "));
		result = string_resize(buffer, 7, 32, 8, ' ');
		EXPECT_STRINGEQ(result, string_const(STRING_CONST("        ")));
		EXPECT_EQ(result.str, buffer);
		EXPECT_EQ(result.str[result.length], 0);
		memory_deallocate(buffer);
	}
	return 0;
}

DECLARE_TEST(string, queries) {
	{
		EXPECT_EQ(string_length(nullptr), 0);
		EXPECT_EQ(string_length(""), 0);
		EXPECT_EQ(string_length("test"), 4);
		EXPECT_EQ(string_length("test\0test"), 4);
		EXPECT_EQ(string_length("test test test test test test test test test test test test test test "), 70);
	}
	{
		//"®᧼aҖ<BOM>𤭢b<INV>c" where <BOM> is byte order mark, will be treated as two glyphs,
		// and <INV> is an utf-8 invalid 6-byte sequence (but we treat is as one glyph, like wtf-8)
		unsigned char utfstr[] = {0xC2, 0xAE, 0xE1, 0xA7, 0xBC, 0x61, 0xD2, 0x96, 0xFE, 0xFF, 0xF0, 0xA4,
		                          0xAD, 0xA2, 0x62, 0xFC, 0xA4, 0xA3, 0xA2, 0xA0, 0xA1, 0x63, 0};
		// Invalid, byte sequence is incomplete, but should be safe in string_glyphs/string_glyph calls
		unsigned char invalidstr[] = {0xFC, 0xA4, 0};
		unsigned char twostr[] = {0xFC, 0xA4, 0xA3, 0xA2, 0xA0, 0xA1, 0x62, 0};
		unsigned char bomstr[] = {0xFF, 0xFE, 0xFC, 0xA4, 0xA3, 0xA2, 0xA0, 0xA1, 0x62, 0};
		size_t consumed, offset;

		EXPECT_EQ(string_glyphs(nullptr, 0), 0);
		EXPECT_EQ(string_glyphs("foobar", 0), 0);
		EXPECT_EQ(string_glyphs("foobar", 4), 4);
		EXPECT_INTEQ(string_glyphs((const char*)invalidstr, sizeof(invalidstr) - 1), 1);
		EXPECT_INTEQ(string_glyphs((const char*)twostr, sizeof(twostr) - 1), 2);
		EXPECT_INTEQ(string_glyphs((const char*)bomstr, sizeof(bomstr) - 1), 4);
		EXPECT_INTEQ(string_glyphs((const char*)utfstr, sizeof(utfstr) - 1), 10);

		offset = 0;
		EXPECT_EQ(string_glyph((const char*)utfstr, sizeof(utfstr) - 1, offset, &consumed), 0xAE);  //'®'
		EXPECT_EQ(consumed, 2);
		offset += consumed;
		EXPECT_EQ(string_glyph((const char*)utfstr, sizeof(utfstr) - 1, offset, &consumed), 0x19FC);  // L'᧼'
		EXPECT_EQ(consumed, 3);
		offset += consumed;
		EXPECT_EQ(string_glyph((const char*)utfstr, sizeof(utfstr) - 1, offset, &consumed), (uint32_t)L'a');
		EXPECT_EQ(consumed, 1);
		offset += consumed;
		EXPECT_EQ(string_glyph((const char*)utfstr, sizeof(utfstr) - 1, offset, &consumed), 0x496);  // L'Җ'
		EXPECT_EQ(consumed, 2);
		offset += consumed;
		EXPECT_EQ(string_glyph((const char*)utfstr, sizeof(utfstr) - 1, offset, &consumed), (uint32_t)0xFE & 0x3F);
		EXPECT_EQ(consumed, 1);
		offset += consumed;
		EXPECT_EQ(string_glyph((const char*)utfstr, sizeof(utfstr) - 1, offset, &consumed), (uint32_t)0xFF & 0x3F);
		EXPECT_EQ(consumed, 1);
		offset += consumed;
		EXPECT_EQ(string_glyph((const char*)utfstr, sizeof(utfstr) - 1, offset, &consumed), 0x24B62);  //'𤭢'
		EXPECT_EQ(consumed, 4);
		offset += consumed;
		EXPECT_EQ(string_glyph((const char*)utfstr, sizeof(utfstr) - 1, offset, &consumed), (uint32_t)L'b');
		EXPECT_EQ(consumed, 1);
		offset += consumed;
		EXPECT_EQ(string_glyph((const char*)utfstr, sizeof(utfstr) - 1, offset, &consumed), (uint32_t)0x248E2821);
		EXPECT_EQ(consumed, 6);
		offset += consumed;
		EXPECT_EQ(string_glyph((const char*)utfstr, sizeof(utfstr) - 1, offset, &consumed), (uint32_t)L'c');
		EXPECT_EQ(consumed, 1);
		offset += consumed;
		EXPECT_EQ(string_glyph((const char*)utfstr, sizeof(utfstr) - 1, offset, &consumed), 0);
		EXPECT_EQ(consumed, 0);

		offset = 0;
		EXPECT_INTEQ(string_glyph((const char*)invalidstr, sizeof(invalidstr) - 1, offset, &consumed), 0x24000000);
		EXPECT_EQ(consumed, 2);
		offset += consumed;
		EXPECT_EQ(string_glyph((const char*)invalidstr, sizeof(invalidstr) - 1, offset, &consumed), 0);
		EXPECT_EQ(consumed, 0);
	}
	{
		EXPECT_EQ(string_hash(nullptr, 0), HASH_EMPTY_STRING);
		EXPECT_EQ(string_hash("", 0), HASH_EMPTY_STRING);
		EXPECT_EQ(string_hash("foobar", 0), HASH_EMPTY_STRING);
		EXPECT_EQ(string_hash(STRING_CONST("")), HASH_EMPTY_STRING);
		EXPECT_EQ(string_hash(STRING_CONST("foundation")), HASH_FOUNDATION);
		EXPECT_EQ(string_hash("foundation string", 10), HASH_FOUNDATION);
	}
	{
		char teststr[] = "";
		char teststr2[] = "test";
		char teststr3[] = "testing long string with more than 16 characters";
		char teststr4[] = "01234567890123456789012345678901234567890123456789012345678901234567890123456789";

		string_const_t str1 = string_substr(STRING_CONST(teststr), 0, 0);
		string_const_t str2 = string_substr(STRING_CONST(teststr2), 0, 4);
		string_const_t str3 = string_substr(STRING_CONST(teststr2), 0, 20);
		string_const_t str4 = string_substr(STRING_CONST(teststr3), 0, 0);
		string_const_t str5 = string_substr(STRING_CONST(teststr3), 4, 10);
		string_const_t str6 = string_substr(STRING_CONST(teststr3), 0, STRING_NPOS);
		string_const_t str7 = string_substr(STRING_CONST(teststr4), 10, 0);
		string_const_t str8 = string_substr(STRING_CONST(teststr4), 0, 20);
		string_const_t str9 = string_substr(STRING_CONST(teststr4), 10, STRING_NPOS);

		EXPECT_SIZEEQ(strlen(teststr), str1.length);
		EXPECT_SIZEEQ(strlen(teststr2), str2.length);
		EXPECT_SIZEEQ(strlen(teststr2), str3.length);
		EXPECT_SIZEEQ(0, str4.length);
		EXPECT_SIZEEQ(10, str5.length);
		EXPECT_SIZEEQ(strlen(teststr3), str6.length);
		EXPECT_SIZEEQ(0U, str7.length);
		EXPECT_SIZEEQ(20, str8.length);
		EXPECT_SIZEEQ(strlen(teststr4) - 10, str9.length);
	}
	{
		// Only ASCII characters, so wstring->string conversion should not introduce any extra UTF-8 sequences
		wchar_t teststr[] = L"";
		wchar_t teststr2[] = L"test";
		wchar_t teststr3[] = L"testing long string with more than 16 characters";
		wchar_t teststr4[] = L"01234567890123456789012345678901234567890123456789012345678901234567890123456789";

		string_t str1 = string_allocate_from_wstring(teststr, (sizeof(teststr) / sizeof(teststr[0])) - 1);
		string_t str2 = string_allocate_from_wstring(teststr2, (sizeof(teststr2) / sizeof(teststr2[0])) - 1);
		string_t str3 = string_allocate_from_wstring(teststr3, 0);
		string_t str4 = string_allocate_from_wstring(teststr3, 10);
		string_t str5 = string_allocate_from_wstring(teststr3, (sizeof(teststr3) / sizeof(teststr3[0])) - 1);
		string_t str6 = string_allocate_from_wstring(teststr4, (sizeof(teststr4) / sizeof(teststr4[0])) - 1);
		string_t str7 = string_allocate_from_wstring(teststr4, 20);

		EXPECT_EQ(0, str1.str[str1.length]);
		EXPECT_EQ(0, str2.str[str2.length]);
		EXPECT_EQ(0, str3.str[str3.length]);
		EXPECT_EQ(0, str4.str[str4.length]);
		EXPECT_EQ(0, str5.str[str5.length]);
		EXPECT_EQ(0, str6.str[str6.length]);
		EXPECT_EQ(0, str7.str[str7.length]);

		EXPECT_SIZEEQ(wcslen(teststr), str1.length);
		EXPECT_SIZEEQ(wcslen(teststr2), str2.length);
		EXPECT_SIZEEQ(0, str3.length);
		EXPECT_SIZEEQ(10, str4.length);
		EXPECT_SIZEEQ(wcslen(teststr3), str5.length);
		EXPECT_SIZEEQ(wcslen(teststr4), str6.length);
		EXPECT_SIZEEQ(20, str7.length);

		string_deallocate(str1.str);
		string_deallocate(str2.str);
		string_deallocate(str3.str);
		string_deallocate(str4.str);
		string_deallocate(str5.str);
		string_deallocate(str6.str);
		string_deallocate(str7.str);
	}
	{
		string_const_t nullstr = string_null();
		EXPECT_TRUE(string_equal(STRING_CONST("foo"), STRING_CONST("foo")));
		EXPECT_TRUE(string_equal(STRING_CONST("foo bar foo bar foo bar foo bar foo bar foo bar"),
		                         STRING_CONST("foo bar foo bar foo bar foo bar foo bar foo bar")));
		EXPECT_FALSE(string_equal(STRING_CONST("foo"), STRING_CONST("Foo")));
		EXPECT_FALSE(string_equal(STRING_CONST("bfoo"), STRING_CONST("foo")));
		EXPECT_TRUE(string_equal(STRING_ARGS(nullstr), STRING_CONST("")));
		EXPECT_TRUE(string_equal(STRING_CONST(""), STRING_ARGS(nullstr)));
		EXPECT_FALSE(string_equal(STRING_CONST(" string"), STRING_ARGS(nullstr)));
		EXPECT_FALSE(string_equal(STRING_ARGS(nullstr), STRING_CONST("0")));

		EXPECT_TRUE(string_equal_nocase(STRING_CONST("foo"), STRING_CONST("Foo")));
		EXPECT_TRUE(string_equal_nocase(STRING_CONST("foo bar foo Bar foo bar foo bar foo bar foo baR"),
		                                STRING_CONST("Foo Bar foo bar foo bar foo bar FOO bar foo bar")));
		EXPECT_FALSE(string_equal_nocase(STRING_CONST("foo"), STRING_CONST("Foob")));
		EXPECT_FALSE(string_equal_nocase(STRING_CONST("bfoo"), STRING_CONST("foo")));
		EXPECT_TRUE(string_equal_nocase(STRING_ARGS(nullstr), STRING_CONST("")));
		EXPECT_TRUE(string_equal_nocase(STRING_CONST(""), STRING_ARGS(nullstr)));
		EXPECT_FALSE(string_equal_nocase(STRING_CONST(" String"), STRING_ARGS(nullstr)));
		EXPECT_FALSE(string_equal_nocase(STRING_ARGS(nullstr), STRING_CONST("0")));

		EXPECT_TRUE(string_equal_substr(STRING_CONST("foo"), 1, STRING_CONST("foo"), 1));
		EXPECT_FALSE(string_equal_substr(STRING_CONST("foo"), 1, STRING_CONST("foo"), 2));
		EXPECT_TRUE(string_equal_substr(STRING_CONST("foo"), 4, STRING_CONST("foo"), STRING_NPOS));
		EXPECT_FALSE(string_equal_substr(STRING_CONST("foo"), 0, STRING_CONST("Foo"), 0));
		EXPECT_TRUE(string_equal_substr(STRING_CONST("foo"), 0, STRING_CONST("foo"), 0));
		EXPECT_TRUE(string_equal_substr(STRING_ARGS(string_empty()), 0, STRING_ARGS(string_null()), 0));

		EXPECT_TRUE(string_equal_substr_nocase(STRING_CONST("foo"), 1, STRING_CONST("FoO"), 1));
		EXPECT_FALSE(string_equal_substr_nocase(STRING_CONST("Foo"), 1, STRING_CONST("foo"), 2));
		EXPECT_TRUE(string_equal_substr_nocase(STRING_CONST("Foo"), 4, STRING_CONST("foo"), STRING_NPOS));
		EXPECT_TRUE(string_equal_substr_nocase(STRING_CONST("foo"), 0, STRING_CONST("Foo"), 0));
		EXPECT_TRUE(string_equal_substr_nocase(STRING_CONST("FoO"), 0, STRING_CONST("foo"), 0));
		EXPECT_TRUE(string_equal_substr_nocase(STRING_ARGS(string_empty()), 0, STRING_ARGS(string_null()), 0));
	}
	{
		string_const_t emptystr = string_null();
		string_const_t shortstr = string_const(STRING_CONST("short string"));
		string_const_t longstr =
		    string_const(STRING_CONST("testing utility string methods like finds, split, merge, explode and similar."));
		{
			size_t find = string_find(STRING_ARGS(longstr), ' ', 0);
			size_t find2 = string_find(STRING_ARGS(longstr), 12, 0);
			size_t find3 = string_find(STRING_ARGS(emptystr), ' ', 0);
			size_t find4 = string_find(STRING_ARGS(shortstr), ' ', 0);
			size_t find5 = string_find(STRING_ARGS(shortstr), 'z', 0);
			size_t findofs = string_find(STRING_ARGS(longstr), ' ', find);
			size_t findofs2 = string_find(STRING_ARGS(longstr), ' ', find + 1);
			size_t findofs3 = string_find(STRING_ARGS(longstr), 'z', 10);
			size_t findofs4 = string_find(STRING_ARGS(emptystr), 'z', STRING_NPOS);
			size_t findofs5 = string_find(STRING_ARGS(shortstr), 's', 10);
			size_t findofs6 = string_find(STRING_ARGS(shortstr), 's', findofs5);
			size_t findofs7 = string_find(STRING_ARGS(shortstr), 't', 0);
			size_t findofs8 = string_find(STRING_ARGS(shortstr), ' ', 5);
			size_t findofs9 = string_find(STRING_ARGS(longstr), ' ', STRING_NPOS);

			EXPECT_EQ(find, 7U);
			EXPECT_EQ(find2, STRING_NPOS);
			EXPECT_EQ(find3, STRING_NPOS);
			EXPECT_EQ(find4, 5U);
			EXPECT_EQ(find5, STRING_NPOS);
			EXPECT_EQ(findofs, find);
			EXPECT_EQ(findofs2, 15U);
			EXPECT_EQ(findofs3, STRING_NPOS);
			EXPECT_EQ(findofs4, STRING_NPOS);
			EXPECT_EQ(findofs5, STRING_NPOS);
			EXPECT_EQ(findofs6, STRING_NPOS);
			EXPECT_EQ(findofs7, 4U);
			EXPECT_EQ(findofs8, 5U);
			EXPECT_EQ(findofs9, STRING_NPOS);
		}
		{
			size_t rfind = string_rfind(STRING_ARGS(longstr), ' ', STRING_NPOS);
			size_t rfind2 = string_rfind(STRING_ARGS(longstr), ';', STRING_NPOS);
			size_t rfind3 = string_rfind(STRING_ARGS(emptystr), ';', STRING_NPOS);
			size_t rfind4 = string_rfind(STRING_ARGS(shortstr), 's', STRING_NPOS);
			size_t rfind5 = string_rfind(STRING_ARGS(shortstr), 'z', STRING_NPOS);
			size_t rfindofs = string_rfind(STRING_ARGS(longstr), ' ', rfind);
			size_t rfindofs2 = string_rfind(STRING_ARGS(longstr), ' ', rfind - 1);
			size_t rfindofs3 = string_rfind(STRING_ARGS(longstr), ' ', longstr.length - 1);
			size_t rfindofs4 = string_rfind(STRING_ARGS(emptystr), ' ', 0);
			size_t rfindofs5 = string_rfind(STRING_ARGS(longstr), ' ', 0);
			size_t rfindofs6 = string_rfind(STRING_ARGS(shortstr), 's', 5);
			size_t rfindofs7 = string_rfind(STRING_ARGS(shortstr), 's', 0);

			EXPECT_EQ(rfind, 68U);
			EXPECT_EQ(rfind2, STRING_NPOS);
			EXPECT_EQ(rfind3, STRING_NPOS);
			EXPECT_EQ(rfind4, 6U);
			EXPECT_EQ(rfind5, STRING_NPOS);
			EXPECT_EQ(rfindofs, rfind);
			EXPECT_EQ(rfindofs2, 64U);
			EXPECT_EQ(rfindofs3, rfind);
			EXPECT_EQ(rfindofs4, STRING_NPOS);
			EXPECT_EQ(rfindofs5, STRING_NPOS);
			EXPECT_EQ(rfindofs6, 0U);
			EXPECT_EQ(rfindofs7, 0U);
		}
		{
			size_t findstr = string_find_string(STRING_ARGS(longstr), STRING_CONST("st"), 0);
			size_t findstr2 = string_find_string(STRING_ARGS(longstr), STRING_CONST("xwqz"), 0);
			size_t findstr3 = string_find_string(STRING_ARGS(emptystr), STRING_CONST("xwqz"), 0);
			size_t findstr4 = string_find_string(STRING_ARGS(longstr), STRING_CONST(""), 0);
			size_t findstr5 = string_find_string(STRING_ARGS(longstr), STRING_CONST("dslike"), 0);
			size_t findstr6 = string_find_string(STRING_ARGS(shortstr), STRING_CONST("rt"), 0);
			size_t findstr7 =
			    string_find_string(STRING_ARGS(shortstr), STRING_CONST("long key that does not exist"), 0);
			size_t findstr8 = string_find_string(STRING_ARGS(shortstr), STRING_CONST("so"), 0);
			size_t findstrofs = string_find_string(STRING_ARGS(longstr), STRING_CONST("st"), findstr);
			size_t findstrofs2 = string_find_string(STRING_ARGS(longstr), STRING_CONST("st"), findstr + 1);
			size_t findstrofs3 = string_find_string(STRING_ARGS(longstr), STRING_CONST("xwqz"), longstr.length);
			size_t findstrofs4 = string_find_string(STRING_ARGS(emptystr), STRING_CONST("xwqz"), emptystr.length);
			size_t findstrofs5 = string_find_string(STRING_ARGS(shortstr), STRING_CONST(""), 5);
			size_t findstrofs6 = string_find_string(STRING_ARGS(shortstr), STRING_CONST("string"), 0);
			size_t findstrofs7 = string_find_string(STRING_ARGS(shortstr), STRING_CONST("string"), 7);
			size_t findstrofs8 = string_find_string(STRING_ARGS(longstr), STRING_CONST("utility"), 14);
			size_t findstrofs9 = string_find_string(STRING_ARGS(longstr), STRING_CONST(""), longstr.length);
			size_t findstrofs10 = string_find_string(STRING_ARGS(longstr), STRING_CONST(""), STRING_NPOS);
			size_t findstrofs11 = string_find_string(STRING_ARGS(longstr), STRING_CONST("string"), STRING_NPOS);

			EXPECT_EQ(findstr, 2U);
			EXPECT_EQ(findstr2, STRING_NPOS);
			EXPECT_EQ(findstr3, STRING_NPOS);
			EXPECT_EQ(findstr4, 0U);
			EXPECT_EQ(findstr5, STRING_NPOS);
			EXPECT_EQ(findstr6, 3U);
			EXPECT_EQ(findstr7, STRING_NPOS);
			EXPECT_EQ(findstr8, STRING_NPOS);
			EXPECT_EQ(findstrofs, findstr);
			EXPECT_EQ(findstrofs2, 16U);
			EXPECT_EQ(findstrofs3, STRING_NPOS);
			EXPECT_EQ(findstrofs4, STRING_NPOS);
			EXPECT_EQ(findstrofs5, 5U);
			EXPECT_EQ(findstrofs6, 6U);
			EXPECT_EQ(findstrofs7, STRING_NPOS);
			EXPECT_EQ(findstrofs8, STRING_NPOS);
			EXPECT_EQ(findstrofs9, longstr.length);
			EXPECT_EQ(findstrofs10, STRING_NPOS);
			EXPECT_EQ(findstrofs11, STRING_NPOS);
		}
		{
			size_t rfindstr = string_rfind_string(STRING_ARGS(longstr), STRING_CONST(", "), STRING_NPOS);
			size_t rfindstr2 = string_rfind_string(STRING_ARGS(longstr), STRING_CONST(":;"), STRING_NPOS);
			size_t rfindstr3 = string_rfind_string(STRING_ARGS(emptystr), STRING_CONST(":;"), STRING_NPOS);
			size_t rfindstr4 = string_rfind_string(STRING_ARGS(longstr), STRING_CONST(""), STRING_NPOS);
			size_t rfindstr5 = string_rfind_string(STRING_ARGS(shortstr), STRING_CONST("string"), STRING_NPOS);
			size_t rfindstr6 = string_rfind_string(STRING_ARGS(shortstr), STRING_CONST(" tring"), STRING_NPOS);
			size_t rfindstrofs = string_rfind_string(STRING_ARGS(longstr), STRING_CONST(", "), rfindstr);
			size_t rfindstrofs2 = string_rfind_string(STRING_ARGS(longstr), STRING_CONST(", "), rfindstr - 1);
			size_t rfindstrofs3 = string_rfind_string(STRING_ARGS(longstr), STRING_CONST(":;"), 0);
			size_t rfindstrofs4 = string_rfind_string(STRING_ARGS(emptystr), STRING_CONST(":;"), 0);
			size_t rfindstrofs5 = string_rfind_string(STRING_ARGS(longstr), STRING_CONST(""), 5);
			size_t rfindstrofs6 = string_rfind_string(STRING_ARGS(shortstr), STRING_CONST("ort str"), 6);
			size_t rfindstrofs7 = string_rfind_string(STRING_ARGS(shortstr), STRING_CONST("ort str"), 1);

			EXPECT_EQ(rfindstr, 55U);
			EXPECT_EQ(rfindstr2, STRING_NPOS);
			EXPECT_EQ(rfindstr3, STRING_NPOS);
			EXPECT_EQ(rfindstr4, longstr.length);
			EXPECT_EQ(rfindstr5, 6U);
			EXPECT_EQ(rfindstr6, STRING_NPOS);
			EXPECT_EQ(rfindstrofs, rfindstr);
			EXPECT_EQ(rfindstrofs2, 48U);
			EXPECT_EQ(rfindstrofs3, STRING_NPOS);
			EXPECT_EQ(rfindstrofs4, STRING_NPOS);
			EXPECT_EQ(rfindstrofs5, 5U);
			EXPECT_EQ(rfindstrofs6, 2U);
			EXPECT_EQ(rfindstrofs7, STRING_NPOS);
		}
		{
			size_t findof = string_find_first_of(STRING_ARGS(longstr), STRING_CONST("ui"), 0);
			size_t findof2 = string_find_first_of(STRING_ARGS(longstr), STRING_CONST(";:"), 0);
			size_t findof3 = string_find_first_of(STRING_ARGS(emptystr), STRING_CONST(""), 0);
			size_t findof4 = string_find_first_of(STRING_ARGS(emptystr), STRING_CONST(" "), 0);
			size_t findof5 = string_find_first_of(STRING_ARGS(shortstr), STRING_CONST(""), 0);
			size_t findofofs = string_find_first_of(STRING_ARGS(longstr), STRING_CONST("ui"), findof);
			size_t findofofs2 = string_find_first_of(STRING_ARGS(longstr), STRING_CONST("ui"), findof - 1);
			size_t findofofs3 = string_find_first_of(STRING_ARGS(longstr), STRING_CONST("ui"), findof + 1);
			size_t findofofs4 = string_find_first_of(STRING_ARGS(longstr), STRING_CONST("ui"), longstr.length);
			size_t findofofs5 = string_find_first_of(STRING_ARGS(emptystr), STRING_CONST(""), emptystr.length);
			size_t findofofs6 = string_find_first_of(STRING_ARGS(shortstr), STRING_CONST("string"), 6);
			size_t findofofs7 = string_find_first_of(STRING_ARGS(shortstr), STRING_CONST(""), 6);
			size_t findofofs8 = string_find_first_of(STRING_ARGS(longstr), STRING_CONST(""), 10);
			size_t findofofs9 = string_find_first_of(STRING_ARGS(longstr), STRING_CONST(""), longstr.length);

			EXPECT_EQ(findof, 4U);
			EXPECT_EQ(findof2, STRING_NPOS);
			EXPECT_EQ(findof3, STRING_NPOS);
			EXPECT_EQ(findof4, STRING_NPOS);
			EXPECT_EQ(findof5, STRING_NPOS);
			EXPECT_EQ(findofofs, 4U);
			EXPECT_EQ(findofofs2, 4U);
			EXPECT_EQ(findofofs3, 8U);
			EXPECT_EQ(findofofs4, STRING_NPOS);
			EXPECT_EQ(findofofs5, STRING_NPOS);
			EXPECT_EQ(findofofs6, 6U);
			EXPECT_EQ(findofofs7, STRING_NPOS);
			EXPECT_EQ(findofofs8, STRING_NPOS);
			EXPECT_EQ(findofofs9, STRING_NPOS);
		}
		{
			size_t findnotof = string_find_first_not_of(STRING_ARGS(longstr), STRING_CONST("testing "), 0);
			size_t findnotof2 = string_find_first_not_of(STRING_ARGS(longstr), STRING_ARGS(longstr), 0);
			size_t findnotof3 = string_find_first_not_of(STRING_ARGS(shortstr), STRING_CONST(""), 0);
			size_t findnotofofs = string_find_first_not_of(STRING_ARGS(longstr), STRING_CONST("testing "), findnotof);
			size_t findnotofofs2 =
			    string_find_first_not_of(STRING_ARGS(longstr), STRING_CONST("testing "), findnotof + 1);
			size_t findnotofofs3 =
			    string_find_first_not_of(STRING_ARGS(longstr), STRING_CONST("testing "), longstr.length);
			size_t findnotofofs4 = string_find_first_not_of(STRING_ARGS(shortstr), STRING_CONST(""), shortstr.length);

			EXPECT_EQ(findnotof, 8U);
			EXPECT_EQ(findnotof2, STRING_NPOS);
			EXPECT_EQ(findnotof3, 0U);
			EXPECT_EQ(findnotofofs, 8U);
			EXPECT_EQ(findnotofofs2, 11U);
			EXPECT_EQ(findnotofofs3, STRING_NPOS);
			EXPECT_EQ(findnotofofs4, STRING_NPOS);
		}
		{
			size_t findlastof = string_find_last_of(STRING_ARGS(longstr), STRING_CONST("xp"), STRING_NPOS);
			size_t findlastof2 = string_find_last_of(STRING_ARGS(longstr), STRING_CONST(";:"), STRING_NPOS);
			size_t findlastof3 = string_find_last_of(STRING_ARGS(emptystr), STRING_CONST(""), STRING_NPOS);
			size_t findlastof4 = string_find_last_of(STRING_ARGS(shortstr), STRING_CONST(""), STRING_NPOS);
			size_t findlastofofs = string_find_last_of(STRING_ARGS(longstr), STRING_CONST("xp"), findlastof);
			size_t findlastofofs2 = string_find_last_of(STRING_ARGS(longstr), STRING_CONST("xp"), findlastof - 2);
			size_t findlastofofs3 = string_find_last_of(STRING_ARGS(longstr), STRING_CONST("xp"), 0);
			size_t findlastofofs4 = string_find_last_of(STRING_ARGS(emptystr), STRING_CONST(""), 0);
			size_t findlastofofs5 = string_find_last_of(STRING_ARGS(shortstr), STRING_CONST(""), 5);
			size_t findlastofofs6 = string_find_last_of(STRING_ARGS(shortstr), STRING_CONST(""), shortstr.length);
			size_t findlastofofs7 = string_find_last_of(STRING_ARGS(shortstr), STRING_CONST("short"), 5);

			EXPECT_EQ(findlastof, 59U);
			EXPECT_EQ(findlastof2, STRING_NPOS);
			EXPECT_EQ(findlastof3, STRING_NPOS);
			EXPECT_EQ(findlastof4, STRING_NPOS);
			EXPECT_EQ(findlastofofs, 59U);
			EXPECT_EQ(findlastofofs2, 44U);
			EXPECT_EQ(findlastofofs3, STRING_NPOS);
			EXPECT_EQ(findlastofofs4, STRING_NPOS);
			EXPECT_EQ(findlastofofs5, STRING_NPOS);
			EXPECT_EQ(findlastofofs6, STRING_NPOS);
			EXPECT_EQ(findlastofofs7, 4U);
		}
		{
			size_t findlastnotof =
			    string_find_last_not_of(STRING_ARGS(longstr), STRING_CONST(" similar."), STRING_NPOS);
			size_t findlastnotof2 = string_find_last_not_of(STRING_ARGS(longstr), STRING_ARGS(longstr), STRING_NPOS);
			size_t findlastnotof3 = string_find_last_not_of(STRING_ARGS(emptystr), STRING_CONST(""), STRING_NPOS);
			size_t findlastnotof4 = string_find_last_not_of(STRING_ARGS(shortstr), STRING_CONST(""), STRING_NPOS);
			size_t findlastnotof5 =
			    string_find_last_not_of(STRING_ARGS(longstr), STRING_CONST(" similar"), STRING_NPOS);
			size_t findlastnotofofs =
			    string_find_last_not_of(STRING_ARGS(longstr), STRING_CONST(" similar."), findlastnotof);
			size_t findlastnotofofs2 =
			    string_find_last_not_of(STRING_ARGS(longstr), STRING_CONST(" and similar."), findlastnotof - 1);
			size_t findlastnotofofs3 = string_find_last_not_of(STRING_ARGS(longstr), STRING_ARGS(longstr), 0);
			size_t findlastnotofofs4 = string_find_last_not_of(STRING_ARGS(emptystr), STRING_CONST(""), 0);
			size_t findlastnotofofs5 = string_find_last_not_of(STRING_ARGS(shortstr), STRING_CONST("string"), 5);
			size_t findlastnotofofs6 = string_find_last_not_of(STRING_ARGS(shortstr), STRING_CONST("string "), 5);
			size_t findlastnotofofs7 = string_find_last_not_of(STRING_ARGS(shortstr), STRING_CONST(""), 5);
			size_t findlastnotofofs8 = string_find_last_not_of(STRING_ARGS(longstr), STRING_CONST(""), longstr.length);

			EXPECT_EQ(findlastnotof, 67U);
			EXPECT_EQ(findlastnotof2, STRING_NPOS);
			EXPECT_EQ(findlastnotof3, STRING_NPOS);
			EXPECT_EQ(findlastnotof4, 11U);
			EXPECT_EQ(findlastnotof5, 76U);
			EXPECT_EQ(findlastnotofofs, 67U);
			EXPECT_EQ(findlastnotofofs2, 63U);
			EXPECT_EQ(findlastnotofofs3, STRING_NPOS);
			EXPECT_EQ(findlastnotofofs4, STRING_NPOS);
			EXPECT_EQ(findlastnotofofs5, 5U);
			EXPECT_EQ(findlastnotofofs6, 2U);
			EXPECT_EQ(findlastnotofofs7, 5U);
			EXPECT_EQ(findlastnotofofs8, longstr.length - 1);
		}
	}
	{
		EXPECT_TRUE(string_ends_with(STRING_CONST("foobar"), STRING_CONST("bar")));
		EXPECT_FALSE(string_ends_with(STRING_CONST("foobar"), STRING_CONST("foo")));
		EXPECT_FALSE(string_ends_with(STRING_CONST("foobar"), STRING_CONST("foofoobar")));
		EXPECT_TRUE(string_ends_with(STRING_CONST("foobar"), STRING_CONST("")));
	}
	{
		EXPECT_TRUE(string_match_pattern(STRING_CONST(""), STRING_CONST("*")));
		EXPECT_TRUE(string_match_pattern(STRING_CONST("foo bar"), STRING_CONST("*")));
		EXPECT_TRUE(string_match_pattern(STRING_CONST(""), STRING_CONST("")));
		EXPECT_FALSE(string_match_pattern(STRING_CONST(""), STRING_CONST("?*")));
		EXPECT_FALSE(string_match_pattern(STRING_CONST(""), STRING_CONST("?")));
		EXPECT_TRUE(string_match_pattern(STRING_CONST(""), STRING_CONST("****")));
		EXPECT_FALSE(string_match_pattern(STRING_CONST(""), STRING_CONST("****?")));

		EXPECT_TRUE(string_match_pattern(STRING_CONST("foo bar"), STRING_CONST("foo?bar")));
		EXPECT_FALSE(string_match_pattern(STRING_CONST("foo bar"), STRING_CONST("foo bar?")));
		EXPECT_TRUE(string_match_pattern(STRING_CONST("foo bar"), STRING_CONST("fo?*?ar")));

		EXPECT_TRUE(string_match_pattern(STRING_CONST("foo bar"), STRING_CONST("foo bar")));
		EXPECT_FALSE(string_match_pattern(STRING_CONST("foo bar"), STRING_CONST("foo_bar")));
		EXPECT_TRUE(string_match_pattern(STRING_CONST("foo bar"), STRING_CONST("?oo bar")));
		EXPECT_TRUE(string_match_pattern(STRING_CONST("foo bar"), STRING_CONST("?oo ba?")));
		EXPECT_TRUE(string_match_pattern(STRING_CONST("foo bar"), STRING_CONST("?*?")));
		EXPECT_TRUE(string_match_pattern(STRING_CONST("foobar"), STRING_CONST("****?***?*****??")));
	}
	return 0;
}

DECLARE_TEST(string, append) {
	string_const_t nullstr = string_null();
	string_const_t emptystr = string_empty();
	string_const_t shortstr = string_const(STRING_CONST("short"));
	string_const_t longstr = string_const(STRING_CONST("long long long long long long long long long"));
	string_t val;
	char buffer[512];

	val = string_append(0, 0, 0, STRING_ARGS(nullstr));
	EXPECT_STRINGEQ(val, nullstr);
	EXPECT_STRINGEQ(val, emptystr);
	EXPECT_EQ(val.str, 0);

	val = string_append(0, 0, 0, STRING_ARGS(emptystr));
	EXPECT_STRINGEQ(val, nullstr);
	EXPECT_STRINGEQ(val, emptystr);
	EXPECT_EQ(val.str, 0);

	val = string_append(buffer, 0, 0, STRING_ARGS(nullstr));
	EXPECT_STRINGEQ(val, nullstr);
	EXPECT_STRINGEQ(val, emptystr);
	EXPECT_NE(val.str, 0);

	val = string_append(buffer, 0, sizeof(buffer), STRING_ARGS(nullstr));
	EXPECT_STRINGEQ(val, nullstr);
	EXPECT_STRINGEQ(val, emptystr);
	EXPECT_EQ(val.str, buffer);

	val = string_append_varg(buffer, 0, sizeof(buffer), STRING_ARGS(emptystr), STRING_ARGS(emptystr),
	                         STRING_ARGS(nullstr), nullptr);
	EXPECT_STRINGEQ(val, nullstr);
	EXPECT_STRINGEQ(val, emptystr);
	EXPECT_EQ(val.str, buffer);

	val = string_append(buffer, 0, sizeof(buffer), STRING_ARGS(shortstr));
	EXPECT_STRINGEQ(val, shortstr);
	EXPECT_EQ(val.str, buffer);

	val = string_clone(STRING_ARGS(shortstr));
	val = string_append(STRING_ARGS_CAPACITY(val), STRING_ARGS(nullstr));
	EXPECT_STRINGEQ(val, shortstr);
	string_deallocate(val.str);

	val = string_clone(STRING_ARGS(shortstr));
	val = string_append(STRING_ARGS_CAPACITY(val), STRING_ARGS(shortstr));
	EXPECT_STRINGEQ(val, shortstr);
	string_deallocate(val.str);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(shortstr));
	val = string_append(STRING_ARGS(val), sizeof(buffer), STRING_ARGS(emptystr));
	EXPECT_STRINGEQ(val, shortstr);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(shortstr));
	val = string_append(STRING_ARGS(val), sizeof(buffer), STRING_ARGS(shortstr));
	EXPECT_STRINGEQ(val, string_const(STRING_CONST("shortshort")));

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(shortstr));
	val = string_append_varg(STRING_ARGS(val), sizeof(buffer), STRING_ARGS(shortstr), STRING_ARGS(emptystr),
	                         STRING_ARGS(shortstr), STRING_ARGS(emptystr), nullptr);
	EXPECT_STRINGEQ(val, string_const(STRING_CONST("shortshortshort")));

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(shortstr));
	val = string_append_varg(STRING_ARGS_CAPACITY(val), STRING_ARGS(shortstr), STRING_ARGS(emptystr),
	                         STRING_ARGS(shortstr), STRING_ARGS(emptystr), nullptr);
	EXPECT_STRINGEQ(val, shortstr);

	val = string_append(buffer, 0, sizeof(buffer), STRING_ARGS(longstr));
	EXPECT_STRINGEQ(val, longstr);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(longstr));
	val = string_append(STRING_ARGS_CAPACITY(val), STRING_ARGS(nullstr));
	EXPECT_STRINGEQ(val, longstr);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(emptystr));
	val = string_append(STRING_ARGS(val), sizeof(buffer), STRING_ARGS(longstr));
	EXPECT_STRINGEQ(val, longstr);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(emptystr));
	val = string_append(STRING_ARGS_CAPACITY(val), STRING_ARGS(longstr));
	EXPECT_STRINGEQ(val, emptystr);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(longstr));
	val = string_append(STRING_ARGS_CAPACITY(val), STRING_ARGS(emptystr));
	EXPECT_STRINGEQ(val, longstr);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(shortstr));
	val = string_append(STRING_ARGS(val), sizeof(buffer), STRING_ARGS(longstr));
	EXPECT_STRINGEQ(val, string_const(STRING_CONST("shortlong long long long long long long long long")));

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(shortstr));
	val = string_append(STRING_ARGS_CAPACITY(val), STRING_ARGS(longstr));
	EXPECT_STRINGEQ(val, shortstr);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(longstr));
	val = string_append(STRING_ARGS(val), sizeof(buffer), STRING_ARGS(shortstr));
	EXPECT_STRINGEQ(val, string_const(STRING_CONST("long long long long long long long long longshort")));

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(longstr));
	val = string_append(STRING_ARGS(val), sizeof(buffer), STRING_ARGS(longstr));
	EXPECT_STRINGEQ(val,
	                string_const(STRING_CONST(
	                    "long long long long long long long long longlong long long long long long long long long")));

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(shortstr));
	val = string_append_varg(buffer, 0, sizeof(buffer), STRING_ARGS(longstr), STRING_ARGS(emptystr),
	                         STRING_ARGS(shortstr), STRING_ARGS(longstr), STRING_ARGS(emptystr), STRING_ARGS(shortstr),
	                         STRING_ARGS(nullstr), nullptr);
	EXPECT_STRINGEQ(val, string_const(STRING_CONST("long long long long long long long long long"
	                                               "shortlong long long long long long long long longshort")));

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(longstr));
	val = string_append_varg(val.str, 0, 32, STRING_ARGS(shortstr), STRING_ARGS(emptystr), STRING_ARGS(longstr),
	                         STRING_ARGS(longstr), STRING_ARGS(emptystr), STRING_ARGS(shortstr), STRING_ARGS(longstr),
	                         STRING_ARGS(nullstr), nullptr);
	EXPECT_STRINGEQ(val, string_const(STRING_CONST("shortlong long long long long l")));

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(longstr));
	val = string_append_varg(STRING_ARGS(val), sizeof(buffer), nullptr, (size_t)0, STRING_ARGS(emptystr),
	                         STRING_ARGS(shortstr), STRING_ARGS(longstr), STRING_ARGS(emptystr), STRING_ARGS(shortstr),
	                         STRING_ARGS(nullstr), nullptr, (size_t)32, STRING_ARGS(longstr), nullptr);
	EXPECT_STRINGEQ(val, string_const(STRING_CONST("long long long long long long long long long"
	                                               "shortlong long long long long long long long longshort")));

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(longstr));
	val = string_append_varg(STRING_ARGS(val), 2, nullptr, (size_t)0, STRING_ARGS(emptystr), STRING_ARGS(shortstr),
	                         STRING_ARGS(longstr), STRING_ARGS(emptystr), STRING_ARGS(shortstr), STRING_ARGS(nullstr),
	                         nullptr, (size_t)32, STRING_ARGS(longstr), nullptr);
	EXPECT_STRINGEQ(val, string_const(STRING_CONST("l")));

	return 0;
}

DECLARE_TEST(string, prepend) {
	string_const_t nullstr = string_null();
	string_const_t emptystr = string_empty();
	string_const_t shortstr = string_const(STRING_CONST("short"));
	string_const_t longstr = string_const(STRING_CONST("long long long long long long long long long"));
	string_t val;
	char buffer[512];

	val = string_prepend(0, 0, 0, STRING_ARGS(nullstr));
	EXPECT_STRINGEQ(val, nullstr);
	EXPECT_STRINGEQ(val, emptystr);
	EXPECT_EQ(val.str, 0);

	val = string_prepend(0, 0, 0, STRING_ARGS(emptystr));
	EXPECT_STRINGEQ(val, nullstr);
	EXPECT_STRINGEQ(val, emptystr);
	EXPECT_EQ(val.str, 0);

	val = string_prepend(buffer, 0, 0, STRING_ARGS(nullstr));
	EXPECT_STRINGEQ(val, nullstr);
	EXPECT_STRINGEQ(val, emptystr);
	EXPECT_NE(val.str, 0);

	val = string_prepend(buffer, 0, sizeof(buffer), STRING_ARGS(nullstr));
	EXPECT_STRINGEQ(val, nullstr);
	EXPECT_STRINGEQ(val, emptystr);
	EXPECT_EQ(val.str, buffer);

	val = string_prepend_varg(buffer, 0, sizeof(buffer), STRING_ARGS(emptystr), STRING_ARGS(emptystr),
	                          STRING_ARGS(emptystr), STRING_ARGS(nullstr), nullptr);
	EXPECT_STRINGEQ(val, nullstr);
	EXPECT_STRINGEQ(val, emptystr);
	EXPECT_EQ(val.str, buffer);

	val = string_prepend(buffer, 0, sizeof(buffer), STRING_ARGS(shortstr));
	EXPECT_STRINGEQ(val, shortstr);
	EXPECT_EQ(val.str, buffer);

	val = string_clone(STRING_ARGS(shortstr));
	val = string_prepend(STRING_ARGS_CAPACITY(val), STRING_ARGS(nullstr));
	EXPECT_STRINGEQ(val, shortstr);
	string_deallocate(val.str);

	val = string_clone(STRING_ARGS(shortstr));
	val = string_prepend(STRING_ARGS(val), val.length, STRING_CONST("nullstr"));
	EXPECT_STRINGEQ(val, string_const(STRING_ARGS(shortstr) - 1));
	string_deallocate(val.str);

	val = string_clone(STRING_ARGS(shortstr));
	val = string_prepend(STRING_ARGS_CAPACITY(val), STRING_ARGS(shortstr));
	EXPECT_STRINGEQ(val, shortstr);
	string_deallocate(val.str);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(shortstr));
	val = string_prepend(STRING_ARGS(val), sizeof(buffer), STRING_ARGS(emptystr));
	EXPECT_STRINGEQ(val, shortstr);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(shortstr));
	val = string_prepend(STRING_ARGS(val), sizeof(buffer), STRING_ARGS(shortstr));
	EXPECT_STRINGEQ(val, string_const(STRING_CONST("shortshort")));

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(shortstr));
	val = string_prepend_varg(STRING_ARGS(val), sizeof(buffer), STRING_ARGS(shortstr), STRING_ARGS(emptystr),
	                          STRING_ARGS(shortstr), STRING_ARGS(emptystr), nullptr);
	EXPECT_STRINGEQ(val, string_const(STRING_CONST("shortshortshort")));

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(shortstr));
	val = string_prepend_varg(STRING_ARGS_CAPACITY(val), STRING_ARGS(shortstr), STRING_ARGS(emptystr),
	                          STRING_ARGS(shortstr), STRING_ARGS(emptystr), nullptr);
	EXPECT_STRINGEQ(val, shortstr);

	val = string_prepend(buffer, 0, sizeof(buffer), STRING_ARGS(longstr));
	EXPECT_STRINGEQ(val, longstr);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(longstr));
	val = string_prepend(STRING_ARGS_CAPACITY(val), STRING_ARGS(nullstr));
	EXPECT_STRINGEQ(val, longstr);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(emptystr));
	val = string_prepend(STRING_ARGS(val), sizeof(buffer), STRING_ARGS(longstr));
	EXPECT_STRINGEQ(val, longstr);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(emptystr));
	val = string_prepend(STRING_ARGS_CAPACITY(val), STRING_ARGS(longstr));
	EXPECT_STRINGEQ(val, emptystr);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(longstr));
	val = string_prepend(STRING_ARGS_CAPACITY(val), STRING_ARGS(emptystr));
	EXPECT_STRINGEQ(val, longstr);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(shortstr));
	val = string_prepend(STRING_ARGS(val), sizeof(buffer), STRING_ARGS(longstr));
	EXPECT_STRINGEQ(val, string_const(STRING_CONST("long long long long long long long long longshort")));

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(shortstr));
	val = string_prepend(STRING_ARGS_CAPACITY(val), STRING_ARGS(longstr));
	EXPECT_STRINGEQ(val, shortstr);

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(longstr));
	val = string_prepend(STRING_ARGS(val), sizeof(buffer), STRING_ARGS(shortstr));
	EXPECT_STRINGEQ(val, string_const(STRING_CONST("shortlong long long long long long long long long")));

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(longstr));
	val = string_prepend(STRING_ARGS(val), sizeof(buffer), STRING_ARGS(longstr));
	EXPECT_STRINGEQ(val,
	                string_const(STRING_CONST(
	                    "long long long long long long long long longlong long long long long long long long long")));

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(shortstr));
	val = string_prepend_varg(buffer, 0, sizeof(buffer), STRING_ARGS(longstr), STRING_ARGS(emptystr),
	                          STRING_ARGS(shortstr), STRING_ARGS(longstr), STRING_ARGS(emptystr), STRING_ARGS(shortstr),
	                          STRING_ARGS(nullstr), nullptr);
	EXPECT_STRINGEQ(
	    val,
	    string_const(STRING_CONST(
	        "shortlong long long long long long long long longshortlong long long long long long long long long")));

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(longstr));
	val = string_prepend_varg(val.str, 0, 32, STRING_ARGS(shortstr), STRING_ARGS(emptystr), STRING_ARGS(longstr),
	                          STRING_ARGS(longstr), STRING_ARGS(emptystr), STRING_ARGS(shortstr), STRING_ARGS(longstr),
	                          STRING_ARGS(nullstr), nullptr);
	EXPECT_STRINGEQ(val, string_const(STRING_CONST("g long long long long longshort")));

	val = string_copy(buffer, sizeof(buffer), STRING_ARGS(shortstr));
	val = string_prepend_varg(STRING_ARGS(val), sizeof(buffer), nullptr, (size_t)0, STRING_ARGS(emptystr),
	                          STRING_ARGS(shortstr), STRING_ARGS(longstr), STRING_ARGS(emptystr), STRING_ARGS(shortstr),
	                          STRING_ARGS(nullstr), nullptr);
	EXPECT_STRINGEQ(val, string_const(STRING_CONST("shortlong long long long long long long long longshortshort")));

	return 0;
}

static string_t
string_concat_vlist_wrapper(char* str, size_t capacity, ...) {
	va_list list;
	string_t result;
	va_start(list, capacity);
	result = string_concat_vlist(str, capacity, list);
	va_end(list);
	return result;
}

static string_t
string_allocate_concat_vlist_wrapper(int token, ...) {
	va_list list;
	string_t result;
	FOUNDATION_UNUSED(token);
	va_start(list, token);
	result = string_allocate_concat_vlist(list);
	va_end(list);
	return result;
}

static string_t
string_merge_vlist_wrapper(char* dst, size_t capacity, const char* delimiter, size_t delim_length, ...) {
	va_list list;
	string_t result;
	va_start(list, delim_length);
	result = string_merge_vlist(dst, capacity, delimiter, delim_length, list);
	va_end(list);
	return result;
}

#define SHORTSTRING "short"
#define LONGSTRING "long string with dynamic buffer storage but with no real useful data"

DECLARE_TEST(string, utility) {
	{
		char buffer[256];
		string_t result;

		result = string_copy(nullptr, 0, STRING_CONST("foobar"));
		EXPECT_EQ(result.str, nullptr);
		EXPECT_EQ(result.length, 0);

		result = string_copy(buffer, 0, STRING_CONST("foobar"));
		EXPECT_EQ(result.str, buffer);
		EXPECT_EQ(result.length, 0);

		result = string_copy(buffer, sizeof(buffer), STRING_CONST("foobar"));
		EXPECT_STRINGEQ(result, string_const(STRING_CONST("foobar")));
		EXPECT_EQ(result.str[result.length], 0);

		result = string_copy(buffer, 7, STRING_CONST("foobar"));
		EXPECT_STRINGEQ(result, string_const(STRING_CONST("foobar")));
		EXPECT_EQ(result.str[result.length], 0);

		result = string_copy(buffer, 6, STRING_CONST("foobar"));
		EXPECT_STRINGEQ(result, string_const(STRING_CONST("fooba")));
		EXPECT_EQ(result.str[result.length], 0);

		result = string_copy(buffer, sizeof(buffer), "foobar", 0);
		EXPECT_EQ(result.str, buffer);
		EXPECT_EQ(result.length, 0);
		EXPECT_EQ(result.str[result.length], 0);

		result = string_copy(buffer, sizeof(buffer), nullptr, 0);
		EXPECT_EQ(result.str, buffer);
		EXPECT_EQ(result.length, 0);
		EXPECT_EQ(result.str[result.length], 0);
	}
	{
		string_const_t explodearr[32];
		string_const_t explodestr =
		    string_const(STRING_CONST("  .,testing,    .,utility.,string  methods ..., like,,,finds  split..merge     "
		                              ".,.explode.and. .., ., similar   .,,,. "));

		string_t mergestr = string_clone(STRING_CONST("    testing   merge string   "));
		string_t mergestr2 = string_clone(STRING_CONST(" ., testing, .merge.string,. "));
		string_t merged;

		string_t splitstr = string_clone(STRING_CONST(" testing split"));
		string_const_t splitright = string_null();
		string_const_t splitleft = string_null();
		string_const_t splitright2 = string_null();
		string_const_t splitleft2 = string_null();

		string_t substrtest = string_clone(STRING_CONST("testing substr"));

		size_t numtokens;
		char buffer[512];

		numtokens = string_explode(STRING_ARGS(explodestr), STRING_CONST(" ,."), explodearr,
		                           sizeof(explodearr) / sizeof(explodearr[0]), false);

		EXPECT_EQ(numtokens, 11);
		EXPECT_CONSTSTRINGEQ(explodearr[0], string_const(STRING_CONST("testing")));
		EXPECT_CONSTSTRINGEQ(explodearr[1], string_const(STRING_CONST("utility")));
		EXPECT_CONSTSTRINGEQ(explodearr[2], string_const(STRING_CONST("string")));
		EXPECT_CONSTSTRINGEQ(explodearr[3], string_const(STRING_CONST("methods")));
		EXPECT_CONSTSTRINGEQ(explodearr[4], string_const(STRING_CONST("like")));
		EXPECT_CONSTSTRINGEQ(explodearr[5], string_const(STRING_CONST("finds")));
		EXPECT_CONSTSTRINGEQ(explodearr[6], string_const(STRING_CONST("split")));
		EXPECT_CONSTSTRINGEQ(explodearr[7], string_const(STRING_CONST("merge")));
		EXPECT_CONSTSTRINGEQ(explodearr[8], string_const(STRING_CONST("explode")));
		EXPECT_CONSTSTRINGEQ(explodearr[9], string_const(STRING_CONST("and")));
		EXPECT_CONSTSTRINGEQ(explodearr[10], string_const(STRING_CONST("similar")));

		memset(explodearr, 0, sizeof(explodearr));
		numtokens = string_explode(STRING_ARGS(explodestr), STRING_CONST(" ,."), explodearr, 3, false);

		EXPECT_EQ(numtokens, 3);
		EXPECT_CONSTSTRINGEQ(explodearr[0], string_const(STRING_CONST("testing")));
		EXPECT_CONSTSTRINGEQ(explodearr[1], string_const(STRING_CONST("utility")));
		EXPECT_CONSTSTRINGEQ(explodearr[2], string_const(STRING_CONST("string")));
		EXPECT_EQ(explodearr[3].str, 0);
		EXPECT_EQ(explodearr[3].length, 0);

		numtokens = string_explode(STRING_ARGS(mergestr2), STRING_CONST(" .,"), explodearr, 32, true);

		EXPECT_EQ(numtokens, 12);
		EXPECT_CONSTSTRINGEQ(explodearr[0], string_const(STRING_CONST("")));
		EXPECT_CONSTSTRINGEQ(explodearr[1], string_const(STRING_CONST("")));
		EXPECT_CONSTSTRINGEQ(explodearr[2], string_const(STRING_CONST("")));
		EXPECT_CONSTSTRINGEQ(explodearr[3], string_const(STRING_CONST("")));
		EXPECT_CONSTSTRINGEQ(explodearr[4], string_const(STRING_CONST("testing")));
		EXPECT_CONSTSTRINGEQ(explodearr[5], string_const(STRING_CONST("")));
		EXPECT_CONSTSTRINGEQ(explodearr[6], string_const(STRING_CONST("")));
		EXPECT_CONSTSTRINGEQ(explodearr[7], string_const(STRING_CONST("merge")));
		EXPECT_CONSTSTRINGEQ(explodearr[8], string_const(STRING_CONST("string")));
		EXPECT_CONSTSTRINGEQ(explodearr[9], string_const(STRING_CONST("")));
		EXPECT_CONSTSTRINGEQ(explodearr[10], string_const(STRING_CONST("")));
		EXPECT_CONSTSTRINGEQ(explodearr[11], string_const(STRING_CONST("")));

		numtokens = string_explode(STRING_ARGS(mergestr2), STRING_CONST(" .,"), explodearr, 32, false);
		EXPECT_EQ(numtokens, 3);
		EXPECT_CONSTSTRINGEQ(explodearr[0], string_const(STRING_CONST("testing")));
		EXPECT_CONSTSTRINGEQ(explodearr[1], string_const(STRING_CONST("merge")));
		EXPECT_CONSTSTRINGEQ(explodearr[2], string_const(STRING_CONST("string")));

		numtokens = string_explode(STRING_ARGS(mergestr2), STRING_CONST(" .,"), explodearr, 32, true);
		merged = string_merge(buffer, sizeof(buffer), explodearr, numtokens, STRING_CONST(" "));
		EXPECT_STRINGEQ(merged, string_const(STRING_ARGS(mergestr)));

		memset(explodearr, 0, sizeof(explodearr));
		numtokens = string_explode(mergestr2.str, 0, STRING_CONST(" .,"), explodearr, 32, true);
		EXPECT_EQ(numtokens, 0);
		EXPECT_EQ(explodearr[0].str, nullptr);
		EXPECT_EQ(explodearr[0].length, 0);

		memset(explodearr, 0, sizeof(explodearr));
		numtokens = string_explode(STRING_ARGS(mergestr2), " .,", 0, explodearr, 32, true);
		EXPECT_EQ(numtokens, 1);
		EXPECT_STRINGEQ(mergestr2, explodearr[0]);
		EXPECT_EQ(explodearr[1].str, nullptr);
		EXPECT_EQ(explodearr[1].length, 0);

		merged = string_merge(buffer, 0, explodearr, 8, STRING_CONST("foo"));
		EXPECT_EQ(merged.str, buffer);
		EXPECT_EQ(merged.length, 0);

		merged = string_merge(buffer, 1, explodearr, 1, STRING_CONST("foo"));
		EXPECT_EQ(merged.str, buffer);
		EXPECT_EQ(merged.str[0], 0);
		EXPECT_EQ(merged.length, 0);

		merged = string_merge(buffer, 2, explodearr, 1, STRING_CONST("foo"));
		EXPECT_EQ(merged.str, buffer);
		EXPECT_EQ(merged.str[0], explodearr[0].str[0]);
		EXPECT_EQ(merged.length, 1);

		merged = string_merge(buffer, 2, explodearr, 0, STRING_CONST("foo"));
		EXPECT_EQ(merged.str, buffer);
		EXPECT_EQ(merged.str[0], 0);
		EXPECT_EQ(merged.length, 0);

		merged = string_merge_varg(buffer, 0, STRING_CONST("foo"), STRING_CONST(SHORTSTRING), STRING_CONST(LONGSTRING),
		                           nullptr);
		EXPECT_EQ(merged.str, buffer);
		EXPECT_EQ(merged.length, 0);

		merged = string_merge_varg(buffer, 1, STRING_CONST("foo"), STRING_CONST(SHORTSTRING), STRING_CONST(LONGSTRING),
		                           nullptr);
		EXPECT_EQ(merged.str, buffer);
		EXPECT_EQ(merged.str[0], 0);
		EXPECT_EQ(merged.length, 0);

		merged = string_merge_varg(buffer, 2, STRING_CONST("foo"), STRING_CONST(SHORTSTRING), STRING_CONST(LONGSTRING),
		                           nullptr);
		EXPECT_EQ(merged.str, buffer);
		EXPECT_EQ(merged.str[0], SHORTSTRING[0]);
		EXPECT_EQ(merged.str[1], 0);
		EXPECT_EQ(merged.length, 1);

		merged = string_merge_varg(buffer, sizeof(buffer), STRING_CONST("foo"), STRING_CONST(SHORTSTRING),
		                           STRING_ARGS(string_null()), nullptr, (size_t)0, STRING_CONST("bar"), nullptr);
		EXPECT_STRINGEQ(merged, string_const(STRING_CONST(SHORTSTRING)));

		merged = string_merge_varg(buffer, sizeof(buffer), STRING_CONST("foo"), STRING_CONST(SHORTSTRING),
		                           STRING_CONST(""), nullptr, (size_t)0, STRING_CONST("bar"), nullptr);
		EXPECT_STRINGEQ(merged, string_const(STRING_CONST(SHORTSTRING "foo")));

		merged = string_merge_varg(buffer, sizeof(buffer), STRING_CONST("foo"), STRING_CONST(SHORTSTRING),
		                           STRING_CONST(LONGSTRING), nullptr, (size_t)0, STRING_CONST("bar"), nullptr);
		EXPECT_STRINGEQ(merged, string_const(STRING_CONST(SHORTSTRING "foo" LONGSTRING)));

		merged = string_merge_varg(buffer, sizeof(buffer), STRING_CONST("foo"), STRING_CONST(SHORTSTRING),
		                           STRING_CONST(LONGSTRING), STRING_CONST(LONGSTRING), STRING_CONST("bar"), nullptr);
		EXPECT_STRINGEQ(merged, string_const(STRING_CONST(SHORTSTRING "foo" LONGSTRING "foo" LONGSTRING "foo"
		                                                              "bar")));

		merged = string_merge_varg(buffer, 4, STRING_CONST("foo"), STRING_CONST("bar"), STRING_CONST(LONGSTRING),
		                           STRING_CONST(LONGSTRING), STRING_CONST("bar"), nullptr);
		EXPECT_STRINGEQ(merged, string_const(STRING_CONST("bar")));

		merged =
		    string_merge_vlist_wrapper(buffer, 0, STRING_CONST("foo"), STRING_CONST("bar"), STRING_CONST(LONGSTRING),
		                               STRING_CONST(LONGSTRING), STRING_CONST("bar"), nullptr);
		EXPECT_EQ(merged.str, buffer);
		EXPECT_EQ(merged.length, 0);

		string_split(STRING_ARGS(splitstr), STRING_CONST(" "), &splitleft, &splitright, false);
		string_split(STRING_ARGS(splitstr), STRING_CONST(" "), &splitleft2, &splitright2, true);

		EXPECT_CONSTSTRINGEQ(splitleft, string_const(STRING_CONST("testing")));
		EXPECT_CONSTSTRINGEQ(splitright, string_const(STRING_CONST("split")));
		EXPECT_CONSTSTRINGEQ(splitleft2, string_const(STRING_CONST("")));
		EXPECT_CONSTSTRINGEQ(splitright2, string_const(STRING_CONST("testing split")));

		string_split(STRING_ARGS(splitstr), STRING_ARGS(splitstr), &splitleft, &splitright, false);
		string_split(STRING_ARGS(splitstr), STRING_ARGS(splitstr), &splitleft2, &splitright2, true);

		EXPECT_CONSTSTRINGEQ(splitleft, string_empty());
		EXPECT_CONSTSTRINGEQ(splitright, string_empty());
		EXPECT_CONSTSTRINGEQ(splitleft2, string_empty());
		EXPECT_CONSTSTRINGEQ(splitright2, string_empty());

		string_split(STRING_ARGS(splitstr), STRING_CONST("ng \t"), &splitleft, &splitright, false);
		string_split(STRING_ARGS(splitstr), STRING_CONST("ng \t"), &splitleft2, &splitright2, true);

		EXPECT_CONSTSTRINGEQ(splitleft, string_const(STRING_CONST("testi")));
		EXPECT_CONSTSTRINGEQ(splitright, string_const(STRING_CONST("split")));
		EXPECT_CONSTSTRINGEQ(splitleft2, string_const(STRING_CONST("")));
		EXPECT_CONSTSTRINGEQ(splitright2, string_const(STRING_CONST("testing split")));

		string_split(STRING_ARGS(splitstr), STRING_CONST("!?*.,"), &splitleft, &splitright, false);
		string_split(STRING_ARGS(splitstr), STRING_CONST("!?*.,"), &splitleft2, &splitright2, true);

		EXPECT_CONSTSTRINGEQ(splitleft, string_to_const(splitstr));
		EXPECT_CONSTSTRINGEQ(splitright, string_empty());
		EXPECT_CONSTSTRINGEQ(splitleft2, string_to_const(splitstr));
		EXPECT_CONSTSTRINGEQ(splitright2, string_null());
		EXPECT_EQ(splitright2.str, splitstr.str + splitstr.length);

		EXPECT_CONSTSTRINGEQ(string_substr(STRING_ARGS(substrtest), 0, 4), string_const(STRING_CONST("test")));
		EXPECT_CONSTSTRINGEQ(string_substr(STRING_ARGS(substrtest), 0, 14),
		                     string_const(STRING_CONST("testing substr")));
		EXPECT_CONSTSTRINGEQ(string_substr(STRING_ARGS(substrtest), 0, 20),
		                     string_const(STRING_CONST("testing substr")));
		EXPECT_CONSTSTRINGEQ(string_substr(STRING_ARGS(substrtest), 3, 20), string_const(STRING_CONST("ting substr")));
		EXPECT_CONSTSTRINGEQ(string_substr(STRING_ARGS(substrtest), 3, 11), string_const(STRING_CONST("ting substr")));
		EXPECT_CONSTSTRINGEQ(string_substr(STRING_ARGS(substrtest), 3, 1), string_const(STRING_CONST("t")));
		EXPECT_CONSTSTRINGEQ(string_substr(STRING_ARGS(substrtest), 3, 0), string_const(STRING_CONST("")));
		EXPECT_CONSTSTRINGEQ(string_substr(STRING_ARGS(substrtest), 20, 0), string_const(STRING_CONST("")));
		EXPECT_CONSTSTRINGEQ(string_substr(STRING_ARGS(substrtest), 20, 20), string_const(STRING_CONST("")));

		{
			string_t replacestr = string_copy(buffer, sizeof(buffer), STRING_CONST("testing replace"));
			replacestr = string_replace(STRING_ARGS(replacestr), sizeof(buffer), STRING_CONST("rep"),
			                            STRING_CONST("testrep"), false);
			EXPECT_STRINGEQ(replacestr, string_const(STRING_CONST("testing testreplace")));

			replacestr = string_copy(buffer, sizeof(buffer), STRING_CONST("testing replace"));
			replacestr = string_replace(STRING_ARGS(replacestr), sizeof(buffer), STRING_CONST("rep"),
			                            STRING_CONST("testrep"), true);
			EXPECT_STRINGEQ(replacestr, string_const(STRING_CONST("testing testreplace")));

			replacestr = string_copy(buffer, sizeof(buffer), STRING_CONST("testing replacelace"));
			replacestr = string_replace(STRING_ARGS(replacestr), sizeof(buffer), STRING_CONST("replace"),
			                            STRING_CONST("testrep"), true);
			EXPECT_STRINGEQ(replacestr, string_const(STRING_CONST("testing testtestrep")));

			replacestr = string_copy(buffer, sizeof(buffer), STRING_CONST(""));
			replacestr =
			    string_replace(STRING_ARGS(replacestr), sizeof(buffer), STRING_CONST("foo"), STRING_CONST("bar"), true);
			EXPECT_STRINGEQ(replacestr, string_const(STRING_CONST("")));

			replacestr = string_copy(buffer, sizeof(buffer), STRING_CONST("repppppppppp"));
			replacestr =
			    string_replace(STRING_ARGS(replacestr), sizeof(buffer), STRING_CONST("rep"), STRING_CONST("re"), true);
			EXPECT_STRINGEQ(replacestr, string_const(STRING_CONST("re")));

			replacestr = string_copy(buffer, sizeof(buffer), STRING_CONST("testing replace"));
			replacestr = string_replace(replacestr.str, 7, 7, STRING_CONST("ting"), STRING_CONST("1234"), true);
			EXPECT_STRINGEQ(replacestr, string_const(STRING_CONST("tes1234")));

			replacestr = string_copy(buffer, sizeof(buffer), STRING_CONST("testing replace"));
			replacestr = string_replace(replacestr.str, 7, 7, STRING_CONST("ting"), STRING_CONST("12345"), true);
			EXPECT_STRINGEQ(replacestr, string_const(STRING_CONST("tes1234")));
			EXPECT_EQ(replacestr.str[7], ' ');

			replacestr = string_copy(buffer, sizeof(buffer), STRING_CONST("testing repting"));
			replacestr = string_replace(STRING_ARGS(replacestr), sizeof(buffer), STRING_CONST("ting"),
			                            STRING_CONST("123456"), true);
			EXPECT_STRINGEQ(replacestr, string_const(STRING_CONST("tes123456 rep123456")));

			replacestr = string_copy(buffer, sizeof(buffer), STRING_CONST("testing repting"));
			replacestr = string_replace(STRING_ARGS(replacestr), replacestr.length + 1, STRING_CONST("ting"),
			                            STRING_CONST("123456"), true);
			EXPECT_STRINGEQ(replacestr, string_const(STRING_CONST("tes123456 repti")));
			EXPECT_EQ(replacestr.str[replacestr.length], 0);

			replacestr = string_copy(buffer, sizeof(buffer), STRING_CONST("testing repting"));
			replacestr = string_replace(STRING_ARGS(replacestr), replacestr.length + 1, STRING_CONST(" "),
			                            STRING_CONST("12345678"), true);
			EXPECT_STRINGEQ(replacestr, string_const(STRING_CONST("testing12345678")));
			EXPECT_EQ(replacestr.str[replacestr.length], 0);
		}
		{
			string_const_t strippedstr;

			strippedstr = string_strip(STRING_CONST("\t   testing strip :   "), STRING_CONST(" tp: \t  "));
			EXPECT_CONSTSTRINGEQ(strippedstr, string_const(STRING_CONST("esting stri")));

			strippedstr = string_strip(STRING_CONST("\t   testing strip :   !"), STRING_CONST(" tp: \t  "));
			EXPECT_CONSTSTRINGEQ(strippedstr, string_const(STRING_CONST("esting strip :   !")));

			strippedstr = string_strip(STRING_CONST("   testing strip :   "), STRING_CONST(""));
			EXPECT_CONSTSTRINGEQ(strippedstr, string_const(STRING_CONST("   testing strip :   ")));

			strippedstr = string_strip(STRING_CONST("   testing strip :   "), STRING_CONST(" tesingrp:"));
			EXPECT_CONSTSTRINGEQ(strippedstr, string_const(STRING_CONST("")));

			strippedstr = string_strip(STRING_CONST("   testing strip :   "), " tesingrp:", 0);
			EXPECT_CONSTSTRINGEQ(strippedstr, string_const(STRING_CONST("   testing strip :   ")));

			strippedstr = string_strip(STRING_CONST("   testing strip :   "), nullptr, 0);
			EXPECT_CONSTSTRINGEQ(strippedstr, string_const(STRING_CONST("   testing strip :   ")));

			strippedstr = string_strip("   testing strip :   ", 0, STRING_CONST(" tp:   "));
			EXPECT_CONSTSTRINGEQ(strippedstr, string_const(STRING_CONST("")));

			strippedstr = string_strip(nullptr, 0, STRING_CONST(" tp:   "));
			EXPECT_EQ(strippedstr.str, nullptr);
			EXPECT_EQ(strippedstr.length, 0);
		}
		string_deallocate(mergestr.str);
		string_deallocate(mergestr2.str);
		string_deallocate(splitstr.str);
		string_deallocate(substrtest.str);
	}
	{
		char buf[256];
		string_t clonestr = string_clone(STRING_CONST(""));
		string_t clonestr2 = string_clone(STRING_CONST(SHORTSTRING));
		string_t clonestr3 = string_clone(STRING_CONST(LONGSTRING));

		string_t teststr = string_clone(STRING_ARGS(clonestr));
		string_t teststr2 = string_clone(STRING_ARGS(clonestr2));
		string_t teststr3 = string_clone(STRING_ARGS(clonestr3));

		string_t concatstr = string_allocate_concat(STRING_ARGS(clonestr), STRING_ARGS(teststr));
		string_t concatstr2 = string_allocate_concat(STRING_ARGS(clonestr), STRING_ARGS(teststr2));
		string_t concatstr3 = string_allocate_concat(STRING_ARGS(teststr2), STRING_ARGS(clonestr));
		string_t concatstr4 = string_allocate_concat(STRING_ARGS(clonestr2), STRING_ARGS(teststr2));
		string_t concatstr5 = string_allocate_concat(STRING_ARGS(clonestr), STRING_ARGS(teststr3));
		string_t concatstr6 = string_allocate_concat(STRING_ARGS(teststr3), STRING_ARGS(clonestr));
		string_t concatstr7 = string_allocate_concat(STRING_ARGS(clonestr2), STRING_ARGS(teststr3));
		string_t concatstr8 = string_allocate_concat(STRING_ARGS(teststr3), STRING_ARGS(clonestr2));
		string_t concatstr9 = string_allocate_concat(STRING_ARGS(clonestr3), STRING_ARGS(teststr3));
		string_t concatstr10 = string_allocate_concat(STRING_ARGS(teststr3), STRING_ARGS(clonestr3));

		EXPECT_NE(teststr.str, clonestr.str);
		EXPECT_STRINGEQ(teststr, string_const(STRING_CONST("")));

		EXPECT_NE(teststr2.str, clonestr2.str);
		EXPECT_STRINGEQ(teststr2, string_const(STRING_CONST(SHORTSTRING)));

		EXPECT_NE(teststr3.str, clonestr3.str);
		EXPECT_STRINGEQ(teststr3, string_const(STRING_CONST(LONGSTRING)));

		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST("")));
		EXPECT_STRINGEQ(concatstr2, string_const(STRING_CONST(SHORTSTRING)));
		EXPECT_STRINGEQ(concatstr3, string_const(STRING_CONST(SHORTSTRING)));
		EXPECT_STRINGEQ(concatstr4, string_const(STRING_CONST(SHORTSTRING SHORTSTRING)));
		EXPECT_STRINGEQ(concatstr5, string_const(STRING_CONST(LONGSTRING)));
		EXPECT_STRINGEQ(concatstr6, string_const(STRING_CONST(LONGSTRING)));
		EXPECT_STRINGEQ(concatstr7, string_const(STRING_CONST(SHORTSTRING LONGSTRING)));
		EXPECT_STRINGEQ(concatstr8, string_const(STRING_CONST(LONGSTRING SHORTSTRING)));
		EXPECT_STRINGEQ(concatstr9, string_const(STRING_CONST(LONGSTRING LONGSTRING)));
		EXPECT_STRINGEQ(concatstr10, string_const(STRING_CONST(LONGSTRING LONGSTRING)));

		string_deallocate(concatstr.str);
		string_deallocate(concatstr2.str);
		string_deallocate(concatstr3.str);
		string_deallocate(concatstr4.str);
		string_deallocate(concatstr5.str);
		string_deallocate(concatstr6.str);
		string_deallocate(concatstr7.str);
		string_deallocate(concatstr8.str);
		string_deallocate(concatstr9.str);
		string_deallocate(concatstr10.str);

		concatstr = string_allocate_concat(nullptr, 0, nullptr, 0);
		EXPECT_STRINGEQ(concatstr, string_empty());
		EXPECT_NE(concatstr.str, nullptr);
		string_deallocate(concatstr.str);

		concatstr = string_allocate_concat(nullptr, 0, "", 0);
		EXPECT_STRINGEQ(concatstr, string_empty());
		EXPECT_NE(concatstr.str, nullptr);
		string_deallocate(concatstr.str);

		concatstr = string_allocate_concat("", 0, "", 0);
		EXPECT_STRINGEQ(concatstr, string_empty());
		EXPECT_NE(concatstr.str, nullptr);
		string_deallocate(concatstr.str);

		concatstr = string_allocate_concat(nullptr, 0, STRING_CONST(LONGSTRING LONGSTRING));
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST(LONGSTRING LONGSTRING)));
		string_deallocate(concatstr.str);

		concatstr = string_allocate_concat_varg(nullptr, (size_t)0, nullptr, (size_t)0, nullptr);
		EXPECT_STRINGEQ(concatstr, string_empty());
		EXPECT_NE(concatstr.str, nullptr);
		string_deallocate(concatstr.str);

		concatstr = string_allocate_concat_varg(nullptr, (size_t)0, nullptr, (size_t)0, "", (size_t)0, nullptr);
		EXPECT_STRINGEQ(concatstr, string_empty());
		EXPECT_NE(concatstr.str, nullptr);
		string_deallocate(concatstr.str);

		concatstr = string_allocate_concat_varg(nullptr, (size_t)0, nullptr, (size_t)0,
		                                        STRING_CONST(LONGSTRING LONGSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST(LONGSTRING LONGSTRING)));
		string_deallocate(concatstr.str);

		concatstr =
		    string_allocate_concat_varg(nullptr, (size_t)0, nullptr, (size_t)0, STRING_CONST(LONGSTRING LONGSTRING),
		                                STRING_CONST(SHORTSTRING), STRING_CONST(SHORTSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST(LONGSTRING LONGSTRING SHORTSTRING SHORTSTRING)));
		string_deallocate(concatstr.str);

		concatstr =
		    string_allocate_concat_varg(STRING_CONST(""), STRING_CONST(LONGSTRING), STRING_CONST(LONGSTRING LONGSTRING),
		                                STRING_CONST(SHORTSTRING), STRING_CONST(SHORTSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr,
		                string_const(STRING_CONST(LONGSTRING LONGSTRING LONGSTRING SHORTSTRING SHORTSTRING)));
		string_deallocate(concatstr.str);

		concatstr = string_allocate_concat_vlist_wrapper(0, nullptr, 0, nullptr, 0, nullptr);
		EXPECT_STRINGEQ(concatstr, string_empty());
		EXPECT_NE(concatstr.str, nullptr);
		string_deallocate(concatstr.str);

		concatstr = string_allocate_concat_vlist_wrapper(0, nullptr, 0, nullptr, 0, "", 0, nullptr);
		EXPECT_STRINGEQ(concatstr, string_empty());
		EXPECT_NE(concatstr.str, nullptr);
		string_deallocate(concatstr.str);

		concatstr = string_allocate_concat_vlist_wrapper(0, STRING_CONST(LONGSTRING LONGSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST(LONGSTRING LONGSTRING)));
		string_deallocate(concatstr.str);

		concatstr = string_allocate_concat_vlist_wrapper(0, STRING_CONST(LONGSTRING LONGSTRING),
		                                                 STRING_CONST(SHORTSTRING), STRING_CONST(SHORTSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST(LONGSTRING LONGSTRING SHORTSTRING SHORTSTRING)));
		string_deallocate(concatstr.str);

		concatstr = string_allocate_concat_vlist_wrapper(0, STRING_CONST(LONGSTRING LONGSTRING),
		                                                 STRING_CONST(SHORTSTRING), STRING_CONST(SHORTSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST(LONGSTRING LONGSTRING SHORTSTRING SHORTSTRING)));
		string_deallocate(concatstr.str);

		concatstr = string_allocate_concat_vlist_wrapper(0, STRING_CONST(""), STRING_CONST(LONGSTRING),
		                                                 STRING_CONST(LONGSTRING LONGSTRING), STRING_CONST(SHORTSTRING),
		                                                 STRING_CONST(SHORTSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr,
		                string_const(STRING_CONST(LONGSTRING LONGSTRING LONGSTRING SHORTSTRING SHORTSTRING)));
		string_deallocate(concatstr.str);

		concatstr = string_concat(nullptr, 0, STRING_ARGS(clonestr), STRING_ARGS(teststr));
		EXPECT_EQ(concatstr.str, nullptr);
		EXPECT_EQ(concatstr.length, 0);

		concatstr = string_concat(buf, 0, STRING_ARGS(clonestr), STRING_ARGS(teststr));
		EXPECT_EQ(concatstr.str, buf);
		EXPECT_EQ(concatstr.length, 0);

		concatstr = string_concat(buf, sizeof(buf), nullptr, 0, nullptr, 0);
		EXPECT_EQ(concatstr.str, buf);
		EXPECT_EQ(concatstr.length, 0);

		concatstr = string_concat(buf, sizeof(buf), clonestr3.str, 0, teststr.str, 0);
		EXPECT_EQ(concatstr.str, buf);
		EXPECT_EQ(concatstr.length, 0);

		concatstr = string_concat(buf, sizeof(buf), STRING_ARGS(clonestr3), teststr.str, 0);
		EXPECT_STRINGEQ(concatstr, string_to_const(clonestr3));

		concatstr = string_concat(buf, sizeof(buf), clonestr3.str, 4, nullptr, 0);
		EXPECT_STRINGEQ(concatstr, string_substr(STRING_ARGS(clonestr3), 0, 4));

		concatstr = string_concat(buf, sizeof(buf), STRING_ARGS(clonestr), STRING_ARGS(teststr));
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST("")));

		concatstr2 = string_concat(buf, sizeof(buf), STRING_ARGS(clonestr), STRING_ARGS(teststr2));
		EXPECT_STRINGEQ(concatstr2, string_const(STRING_CONST(SHORTSTRING)));

		concatstr3 = string_concat(buf, sizeof(buf), STRING_ARGS(teststr2), STRING_ARGS(clonestr));
		EXPECT_STRINGEQ(concatstr3, string_const(STRING_CONST(SHORTSTRING)));

		concatstr4 = string_concat(buf, sizeof(buf), STRING_ARGS(clonestr2), STRING_ARGS(teststr2));
		EXPECT_STRINGEQ(concatstr4, string_const(STRING_CONST(SHORTSTRING SHORTSTRING)));

		concatstr5 = string_concat(buf, sizeof(buf), STRING_ARGS(clonestr), STRING_ARGS(teststr3));
		EXPECT_STRINGEQ(concatstr5, string_const(STRING_CONST(LONGSTRING)));

		concatstr6 = string_concat(buf, sizeof(buf), STRING_ARGS(teststr3), STRING_ARGS(clonestr));
		EXPECT_STRINGEQ(concatstr6, string_const(STRING_CONST(LONGSTRING)));

		concatstr7 = string_concat(buf, sizeof(buf), STRING_ARGS(clonestr2), STRING_ARGS(teststr3));
		EXPECT_STRINGEQ(concatstr7, string_const(STRING_CONST(SHORTSTRING LONGSTRING)));

		concatstr8 = string_concat(buf, sizeof(buf), STRING_ARGS(teststr3), STRING_ARGS(clonestr2));
		EXPECT_STRINGEQ(concatstr8, string_const(STRING_CONST(LONGSTRING SHORTSTRING)));

		concatstr9 = string_concat(buf, sizeof(buf), STRING_ARGS(clonestr3), STRING_ARGS(teststr3));
		EXPECT_STRINGEQ(concatstr9, string_const(STRING_CONST(LONGSTRING LONGSTRING)));

		concatstr10 = string_concat(buf, sizeof(buf), STRING_ARGS(teststr3), STRING_ARGS(clonestr3));
		EXPECT_STRINGEQ(concatstr10, string_const(STRING_CONST(LONGSTRING LONGSTRING)));

		concatstr = string_concat(buf, 4, STRING_ARGS(teststr3), STRING_ARGS(clonestr3));
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST("lon")));

		concatstr = string_concat(buf, 0, STRING_ARGS(teststr3), STRING_ARGS(clonestr3));
		EXPECT_EQ(buf[0], 'l');  // Expect buf to be unchanged from previous test
		EXPECT_EQ(concatstr.length, 0);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST("")));

		concatstr = string_concat_varg(nullptr, 0, STRING_ARGS(clonestr), STRING_ARGS(teststr), nullptr);
		EXPECT_EQ(concatstr.str, nullptr);
		EXPECT_EQ(concatstr.length, 0);

		concatstr =
		    string_concat_varg(buf, 0, STRING_ARGS(clonestr), STRING_ARGS(teststr), STRING_CONST(LONGSTRING), nullptr);
		EXPECT_EQ(concatstr.str, buf);
		EXPECT_EQ(concatstr.length, 0);

		concatstr =
		    string_concat_varg(buf, sizeof(buf), nullptr, (size_t)0, nullptr, (size_t)0, STRING_CONST(""), nullptr);
		EXPECT_EQ(concatstr.str, buf);
		EXPECT_EQ(concatstr.length, 0);

		concatstr = string_concat_varg(buf, sizeof(buf), clonestr3.str, (size_t)0, teststr.str, (size_t)0,
		                               STRING_CONST(LONGSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST(LONGSTRING)));

		concatstr = string_concat_varg(buf, sizeof(buf), STRING_CONST(LONGSTRING), teststr.str, (size_t)0,
		                               STRING_CONST(LONGSTRING), STRING_CONST(SHORTSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST(LONGSTRING LONGSTRING SHORTSTRING)));

		concatstr = string_concat_varg(buf, sizeof(buf), "testing", (size_t)4, nullptr, (size_t)0,
		                               STRING_CONST(SHORTSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST("test" SHORTSTRING)));

		concatstr = string_concat_varg(buf, sizeof(buf), STRING_CONST("foobar"), "testing", (size_t)4,
		                               STRING_CONST(LONGSTRING), STRING_CONST(SHORTSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST("foobar"
		                                                     "test" LONGSTRING SHORTSTRING)));

		concatstr = string_concat_varg(buf, 10, STRING_CONST("foobar"), "testing", (size_t)4, STRING_CONST(LONGSTRING),
		                               STRING_CONST(SHORTSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST("foobar"
		                                                     "tes")));

		concatstr = string_concat_vlist_wrapper(nullptr, 0, STRING_ARGS(clonestr), STRING_ARGS(teststr), nullptr);
		EXPECT_EQ(concatstr.str, nullptr);
		EXPECT_EQ(concatstr.length, 0);

		concatstr = string_concat_vlist_wrapper(buf, 0, STRING_ARGS(clonestr), STRING_ARGS(teststr),
		                                        STRING_CONST(LONGSTRING), nullptr);
		EXPECT_EQ(concatstr.str, buf);
		EXPECT_EQ(concatstr.length, 0);

		concatstr = string_concat_vlist_wrapper(buf, sizeof(buf), nullptr, (size_t)0, nullptr, (size_t)0,
		                                        STRING_CONST(""), nullptr);
		EXPECT_EQ(concatstr.str, buf);
		EXPECT_EQ(concatstr.length, 0);

		concatstr = string_concat_vlist_wrapper(buf, sizeof(buf), clonestr3.str, (size_t)0, teststr.str, (size_t)0,
		                                        STRING_CONST(LONGSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST(LONGSTRING)));

		concatstr = string_concat_vlist_wrapper(buf, sizeof(buf), STRING_CONST(LONGSTRING), teststr.str, (size_t)0,
		                                        STRING_CONST(LONGSTRING), STRING_CONST(SHORTSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST(LONGSTRING LONGSTRING SHORTSTRING)));

		concatstr = string_concat_vlist_wrapper(buf, sizeof(buf), "testing", (size_t)4, "", (size_t)0,
		                                        STRING_CONST(SHORTSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST("test" SHORTSTRING)));

		concatstr = string_concat_vlist_wrapper(buf, sizeof(buf), STRING_CONST("foobar"), "testing", (size_t)4,
		                                        STRING_CONST(LONGSTRING), STRING_CONST(SHORTSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST("foobar"
		                                                     "test" LONGSTRING SHORTSTRING)));

		concatstr = string_concat_vlist_wrapper(buf, 10, STRING_CONST("foobar"), "testing", (size_t)4,
		                                        STRING_CONST(LONGSTRING), STRING_CONST(SHORTSTRING), nullptr);
		EXPECT_STRINGEQ(concatstr, string_const(STRING_CONST("foobar"
		                                                     "tes")));

		string_deallocate(teststr.str);
		string_deallocate(clonestr.str);
		string_deallocate(teststr2.str);
		string_deallocate(clonestr2.str);
		string_deallocate(teststr3.str);
		string_deallocate(clonestr3.str);
	}
	return 0;
}

#undef SHORTSTRING
#undef LONGSTRING

static string_t
string_allocate_vformat_wrapper(const char* format, size_t length, ...) FOUNDATION_ATTRIBUTE4(format, printf, 1, 3);

static string_t
string_vformat_wrapper(char* buffer, size_t capacity, const char* format, size_t length, ...)
    FOUNDATION_ATTRIBUTE4(format, printf, 3, 5);

static string_t
string_allocate_vformat_wrapper(const char* format, size_t length, ...) {
	va_list list;
	string_t result;
	va_start(list, length);
	result = string_allocate_vformat(format, length, list);
	va_end(list);
	return result;
}

static string_t
string_vformat_wrapper(char* buffer, size_t capacity, const char* format, size_t length, ...) {
	va_list list;
	string_t result;
	va_start(list, length);
	result = string_vformat(buffer, capacity, format, length, list);
	va_end(list);
	return result;
}

DECLARE_TEST(string, format) {
	char buffer[256];
	{
		int64_t ival = -1;
		uint64_t uval = 0x123456789abULL;
		const char longstr[] =
		    "A really long string"
		    "to force reallocation of output buffer in format loop, which"
		    "should be length + 32 bytes by default... this is more!";
		string_t teststr;

		teststr = string_allocate_format(STRING_CONST("%" PRId64), ival);
		EXPECT_STRINGEQ(teststr, string_const(STRING_CONST("-1")));
		string_deallocate(teststr.str);

		teststr = string_allocate_format(STRING_CONST("0x%" PRIx64), ival);
		EXPECT_STRINGEQ(teststr, string_const(STRING_CONST("0xffffffffffffffff")));
		string_deallocate(teststr.str);

		teststr = string_allocate_format(STRING_CONST("%016" PRIX64), uval);
		EXPECT_STRINGEQ(teststr, string_const(STRING_CONST("00000123456789AB")));
		string_deallocate(teststr.str);

		teststr = string_allocate_format(nullptr, 0);
		EXPECT_NE(teststr.str, nullptr);
		EXPECT_EQ(teststr.length, 0);
		string_deallocate(teststr.str);

		teststr = string_allocate_format(STRING_CONST("%s"), longstr);
		EXPECT_STRINGEQ(teststr, string_const(STRING_CONST(longstr)));
		string_deallocate(teststr.str);

		teststr = string_allocate_format("%s", 0, longstr);
		EXPECT_NE(teststr.str, nullptr);
		EXPECT_EQ(teststr.length, 0);
		string_deallocate(teststr.str);

		// This test passed invalid data (string not zero terminated at the given length)
		// Verify the function uses the entire string literal
		teststr = string_allocate_format("%s", 1, longstr);
		EXPECT_STRINGEQ(teststr, string_const(STRING_CONST(longstr)));
		string_deallocate(teststr.str);
	}
	{
		string_t teststr;
		teststr = string_format(nullptr, 0, STRING_CONST("foobar %d"), 10);
		EXPECT_EQ(teststr.str, nullptr);
		EXPECT_EQ(teststr.length, 0);

		teststr = string_format(buffer, 0, STRING_CONST("foobar %d"), 10);
		EXPECT_EQ(teststr.str, buffer);
		EXPECT_EQ(teststr.length, 0);

		teststr = string_format(buffer, sizeof(buffer), nullptr, 0);
		EXPECT_EQ(teststr.str, buffer);
		EXPECT_EQ(teststr.length, 0);

		teststr = string_format(buffer, sizeof(buffer), "foobar %d", 0, 10);
		EXPECT_EQ(teststr.str, buffer);
		EXPECT_EQ(teststr.length, 0);

		// This test passed invalid data (string not zero terminated at the given length)
		// Verify the function uses the entire string literal
		teststr = string_format(buffer, sizeof(buffer), "%d", 1, 10);
		EXPECT_STRINGEQ(teststr, string_const(STRING_CONST("10")));
	}
	{
		int64_t ival = -1;
		uint64_t uval = 0x123456789abULL;
		const char longstr[] =
		    "A really long string"
		    "to force reallocation of output buffer in format loop, which"
		    "should be length + 32 bytes by default... this is more!";
		string_t teststr;

		teststr = string_allocate_vformat_wrapper(STRING_CONST("%" PRId64), ival);
		EXPECT_STRINGEQ(teststr, string_const(STRING_CONST("-1")));
		string_deallocate(teststr.str);

		teststr = string_allocate_vformat_wrapper(STRING_CONST("0x%" PRIx64), ival);
		EXPECT_STRINGEQ(teststr, string_const(STRING_CONST("0xffffffffffffffff")));
		string_deallocate(teststr.str);

		teststr = string_allocate_vformat_wrapper(STRING_CONST("%016" PRIX64), uval);
		EXPECT_STRINGEQ(teststr, string_const(STRING_CONST("00000123456789AB")));
		string_deallocate(teststr.str);

		teststr = string_allocate_vformat_wrapper(nullptr, 0);
		EXPECT_NE(teststr.str, nullptr);
		EXPECT_EQ(teststr.length, 0);
		string_deallocate(teststr.str);

		teststr = string_allocate_vformat_wrapper(STRING_CONST("%s"), longstr);
		EXPECT_STRINGEQ(teststr, string_const(STRING_CONST(longstr)));
		string_deallocate(teststr.str);

		teststr = string_allocate_vformat_wrapper("%s", 0, longstr);
		EXPECT_NE(teststr.str, nullptr);
		EXPECT_EQ(teststr.length, 0);
		string_deallocate(teststr.str);

		// This test passed invalid data (string not zero terminated at the given length)
		// Verify the function uses the entire string literal
		teststr = string_allocate_vformat_wrapper("%s", 1, longstr);
		EXPECT_STRINGEQ(teststr, string_const(STRING_CONST(longstr)));
		string_deallocate(teststr.str);
	}
	{
		string_t teststr;
		teststr = string_vformat_wrapper(nullptr, 0, STRING_CONST("foobar %d"), 10);
		EXPECT_EQ(teststr.str, nullptr);
		EXPECT_EQ(teststr.length, 0);

		teststr = string_vformat_wrapper(buffer, 0, STRING_CONST("foobar %d"), 10);
		EXPECT_EQ(teststr.str, buffer);
		EXPECT_EQ(teststr.length, 0);

		teststr = string_vformat_wrapper(buffer, sizeof(buffer), nullptr, 0);
		EXPECT_EQ(teststr.str, buffer);
		EXPECT_EQ(teststr.length, 0);

		teststr = string_vformat_wrapper(buffer, sizeof(buffer), "foobar %d", 0, 10);
		EXPECT_EQ(teststr.str, buffer);
		EXPECT_EQ(teststr.length, 0);

		teststr = string_vformat_wrapper(buffer, 6, STRING_CONST("foobar %d"), 10);
		EXPECT_STRINGEQ(teststr, string_const(STRING_CONST("fooba")));
		EXPECT_EQ(teststr.length, 5);

		// This test passed invalid data (string not zero terminated at the given length)
		// Verify the function uses the entire string literal
		teststr = string_vformat_wrapper(buffer, sizeof(buffer), "%d", 1, 10);
		EXPECT_STRINGEQ(teststr, string_const(STRING_CONST("10")));
	}
	{
		int64_t ival = -1;
		uint64_t uval = 0x123456789abULL;
		string_t teststr1, teststr2, teststr3;

		teststr1 = string_format(buffer, sizeof(buffer), STRING_CONST("%" PRId64), ival);
		EXPECT_STRINGEQ(teststr1, string_const(STRING_CONST("-1")));

		teststr2 = string_format(buffer, sizeof(buffer), STRING_CONST("0x%" PRIx64), ival);
		EXPECT_STRINGEQ(teststr2, string_const(STRING_CONST("0xffffffffffffffff")));

		teststr3 = string_format(buffer, 8, STRING_CONST("%016" PRIX64), uval);
		EXPECT_STRINGEQ(teststr3, string_const(STRING_CONST("0000012")));
	}
	{
		string_t teststr1 = string_allocate_format(STRING_CONST("0x%" PRIfixPTR), (uintptr_t) nullptr);
		string_t teststr2 = string_allocate_format(STRING_CONST("0x%" PRIfixPTR), (uintptr_t)((void*)-1));
		string_t teststr3 = string_allocate_format(STRING_CONST("0x%" PRIfixPTR), (uintptr_t)((void*)0x1234abULL));

#if FOUNDATION_SIZE_POINTER == 8
		EXPECT_STRINGEQ(teststr1, string_const(STRING_CONST("0x0000000000000000")));
		EXPECT_STRINGEQ(teststr2, string_const(STRING_CONST("0xFFFFFFFFFFFFFFFF")));
		EXPECT_STRINGEQ(teststr3, string_const(STRING_CONST("0x00000000001234AB")));
#else
		EXPECT_STRINGEQ(teststr1, string_const(STRING_CONST("0x00000000")));
		EXPECT_STRINGEQ(teststr2, string_const(STRING_CONST("0xFFFFFFFF")));
		EXPECT_STRINGEQ(teststr3, string_const(STRING_CONST("0x001234AB")));
#endif

		string_deallocate(teststr1.str);
		string_deallocate(teststr2.str);
		string_deallocate(teststr3.str);
	}
	{
		string_t teststr1 = string_format(buffer, sizeof(buffer), STRING_CONST("0x%" PRIfixPTR), (uintptr_t) nullptr);
#if FOUNDATION_SIZE_POINTER == 8
		EXPECT_STRINGEQ(teststr1, string_const(STRING_CONST("0x0000000000000000")));
#else
		EXPECT_STRINGEQ(teststr1, string_const(STRING_CONST("0x00000000")));
#endif

		string_t teststr2 =
		    string_format(buffer, sizeof(buffer), STRING_CONST("0x%" PRIfixPTR), (uintptr_t)((void*)-1));
#if FOUNDATION_SIZE_POINTER == 8
		EXPECT_STRINGEQ(teststr2, string_const(STRING_CONST("0xFFFFFFFFFFFFFFFF")));
#else
		EXPECT_STRINGEQ(teststr2, string_const(STRING_CONST("0xFFFFFFFF")));
#endif

		string_t teststr3 = string_format(buffer, 8, STRING_CONST("0x%" PRIfixPTR), (uintptr_t)((void*)0x1234abULL));
#if FOUNDATION_SIZE_POINTER == 8
		EXPECT_STRINGEQ(teststr3, string_const(STRING_CONST("0x00000")));
#else
		EXPECT_STRINGEQ(teststr3, string_const(STRING_CONST("0x00123")));
#endif
	}
	return 0;
}

DECLARE_TEST(string, convert) {
	char buffer[256];
	string_t str;
	string_const_t conststr;
	tick_t systime;

	str = string_from_int(buffer, 0, 0, 0, 0);
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.length, 0);

	str = string_from_int(buffer, 1, 42, 8, 0);
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.str[0], 0);
	EXPECT_EQ(str.length, 0);

	str = string_from_int(buffer, 2, 42, 8, 0);
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("4")));

	str = string_from_int(buffer, 3, 42, 8, 0);
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("42")));

	str = string_from_int(buffer, sizeof(buffer), -42, 32, '=');
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("=============================-42")));

	conststr = string_from_int_static(-12345678901234567LL, 7, '=');
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("-12345678901234567")));

	str = string_from_uint(buffer, 0, 0, false, 0, 0);
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.length, 0);

	str = string_from_uint(buffer, 1, 42, true, 8, 0);
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.str[0], 0);
	EXPECT_EQ(str.length, 0);

	str = string_from_uint(buffer, 2, 42, false, 8, 0);
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("4")));

	str = string_from_uint(buffer, 3, 42, false, 8, 0);
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("42")));

	str = string_from_uint(buffer, 2, 42, true, 8, 0);
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("2")));

	str = string_from_uint(buffer, 3, 42, true, 8, 0);
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("2a")));

	str = string_from_uint(buffer, sizeof(buffer), 42, false, 32, '=');
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("==============================42")));

	str = string_from_uint(buffer, sizeof(buffer), 42, true, 32, '=');
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("==============================2a")));

	conststr = string_from_uint_static(12345678901234567ULL, false, 7, '=');
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("12345678901234567")));

	conststr = string_from_uint_static(12345678901234567ULL, true, 16, '=');
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("==2bdc545d6b4b87")));

	str = string_from_uint128(buffer, 1, uint128_make(1, 2));
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.str[0], 0);
	EXPECT_EQ(str.length, 0);

	str = string_from_uint128(buffer, 3, uint128_make(0x1234567890234567ULL, 0x2345678902345678ULL));
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("12")));

	str = string_from_uint128(buffer, sizeof(buffer), uint128_make(0xa234567890234567ULL, 0xb345678902345678ULL));
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("a234567890234567b345678902345678")));

	conststr = string_from_uint128_static(uint128_make(0x1234567890234567ULL, 0x2345678902345678ULL));
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("12345678902345672345678902345678")));

	conststr = string_from_uint128_static(uint128_make(0xa234567890234567ULL, 0xb345678902345678ULL));
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("a234567890234567b345678902345678")));

	str = string_from_uint256(buffer, 1, uint256_make(1, 2, 3, 4));
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.str[0], 0);
	EXPECT_EQ(str.length, 0);

	str = string_from_uint256(
	    buffer, 3,
	    uint256_make(0x1234567890234567ULL, 0x2345678902345678ULL, 0x3456789023456789ULL, 0x4567890234567890ULL));
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("12")));

	str = string_from_uint256(
	    buffer, sizeof(buffer),
	    uint256_make(0xa234567890234567ULL, 0xb345678902345678ULL, 0x38fa67e10d0b7e01ULL, 0xa8326fd6752bcb78ULL));
	EXPECT_STRINGEQ(str,
	                string_const(STRING_CONST("a234567890234567b34567890234567838fa67e10d0b7e01a8326fd6752bcb78")));

	conststr = string_from_uint256_static(
	    uint256_make(0xa234567890234567ULL, 0xb345678902345678ULL, 0x38fa67e10d0b7e01ULL, 0xa8326fd6752bcb78ULL));
	EXPECT_CONSTSTRINGEQ(
	    conststr, string_const(STRING_CONST("a234567890234567b34567890234567838fa67e10d0b7e01a8326fd6752bcb78")));

	str = string_from_uint512(buffer, 1, uint512_make(1, 2, 3, 4, 5, 6, 7, 8));
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.str[0], 0);
	EXPECT_EQ(str.length, 0);

	str = string_from_uint512(
	    buffer, 3,
	    uint512_make(0x1234567890234567ULL, 0x2345678902345678ULL, 0x3456789023456789ULL, 0x4567890234567890ULL,
	                 0x98f08afab5bfeb9dULL, 0x96913cecaf618430ULL, 0xc5557794f244d66bULL, 0x2f3ae1c5b212218aULL));
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("12")));

	str = string_from_uint512(
	    buffer, sizeof(buffer),
	    uint512_make(0xa234567890234567ULL, 0xb345678902345678ULL, 0x38fa67e10d0b7e01ULL, 0xa8326fd6752bcb78ULL,
	                 0x98f08afab5bfeb9dULL, 0x96913cecaf618430ULL, 0xc5557794f244d66bULL, 0x2f3ae1c5b212218aULL));
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("a234567890234567b34567890234567838fa67e10d0b7e01a8326fd6752bcb7898f"
	                                               "08afab5bfeb9d96913cecaf618430c5557794f244d66b2f3ae1c5b212218a")));

	conststr = string_from_uint512_static(
	    uint512_make(0xa234567890234567ULL, 0xb345678902345678ULL, 0x38fa67e10d0b7e01ULL, 0xa8326fd6752bcb78ULL,
	                 0x98f08afab5bfeb9dULL, 0x96913cecaf618430ULL, 0xc5557794f244d66bULL, 0x2f3ae1c5b212218aULL));
	EXPECT_CONSTSTRINGEQ(conststr,
	                     string_const(STRING_CONST("a234567890234567b34567890234567838fa67e10d0b7e01a8326fd6752bcb7898f"
	                                               "08afab5bfeb9d96913cecaf618430c5557794f244d66b2f3ae1c5b212218a")));

	str = string_from_real(buffer, 0, 1, 0, 0, '=');
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.length, 0);

	str = string_from_real(buffer, 1, 1, 0, 0, '=');
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.str[0], 0);
	EXPECT_EQ(str.length, 0);

	str = string_from_real(buffer, 3, 1, 0, 0, '=');
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("1")));

	str = string_from_real(buffer, 3, REAL_C(1.1), 8, 16, '=');
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("1.")));

	str = string_from_real(buffer, sizeof(buffer), REAL_C(1.5), 0, 0, '=');
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("1.5")));

	str = string_from_real(buffer, sizeof(buffer), REAL_C(0.1), 8, 16, '=');
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("=============0.1")));

	str = string_from_real(buffer, sizeof(buffer), -REAL_C(0.0), 8, 16, '=');
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("===============0")));

	conststr = string_from_real_static(REAL_C(1.5), 0, 0, '=');
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("1.5")));

	conststr = string_from_real_static(REAL_C(0.1), 8, 16, '=');
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("=============0.1")));

	conststr = string_from_real_static(-REAL_C(0.0), 8, 16, '=');
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("===============0")));

	str = string_from_float32(buffer, sizeof(buffer), 1.5f, 0, 0, '=');
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("1.5")));

	str = string_from_float32(buffer, sizeof(buffer), 0.1f, 8, 16, '=');
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("=============0.1")));

	str = string_from_float32(buffer, sizeof(buffer), -0.0f, 8, 16, '=');
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("===============0")));

	conststr = string_from_float32_static(1.5f, 0, 0, '=');
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("1.5")));

	conststr = string_from_float32_static(0.1f, 8, 16, '=');
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("=============0.1")));

	conststr = string_from_float32_static(-0.0f, 8, 16, '=');
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("===============0")));

	str = string_from_float64(buffer, sizeof(buffer), 1.5, 0, 0, '=');
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("1.5")));

	str = string_from_float64(buffer, sizeof(buffer), 0.1, 8, 16, '=');
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("=============0.1")));

	str = string_from_float64(buffer, sizeof(buffer), -0.0, 8, 16, '=');
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("===============0")));

	conststr = string_from_float64_static(1.5, 0, 0, '=');
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("1.5")));

	conststr = string_from_float64_static(0.1, 8, 16, '=');
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("=============0.1")));

	conststr = string_from_float64_static(-0.0, 8, 16, '=');
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("===============0")));

	str = string_from_time(buffer, 0, time_system(), false);
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.length, 0);

	str = string_from_time(buffer, 1, time_system(), true);
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.str[0], 0);
	EXPECT_EQ(str.length, 0);

	str = string_from_time(buffer, 10, time_system(), false);
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.str[0], 0);
	EXPECT_EQ(str.length, 0);

	str = string_from_time(buffer, 25, 0, false);
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("Thu Jan 01 00:00:00 1970")));

	conststr = string_from_time_static(0, false);
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("Thu Jan 01 00:00:00 1970")));

	systime = time_system();
	str = string_from_time(buffer, 25, systime, false);
	EXPECT_FALSE(string_equal(STRING_ARGS(str), STRING_CONST("Thu Jan 01 00:00:00 1970")));
	EXPECT_INTEQ(str.length, 24);

	conststr = string_from_time_static(systime, false);
	EXPECT_INTEQ(conststr.length, 24);
	EXPECT_TRUE(string_equal(STRING_ARGS(str), STRING_ARGS(conststr)));

	str = string_from_uuid(buffer, 0, UUID_DNS);
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.length, 0);

	str = string_from_uuid(buffer, 1, UUID_DNS);
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.str[0], 0);
	EXPECT_EQ(str.length, 0);

	str = string_from_uuid(buffer, 10, UUID_DNS);
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("6ba7b810-")));

	str = string_from_uuid(buffer, sizeof(buffer), UUID_DNS);
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("6ba7b810-9dad-11d1-80b4-00c04fd430c8")));

	conststr = string_from_uuid_static(UUID_DNS);
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("6ba7b810-9dad-11d1-80b4-00c04fd430c8")));

	str = string_from_version(buffer, 0, version_make(0, 0, 0, 0, 0));
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.length, 0);

	str = string_from_version(buffer, 1, version_make(0, 0, 0, 0, 0));
	EXPECT_EQ(str.str, buffer);
	EXPECT_EQ(str.str[0], 0);
	EXPECT_EQ(str.length, 0);

	str = string_from_version(buffer, 5, version_make(1, 2, 3, 0, 0));
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("1.2.")));

	str = string_from_version(buffer, sizeof(buffer), version_make(1, 2, 3, 0, 0));
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("1.2.3")));

	str = string_from_version(buffer, 7, version_make(1, 2, 3, 4, 0));
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("1.2.3-")));

	str = string_from_version(buffer, sizeof(buffer), version_make(1, 2, 3, 4, 0));
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("1.2.3-4")));

	str = string_from_version(buffer, 9, version_make(1, 2, 3, 4, 42));
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("1.2.3-4-")));

	str = string_from_version(buffer, sizeof(buffer), version_make(1, 2, 3, 4, 42));
	EXPECT_STRINGEQ(str, string_const(STRING_CONST("1.2.3-4-2a")));

	conststr = string_from_version_static(version_make(1, 2, 3, 4, 42));
	EXPECT_CONSTSTRINGEQ(conststr, string_const(STRING_CONST("1.2.3-4-2a")));

	EXPECT_INTEQ(string_to_int(nullptr, 0), 0);
	EXPECT_INTEQ(string_to_int("1", 0), 0);
	EXPECT_INTEQ(string_to_int(STRING_CONST("1")), 1);
	EXPECT_INTEQ(string_to_int(STRING_CONST("-12345abvs")), -12345);
	EXPECT_INTEQ(string_to_int("-12345abvs", 3), -12);
	EXPECT_INTEQ(string_to_int(STRING_CONST("a-12345abvs")), 0);

	EXPECT_INTEQ(string_to_uint(nullptr, 0, false), 0);
	EXPECT_INTEQ(string_to_uint("1", 0, false), 0);
	EXPECT_INTEQ(string_to_uint(STRING_CONST("1"), false), 1);
	EXPECT_INTEQ(string_to_uint(STRING_CONST("123456asv"), false), 123456);
	EXPECT_INTEQ(string_to_uint(STRING_CONST("-123456asv"), false), (unsigned int)-123456);
	EXPECT_INTEQ(string_to_uint("-123456asv", 3, false), (unsigned int)-12);
	EXPECT_INTEQ(string_to_uint(STRING_CONST("abc"), false), 0);
	EXPECT_INTEQ(string_to_uint(STRING_CONST("abc"), true), 0xabc);

	EXPECT_INT64EQ(string_to_int64(nullptr, 0), 0);
	EXPECT_INT64EQ(string_to_int64("1", 0), 0);
	EXPECT_INT64EQ(string_to_int64(STRING_CONST("1")), 1);
	EXPECT_INT64EQ(string_to_int64(STRING_CONST("-1234567890123456789abvs")), -1234567890123456789LL);
	EXPECT_INT64EQ(string_to_int64("-1234567890123456789abvs", 4), -123);
	EXPECT_INT64EQ(string_to_int64(STRING_CONST("a-12345abvs")), 0);

	EXPECT_UINT64EQ(string_to_uint64(nullptr, 0, false), 0);
	EXPECT_UINT64EQ(string_to_uint64("1", 0, false), 0);
	EXPECT_UINT64EQ(string_to_uint64(STRING_CONST("1"), false), 1);
	EXPECT_UINT64EQ(string_to_uint64(STRING_CONST("1234567890123456789asv"), false), 1234567890123456789LL);
	EXPECT_UINT64EQ(string_to_uint64(STRING_CONST("-1234567890123456789asv"), false), (uint64_t)-1234567890123456789);
	EXPECT_UINT64EQ(string_to_uint64(STRING_CONST("abcdef123456"), false), 0);
	EXPECT_UINT64EQ(string_to_uint64(STRING_CONST("abcdef123456"), true), 0xabcdef123456ULL);
	EXPECT_UINT64EQ(string_to_uint64("abcdef123456", 5, true), 0xabcdeULL);

	EXPECT_TRUE(uint128_equal(string_to_uint128(nullptr, 0), uint128_make(0, 0)));
	EXPECT_TRUE(uint128_equal(string_to_uint128("1234567890abcdef00112233aabbccdd", 0), uint128_make(0, 0)));
	EXPECT_TRUE(
	    uint128_equal(string_to_uint128("1234567890abcdef00112233aabbccdd", 12), uint128_make(0, 0x1234567890abULL)));
	EXPECT_TRUE(uint128_equal(string_to_uint128("1234567890abcdef00112233aabbccdd", 24),
	                          uint128_make(0x12345678ULL, 0x90abcdef00112233ULL)));
	EXPECT_TRUE(uint128_equal(string_to_uint128(STRING_CONST("1234567890abcdef00112233aabbccdd")),
	                          uint128_make(0x1234567890abcdefULL, 0x00112233aabbccddULL)));
	EXPECT_TRUE(
	    uint128_equal(string_to_uint128("1234567 890abcdef00112233aabbccdd", 24), uint128_make(0, 0x1234567ULL)));
	EXPECT_TRUE(uint128_equal(string_to_uint128("1234567890abcdef001U12233aabbccdd", 24),
	                          uint128_make(0x0123, 0x4567890abcdef001ULL)));
	EXPECT_TRUE(uint128_equal(string_to_uint128("1234567890abcdef0011223U3aabbccdd", 24),
	                          uint128_make(0x01234567, 0x890abcdef0011223ULL)));
	EXPECT_TRUE(uint128_equal(string_to_uint128("1234567890abcdef00112233Uaabbccdd", 24),
	                          uint128_make(0x012345678, 0x90abcdef00112233ULL)));

	EXPECT_TRUE(uint256_equal(string_to_uint256(nullptr, 0), uint256_make(0, 0, 0, 0)));
	EXPECT_TRUE(uint256_equal(string_to_uint256("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe21", 0),
	                          uint256_make(0, 0, 0, 0)));
	EXPECT_TRUE(uint256_equal(string_to_uint256("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe21", 12),
	                          uint256_make(0, 0, 0, 0x1234567890abULL)));
	EXPECT_TRUE(uint256_equal(string_to_uint256("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe21", 24),
	                          uint256_make(0, 0, 0x12345678ULL, 0x90abcdef00112233ULL)));
	EXPECT_TRUE(uint256_equal(string_to_uint256("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe21", 34),
	                          uint256_make(0, 0x12, 0x34567890abcdef00ULL, 0x112233aabbccdd74ULL)));
	EXPECT_TRUE(uint256_equal(
	    string_to_uint256(STRING_CONST("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe21")),
	    uint256_make(0x1234567890abcdefULL, 0x00112233aabbccddULL, 0x744b5a85f8e5446aULL, 0xae9c8eb4bb7ffe21ULL)));
	EXPECT_TRUE(
	    uint256_equal(string_to_uint256("123456789 0abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe21", 57),
	                  uint256_make(0, 0, 0, 0x123456789ULL)));
	EXPECT_TRUE(
	    uint256_equal(string_to_uint256("1234567890abcdef0011U2233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe21", 57),
	                  uint256_make(0, 0, 0x1234ULL, 0x567890abcdef0011ULL)));
	EXPECT_TRUE(
	    uint256_equal(string_to_uint256("1234567890abcdef00112233aabbccdd744b5Ua85f8e5446aae9c8eb4bb7ffe21", 57),
	                  uint256_make(0, 0x12345ULL, 0x67890abcdef00112ULL, 0x233aabbccdd744b5ULL)));
	EXPECT_TRUE(uint256_equal(
	    string_to_uint256("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4Ubb7ffe21", 57),
	    uint256_make(0x12345678ULL, 0x90abcdef00112233ULL, 0xaabbccdd744b5a85ULL, 0xf8e5446aae9c8eb4ULL)));
	EXPECT_TRUE(uint256_equal(
	    string_to_uint256("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bUb7ffe21", 57),
	    uint256_make(0x123456789ULL, 0x0abcdef00112233aULL, 0xabbccdd744b5a85fULL, 0x8e5446aae9c8eb4bULL)));

	EXPECT_TRUE(uint512_equal(string_to_uint512(nullptr, 0), uint512_make(0, 0, 0, 0, 0, 0, 0, 0)));
	EXPECT_TRUE(uint512_equal(string_to_uint512("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe214ba3a1"
	                                            "9049fb49b1b17e56ff1ef93d9caa47e331a5f14af481d3f864b3f0bf2f",
	                                            0),
	                          uint512_make(0, 0, 0, 0, 0, 0, 0, 0)));
	EXPECT_TRUE(uint512_equal(string_to_uint512("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe214ba3a1"
	                                            "9049fb49b1b17e56ff1ef93d9caa47e331a5f14af481d3f864b3f0bf2f",
	                                            12),
	                          uint512_make(0, 0, 0, 0, 0, 0, 0, 0x1234567890abULL)));
	EXPECT_TRUE(uint512_equal(string_to_uint512("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe214ba3a1"
	                                            "9049fb49b1b17e56ff1ef93d9caa47e331a5f14af481d3f864b3f0bf2f",
	                                            24),
	                          uint512_make(0, 0, 0, 0, 0, 0, 0x12345678ULL, 0x90abcdef00112233ULL)));
	EXPECT_TRUE(uint512_equal(string_to_uint512("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe214ba3a1"
	                                            "9049fb49b1b17e56ff1ef93d9caa47e331a5f14af481d3f864b3f0bf2f",
	                                            34),
	                          uint512_make(0, 0, 0, 0, 0, 0x12, 0x34567890abcdef00ULL, 0x112233aabbccdd74ULL)));
	EXPECT_TRUE(uint512_equal(
	    string_to_uint512(STRING_CONST("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe214ba3a19049fb49b"
	                                   "1b17e56ff1ef93d9caa47e331a5f14af481d3f864b3f0bf2f")),
	    uint512_make(0x1234567890abcdefULL, 0x00112233aabbccddULL, 0x744b5a85f8e5446aULL, 0xae9c8eb4bb7ffe21ULL,
	                 0x4ba3a19049fb49b1ULL, 0xb17e56ff1ef93d9cULL, 0xaa47e331a5f14af4ULL, 0x81d3f864b3f0bf2fULL)));
	EXPECT_TRUE(uint512_equal(string_to_uint512("123456789 "
	                                            "0abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe214ba3a19049fb49b"
	                                            "1b17e56ff1ef93d9caa47e331a5f14af481d3f864b3f0bf2f",
	                                            117),
	                          uint512_make(0, 0, 0, 0, 0, 0, 0, 0x123456789ULL)));
	EXPECT_TRUE(uint512_equal(string_to_uint512("1234567890abcdef0011U2233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe214ba3a"
	                                            "19049fb49b1b17e56ff1ef93d9caa47e331a5f14af481d3f864b3f0bf2f",
	                                            117),
	                          uint512_make(0, 0, 0, 0, 0, 0, 0x1234ULL, 0x567890abcdef0011ULL)));
	EXPECT_TRUE(uint512_equal(string_to_uint512("1234567890abcdef00112233aabbccdd744b5Ua85f8e5446aae9c8eb4bb7ffe214ba3a"
	                                            "19049fb49b1b17e56ff1ef93d9caa47e331a5f14af481d3f864b3f0bf2f",
	                                            117),
	                          uint512_make(0, 0, 0, 0, 0, 0x12345ULL, 0x67890abcdef00112ULL, 0x233aabbccdd744b5ULL)));
	EXPECT_TRUE(uint512_equal(
	    string_to_uint512("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4Ubb7ffe214ba3a19049fb49b1b17e56ff1ef"
	                      "93d9caa47e331a5f14af481d3f864b3f0bf2f",
	                      117),
	    uint512_make(0, 0, 0, 0, 0x12345678ULL, 0x90abcdef00112233ULL, 0xaabbccdd744b5a85ULL, 0xf8e5446aae9c8eb4ULL)));
	EXPECT_TRUE(uint512_equal(string_to_uint512("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe214ba3Ua"
	                                            "19049fb49b1b17e56ff1ef93d9caa47e331a5f14af481d3f864b3f0bf2f",
	                                            117),
	                          uint512_make(0, 0, 0, 0x01234ULL, 0x567890abcdef0011ULL, 0x2233aabbccdd744bULL,
	                                       0x5a85f8e5446aae9cULL, 0x8eb4bb7ffe214ba3ULL)));
	EXPECT_TRUE(uint512_equal(string_to_uint512("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe214ba3a1"
	                                            "9049fb49b1b1U7e56ff1ef93d9caa47e331a5f14af481d3f864b3f0bf2f",
	                                            117),
	                          uint512_make(0, 0, 0x012ULL, 0x34567890abcdef00ULL, 0x112233aabbccdd74ULL,
	                                       0x4b5a85f8e5446aaeULL, 0x9c8eb4bb7ffe214bULL, 0xa3a19049fb49b1b1ULL)));
	EXPECT_TRUE(uint512_equal(
	    string_to_uint512("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe214ba3a19049fb49b1b17e56ff1ef9"
	                      "3d9caa47e331a5fU14af481d3f864b3f0bf2f",
	                      117),
	    uint512_make(0, 0x01234567890aULL, 0xbcdef00112233aabULL, 0xbccdd744b5a85f8eULL, 0x5446aae9c8eb4bb7ULL,
	                 0xffe214ba3a19049fULL, 0xb49b1b17e56ff1efULL, 0x93d9caa47e331a5fULL)));
	EXPECT_TRUE(uint512_equal(
	    string_to_uint512("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe214ba3a19049fb49b1b17e56ff1ef9"
	                      "3d9caa47e331a5f14af481d3Uf864b3f0bf2f",
	                      117),
	    uint512_make(0x01234ULL, 0x567890abcdef0011ULL, 0x2233aabbccdd744bULL, 0x5a85f8e5446aae9cULL,
	                 0x8eb4bb7ffe214ba3ULL, 0xa19049fb49b1b17eULL, 0x56ff1ef93d9caa47ULL, 0xe331a5f14af481d3ULL)));
	EXPECT_TRUE(uint512_equal(
	    string_to_uint512("1234567890abcdef00112233aabbccdd744b5a85f8e5446aae9c8eb4bb7ffe214ba3a19049fb49b1b17e56ff1ef9"
	                      "3d9caa47e331a5f14af481d3fU864b3f0bf2f",
	                      117),
	    uint512_make(0x012345ULL, 0x67890abcdef00112ULL, 0x233aabbccdd744b5ULL, 0xa85f8e5446aae9c8ULL,
	                 0xeb4bb7ffe214ba3aULL, 0x19049fb49b1b17e5ULL, 0x6ff1ef93d9caa47eULL, 0x331a5f14af481d3fULL)));

	EXPECT_REALEQ(string_to_float32(nullptr, 0), 0);
	EXPECT_REALEQ(string_to_float32(STRING_CONST("0")), 0);
	EXPECT_REALEQ(string_to_float32(STRING_CONST("-0")), 0);
	EXPECT_REALEQ(string_to_float32(STRING_CONST("1234.5f")), REAL_C(1234.5));
	EXPECT_REALEQ(string_to_float32(STRING_CONST("-1234.5f")), REAL_C(-1234.5));
	EXPECT_REALEQ(string_to_float32(STRING_CONST("a1234.5f")), 0);
	EXPECT_REALEQ(string_to_float32(STRING_CONST("b-1234.5f")), 0);

	EXPECT_REALEQ((real)string_to_float64(nullptr, 0), 0);
	EXPECT_REALEQ((real)string_to_float64(STRING_CONST("0")), 0);
	EXPECT_REALEQ((real)string_to_float64(STRING_CONST("-0")), 0);
	EXPECT_REALEQ((real)string_to_float64(STRING_CONST("1234.5")), REAL_C(1234.5));
	EXPECT_REALEQ((real)string_to_float64(STRING_CONST("-1234.5")), REAL_C(-1234.5));
	EXPECT_REALEQ((real)string_to_float64(STRING_CONST("a1234.5f")), 0);
	EXPECT_REALEQ((real)string_to_float64(STRING_CONST("b-1234.5f")), 0);

	EXPECT_REALEQ((real)string_to_real(nullptr, 0), 0);
	EXPECT_REALEQ((real)string_to_real(STRING_CONST("0")), 0);
	EXPECT_REALEQ((real)string_to_real(STRING_CONST("-0")), 0);
	EXPECT_REALEQ((real)string_to_real(STRING_CONST("1234.5")), REAL_C(1234.5));
	EXPECT_REALEQ((real)string_to_real(STRING_CONST("-1234.5")), REAL_C(-1234.5));
	EXPECT_REALEQ((real)string_to_real(STRING_CONST("a1234.5f")), 0);
	EXPECT_REALEQ((real)string_to_real(STRING_CONST("b-1234.5f")), 0);

	EXPECT_TRUE(uuid_equal(string_to_uuid(nullptr, 0), uuid_null()));
	EXPECT_TRUE(uuid_equal(string_to_uuid(STRING_CONST("0")), uuid_null()));
	EXPECT_TRUE(uuid_equal(string_to_uuid(STRING_CONST("6ba7b810-9dad-11d1-80b4-00c04fd430c8")), UUID_DNS));
	EXPECT_TRUE(uuid_equal(string_to_uuid(STRING_CONST("00000000-0000-0000-0000-000000000000")), uuid_null()));
	EXPECT_TRUE(uuid_equal(string_to_uuid(STRING_CONST("0-0-0-0-0")), uuid_null()));
	EXPECT_TRUE(uuid_equal(string_to_uuid(STRING_CONST("just-string")), uuid_null()));

	EXPECT_TRUE(uint128_equal(string_to_version(nullptr, 0).version, uint128_make(0, 0)));
	EXPECT_TRUE(uint128_equal(string_to_version("1.2.3-4-5", 0).version, uint128_make(0, 0)));
	EXPECT_TRUE(uint128_equal(string_to_version(STRING_CONST("1.2.3-4-2abversion")).version,
	                          version_make(1, 2, 3, 4, 0x2ab).version));
	EXPECT_TRUE(uint128_equal(string_to_version("1.2.3-4-2abc", 8).version, version_make(1, 2, 3, 4, 0).version));
	EXPECT_TRUE(uint128_equal(string_to_version("1.2.3-4-5", 6).version, version_make(1, 2, 3, 0, 0).version));
	EXPECT_TRUE(uint128_equal(string_to_version("1.2.3-4-5", 3).version, version_make(1, 2, 0, 0, 0).version));

	return 0;
}

/*
TODO: Make string conversion routines locale-invariant
DECLARE_TEST(string, locale) {
    const char* test_locales[] = {"C", "", "en-US", "sv-SE"};

    for (size_t iloc = 0, lcount = sizeof(test_locales) / sizeof(test_locales[0]); iloc < lcount; ++iloc) {
        setlocale(LC_ALL, test_locales[iloc]);

        string_const_t str = string_from_real_static(0, 0, 0, 0);
        EXPECT_CONSTSTRINGEQ(str, string_const(STRING_CONST("0")));
        str = string_from_real_static(1.5, 0, 0, 0);
        EXPECT_CONSTSTRINGEQ(str, string_const(STRING_CONST("1.5")));
    }

    return 0;
}
*/

static void
test_string_declare(void) {
	ADD_TEST(string, allocate);
	ADD_TEST(string, queries);
	ADD_TEST(string, utility);
	ADD_TEST(string, append);
	ADD_TEST(string, prepend);
	ADD_TEST(string, format);
	ADD_TEST(string, convert);
	// ADD_TEST(string, locale);
}

static test_suite_t test_string_suite = {test_string_application,
                                         test_string_memory_system,
                                         test_string_config,
                                         test_string_declare,
                                         test_string_initialize,
                                         test_string_finalize,
                                         0};

#if BUILD_MONOLITHIC

int
test_string_run(void);

int
test_string_run(void) {
	test_suite = test_string_suite;
	return test_run_all();
}

#else

test_suite_t
test_suite_define(void);

test_suite_t
test_suite_define(void) {
	return test_string_suite;
}

#endif
