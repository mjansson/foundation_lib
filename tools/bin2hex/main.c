/* main.c  -  Foundation bin2hex tool  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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
	char**       input_files;
	char**       output_files;
	int          columns;
} bin2hex_input_t;

static bin2hex_input_t      bin2hex_parse_command_line( char const* const* cmdline );

static int                  bin2hex_process_files( char const* const* input, char const* const* output, int columns );
static int                  bin2hex_process_file( stream_t* input, stream_t* output, int columns );

static void                 bin2hex_print_usage( void );


int main_initialize( void )
{
	int ret = 0;

	application_t application = {0};
	application.name = "bin2hex";
	application.short_name = "bin2hex";
	application.config_dir = "bin2hex";
	application.flags = APPLICATION_UTILITY;

	log_enable_prefix( false );

	if( ( ret = foundation_initialize( memory_system_malloc(), application ) ) < 0 )
		return ret;

	config_set_int( HASH_FOUNDATION, HASH_TEMPORARY_MEMORY, 32 * 1024 );

	return 0;
}


int main_run( void* main_arg )
{
	int result = BIN2HEX_RESULT_OK;

	bin2hex_input_t input = bin2hex_parse_command_line( environment_command_line() );

	if( !array_size( input.input_files ) )
		bin2hex_print_usage();
	else
	{
		result = bin2hex_process_files( (char const* const*)input.input_files, (char const* const*)input.output_files, input.columns );
		if( result < 0 )
			goto exit;
	}

exit:

	string_array_deallocate( input.input_files );
	string_array_deallocate( input.output_files );

	return result;
}


void main_shutdown( void )
{
	foundation_shutdown();
}


bin2hex_input_t bin2hex_parse_command_line( char const* const* cmdline )
{
	bin2hex_input_t input = {0};
	int arg, asize;

	error_context_push( "parsing command line", "" );
	for( arg = 1, asize = array_size( cmdline ); arg < asize; ++arg )
	{
		if( string_equal( cmdline[arg], "--columns" ) )
		{
			if( arg < ( asize - 1 ) )
				input.columns = string_to_int( cmdline[++arg] );
		}
		else if( string_equal( cmdline[arg], "--" ) )
			break; //Stop parsing cmdline options
		else if( ( string_length( cmdline[arg] ) > 2 ) && string_equal_substr( cmdline[arg], "--", 2 ) )
			continue; //Cmdline argument not parsed here			
		else
		{
			array_push( input.input_files, string_clone( cmdline[arg] ) );
			array_push( input.output_files, string_format( "%s.hex", cmdline[arg] ) );
		}
	}
	error_context_pop();

	return input;
}


int bin2hex_process_files( char const* const* input, char const* const* output, int columns )
{
	int result = BIN2HEX_RESULT_OK;
	unsigned int ifile, files_size;
	for( ifile = 0, files_size = array_size( input ); ( result == BIN2HEX_RESULT_OK ) && ( ifile < files_size ); ++ifile )
	{
		char* input_filename = 0;
		char* output_filename = 0;

		stream_t* input_file = 0;
		stream_t* output_file = 0;

		input_filename = path_clean( string_clone( input[ifile] ), path_is_absolute( input[ifile] ) );
		error_context_push( "parsing file", input_filename );

		output_filename = path_clean( string_clone( output[ifile] ), path_is_absolute( output[ifile] ) );

		log_infof( "bin2hex %s -> %s", input_filename, output_filename );

		input_file = stream_open( input_filename, STREAM_IN | STREAM_BINARY );

		if( !input_file )
		{
			log_warnf( WARNING_BAD_DATA, "Unable to open input file: %s", input_filename );
			result = BIN2HEX_RESULT_MISSING_INPUT_FILE;
		}
		else
		{
			output_file = stream_open( output_filename, STREAM_OUT );
			if( !output_file )
			{
				log_warnf( WARNING_BAD_DATA, "Unable to open output file: %s", output_filename );
				result = BIN2HEX_RESULT_UNABLE_TO_OPEN_OUTPUT_FILE;
			}
		}
		
		if( input_file && output_file )
			result = bin2hex_process_file( input_file, output_file, columns );

		stream_deallocate( input_file );
		stream_deallocate( output_file );

		string_deallocate( output_filename );

		error_context_pop();
		string_deallocate( input_filename );		
	}

	if( ( result == BIN2HEX_RESULT_OK ) && ( files_size > 0 ) )
		log_infof( "All files generated" );

	return result;
}


int bin2hex_process_file( stream_t* input, stream_t* output, int columns )
{
	uint8_t block[512];
	int64_t read, byte;

	if( !columns )
		columns = 32;
	if( columns > 512 )
		columns = 512;

	while( !stream_eos( input ) )
	{
		read = stream_read( input, block, columns );
		if( !read )
			break;

		for( byte = 0; byte < read; ++byte )
			stream_write_format( output, "0x%02x, ", (unsigned int)block[byte] );
		stream_write_endl( output );
	}
	return BIN2HEX_RESULT_OK;
}


void bin2hex_print_usage( void )
{
	log_infof( 
		"bin2hex usage:\n"
		"  bin2hex [--columns n] <file> <file> <file> <...>\n"
		"    Required arguments:\n"
		"      <file>           Input filename (any number of input files allowed). Output will be named \"<file>.hex\"\n"
		"    Optional arguments:\n"
		"      --columns n      Print n bytes in each column (default is 32)\n"
	);
}
