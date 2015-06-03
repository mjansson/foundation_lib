/* main.c  -  Foundation string tests  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
 *
 * This library provides a cross-platform foundation library in C11 providing basic support data types and
 * functions to write applications and games in a platform-independent fashion. The latest source code is
 * always available at
 *
 * https://github.com/rampantpixels/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without any restrictions.
 *
 */

#include <foundation/foundation.h>
#include <test/test.h>


static application_t test_string_application( void )
{
	application_t app;
	memset( &app, 0, sizeof( app ) );
	app.name = string_const( STRING_CONST( "Foundation string tests" ) );
	app.short_name = string_const( STRING_CONST( "test_string" ) );
	app.config_dir = string_const( STRING_CONST( "test_string" ) );
	app.flags = APPLICATION_UTILITY;
	app.dump_callback = test_crash_handler;
	return app;
}


static memory_system_t test_string_memory_system( void )
{
	return memory_system_malloc();
}


static foundation_config_t test_string_config( void )
{
	foundation_config_t config;
	memset( &config, 0, sizeof( config ) );
	return config;
}


static int test_string_initialize( void )
{
	return 0;
}


static void test_string_shutdown( void )
{
}


DECLARE_TEST( string, initialize )
{
	{
		string_t nullstr1 = string_allocate( 0, 0 );
		string_t nullstr2 = string_allocate( 1, 0 );
		string_t nullstr3 = string_allocate( 0, 1 );
		string_t nullstr4 = string_allocate( 1, 1 );
		string_t nullstr5 = string_allocate( 10, 32 );

		EXPECT_EQ( 0, nullstr1.length );
		EXPECT_EQ( 0, nullstr2.length );
		EXPECT_EQ( 0, nullstr3.length );
		EXPECT_EQ( 0, nullstr4.length );
		EXPECT_EQ( 10, nullstr5.length );
		EXPECT_EQ( 0, strcmp( nullstr1.str, "" ) );
		EXPECT_EQ( 0, strcmp( nullstr2.str, "" ) );
		EXPECT_EQ( 0, strcmp( nullstr3.str, "" ) );
		EXPECT_EQ( 0, strcmp( nullstr4.str, "" ) );
		EXPECT_EQ( 0, strcmp( nullstr5.str, "" ) );

		string_deallocate( nullstr1.str );
		string_deallocate( nullstr2.str );
		string_deallocate( nullstr3.str );
		string_deallocate( nullstr4.str );
		string_deallocate( nullstr5.str );
	}
	{
		char teststr1[] = "test";
		char teststr2[] = "testing long string with more than 16 characters";
		char teststr3[] = "\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x09";
		string_const_t substr;
		string_t str1, str2, str3, str4, str5, str6, str7, str8, str9, str10, str11, str12, str13, str14, str15, str16;

		str1 = string_clone( STRING_CONST( teststr1 ) );
		substr = string_substr( STRING_CONST( teststr1 ), 0, 3 ); str2 = string_clone( STRING_ARGS( substr ) );
		substr = string_substr( STRING_CONST( teststr1 ), 0, 4 ); str3 = string_clone( STRING_ARGS( substr ) );
		substr = string_substr( STRING_CONST( teststr1 ), 0, 32 ); str4 = string_clone( STRING_ARGS( substr ) );
		substr = string_substr( STRING_CONST( teststr1 ), 0, STRING_NPOS ); str5 = string_clone( STRING_ARGS( substr ) );
		substr = string_substr( STRING_CONST( teststr1 ), 5, STRING_NPOS ); str6 = string_clone( STRING_ARGS( substr ) );

		str7 = string_clone( STRING_CONST( teststr2 ) );
		substr = string_substr( STRING_CONST( teststr2 ), 0, 3 ); str8 = string_clone( STRING_ARGS( substr ) );
		substr = string_substr( STRING_CONST( teststr2 ), 0, 20 ); str9 = string_clone( STRING_ARGS( substr ) );
		substr = string_substr( STRING_CONST( teststr2 ), 0, STRING_NPOS ); str10 = string_clone( STRING_ARGS( substr ) );
		substr = string_substr( STRING_CONST( teststr2 ), 256, STRING_NPOS ); str11 = string_clone( STRING_ARGS( substr ) );

		str12 = string_clone( STRING_CONST( teststr3 ) );
		substr = string_substr( STRING_CONST( teststr3 ), 0, 3 ); str13 = string_clone( STRING_ARGS( substr ) );
		substr = string_substr( STRING_CONST( teststr3 ), 0, 20 ); str14 = string_clone( STRING_ARGS( substr ) );
		substr = string_substr( STRING_CONST( teststr3 ), 0, STRING_NPOS ); str15 = string_clone( STRING_ARGS( substr ) );
		substr = string_substr( STRING_CONST( teststr3 ), STRING_NPOS, STRING_NPOS ); str16 = string_clone( STRING_ARGS( substr ) );

		EXPECT_EQ( 0, strcmp( str1.str, teststr1 ) );
		EXPECT_EQ( 0, strcmp( str2.str, "tes" ) );
		EXPECT_EQ( 0, strcmp( str3.str, teststr1 ) );
		EXPECT_EQ( 0, strcmp( str4.str, teststr1 ) );
		EXPECT_EQ( 0, strcmp( str5.str, teststr1 ) );
		EXPECT_EQ( 0, strcmp( str5.str, "" ) );

		EXPECT_EQ( 0, strcmp( str7.str, teststr2 ) );
		EXPECT_EQ( 0, strcmp( str8.str, "tes" ) );
		EXPECT_EQ( 0, strcmp( str9.str, "testing long string " ) );
		EXPECT_EQ( 0, strcmp( str10.str, teststr2 ) );
		EXPECT_EQ( 0, strcmp( str11.str, "" ) );

		EXPECT_EQ( 0, strcmp( str12.str, teststr3 ) );
		EXPECT_EQ( 0, strcmp( str13.str, "\x01\x02\x03" ) );
		EXPECT_EQ( 0, strcmp( str14.str, "\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02" ) );
		EXPECT_EQ( 0, strcmp( str15.str, teststr3 ) );
		EXPECT_EQ( 0, strcmp( str16.str, "" ) );

		string_deallocate( str1.str );
		string_deallocate( str2.str );
		string_deallocate( str3.str );
		string_deallocate( str4.str );
		string_deallocate( str5.str );
		string_deallocate( str6.str );
		string_deallocate( str7.str );
		string_deallocate( str8.str );
		string_deallocate( str9.str );
		string_deallocate( str10.str );
		string_deallocate( str11.str );
		string_deallocate( str12.str );
		string_deallocate( str13.str );
		string_deallocate( str14.str );
		string_deallocate( str15.str );
		string_deallocate( str16.str );
	}
	{
		wchar_t teststr1[] = L"test";
		wchar_t teststr2[] = L"testing long string with more than 16 characters";
		wchar_t teststr3[] = L"0\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009";
		wchar_t teststr4[] = L"Wide-char string with some strange characters: åäöÅÄÖ_________ (test utf-8 conversions with long strings)";
		//char utfteststr4[] = "Wide-char string with some strange characters: \xC3\xA5\xC3\xA4\xC3\xB6\xC3\x85\xC3\x84\xC3\x96\xEF\xB7\xB2\xDA\x81\xED\x80\x83\xEC\x99\xA7\xEB\x96\xAF\xE7\xBF\xB3\xE5\xA1\x8C\xE1\xA7\xBC\xE1\xA7\xAD (test utf-8 conversions with long strings)";

		string_t str1, str2, str3, str4, str5, str6, str7, str8, str9, str10, str11, str12, str13, str14, str15, str16, str17;
		wchar_t *wstr1, *wstr2, *wstr3, *wstr4, *wstr5, *wstr6, *wstr7, *wstr8, *wstr9, *wstr10, *wstr11, *wstr12, *wstr13, *wstr14, *wstr15, *wstr16, *wstr17;

		teststr4[53] = (wchar_t)0x602f;//L'ﷲ';
		teststr4[54] = (wchar_t)0xf045;//L'ځ';
		teststr4[55] = (wchar_t)0x1d30;//L'퀃';
		teststr4[56] = (wchar_t)0x0378;//L'왧';
		teststr4[57] = (wchar_t)0x0ffe;//L'떯';
		teststr4[58] = (wchar_t)0xffe0;//L'翳';
		teststr4[59] = (wchar_t)0x1234;//L'塌';
		teststr4[60] = (wchar_t)0x4321;//L'᧼';
		teststr4[61] = (wchar_t)0x0f0f;//L'᧭';

		str1 = string_allocate_from_wstring( teststr1, 0 );
		str2 = string_allocate_from_wstring( teststr1, 3 );
		str3 = string_allocate_from_wstring( teststr1, 4 );
		str4 = string_allocate_from_wstring( teststr1, 32 );
		str5 = string_allocate_from_wstring( teststr1, STRING_NPOS );

		str6 = string_allocate_from_wstring( teststr2, 0 );
		str7 = string_allocate_from_wstring( teststr2, 3 );
		str8 = string_allocate_from_wstring( teststr2, 20 );
		str9 = string_allocate_from_wstring( teststr2, STRING_NPOS );

		str10 = string_allocate_from_wstring( teststr3, 0 );
		str11 = string_allocate_from_wstring( teststr3, 3 );
		str12 = string_allocate_from_wstring( teststr3, 20 );
		str13 = string_allocate_from_wstring( teststr3, STRING_NPOS );

		str14 = string_allocate_from_wstring( teststr4, 0 );
		str15 = string_allocate_from_wstring( teststr4, 3 );
		str16 = string_allocate_from_wstring( teststr4, 63 );
		str17 = string_allocate_from_wstring( teststr4, STRING_NPOS );

		wstr1 = wstring_allocate_from_string( str1.str, str1.length );
		wstr2 = wstring_allocate_from_string( str2.str, 2 );
		wstr3 = wstring_allocate_from_string( str3.str, str3.length );
		wstr4 = wstring_allocate_from_string( str4.str, str4.length );
		wstr5 = wstring_allocate_from_string( str5.str, str5.length );

		wstr6 = wstring_allocate_from_string( str6.str, str6.length );
		wstr7 = wstring_allocate_from_string( str7.str, str7.length );
		wstr8 = wstring_allocate_from_string( str8.str, str8.length );
		wstr9 = wstring_allocate_from_string( str9.str, str9.length );

		wstr10 = wstring_allocate_from_string( str10.str, str10.length );
		wstr11 = wstring_allocate_from_string( str11.str, str11.length );
		wstr12 = wstring_allocate_from_string( str12.str, str12.length );
		wstr13 = wstring_allocate_from_string( str13.str, str13.length );

		wstr14 = wstring_allocate_from_string( str14.str, str14.length );
		wstr15 = wstring_allocate_from_string( str15.str, str15.length );
		wstr16 = wstring_allocate_from_string( str16.str, str16.length );
		wstr17 = wstring_allocate_from_string( str17.str, str17.length );

		EXPECT_EQ( 0, wcscmp( wstr1, teststr1 ) );
		EXPECT_EQ( 0, wcscmp( wstr2, L"te" ) );
		EXPECT_EQ( 0, wcscmp( wstr3, teststr1 ) );
		EXPECT_EQ( 0, wcscmp( wstr4, teststr1 ) );
		EXPECT_EQ( 0, wcscmp( wstr5, teststr1 ) );

		EXPECT_EQ( 0, wcscmp( wstr6, teststr2 ) );
		EXPECT_EQ( 0, wcscmp( wstr7, L"tes" ) );
		EXPECT_EQ( 0, wcscmp( wstr8, L"testing long string " ) );
		EXPECT_EQ( 0, wcscmp( wstr9, teststr2 ) );

		EXPECT_EQ( 0, wcscmp( wstr10, teststr3 ) );
		EXPECT_EQ( 0, wcscmp( wstr11, L"0\x0001\x0002" ) );
		EXPECT_EQ( 0, wcscmp( wstr12, L"0\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009" ) );
		EXPECT_EQ( 0, wcscmp( wstr13, teststr3 ) );

		EXPECT_EQ( 0, wcscmp( wstr14, teststr4 ) );
		EXPECT_EQ( 0, wcscmp( wstr15, L"Wid" ) );
		EXPECT_EQ( 0, wcscmp( wstr17, teststr4 ) );
		teststr4[63] = 0;
		EXPECT_EQ( 0, wcscmp( wstr16, teststr4 ) );

		{
			wchar_t wteststr[] = { 0x0100, 0x078f, 0x1234, 0xFF03, 0xD854, 0xDC53, 0x0032, 0 };
			string_t utf8_teststr = string_allocate_from_wstring( wteststr, sizeof( wteststr ) );
			wchar_t* wchar_teststr = wstring_allocate_from_string( STRING_ARGS( utf8_teststr ) );
			EXPECT_EQ( 0, wcscmp( wteststr, wchar_teststr ) );
			wstring_deallocate( wchar_teststr );
			string_deallocate( utf8_teststr.str );
		}

		{
			uint16_t wteststr[] = { 0xFEFF, 0x0100, 0x078f, 0x1234, 0xFF03, 0xDB02, 0xDC54, 0x0032, 0xFFFE, 0x1234, 0xFF03, 0 };
#if FOUNDATION_SIZE_WCHAR == 4
			wchar_t wtestcmpstr[] = { 0x0100, 0x078f, 0x1234, 0xFF03, 0x000D0854, 0x0032, 0x3412, 0x03FF, 0 };
#else
			wchar_t wtestcmpstr[] = { 0x0100, 0x078f, 0x1234, 0xFF03, 0xDB02, 0xDC54, 0x0032, 0x3412, 0x03FF, 0 };
#endif
			string_t utf8_teststr = string_allocate_from_utf16( wteststr, sizeof( wteststr ) );
			wchar_t* wchar_teststr = wstring_allocate_from_string( STRING_ARGS( utf8_teststr ) );
			EXPECT_EQ( 0, wcscmp( wtestcmpstr, wchar_teststr ) );
			wstring_deallocate( wchar_teststr );
			string_deallocate( utf8_teststr.str );
		}

		{
			uint32_t wteststr[] = { 0x0000FEFF, 0x00000100, 0x0000078f, 0x00001234, 0x0000FF03, 0x000D0854, 0x000D0C53, 0x00000032, 0xFFFE0000, 0x12340000, 0xFF030000, 0 };
#if FOUNDATION_SIZE_WCHAR == 4
			wchar_t wtestcmpstr[] = { 0x0100, 0x078f, 0x1234, 0xFF03, 0x000D0854, 0x000D0C53, 0x0032, 0x3412, 0x03FF, 0 };
#else
			wchar_t wtestcmpstr[] = { 0x0100, 0x078f, 0x1234, 0xFF03, 0xDB02, 0xDC54, 0xDB03, 0xDC53, 0x0032, 0x3412, 0x03FF, 0 };
#endif
			string_t utf8_teststr = string_allocate_from_utf32( wteststr, sizeof( wteststr ) );
			wchar_t* wchar_teststr = wstring_allocate_from_string( STRING_ARGS( utf8_teststr ) );
			EXPECT_EQ( 0, wcscmp( wtestcmpstr, wchar_teststr ) );
			wstring_deallocate( wchar_teststr );
			string_deallocate( utf8_teststr.str );
		}

		string_deallocate( str1.str );
		string_deallocate( str2.str );
		string_deallocate( str3.str );
		string_deallocate( str4.str );
		string_deallocate( str5.str );
		string_deallocate( str6.str );
		string_deallocate( str7.str );
		string_deallocate( str8.str );
		string_deallocate( str9.str );
		string_deallocate( str10.str );
		string_deallocate( str11.str );
		string_deallocate( str12.str );
		string_deallocate( str13.str );
		string_deallocate( str14.str );
		string_deallocate( str15.str );
		string_deallocate( str16.str );
		string_deallocate( str17.str );

		wstring_deallocate( wstr1 );
		wstring_deallocate( wstr2 );
		wstring_deallocate( wstr3 );
		wstring_deallocate( wstr4 );
		wstring_deallocate( wstr5 );
		wstring_deallocate( wstr6 );
		wstring_deallocate( wstr7 );
		wstring_deallocate( wstr8 );
		wstring_deallocate( wstr9 );
		wstring_deallocate( wstr10 );
		wstring_deallocate( wstr11 );
		wstring_deallocate( wstr12 );
		wstring_deallocate( wstr13 );
		wstring_deallocate( wstr14 );
		wstring_deallocate( wstr15 );
		wstring_deallocate( wstr16 );
		wstring_deallocate( wstr17 );
	}
	return 0;
}


DECLARE_TEST( string, queries )
{
	{
		char teststr[]  = "";
		char teststr2[] = "test";
		char teststr3[] = "testing long string with more than 16 characters";
		char teststr4[] = "01234567890123456789012345678901234567890123456789012345678901234567890123456789";

		string_const_t str1 = string_substr( STRING_CONST( teststr ), 0, 0 );
		string_const_t str2 = string_substr( STRING_CONST( teststr2 ), 0, 4 );
		string_const_t str3 = string_substr( STRING_CONST( teststr2 ), 0, 20 );
		string_const_t str4 = string_substr( STRING_CONST( teststr3 ), 0, 0 );
		string_const_t str5 = string_substr( STRING_CONST( teststr3 ), 4, 10 );
		string_const_t str6 = string_substr( STRING_CONST( teststr3 ), 0, STRING_NPOS );
		string_const_t str7 = string_substr( STRING_CONST( teststr4 ), 10, 0 );
		string_const_t str8 = string_substr( STRING_CONST( teststr4 ), 0, 20 );
		string_const_t str9 = string_substr( STRING_CONST( teststr4 ), 10, STRING_NPOS );

		EXPECT_EQ( strlen( teststr ), str1.length );
		EXPECT_EQ( strlen( teststr2 ), str2.length );
		EXPECT_EQ( strlen( teststr2 ), str3.length );
		EXPECT_EQ( 0U, str4.length );
		EXPECT_EQ( 10U, str5.length );
		EXPECT_EQ( strlen( teststr3 ), str6.length );
		EXPECT_EQ( 0U, str7.length );
		EXPECT_EQ( 20U, str8.length );
		EXPECT_EQ( strlen( teststr4 ) - 10, str9.length );
	}
	{
		//Only ASCII characters, so wstring->string conversion should not introduce any extra UTF-8 sequences
		wchar_t teststr[]  = L"";
		wchar_t teststr2[] = L"test";
		wchar_t teststr3[] = L"testing long string with more than 16 characters";
		wchar_t teststr4[] = L"01234567890123456789012345678901234567890123456789012345678901234567890123456789";

		string_t str1 = string_allocate_from_wstring( teststr, sizeof( teststr ) );
		string_t str2 = string_allocate_from_wstring( teststr2, sizeof( teststr2 ) );
		string_t str3 = string_allocate_from_wstring( teststr2, 20 );
		string_t str4 = string_allocate_from_wstring( teststr3, 0 );
		string_t str5 = string_allocate_from_wstring( teststr3, 10 );
		string_t str6 = string_allocate_from_wstring( teststr3, STRING_NPOS );
		string_t str7 = string_allocate_from_wstring( teststr4, sizeof( teststr4 ) );
		string_t str8 = string_allocate_from_wstring( teststr4, 20 );
		string_t str9 = string_allocate_from_wstring( teststr4, STRING_NPOS );

		EXPECT_EQ( 0, str1.str[ str1.length ] );
		EXPECT_EQ( 0, str2.str[ str2.length ] );
		EXPECT_EQ( 0, str3.str[ str3.length ] );
		EXPECT_EQ( 0, str4.str[ str4.length ] );
		EXPECT_EQ( 0, str5.str[ str5.length ] );
		EXPECT_EQ( 0, str6.str[ str6.length ] );
		EXPECT_EQ( 0, str7.str[ str7.length ] );
		EXPECT_EQ( 0, str8.str[ str8.length ] );
		EXPECT_EQ( 0, str9.str[ str9.length ] );

		EXPECT_EQ( wcslen( teststr ), str1.length );
		EXPECT_EQ( wcslen( teststr2 ), str2.length );
		EXPECT_EQ( wcslen( teststr2 ), str3.length );
		EXPECT_EQ( 0, str4.length );
		EXPECT_EQ( 10U, str5.length );
		EXPECT_EQ( wcslen( teststr3 ), str6.length );
		EXPECT_EQ( wcslen( teststr4 ), str7.length );
		EXPECT_EQ( 20U, str8.length );
		EXPECT_EQ( wcslen( teststr4 ), str9.length );

		string_deallocate( str1.str );
		string_deallocate( str2.str );
		string_deallocate( str3.str );
		string_deallocate( str4.str );
		string_deallocate( str5.str );
		string_deallocate( str6.str );
		string_deallocate( str7.str );
		string_deallocate( str8.str );
		string_deallocate( str9.str );
	}
	{
		string_const_t emptystr = string_null();
		string_const_t shortstr = string_const( STRING_CONST( "short string" ) );
		string_const_t longstr  = string_const( STRING_CONST( "testing utility string methods like finds, split, merge, explode and similar." ) );
		{
			size_t find     = string_find( STRING_ARGS( longstr ), ' ', 0 );
			size_t find2    = string_find( STRING_ARGS( longstr ), 12, 0 );
			size_t find3    = string_find( STRING_ARGS( emptystr ), ' ', 0 );
			size_t find4    = string_find( STRING_ARGS( shortstr ), ' ', 0 );
			size_t find5    = string_find( STRING_ARGS( shortstr ), 'z', 0 );
			size_t findofs  = string_find( STRING_ARGS( longstr ), ' ', find );
			size_t findofs2 = string_find( STRING_ARGS( longstr ), ' ', find + 1 );
			size_t findofs3 = string_find( STRING_ARGS( longstr ), 'z', 10 );
			size_t findofs4 = string_find( STRING_ARGS( emptystr ), 'z', STRING_NPOS );
			size_t findofs5 = string_find( STRING_ARGS( shortstr ), 's', 10 );
			size_t findofs6 = string_find( STRING_ARGS( shortstr ), 's', findofs5 );
			size_t findofs7 = string_find( STRING_ARGS( shortstr ), 't', 0 );
			size_t findofs8 = string_find( STRING_ARGS( shortstr ), ' ', 5 );
			size_t findofs9 = string_find( STRING_ARGS( longstr ), ' ', STRING_NPOS );

			EXPECT_EQ( find, 7U );
			EXPECT_EQ( find2, STRING_NPOS );
			EXPECT_EQ( find3, STRING_NPOS );
			EXPECT_EQ( find4, 5U );
			EXPECT_EQ( find5, STRING_NPOS );
			EXPECT_EQ( findofs, find );
			EXPECT_EQ( findofs2, 15U );
			EXPECT_EQ( findofs3, STRING_NPOS );
			EXPECT_EQ( findofs4, STRING_NPOS );
			EXPECT_EQ( findofs5, STRING_NPOS );
			EXPECT_EQ( findofs6, STRING_NPOS );
			EXPECT_EQ( findofs7, 4U );
			EXPECT_EQ( findofs8, 5U );
			EXPECT_EQ( findofs9, STRING_NPOS );
		}
		{
			size_t rfind     = string_rfind( STRING_ARGS( longstr ), ' ', STRING_NPOS );
			size_t rfind2    = string_rfind( STRING_ARGS( longstr ), ';', STRING_NPOS );
			size_t rfind3    = string_rfind( STRING_ARGS( emptystr ), ';', STRING_NPOS );
			size_t rfind4    = string_rfind( STRING_ARGS( shortstr ), 's', STRING_NPOS );
			size_t rfind5    = string_rfind( STRING_ARGS( shortstr ), 'z', STRING_NPOS );
			size_t rfindofs  = string_rfind( STRING_ARGS( longstr ), ' ', rfind );
			size_t rfindofs2 = string_rfind( STRING_ARGS( longstr ), ' ', rfind - 1 );
			size_t rfindofs3 = string_rfind( STRING_ARGS( longstr ), ' ', longstr.length - 1 );
			size_t rfindofs4 = string_rfind( STRING_ARGS( emptystr ), ' ', 0 );
			size_t rfindofs5 = string_rfind( STRING_ARGS( longstr ), ' ', 0 );
			size_t rfindofs6 = string_rfind( STRING_ARGS( shortstr ), 's', 5 );
			size_t rfindofs7 = string_rfind( STRING_ARGS( shortstr ), 's', 0 );

			EXPECT_EQ( rfind, 68U );
			EXPECT_EQ( rfind2, STRING_NPOS );
			EXPECT_EQ( rfind3, STRING_NPOS );
			EXPECT_EQ( rfind4, 6U );
			EXPECT_EQ( rfind5, STRING_NPOS );
			EXPECT_EQ( rfindofs, rfind );
			EXPECT_EQ( rfindofs2, 64U );
			EXPECT_EQ( rfindofs3, rfind );
			EXPECT_EQ( rfindofs4, STRING_NPOS );
			EXPECT_EQ( rfindofs5, STRING_NPOS );
			EXPECT_EQ( rfindofs6, 0U );
			EXPECT_EQ( rfindofs7, 0U );
		}
		{
			size_t findstr     = string_find_string( STRING_ARGS( longstr ), STRING_CONST( "st" ), 0 );
			size_t findstr2    = string_find_string( STRING_ARGS( longstr ), STRING_CONST( "xwqz" ), 0 );
			size_t findstr3    = string_find_string( STRING_ARGS( emptystr ), STRING_CONST( "xwqz" ), 0 );
			size_t findstr4    = string_find_string( STRING_ARGS( longstr ), STRING_CONST( "" ), 0 );
			size_t findstr5    = string_find_string( STRING_ARGS( longstr ), STRING_CONST( "dslike" ), 0 );
			size_t findstr6    = string_find_string( STRING_ARGS( shortstr ), STRING_CONST( "rt" ), 0 );
			size_t findstr7    = string_find_string( STRING_ARGS( shortstr ), STRING_CONST( "long key that does not exist" ), 0 );
			size_t findstr8    = string_find_string( STRING_ARGS( shortstr ), STRING_CONST( "so" ), 0 );
			size_t findstrofs   = string_find_string( STRING_ARGS( longstr ), STRING_CONST( "st" ), findstr );
			size_t findstrofs2  = string_find_string( STRING_ARGS( longstr ), STRING_CONST( "st" ), findstr + 1 );
			size_t findstrofs3  = string_find_string( STRING_ARGS( longstr ), STRING_CONST( "xwqz" ), longstr.length );
			size_t findstrofs4  = string_find_string( STRING_ARGS( emptystr ), STRING_CONST( "xwqz" ), emptystr.length );
			size_t findstrofs5  = string_find_string( STRING_ARGS( shortstr ), STRING_CONST( "" ), 5 );
			size_t findstrofs6  = string_find_string( STRING_ARGS( shortstr ), STRING_CONST( "string" ), 0 );
			size_t findstrofs7  = string_find_string( STRING_ARGS( shortstr ), STRING_CONST( "string" ), 7 );
			size_t findstrofs8  = string_find_string( STRING_ARGS( longstr ), STRING_CONST( "utility" ), 14 );
			size_t findstrofs9  = string_find_string( STRING_ARGS( longstr ), STRING_CONST( "" ), longstr.length );
			size_t findstrofs10 = string_find_string( STRING_ARGS( longstr ), STRING_CONST( "" ), STRING_NPOS );
			size_t findstrofs11 = string_find_string( STRING_ARGS( longstr ), STRING_CONST( "string" ), STRING_NPOS );

			EXPECT_EQ( findstr, 2U );
			EXPECT_EQ( findstr2, STRING_NPOS );
			EXPECT_EQ( findstr3, STRING_NPOS );
			EXPECT_EQ( findstr4, 0U );
			EXPECT_EQ( findstr5, STRING_NPOS );
			EXPECT_EQ( findstr6, 3U );
			EXPECT_EQ( findstr7, STRING_NPOS );
			EXPECT_EQ( findstr8, STRING_NPOS );
			EXPECT_EQ( findstrofs, findstr );
			EXPECT_EQ( findstrofs2, 16U );
			EXPECT_EQ( findstrofs3, STRING_NPOS );
			EXPECT_EQ( findstrofs4, STRING_NPOS );
			EXPECT_EQ( findstrofs5, 5U );
			EXPECT_EQ( findstrofs6, 6U );
			EXPECT_EQ( findstrofs7, STRING_NPOS );
			EXPECT_EQ( findstrofs8, STRING_NPOS );
			EXPECT_EQ( findstrofs9, longstr.length );
			EXPECT_EQ( findstrofs10, STRING_NPOS );
			EXPECT_EQ( findstrofs11, STRING_NPOS );
		}
		{
			size_t rfindstr     = string_rfind_string( STRING_ARGS( longstr ), STRING_CONST( ", " ), STRING_NPOS );
			size_t rfindstr2    = string_rfind_string( STRING_ARGS( longstr ), STRING_CONST( ":;" ), STRING_NPOS );
			size_t rfindstr3    = string_rfind_string( STRING_ARGS( emptystr ), STRING_CONST( ":;" ), STRING_NPOS );
			size_t rfindstr4    = string_rfind_string( STRING_ARGS( longstr ), STRING_CONST( "" ), STRING_NPOS );
			size_t rfindstr5    = string_rfind_string( STRING_ARGS( shortstr ), STRING_CONST( "string" ), STRING_NPOS );
			size_t rfindstr6    = string_rfind_string( STRING_ARGS( shortstr ), STRING_CONST( " tring" ), STRING_NPOS );
			size_t rfindstrofs  = string_rfind_string( STRING_ARGS( longstr ), STRING_CONST( ", " ), rfindstr );
			size_t rfindstrofs2 = string_rfind_string( STRING_ARGS( longstr ), STRING_CONST( ", " ), rfindstr - 1 );
			size_t rfindstrofs3 = string_rfind_string( STRING_ARGS( longstr ), STRING_CONST( ":;" ), 0 );
			size_t rfindstrofs4 = string_rfind_string( STRING_ARGS( emptystr ), STRING_CONST( ":;" ), 0 );
			size_t rfindstrofs5 = string_rfind_string( STRING_ARGS( longstr ), STRING_CONST( "" ), 5 );
			size_t rfindstrofs6 = string_rfind_string( STRING_ARGS( shortstr ), STRING_CONST( "ort str" ), 6 );
			size_t rfindstrofs7 = string_rfind_string( STRING_ARGS( shortstr ), STRING_CONST( "ort str" ), 1 );

			EXPECT_EQ( rfindstr, 55U );
			EXPECT_EQ( rfindstr2, STRING_NPOS );
			EXPECT_EQ( rfindstr3, STRING_NPOS );
			EXPECT_EQ( rfindstr4, longstr.length );
			EXPECT_EQ( rfindstr5, 6U );
			EXPECT_EQ( rfindstr6, STRING_NPOS );
			EXPECT_EQ( rfindstrofs, rfindstr );
			EXPECT_EQ( rfindstrofs2, 48U );
			EXPECT_EQ( rfindstrofs3, STRING_NPOS );
			EXPECT_EQ( rfindstrofs4, STRING_NPOS );
			EXPECT_EQ( rfindstrofs5, 5U );
			EXPECT_EQ( rfindstrofs6, 2U );
			EXPECT_EQ( rfindstrofs7, STRING_NPOS );
		}
		{
			size_t findof        = string_find_first_of( STRING_ARGS( longstr ), STRING_CONST( "ui" ), 0 );
			size_t findof2       = string_find_first_of( STRING_ARGS( longstr ), STRING_CONST( ";:" ), 0 );
			size_t findof3       = string_find_first_of( STRING_ARGS( emptystr ), STRING_CONST( "" ), 0 );
			size_t findof4       = string_find_first_of( STRING_ARGS( emptystr ), STRING_CONST( " " ), 0 );
			size_t findof5       = string_find_first_of( STRING_ARGS( shortstr ), STRING_CONST( "" ), 0 );
			size_t findofofs     = string_find_first_of( STRING_ARGS( longstr ), STRING_CONST( "ui" ), findof );
			size_t findofofs2    = string_find_first_of( STRING_ARGS( longstr ), STRING_CONST( "ui" ), findof - 1 );
			size_t findofofs3    = string_find_first_of( STRING_ARGS( longstr ), STRING_CONST( "ui" ), findof + 1 );
			size_t findofofs4    = string_find_first_of( STRING_ARGS( longstr ), STRING_CONST( "ui" ), longstr.length );
			size_t findofofs5    = string_find_first_of( STRING_ARGS( emptystr ), STRING_CONST( "" ), emptystr.length );
			size_t findofofs6    = string_find_first_of( STRING_ARGS( shortstr ), STRING_CONST( "string" ), 6 );
			size_t findofofs7    = string_find_first_of( STRING_ARGS( shortstr ), STRING_CONST( "" ), 6 );
			size_t findofofs8    = string_find_first_of( STRING_ARGS( longstr ), STRING_CONST( "" ), 10 );
			size_t findofofs9    = string_find_first_of( STRING_ARGS( longstr ), STRING_CONST( "" ), longstr.length );

			EXPECT_EQ( findof, 4U );
			EXPECT_EQ( findof2, STRING_NPOS );
			EXPECT_EQ( findof3, STRING_NPOS );
			EXPECT_EQ( findof4, STRING_NPOS );
			EXPECT_EQ( findof5, STRING_NPOS );
			EXPECT_EQ( findofofs, 4U );
			EXPECT_EQ( findofofs2, 4U );
			EXPECT_EQ( findofofs3, 8U );
			EXPECT_EQ( findofofs4, STRING_NPOS );
			EXPECT_EQ( findofofs5, STRING_NPOS );
			EXPECT_EQ( findofofs6, 6U );
			EXPECT_EQ( findofofs7, STRING_NPOS );
			EXPECT_EQ( findofofs8, STRING_NPOS );
			EXPECT_EQ( findofofs9, STRING_NPOS );
		}
		{
			size_t findnotof     = string_find_first_not_of( STRING_ARGS( longstr ), STRING_CONST( "testing " ), 0 );
			size_t findnotof2    = string_find_first_not_of( STRING_ARGS( longstr ), STRING_ARGS( longstr ), 0 );
			size_t findnotof3    = string_find_first_not_of( STRING_ARGS( shortstr ), STRING_CONST( "" ), 0 );
			size_t findnotofofs  = string_find_first_not_of( STRING_ARGS( longstr ), STRING_CONST( "testing " ), findnotof );
			size_t findnotofofs2 = string_find_first_not_of( STRING_ARGS( longstr ), STRING_CONST( "testing " ), findnotof + 1 );
			size_t findnotofofs3 = string_find_first_not_of( STRING_ARGS( longstr ), STRING_CONST( "testing " ), longstr.length );
			size_t findnotofofs4 = string_find_first_not_of( STRING_ARGS( shortstr ), STRING_CONST( "" ), shortstr.length );

			EXPECT_EQ( findnotof, 8U );
			EXPECT_EQ( findnotof2, STRING_NPOS );
			EXPECT_EQ( findnotof3, 0U );
			EXPECT_EQ( findnotofofs, 8U );
			EXPECT_EQ( findnotofofs2, 11U );
			EXPECT_EQ( findnotofofs3, STRING_NPOS );
			EXPECT_EQ( findnotofofs4, STRING_NPOS );
		}
		{
			size_t findlastof        = string_find_last_of( STRING_ARGS( longstr ), STRING_CONST( "xp" ), STRING_NPOS );
			size_t findlastof2       = string_find_last_of( STRING_ARGS( longstr ), STRING_CONST( ";:" ), STRING_NPOS );
			size_t findlastof3       = string_find_last_of( STRING_ARGS( emptystr ), STRING_CONST( "" ), STRING_NPOS );
			size_t findlastof4       = string_find_last_of( STRING_ARGS( shortstr ), STRING_CONST( "" ), STRING_NPOS );
			size_t findlastofofs     = string_find_last_of( STRING_ARGS( longstr ), STRING_CONST( "xp" ), findlastof );
			size_t findlastofofs2    = string_find_last_of( STRING_ARGS( longstr ), STRING_CONST( "xp" ), findlastof - 2 );
			size_t findlastofofs3    = string_find_last_of( STRING_ARGS( longstr ), STRING_CONST( "xp" ), 0 );
			size_t findlastofofs4    = string_find_last_of( STRING_ARGS( emptystr ), STRING_CONST( "" ), 0 );
			size_t findlastofofs5    = string_find_last_of( STRING_ARGS( shortstr ), STRING_CONST( "" ), 5 );
			size_t findlastofofs6    = string_find_last_of( STRING_ARGS( shortstr ), STRING_CONST( "" ), shortstr.length );
			size_t findlastofofs7    = string_find_last_of( STRING_ARGS( shortstr ), STRING_CONST( "short" ), 5 );

			EXPECT_EQ( findlastof, 59U );
			EXPECT_EQ( findlastof2, STRING_NPOS );
			EXPECT_EQ( findlastof3, STRING_NPOS );
			EXPECT_EQ( findlastof4, STRING_NPOS );
			EXPECT_EQ( findlastofofs, 59U );
			EXPECT_EQ( findlastofofs2, 44U );
			EXPECT_EQ( findlastofofs3, STRING_NPOS );
			EXPECT_EQ( findlastofofs4, STRING_NPOS );
			EXPECT_EQ( findlastofofs5, STRING_NPOS );
			EXPECT_EQ( findlastofofs6, STRING_NPOS );
			EXPECT_EQ( findlastofofs7, 4U );
		}
		{
			size_t findlastnotof     = string_find_last_not_of( STRING_ARGS( longstr ), STRING_CONST( " similar." ), STRING_NPOS );
			size_t findlastnotof2    = string_find_last_not_of( STRING_ARGS( longstr ), STRING_ARGS( longstr ), STRING_NPOS );
			size_t findlastnotof3    = string_find_last_not_of( STRING_ARGS( emptystr ), STRING_CONST( "" ), STRING_NPOS );
			size_t findlastnotof4    = string_find_last_not_of( STRING_ARGS( shortstr ), STRING_CONST( "" ), STRING_NPOS );
			size_t findlastnotof5    = string_find_last_not_of( STRING_ARGS( longstr ), STRING_CONST( " similar" ), STRING_NPOS );
			size_t findlastnotofofs  = string_find_last_not_of( STRING_ARGS( longstr ), STRING_CONST( " similar." ), findlastnotof );
			size_t findlastnotofofs2 = string_find_last_not_of( STRING_ARGS( longstr ), STRING_CONST( " and similar." ), findlastnotof - 1 );
			size_t findlastnotofofs3 = string_find_last_not_of( STRING_ARGS( longstr ), STRING_ARGS( longstr ), 0 );
			size_t findlastnotofofs4 = string_find_last_not_of( STRING_ARGS( emptystr ), STRING_CONST( "" ), 0 );
			size_t findlastnotofofs5 = string_find_last_not_of( STRING_ARGS( shortstr ), STRING_CONST( "string" ), 5 );
			size_t findlastnotofofs6 = string_find_last_not_of( STRING_ARGS( shortstr ), STRING_CONST( "string " ), 5 );
			size_t findlastnotofofs7 = string_find_last_not_of( STRING_ARGS( shortstr ), STRING_CONST( "" ), 5 );
			size_t findlastnotofofs8 = string_find_last_not_of( STRING_ARGS( longstr ), STRING_CONST( "" ), longstr.length );

			EXPECT_EQ( findlastnotof, 67U );
			EXPECT_EQ( findlastnotof2, STRING_NPOS );
			EXPECT_EQ( findlastnotof3, STRING_NPOS );
			EXPECT_EQ( findlastnotof4, 11U );
			EXPECT_EQ( findlastnotof5, 76U );
			EXPECT_EQ( findlastnotofofs, 67U );
			EXPECT_EQ( findlastnotofofs2, 63U );
			EXPECT_EQ( findlastnotofofs3, STRING_NPOS );
			EXPECT_EQ( findlastnotofofs4, STRING_NPOS );
			EXPECT_EQ( findlastnotofofs5, 5U );
			EXPECT_EQ( findlastnotofofs6, 2U );
			EXPECT_EQ( findlastnotofofs7, 5U );
			EXPECT_EQ( findlastnotofofs8, longstr.length - 1 );
		}
	}
	return 0;
}


DECLARE_TEST( string, append )
{
	string_const_t nullstr = string_null();
	string_const_t emptystr = string_empty();
	string_const_t shortstr = string_const( STRING_CONST( "short" ) );
	string_const_t longstr = string_const( STRING_CONST( "long long long long long long long long long" ) );
	string_t val;
	char buffer[512];

	val = string_append( 0, 0, 0, STRING_ARGS( nullstr ) );
	EXPECT_STRINGEQ( val, nullstr );
	EXPECT_STRINGEQ( val, emptystr );
	EXPECT_EQ( val.str, 0 );

	val = string_append( 0, 0, 0, STRING_ARGS( emptystr ) );
	EXPECT_STRINGEQ( val, nullstr );
	EXPECT_STRINGEQ( val, emptystr );
	EXPECT_EQ( val.str, 0 );

	val = string_append( buffer, 0, 0, STRING_ARGS( nullstr ) );
	EXPECT_STRINGEQ( val, nullstr );
	EXPECT_STRINGEQ( val, emptystr );
	EXPECT_NE( val.str, 0 );

	val = string_append( buffer, 0, sizeof( buffer ), STRING_ARGS( nullstr ) );
	EXPECT_STRINGEQ( val, nullstr );
	EXPECT_STRINGEQ( val, emptystr );
	EXPECT_EQ( val.str, buffer );

	val = string_append_varg( buffer, 0, sizeof( buffer ), STRING_ARGS( nullstr ), STRING_ARGS( emptystr ), STRING_ARGS( nullstr ), STRING_ARGS( emptystr ), nullptr );
	EXPECT_STRINGEQ( val, nullstr );
	EXPECT_STRINGEQ( val, emptystr );
	EXPECT_EQ( val.str, buffer );

	val = string_append( buffer, 0, sizeof( buffer ), STRING_ARGS( shortstr ) );
	EXPECT_STRINGEQ( val, shortstr );
	EXPECT_EQ( val.str, buffer );

	val = string_clone( STRING_ARGS( shortstr ) );
	val = string_append( STRING_ARGS_CAPACITY( val ), STRING_ARGS( nullstr ) );
	EXPECT_STRINGEQ( val, shortstr );
	string_deallocate( val.str );

	val = string_clone( STRING_ARGS( shortstr ) );
	val = string_append( STRING_ARGS_CAPACITY( val ), STRING_ARGS( shortstr ) );
	EXPECT_STRINGEQ( val, shortstr );
	string_deallocate( val.str );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( shortstr ) );
	val = string_append( STRING_ARGS( val ), sizeof( buffer ), STRING_ARGS( emptystr ) );
	EXPECT_STRINGEQ( val, shortstr );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( shortstr ) );
	val = string_append( STRING_ARGS( val ), sizeof( buffer ), STRING_ARGS( shortstr ) );
	EXPECT_STRINGEQ( val, string_const( STRING_CONST( "shortshort" ) ) );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( shortstr ) );
	val = string_append_varg( STRING_ARGS( val ), sizeof( buffer ), STRING_ARGS( shortstr ), STRING_ARGS( nullstr ), STRING_ARGS( shortstr ), STRING_ARGS( emptystr ), nullptr );
	EXPECT_STRINGEQ( val, string_const( STRING_CONST( "shortshort" ) ) );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( shortstr ) );
	val = string_append_varg( STRING_ARGS_CAPACITY( val ), STRING_ARGS( shortstr ), STRING_ARGS( nullstr ), STRING_ARGS( shortstr ), STRING_ARGS( emptystr ), nullptr );
	EXPECT_STRINGEQ( val, shortstr );

	val = string_append( buffer, 0, sizeof( buffer ), STRING_ARGS( longstr ) );
	EXPECT_STRINGEQ( val, longstr );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( longstr ) );
	val = string_append( STRING_ARGS_CAPACITY( val ), STRING_ARGS( nullstr ) );
	EXPECT_STRINGEQ( val, longstr );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( emptystr ) );
	val = string_append( STRING_ARGS( val ), sizeof( buffer ), STRING_ARGS( longstr ) );
	EXPECT_STRINGEQ( val, longstr );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( emptystr ) );
	val = string_append( STRING_ARGS_CAPACITY( val ), STRING_ARGS( longstr ) );
	EXPECT_STRINGEQ( val, emptystr );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( longstr ) );
	val = string_append( STRING_ARGS_CAPACITY( val ), STRING_ARGS( emptystr ) );
	EXPECT_STRINGEQ( val, longstr );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( shortstr ) );
	val = string_append( STRING_ARGS( val ), sizeof( buffer ), STRING_ARGS( longstr ) );
	EXPECT_STRINGEQ( val, string_const( STRING_CONST( "shortlong long long long long long long long long" ) ) );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( shortstr ) );
	val = string_append( STRING_ARGS_CAPACITY( val ), STRING_ARGS( longstr ) );
	EXPECT_STRINGEQ( val, shortstr );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( longstr ) );
	val = string_append( STRING_ARGS( val ), sizeof( buffer ), STRING_ARGS( shortstr ) );
	EXPECT_STRINGEQ( val, string_const( STRING_CONST( "long long long long long long long long longshort" ) ) );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( longstr ) );
	val = string_append( STRING_ARGS( val ), sizeof( buffer ), STRING_ARGS( longstr ) );
	EXPECT_STRINGEQ( val, string_const( STRING_CONST( "long long long long long long long long longlong long long long long long long long long" ) ) );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( shortstr ) );
	val = string_append_varg( buffer, 0, sizeof( buffer ), STRING_ARGS( longstr ), STRING_ARGS( nullstr ), STRING_ARGS( shortstr ), STRING_ARGS( longstr ), STRING_ARGS( emptystr ), STRING_ARGS( shortstr ), STRING_ARGS( nullstr ), nullptr );
	EXPECT_STRINGEQ( val, string_const( STRING_CONST( "long long long long long long long long longshortlong long long long long long long long longshort" ) ) );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( longstr ) );
	val = string_append_varg( val.str, 0, 32, STRING_ARGS( shortstr ), STRING_ARGS( nullstr ), STRING_ARGS( longstr ), STRING_ARGS( longstr ), STRING_ARGS( emptystr ), STRING_ARGS( shortstr ), STRING_ARGS( longstr ), STRING_ARGS( nullstr ), nullptr );
	EXPECT_STRINGEQ( val, string_const( STRING_CONST( "shortlong long long long long l" ) ) );

	return 0;
}


DECLARE_TEST( string, prepend )
{
	string_const_t nullstr = string_null();
	string_const_t emptystr = string_empty();
	string_const_t shortstr = string_const( STRING_CONST( "short" ) );
	string_const_t longstr = string_const( STRING_CONST( "long long long long long long long long long" ) );
	string_t val;
	char buffer[512];

	val = string_prepend( 0, 0, 0, STRING_ARGS( nullstr ) );
	EXPECT_STRINGEQ( val, nullstr );
	EXPECT_STRINGEQ( val, emptystr );
	EXPECT_EQ( val.str, 0 );

	val = string_prepend( 0, 0, 0, STRING_ARGS( emptystr ) );
	EXPECT_STRINGEQ( val, nullstr );
	EXPECT_STRINGEQ( val, emptystr );
	EXPECT_EQ( val.str, 0 );

	val = string_prepend( buffer, 0, 0, STRING_ARGS( nullstr ) );
	EXPECT_STRINGEQ( val, nullstr );
	EXPECT_STRINGEQ( val, emptystr );
	EXPECT_NE( val.str, 0 );

	val = string_prepend( buffer, 0, sizeof( buffer ), STRING_ARGS( nullstr ) );
	EXPECT_STRINGEQ( val, nullstr );
	EXPECT_STRINGEQ( val, emptystr );
	EXPECT_EQ( val.str, buffer );

	val = string_prepend_varg( buffer, 0, sizeof( buffer ), STRING_ARGS( nullstr ), STRING_ARGS( emptystr ), STRING_ARGS( nullstr ), STRING_ARGS( emptystr ), nullptr );
	EXPECT_STRINGEQ( val, nullstr );
	EXPECT_STRINGEQ( val, emptystr );
	EXPECT_EQ( val.str, buffer );

	val = string_prepend( buffer, 0, sizeof( buffer ), STRING_ARGS( shortstr ) );
	EXPECT_STRINGEQ( val, shortstr );
	EXPECT_EQ( val.str, buffer );

	val = string_clone( STRING_ARGS( shortstr ) );
	val = string_prepend( STRING_ARGS_CAPACITY( val ), STRING_ARGS( nullstr ) );
	EXPECT_STRINGEQ( val, shortstr );
	string_deallocate( val.str );

	val = string_clone( STRING_ARGS( shortstr ) );
	val = string_prepend( STRING_ARGS_CAPACITY( val ), STRING_ARGS( shortstr ) );
	EXPECT_STRINGEQ( val, shortstr );
	string_deallocate( val.str );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( shortstr ) );
	val = string_prepend( STRING_ARGS( val ), sizeof( buffer ), STRING_ARGS( emptystr ) );
	EXPECT_STRINGEQ( val, shortstr );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( shortstr ) );
	val = string_prepend( STRING_ARGS( val ), sizeof( buffer ), STRING_ARGS( shortstr ) );
	EXPECT_STRINGEQ( val, string_const( STRING_CONST( "shortshort" ) ) );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( shortstr ) );
	val = string_prepend_varg( STRING_ARGS( val ), sizeof( buffer ), STRING_ARGS( shortstr ), STRING_ARGS( nullstr ), STRING_ARGS( shortstr ), STRING_ARGS( emptystr ), nullptr );
	EXPECT_STRINGEQ( val, string_const( STRING_CONST( "shortshort" ) ) );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( shortstr ) );
	val = string_prepend_varg( STRING_ARGS_CAPACITY( val ), STRING_ARGS( shortstr ), STRING_ARGS( nullstr ), STRING_ARGS( shortstr ), STRING_ARGS( emptystr ), nullptr );
	EXPECT_STRINGEQ( val, shortstr );

	val = string_prepend( buffer, 0, sizeof( buffer ), STRING_ARGS( longstr ) );
	EXPECT_STRINGEQ( val, longstr );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( longstr ) );
	val = string_prepend( STRING_ARGS_CAPACITY( val ), STRING_ARGS( nullstr ) );
	EXPECT_STRINGEQ( val, longstr );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( emptystr ) );
	val = string_prepend( STRING_ARGS( val ), sizeof( buffer ), STRING_ARGS( longstr ) );
	EXPECT_STRINGEQ( val, longstr );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( emptystr ) );
	val = string_prepend( STRING_ARGS_CAPACITY( val ), STRING_ARGS( longstr ) );
	EXPECT_STRINGEQ( val, emptystr );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( longstr ) );
	val = string_prepend( STRING_ARGS_CAPACITY( val ), STRING_ARGS( emptystr ) );
	EXPECT_STRINGEQ( val, longstr );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( shortstr ) );
	val = string_prepend( STRING_ARGS( val ), sizeof( buffer ), STRING_ARGS( longstr ) );
	EXPECT_STRINGEQ( val, string_const( STRING_CONST( "long long long long long long long long longshort" ) ) );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( shortstr ) );
	val = string_prepend( STRING_ARGS_CAPACITY( val ), STRING_ARGS( longstr ) );
	EXPECT_STRINGEQ( val, shortstr );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( longstr ) );
	val = string_prepend( STRING_ARGS( val ), sizeof( buffer ), STRING_ARGS( shortstr ) );
	EXPECT_STRINGEQ( val, string_const( STRING_CONST( "shortlong long long long long long long long long" ) ) );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( longstr ) );
	val = string_prepend( STRING_ARGS( val ), sizeof( buffer ), STRING_ARGS( longstr ) );
	EXPECT_STRINGEQ( val, string_const( STRING_CONST( "long long long long long long long long longlong long long long long long long long long" ) ) );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( shortstr ) );
	val = string_prepend_varg( buffer, 0, sizeof( buffer ), STRING_ARGS( longstr ), STRING_ARGS( nullstr ), STRING_ARGS( shortstr ), STRING_ARGS( longstr ), STRING_ARGS( emptystr ), STRING_ARGS( shortstr ), STRING_ARGS( nullstr ), nullptr );
	EXPECT_STRINGEQ( val, string_const( STRING_CONST( "shortlong long long long long long long long longshortlong long long long long long long long long" ) ) );

	val = string_copy( buffer, sizeof( buffer ), STRING_ARGS( longstr ) );
	val = string_prepend_varg( val.str, 0, 32, STRING_ARGS( shortstr ), STRING_ARGS( nullstr ), STRING_ARGS( longstr ), STRING_ARGS( longstr ), STRING_ARGS( emptystr ), STRING_ARGS( shortstr ), STRING_ARGS( longstr ), STRING_ARGS( nullstr ), nullptr );
	EXPECT_STRINGEQ( val, string_const( STRING_CONST( "long long long long long long l" ) ) );

	return 0;
}


DECLARE_TEST( string, utility )
{
	{
		string_const_t explodearr[32];
		string_const_t explodestr = string_const( STRING_CONST( "  .,testing,    .,utility.,string  methods ..., like,,,finds  split..merge     .,.explode.and. .., ., similar   .,,,. " ) );

		string_t mergestr = string_clone( STRING_CONST( "    testing   merge string   " ) );
		string_t mergestr2 = string_clone( STRING_CONST( " ., testing, .merge.string,. " ) );
		string_t merged;

		string_t splitstr = string_clone( STRING_CONST( " testing split" ) );
		string_const_t splitright = string_null();
		string_const_t splitleft = string_null();
		string_const_t splitright2 = string_null();
		string_const_t splitleft2 = string_null();

		string_t substrtest = string_clone( STRING_CONST( "testing substr" ) );

		size_t numtokens;
		char buffer[512];

		numtokens = string_explode( STRING_ARGS( explodestr ), STRING_CONST( " ,." ), explodearr, sizeof( explodearr ) / sizeof( explodearr[0] ), false );

		EXPECT_EQ( numtokens, 11 );
		EXPECT_STREQ( STRING_ARGS( explodearr[0] ), STRING_CONST( "testing" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[1] ), STRING_CONST( "utility" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[2] ), STRING_CONST( "string" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[3] ), STRING_CONST( "methods" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[4] ), STRING_CONST( "like" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[5] ), STRING_CONST( "finds" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[6] ), STRING_CONST( "split" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[7] ), STRING_CONST( "merge" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[8] ), STRING_CONST( "explode" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[9] ), STRING_CONST( "and" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[10] ), STRING_CONST( "similar" ) );

		memset( explodearr, 0, sizeof( explodearr ) );
		numtokens = string_explode( STRING_ARGS( explodestr ), STRING_CONST( " ,." ), explodearr, 3, false );

		EXPECT_EQ( numtokens, 3 );
		EXPECT_STREQ( STRING_ARGS( explodearr[0] ), STRING_CONST( "testing" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[1] ), STRING_CONST( "utility" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[2] ), STRING_CONST( "string" ) );
		EXPECT_EQ( explodearr[3].str, 0 );
		EXPECT_EQ( explodearr[3].length, 0 );

		numtokens = string_explode( STRING_ARGS( mergestr2 ), STRING_CONST( " .," ), explodearr, 32, true );

		EXPECT_EQ( numtokens, 12 );
		EXPECT_STREQ( STRING_ARGS( explodearr[0] ), STRING_CONST( "" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[1] ), STRING_CONST( "" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[2] ), STRING_CONST( "" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[3] ), STRING_CONST( "" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[4] ), STRING_CONST( "testing" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[5] ), STRING_CONST( "" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[6] ), STRING_CONST( "" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[7] ), STRING_CONST( "merge" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[8] ), STRING_CONST( "string" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[9] ), STRING_CONST( "" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[10] ), STRING_CONST( "" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[11] ), STRING_CONST( "" ) );

		numtokens = string_explode( STRING_ARGS( mergestr2 ), STRING_CONST( " .," ), explodearr, 32, false );
		EXPECT_EQ( numtokens, 3 );
		EXPECT_STREQ( STRING_ARGS( explodearr[0] ), STRING_CONST( "testing" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[1] ), STRING_CONST( "merge" ) );
		EXPECT_STREQ( STRING_ARGS( explodearr[2] ), STRING_CONST( "string" ) );

		numtokens = string_explode( STRING_ARGS( mergestr2 ), STRING_CONST( " .," ), explodearr, 32, true );
		merged = string_merge( buffer, sizeof( buffer ), explodearr, numtokens, STRING_CONST( " " ) );
		EXPECT_STRINGEQ( merged, mergestr );

		string_split( STRING_ARGS( splitstr ), STRING_CONST( " " ), &splitleft, &splitright, false );
		string_split( STRING_ARGS( splitstr ), STRING_CONST( " " ), &splitleft2, &splitright2, true );

		EXPECT_TRUE( string_equal( STRING_ARGS( splitleft ), STRING_CONST( "testing" ) ) );
		EXPECT_TRUE( string_equal( STRING_ARGS( splitright ), STRING_CONST( "split" ) ) );
		EXPECT_TRUE( string_equal( STRING_ARGS( splitleft2 ), STRING_CONST( "" ) ) );
		EXPECT_TRUE( string_equal( STRING_ARGS( splitright2 ), STRING_CONST( "testing split" ) ) );

		EXPECT_STRINGEQ( string_substr( STRING_ARGS( substrtest ), 0, 4 ), string_const( STRING_CONST( "test" ) ) );
		EXPECT_STRINGEQ( string_substr( STRING_ARGS( substrtest ), 0, 14 ), string_const( STRING_CONST( "testing substr" ) ) );
		EXPECT_STRINGEQ( string_substr( STRING_ARGS( substrtest ), 0, 20 ), string_const( STRING_CONST( "testing substr" ) ) );
		EXPECT_STRINGEQ( string_substr( STRING_ARGS( substrtest ), 3, 20 ), string_const( STRING_CONST( "ting substr" ) ) );
		EXPECT_STRINGEQ( string_substr( STRING_ARGS( substrtest ), 3, 11 ), string_const( STRING_CONST( "ting substr" ) ) );
		EXPECT_STRINGEQ( string_substr( STRING_ARGS( substrtest ), 3, 1 ), string_const( STRING_CONST( "t" ) ) );
		EXPECT_STRINGEQ( string_substr( STRING_ARGS( substrtest ), 3, 0 ), string_const( STRING_CONST( "" ) ) );
		EXPECT_STRINGEQ( string_substr( STRING_ARGS( substrtest ), 20, 0 ), string_const( STRING_CONST( "" ) ) );
		EXPECT_STRINGEQ( string_substr( STRING_ARGS( substrtest ), 20, 20 ), string_const( STRING_CONST( "" ) ) );

		{
			string_t replacestr = string_copy( buffer, sizeof( buffer ), STRING_CONST( "testing replace" ) );
			replacestr = string_replace( STRING_ARGS( replacestr ), sizeof( buffer ), STRING_CONST( "rep" ), STRING_CONST( "testrep" ), false );
			EXPECT_STRINGEQ( replacestr, string_const( STRING_CONST( "testing testreplace" ) ) );

			replacestr = string_copy( buffer, sizeof( buffer ), STRING_CONST( "testing replace" ) );
			replacestr = string_replace( STRING_ARGS( replacestr ), sizeof( buffer ), STRING_CONST( "rep" ), STRING_CONST( "testrep" ), true );
			EXPECT_STRINGEQ( replacestr, string_const( STRING_CONST( "testing testreplace" ) ) );

			replacestr = string_copy( buffer, sizeof( buffer ), STRING_CONST( "testing replacelace" ) );
			replacestr = string_replace( STRING_ARGS( replacestr ), sizeof( buffer ), STRING_CONST( "replace" ), STRING_CONST( "testrep" ), true );
			EXPECT_STRINGEQ( replacestr, string_const( STRING_CONST( "testing testtestrep" ) ) );

			replacestr = string_copy( buffer, sizeof( buffer ), STRING_CONST( "" ) );
			replacestr = string_replace( STRING_ARGS( replacestr ), sizeof( buffer ), STRING_CONST( "foo" ), STRING_CONST( "bar" ), true );
			EXPECT_STRINGEQ( replacestr, string_const( STRING_CONST( "" ) ) );

			replacestr = string_copy( buffer, sizeof( buffer ), STRING_CONST( "repppppppppp" ) );
			replacestr = string_replace( STRING_ARGS( replacestr ), sizeof( buffer ), STRING_CONST( "rep" ), STRING_CONST( "re" ), true );
			EXPECT_STRINGEQ( replacestr, string_const( STRING_CONST( "re" ) ) );
		}
		{
			string_t stripstr = string_clone( STRING_CONST( "   testing strip :   " ) );
			string_t stripstr2 = string_clone( STRING_CONST( "   testing strip :   " ) );
			string_t stripstr3 = string_clone( STRING_CONST( "   testing strip :   " ) );

			string_const_t strippedstr = string_strip( STRING_ARGS( stripstr ), STRING_CONST( " tp:   " ) );
			string_const_t strippedstr2 = string_strip( STRING_ARGS( stripstr2 ), STRING_CONST( "" ) );
			string_const_t strippedstr3 = string_strip( STRING_ARGS( stripstr3 ), STRING_CONST( " tesingrp:" ) );

			EXPECT_STRINGEQ( strippedstr, string_const( STRING_CONST( "esting stri" ) ) );
			EXPECT_STRINGEQ( strippedstr2, string_const( STRING_CONST( "   testing strip :   " ) ) );
			EXPECT_STRINGEQ( strippedstr3, string_const( STRING_CONST( "" ) ) );

			string_deallocate( stripstr.str );
			string_deallocate( stripstr2.str );
			string_deallocate( stripstr3.str );
		}
		string_deallocate( mergestr.str );
		string_deallocate( mergestr2.str );
		string_deallocate( merged.str );
		string_deallocate( splitstr.str );
		string_deallocate( substrtest.str );
	}
	{
		#define SHORTSTRING "short"
		#define LONGSTRING  "long string with dynamic buffer storage but with no real useful data"
		string_t clonestr = string_clone( STRING_CONST( "" ) );
		string_t clonestr2 = string_clone( STRING_CONST( SHORTSTRING ) );
		string_t clonestr3 = string_clone( STRING_CONST( LONGSTRING ) );

		string_t teststr = string_clone( STRING_ARGS( clonestr ) );
		string_t teststr2 = string_clone( STRING_ARGS( clonestr2 ) );
		string_t teststr3 = string_clone( STRING_ARGS( clonestr3 ) );
		char buf[256];

		string_t concatstr = string_allocate_concat( STRING_ARGS( clonestr ), STRING_ARGS( teststr ) );
		string_t concatstr2 = string_allocate_concat( STRING_ARGS( clonestr ), STRING_ARGS( teststr2 ) );
		string_t concatstr3 = string_allocate_concat( STRING_ARGS( teststr2 ), STRING_ARGS( clonestr ) );
		string_t concatstr4 = string_allocate_concat( STRING_ARGS( clonestr2 ), STRING_ARGS( teststr2 ) );
		string_t concatstr5 = string_allocate_concat( STRING_ARGS( clonestr ), STRING_ARGS( teststr3 ) );
		string_t concatstr6 = string_allocate_concat( STRING_ARGS( teststr3 ), STRING_ARGS( clonestr ) );
		string_t concatstr7 = string_allocate_concat( STRING_ARGS( clonestr2 ), STRING_ARGS( teststr3 ) );
		string_t concatstr8 = string_allocate_concat( STRING_ARGS( teststr3 ), STRING_ARGS( clonestr2 ) );
		string_t concatstr9 = string_allocate_concat( STRING_ARGS( clonestr3 ), STRING_ARGS( teststr3 ) );
		string_t concatstr10 = string_allocate_concat( STRING_ARGS( teststr3 ), STRING_ARGS( clonestr3 ) );

		EXPECT_NE( teststr.str, clonestr.str );
		EXPECT_STRINGEQ( teststr, clonestr );

		EXPECT_NE( teststr2.str, clonestr2.str );
		EXPECT_STRINGEQ( teststr2, clonestr2 );

		EXPECT_NE( teststr3.str, clonestr3.str );
		EXPECT_STRINGEQ( teststr3, clonestr3 );

		EXPECT_STRINGEQ( concatstr, string_const( STRING_CONST( "" ) ) );
		EXPECT_STRINGEQ( concatstr2, string_const( STRING_CONST( SHORTSTRING ) ) );
		EXPECT_STRINGEQ( concatstr3, string_const( STRING_CONST( SHORTSTRING ) ) );
		EXPECT_STRINGEQ( concatstr4, string_const( STRING_CONST( SHORTSTRING SHORTSTRING ) ) );
		EXPECT_STRINGEQ( concatstr5, string_const( STRING_CONST( LONGSTRING ) ) );
		EXPECT_STRINGEQ( concatstr6, string_const( STRING_CONST( LONGSTRING ) ) );
		EXPECT_STRINGEQ( concatstr7, string_const( STRING_CONST( SHORTSTRING LONGSTRING ) ) );
		EXPECT_STRINGEQ( concatstr8, string_const( STRING_CONST( LONGSTRING SHORTSTRING ) ) );
		EXPECT_STRINGEQ( concatstr9, string_const( STRING_CONST( LONGSTRING LONGSTRING ) ) );
		EXPECT_STRINGEQ( concatstr10, string_const( STRING_CONST( LONGSTRING LONGSTRING ) ) );

		string_deallocate( teststr.str );
		string_deallocate( clonestr.str );
		string_deallocate( teststr2.str );
		string_deallocate( clonestr2.str );
		string_deallocate( teststr3.str );
		string_deallocate( clonestr3.str );
		string_deallocate( concatstr.str );
		string_deallocate( concatstr2.str );
		string_deallocate( concatstr3.str );
		string_deallocate( concatstr4.str );
		string_deallocate( concatstr5.str );
		string_deallocate( concatstr6.str );
		string_deallocate( concatstr7.str );
		string_deallocate( concatstr8.str );
		string_deallocate( concatstr9.str );
		string_deallocate( concatstr10.str );

		concatstr = string_concat( buf, sizeof( buf ), STRING_ARGS( clonestr ), STRING_ARGS( teststr ) );
		concatstr2 = string_concat( buf, sizeof( buf ), STRING_ARGS( clonestr ), STRING_ARGS( teststr2 ) );
		concatstr3 = string_concat( buf, sizeof( buf ), STRING_ARGS( teststr2 ), STRING_ARGS( clonestr ) );
		concatstr4 = string_concat( buf, sizeof( buf ), STRING_ARGS( clonestr2 ), STRING_ARGS( teststr2 ) );
		concatstr5 = string_concat( buf, sizeof( buf ), STRING_ARGS( clonestr ), STRING_ARGS( teststr3 ) );
		concatstr6 = string_concat( buf, sizeof( buf ), STRING_ARGS( teststr3 ), STRING_ARGS( clonestr ) );
		concatstr7 = string_concat( buf, sizeof( buf ), STRING_ARGS( clonestr2 ), STRING_ARGS( teststr3 ) );
		concatstr8 = string_concat( buf, sizeof( buf ), STRING_ARGS( teststr3 ), STRING_ARGS( clonestr2 ) );
		concatstr9 = string_concat( buf, sizeof( buf ), STRING_ARGS( clonestr3 ), STRING_ARGS( teststr3 ) );
		concatstr10 = string_concat( buf, sizeof( buf ), STRING_ARGS( teststr3 ), STRING_ARGS( clonestr3 ) );

		EXPECT_STRINGEQ( concatstr, string_const( STRING_CONST( "" ) ) );
		EXPECT_STRINGEQ( concatstr2, string_const( STRING_CONST( SHORTSTRING ) ) );
		EXPECT_STRINGEQ( concatstr3, string_const( STRING_CONST( SHORTSTRING ) ) );
		EXPECT_STRINGEQ( concatstr4, string_const( STRING_CONST( SHORTSTRING SHORTSTRING ) ) );
		EXPECT_STRINGEQ( concatstr5, string_const( STRING_CONST( LONGSTRING ) ) );
		EXPECT_STRINGEQ( concatstr6, string_const( STRING_CONST( LONGSTRING ) ) );
		EXPECT_STRINGEQ( concatstr7, string_const( STRING_CONST( SHORTSTRING LONGSTRING ) ) );
		EXPECT_STRINGEQ( concatstr8, string_const( STRING_CONST( LONGSTRING SHORTSTRING ) ) );
		EXPECT_STRINGEQ( concatstr9, string_const( STRING_CONST( LONGSTRING LONGSTRING ) ) );
		EXPECT_STRINGEQ( concatstr10, string_const( STRING_CONST( LONGSTRING LONGSTRING ) ) );

		concatstr = string_concat( buf, 4, STRING_ARGS( teststr3 ), STRING_ARGS( clonestr3 ) );
		EXPECT_STRINGEQ( concatstr, string_const( STRING_CONST( "lon" ) ) );

		concatstr = string_concat( buf, 0, STRING_ARGS( teststr3 ), STRING_ARGS( clonestr3 ) );
		EXPECT_EQ( buf[0], 'l' ); //Expect buf to be unchanged from previous test
		EXPECT_EQ( concatstr.length, 0 );
		EXPECT_STRINGEQ( concatstr, string_const( STRING_CONST( "" ) ) );

		#undef SHORTSTRING
		#undef LONGSTRING
	}
	return 0;
}


DECLARE_TEST( string, format )
{
	char buffer[256];
	{
		int64_t ival = -1;
		uint64_t uval = 0x123456789abULL;
		string_t teststr1 = string_allocate_format( STRING_CONST( "%" PRId64 ), ival );
		string_t teststr2 = string_allocate_format( STRING_CONST( "0x%" PRIx64 ), ival );
		string_t teststr3 = string_allocate_format( STRING_CONST( "%016" PRIX64 ), uval );

		EXPECT_STREQ( STRING_ARGS( teststr1 ), STRING_CONST( "-1" ) );
		EXPECT_STREQ( STRING_ARGS( teststr2 ), STRING_CONST( "0xffffffffffffffff" ) );
		EXPECT_STREQ( STRING_ARGS( teststr3 ), STRING_CONST( "00000123456789AB" ) );

		string_deallocate( teststr1.str );
		string_deallocate( teststr2.str );
		string_deallocate( teststr3.str );
	}
	{
		int64_t ival = -1;
		uint64_t uval = 0x123456789abULL;
		string_t teststr1;
		string_t teststr2;
		string_t teststr3;

		teststr1 = string_format( buffer, sizeof( buffer ), STRING_CONST( "%" PRId64 ), ival );
		EXPECT_STREQ( STRING_ARGS( teststr1 ), STRING_CONST( "-1" ) );

		teststr2 = string_format( buffer, sizeof( buffer ), STRING_CONST( "0x%" PRIx64 ), ival );
		EXPECT_STREQ( STRING_ARGS( teststr2 ), STRING_CONST( "0xffffffffffffffff" ) );

		teststr3 = string_format( buffer, 8, STRING_CONST( "%016" PRIX64 ), uval );
		EXPECT_STREQ( STRING_ARGS( teststr3 ), STRING_CONST( "0000012" ) );
	}
	{
		string_t teststr1 = string_allocate_format( STRING_CONST( "0x%" PRIfixPTR ), (uintptr_t)nullptr );
		string_t teststr2 = string_allocate_format( STRING_CONST( "0x%" PRIfixPTR ), (uintptr_t)((void*)-1) );
		string_t teststr3 = string_allocate_format( STRING_CONST( "0x%" PRIfixPTR ), (uintptr_t)((void*)0x1234abULL) );

#if FOUNDATION_SIZE_POINTER == 8
		EXPECT_STREQ( STRING_ARGS( teststr1 ), STRING_CONST( "0x0000000000000000" ) );
		EXPECT_STREQ( STRING_ARGS( teststr2 ), STRING_CONST( "0xFFFFFFFFFFFFFFFF" ) );
		EXPECT_STREQ( STRING_ARGS( teststr3 ), STRING_CONST( "0x00000000001234AB" ) );
#else
		EXPECT_STREQ( STRING_ARGS( teststr1 ), STRING_CONST( "0x00000000" ) );
		EXPECT_STREQ( STRING_ARGS( teststr2 ), STRING_CONST( "0xFFFFFFFF" ) );
		EXPECT_STREQ( STRING_ARGS( teststr3 ), STRING_CONST( "0x001234AB" ) );
#endif

		string_deallocate( teststr1.str );
		string_deallocate( teststr2.str );
		string_deallocate( teststr3.str );
	}
	{
		string_t teststr1 = string_format( buffer, sizeof( buffer ), STRING_CONST( "0x%" PRIfixPTR ), (uintptr_t)nullptr );
		string_t teststr2 = string_format( buffer, sizeof( buffer ), STRING_CONST( "0x%" PRIfixPTR ), (uintptr_t)((void*)-1) );
		string_t teststr3 = string_format( buffer, 8, STRING_CONST( "0x%" PRIfixPTR ), (uintptr_t)((void*)0x1234abULL) );

#if FOUNDATION_SIZE_POINTER == 8
		EXPECT_STREQ( STRING_ARGS( teststr1 ), STRING_CONST( "0x0000000000000000" ) );
		EXPECT_STREQ( STRING_ARGS( teststr2 ), STRING_CONST( "0xFFFFFFFFFFFFFFFF" ) );
		EXPECT_STREQ( STRING_ARGS( teststr3 ), STRING_CONST( "0x00000" ) );
#else
		EXPECT_STREQ( STRING_ARGS( teststr1 ), STRING_CONST( "0x00000000" ) );
		EXPECT_STREQ( STRING_ARGS( teststr2 ), STRING_CONST( "0xFFFFFFFF" ) );
		EXPECT_STREQ( STRING_ARGS( teststr3 ), STRING_CONST( "0x00123" ) );
#endif

		string_deallocate( teststr1.str );
		string_deallocate( teststr2.str );
		string_deallocate( teststr3.str );
	}
	return 0;
}


static void test_string_declare( void )
{
	ADD_TEST( string, initialize );
	ADD_TEST( string, queries );
	ADD_TEST( string, utility );
	ADD_TEST( string, append );
	ADD_TEST( string, prepend );
	ADD_TEST( string, format );
}


static test_suite_t test_string_suite = {
	test_string_application,
	test_string_memory_system,
	test_string_config,
	test_string_declare,
	test_string_initialize,
	test_string_shutdown
};


#if BUILD_MONOLITHIC

int test_string_run( void );
int test_string_run( void )
{
	test_suite = test_string_suite;
	return test_run_all();
}

#else

test_suite_t test_suite_define( void );
test_suite_t test_suite_define( void )
{
	return test_string_suite;
}

#endif
