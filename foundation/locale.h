/* locale.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#pragma once

/*! \file locale.h
\brief Locale definitions
\details Locale definitions */

#include <foundation/platform.h>

/*! \brief Define locale
Define a locale with a four character code, two lowercase language characters and two
uppercase country characters, for example LOCALE_DEF('e','n','U','S')
\param x First language character
\param y Second language character
\param z First country character
\param w Second country character */
#define LOCALE_DEF(x,y,z,w)          ((uint32_t)x|((uint32_t)y<<8)|((uint32_t)z<<16)|((uint32_t)w<<24))

/*! \brief Define language
Define a language with a two character lowercase code, for example LANGUAGE_DEF('e','n')
\param x First character
\param y Second character */
#define LANGUAGE_DEF(x,y)            ((uint16_t)x|((uint16_t)y<<8))

/*! \brief Define country
Define a country with a two character uppercase code, for example COUNTRY_DEF('U','S')
\param x First character
\param y Second character */
#define COUNTRY_DEF(x,y)             ((uint16_t)x|((uint16_t)y<<8))

/*! \brief Define locale
Define a locale from a compound language and country code, for example
LOCALE_FROM_LANGUAGE_COUNTRY(LANGUAGE_ENGLISH, COUNTRY_UNITED_STATES)
\param lang Language code
\param country Country code */
#define LOCALE_FROM_LANGUAGE_COUNTRY(lang,country)  ((uint32_t)(lang)|((uint32_t)(country)<<16))

/*! \brief Blank locale */
#define LOCALE_BLANK                 LOCALE_DEF(0,0,0,0)

/*! \brief Wildcard language */
#define LANGUAGE_WILDCARD            LANGUAGE_DEF('*','*')

/*! \brief Wildcard country */
#define COUNTRY_WILDCARD             COUNTRY_DEF('*','*')

/*! \brief Wildcard locale */
#define LOCALE_WILDCARD              LOCALE_DEF('*','*','*','*')

/*! \brief Unknown language */
#define LANGUAGE_UNKNOWN             LANGUAGE_DEF('z','z')

/*! \brief Unknown country */
#define COUNTRY_UNKNOWN              COUNTRY_DEF('Z','Z')

/*! \brief Unknown locale */
#define LOCALE_UNKNOWN               LOCALE_DEF('z','z','Z','Z')

// Predefined language identifiers
#define LANGUAGE_AFAN_OROMO          LANGUAGE_DEF('o','m')
#define LANGUAGE_ABKHAZIAN           LANGUAGE_DEF('a','b')
#define LANGUAGE_AFAR                LANGUAGE_DEF('a','a')
#define LANGUAGE_AFRIKAANS           LANGUAGE_DEF('a','f')
#define LANGUAGE_ALBANIAN            LANGUAGE_DEF('s','q')
#define LANGUAGE_AMHARIC             LANGUAGE_DEF('a','m')
#define LANGUAGE_ARABIC              LANGUAGE_DEF('a','r')
#define LANGUAGE_ARMENIAN            LANGUAGE_DEF('h','y')
#define LANGUAGE_ASSAMESE            LANGUAGE_DEF('a','s')
#define LANGUAGE_AYMARA              LANGUAGE_DEF('a','y')
#define LANGUAGE_AZERBAIJANI         LANGUAGE_DEF('a','z')
#define LANGUAGE_BASHKIR             LANGUAGE_DEF('b','a')
#define LANGUAGE_BASQUE              LANGUAGE_DEF('e','u')
#define LANGUAGE_BENGALI             LANGUAGE_DEF('b','n')
#define LANGUAGE_BHUTANI             LANGUAGE_DEF('d','z')
#define LANGUAGE_BIHARI              LANGUAGE_DEF('b','h')
#define LANGUAGE_BISLAMA             LANGUAGE_DEF('b','i')
#define LANGUAGE_BOSNIAN             LANGUAGE_DEF('b','s')
#define LANGUAGE_BRETON              LANGUAGE_DEF('b','r')
#define LANGUAGE_BULGARIAN           LANGUAGE_DEF('b','g')
#define LANGUAGE_BURMESE             LANGUAGE_DEF('m','y')
#define LANGUAGE_BYELORUSSIAN        LANGUAGE_DEF('b','e')
#define LANGUAGE_CAMBODIAN           LANGUAGE_DEF('k','m')
#define LANGUAGE_CATALAN             LANGUAGE_DEF('c','a')
#define LANGUAGE_CHINESE             LANGUAGE_DEF('z','h')
#define LANGUAGE_CORSICAN            LANGUAGE_DEF('c','o')
#define LANGUAGE_CROATIAN            LANGUAGE_DEF('h','r')
#define LANGUAGE_CZECH               LANGUAGE_DEF('c','s')
#define LANGUAGE_DANISH              LANGUAGE_DEF('d','a')
#define LANGUAGE_DIVEHI              LANGUAGE_DEF('d','v')
#define LANGUAGE_DUTCH               LANGUAGE_DEF('n','l')
#define LANGUAGE_ENGLISH             LANGUAGE_DEF('e','n')
#define LANGUAGE_ESPERANTO           LANGUAGE_DEF('e','o')
#define LANGUAGE_ESTONIAN            LANGUAGE_DEF('e','t')
#define LANGUAGE_FAEROESE            LANGUAGE_DEF('f','o')
#define LANGUAGE_FIJI                LANGUAGE_DEF('f','j')
#define LANGUAGE_FINNISH             LANGUAGE_DEF('f','i')
#define LANGUAGE_FRENCH              LANGUAGE_DEF('f','r')
#define LANGUAGE_FRISIAN             LANGUAGE_DEF('f','y')
#define LANGUAGE_GALICIAN            LANGUAGE_DEF('g','l')
#define LANGUAGE_GEORGIAN            LANGUAGE_DEF('k','a')
#define LANGUAGE_GERMAN              LANGUAGE_DEF('d','e')
#define LANGUAGE_GREEK               LANGUAGE_DEF('e','l')
#define LANGUAGE_GREENLANDIC         LANGUAGE_DEF('k','l')
#define LANGUAGE_GUARANI             LANGUAGE_DEF('g','n')
#define LANGUAGE_GUJARATI            LANGUAGE_DEF('g','u')
#define LANGUAGE_HAUSA               LANGUAGE_DEF('h','a')
#define LANGUAGE_HEBREW              LANGUAGE_DEF('h','e')
#define LANGUAGE_HINDI               LANGUAGE_DEF('h','i')
#define LANGUAGE_HUNGARIAN           LANGUAGE_DEF('h','u')
#define LANGUAGE_ICELANDIC           LANGUAGE_DEF('i','s')
#define LANGUAGE_IGBO                LANGUAGE_DEF('i','g')
#define LANGUAGE_INDONESIAN          LANGUAGE_DEF('i','d')
#define LANGUAGE_INTERLINGUA         LANGUAGE_DEF('i','a')
#define LANGUAGE_INTERLINGUE         LANGUAGE_DEF('i','e')
#define LANGUAGE_INUPIAK             LANGUAGE_DEF('i','k')
#define LANGUAGE_INUKTITUT           LANGUAGE_DEF('i','u')
#define LANGUAGE_IRISH               LANGUAGE_DEF('g','a')
#define LANGUAGE_ITALIAN             LANGUAGE_DEF('i','t')
#define LANGUAGE_JAPANESE            LANGUAGE_DEF('j','a')
#define LANGUAGE_JAVANESE            LANGUAGE_DEF('j','w')
#define LANGUAGE_KANNADA             LANGUAGE_DEF('k','n')
#define LANGUAGE_KASHMIRI            LANGUAGE_DEF('k','s')
#define LANGUAGE_KAZAKH              LANGUAGE_DEF('k','k')
#define LANGUAGE_KINYARWANDA         LANGUAGE_DEF('r','w')
#define LANGUAGE_KIRGHIZ             LANGUAGE_DEF('k','y')
#define LANGUAGE_KIRUNDI             LANGUAGE_DEF('r','n')
#define LANGUAGE_KOREAN              LANGUAGE_DEF('k','o')
#define LANGUAGE_KURDISH             LANGUAGE_DEF('k','u')
#define LANGUAGE_LAOTHIAN            LANGUAGE_DEF('l','o')
#define LANGUAGE_LATIN               LANGUAGE_DEF('l','a')
#define LANGUAGE_LATVIAN_LETTISH     LANGUAGE_DEF('l','v')
#define LANGUAGE_LINGALA             LANGUAGE_DEF('l','n')
#define LANGUAGE_LITHUANIAN          LANGUAGE_DEF('l','t')
#define LANGUAGE_LUXEMBOURGISH       LANGUAGE_DEF('l','b')
#define LANGUAGE_MACEDONIAN          LANGUAGE_DEF('m','k')
#define LANGUAGE_MALAGASY            LANGUAGE_DEF('m','g')
#define LANGUAGE_MALAY               LANGUAGE_DEF('m','s')
#define LANGUAGE_MALAYALAM           LANGUAGE_DEF('m','l')
#define LANGUAGE_MALTESE             LANGUAGE_DEF('m','t')
#define LANGUAGE_MAORI               LANGUAGE_DEF('m','i')
#define LANGUAGE_MARATHI             LANGUAGE_DEF('m','r')
#define LANGUAGE_MOLDAVIAN           LANGUAGE_DEF('m','o')
#define LANGUAGE_MONGOLIAN           LANGUAGE_DEF('m','n')
#define LANGUAGE_NAURU               LANGUAGE_DEF('n','a')
#define LANGUAGE_NEPALI              LANGUAGE_DEF('n','e')
#define LANGUAGE_NORWEGIAN           LANGUAGE_DEF('n','o')
#define LANGUAGE_NORWEGIAN_BOKMAL    LANGUAGE_DEF('n','b')
#define LANGUAGE_NORWEGIAN_NYNORSK   LANGUAGE_DEF('n','n')
#define LANGUAGE_OCCITAN             LANGUAGE_DEF('o','c')
#define LANGUAGE_ORIYA               LANGUAGE_DEF('o','r')
#define LANGUAGE_PASHTO_PUSHTO       LANGUAGE_DEF('p','s')
#define LANGUAGE_PERSIAN             LANGUAGE_DEF('f','a')
#define LANGUAGE_POLISH              LANGUAGE_DEF('p','l')
#define LANGUAGE_PORTUGUESE          LANGUAGE_DEF('p','t')
#define LANGUAGE_PUNJABI             LANGUAGE_DEF('p','a')
#define LANGUAGE_QUECHUA             LANGUAGE_DEF('q','u')
#define LANGUAGE_RHAETO_ROMANCE      LANGUAGE_DEF('r','m')
#define LANGUAGE_ROMANIAN            LANGUAGE_DEF('r','o')
#define LANGUAGE_RUSSIAN             LANGUAGE_DEF('r','u')
#define LANGUAGE_SAMI                LANGUAGE_DEF('s','e')
#define LANGUAGE_SAMOAN              LANGUAGE_DEF('s','m')
#define LANGUAGE_SANGRO              LANGUAGE_DEF('s','g')
#define LANGUAGE_SANSKRIT            LANGUAGE_DEF('s','a')
#define LANGUAGE_SCOTS_GAELIC        LANGUAGE_DEF('g','d')
#define LANGUAGE_SERBIAN             LANGUAGE_DEF('s','r')
#define LANGUAGE_SERBO_CROATIAN      LANGUAGE_DEF('s','h')
#define LANGUAGE_SESOTHO             LANGUAGE_DEF('s','t')
#define LANGUAGE_SETSWANA            LANGUAGE_DEF('t','n')
#define LANGUAGE_SHONA               LANGUAGE_DEF('s','n')
#define LANGUAGE_SINDHI              LANGUAGE_DEF('s','d')
#define LANGUAGE_SINGHALESE          LANGUAGE_DEF('s','i')
#define LANGUAGE_SISWATI             LANGUAGE_DEF('s','s')
#define LANGUAGE_SLOVAK              LANGUAGE_DEF('s','k')
#define LANGUAGE_SLOVENIAN           LANGUAGE_DEF('s','l')
#define LANGUAGE_SOMALI              LANGUAGE_DEF('s','o')
#define LANGUAGE_SPANISH             LANGUAGE_DEF('e','s')
#define LANGUAGE_SUDANESE            LANGUAGE_DEF('s','u')
#define LANGUAGE_SWAHILI             LANGUAGE_DEF('s','w')
#define LANGUAGE_SWEDISH             LANGUAGE_DEF('s','v')
#define LANGUAGE_TAGALOG             LANGUAGE_DEF('t','l')
#define LANGUAGE_TAJIK               LANGUAGE_DEF('t','g')
#define LANGUAGE_TAMIL               LANGUAGE_DEF('t','a')
#define LANGUAGE_TATAR               LANGUAGE_DEF('t','t')
#define LANGUAGE_TELUGU              LANGUAGE_DEF('t','e')
#define LANGUAGE_THAI                LANGUAGE_DEF('t','h')
#define LANGUAGE_TIBETAN             LANGUAGE_DEF('b','o')
#define LANGUAGE_TIGRINYA            LANGUAGE_DEF('t','i')
#define LANGUAGE_TONGA               LANGUAGE_DEF('t','o')
#define LANGUAGE_TSONGA              LANGUAGE_DEF('t','s')
#define LANGUAGE_TURKISH             LANGUAGE_DEF('t','r')
#define LANGUAGE_TURKMEN             LANGUAGE_DEF('t','k')
#define LANGUAGE_TWI                 LANGUAGE_DEF('t','w')
#define LANGUAGE_UIGHUR              LANGUAGE_DEF('u','g')
#define LANGUAGE_UKRAINIAN           LANGUAGE_DEF('u','k')
#define LANGUAGE_URDU                LANGUAGE_DEF('u','r')
#define LANGUAGE_UZBEK               LANGUAGE_DEF('u','z')
#define LANGUAGE_VIETNAMESE          LANGUAGE_DEF('v','i')
#define LANGUAGE_VOLAPUK             LANGUAGE_DEF('v','o')
#define LANGUAGE_WELSH               LANGUAGE_DEF('c','y')
#define LANGUAGE_WOLOF               LANGUAGE_DEF('w','o')
#define LANGUAGE_XHOSA               LANGUAGE_DEF('x','h')
#define LANGUAGE_YI                  LANGUAGE_DEF('i','i')
#define LANGUAGE_YIDDISH             LANGUAGE_DEF('y','i')
#define LANGUAGE_YORUBA              LANGUAGE_DEF('y','o')
#define LANGUAGE_ZHUANG              LANGUAGE_DEF('z','a')
#define LANGUAGE_ZULU                LANGUAGE_DEF('z','u')

#define LANGUAGE_DEFAULT             LANGUAGE_ENGLISH

// Predefined country identifiers
#define COUNTRY_AFGHANISTAN                                      COUNTRY_DEF('A','F')
#define COUNTRY_ALBANIA                                          COUNTRY_DEF('A','L')
#define COUNTRY_ALGERIA                                          COUNTRY_DEF('D','Z')
#define COUNTRY_AMERICAN_SAMOA                                   COUNTRY_DEF('A','S')
#define COUNTRY_ANDORRA                                          COUNTRY_DEF('A','D')
#define COUNTRY_ANGOLA                                           COUNTRY_DEF('A','O')
#define COUNTRY_ANGUILLA                                         COUNTRY_DEF('A','I')
#define COUNTRY_ANTARCTICA                                       COUNTRY_DEF('A','Q')
#define COUNTRY_ANTIGUA_BARBUDA                                  COUNTRY_DEF('A','G')
#define COUNTRY_ARGENTINA                                        COUNTRY_DEF('A','R')
#define COUNTRY_ARMENIA                                          COUNTRY_DEF('A','M')
#define COUNTRY_ARUBA                                            COUNTRY_DEF('A','W')
#define COUNTRY_AUSTRALIA                                        COUNTRY_DEF('A','U')
#define COUNTRY_AUSTRIA                                          COUNTRY_DEF('A','T')
#define COUNTRY_AZERBAIJAN                                       COUNTRY_DEF('A','Z')
#define COUNTRY_BAHAMAS                                          COUNTRY_DEF('B','S')
#define COUNTRY_BAHRAIN                                          COUNTRY_DEF('B','H')
#define COUNTRY_BANGLADESH                                       COUNTRY_DEF('B','D')
#define COUNTRY_BARBADOS                                         COUNTRY_DEF('B','B')
#define COUNTRY_BELARUS                                          COUNTRY_DEF('B','Y')
#define COUNTRY_BELGIUM                                          COUNTRY_DEF('B','E')
#define COUNTRY_BELIZE                                           COUNTRY_DEF('B','Z')
#define COUNTRY_BENIN                                            COUNTRY_DEF('B','J')
#define COUNTRY_BERMUDA                                          COUNTRY_DEF('B','M')
#define COUNTRY_BHUTAN                                           COUNTRY_DEF('B','T')
#define COUNTRY_BOLIVIA                                          COUNTRY_DEF('B','O')
#define COUNTRY_BOSNIA_HERZEGOVINA                               COUNTRY_DEF('B','A')
#define COUNTRY_BOTSWANA                                         COUNTRY_DEF('B','W')
#define COUNTRY_BOUVET_ISLAND                                    COUNTRY_DEF('B','V')
#define COUNTRY_BRAZIL                                           COUNTRY_DEF('B','R')
#define COUNTRY_BRITISH_INDIAN_OCEAN_TERRITORY                   COUNTRY_DEF('I','O')
#define COUNTRY_BRUNEI_DARUSSALAM                                COUNTRY_DEF('B','N')
#define COUNTRY_BULGARIA                                         COUNTRY_DEF('B','G')
#define COUNTRY_BURKINA_FASO                                     COUNTRY_DEF('B','F')
#define COUNTRY_BURUNDI                                          COUNTRY_DEF('B','I')
#define COUNTRY_CAMBODIA                                         COUNTRY_DEF('K','H')
#define COUNTRY_CAMEROON                                         COUNTRY_DEF('C','M')
#define COUNTRY_CANADA                                           COUNTRY_DEF('C','A')
#define COUNTRY_CAPE_VERDE                                       COUNTRY_DEF('C','V')
#define COUNTRY_CAYMAN_ISLANDS                                   COUNTRY_DEF('K','Y')
#define COUNTRY_CENTRAL_AFRICAN_REPUBLIC                         COUNTRY_DEF('C','F')
#define COUNTRY_CHAD                                             COUNTRY_DEF('T','D')
#define COUNTRY_CHILE                                            COUNTRY_DEF('C','L')
#define COUNTRY_CHINA                                            COUNTRY_DEF('C','N')
#define COUNTRY_CHRISTMAS_ISLAND                                 COUNTRY_DEF('C','X')
#define COUNTRY_COCOS_KEELING_ISLANDS                            COUNTRY_DEF('C','C')
#define COUNTRY_COLOMBIA                                         COUNTRY_DEF('C','O')
#define COUNTRY_COMOROS                                          COUNTRY_DEF('K','M')
#define COUNTRY_CONGO                                            COUNTRY_DEF('C','G')
#define COUNTRY_CONGO_DRC                                        COUNTRY_DEF('C','D')
#define COUNTRY_COOK_ISLANDS                                     COUNTRY_DEF('C','K')
#define COUNTRY_COSTA_RICA                                       COUNTRY_DEF('C','R')
#define COUNTRY_COTE_DIVOIRE                                     COUNTRY_DEF('C','I')
#define COUNTRY_CROATIA                                          COUNTRY_DEF('H','R')
#define COUNTRY_CUBA                                             COUNTRY_DEF('C','U')
#define COUNTRY_CYPRUS                                           COUNTRY_DEF('C','Y')
#define COUNTRY_CZECH_REPUBLIC                                   COUNTRY_DEF('C','Z')
#define COUNTRY_DENMARK                                          COUNTRY_DEF('D','K')
#define COUNTRY_DJIBOUTI                                         COUNTRY_DEF('D','J')
#define COUNTRY_DOMINICA                                         COUNTRY_DEF('D','M')
#define COUNTRY_DOMINICAN_REPUBLIC                               COUNTRY_DEF('D','O')
#define COUNTRY_EAST_TIMOR                                       COUNTRY_DEF('T','P')
#define COUNTRY_ECUADOR                                          COUNTRY_DEF('E','C')
#define COUNTRY_EGYPT                                            COUNTRY_DEF('E','G')
#define COUNTRY_EL_SALVADOR                                      COUNTRY_DEF('S','V')
#define COUNTRY_EQUATORIAL_GUINEA                                COUNTRY_DEF('G','Q')
#define COUNTRY_ERITREA                                          COUNTRY_DEF('E','R')
#define COUNTRY_ESTONIA                                          COUNTRY_DEF('E','E')
#define COUNTRY_ETHIOPIA                                         COUNTRY_DEF('E','T')
#define COUNTRY_EUROPE_SSGFI_ONLY                                COUNTRY_DEF('E','U')
#define COUNTRY_FALKLAND_ISLANDS                                 COUNTRY_DEF('F','K')
#define COUNTRY_FAEROE_ISLANDS                                   COUNTRY_DEF('F','O')
#define COUNTRY_FIJI                                             COUNTRY_DEF('F','J')
#define COUNTRY_FINLAND                                          COUNTRY_DEF('F','I')
#define COUNTRY_FRANCE                                           COUNTRY_DEF('F','R')
#define COUNTRY_FRANCE_METROPOLITAN                              COUNTRY_DEF('F','X')
#define COUNTRY_FRENCH_GUIANA                                    COUNTRY_DEF('G','F')
#define COUNTRY_FRENCH_POLYNESIA                                 COUNTRY_DEF('P','F')
#define COUNTRY_FRENCH_SOUTHERN_TERRITORIES                      COUNTRY_DEF('T','F')
#define COUNTRY_GABON                                            COUNTRY_DEF('G','A')
#define COUNTRY_GAMBIA                                           COUNTRY_DEF('G','M')
#define COUNTRY_GEORGIA                                          COUNTRY_DEF('G','E')
#define COUNTRY_GERMANY                                          COUNTRY_DEF('D','E')
#define COUNTRY_GHANA                                            COUNTRY_DEF('G','H')
#define COUNTRY_GIBRALTAR                                        COUNTRY_DEF('G','I')
#define COUNTRY_GREECE                                           COUNTRY_DEF('G','R')
#define COUNTRY_GREENLAND                                        COUNTRY_DEF('G','L')
#define COUNTRY_GRENADA                                          COUNTRY_DEF('G','D')
#define COUNTRY_GUADELOUPE                                       COUNTRY_DEF('G','P')
#define COUNTRY_GUAM                                             COUNTRY_DEF('G','U')
#define COUNTRY_GUATEMALA                                        COUNTRY_DEF('G','T')
#define COUNTRY_GUERNSEY                                         COUNTRY_DEF('G','G')
#define COUNTRY_GUINEA                                           COUNTRY_DEF('G','N')
#define COUNTRY_GUINEA_BISSAU                                    COUNTRY_DEF('G','W')
#define COUNTRY_GUYANA                                           COUNTRY_DEF('G','Y')
#define COUNTRY_HAITI                                            COUNTRY_DEF('H','T')
#define COUNTRY_HEARD_AND_MC_DONALD_ISLANDS                      COUNTRY_DEF('H','M')
#define COUNTRY_HONDURAS                                         COUNTRY_DEF('H','N')
#define COUNTRY_HONG_KONG                                        COUNTRY_DEF('H','K')
#define COUNTRY_HUNGARY                                          COUNTRY_DEF('H','U')
#define COUNTRY_ICELAND                                          COUNTRY_DEF('I','S')
#define COUNTRY_INDIA                                            COUNTRY_DEF('I','N')
#define COUNTRY_INDONESIA                                        COUNTRY_DEF('I','D')
#define COUNTRY_INTERNATIONAL_SSGFI_ONLY                         COUNTRY_DEF('I','I')
#define COUNTRY_IRAN                                             COUNTRY_DEF('I','R')
#define COUNTRY_IRAQ                                             COUNTRY_DEF('I','Q')
#define COUNTRY_IRELAND                                          COUNTRY_DEF('I','E')
#define COUNTRY_ISLE_OF_MAN                                      COUNTRY_DEF('I','M')
#define COUNTRY_ISRAEL                                           COUNTRY_DEF('I','L')
#define COUNTRY_ITALY                                            COUNTRY_DEF('I','T')
#define COUNTRY_JAMAICA                                          COUNTRY_DEF('J','M')
#define COUNTRY_JAPAN                                            COUNTRY_DEF('J','P')
#define COUNTRY_JERSEY                                           COUNTRY_DEF('J','E')
#define COUNTRY_JORDAN                                           COUNTRY_DEF('J','O')
#define COUNTRY_KAZAKHSTAN                                       COUNTRY_DEF('K','Z')
#define COUNTRY_KENYA                                            COUNTRY_DEF('K','E')
#define COUNTRY_KIRIBATI                                         COUNTRY_DEF('K','I')
#define COUNTRY_KOREA_DEMOCRATIC_PEOPLES_REPUBLIC_OF             COUNTRY_DEF('K','P')
#define COUNTRY_KOREA_REPUBLIC_OF                                COUNTRY_DEF('K','R')
#define COUNTRY_KUWAIT                                           COUNTRY_DEF('K','W')
#define COUNTRY_KYRGYZSTAN                                       COUNTRY_DEF('K','G')
#define COUNTRY_LAO_PEOPLES_DEMOCRATIC_REPUBLIC                  COUNTRY_DEF('L','A')
#define COUNTRY_LATVIA                                           COUNTRY_DEF('L','V')
#define COUNTRY_LEBANON                                          COUNTRY_DEF('L','B')
#define COUNTRY_LESOTHO                                          COUNTRY_DEF('L','S')
#define COUNTRY_LIBERIA                                          COUNTRY_DEF('L','R')
#define COUNTRY_LIBYAN_ARAB_JAMAHIRIYA                           COUNTRY_DEF('L','Y')
#define COUNTRY_LIECHTENSTEIN                                    COUNTRY_DEF('L','I')
#define COUNTRY_LITHUANIA                                        COUNTRY_DEF('L','T')
#define COUNTRY_LUXEMBOURG                                       COUNTRY_DEF('L','U')
#define COUNTRY_MACAU                                            COUNTRY_DEF('M','O')
#define COUNTRY_MACEDONIA_THE_FORMER_YUGOSLAV_REPUBLIC_OF        COUNTRY_DEF('M','K')
#define COUNTRY_MADAGASCAR                                       COUNTRY_DEF('M','G')
#define COUNTRY_MALAWI                                           COUNTRY_DEF('M','W')
#define COUNTRY_MALAYSIA                                         COUNTRY_DEF('M','Y')
#define COUNTRY_MALDIVES                                         COUNTRY_DEF('M','V')
#define COUNTRY_MALI                                             COUNTRY_DEF('M','L')
#define COUNTRY_MALTA                                            COUNTRY_DEF('M','T')
#define COUNTRY_MARSHALL_ISLANDS                                 COUNTRY_DEF('M','H')
#define COUNTRY_MARTINIQUE                                       COUNTRY_DEF('M','Q')
#define COUNTRY_MAURITANIA                                       COUNTRY_DEF('M','R')
#define COUNTRY_MAURITIUS                                        COUNTRY_DEF('M','U')
#define COUNTRY_MAYOTTE                                          COUNTRY_DEF('Y','T')
#define COUNTRY_MEXICO                                           COUNTRY_DEF('M','X')
#define COUNTRY_MICRONESIA_FEDERATED_STATES_OF                   COUNTRY_DEF('F','M')
#define COUNTRY_MOLDOVA_REPUBLIC_OF                              COUNTRY_DEF('M','D')
#define COUNTRY_MONACO                                           COUNTRY_DEF('M','C')
#define COUNTRY_MONGOLIA                                         COUNTRY_DEF('M','N')
#define COUNTRY_MONTENEGRO                                       COUNTRY_DEF('M','E')
#define COUNTRY_MONTSERRAT                                       COUNTRY_DEF('M','S')
#define COUNTRY_MOROCCO                                          COUNTRY_DEF('M','A')
#define COUNTRY_MOZAMBIQUE                                       COUNTRY_DEF('M','Z')
#define COUNTRY_MYANMAR                                          COUNTRY_DEF('M','M')
#define COUNTRY_NAMIBIA                                          COUNTRY_DEF('N','A')
#define COUNTRY_NAURU                                            COUNTRY_DEF('N','R')
#define COUNTRY_NEPAL                                            COUNTRY_DEF('N','P')
#define COUNTRY_NETHERLANDS                                      COUNTRY_DEF('N','L')
#define COUNTRY_NETHERLANDS_ANTILLES                             COUNTRY_DEF('A','N')
#define COUNTRY_NEW_CALEDONIA                                    COUNTRY_DEF('N','C')
#define COUNTRY_NEW_ZEALAND                                      COUNTRY_DEF('N','Z')
#define COUNTRY_NICARAGUA                                        COUNTRY_DEF('N','I')
#define COUNTRY_NIGER                                            COUNTRY_DEF('N','E')
#define COUNTRY_NIGERIA                                          COUNTRY_DEF('N','G')
#define COUNTRY_NIUE                                             COUNTRY_DEF('N','U')
#define COUNTRY_NORFOLK_ISLAND                                   COUNTRY_DEF('N','F')
#define COUNTRY_NORTHERN_MARIANA_ISLANDS                         COUNTRY_DEF('M','P')
#define COUNTRY_NORWAY                                           COUNTRY_DEF('N','O')
#define COUNTRY_OMAN                                             COUNTRY_DEF('O','M')
#define COUNTRY_PAKISTAN                                         COUNTRY_DEF('P','K')
#define COUNTRY_PALAU                                            COUNTRY_DEF('P','W')
#define COUNTRY_PALESTINIAN_TERRITORY                            COUNTRY_DEF('P','S')
#define COUNTRY_PANAMA                                           COUNTRY_DEF('P','A')
#define COUNTRY_PAPUA_NEW_GUINEA                                 COUNTRY_DEF('P','G')
#define COUNTRY_PARAGUAY                                         COUNTRY_DEF('P','Y')
#define COUNTRY_PERU                                             COUNTRY_DEF('P','E')
#define COUNTRY_PHILIPPINES                                      COUNTRY_DEF('P','H')
#define COUNTRY_PITCAIRN                                         COUNTRY_DEF('P','N')
#define COUNTRY_POLAND                                           COUNTRY_DEF('P','L')
#define COUNTRY_PORTUGAL                                         COUNTRY_DEF('P','T')
#define COUNTRY_PUERTO_RICO                                      COUNTRY_DEF('P','R')
#define COUNTRY_QATAR                                            COUNTRY_DEF('Q','A')
#define COUNTRY_REUNION                                          COUNTRY_DEF('R','E')
#define COUNTRY_ROMANIA                                          COUNTRY_DEF('R','O')
#define COUNTRY_RUSSIAN_FEDERATION                               COUNTRY_DEF('R','U')
#define COUNTRY_RWANDA                                           COUNTRY_DEF('R','W')
#define COUNTRY_SAINT_KITTS_AND_NEVIS                            COUNTRY_DEF('K','N')
#define COUNTRY_SAINT_LUCIA                                      COUNTRY_DEF('L','C')
#define COUNTRY_SAINT_VINCENT_AND_THE_GRENADINES                 COUNTRY_DEF('V','C')
#define COUNTRY_SAMOA                                            COUNTRY_DEF('W','S')
#define COUNTRY_SAN_MARINO                                       COUNTRY_DEF('S','M')
#define COUNTRY_SAO_TOME_AND_PRINCIPE                            COUNTRY_DEF('S','T')
#define COUNTRY_SAUDI_ARABIA                                     COUNTRY_DEF('S','A')
#define COUNTRY_SENEGAL                                          COUNTRY_DEF('S','N')
#define COUNTRY_SERBIA                                           COUNTRY_DEF('R','S')
#define COUNTRY_SERBIA_AND_MONTENEGRO                            COUNTRY_DEF('C','S')
#define COUNTRY_SEYCHELLES                                       COUNTRY_DEF('S','C')
#define COUNTRY_SIERRA_LEONE                                     COUNTRY_DEF('S','L')
#define COUNTRY_SINGAPORE                                        COUNTRY_DEF('S','G')
#define COUNTRY_SLOVAKIA                                         COUNTRY_DEF('S','K')
#define COUNTRY_SLOVENIA                                         COUNTRY_DEF('S','I')
#define COUNTRY_SOLOMON_ISLANDS                                  COUNTRY_DEF('S','B')
#define COUNTRY_SOMALIA                                          COUNTRY_DEF('S','O')
#define COUNTRY_SOUTH_AFRICA                                     COUNTRY_DEF('Z','A')
#define COUNTRY_SOUTH_GEORGIA_AND_THE_SOUTH_SANDWICH_ISLANDS     COUNTRY_DEF('G','S')
#define COUNTRY_SPAIN                                            COUNTRY_DEF('E','S')
#define COUNTRY_SRI_LANKA                                        COUNTRY_DEF('L','K')
#define COUNTRY_ST_HELENA                                        COUNTRY_DEF('S','H')
#define COUNTRY_ST_PIERRE_AND_MIQUELON                           COUNTRY_DEF('P','M')
#define COUNTRY_ST_HELENA_ASCENSION_AND_TRISTAN_DA_CUNHA         COUNTRY_DEF('S','H')
#define COUNTRY_SUDAN                                            COUNTRY_DEF('S','D')
#define COUNTRY_SURINAME                                         COUNTRY_DEF('S','R')
#define COUNTRY_SVALBARD_AND_JAN_MAYEN_ISLANDS                   COUNTRY_DEF('S','J')
#define COUNTRY_SWAZILAND                                        COUNTRY_DEF('S','Z')
#define COUNTRY_SWEDEN                                           COUNTRY_DEF('S','E')
#define COUNTRY_SWITZERLAND                                      COUNTRY_DEF('C','H')
#define COUNTRY_SYRIAN_ARAB_REPUBLIC                             COUNTRY_DEF('S','Y')
#define COUNTRY_TAIWAN                                           COUNTRY_DEF('T','W')
#define COUNTRY_TAJIKISTAN                                       COUNTRY_DEF('T','J')
#define COUNTRY_TANZANIA_UNITED_REPUBLIC_OF                      COUNTRY_DEF('T','Z')
#define COUNTRY_THAILAND                                         COUNTRY_DEF('T','H')
#define COUNTRY_TIMOR_LESTE                                      COUNTRY_DEF('T','L')
#define COUNTRY_TOGO                                             COUNTRY_DEF('T','G')
#define COUNTRY_TOKELAU                                          COUNTRY_DEF('T','K')
#define COUNTRY_TONGA                                            COUNTRY_DEF('T','O')
#define COUNTRY_TRINIDAD_AND_TOBAGO                              COUNTRY_DEF('T','T')
#define COUNTRY_TUNISIA                                          COUNTRY_DEF('T','N')
#define COUNTRY_TURKEY                                           COUNTRY_DEF('T','R')
#define COUNTRY_TURKMENISTAN                                     COUNTRY_DEF('T','M')
#define COUNTRY_TURKS_AND_CAICOS_ISLANDS                         COUNTRY_DEF('T','C')
#define COUNTRY_TUVALU                                           COUNTRY_DEF('T','V')
#define COUNTRY_UGANDA                                           COUNTRY_DEF('U','G')
#define COUNTRY_UKRAINE                                          COUNTRY_DEF('U','A')
#define COUNTRY_UNITED_ARAB_EMIRATES                             COUNTRY_DEF('A','E')
#define COUNTRY_UNITED_KINGDOM                                   COUNTRY_DEF('G','B')
#define COUNTRY_UNITED_STATES                                    COUNTRY_DEF('U','S')
#define COUNTRY_UNITED_STATES_MINOR_OUTLYING_ISLANDS             COUNTRY_DEF('U','M')
#define COUNTRY_URUGUAY                                          COUNTRY_DEF('U','Y')
#define COUNTRY_UZBEKISTAN                                       COUNTRY_DEF('U','Z')
#define COUNTRY_VANUATU                                          COUNTRY_DEF('V','U')
#define COUNTRY_VATICAN_CITY_STATE                               COUNTRY_DEF('V','A')
#define COUNTRY_VENEZUELA                                        COUNTRY_DEF('V','E')
#define COUNTRY_VIETNAM                                          COUNTRY_DEF('V','N')
#define COUNTRY_VIRGIN_ISLANDS_BRITISH                           COUNTRY_DEF('V','G')
#define COUNTRY_VIRGIN_ISLANDS_US                                COUNTRY_DEF('V','I')
#define COUNTRY_WALLIS_AND_FUTUNA_ISLANDS                        COUNTRY_DEF('W','F')
#define COUNTRY_WESTERN_SAHARA                                   COUNTRY_DEF('E','H')
#define COUNTRY_YEMEN                                            COUNTRY_DEF('Y','E')
#define COUNTRY_YUGOSLAVIA                                       COUNTRY_DEF('Y','U')
#define COUNTRY_ZAIRE                                            COUNTRY_DEF('Z','R')
#define COUNTRY_ZAMBIA                                           COUNTRY_DEF('Z','M')
#define COUNTRY_ZIMBABWE                                         COUNTRY_DEF('Z','W')

/*! \brief Default country */
#define COUNTRY_DEFAULT                                          COUNTRY_UNITED_STATES

// Predefined standard locales

/*! \brief Predefined 'enUS' locale */
#define LOCALE_EN_US     LOCALE_FROM_LANGUAGE_COUNTRY( LANGUAGE_ENGLISH, COUNTRY_UNITED_STATES )

/*! \brief Predefined default locale */
#define LOCALE_DEFAULT   LOCALE_EN_US

/*!
\def LANGUAGE_AFAN_OROMO
\brief Afan oromo language

\def LANGUAGE_ABKHAZIAN
\brief Abkhazian language

\def LANGUAGE_AFAR
\brief Afar language

\def LANGUAGE_AFRIKAANS
\brief Afrikaans language

\def LANGUAGE_ALBANIAN
\brief Albanian language

\def LANGUAGE_AMHARIC
\brief Amharic language

\def LANGUAGE_ARABIC
\brief Arabic language

\def LANGUAGE_ARMENIAN
\brief Armenian language

\def LANGUAGE_ASSAMESE
\brief Assamese language

\def LANGUAGE_AYMARA
\brief Aymara language

\def LANGUAGE_AZERBAIJANI
\brief Azerbaijani language

\def LANGUAGE_BASHKIR
\brief Bashkir language

\def LANGUAGE_BASQUE
\brief Basque language

\def LANGUAGE_BENGALI
\brief Bengali language

\def LANGUAGE_BHUTANI
\brief Bhutani language

\def LANGUAGE_BIHARI
\brief Bihari language

\def LANGUAGE_BISLAMA
\brief Bislama language

\def LANGUAGE_BOSNIAN
\brief Bosnian language

\def LANGUAGE_BRETON
\brief Breton language

\def LANGUAGE_BULGARIAN
\brief Bulgarian language

\def LANGUAGE_BURMESE
\brief Burmese language

\def LANGUAGE_BYELORUSSIAN
\brief Byelorussian language

\def LANGUAGE_CAMBODIAN
\brief Cambodian language

\def LANGUAGE_CATALAN
\brief Catalan language

\def LANGUAGE_CHINESE
\brief Chinese language

\def LANGUAGE_CORSICAN
\brief Corsican language

\def LANGUAGE_CROATIAN
\brief Croatian language

\def LANGUAGE_CZECH
\brief Czech language

\def LANGUAGE_DANISH
\brief Danish language

\def LANGUAGE_DIVEHI
\brief Divehi language

\def LANGUAGE_DUTCH
\brief Dutch language

\def LANGUAGE_ENGLISH
\brief English language

\def LANGUAGE_ESPERANTO
\brief Esperanto language

\def LANGUAGE_ESTONIAN
\brief Estonian language

\def LANGUAGE_FAEROESE
\brief Faeroese language

\def LANGUAGE_FIJI
\brief Fiji language

\def LANGUAGE_FINNISH
\brief Finnish language

\def LANGUAGE_FRENCH
\brief French language

\def LANGUAGE_FRISIAN
\brief Frisian language

\def LANGUAGE_GALICIAN
\brief Galician language

\def LANGUAGE_GEORGIAN
\brief Georgian language

\def LANGUAGE_GERMAN
\brief German language

\def LANGUAGE_GREEK
\brief Greek language

\def LANGUAGE_GREENLANDIC
\brief Greenlandic language

\def LANGUAGE_GUARANI
\brief Guarani language

\def LANGUAGE_GUJARATI
\brief Gujarati language

\def LANGUAGE_HAUSA
\brief Hausa language

\def LANGUAGE_HEBREW
\brief Hebrew language

\def LANGUAGE_HINDI
\brief Hindi language

\def LANGUAGE_HUNGARIAN
\brief Hungarian language

\def LANGUAGE_ICELANDIC
\brief Icelandic language

\def LANGUAGE_IGBO
\brief Igbo language

\def LANGUAGE_INDONESIAN
\brief Indonesian language

\def LANGUAGE_INTERLINGUA
\brief Interlingua language

\def LANGUAGE_INTERLINGUE
\brief Interlingue language

\def LANGUAGE_INUPIAK
\brief Inupiak language

\def LANGUAGE_INUKTITUT
\brief Inuktitut language

\def LANGUAGE_IRISH
\brief Irish language

\def LANGUAGE_ITALIAN
\brief Italian language

\def LANGUAGE_JAPANESE
\brief Japanese language

\def LANGUAGE_JAVANESE
\brief Javanese language

\def LANGUAGE_KANNADA
\brief Kannada language

\def LANGUAGE_KASHMIRI
\brief Kashmiri language

\def LANGUAGE_KAZAKH
\brief Kazakh language

\def LANGUAGE_KINYARWANDA
\brief Kinyarwanda language

\def LANGUAGE_KIRGHIZ
\brief Kirghiz language

\def LANGUAGE_KIRUNDI
\brief Kirundi language

\def LANGUAGE_KOREAN
\brief Korean language

\def LANGUAGE_KURDISH
\brief Kurdish language

\def LANGUAGE_LAOTHIAN
\brief Laothian language

\def LANGUAGE_LATIN
\brief Latin language

\def LANGUAGE_LATVIAN_LETTISH
\brief Latvian lettish language

\def LANGUAGE_LINGALA
\brief Lingala language

\def LANGUAGE_LITHUANIAN
\brief Lithuanian language

\def LANGUAGE_LUXEMBOURGISH
\brief Luxembourgish language

\def LANGUAGE_MACEDONIAN
\brief Macedonian language

\def LANGUAGE_MALAGASY
\brief Malagasy language

\def LANGUAGE_MALAY
\brief Malay language

\def LANGUAGE_MALAYALAM
\brief Malayalam language

\def LANGUAGE_MALTESE
\brief Maltese language

\def LANGUAGE_MAORI
\brief Maori language

\def LANGUAGE_MARATHI
\brief Marathi language

\def LANGUAGE_MOLDAVIAN
\brief Moldavian language

\def LANGUAGE_MONGOLIAN
\brief Mongolian language

\def LANGUAGE_NAURU
\brief Nauru language

\def LANGUAGE_NEPALI
\brief Nepali language

\def LANGUAGE_NORWEGIAN
\brief Norwegian language

\def LANGUAGE_NORWEGIAN_BOKMAL
\brief Norwegian bokmal language

\def LANGUAGE_NORWEGIAN_NYNORSK
\brief Norwegian nynorsk language

\def LANGUAGE_OCCITAN
\brief Occitan language

\def LANGUAGE_ORIYA
\brief Oriya language

\def LANGUAGE_PASHTO_PUSHTO
\brief Pashto pushto language

\def LANGUAGE_PERSIAN
\brief Persian language

\def LANGUAGE_POLISH
\brief Polish language

\def LANGUAGE_PORTUGUESE
\brief Portuguese language

\def LANGUAGE_PUNJABI
\brief Punjabi language

\def LANGUAGE_QUECHUA
\brief Quechua language

\def LANGUAGE_RHAETO_ROMANCE
\brief Rhaeto romance language

\def LANGUAGE_ROMANIAN
\brief Romanian language

\def LANGUAGE_RUSSIAN
\brief Russian language

\def LANGUAGE_SAMI
\brief Sami language

\def LANGUAGE_SAMOAN
\brief Samoan language

\def LANGUAGE_SANGRO
\brief Sangro language

\def LANGUAGE_SANSKRIT
\brief Sanskrit language

\def LANGUAGE_SCOTS_GAELIC
\brief Scots gaelic language

\def LANGUAGE_SERBIAN
\brief Serbian language

\def LANGUAGE_SERBO_CROATIAN
\brief Serbo croatian language

\def LANGUAGE_SESOTHO
\brief Sesotho language

\def LANGUAGE_SETSWANA
\brief Setswana language

\def LANGUAGE_SHONA
\brief Shona language

\def LANGUAGE_SINDHI
\brief Sindhi language

\def LANGUAGE_SINGHALESE
\brief Singhalese language

\def LANGUAGE_SISWATI
\brief Siswati language

\def LANGUAGE_SLOVAK
\brief Slovak language

\def LANGUAGE_SLOVENIAN
\brief Slovenian language

\def LANGUAGE_SOMALI
\brief Somali language

\def LANGUAGE_SPANISH
\brief Spanish language

\def LANGUAGE_SUDANESE
\brief Sudanese language

\def LANGUAGE_SWAHILI
\brief Swahili language

\def LANGUAGE_SWEDISH
\brief Swedish language

\def LANGUAGE_TAGALOG
\brief Tagalog language

\def LANGUAGE_TAJIK
\brief Tajik language

\def LANGUAGE_TAMIL
\brief Tamil language

\def LANGUAGE_TATAR
\brief Tatar language

\def LANGUAGE_TELUGU
\brief Telugu language

\def LANGUAGE_THAI
\brief Thai language

\def LANGUAGE_TIBETAN
\brief Tibetan language

\def LANGUAGE_TIGRINYA
\brief Tigrinya language

\def LANGUAGE_TONGA
\brief Tonga language

\def LANGUAGE_TSONGA
\brief Tsonga language

\def LANGUAGE_TURKISH
\brief Turkish language

\def LANGUAGE_TURKMEN
\brief Turkmen language

\def LANGUAGE_TWI
\brief Twi language

\def LANGUAGE_UIGHUR
\brief Uighur language

\def LANGUAGE_UKRAINIAN
\brief Ukrainian language

\def LANGUAGE_URDU
\brief Urdu language

\def LANGUAGE_UZBEK
\brief Uzbek language

\def LANGUAGE_VIETNAMESE
\brief Vietnamese language

\def LANGUAGE_VOLAPUK
\brief Volapuk language

\def LANGUAGE_WELSH
\brief Welsh language

\def LANGUAGE_WOLOF
\brief Wolof language

\def LANGUAGE_XHOSA
\brief Xhosa language

\def LANGUAGE_YI
\brief Yi language

\def LANGUAGE_YIDDISH
\brief Yiddish language

\def LANGUAGE_YORUBA
\brief Yoruba language

\def LANGUAGE_ZHUANG
\brief Zhuang language

\def LANGUAGE_ZULU
\brief Zulu language

\def LANGUAGE_DEFAULT
\brief Default language

\def COUNTRY_AFGHANISTAN
\brief Afghanistan country

\def COUNTRY_ALBANIA
\brief Albania country

\def COUNTRY_ALGERIA
\brief Algeria country

\def COUNTRY_AMERICAN_SAMOA
\brief American Samoa country

\def COUNTRY_ANDORRA
\brief Andorra country

\def COUNTRY_ANGOLA
\brief Angola country

\def COUNTRY_ANGUILLA
\brief Anguilla country

\def COUNTRY_ANTARCTICA
\brief Antarctica country

\def COUNTRY_ANTIGUA_BARBUDA
\brief Antigua Barbuda country

\def COUNTRY_ARGENTINA
\brief Argentina country

\def COUNTRY_ARMENIA
\brief Armenia country

\def COUNTRY_ARUBA
\brief Aruba country

\def COUNTRY_AUSTRALIA
\brief Australia country

\def COUNTRY_AUSTRIA
\brief Austria country

\def COUNTRY_AZERBAIJAN
\brief Azerbaijan country

\def COUNTRY_BAHAMAS
\brief Bahamas country

\def COUNTRY_BAHRAIN
\brief Bahrain country

\def COUNTRY_BANGLADESH
\brief Bangladesh country

\def COUNTRY_BARBADOS
\brief Barbados country

\def COUNTRY_BELARUS
\brief Belarus country

\def COUNTRY_BELGIUM
\brief Belgium country

\def COUNTRY_BELIZE
\brief Belize country

\def COUNTRY_BENIN
\brief Benin country

\def COUNTRY_BERMUDA
\brief Bermuda country

\def COUNTRY_BHUTAN
\brief Bhutan country

\def COUNTRY_BOLIVIA
\brief Bolivia country

\def COUNTRY_BOSNIA_HERZEGOVINA
\brief Bosnia Herzegovina country

\def COUNTRY_BOTSWANA
\brief Botswana country

\def COUNTRY_BOUVET_ISLAND
\brief Bouvet Island country

\def COUNTRY_BRAZIL
\brief Brazil country

\def COUNTRY_BRITISH_INDIAN_OCEAN_TERRITORY
\brief British Indian Ocean Territory country

\def COUNTRY_BRUNEI_DARUSSALAM
\brief Brunei Darussalam country

\def COUNTRY_BULGARIA
\brief Bulgaria country

\def COUNTRY_BURKINA_FASO
\brief Burkina Faso country

\def COUNTRY_BURUNDI
\brief Burundi country

\def COUNTRY_CAMBODIA
\brief Cambodia country

\def COUNTRY_CAMEROON
\brief Cameroon country

\def COUNTRY_CANADA
\brief Canada country

\def COUNTRY_CAPE_VERDE
\brief Cape Verde country

\def COUNTRY_CAYMAN_ISLANDS
\brief Cayman Islands country

\def COUNTRY_CENTRAL_AFRICAN_REPUBLIC
\brief Central African Republic country

\def COUNTRY_CHAD
\brief Chad country

\def COUNTRY_CHILE
\brief Chile country

\def COUNTRY_CHINA
\brief China country

\def COUNTRY_CHRISTMAS_ISLAND
\brief Christmas Island country

\def COUNTRY_COCOS_KEELING_ISLANDS
\brief Cocos Keeling Islands country

\def COUNTRY_COLOMBIA
\brief Colombia country

\def COUNTRY_COMOROS
\brief Comoros country

\def COUNTRY_CONGO
\brief Congo country

\def COUNTRY_CONGO_DRC
\brief Congo DRC country

\def COUNTRY_COOK_ISLANDS
\brief Cook Islands country

\def COUNTRY_COSTA_RICA
\brief Costa Rica country

\def COUNTRY_COTE_DIVOIRE
\brief Cote Divoire country

\def COUNTRY_CROATIA
\brief Croatia country

\def COUNTRY_CUBA
\brief Cuba country

\def COUNTRY_CYPRUS
\brief Cyprus country

\def COUNTRY_CZECH_REPUBLIC
\brief Czech Republic country

\def COUNTRY_DENMARK
\brief Denmark country

\def COUNTRY_DJIBOUTI
\brief Djibouti country

\def COUNTRY_DOMINICA
\brief Dominica country

\def COUNTRY_DOMINICAN_REPUBLIC
\brief Dominican Republic country

\def COUNTRY_EAST_TIMOR
\brief East Timor country

\def COUNTRY_ECUADOR
\brief Ecuador country

\def COUNTRY_EGYPT
\brief Egypt country

\def COUNTRY_EL_SALVADOR
\brief El Salvador country

\def COUNTRY_EQUATORIAL_GUINEA
\brief Equatorial Guinea country

\def COUNTRY_ERITREA
\brief Eritrea country

\def COUNTRY_ESTONIA
\brief Estonia country

\def COUNTRY_ETHIOPIA
\brief Ethiopia country

\def COUNTRY_EUROPE_SSGFI_ONLY
\brief Europe SSGFI only country

\def COUNTRY_FALKLAND_ISLANDS
\brief Falkland Islands country

\def COUNTRY_FAEROE_ISLANDS
\brief Faeroe Islands country

\def COUNTRY_FIJI
\brief Fiji country

\def COUNTRY_FINLAND
\brief Finland country

\def COUNTRY_FRANCE
\brief France country

\def COUNTRY_FRANCE_METROPOLITAN
\brief France Metropolitan country

\def COUNTRY_FRENCH_GUIANA
\brief French Guiana country

\def COUNTRY_FRENCH_POLYNESIA
\brief French Polynesia country

\def COUNTRY_FRENCH_SOUTHERN_TERRITORIES
\brief French Southern Territories country

\def COUNTRY_GABON
\brief Gabon country

\def COUNTRY_GAMBIA
\brief Gambia country

\def COUNTRY_GEORGIA
\brief Georgia country

\def COUNTRY_GERMANY
\brief Germany country

\def COUNTRY_GHANA
\brief Ghana country

\def COUNTRY_GIBRALTAR
\brief Gibraltar country

\def COUNTRY_GREECE
\brief Greece country

\def COUNTRY_GREENLAND
\brief Greenland country

\def COUNTRY_GRENADA
\brief Grenada country

\def COUNTRY_GUADELOUPE
\brief Guadeloupe country

\def COUNTRY_GUAM
\brief Guam country

\def COUNTRY_GUATEMALA
\brief Guatemala country

\def COUNTRY_GUERNSEY
\brief Guernsey country

\def COUNTRY_GUINEA
\brief Guinea country

\def COUNTRY_GUINEA_BISSAU
\brief Guinea Bissau country

\def COUNTRY_GUYANA
\brief Guyana country

\def COUNTRY_HAITI
\brief Haiti country

\def COUNTRY_HEARD_AND_MC_DONALD_ISLANDS
\brief Heard And Mc Donald Islands country

\def COUNTRY_HONDURAS
\brief Honduras country

\def COUNTRY_HONG_KONG
\brief Hong_kong country

\def COUNTRY_HUNGARY
\brief Hungary country

\def COUNTRY_ICELAND
\brief Iceland country

\def COUNTRY_INDIA
\brief India country

\def COUNTRY_INDONESIA
\brief Indonesia country

\def COUNTRY_INTERNATIONAL_SSGFI_ONLY
\brief International SSGFI only country

\def COUNTRY_IRAN
\brief Iran country

\def COUNTRY_IRAQ
\brief Iraq country

\def COUNTRY_IRELAND
\brief Ireland country

\def COUNTRY_ISLE_OF_MAN
\brief Isle of Man country

\def COUNTRY_ISRAEL
\brief Israel country

\def COUNTRY_ITALY
\brief Italy country

\def COUNTRY_JAMAICA
\brief Jamaica country

\def COUNTRY_JAPAN
\brief Japan country

\def COUNTRY_JERSEY
\brief Jersey country

\def COUNTRY_JORDAN
\brief Jordan country

\def COUNTRY_KAZAKHSTAN
\brief Kazakhstan country

\def COUNTRY_KENYA
\brief Kenya country

\def COUNTRY_KIRIBATI
\brief Kiribati country

\def COUNTRY_KOREA_DEMOCRATIC_PEOPLES_REPUBLIC_OF
\brief Democratic Peoples Republic of Korea country

\def COUNTRY_KOREA_REPUBLIC_OF
\brief Republic of Korea country

\def COUNTRY_KUWAIT
\brief Kuwait country

\def COUNTRY_KYRGYZSTAN
\brief Kyrgyzstan country

\def COUNTRY_LAO_PEOPLES_DEMOCRATIC_REPUBLIC
\brief Lao Peoples Democratic Republic country

\def COUNTRY_LATVIA
\brief Latvia country

\def COUNTRY_LEBANON
\brief Lebanon country

\def COUNTRY_LESOTHO
\brief Lesotho country

\def COUNTRY_LIBERIA
\brief Liberia country

\def COUNTRY_LIBYAN_ARAB_JAMAHIRIYA
\brief Libyan Arab Jamahiriya country

\def COUNTRY_LIECHTENSTEIN
\brief Liechtenstein country

\def COUNTRY_LITHUANIA
\brief Lithuania country

\def COUNTRY_LUXEMBOURG
\brief Luxembourg country

\def COUNTRY_MACAU
\brief Macau country

\def COUNTRY_MACEDONIA_THE_FORMER_YUGOSLAV_REPUBLIC_OF
\brief The Former Yugoslav Republic of Macedonia country

\def COUNTRY_MADAGASCAR
\brief Madagascar country

\def COUNTRY_MALAWI
\brief Malawi country

\def COUNTRY_MALAYSIA
\brief Malaysia country

\def COUNTRY_MALDIVES
\brief Maldives country

\def COUNTRY_MALI
\brief Mali country

\def COUNTRY_MALTA
\brief Malta country

\def COUNTRY_MARSHALL_ISLANDS
\brief Marshall Islands country

\def COUNTRY_MARTINIQUE
\brief Martinique country

\def COUNTRY_MAURITANIA
\brief Mauritania country

\def COUNTRY_MAURITIUS
\brief Mauritius country

\def COUNTRY_MAYOTTE
\brief Mayotte country

\def COUNTRY_MEXICO
\brief Mexico country

\def COUNTRY_MICRONESIA_FEDERATED_STATES_OF
\brief Federated States of Micronesia country

\def COUNTRY_MOLDOVA_REPUBLIC_OF
\brief Republic of Moldova country

\def COUNTRY_MONACO
\brief Monaco country

\def COUNTRY_MONGOLIA
\brief Mongolia country

\def COUNTRY_MONTENEGRO
\brief Montenegro country

\def COUNTRY_MONTSERRAT
\brief Montserrat country

\def COUNTRY_MOROCCO
\brief Morocco country

\def COUNTRY_MOZAMBIQUE
\brief Mozambique country

\def COUNTRY_MYANMAR
\brief Myanmar country

\def COUNTRY_NAMIBIA
\brief Namibia country

\def COUNTRY_NAURU
\brief Nauru country

\def COUNTRY_NEPAL
\brief Nepal country

\def COUNTRY_NETHERLANDS
\brief Netherlands country

\def COUNTRY_NETHERLANDS_ANTILLES
\brief Netherlands Antilles country

\def COUNTRY_NEW_CALEDONIA
\brief New Caledonia country

\def COUNTRY_NEW_ZEALAND
\brief New Zealand country

\def COUNTRY_NICARAGUA
\brief Nicaragua country

\def COUNTRY_NIGER
\brief Niger country

\def COUNTRY_NIGERIA
\brief Nigeria country

\def COUNTRY_NIUE
\brief Niue country

\def COUNTRY_NORFOLK_ISLAND
\brief Norfolk Island country

\def COUNTRY_NORTHERN_MARIANA_ISLANDS
\brief Northern Mariana Islands country

\def COUNTRY_NORWAY
\brief Norway country

\def COUNTRY_OMAN
\brief Oman country

\def COUNTRY_PAKISTAN
\brief Pakistan country

\def COUNTRY_PALAU
\brief Palau country

\def COUNTRY_PALESTINIAN_TERRITORY
\brief Palestinian Territory country

\def COUNTRY_PANAMA
\brief Panama country

\def COUNTRY_PAPUA_NEW_GUINEA
\brief Papua New Guinea country

\def COUNTRY_PARAGUAY
\brief Paraguay country

\def COUNTRY_PERU
\brief Peru country

\def COUNTRY_PHILIPPINES
\brief Philippines country

\def COUNTRY_PITCAIRN
\brief Pitcairn country

\def COUNTRY_POLAND
\brief Poland country

\def COUNTRY_PORTUGAL
\brief Portugal country

\def COUNTRY_PUERTO_RICO
\brief Puerto Rico country

\def COUNTRY_QATAR
\brief Qatar country

\def COUNTRY_REUNION
\brief Reunion country

\def COUNTRY_ROMANIA
\brief Romania country

\def COUNTRY_RUSSIAN_FEDERATION
\brief Russian Federation country

\def COUNTRY_RWANDA
\brief Rwanda country

\def COUNTRY_SAINT_KITTS_AND_NEVIS
\brief Saint Kitts and Nevis country

\def COUNTRY_SAINT_LUCIA
\brief Saint Lucia country

\def COUNTRY_SAINT_VINCENT_AND_THE_GRENADINES
\brief Saint Vincent and the Grenadines country

\def COUNTRY_SAMOA
\brief Samoa country

\def COUNTRY_SAN_MARINO
\brief San Marino country

\def COUNTRY_SAO_TOME_AND_PRINCIPE
\brief Sao Tome and Principe country

\def COUNTRY_SAUDI_ARABIA
\brief Saudi Arabia country

\def COUNTRY_SENEGAL
\brief Senegal country

\def COUNTRY_SERBIA
\brief Serbia country

\def COUNTRY_SERBIA_AND_MONTENEGRO
\brief Serbia and Montenegro country

\def COUNTRY_SEYCHELLES
\brief Seychelles country

\def COUNTRY_SIERRA_LEONE
\brief Sierra Leone country

\def COUNTRY_SINGAPORE
\brief Singapore country

\def COUNTRY_SLOVAKIA
\brief Slovakia country

\def COUNTRY_SLOVENIA
\brief Slovenia country

\def COUNTRY_SOLOMON_ISLANDS
\brief Solomon Islands country

\def COUNTRY_SOMALIA
\brief Somalia country

\def COUNTRY_SOUTH_AFRICA
\brief South Africa country

\def COUNTRY_SOUTH_GEORGIA_AND_THE_SOUTH_SANDWICH_ISLANDS
\brief South Georgia and the South Sandwich Islands country

\def COUNTRY_SPAIN
\brief Spain country

\def COUNTRY_SRI_LANKA
\brief Sri Lanka country

\def COUNTRY_ST_HELENA
\brief St Helena country

\def COUNTRY_ST_PIERRE_AND_MIQUELON
\brief St Pierre and Miquelon country

\def COUNTRY_ST_HELENA_ASCENSION_AND_TRISTAN_DA_CUNHA
\brief St Helena Ascension and Tristan da Cunha country

\def COUNTRY_SUDAN
\brief Sudan country

\def COUNTRY_SURINAME
\brief Suriname country

\def COUNTRY_SVALBARD_AND_JAN_MAYEN_ISLANDS
\brief Svalbard and Jan Mayen Islands country

\def COUNTRY_SWAZILAND
\brief Swaziland country

\def COUNTRY_SWEDEN
\brief Sweden country

\def COUNTRY_SWITZERLAND
\brief Switzerland country

\def COUNTRY_SYRIAN_ARAB_REPUBLIC
\brief Syrian Arab Republic country

\def COUNTRY_TAIWAN
\brief Taiwan country

\def COUNTRY_TAJIKISTAN
\brief Tajikistan country

\def COUNTRY_TANZANIA_UNITED_REPUBLIC_OF
\brief United Republic of Tanzania country

\def COUNTRY_THAILAND
\brief Thailand country

\def COUNTRY_TIMOR_LESTE
\brief Timor Leste country

\def COUNTRY_TOGO
\brief Togo country

\def COUNTRY_TOKELAU
\brief Tokelau country

\def COUNTRY_TONGA
\brief Tonga country

\def COUNTRY_TRINIDAD_AND_TOBAGO
\brief Trinidad and Tobago country

\def COUNTRY_TUNISIA
\brief Tunisia country

\def COUNTRY_TURKEY
\brief Turkey country

\def COUNTRY_TURKMENISTAN
\brief Turkmenistan country

\def COUNTRY_TURKS_AND_CAICOS_ISLANDS
\brief Turks and Caicos Islands country

\def COUNTRY_TUVALU
\brief Tuvalu country

\def COUNTRY_UGANDA
\brief Uganda country

\def COUNTRY_UKRAINE
\brief Ukraine country

\def COUNTRY_UNITED_ARAB_EMIRATES
\brief United Arab Emirates country

\def COUNTRY_UNITED_KINGDOM
\brief United_kingdom country

\def COUNTRY_UNITED_STATES
\brief United States country

\def COUNTRY_UNITED_STATES_MINOR_OUTLYING_ISLANDS
\brief United States Minor Outlying Islands country

\def COUNTRY_URUGUAY
\brief Uruguay country

\def COUNTRY_UZBEKISTAN
\brief Uzbekistan country

\def COUNTRY_VANUATU
\brief Vanuatu country

\def COUNTRY_VATICAN_CITY_STATE
\brief Vatican City State country

\def COUNTRY_VENEZUELA
\brief Venezuela country

\def COUNTRY_VIETNAM
\brief Vietnam country

\def COUNTRY_VIRGIN_ISLANDS_BRITISH
\brief Virgin Islands British country

\def COUNTRY_VIRGIN_ISLANDS_US
\brief Virgin Islands US country

\def COUNTRY_WALLIS_AND_FUTUNA_ISLANDS
\brief Wallis and Futuna Islands country

\def COUNTRY_WESTERN_SAHARA
\brief Western Sahara country

\def COUNTRY_YEMEN
\brief Yemen country

\def COUNTRY_YUGOSLAVIA
\brief Yugoslavia country

\def COUNTRY_ZAIRE
\brief Zaire country

\def COUNTRY_ZAMBIA
\brief Zambia country

\def COUNTRY_ZIMBABWE
\brief Zimbabwe country
*/
