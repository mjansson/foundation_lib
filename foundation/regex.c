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


#define REGEXERR_OK                 0
#define REGEXERR_TOO_LONG          -1


typedef enum
{
	REGEXOP_BEGIN_CAPTURE,
	REGEXOP_END_CAPTURE,
	REGEXOP_BEGINNING_OF_LINE,
	REGEXOP_END_OF_LINE
} regex_op_t;


struct _foundation_regex
{
	unsigned int    num_captures;
	unsigned int    code_length;
	unsigned int    code_allocated;
	uint8_t         code[];
};


static int _regex_emit( regex_t** target, bool allow_grow, int ops, ... )
{
	if( (*target)->code_length >= (*target)->code_allocated )
	{
		int new_allocated;
		if( !allow_grow )
			return REGEXERR_TOO_LONG;
		
		new_allocated = (*target)->code_allocated << 1;
		*target = memory_reallocate( *target, sizeof( regex_t ) + new_allocated, 0, sizeof( regex_t ) + (*target)->code_allocated );
		(*target)->code_allocated = new_allocated;
	}
	
	(*target)->code[ (*target)->code_length++ ] = op;
	
	return REGEXERR_OK;
}


static int _regex_parse( regex_t** target, const char* pattern, bool allow_grow )
{
	int ret = 0;
	int last_op = 0;
	
	do switch( *pattern )
	{
		case '^':
		{
			++pattern;
			if( ( ret = _regex_emit( target, allow_grow, 1, REGEXOP_BEGINNING_OF_LINE ) ) )
				return ret;
			break;
		}
			
		case '$':
		{
			++pattern;
			if( ( ret = _regex_emit( target, allow_grow, 1, REGEXOP_END_OF_LINE ) ) )
				return ret;
			break;
		}
			
		case '(':
		{
			last_op = (*target)->code_length;
			if( ( ret = _regex_emit( target, REGEXOP_END_OF_LINE, allow_grow ) ) )
				return ret;
			++(*target)->num_captures;
			
			break;
		}
		
		default:
		{
			//Exact match...
			break;
		}
	} while( true );
}


regex_t* regex_compile( const char* pattern )
{
	unsigned int pattern_length = string_length( pattern );
	regex_t* compiled = memory_allocate_context( HASH_STRING, sizeof( regex_t ) + pattern_length + 1, 0, MEMORY_PERSISTENT );
	
	compiled->num_captures = 0;
	compiled->code_length = 0;
	compiled->code_allocated = pattern_length + 1;
	
	_regex_parse( &compiled, pattern, true );
	
	return compiled;
}


bool regex_match( regex_t* regex, const char* input )
{
	return false;
}


void regex_free( regex_t* regex )
{
	memory_deallocate( regex );
}

