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
	const char* const* cmdline;
	hash_t* history_hash = 0;
	char** history_string = 0;
	char def_buffer[1024];
	char str_buffer[1024];
	char* output_filename;
	const char* input_filename;
	stream_t* input;
	stream_t* output;
	char* preamble;
	hash_t hash_value;

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

	for( arg = 1, asize = array_size( files ); arg < asize; ++arg )
	{
		input_filename = files[arg];
		error_context_push( "Parsing file", input_filename );

		output_filename = string_append( path_base_file_name_with_path( input_filename ), ".h" );

		debug_logf( "Hashifying %s -> %s", input_filename, output_filename );

		input  = stream_open( input_filename, STREAM_IN );
		output = _hashify_check_only ? 0 : stream_open( output_filename, STREAM_IN | STREAM_OUT );

		if( !input )
		{
			warn_logf( WARNING_BAD_DATA, "Unable to open input file: %s", input_filename );
			goto next;
		}

		if( !_hashify_check_only )
		{
			preamble = 0;
			if( output )
			{
				//Read and preserve everything before #pragma once
				do
				{
					char* line = stream_read_line( output, '\n' );
					preamble = string_append( preamble, line );
					if( ( string_find_string( line, "pragma", 0 ) != STRING_NPOS ) && ( string_find_string( line, "once", 0 ) != STRING_NPOS ) )
						break;
				} while( !stream_eos( output ) );
					
				stream_deallocate( output );
				output = stream_open( output_filename, STREAM_OUT );
			}
			else
			{
				preamble = string_clone( "#pragma once\n" );
				output = stream_open( output_filename, STREAM_OUT );
			}

			if( !output && !_hashify_check_only )
			{
				warn_logf( WARNING_BAD_DATA, "Unable to open output file: %s", output_filename );
				goto next;
			}

			stream_write_string( output, preamble );
			stream_write_endl( output );
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

				//Check history
				for( ihist = 0, histsize = array_size( history_hash ); ihist < histsize; ++ihist )
				{
					if( history_hash[ihist] == hash_value )
					{
						error_logf( ERRORLEVEL_ERROR, ERROR_INVALID_VALUE, "Hash collision, 0x%llx between: \"%s\" and \"%s\" ", hash_value, str_buffer, history_string[ihist] );
						if( _hashify_check_only )
							goto exit;
					}
				}

				array_push( history_hash, hash_value );
				array_push( history_string, string_clone( str_buffer ) );
			}
			else if( string_length( def_buffer ) || string_length( str_buffer ) )
			{
				warn_logf( WARNING_BAD_DATA, "  invalid line encountered: %s %s", def_buffer, str_buffer );
			}
		}
		
next:
		stream_deallocate( input );
		stream_deallocate( output );
		string_deallocate( output_filename );

		input = 0;
		output = 0;
		output_filename = 0;

		error_context_pop();
	}

exit:

	string_deallocate( preamble );
	stream_deallocate( input );
	stream_deallocate( output );
	string_deallocate( output_filename );

	string_array_deallocate( history_string );
	array_deallocate( history_hash );

	string_array_deallocate( files );

	foundation_shutdown();

	return 0;
}
