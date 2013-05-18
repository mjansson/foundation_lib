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


application_t test_application( void )
{
	application_t app = {0};
	app.name = "Foundation string tests";
	app.short_name = "test_string";
	app.config_dir = "test_string";
	app.flags = APPLICATION_UTILITY;
	return app;
}


int test_initialize( void )
{
	return 0;
}


void test_shutdown( void )
{
}


DECLARE_TEST( string, initialize )
{
	{
		char* nullstr1 = string_allocate( 0 );
		char* nullstr2 = string_allocate( 1 );
		char* nullstr3 = string_allocate( 10 );

		EXPECT_EQ( 0U, string_length( nullstr1 ) );
		EXPECT_EQ( 0U, string_length( nullstr2 ) );
		EXPECT_EQ( 0U, string_length( nullstr3 ) );
		EXPECT_EQ( 0, strcmp( nullstr1, "" ) );
		EXPECT_EQ( 0, strcmp( nullstr2, "" ) );
		EXPECT_EQ( 0, strcmp( nullstr3, "" ) );	

		string_deallocate( nullstr1 );
		string_deallocate( nullstr2 );
		string_deallocate( nullstr3 );
	}
	{
		char teststr1[] = "test";
		char teststr2[] = "testing long string with more than 16 characters";
		char teststr3[] = "\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x09";

		char* str1 = string_clone( teststr1 );
		char* str2 = string_substr( teststr1, 0, 3 );
		char* str3 = string_substr( teststr1, 0, 4 );
		char* str4 = string_substr( teststr1, 0, 32 );
		char* str5 = string_substr( teststr1, 0, STRING_NPOS );

		char* str6 = string_clone( teststr2 );
		char* str7 = string_substr( teststr2, 0, 3 );
		char* str8 = string_substr( teststr2, 0, 20 );
		char* str9 = string_substr( teststr2, 0, STRING_NPOS );

		char* str10 = string_clone( teststr3 );
		char* str11 = string_substr( teststr3, 0, 3 );
		char* str12 = string_substr( teststr3, 0, 20 );
		char* str13 = string_substr( teststr3, 0, STRING_NPOS );

		EXPECT_EQ( 0, strcmp( str1, teststr1 ) );
		EXPECT_EQ( 0, strcmp( str2, "tes" ) );
		EXPECT_EQ( 0, strcmp( str3, teststr1 ) );
		EXPECT_EQ( 0, strcmp( str4, teststr1 ) );
		EXPECT_EQ( 0, strcmp( str5, teststr1 ) );

		EXPECT_EQ( 0, strcmp( str6, teststr2 ) );
		EXPECT_EQ( 0, strcmp( str7, "tes" ) );
		EXPECT_EQ( 0, strcmp( str8, "testing long string " ) );
		EXPECT_EQ( 0, strcmp( str9, teststr2 ) );

		EXPECT_EQ( 0, strcmp( str10, teststr3 ) );
		EXPECT_EQ( 0, strcmp( str11, "\x01\x02\x03" ) );
		EXPECT_EQ( 0, strcmp( str12, "\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02\x03\x04\x05\x06\x07\x08\x90\x01\x02" ) );
		EXPECT_EQ( 0, strcmp( str13, teststr3 ) );

		string_deallocate( str1 );
		string_deallocate( str2 );
		string_deallocate( str3 );
		string_deallocate( str4 );
		string_deallocate( str5 );
		string_deallocate( str6 );
		string_deallocate( str7 );
		string_deallocate( str8 );
		string_deallocate( str9 );
		string_deallocate( str10 );
		string_deallocate( str11 );
		string_deallocate( str12 );
		string_deallocate( str13 );
	}
	{
		wchar_t teststr1[] = L"test";
		wchar_t teststr2[] = L"testing long string with more than 16 characters";
		wchar_t teststr3[] = L"0\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009\x0030\x0001\x0002\x0003\x0004\x0005\x0006\x0007\x0008\x0009";
		wchar_t teststr4[] = L"Wide-char string with some strange characters: åäöÅÄÖ_________ (test utf-8 conversions with long strings)";
		//char utfteststr4[] = "Wide-char string with some strange characters: \xC3\xA5\xC3\xA4\xC3\xB6\xC3\x85\xC3\x84\xC3\x96\xEF\xB7\xB2\xDA\x81\xED\x80\x83\xEC\x99\xA7\xEB\x96\xAF\xE7\xBF\xB3\xE5\xA1\x8C\xE1\xA7\xBC\xE1\xA7\xAD (test utf-8 conversions with long strings)";

		char *str1, *str2, *str3, *str4, *str5, *str6, *str7, *str8, *str9, *str10, *str11, *str12, *str13, *str14, *str15, *str16, *str17;
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

		wstr1 = wstring_allocate_from_string( str1, 0 );
		wstr2 = wstring_allocate_from_string( str2, 0 );
		wstr3 = wstring_allocate_from_string( str3, 0 );
		wstr4 = wstring_allocate_from_string( str4, 0 );
		wstr5 = wstring_allocate_from_string( str5, 0 );

		wstr6 = wstring_allocate_from_string( str6, 0 );
		wstr7 = wstring_allocate_from_string( str7, 0 );
		wstr8 = wstring_allocate_from_string( str8, 0 );
		wstr9 = wstring_allocate_from_string( str9, 0 );

		wstr10 = wstring_allocate_from_string( str10, 0 );
		wstr11 = wstring_allocate_from_string( str11, 0 );
		wstr12 = wstring_allocate_from_string( str12, 0 );
		wstr13 = wstring_allocate_from_string( str13, 0 );

		wstr14 = wstring_allocate_from_string( str14, 0 );
		wstr15 = wstring_allocate_from_string( str15, 0 );
		wstr16 = wstring_allocate_from_string( str16, 0 );
		wstr17 = wstring_allocate_from_string( str17, 0 );

		EXPECT_EQ( 0, wcscmp( wstr1, teststr1 ) );
		EXPECT_EQ( 0, wcscmp( wstr2, L"tes" ) );
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
			char* utf8_teststr = string_allocate_from_wstring( wteststr, 0 );
			wchar_t* wchar_teststr = wstring_allocate_from_string( utf8_teststr, 0 );
			EXPECT_EQ( 0, wcscmp( wteststr, wchar_teststr ) );
			wstring_deallocate( wchar_teststr );
			string_deallocate( utf8_teststr );
		}

		{
			uint16_t wteststr[] = { 0xFEFF, 0x0100, 0x078f, 0x1234, 0xFF03, 0xDB02, 0xDC54, 0x0032, 0xFFFE, 0x1234, 0xFF03, 0 };
#if FOUNDATION_WCHAR_SIZE == 32
			wchar_t wtestcmpstr[] = { 0x0100, 0x078f, 0x1234, 0xFF03, 0x000D0854, 0x0032, 0x3412, 0x03FF, 0 };
#else
			wchar_t wtestcmpstr[] = { 0x0100, 0x078f, 0x1234, 0xFF03, 0xDB02, 0xDC54, 0x0032, 0x3412, 0x03FF, 0 };
#endif
			char* utf8_teststr = string_allocate_from_utf16( wteststr, 0 );
			wchar_t* wchar_teststr = wstring_allocate_from_string( utf8_teststr, 0 );
			EXPECT_EQ( 0, wcscmp( wtestcmpstr, wchar_teststr ) );
			wstring_deallocate( wchar_teststr );
			string_deallocate( utf8_teststr );
		}

		{
			uint32_t wteststr[] = { 0x0000FEFF, 0x00000100, 0x0000078f, 0x00001234, 0x0000FF03, 0x000D0854, 0x000D0C53, 0x00000032, 0xFFFE0000, 0x12340000, 0xFF030000, 0 };
#if FOUNDATION_WCHAR_SIZE == 32
			wchar_t wtestcmpstr[] = { 0x0100, 0x078f, 0x1234, 0xFF03, 0x000D0854, 0x000D0C53, 0x0032, 0x3412, 0x03FF, 0 };
#else
			wchar_t wtestcmpstr[] = { 0x0100, 0x078f, 0x1234, 0xFF03, 0xDB02, 0xDC54, 0xDB03, 0xDC53, 0x0032, 0x3412, 0x03FF, 0 };
#endif
			char* utf8_teststr = string_allocate_from_utf32( wteststr, 0 );
			wchar_t* wchar_teststr = wstring_allocate_from_string( utf8_teststr, 0 );
			EXPECT_EQ( 0, wcscmp( wtestcmpstr, wchar_teststr ) );
			wstring_deallocate( wchar_teststr );
			string_deallocate( utf8_teststr );
		}

		string_deallocate( str1 );
		string_deallocate( str2 );
		string_deallocate( str3 );
		string_deallocate( str4 );
		string_deallocate( str5 );
		string_deallocate( str6 );
		string_deallocate( str7 );
		string_deallocate( str8 );
		string_deallocate( str9 );
		string_deallocate( str10 );
		string_deallocate( str11 );
		string_deallocate( str12 );
		string_deallocate( str13 );
		string_deallocate( str14 );
		string_deallocate( str15 );
		string_deallocate( str16 );
		string_deallocate( str17 );

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

		char* str1 = string_substr( teststr, 0, 0 );
		char* str2 = string_substr( teststr2, 0, 4 );
		char* str3 = string_substr( teststr2, 0, 20 );
		char* str4 = string_substr( teststr3, 0, 0 );
		char* str5 = string_substr( teststr3, 0, 10 );
		char* str6 = string_substr( teststr3, 0, STRING_NPOS );
		char* str7 = string_substr( teststr4, 0, 0 );
		char* str8 = string_substr( teststr4, 0, 20 );
		char* str9 = string_substr( teststr4, 0, STRING_NPOS );

		EXPECT_EQ( 0, str1[0] );
		EXPECT_EQ( 0, str2[ string_length( str2 ) ] );
		EXPECT_EQ( 0, str3[ string_length( str3 ) ] );
		EXPECT_EQ( 0, str4[ string_length( str4 ) ] );
		EXPECT_EQ( 0, str5[ string_length( str5 ) ] );
		EXPECT_EQ( 0, str6[ string_length( str6 ) ] );
		EXPECT_EQ( 0, str7[ string_length( str7 ) ] );
		EXPECT_EQ( 0, str8[ string_length( str8 ) ] );
		EXPECT_EQ( 0, str9[ string_length( str9 ) ] );

		EXPECT_EQ( strlen( teststr ), string_length( str1 ) );
		EXPECT_EQ( strlen( teststr2 ), string_length( str2 ) );
		EXPECT_EQ( strlen( teststr2 ), string_length( str3 ) );
		EXPECT_EQ( 0U, string_length( str4 ) );
		EXPECT_EQ( 10U, string_length( str5 ) );
		EXPECT_EQ( strlen( teststr3 ), string_length( str6 ) );
		EXPECT_EQ( 0U, string_length( str7 ) );
		EXPECT_EQ( 20U, string_length( str8 ) );
		EXPECT_EQ( strlen( teststr4 ), string_length( str9 ) );

		string_deallocate( str1 );
		string_deallocate( str2 );
		string_deallocate( str3 );
		string_deallocate( str4 );
		string_deallocate( str5 );
		string_deallocate( str6 );
		string_deallocate( str7 );
		string_deallocate( str8 );
		string_deallocate( str9 );
	}
	{
		//Only ASCII characters, so wstring->string conversion should not introduce any extra UTF-8 sequences
		wchar_t teststr[]  = L"";
		wchar_t teststr2[] = L"test";
		wchar_t teststr3[] = L"testing long string with more than 16 characters";
		wchar_t teststr4[] = L"01234567890123456789012345678901234567890123456789012345678901234567890123456789";

		char* str1 = string_allocate_from_wstring( teststr, 0 );
		char* str2 = string_allocate_from_wstring( teststr2, 0 );
		char* str3 = string_allocate_from_wstring( teststr2, 20 );
		char* str4 = string_allocate_from_wstring( teststr3, 0 );
		char* str5 = string_allocate_from_wstring( teststr3, 10 );
		char* str6 = string_allocate_from_wstring( teststr3, STRING_NPOS );
		char* str7 = string_allocate_from_wstring( teststr4, 0 );
		char* str8 = string_allocate_from_wstring( teststr4, 20 );
		char* str9 = string_allocate_from_wstring( teststr4, STRING_NPOS );

		EXPECT_EQ( 0, str1[0] );
		EXPECT_EQ( 0, str2[ string_length( str2 ) ] );
		EXPECT_EQ( 0, str3[ string_length( str3 ) ] );
		EXPECT_EQ( 0, str4[ string_length( str4 ) ] );
		EXPECT_EQ( 0, str5[ string_length( str5 ) ] );
		EXPECT_EQ( 0, str6[ string_length( str6 ) ] );
		EXPECT_EQ( 0, str7[ string_length( str7 ) ] );
		EXPECT_EQ( 0, str8[ string_length( str8 ) ] );
		EXPECT_EQ( 0, str9[ string_length( str9 ) ] );

		EXPECT_EQ( wcslen( teststr ), string_length( str1 ) );
		EXPECT_EQ( wcslen( teststr2 ), string_length( str2 ) );
		EXPECT_EQ( wcslen( teststr2 ), string_length( str3 ) );
		EXPECT_EQ( wcslen( teststr3 ), string_length( str4 ) );
		EXPECT_EQ( 10U, string_length( str5 ) );
		EXPECT_EQ( wcslen( teststr3 ), string_length( str6 ) );
		EXPECT_EQ( wcslen( teststr4 ), string_length( str7 ) );
		EXPECT_EQ( 20U, string_length( str8 ) );
		EXPECT_EQ( wcslen( teststr4 ), string_length( str9 ) );

		string_deallocate( str1 );
		string_deallocate( str2 );
		string_deallocate( str3 );
		string_deallocate( str4 );
		string_deallocate( str5 );
		string_deallocate( str6 );
		string_deallocate( str7 );
		string_deallocate( str8 );
		string_deallocate( str9 );
	}
	{
		char* emptystr = string_allocate( 0 );
		char* shortstr = string_clone( "short string" );
		char* longstr  = string_clone( "testing utility string methods like finds, split, merge, explode and similar." );
		{
			unsigned int find     = string_find( longstr, ' ', 0 );
			unsigned int find2    = string_find( longstr, 12, 0 );
			unsigned int find3    = string_find( emptystr, ' ', 0 );
			unsigned int find4    = string_find( shortstr, ' ', 0 );
			unsigned int find5    = string_find( shortstr, 'z', 0 );
			unsigned int findofs  = string_find( longstr, ' ', find );
			unsigned int findofs2 = string_find( longstr, ' ', find + 1 );
			unsigned int findofs3 = string_find( longstr, 'z', 10 );
			unsigned int findofs4 = string_find( emptystr, 'z', STRING_NPOS );
			unsigned int findofs5 = string_find( shortstr, 's', 10 );
			unsigned int findofs6 = string_find( shortstr, 's', findofs5 );
			unsigned int findofs7 = string_find( shortstr, 't', 0 );
			unsigned int findofs8 = string_find( shortstr, ' ', 5 );
			unsigned int findofs9 = string_find( longstr, ' ', STRING_NPOS );

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
			unsigned int rfind     = string_rfind( longstr, ' ', STRING_NPOS );
			unsigned int rfind2    = string_rfind( longstr, ';', STRING_NPOS );
			unsigned int rfind3    = string_rfind( emptystr, ';', STRING_NPOS );
			unsigned int rfind4    = string_rfind( shortstr, 's', STRING_NPOS );
			unsigned int rfind5    = string_rfind( shortstr, 'z', STRING_NPOS );
			unsigned int rfindofs  = string_rfind( longstr, ' ', rfind );
			unsigned int rfindofs2 = string_rfind( longstr, ' ', rfind - 1 );
			unsigned int rfindofs3 = string_rfind( longstr, ' ', string_length( longstr ) - 1 );
			unsigned int rfindofs4 = string_rfind( emptystr, ' ', 0 );
			unsigned int rfindofs5 = string_rfind( longstr, ' ', 0 );
			unsigned int rfindofs6 = string_rfind( shortstr, 's', 5 );
			unsigned int rfindofs7 = string_rfind( shortstr, 's', 0 );
		
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
			unsigned int findstr     = string_find_string( longstr, "st", 0 );
			unsigned int findstr2    = string_find_string( longstr, "xwqz", 0 );
			unsigned int findstr3    = string_find_string( emptystr, "xwqz", 0 );
			unsigned int findstr4    = string_find_string( longstr, "", 0 );
			unsigned int findstr5    = string_find_string( longstr, "dslike", 0 );
			unsigned int findstr6    = string_find_string( shortstr, "rt", 0 );
			unsigned int findstr7    = string_find_string( shortstr, "long key that does not exist", 0 );
			unsigned int findstr8    = string_find_string( shortstr, "so", 0 );
			unsigned int findstrofs   = string_find_string( longstr, "st", findstr );
			unsigned int findstrofs2  = string_find_string( longstr, "st", findstr + 1 );
			unsigned int findstrofs3  = string_find_string( longstr, "xwqz", string_length( longstr ) );
			unsigned int findstrofs4  = string_find_string( emptystr, "xwqz", string_length( emptystr ) );
			unsigned int findstrofs5  = string_find_string( shortstr, "", 5 );
			unsigned int findstrofs6  = string_find_string( shortstr, "string", 0 );
			unsigned int findstrofs7  = string_find_string( shortstr, "string", 7 );
			unsigned int findstrofs8  = string_find_string( longstr, "utility", 14 );
			unsigned int findstrofs9  = string_find_string( longstr, "", string_length( longstr ) );
			unsigned int findstrofs10 = string_find_string( longstr, "", STRING_NPOS );
			unsigned int findstrofs11 = string_find_string( longstr, "string", STRING_NPOS );
		
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
			EXPECT_EQ( findstrofs9, strlen( longstr ) );
			EXPECT_EQ( findstrofs10, STRING_NPOS );
			EXPECT_EQ( findstrofs11, STRING_NPOS );
		}
		{
			unsigned int rfindstr     = string_rfind_string( longstr, ", ", STRING_NPOS );
			unsigned int rfindstr2    = string_rfind_string( longstr, ":;", STRING_NPOS );
			unsigned int rfindstr3    = string_rfind_string( emptystr, ":;", STRING_NPOS );
			unsigned int rfindstr4    = string_rfind_string( longstr, "", STRING_NPOS );
			unsigned int rfindstr5    = string_rfind_string( shortstr, "string", STRING_NPOS );
			unsigned int rfindstr6    = string_rfind_string( shortstr, " tring", STRING_NPOS );
			unsigned int rfindstrofs  = string_rfind_string( longstr, ", ", rfindstr );
			unsigned int rfindstrofs2 = string_rfind_string( longstr, ", ", rfindstr - 1 );
			unsigned int rfindstrofs3 = string_rfind_string( longstr, ":;", 0 );
			unsigned int rfindstrofs4 = string_rfind_string( emptystr, ":;", 0 );
			unsigned int rfindstrofs5 = string_rfind_string( longstr, "", 5 );
			unsigned int rfindstrofs6 = string_rfind_string( shortstr, "ort str", 6 );
			unsigned int rfindstrofs7 = string_rfind_string( shortstr, "ort str", 1 );

			EXPECT_EQ( rfindstr, 55U );
			EXPECT_EQ( rfindstr2, STRING_NPOS );
			EXPECT_EQ( rfindstr3, STRING_NPOS );
			EXPECT_EQ( rfindstr4, strlen( longstr ) );
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
			unsigned int findof        = string_find_first_of( longstr, "ui", 0 );
			unsigned int findof2       = string_find_first_of( longstr, ";:", 0 );
			unsigned int findof3       = string_find_first_of( emptystr, "", 0 );
			unsigned int findof4       = string_find_first_of( emptystr, " ", 0 );
			unsigned int findof5       = string_find_first_of( shortstr, "", 0 );
			unsigned int findofofs     = string_find_first_of( longstr, "ui", findof );
			unsigned int findofofs2    = string_find_first_of( longstr, "ui", findof - 1 );
			unsigned int findofofs3    = string_find_first_of( longstr, "ui", findof + 1 );
			unsigned int findofofs4    = string_find_first_of( longstr, "ui", string_length( longstr ) );
			unsigned int findofofs5    = string_find_first_of( emptystr, "", string_length( emptystr ) );
			unsigned int findofofs6    = string_find_first_of( shortstr, "string", 6 );
			unsigned int findofofs7    = string_find_first_of( shortstr, "", 6 );
			unsigned int findofofs8    = string_find_first_of( longstr, "", 10 );
			unsigned int findofofs9    = string_find_first_of( longstr, "", string_length( longstr ) );

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
			unsigned int findnotof     = string_find_first_not_of( longstr, "testing ", 0 );
			unsigned int findnotof2    = string_find_first_not_of( longstr, longstr, 0 );
			unsigned int findnotof3    = string_find_first_not_of( shortstr, "", 0 );
			unsigned int findnotofofs  = string_find_first_not_of( longstr, "testing ", findnotof );
			unsigned int findnotofofs2 = string_find_first_not_of( longstr, "testing ", findnotof + 1 );
			unsigned int findnotofofs3 = string_find_first_not_of( longstr, "testing ", string_length( longstr ) );
			unsigned int findnotofofs4 = string_find_first_not_of( shortstr, "", string_length( shortstr ) );

			EXPECT_EQ( findnotof, 8U );
			EXPECT_EQ( findnotof2, STRING_NPOS );
			EXPECT_EQ( findnotof3, 0U );
			EXPECT_EQ( findnotofofs, 8U );
			EXPECT_EQ( findnotofofs2, 11U );
			EXPECT_EQ( findnotofofs3, STRING_NPOS );
			EXPECT_EQ( findnotofofs4, STRING_NPOS );
		}
		{
			unsigned int findlastof        = string_find_last_of( longstr, "xp", STRING_NPOS );
			unsigned int findlastof2       = string_find_last_of( longstr, ";:", STRING_NPOS );
			unsigned int findlastof3       = string_find_last_of( emptystr, "", STRING_NPOS );
			unsigned int findlastof4       = string_find_last_of( shortstr, "", STRING_NPOS );
			unsigned int findlastofofs     = string_find_last_of( longstr, "xp", findlastof );
			unsigned int findlastofofs2    = string_find_last_of( longstr, "xp", findlastof - 2 );
			unsigned int findlastofofs3    = string_find_last_of( longstr, "xp", 0 );
			unsigned int findlastofofs4    = string_find_last_of( emptystr, "", 0 );
			unsigned int findlastofofs5    = string_find_last_of( shortstr, "", 5 );
			unsigned int findlastofofs6    = string_find_last_of( shortstr, "", string_length( shortstr ) );
			unsigned int findlastofofs7    = string_find_last_of( shortstr, "short", 5 );

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
			unsigned int findlastnotof     = string_find_last_not_of( longstr, " similar.", STRING_NPOS );
			unsigned int findlastnotof2    = string_find_last_not_of( longstr, longstr, STRING_NPOS );
			unsigned int findlastnotof3    = string_find_last_not_of( emptystr, "", STRING_NPOS );
			unsigned int findlastnotof4    = string_find_last_not_of( shortstr, "", STRING_NPOS );
			unsigned int findlastnotof5    = string_find_last_not_of( longstr, " similar", STRING_NPOS );
			unsigned int findlastnotofofs  = string_find_last_not_of( longstr, " similar.", findlastnotof );
			unsigned int findlastnotofofs2 = string_find_last_not_of( longstr, " and similar.", findlastnotof - 1 );
			unsigned int findlastnotofofs3 = string_find_last_not_of( longstr, longstr, 0 );
			unsigned int findlastnotofofs4 = string_find_last_not_of( emptystr, "", 0 );
			unsigned int findlastnotofofs5 = string_find_last_not_of( shortstr, "string", 5 );
			unsigned int findlastnotofofs6 = string_find_last_not_of( shortstr, "string ", 5 );
			unsigned int findlastnotofofs7 = string_find_last_not_of( shortstr, "", 5 );
			unsigned int findlastnotofofs8 = string_find_last_not_of( longstr, "", string_length( longstr ) );

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
			EXPECT_EQ( findlastnotofofs8, strlen( longstr ) - 1 );
		}
		string_deallocate( emptystr );
		string_deallocate( shortstr );
		string_deallocate( longstr );
	}
	return 0;
}


DECLARE_TEST( string, utility )
{
	{
		char* path1 = string_clone( "" );
		char* path2 = string_clone( "/" );
		char* path3 = string_clone( "/." );
		char* path4 = string_clone( "./" );
		char* path5 = string_clone( "./." );
		char* path6 = string_clone( "././" );
		char* path7 = string_clone( "././//" );
		char* path8 = string_clone( "././//./////././////.//////.//." );
		char* path9 = string_clone( "http://././//./////././////.//////.//." );
		char* path10 = string_clone( "" );
		char* path11 = string_clone( "\\" );
		char* path12 = string_clone( "/\\." );
		char* path13 = string_clone( ".\\/" );
		char* path14 = string_clone( "./\\." );
		char* path15 = string_clone( ".\\.//\\" );
		char* path16 = string_clone( ".\\.\\\\\\" );
		char* path17 = string_clone( ".\\.\\\\\\.\\\\////\\///\\\\.\\.\\\\\\\\\\.\\\\\\\\\\\\.\\\\." );
		char* path18 = string_clone( "http://\\.\\.\\\\\\.\\\\\\\\//\\.\\.\\\\\\\\//\\.\\\\\\\\\\\\.\\\\." );
		
		char* path19 = string_clone( "testing/path/ext" );
		char* path20 = string_clone( "testing/path/extend" );
		char* path21 = string_clone( "testing/path/extend/dyn" );
		char* path22 = string_clone( "testing/./\\\\/\\/./path/././//./extend/\\\\" );

		path1 = path_clean( path1, true );
		path2 = path_clean( path2, true );
		path3 = path_clean( path3, true );
		path4 = path_clean( path4, true );
		path5 = path_clean( path5, true );
		path6 = path_clean( path6, true );
		path7 = path_clean( path7, true );
		path8 = path_clean( path8, true );
		path9 = path_clean( path9, true );
		path10 = path_clean( path10, true );
		path11 = path_clean( path11, true );
		path12 = path_clean( path12, true );
		path13 = path_clean( path13, true );
		path14 = path_clean( path14, true );
		path15 = path_clean( path15, true );
		path16 = path_clean( path16, true );
		path17 = path_clean( path17, true );
		path18 = path_clean( path18, true );
		path19 = path_clean( path19, true );
		path20 = path_clean( path20, true );
		path21 = path_clean( path21, true );
		path22 = path_clean( path22, true );

		EXPECT_TRUE( string_equal( path1, "/" ) );
		EXPECT_TRUE( string_equal( path2, "/" ) );
		EXPECT_TRUE( string_equal( path3, "/" ) );
		EXPECT_TRUE( string_equal( path4, "/" ) );
		EXPECT_TRUE( string_equal( path5, "/" ) );
		EXPECT_TRUE( string_equal( path6, "/" ) );
		EXPECT_TRUE( string_equal( path7, "/" ) );
		EXPECT_TRUE( string_equal( path8, "/" ) );
		EXPECT_TRUE( string_equal( path9, "http://" ) );
		EXPECT_TRUE( string_equal( path10, "/" ) );
		EXPECT_TRUE( string_equal( path11, "/" ) );
		EXPECT_TRUE( string_equal( path12, "/" ) );
		EXPECT_TRUE( string_equal( path13, "/" ) );
		EXPECT_TRUE( string_equal( path14, "/" ) );
		EXPECT_TRUE( string_equal( path15, "/" ) );
		EXPECT_TRUE( string_equal( path16, "/" ) );
		EXPECT_TRUE( string_equal( path17, "/" ) );
		EXPECT_TRUE( string_equal( path18, "http://" ) );
		EXPECT_TRUE( string_equal( path19, "/testing/path/ext" ) );
		EXPECT_TRUE( string_equal( path20, "/testing/path/extend" ) );
		EXPECT_TRUE( string_equal( path21, "/testing/path/extend/dyn" ) );
		EXPECT_TRUE( string_equal( path22, "/testing/path/extend" ) );

		string_deallocate( path1 );
		string_deallocate( path2 );
		string_deallocate( path3 );
		string_deallocate( path4 );
		string_deallocate( path5 );
		string_deallocate( path6 );
		string_deallocate( path7 );
		string_deallocate( path8 );
		string_deallocate( path9 );
		string_deallocate( path10 );
		string_deallocate( path11 );
		string_deallocate( path12 );
		string_deallocate( path13 );
		string_deallocate( path14 );
		string_deallocate( path15 );
		string_deallocate( path16 );
		string_deallocate( path17 );
		string_deallocate( path18 );
		string_deallocate( path19 );
		string_deallocate( path20 );
		string_deallocate( path21 );
		string_deallocate( path22 );
	}
	{
		char** explodearr = 0;
		char* explodestr = string_clone( "  .,testing,    .,utility.,string  methods ..., like,,,finds  split..merge     .,.explode.and. .., ., similar   .,,,. " );
		
		char* mergestr = string_clone( "    testing   merge string   " );
		char* mergestr2 = string_clone( " ., testing, .merge.string,. " );
		char* merged = 0;
		char** mergearr, **mergearr2, **mergearr3;

		char* splitstr = string_clone( " testing split" );
		char* splitright = 0, *splitleft = 0;
		char* splitright2 = 0, *splitleft2 = 0;

		char* substrtest = string_clone( "testing substr" );
		char* substr = 0;
		
		explodearr = string_explode( explodestr, " ,.", false );
		mergearr = string_explode( mergestr, " .,", true );
		mergearr2 = string_explode( mergestr, " .,", false );
		mergearr3 = string_explode( mergestr, " .,", true );
		merged = string_merge( (const char* const*)mergearr, array_size( mergearr ), " " );

		string_split( splitstr, " ", &splitleft, &splitright, false );
		string_split( splitstr, " ", &splitleft2, &splitright2, true );
		
		EXPECT_EQ( array_size( explodearr ), 11 );
		EXPECT_TRUE( string_equal( explodearr[0], "testing" ) );
		EXPECT_TRUE( string_equal( explodearr[1], "utility" ) );
		EXPECT_TRUE( string_equal( explodearr[2], "string" ) );
		EXPECT_TRUE( string_equal( explodearr[3], "methods" ) );
		EXPECT_TRUE( string_equal( explodearr[4], "like" ) );
		EXPECT_TRUE( string_equal( explodearr[5], "finds" ) );
		EXPECT_TRUE( string_equal( explodearr[6], "split" ) );
		EXPECT_TRUE( string_equal( explodearr[7], "merge" ) );
		EXPECT_TRUE( string_equal( explodearr[8], "explode" ) );
		EXPECT_TRUE( string_equal( explodearr[9], "and" ) );
		EXPECT_TRUE( string_equal( explodearr[10], "similar" ) );

		EXPECT_EQ( array_size( mergearr ), 12 );
		EXPECT_TRUE( string_equal( mergearr[0], "" ) );
		EXPECT_TRUE( string_equal( mergearr[1], "" ) );
		EXPECT_TRUE( string_equal( mergearr[2], "" ) );
		EXPECT_TRUE( string_equal( mergearr[3], "" ) );
		EXPECT_TRUE( string_equal( mergearr[4], "testing" ) );
		EXPECT_TRUE( string_equal( mergearr[5], "" ) );
		EXPECT_TRUE( string_equal( mergearr[6], "" ) );
		EXPECT_TRUE( string_equal( mergearr[7], "merge" ) );
		EXPECT_TRUE( string_equal( mergearr[8], "string" ) );
		EXPECT_TRUE( string_equal( mergearr[9], "" ) );
		EXPECT_TRUE( string_equal( mergearr[10], "" ) );
		EXPECT_TRUE( string_equal( mergearr[11], "" ) );

		EXPECT_EQ( array_size( mergearr2 ), 3 );
		EXPECT_TRUE( string_equal( mergearr2[0], "testing" ) );
		EXPECT_TRUE( string_equal( mergearr2[1], "merge" ) );
		EXPECT_TRUE( string_equal( mergearr2[2], "string" ) );
		EXPECT_TRUE( string_equal( merged, mergestr ) );

		EXPECT_EQ( array_size( mergearr3 ), 12 );
		EXPECT_TRUE( string_equal( mergearr3[0], "" ) );
		EXPECT_TRUE( string_equal( mergearr3[1], "" ) );
		EXPECT_TRUE( string_equal( mergearr3[2], "" ) );
		EXPECT_TRUE( string_equal( mergearr3[3], "" ) );
		EXPECT_TRUE( string_equal( mergearr3[4], "testing" ) );
		EXPECT_TRUE( string_equal( mergearr3[5], "" ) );
		EXPECT_TRUE( string_equal( mergearr3[6], "" ) );
		EXPECT_TRUE( string_equal( mergearr3[7], "merge" ) );
		EXPECT_TRUE( string_equal( mergearr3[8], "string" ) );
		EXPECT_TRUE( string_equal( mergearr3[9], "" ) );
		EXPECT_TRUE( string_equal( mergearr3[10], "" ) );
		EXPECT_TRUE( string_equal( mergearr3[11], "" ) );

		EXPECT_TRUE( string_equal( substr = string_substr( substrtest, 0, 4 ), "test" ) ); string_deallocate( substr );
		EXPECT_TRUE( string_equal( substr = string_substr( substrtest, 0, 14 ), "testing substr" ) ); string_deallocate( substr );
		EXPECT_TRUE( string_equal( substr = string_substr( substrtest, 0, 20 ), "testing substr" ) ); string_deallocate( substr );
		EXPECT_TRUE( string_equal( substr = string_substr( substrtest, 3, 20 ), "ting substr" ) ); string_deallocate( substr );
		EXPECT_TRUE( string_equal( substr = string_substr( substrtest, 3, 11 ), "ting substr" ) ); string_deallocate( substr );
		EXPECT_TRUE( string_equal( substr = string_substr( substrtest, 3, 1 ), "t" ) ); string_deallocate( substr );
		EXPECT_TRUE( string_equal( substr = string_substr( substrtest, 3, 0 ), "" ) ); string_deallocate( substr );
		EXPECT_TRUE( string_equal( substr = string_substr( substrtest, 20, 0 ), "" ) ); string_deallocate( substr );
		EXPECT_TRUE( string_equal( substr = string_substr( substrtest, 20, 20 ), "" ) ); string_deallocate( substr );

		EXPECT_TRUE( string_equal( splitleft, "testing" ) );
		EXPECT_TRUE( string_equal( splitright, "split" ) );
		EXPECT_TRUE( string_equal( splitleft2, "" ) );
		EXPECT_TRUE( string_equal( splitright2, "testing split" ) );
		{
			char* replacestr = string_clone( "testing replace" );
			char* replacestr2 = string_clone( "testing replace" );
			char* replacestr3 = string_clone( "testing replacelace" );
			char* replacestr4 = string_clone( "" );
			char* replacestr5 = string_clone( "repppppppppp" );

			replacestr = string_replace( replacestr, "rep", "testrep", false );
			replacestr2 = string_replace( replacestr2, "rep", "testrep", true );
			replacestr3 = string_replace( replacestr3, "replace", "testrep", true );
			replacestr4 = string_replace( replacestr4, "foo", "bar", true );
			replacestr5 = string_replace( replacestr5, "rep", "re", true );
		
			EXPECT_TRUE( string_equal( replacestr, "testing testreplace" ) );
			EXPECT_TRUE( string_equal( replacestr2, "testing testreplace" ) );
			EXPECT_TRUE( string_equal( replacestr3, "testing testtestrep" ) );
			EXPECT_TRUE( string_equal( replacestr4, "" ) );
			EXPECT_TRUE( string_equal( replacestr5, "re" ) );

			string_deallocate( replacestr );
			string_deallocate( replacestr2 );
			string_deallocate( replacestr3 );
			string_deallocate( replacestr4 );
			string_deallocate( replacestr5 );
		}
		{
			char* stripstr = string_clone( "   testing strip :   " );
			char* stripstr2 = string_clone( "   testing strip :   " );
			char* stripstr3 = string_clone( "   testing strip :   " );

			stripstr = string_strip( stripstr, " tp:   " );
			stripstr2 = string_strip( stripstr2, "" );
			stripstr3 = string_strip( stripstr3, " tesingrp:" );
		
			EXPECT_TRUE( string_equal( stripstr, "esting stri" ) );
			EXPECT_TRUE( string_equal( stripstr2, "   testing strip :   " ) );
			EXPECT_TRUE( string_equal( stripstr3, "" ) );

			string_deallocate( stripstr );
			string_deallocate( stripstr2 );
			string_deallocate( stripstr3 );
		}
		string_array_deallocate( explodearr );
		string_deallocate( explodestr );
		
		string_deallocate( mergestr );
		string_deallocate( mergestr2 );
		string_deallocate( merged );
		string_array_deallocate( mergearr );
		string_array_deallocate( mergearr2 );
		string_array_deallocate( mergearr3 );

		string_deallocate( splitstr );
		string_deallocate( splitright );
		string_deallocate( splitleft );
		string_deallocate( splitright2 );
		string_deallocate( splitleft2 );

		string_deallocate( substrtest );
	}
	{
		#define SHORTSTRING "short"
		#define LONGSTRING  "long string with dynamic buffer storage but with no real useful data"
		char* clonestr = string_clone( "" );
		char* clonestr2 = string_clone( SHORTSTRING );
		char* clonestr3 = string_clone( LONGSTRING );

		char* teststr = string_clone( clonestr );
		char* teststr2 = string_clone( clonestr2 );
		char* teststr3 = string_clone( clonestr3 );

		char* concatstr = string_concat( clonestr, teststr );
		char* concatstr2 = string_concat( clonestr, teststr2 );
		char* concatstr3 = string_concat( teststr2, clonestr );
		char* concatstr4 = string_concat( clonestr2, teststr2 );
		char* concatstr5 = string_concat( clonestr, teststr3 );
		char* concatstr6 = string_concat( teststr3, clonestr );
		char* concatstr7 = string_concat( clonestr2, teststr3 );
		char* concatstr8 = string_concat( teststr3, clonestr2 );
		char* concatstr9 = string_concat( clonestr3, teststr3 );
		char* concatstr10 = string_concat( teststr3, clonestr3 );
		
		EXPECT_NE( teststr, clonestr );
		EXPECT_TRUE( string_equal( teststr, clonestr ) );
		
		EXPECT_NE( teststr2, clonestr2 );
		EXPECT_TRUE( string_equal( teststr2, clonestr2 ) );

		EXPECT_NE( teststr3, clonestr3 );
		EXPECT_TRUE( string_equal( teststr3, clonestr3 ) );

		EXPECT_TRUE( string_equal( concatstr, "" ) );
		EXPECT_TRUE( string_equal( concatstr2, SHORTSTRING ) );
		EXPECT_TRUE( string_equal( concatstr3, SHORTSTRING ) );
		EXPECT_TRUE( string_equal( concatstr4, SHORTSTRING SHORTSTRING ) );
		EXPECT_TRUE( string_equal( concatstr5, LONGSTRING ) );
		EXPECT_TRUE( string_equal( concatstr6, LONGSTRING ) );
		EXPECT_TRUE( string_equal( concatstr7, SHORTSTRING LONGSTRING ) );
		EXPECT_TRUE( string_equal( concatstr8, LONGSTRING SHORTSTRING ) );
		EXPECT_TRUE( string_equal( concatstr9, LONGSTRING LONGSTRING ) );
		EXPECT_TRUE( string_equal( concatstr10, LONGSTRING LONGSTRING ) );
		
		string_deallocate( teststr );
		string_deallocate( clonestr );
		string_deallocate( teststr2 );
		string_deallocate( clonestr2 );
		string_deallocate( teststr3 );
		string_deallocate( clonestr3 );
		string_deallocate( concatstr );
		string_deallocate( concatstr2 );
		string_deallocate( concatstr3 );
		string_deallocate( concatstr4 );
		string_deallocate( concatstr5 );
		string_deallocate( concatstr6 );
		string_deallocate( concatstr7 );
		string_deallocate( concatstr8 );
		string_deallocate( concatstr9 );
		string_deallocate( concatstr10 );
		#undef SHORTSTRING
		#undef LONGSTRING
	}
	return 0;
}


void test_declare( void )
{
	ADD_TEST( string, initialize );
	ADD_TEST( string, queries );
	ADD_TEST( string, utility );
}
