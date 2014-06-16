/* regex.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson / Rampant Pixels
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


#define REGEXERR_OK                    0
#define REGEXERR_TOO_LONG             -1
#define REGEXERR_MISMATCHED_CAPTURES  -2

#define REGEXRES_INTERNAL_FAILURE     -1
#define REGEXRES_NOMATCH               0
#define REGEXRES_MATCH                 1


typedef enum
{
	REGEXOP_BEGIN_CAPTURE,
	REGEXOP_END_CAPTURE,
	REGEXOP_BEGINNING_OF_LINE,
	REGEXOP_END_OF_LINE,
	REGEXOP_EXACT_MATCH
} regex_op_t;


struct _foundation_regex
{
	unsigned int    num_captures;
	unsigned int    code_length;
	unsigned int    code_allocated;
	uint8_t         code[];
};


static const char* REGEX_META_CHARACTERS = "^$()";


static int _regex_emit( regex_t** target, bool allow_grow, int ops, ... )
{
	int iop;
	va_list arglist;
	
	if( (*target)->code_length + ops >= (*target)->code_allocated )
	{
		int new_allocated;
		if( !allow_grow )
			return REGEXERR_TOO_LONG;
		
		new_allocated = ( (*target)->code_allocated << 1 ) + ops;
		*target = memory_reallocate( *target, sizeof( regex_t ) + new_allocated, 0, sizeof( regex_t ) + (*target)->code_allocated );
		(*target)->code_allocated = new_allocated;
	}
	
	va_start( arglist, ops );

	for( iop = 0; iop < ops; ++iop )
		(*target)->code[ (*target)->code_length++ ] = va_arg( arglist, int );
	
	va_end( arglist );
	
	return REGEXERR_OK;
}


static int _regex_emit_buffer( regex_t** target, bool allow_grow, int ops, const char* buffer )
{
	int iop;
	
	if( (*target)->code_length + ops >= (*target)->code_allocated )
	{
		int new_allocated;
		if( !allow_grow )
			return REGEXERR_TOO_LONG;
		
		new_allocated = ( (*target)->code_allocated << 1 ) + ops;
		*target = memory_reallocate( *target, sizeof( regex_t ) + new_allocated, 0, sizeof( regex_t ) + (*target)->code_allocated );
		(*target)->code_allocated = new_allocated;
	}
	
	for( iop = 0; iop < ops; ++iop )
		(*target)->code[ (*target)->code_length++ ] = buffer[iop];
	
	return REGEXERR_OK;
}


static int _regex_parse( regex_t** target, const char** pattern, bool allow_grow, int level )
{
	int ret = 0;
	
	do switch( *(*pattern)++ )
	{
		case '^':
		{
			if( ( ret = _regex_emit( target, allow_grow, 1, REGEXOP_BEGINNING_OF_LINE ) ) )
				return ret;
			break;
		}
			
		case '$':
		{
			if( ( ret = _regex_emit( target, allow_grow, 1, REGEXOP_END_OF_LINE ) ) )
				return ret;
			break;
		}
			
		case '(':
		{
			int capture = (*target)->num_captures++;
			
			if( ( ret = _regex_emit( target, allow_grow, 2, REGEXOP_BEGIN_CAPTURE, capture ) ) )
				return ret;

			_regex_parse( target, pattern, allow_grow, level + 1 );

			if( **pattern != ')' )
				return REGEXERR_MISMATCHED_CAPTURES;
			++(*pattern);
			
			if( ( ret = _regex_emit( target, allow_grow, 2, REGEXOP_END_CAPTURE, capture ) ) )
			   return ret;
			   
			break;
		}
			
		case ')':
		{
			--(*pattern);
			if( level == 0 )
				return REGEXERR_MISMATCHED_CAPTURES;
			return REGEXERR_OK;
		}
		
		default:
		{
			//Exact match
			int matchlen = 0;
			const char* matchstart = --(*pattern);
			while( **pattern && ( string_find( REGEX_META_CHARACTERS, **pattern, 0 ) == STRING_NPOS ) )
			{
				++matchlen;
				++(*pattern);
			}

			if( ( ret = _regex_emit( target, allow_grow, 2, REGEXOP_EXACT_MATCH, matchlen ) ) )
				return ret;
			if( ( ret = _regex_emit_buffer( target, allow_grow, matchlen, matchstart ) ) )
				return ret;
			
			break;
		}
	} while( **pattern );
	
	return REGEXERR_OK;
}


static int _regex_execute( regex_t* regex, int op, const char* input, int inoffset, int inlength, regex_capture_t* captures, int maxcaptures )
{
	while( op < regex->code_length ) switch( regex->code[op++] )
	{
		case REGEXOP_BEGIN_CAPTURE:
		{
			int capture = regex->code[op++];
			if( captures && ( capture < maxcaptures ) )
				captures[capture].substring = input + inoffset;
			break;
		}
		
		case REGEXOP_END_CAPTURE:
		{
			int capture = regex->code[op++];
			if( captures && ( capture < maxcaptures ) )
				captures[capture].length = (int)pointer_diff( input + inoffset, captures[capture].substring );
			break;
		}
		
		case REGEXOP_BEGINNING_OF_LINE:
		{
			if( inoffset != 0 )
				return REGEXRES_NOMATCH;
			break;
		}
		
		case REGEXOP_END_OF_LINE:
		{
			if( inoffset != inlength )
				return REGEXRES_NOMATCH;
			break;
		}
			
		case REGEXOP_EXACT_MATCH:
		{
			int matchlen = regex->code[op++];
			if( ( matchlen > ( inlength - inoffset ) ) || !string_equal_substr( input + inoffset, (const char*)regex->code + op, matchlen ) )
				return REGEXRES_NOMATCH;
			op += matchlen;
			inoffset += matchlen;
			break;
		}
			
		default:
		{
			log_errorf( 0, ERROR_INTERNAL_FAILURE, "Regex encountered an unsupported op: %02x", (unsigned int)regex->code[op] );
			return REGEXRES_INTERNAL_FAILURE;
		}
	}
	return REGEXRES_MATCH;
}


regex_t* regex_compile( const char* pattern )
{
	unsigned int pattern_length = string_length( pattern );
	regex_t* compiled = memory_allocate_context( HASH_STRING, sizeof( regex_t ) + pattern_length + 1, 0, MEMORY_PERSISTENT );
	
	compiled->num_captures = 0;
	compiled->code_length = 0;
	compiled->code_allocated = pattern_length + 1;
	
	if( _regex_parse( &compiled, &pattern, true, 0 ) == REGEXERR_OK )
		return compiled;
	
	memory_deallocate( compiled );
	
	return 0;
}


bool regex_match( regex_t* regex, const char* input, int inlength, regex_capture_t* captures, int maxcaptures )
{
	int res, iin;
	
	if( !regex || !regex->code_length )
		return true;
	
	if( !inlength )
		inlength = string_length( input );
	
	if( regex->code[0] == REGEXOP_BEGINNING_OF_LINE )
		return _regex_execute( regex, 0, input, 0, inlength, captures, maxcaptures ) > 0;
	
	for( iin = 0; iin < inlength; ++iin )
	{
		if( ( res = _regex_execute( regex, 0, input, iin, inlength, captures, maxcaptures ) ) > 0 )
			return true;
	}
	
	return false;
}


void regex_free( regex_t* regex )
{
	memory_deallocate( regex );
}

