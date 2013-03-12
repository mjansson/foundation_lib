/* main.c  -  Foundation library tools  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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

#include "errorcodes.h"

typedef struct
{
	uint32_t       data1;
	uint16_t       data2;
	uint16_t       data3;
	uint8_t        data4[8];
} uuid_raw_t;

typedef struct
{
	uint32_t       time_low;
	uint16_t       time_mid;
	uint16_t       time_hi_and_version;
	uint8_t        clock_seq_hi_and_reserved;
	uint8_t        clock_seq_low;
	uint8_t        node[6];
} uuid_time_t;

typedef union
{
	uuid_raw_t     uuid;
	uint128_t      rnd;
} uuid_random_t;

typedef union
{
	uuid_raw_t     raw;
	uuid_time_t    time;
	uuid_random_t  random;
	uint128_t      u128;
} uuid_t;

typedef enum
{
	METHOD_RANDOM = 0,
	METHOD_TIME,
	METHOD_NAMESPACE_MD5
} uuid_method_t;

typedef struct
{
	uuid_method_t     method;

	//For random and time based
	unsigned int      num;

	//For name based
	uuid_t            namespace;
	char*             name;
} uuid_instance_t;

typedef struct
{
	bool              display_help;
	bool              output_lowercase;
	bool              output_binary;
	char*             output_file;
	uuid_instance_t*  generate;
} uuidgen_input_t;

//682EAE88-339A-41B6-B8E3-997DAA0466D4
static uuid_raw_t           uuid_namespace_dns = { 0x682eae88, 0x339a, 0x41b6, 0xb8, 0xe3, 0x99, 0x7d, 0xaa, 0x4, 0x66, 0xd4 };

static uuidgen_input_t      uuidgen_parse_command_line( const char* const* cmdline );

static int                  uuidgen_generate( uuid_t** uuid, const uuid_instance_t input );
static int                  uuidgen_generate_namespace_md5( uuid_t** uuid, const uuid_t namespace, const char* name );
static int                  uuidgen_generate_random( uuid_t** uuid );
static int                  uuidgen_generate_time( uuid_t** uuid );

static int                  uuidgen_output( uuid_t* uuid, const char* output, bool binary, bool lowercase );

static void                 uuidgen_print_usage( void );

static uuid_t               string_to_uuid( const char* str );


int main_initialize( void )
{
	application_t application = {0};
	application.name = "uuidgen";
	application.short_name = "uuidgen";
	application.config_dir = "uuidgen";

	config_set_int( HASH_FOUNDATION, HASH_TEMPORARY_MEMORY, 32 * 1024 );

	log_enable_prefix( false );
	log_suppress( ERRORLEVEL_ERROR );

	return foundation_initialize( memory_system_malloc(), application );
}


int main_run( void* main_arg )
{
	int result = 0;
	int iinst, num_instance;
	uuid_t* output = 0;
	uuidgen_input_t input = uuidgen_parse_command_line( environment_command_line() );
	
	//--md5 dns com.rampantpixels.foundation.uuidgen -> BD63FB1E-2EEA-35EB-940F-1042E6008A95

	for( iinst = 0, num_instance = array_size( input.generate ); iinst < num_instance; ++iinst )
	{
		result = uuidgen_generate( &output, input.generate[iinst] );
		if( result < 0 )
			goto exit;
	}

	result = uuidgen_output( output, input.output_file, input.output_binary, input.output_lowercase );
	if( result < 0 )
		goto exit;

exit:

	if( input.display_help )
		uuidgen_print_usage();

	for( iinst = 0, num_instance = array_size( input.generate ); iinst < num_instance; ++iinst )
		string_deallocate( input.generate[iinst].name );
	array_deallocate( input.generate );
	array_deallocate( output );
	string_deallocate( input.output_file );

	return result;
}


void main_shutdown( void )
{
	foundation_shutdown();
}


uuidgen_input_t uuidgen_parse_command_line( const char* const* cmdline )
{
	uuidgen_input_t input = {0};
	int arg, asize;

	error_context_push( "parsing command line", "" );
	for( arg = 1, asize = array_size( cmdline ); arg < asize; ++arg )
	{
		if( string_equal( cmdline[arg], "--help" ) )
			input.display_help = true;
		else if( string_equal( cmdline[arg], "--output" ) )
		{
			if( arg < asize - 1 )
			{
				++arg;
				string_deallocate( input.output_file );
				input.output_file = string_clone( cmdline[arg] );
			}
		}
		else if( string_equal( cmdline[arg], "--binary" ) )
		{
			input.output_binary = true;
		}
		else if( string_equal( cmdline[arg], "--lowercase" ) )
		{
			input.output_lowercase = true;
		}
		else if( string_equal( cmdline[arg], "--uppercase" ) )
		{
			input.output_lowercase = false;
		}
		else if( string_equal( cmdline[arg], "--random" ) )
		{
			uuid_instance_t instance = {0};
			instance.method = METHOD_RANDOM;
			if( ( arg < ( asize - 1 ) ) && ( cmdline[arg+1][0] != '-' ) )
			{
				++arg;
				instance.num = string_to_uint( cmdline[arg], false );
			}
			if( instance.num < 1 )
				instance.num = 1;
			array_push_memcpy( input.generate, &instance );
		}
		else if( string_equal( cmdline[arg], "--time" ) )
		{
			uuid_instance_t instance = {0};
			instance.method = METHOD_TIME;
			if( ( arg < ( asize - 1 ) ) && ( cmdline[arg+1][0] != '-' ) )
			{
				++arg;
				instance.num = string_to_uint( cmdline[arg], false );
			}
			if( instance.num < 1 )
				instance.num = 1;
			array_push_memcpy( input.generate, &instance );
		}
		else if( string_equal( cmdline[arg], "--md5" ) )
		{
			uuid_instance_t instance = {0};
			instance.method = METHOD_NAMESPACE_MD5;
			if( arg < ( asize - 1 ) )
			{
				++arg;
				if( string_equal( cmdline[arg], "dns" ) || string_equal( cmdline[arg], "DNS" ) )
					instance.namespace.raw = uuid_namespace_dns;
				else
					instance.namespace = string_to_uuid( cmdline[arg] );
			}
			if( arg < ( asize - 1 ) )
			{
				++arg;
				instance.name = string_clone( cmdline[arg] );
			}
			array_push_memcpy( input.generate, &instance );
		}
		else if( string_equal( cmdline[arg], "--" ) )
			break; //Stop parsing cmdline options
		else
		{
			//Unknown argument, display help
			input.display_help = true;
		}
	}
	error_context_pop();

	if( !array_size( input.generate ) && !input.display_help )
	{
		//Default to one random-based UUID
		uuid_instance_t instance = {0};
		instance.method = METHOD_RANDOM;
		instance.num = 1;
		array_push_memcpy( input.generate, &instance );
	}

	return input;
}


int uuidgen_generate( uuid_t** uuid, const uuid_instance_t input )
{
	int result = UUIDGEN_RESULT_OK;
	switch( input.method )
	{
		case METHOD_RANDOM:
		{
			unsigned int ii = 0;
			for( ; ii < input.num; ++ii )
			{
				result = uuidgen_generate_random( uuid );
				if( result != UUIDGEN_RESULT_OK )
					break;
			}
			break;
		}

		case METHOD_TIME:
		{
			unsigned int ii = 0;
			for( ; ii < input.num; ++ii )
			{
				result = uuidgen_generate_time( uuid );
				if( result != UUIDGEN_RESULT_OK )
					break;
			}
			break;
		}

		case METHOD_NAMESPACE_MD5:
		{
			result = uuidgen_generate_namespace_md5( uuid, input.namespace, input.name );
			break;
		}

		default:
		{
			result = UUIDGEN_RESULT_UNKNOWN_METHOD;
			break;
		}
	}

	return result;
}


int uuidgen_generate_random( uuid_t** uuid )
{
	uuid_random_t random_uuid;
	random_uuid.rnd = uint128_make( random64(), random64() );

	//Add variant and version
	random_uuid.uuid.data3 &= 0x0FFF;
	random_uuid.uuid.data3 |= 0x4000;
	random_uuid.uuid.data4[0] &= 0x3F;
	random_uuid.uuid.data4[0] |= 0x80;

	array_push_memcpy( *uuid, &random_uuid.uuid );

	return UUIDGEN_RESULT_OK;
}


int uuidgen_generate_time( uuid_t** uuid )
{
	uuid_time_t time_uuid;
	tick_t current_time;
	int in = 0;
	uint32_t clock_seq = 0;
	uint64_t host_id = 0;
	static tick_t last_tick = 0;
	static tick_t tick_local = 0;

	do
	{
		current_time = time_system();
		if( current_time == last_tick )
		{
			if( ++tick_local < 1024 )
				break;
			thread_sleep( 1 );
		}
		else
		{
			tick_local = 0;
		}
	} while( current_time == last_tick );
	last_tick = current_time;
	current_time = current_time + tick_local;

	current_time = ( current_time * 10000ULL ) + 0x01B21DD213814000ULL; //Convert to 100ns since UUID UTC base time, October 15 1582

	//We have no state so clock sequence is random
	clock_seq = random32();

	time_uuid.time_low = (uint32_t)( current_time & 0xFFFFFFFFULL );
	time_uuid.time_mid = (uint16_t)( ( current_time >> 32ULL ) & 0xFFFFULL );
	time_uuid.time_hi_and_version = (uint16_t)( current_time >> 48ULL );
	time_uuid.clock_seq_low = ( clock_seq & 0xFF );
	time_uuid.clock_seq_hi_and_reserved = ( ( clock_seq & 0x3F00 ) >> 8 );

	//If hardware node ID, use random and set identifier (multicast) bit
	host_id = system_hostid();
	if( host_id )
	{
		for( in = 0; in < 6; ++in )
			time_uuid.node[5-in] = (uint8_t)( ( host_id >> ( 8ULL * in ) ) & 0xFF );
	}
	else
	{
		for( in = 0; in < 6; ++in )
			time_uuid.node[in] = (uint8_t)( random32() & 0xFF );
		time_uuid.node[0] |= 0x01;
	}

	//Add variant and version
	time_uuid.time_hi_and_version &= 0x0FFF;
	time_uuid.time_hi_and_version |= ( 1 << 12 );
	time_uuid.clock_seq_hi_and_reserved &= 0x3F;
	time_uuid.clock_seq_hi_and_reserved |= 0x80;

	array_push_memcpy( *uuid, &time_uuid );

	return UUIDGEN_RESULT_OK;
}


int uuidgen_generate_namespace_md5( uuid_t** uuid, const uuid_t namespace, const char* name )
{
	//v3 uuid, namespace and md5
	md5_t* md5;
	uuid_raw_t namespace_id;
	uuid_raw_t gen_uuid;
	uint128_t digest;

	//Namespace in network byte order
	namespace_id = namespace.raw;
	namespace_id.data1 = byteorder_bigendian32( namespace_id.data1 );
	namespace_id.data2 = byteorder_bigendian16( namespace_id.data2 );
	namespace_id.data3 = byteorder_bigendian16( namespace_id.data3 );

	md5 = md5_allocate();
	md5_initialize( md5 );
	md5_digest_raw( md5, &namespace_id, sizeof( namespace_id ) );
	md5_digest( md5, name );
	md5_finalize( md5 );

	//Convert to host order
	digest = md5_get_digest_raw( md5 );
	memcpy( &gen_uuid, &digest, sizeof( uuid_raw_t ) );
	gen_uuid.data1 = byteorder_bigendian32( gen_uuid.data1 );
	gen_uuid.data2 = byteorder_bigendian16( gen_uuid.data2 );
	gen_uuid.data3 = byteorder_bigendian16( gen_uuid.data3 );

	//Add variant and version
	gen_uuid.data3 &= 0x0FFF;
	gen_uuid.data3 |= ( 3 << 12 ); //Variant 3 for MD5
	gen_uuid.data4[0] &= 0x3F;
	gen_uuid.data4[0] |= 0x80;

	array_push_memcpy( *uuid, &gen_uuid );

	return UUIDGEN_RESULT_OK;
}


int uuidgen_output( uuid_t* uuid, const char* output, bool binary, bool lowercase )
{
	if( output )
	{
		int i, uuidsize;
		stream_t* stream = stream_open( output, STREAM_OUT | ( binary ? STREAM_BINARY : 0 ) );
		if( !stream )
			return UUIDGEN_RESULT_UNABLE_TO_OPEN_OUTPUT_FILE;
		for( i = 0, uuidsize = array_size( uuid ); i < uuidsize; ++i )
		{
			if( binary )
				stream_write( stream, uuid + i, sizeof( uuid[i] ) );
			else
			{
				stream_write_format( stream, lowercase ? "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x" : "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", uuid[i].raw.data1, uuid[i].raw.data2, uuid[i].raw.data3, uuid[i].raw.data4[0], uuid[i].raw.data4[1], uuid[i].raw.data4[2], uuid[i].raw.data4[3], uuid[i].raw.data4[4], uuid[i].raw.data4[5], uuid[i].raw.data4[6], uuid[i].raw.data4[7] );
				stream_write_endl( stream );
			}
		}
		stream_deallocate( stream );
	}
	else
	{
		int i, uuidsize;
		log_suppress( ERRORLEVEL_DEBUG );
		for( i = 0, uuidsize = array_size( uuid ); i < uuidsize; ++i )
			log_infof( lowercase ? "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x" : "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", uuid[i].raw.data1, uuid[i].raw.data2, uuid[i].raw.data3, uuid[i].raw.data4[0], uuid[i].raw.data4[1], uuid[i].raw.data4[2], uuid[i].raw.data4[3], uuid[i].raw.data4[4], uuid[i].raw.data4[5], uuid[i].raw.data4[6], uuid[i].raw.data4[7] );
	}
	return UUIDGEN_RESULT_OK;
}


static void uuidgen_print_usage( void )
{
	log_suppress( ERRORLEVEL_DEBUG );
	log_infof( 
		"uuidgen usage:\n"
		"  uuidgen [--time n] [--random n] [--md5 <namespace> <name>] [--output <filename>] [--help]\n"
		"    If no arguments are given, one random-based UUID is output to stdout\n"
		"    Optional arguments:\n"
		"      --time n                     Generate n time-based UUIDs\n"
		"      --random n                   Generate n random-based UUIDs\n"
		"      --md5 <namespace> <name>     Generate a name-based UUID using the namespace UUID specified\n"
		"                                   in <namespace> and a name string specified in <name>\n"
		"      --output <filename>          Output to <filename> instead of stdout\n"
		"      --binary                     Output binary data instead of ASCII (stdout is always ASCII)\n"
		"      --lowercase                  Output UUID in lowercase hex\n"
		"      --uppercase                  Output UUID in uppercase hex (default)\n"
		"      --help                       Show this message"
	);
}


#include <stdio.h>

static uuid_t string_to_uuid( const char* str )
{
	uuid_t uuid = {0};
	unsigned int data1;
	unsigned int data2;
	unsigned int data3;
	unsigned int data4[8];
	int i;
#if FOUNDATION_PLATFORM_WINDOWS
	sscanf_s( str, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", &data1, &data2, &data3, &data4[0], &data4[1], &data4[2], &data4[3], &data4[4], &data4[5], &data4[6], &data4[7] );
#else
	sscanf( str, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", &data1, &data2, &data3, &data4[0], &data4[1], &data4[2], &data4[3], &data4[4], &data4[5], &data4[6], &data4[7] );
#endif
	uuid.raw.data1 = data1;
	uuid.raw.data2 = data2;
	uuid.raw.data3 = data3;
	for( i = 0; i < 8; ++i )
		uuid.raw.data4[i] = data4[i];
	return uuid;
}
