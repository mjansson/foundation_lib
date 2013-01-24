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


static bool _hashify_check_only = true;


int main()
{
	unsigned int arg = 0, asize = 0, ihist = 0, histsize = 0;
	char** files = 0;
	const char* const* cmdline = 0;
	hash_t* history_hash = 0;
	char** history_string = 0;
	hash_t* current_hash = 0;
	char** current_string = 0;
	char** tokens = 0;
	char* hash_str = 0;
	char* line = 0;
	char def_buffer[1024];
	char str_buffer[1024];
	char* output_filename = 0;
	const char* input_filename = 0;
	stream_t* input = 0;
	stream_t* output = 0;
	char* preamble = 0;
	hash_t hash_value;
	bool pragma_found = false;
	int result = -1;

	application_t application;
	application.name = "hashify";
	application.short_name = "hashify";
	application.config_dir = "hashify";

	if( foundation_initialize( memory_system_malloc(), application ) < 0 )
		return -1;

	cmdline = environment_command_line();

	error_context_push( "parsing command line", "" );
	for( arg = 1, asize = array_size( cmdline ); arg < asize; ++arg )
	{
		if( string_equal( cmdline[arg], "--generate" ) )
		{
			_hashify_check_only = false;
			continue;
		}
		else if( string_equal( cmdline[arg], "--generate-string" ) )
		{
			if( arg < asize - 1 )
			{
				++arg;
				hash_value = hash( cmdline[arg], string_length( cmdline[arg] ) );
				info_logf( "String '%s' hash: 0x%llx", cmdline[arg], hash_value );
			}
			continue;
		}
		else if( string_equal( cmdline[arg], "--" ) )
			break; //Stop parsing cmdline options

		array_push( files, string_clone( cmdline[arg] ) );
	}
	error_context_pop();

	for( arg = 0, asize = array_size( files ); arg < asize; ++arg )
	{
		input_filename = path_clean( files[arg], path_is_absolute( files[arg] ) );
		error_context_push( "parsing file", input_filename );

		output_filename = string_append( path_base_file_name_with_path( input_filename ), ".h" );

		debug_logf( "Hashifying %s -> %s", input_filename, output_filename );

		input  = stream_open( input_filename, STREAM_IN );
		output = stream_open( output_filename, STREAM_IN | ( _hashify_check_only ? 0 : STREAM_OUT ) );

		if( !input )
		{
			warn_logf( WARNING_BAD_DATA, "Unable to open input file: %s", input_filename );
			goto next;
		}

		if( !output && !_hashify_check_only )
		{
			warn_logf( WARNING_BAD_DATA, "Unable to open output file for verification: %s", output_filename );
			goto next;
		}

		if( !_hashify_check_only )
		{
			preamble = 0;
			if( output )
			{
				//Read and preserve everything before #pragma once in case it contains header comments to be preserved
				pragma_found = false;
				do
				{
					line = string_strip( stream_read_line( output, '\n' ), "\n\r" );
					preamble = string_append( string_append( preamble, line ), "\n" );
					if( ( string_find_string( line, "pragma", 0 ) != STRING_NPOS ) && ( string_find_string( line, "once", 0 ) != STRING_NPOS ) )
						pragma_found = true;
					string_deallocate( line ), line = 0;
				} while( !stream_eos( output ) && !pragma_found );

				if( !preamble || !string_length( preamble ) || !pragma_found )
				{
					string_deallocate( preamble );
					preamble = string_clone( "\n#pragma once\n" );
				}

				stream_deallocate( output );
				output = stream_open( output_filename, STREAM_OUT );
			}
			else
			{
				preamble = string_clone( "\n#pragma once\n" );
				output = stream_open( output_filename, STREAM_OUT );
			}

			if( !output && !_hashify_check_only )
			{
				warn_logf( WARNING_BAD_DATA, "Unable to open output file: %s", output_filename );
				goto next;
			}

			stream_write_string( output, preamble );
			stream_write_endl( output );
			stream_write_string( output,
				"#include <foundation.h>\n\n"
				"/* ****** AUTOMATICALLY GENERATED, DO NOT EDIT ******\n"
				"    Edit corresponding definitions file and rerun\n"
				"    the foundation hashify tool to update this file */\n\n"
			);

			string_deallocate( preamble );
			preamble = 0;
		}
		else
		{
			//Read in hashes in file for verification
			do
			{
				line = string_strip( stream_read_line( output, '\n' ), "\n\r" );
				if( ( string_find_string( line, "define", 0 ) != STRING_NPOS ) && ( string_find_string( line, "static_hash", 0 ) != STRING_NPOS ) )
				{
					//Format is: #define HASH_ENGINE static_hash_string( "<string>", 0x<hashvalue>ULL )
					tokens = string_explode( line, " \t", false );
					string_deallocate( line ), line = 0;

					if( array_size( tokens ) >= 6 )
					{
						hash_str = string_strip( string_strip( string_clone( tokens[3] ), "," ), "\"" );
						hash_value = string_to_uint64( tokens[4], true );

						if( hash( hash_str, string_length( hash_str ) ) != hash_value )
						{
							error_logf( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE, "  hash output file is out of date, %s is set to 0x%llx but should be 0x%llx ", hash_str, hash_value, hash( hash_str, string_length( hash_str ) ) );
							string_array_deallocate( tokens );
							goto exit;
						}

						array_push( current_string, hash_str );
						array_push( current_hash, hash_value );
					}
					string_array_deallocate( tokens );
				}
				string_deallocate( line ), line = 0;
			} while( !stream_eos( output ) );
		}

		while( !stream_eos( input ) )
		{
			stream_read_string_buffer( input, def_buffer, 1024 );
			stream_read_string_buffer( input, str_buffer, 1024 );

			if( string_length( def_buffer ) && string_length( str_buffer ) )
			{
				hash_value = hash( str_buffer, string_length( str_buffer ) );

				debug_logf( "  %s: %s -> 0x%llx", def_buffer, str_buffer, hash_value );

				if( !_hashify_check_only )
				{
					stream_write_format( output, "#define %s static_hash_string( \"%s\", 0x%llxULL )\n", def_buffer, str_buffer, hash_value );
				}
				else
				{
					//Check history
					for( ihist = 0, histsize = array_size( current_hash ); ihist < histsize; ++ihist )
					{
						if( current_hash[ihist] == hash_value )
						{
							if( !string_equal( current_string[ihist], str_buffer ) )
							{
								error_logf( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE, "  hash string mismatch, \"%s\" with hash 0x%llx stored in output file, read \"%s\" from input file", current_string[ihist], current_hash[ihist], str_buffer );
								goto exit;
							}
							break;
						}
						else if( string_equal( current_string[ihist], str_buffer ) )
						{
							error_logf( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE, "  hash mismatch, \"%s\" with hash 0x%llx stored in output file, read \"%s\" with hash 0x%llx from input file", current_string[ihist], current_hash[ihist], str_buffer, hash_value );
							goto exit;
						}
					}
					if( ihist == histsize )
					{
						error_logf( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE, "  hash missing in output file, \"%s\" with hash 0x%llx ", str_buffer, hash_value );
						goto exit;
					}
				}

				//Check history
				for( ihist = 0, histsize = array_size( history_hash ); ihist < histsize; ++ihist )
				{
					if( history_hash[ihist] == hash_value )
					{
						error_logf( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE, "  global hash collision, 0x%llx between: \"%s\" and \"%s\" ", hash_value, str_buffer, history_string[ihist] );
						if( _hashify_check_only )
							goto exit;
					}
				}

				array_push( history_hash, hash_value );
				array_push( history_string, string_clone( str_buffer ) );
			}
			else if( string_length( def_buffer ) || string_length( str_buffer ) )
			{
				warn_logf( WARNING_BAD_DATA, "  invalid line encountered in input file: %s %s", def_buffer, str_buffer );
			}
		}
		
next:
		string_array_deallocate( current_string );
		array_deallocate( current_hash );
		stream_deallocate( input );
		stream_deallocate( output );
		string_deallocate( output_filename );

		input = 0;
		output = 0;
		output_filename = 0;
		current_string = 0;
		current_hash = 0;

		error_context_pop();
	}

	if( _hashify_check_only )
		info_logf( "All hashes are up to date" );
	result = 0;

exit:

	string_deallocate( preamble );
	stream_deallocate( input );
	stream_deallocate( output );
	string_deallocate( output_filename );

	string_array_deallocate( current_string );
	array_deallocate( current_hash );

	string_array_deallocate( history_string );
	array_deallocate( history_hash );

	string_array_deallocate( files );

	foundation_shutdown();

	return result;
}
