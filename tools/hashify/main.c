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

#include <foundation.h>


#define RESULT_OK                           0
#define RESULT_MISSING_INPUT_FILE          -1
#define RESULT_MISSING_OUTPUT_FILE         -2
#define RESULT_OUTPUT_FILE_OUT_OF_DATE     -3
#define RESULT_HASH_STRING_MISMATCH        -4
#define RESULT_HASH_MISMATCH               -5
#define RESULT_HASH_MISSING                -6
#define RESULT_HASH_COLLISION              -7
#define RESULT_STRING_COLLISION            -8
#define RESULT_EXTRA_STRING                -9

#define HASHIFY_LINEBUFFER_LENGTH           512
#define HASHIFY_STRING_LENGTH               128

typedef struct
{
	bool         check_only;
	char**       strings;
	char**       files;
} hashify_input_t;

typedef struct
{
	char         string[HASHIFY_STRING_LENGTH];
	hash_t       hash;
} hashify_string_t;

static hashify_input_t      hashify_parse_command_line( const char* const* cmdline );

static int                  hashify_process_strings( const char* const* strings );
static int                  hashify_process_files( const char* const* files, bool check_only );

static int                  hashify_process_file( stream_t* input_file, stream_t* output_file, bool check_only, hashify_string_t** history );
static int                  hashify_generate_preamble( stream_t* output_file );
static int                  hashify_read_hashes( stream_t* file, hashify_string_t** hashes );

static int                  hashify_check_local_consistency( const char* string, hash_t hash_value, const hashify_string_t* local_hashes );
static int                  hashify_check_collisions( const char* string, hash_t hash_value, const hashify_string_t* history );
static int                  hashify_check_match( const hashify_string_t* hashes, const hashify_string_t* generated );


int main_initialize( void )
{
	application_t application;
	application.name = "hashify";
	application.short_name = "hashify";
	application.config_dir = "hashify";

	return foundation_initialize( memory_system_malloc(), application );
}


int main_run( void* main_arg )
{
	int result = 0;
	hashify_input_t input = hashify_parse_command_line( environment_command_line() );
	
	result = hashify_process_strings( input.strings );
	if( result < 0 )
		goto exit;

	result = hashify_process_files( input.files, input.check_only );
	if( result < 0 )
		goto exit;

exit:

	string_array_deallocate( input.strings );
	string_array_deallocate( input.files );

	return result;
}


void main_shutdown( void )
{
	foundation_shutdown();
}


hashify_input_t hashify_parse_command_line( const char* const* cmdline )
{
	hashify_input_t input = {0};
	int arg, asize;

	input.check_only = true;

	error_context_push( "parsing command line", "" );
	for( arg = 1, asize = array_size( cmdline ); arg < asize; ++arg )
	{
		if( string_equal( cmdline[arg], "--generate" ) )
		{
			input.check_only = false;
			continue;
		}
		else if( string_equal( cmdline[arg], "--generate-string" ) )
		{
			if( arg < asize - 1 )
			{
				++arg;
				array_push( input.strings, string_clone( cmdline[arg] ) );
			}
			continue;
		}
		else if( string_equal( cmdline[arg], "--" ) )
			break; //Stop parsing cmdline options

		array_push( input.files, string_clone( cmdline[arg] ) );
	}
	error_context_pop();

	return input;
}


int hashify_process_strings( const char* const* strings )
{
	int istr, strings_size;
	for( istr = 0, strings_size = array_size( strings ); istr < strings_size; ++istr )
	{
		uint64_t hash_value = hash( strings[istr], string_length( strings[istr] ) );
		log_infof( "String '%s' hash: 0x%llx", strings[istr], hash_value );
	}
	return 0;
}


int hashify_process_files( const char* const* files, bool check_only )
{
	int result = RESULT_OK;
	hashify_string_t* history = 0;
	unsigned int ifile, files_size;
	for( ifile = 0, files_size = array_size( files ); ( result == RESULT_OK ) && ( ifile < files_size ); ++ifile )
	{
		char* input_filename;
		char* output_filename;

		stream_t* input_file;
		stream_t* output_file;

		input_filename = path_clean( string_clone( files[ifile] ), path_is_absolute( files[ifile] ) );
		error_context_push( "parsing file", input_filename );

		output_filename = string_append( path_base_file_name_with_path( input_filename ), ".h" );

		log_infof( "Hashifying %s -> %s", input_filename, output_filename );

		input_file  = stream_open( input_filename, STREAM_IN );
		output_file = stream_open( output_filename, STREAM_IN | ( check_only ? 0 : STREAM_OUT ) );

		if( !input_file )
		{
			log_warnf( WARNING_BAD_DATA, "Unable to open input file: %s", input_filename );
			result = RESULT_MISSING_INPUT_FILE;
		}
		else if( !output_file )
		{
			log_warnf( WARNING_BAD_DATA, "Unable to open output file: %s", output_filename );
			result = RESULT_MISSING_OUTPUT_FILE;
		}
		else
		{
			result = hashify_process_file( input_file, output_file, check_only, &history );
		}

		stream_deallocate( input_file );
		stream_deallocate( output_file );

		string_deallocate( input_filename );
		string_deallocate( output_filename );
	}

	if( ( result == RESULT_OK ) && ( files_size > 0 ) )
	{
		if( check_only )
			log_infof( "All hashes validated" );
		else
			log_infof( "All hashes generated" );
	}

	array_deallocate( history );

	return result;
}


int hashify_process_file( stream_t* input_file, stream_t* output_file, bool check_only, hashify_string_t** history )
{
	int result = RESULT_OK;
	char def_buffer[HASHIFY_LINEBUFFER_LENGTH];
	char str_buffer[HASHIFY_LINEBUFFER_LENGTH];
	hashify_string_t* local_hashes = 0;
	hashify_string_t* local_generated = 0;

	if( check_only )
		result = hashify_read_hashes( output_file, &local_hashes );
	else
		result = hashify_generate_preamble( output_file );
	
	while( !stream_eos( input_file ) && ( result == RESULT_OK ) )
	{
		stream_read_string_buffer( input_file, def_buffer, HASHIFY_LINEBUFFER_LENGTH );
		stream_read_string_buffer( input_file, str_buffer, HASHIFY_LINEBUFFER_LENGTH );

		if( string_length( def_buffer ) && string_length( str_buffer ) )
		{
			hash_t hash_value = hash( str_buffer, string_length( str_buffer ) );

			log_infof( "  %s: %s -> 0x%llx", def_buffer, str_buffer, hash_value );

			if( check_only )
			{
				//Check local consistency
				result = hashify_check_local_consistency( str_buffer, hash_value, local_hashes );
			}
			else
			{
				stream_write_format( output_file, "#define %s static_hash_string( \"%s\", 0x%llxULL )\n", def_buffer, str_buffer, hash_value );
			}

			if( result == RESULT_OK )
			{
				hashify_string_t hash_string;

				//Check history
				result = hashify_check_collisions( str_buffer, hash_value, *history );

				//Add to history
				string_copy( hash_string.string, str_buffer, HASHIFY_STRING_LENGTH );
				hash_string.hash = hash_value;
				array_push_memcpy( *history, &hash_string );
				array_push_memcpy( local_generated, &hash_string );
			}
		}
		else if( string_length( def_buffer ) || string_length( str_buffer ) )
		{
			log_warnf( WARNING_BAD_DATA, "  invalid line encountered in input file: %s %s", def_buffer, str_buffer );
		}
	}

	if( check_only )
	{
		//Check local consistency
		result = hashify_check_match( local_hashes, local_generated );
	}

	array_deallocate( local_hashes );
	array_deallocate( local_generated );

	return result;
}


int hashify_generate_preamble( stream_t* output_file )
{
	//Read and preserve everything before #pragma once in case it contains header comments to be preserved
	char line_buffer[HASHIFY_LINEBUFFER_LENGTH];
	char* preamble = 0;

	while( !stream_eos( output_file ) )
	{
		char* line;
		uint64_t read;

		read = stream_read_line_buffer( output_file, line_buffer, HASHIFY_LINEBUFFER_LENGTH-1, '\n' );
		line = string_strip( line_buffer, "\n\r" );

		if( ( string_find_string( line, "pragma", 0 ) != STRING_NPOS ) && ( string_find_string( line, "once", 0 ) != STRING_NPOS ) )
			break;

		preamble = string_append( preamble, line );
		if( read < HASHIFY_LINEBUFFER_LENGTH )
			preamble = string_append( preamble, "\n" );
	}

	stream_seek( output_file, 0, STREAM_SEEK_BEGIN );
	stream_write_string( output_file, preamble );
	stream_write_string( output_file,
		"#pragma once\n\n"
		"#include <foundation.h>\n\n"
		"/* ****** AUTOMATICALLY GENERATED, DO NOT EDIT ******\n"
		"    Edit corresponding definitions file and rerun\n"
		"    the foundation hashify tool to update this file */\n\n"
	);

	string_deallocate( preamble );
	
	return 0;
}


int hashify_read_hashes( stream_t* file, hashify_string_t** hashes )
{
	//Read in hashes in file
	char* line;
	char line_buffer[HASHIFY_LINEBUFFER_LENGTH];

	do
	{
		stream_read_line_buffer( file, line_buffer, HASHIFY_LINEBUFFER_LENGTH-1, '\n' );
		line = string_strip( line_buffer, "\n\r" );
		if( ( string_find_string( line, "define", 0 ) != STRING_NPOS ) && ( string_find_string( line, "static_hash", 0 ) != STRING_NPOS ) )
		{
			//Format is: #define HASH_<hashstring> static_hash_string( "<string>", 0x<hashvalue>ULL )
			char** tokens = string_explode( line, " \t", false );

			if( array_size( tokens ) >= 6 )
			{
				hashify_string_t hash_string;

				string_copy( hash_string.string, string_strip( string_strip( tokens[3], "," ), "\"" ), HASHIFY_STRING_LENGTH );
				hash_string.hash = string_to_uint64( tokens[4], true );

				if( hash( hash_string.string, string_length( hash_string.string ) ) != hash_string.hash )
				{
					log_errorf( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE, "  hash output file is out of date, %s is set to 0x%llx but should be 0x%llx ", hash_string.string, hash_string.hash, hash( hash_string.string, string_length( hash_string.string ) ) );
					string_array_deallocate( tokens );
					return RESULT_OUTPUT_FILE_OUT_OF_DATE;
				}

				array_push_memcpy( *hashes, &hash_string );
			}

			string_array_deallocate( tokens );
		}
	} while( !stream_eos( file ) );

	return 0;
}


int hashify_check_local_consistency( const char* string, hash_t hash_value, const hashify_string_t* local_hashes )
{
	int ilocal, localsize;
	for( ilocal = 0, localsize = array_size( local_hashes ); ilocal < localsize; ++ilocal )
	{
		if( local_hashes[ilocal].hash == hash_value )
		{
			if( !string_equal( local_hashes[ilocal].string, string ) )
			{
				log_errorf( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE, "  hash string mismatch, \"%s\" with hash 0x%llx stored in output file, read \"%s\" from input file", local_hashes[ilocal].string, local_hashes[ilocal].hash, string );
				return RESULT_HASH_STRING_MISMATCH;
			}
			break;
		}
		else if( string_equal( local_hashes[ilocal].string, string ) )
		{
			log_errorf( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE, "  hash mismatch, \"%s\" with hash 0x%llx stored in output file, read \"%s\" with hash 0x%llx from input file", local_hashes[ilocal].string, local_hashes[ilocal].hash, string, hash_value );
			return RESULT_HASH_MISMATCH;
		}
	}
	if( ilocal == localsize )
	{
		log_errorf( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE, "  hash missing in output file, \"%s\" with hash 0x%llx ", string, hash_value );
		return RESULT_HASH_MISSING;
	}

	return RESULT_OK;
}


int hashify_check_collisions( const char* string, hash_t hash_value, const hashify_string_t* history )
{
	int ihist, history_size;
	for( ihist = 0, history_size = array_size( history ); ihist < history_size; ++ihist )
	{
		if( history[ihist].hash == hash_value )
		{
			if( string_equal( history[ihist].string, string ) )
			{
				log_errorf( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE, "  global string duplication, \"%s\" ", string );
				return RESULT_STRING_COLLISION;
			}
			else
			{
				log_errorf( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE, "  global hash collision, 0x%llx between: \"%s\" and \"%s\" ", hash_value, string, history[ihist].string );
				return RESULT_HASH_COLLISION;
			}
		}
	}

	return 0;
}


int hashify_check_match( const hashify_string_t* hashes, const hashify_string_t* generated )
{
	//From hashify_check_local_consistency we know that generated array already is a subset of hashes
	//This test checks that hashes is a subset of generated, i.e sets are equal
	int ihash, hashes_size, igen, generated_size;
	for( ihash = 0, hashes_size = array_size( hashes ); ihash < hashes_size; ++ihash )
	{
		for( igen = 0, generated_size = array_size( generated ); igen < generated_size; ++igen )
		{
			if( hashes[ihash].hash == generated[igen].hash )
			{
				if( !string_equal( hashes[ihash].string, generated[igen].string ) )
				{
					log_errorf( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE, "  hash string mismatch, \"%s\" with hash 0x%llx stored in output file, generated by \"%s\" from input file", hashes[ihash].string, hashes[ihash].hash, generated[igen].string );
					return RESULT_HASH_STRING_MISMATCH;
				}
				break;
			}
			else if( string_equal( hashes[ihash].string, generated[igen].string ) )
			{
				log_errorf( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE, "  hash mismatch, \"%s\" with hash 0x%llx stored in output file, \"%s\" generated hash 0x%llx from input file", hashes[ihash].string, hashes[ihash].hash, generated[igen].string, generated[igen].hash );
				return RESULT_HASH_MISMATCH;
			}
		}
		if( igen == generated_size )
		{
			log_errorf( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE, "  extra hash \"%s\" with hash 0x%llx not found in input file", hashes[ihash].string, hashes[ihash].hash );
			return RESULT_EXTRA_STRING;
		}
	}
	return RESULT_OK;
}

