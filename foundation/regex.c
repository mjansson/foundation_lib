/* regex.h  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
 *
 * This library provides a cross-platform foundation library in C11 providing basic support
 * data types and functions to write applications and games in a platform-independent fashion.
 * The latest source code is always available at
 *
 * https://github.com/mjansson/foundation_lib
 *
 * This library is put in the public domain; you can redistribute it and/or modify it without
 * any restrictions.
 */

#include <foundation/foundation.h>

#include <stdarg.h>

/*lint -e773 */

#define REGEXERR_OK (size_t)0
#define REGEXERR_TOO_LONG (size_t) - 1
#define REGEXERR_MISMATCHED_CAPTURES (size_t) - 2
#define REGEXERR_MISMATCHED_BLOCKS (size_t) - 3
#define REGEXERR_INVALID_QUANTIFIER (size_t) - 4
#define REGEXERR_BRANCH_FAILURE (size_t) - 5
#define REGEXERR_INTERNAL_FAILURE (size_t) - 6

#define REGEXRES_INTERNAL_FAILURE (size_t) - 2
#define REGEXRES_NOMATCH (size_t) - 1

#define REGEXCODE_NULL (int16_t)0x0000
#define REGEXCODE_WHITESPACE (int16_t)0x0100
#define REGEXCODE_NONWHITESPACE (int16_t)0x0200
#define REGEXCODE_DIGIT (int16_t)0x0300
#define REGEXCODE_NONDIGIT (int16_t)0x0400

#define REGEXPARSE_NOBRANCH (size_t) - 1

enum {
	REGEXOP_BEGIN_CAPTURE = 0,
	REGEXOP_END_CAPTURE,
	REGEXOP_BEGINNING_OF_LINE,
	REGEXOP_END_OF_LINE,
	REGEXOP_EXACT_MATCH,
	REGEXOP_META_MATCH,
	REGEXOP_ANY,
	REGEXOP_ANY_OF,
	REGEXOP_ANY_BUT,
	REGEXOP_ZERO_OR_MORE,
	REGEXOP_ONE_OR_MORE,
	REGEXOP_ZERO_OR_MORE_SHORTEST,
	REGEXOP_ONE_OR_MORE_SHORTEST,
	REGEXOP_ZERO_OR_ONE,
	REGEXOP_BRANCH,
	REGEXOP_BRANCH_END
};

/*static const char* opname[] = {
    "BEGIN_CAPTURE",
    "END_CAPTURE",
    "BEGINNING_OF_LINE",
    "END_OF_LINE",
    "EXACT_MATCH",
    "META_MATCH",
    "ANY",
    "ANY_OF",
    "ANY_BUT",
    "ZERO_OR_MORE",
    "ONE_OR_MORE",
    "ZERO_OR_MORE_SHORTEST",
    "ONE_OR_MORE_SHORTEST",
    "ZERO_OR_ONE",
    "BRANCH",
    "BRANCH_END"
};*/

struct regex_context_t {
	size_t op;
	size_t inoffset;
};

typedef struct regex_context_t regex_context_t;

static const char REGEX_META_CHARACTERS[] = "^$()[].*+?|\\";

static regex_context_t
regex_execute_single(regex_t* regex, size_t op, const char* input, size_t inoffset, size_t inlength,
                     string_const_t* captures, size_t maxcaptures);

static regex_context_t
regex_execute(regex_t* regex, size_t op, const char* input, size_t inoffset, size_t inlength, string_const_t* captures,
              size_t maxcaptures);

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL regex_context_t
regex_context_nomatch(size_t next_op) {
	return (regex_context_t){next_op, REGEXRES_NOMATCH};
}

static FOUNDATION_FORCEINLINE FOUNDATION_CONSTCALL regex_context_t
regex_context_internal_failure(size_t next_op) {
	return (regex_context_t){next_op, REGEXRES_INTERNAL_FAILURE};
}

static bool
regex_is_hex(const char* p) {
	if (((*p < '0') || (*p > '9')) && ((*p < 'a') || (*p > 'f')) && ((*p < 'A') || (*p > 'F')))
		return false;
	++p;
	if (((*p < '0') || (*p > '9')) && ((*p < 'a') || (*p > 'f')) && ((*p < 'A') || (*p > 'F')))
		return false;
	return true;
}

static uint8_t
regex_parse_hex_digit(const char* p) {
	if ((*p >= '0') && (*p <= '9'))
		return (uint8_t)(*p - '0');
	if ((*p >= 'a') && (*p <= 'f'))
		return (uint8_t)(10 + (*p - 'a'));
	if ((*p >= 'A') && (*p <= 'F'))
		return (uint8_t)(10 + (*p - 'A'));
	return 0;
}

static uint8_t
regex_parse_hex(const char* p) {
	return ((regex_parse_hex_digit(p) << 4) & 0xF0) | (regex_parse_hex_digit(p + 1) & 0xF);
}

static size_t
regex_emit(regex_t** target, bool allow_grow, size_t ops, ...) {
	size_t iop;
	va_list arglist;

	if ((*target)->code_length + ops > (*target)->code_allocated) {
		size_t new_allocated;
		if (!allow_grow)
			return REGEXERR_TOO_LONG;

		new_allocated = ((*target)->code_allocated << 1) + ops;
		*target = memory_reallocate(*target, sizeof(regex_t) + new_allocated, 0,
		                            sizeof(regex_t) + (*target)->code_allocated, 0);
		(*target)->code_allocated = new_allocated;
	}

	va_start(arglist, ops);

	for (iop = 0; iop < ops; ++iop) {
		int val = va_arg(arglist, int);
		if (val > 256)
			return REGEXERR_TOO_LONG;
		(*target)->code[(*target)->code_length++] = (uint8_t)val;
	}

	va_end(arglist);

	return REGEXERR_OK;
}

static size_t
regex_emit_buffer(regex_t** target, bool allow_grow, size_t ops, const uint8_t* buffer) {
	size_t iop;

	if ((*target)->code_length + ops > (*target)->code_allocated) {
		size_t new_allocated;
		if (!allow_grow)
			return REGEXERR_TOO_LONG;

		new_allocated = ((*target)->code_allocated << 1) + ops;
		*target = memory_reallocate(*target, sizeof(regex_t) + new_allocated, 0,
		                            sizeof(regex_t) + (*target)->code_allocated, 0);
		(*target)->code_allocated = new_allocated;
	}

	for (iop = 0; iop < ops; ++iop)
		(*target)->code[(*target)->code_length++] = buffer[iop];

	return REGEXERR_OK;
}

static int16_t
regex_encode_escape(char code) {
	switch (code) {
		case 'n':
			return '\n';
		case 'r':
			return '\r';
		case 't':
			return '\t';
		case '0':
			return REGEXCODE_NULL;
		case 's':
			return REGEXCODE_WHITESPACE;
		case 'S':
			return REGEXCODE_NONWHITESPACE;
		case 'd':
			return REGEXCODE_DIGIT;
		case 'D':
			return REGEXCODE_NONDIGIT;
		default:
			break;
	}
	return code;
}

static bool
regex_match_escape(char c, int code) {
	switch (code) {
		case REGEXCODE_NULL:
			return c == 0;
		case REGEXCODE_WHITESPACE:
			return string_find(STRING_CONST(STRING_WHITESPACE), c, 0) != STRING_NPOS;
		case REGEXCODE_NONWHITESPACE:
			return string_find(STRING_CONST(STRING_WHITESPACE), c, 0) == STRING_NPOS;
		case REGEXCODE_DIGIT:
			return string_find(STRING_CONST("0123456789"), c, 0) != STRING_NPOS;
		case REGEXCODE_NONDIGIT:
			return string_find(STRING_CONST("0123456789"), c, 0) == STRING_NPOS;
		default:
			break;
	}
	return false;
}

static size_t
regex_compile_quantifier(regex_t** target, bool allow_grow, size_t last_code_length, uint8_t quantifier) {
	size_t ret;
	size_t move_size;
	if (((*target)->code[last_code_length] == REGEXOP_EXACT_MATCH) && ((*target)->code[last_code_length + 1] > 1)) {
		uint8_t last_char = (*target)->code[(*target)->code_length - 1];
		// The parser should break an REGEXOP_EXACT_MATCH block at a meta character
		if (!FOUNDATION_VALIDATE((*target)->code[(*target)->code_length - 2] != 0))
			return REGEXERR_INTERNAL_FAILURE;
		--(*target)->code[last_code_length + 1];
		--(*target)->code_length;
		if ((ret = regex_emit(target, allow_grow, 4, (int)quantifier, REGEXOP_EXACT_MATCH, 1, (int)last_char)))
			return ret;
		return REGEXERR_OK;
	}

	move_size = 1 + (*target)->code_length - last_code_length;

	if ((ret = regex_emit(target, allow_grow, 1, 0)))  // Make sure we have buffer space
		return ret;

	memmove((*target)->code + last_code_length + 1, (*target)->code + last_code_length, move_size);
	(*target)->code[last_code_length] = quantifier;

	return REGEXERR_OK;
}

static regex_context_t
regex_consume_longest(regex_t* regex, size_t op, const char* input, size_t inoffset, size_t inlength,
                      string_const_t* captures, size_t maxcaptures) {
	regex_context_t context = {op, inoffset};
	regex_context_t best_context = {(size_t)REGEXRES_NOMATCH, inoffset};
	regex_context_t next_context;

	// TODO: Optimization would be to stack all offsets from execute single, then verify matching
	//      of remaining regex starting with highest saved offset
	/*lint -e{716} */
	while (true) {
		context = regex_execute_single(regex, op, input, context.inoffset, inlength, 0, 0);
		if (context.inoffset > inlength)
			break;
		if (context.op >= regex->code_length)
			return context;

		next_context = regex_execute(regex, context.op, input, context.inoffset, inlength, 0, 0);
		if (next_context.inoffset <= inlength) {
			if (captures)
				regex_execute(regex, context.op, input, context.inoffset, inlength, captures, maxcaptures);
			best_context = next_context;
		}
	}

	// Make sure we return the next op (regex_execute_single returns next op even on failure)
	if (best_context.op > regex->code_length)
		best_context.op = context.op;

	return best_context;
}

static regex_context_t
regex_consume_shortest(regex_t* regex, size_t op, const char* input, size_t inoffset, size_t inlength,
                       string_const_t* captures, size_t maxcaptures) {
	regex_context_t context = {op, inoffset};
	regex_context_t best_context = {(size_t)REGEXRES_NOMATCH, inoffset};
	regex_context_t next_context;

	/*lint -e{716} */
	while (true) {
		context = regex_execute_single(regex, op, input, context.inoffset, inlength, 0, 0);
		if (context.inoffset > inlength)
			break;
		if (context.op >= regex->code_length)
			return context;

		next_context = regex_execute(regex, context.op, input, context.inoffset, inlength, 0, 0);
		if (next_context.inoffset <= inlength) {
			if (captures)
				regex_execute(regex, context.op, input, context.inoffset, inlength, captures, maxcaptures);
			best_context = next_context;
			break;
		}
	}

	// Make sure we return the next op (regex_execute_single returns next op even on failure)
	if (best_context.op > regex->code_length)
		best_context.op = context.op;

	return best_context;
}

static size_t
regex_parse_group(regex_t** target, const char* pattern, size_t offset, size_t length, bool allow_grow) {
	// A single group cannot be more than 256 characters (size is stored in a single byte)
	uint8_t buffer[256];
	size_t buffer_len = 0;
	size_t buffer_maxlen = sizeof(buffer);
	int16_t code;
	bool closed = false;
	int op = REGEXOP_ANY_OF;
	size_t ret = REGEXERR_OK;

	if (pattern[offset] == '^') {
		++offset;
		op = REGEXOP_ANY_BUT;
	}

	while (!closed && !ret && (offset < length)) {
		if (pattern[offset] == ']') {
			if ((ret = regex_emit(target, allow_grow, 2, op, (int)buffer_len)))
				break;
			/*lint -e{603} Buffer is initialized with buffer_len bytes */
			if (buffer_len && (ret = regex_emit_buffer(target, allow_grow, buffer_len, buffer)))
				break;
			offset++;
			closed = true;
			break;
		}

		if (buffer_len >= buffer_maxlen)
			return REGEXERR_TOO_LONG;

		if (pattern[offset] == '\\') {
			++offset;

			if ((offset + 1 < length) && regex_is_hex(pattern + offset)) {
				buffer[buffer_len++] = regex_parse_hex(pattern + offset);
				++offset;
			} else if (offset < length) {
				code = regex_encode_escape(pattern[offset]);
				if (!code || (code > 0xFF)) {
					buffer[buffer_len++] = 0;
					if (buffer_len >= buffer_maxlen)
						return REGEXERR_TOO_LONG;
					buffer[buffer_len++] = (code >> 8) & 0xFF;  // lint !e702
				} else {
					buffer[buffer_len++] = code & 0xFF;
				}
			}
		} else {
			buffer[buffer_len++] = (uint8_t)pattern[offset];
		}
		++offset;
	}

	if (ret)
		return ret;
	if (!closed)
		return REGEXERR_MISMATCHED_BLOCKS;
	return offset;
}

static size_t
regex_parser(regex_t** target, const char* pattern, size_t offset, size_t length, bool allow_grow, size_t level) {
	size_t ret = 0;
	size_t last_code_length = (*target)->code_length;
	size_t branch_begin = (*target)->code_length;
	size_t branch_op = REGEXPARSE_NOBRANCH;
	size_t capture;
	size_t size;
	uint8_t quantifier;
	int16_t code;
	const char* matchstart;

	while (offset < length) {
		switch (pattern[offset++]) {
			case '^':
				if ((ret = regex_emit(target, allow_grow, 1, REGEXOP_BEGINNING_OF_LINE)))
					return ret;
				break;

			case '$':
				if ((ret = regex_emit(target, allow_grow, 1, REGEXOP_END_OF_LINE)))
					return ret;
				break;

			case '(':
				capture = (*target)->capture_count++;
				last_code_length = (*target)->code_length;
				if ((ret = regex_emit(target, allow_grow, 2, REGEXOP_BEGIN_CAPTURE, (int)capture)))
					return ret;

				offset = regex_parser(target, pattern, offset, length, allow_grow, level + 1);

				if ((offset > length) || (pattern[offset - 1] != ')'))
					return REGEXERR_MISMATCHED_CAPTURES;

				if ((ret = regex_emit(target, allow_grow, 2, REGEXOP_END_CAPTURE, (int)capture)))
					return ret;
				break;

			case ')':
				if (branch_op != REGEXPARSE_NOBRANCH)
					(*target)->code[branch_op + 1] = (uint8_t)((*target)->code_length - (branch_op + 2));

				if (level == 0)
					return REGEXERR_MISMATCHED_CAPTURES;
				return offset;

			case '[':
				last_code_length = (*target)->code_length;
				offset = regex_parse_group(target, pattern, offset, length, allow_grow);
				if (offset > length)
					return ret;
				break;

			case '.':
				last_code_length = (*target)->code_length;
				if ((ret = regex_emit(target, allow_grow, 1, REGEXOP_ANY)))
					return ret;
				break;

			case '*':
			case '+':
				quantifier = ((uint8_t)pattern[offset - 1] == '*') ? REGEXOP_ZERO_OR_MORE : REGEXOP_ONE_OR_MORE;

				if (last_code_length >= (*target)->code_length)
					return REGEXERR_INVALID_QUANTIFIER;
				if (((*target)->code[last_code_length] < REGEXOP_EXACT_MATCH) ||
				    ((*target)->code[last_code_length] > REGEXOP_ANY_BUT))
					return REGEXERR_INVALID_QUANTIFIER;

				if ((offset < length) && (pattern[offset] == '?')) {
					quantifier = (quantifier == REGEXOP_ZERO_OR_MORE) ? REGEXOP_ZERO_OR_MORE_SHORTEST :
                                                                        REGEXOP_ONE_OR_MORE_SHORTEST;
					++offset;
				}

				if ((ret = regex_compile_quantifier(target, allow_grow, last_code_length, quantifier)))
					return ret;
				break;

			case '?':
				if (last_code_length >= (*target)->code_length)
					return REGEXERR_INVALID_QUANTIFIER;
				if (((*target)->code[last_code_length] < REGEXOP_EXACT_MATCH) ||
				    ((*target)->code[last_code_length] > REGEXOP_ANY_BUT))
					return REGEXERR_INVALID_QUANTIFIER;

				if ((ret =
				         regex_compile_quantifier(target, allow_grow, last_code_length, (uint8_t)REGEXOP_ZERO_OR_ONE)))
					return ret;
				break;

			case '\\':
				last_code_length = (*target)->code_length;
				if ((offset + 1 < length) && regex_is_hex(pattern + offset)) {
					uint8_t val = regex_parse_hex(pattern + offset);
					offset += 2;
					if ((ret = regex_emit(target, allow_grow, 2, REGEXOP_META_MATCH, (int)val)))
						return ret;
				} else if (offset < length) {
					code = regex_encode_escape(pattern[offset++]);
					if (!code || (code > 0xFF)) {
						/*lint -e{702} */
						if ((ret = regex_emit(target, allow_grow, 3, REGEXOP_META_MATCH, 0, (int)((code >> 8) & 0xFF))))
							return ret;
					} else {
						if ((ret = regex_emit(target, allow_grow, 2, REGEXOP_META_MATCH, (int)code)))
							return ret;
					}
				}
				break;

			case '|':
				if (branch_begin >= (*target)->code_length)
					return REGEXERR_BRANCH_FAILURE;

				size = (*target)->code_length - branch_begin;
				// Make sure we have buffer space
				if ((ret = regex_emit(target, allow_grow, 4, 0, 0, REGEXOP_BRANCH_END, 0)))
					return ret;

				memmove((*target)->code + branch_begin + 2, (*target)->code + branch_begin, size);
				(*target)->code[branch_begin] = REGEXOP_BRANCH;
				(*target)->code[branch_begin + 1] = (size + 2) & 0xFF;
				branch_op = (*target)->code_length - 2;
				break;

			default:
				// Exact match
				size = 0;
				--offset;
				matchstart = pattern + offset;
				while ((offset < length) &&
				       (string_find(STRING_CONST(REGEX_META_CHARACTERS), pattern[offset], 0) == STRING_NPOS)) {
					++size;
					++offset;
				}

				last_code_length = (*target)->code_length;
				if ((ret = regex_emit(target, allow_grow, 2, REGEXOP_EXACT_MATCH, size)))
					return ret;
				if ((ret = regex_emit_buffer(target, allow_grow, size, (const uint8_t*)matchstart)))
					return ret;
				break;
		}
	}

	return offset;
}

// Returns a context with the next op offset to execute and the next input offset
// to read (or error code if no match)
static regex_context_t
regex_execute_single(regex_t* regex, size_t op, const char* input, size_t inoffset, size_t inlength,
                     string_const_t* captures, size_t maxcaptures) {
	regex_context_t context;
	size_t capture;
	char cin, cmatch;
	size_t ibuf, buffer_len;
	size_t matchlen;
	size_t next_op;
	size_t skip;

	switch (regex->code[op++]) {
		case REGEXOP_BEGIN_CAPTURE:
			capture = regex->code[op++];
			if (captures && (capture < maxcaptures))
				captures[capture].str = input + inoffset;
			break;

		case REGEXOP_END_CAPTURE:
			capture = regex->code[op++];
			if (captures && (capture < maxcaptures))
				captures[capture].length = (size_t)pointer_diff(input + inoffset, captures[capture].str);
			break;

		case REGEXOP_BEGINNING_OF_LINE:
			if (inoffset != 0)
				return regex_context_nomatch(op);
			break;

		case REGEXOP_END_OF_LINE:
			if (inoffset != inlength)
				return regex_context_nomatch(op);
			break;

		case REGEXOP_ANY_OF:
			cin = input[inoffset];
			buffer_len = regex->code[op++];

			if (inoffset >= inlength)
				return regex_context_nomatch(op + buffer_len);

			/*lint -e{850} */
			for (ibuf = 0; ibuf < buffer_len; ++ibuf) {
				cmatch = (char)regex->code[op + ibuf];
				if (!cmatch) {
					if (regex_match_escape(cin, regex->code[op + (++ibuf)] << 8))
						break;
				} else if (cin == cmatch)
					break;
			}

			if (ibuf == buffer_len)
				return regex_context_nomatch(op + buffer_len);

			++inoffset;
			op += buffer_len;
			break;

		case REGEXOP_ANY_BUT:
			cin = input[inoffset];
			buffer_len = regex->code[op++];

			if (inoffset >= inlength)
				return regex_context_nomatch(op + buffer_len);

			/*lint -e{850} */
			for (ibuf = 0; ibuf < buffer_len; ++ibuf) {
				cmatch = (char)regex->code[op + ibuf];
				if (!cmatch) {
					if (regex_match_escape(cin, regex->code[op + (++ibuf)] << 8))
						return regex_context_nomatch(op + buffer_len);
				} else if (cin == cmatch)
					return regex_context_nomatch(op + buffer_len);
			}

			++inoffset;
			op += buffer_len;
			break;

		case REGEXOP_ANY:
			if (inoffset < inlength) {
				++inoffset;
				break;
			}
			return regex_context_nomatch(op);

		case REGEXOP_EXACT_MATCH:
			matchlen = regex->code[op++];
			if ((matchlen > (inlength - inoffset)) ||
			    !string_equal(input + inoffset, matchlen, (const char*)regex->code + op, matchlen))
				return regex_context_nomatch(op + matchlen);
			op += matchlen;
			inoffset += matchlen;
			break;

		case REGEXOP_META_MATCH:
			cin = (char)regex->code[op++];
			cmatch = input[inoffset++];
			if (!cin) {
				if (regex_match_escape(cmatch, regex->code[op++] << 8))
					break;
			} else if (cin == cmatch)
				break;
			return regex_context_nomatch(op);

		case REGEXOP_ZERO_OR_MORE:
			context = regex_consume_longest(regex, op, input, inoffset, inlength, captures, maxcaptures);
			inoffset = context.inoffset;
			op = context.op;
			break;

		case REGEXOP_ONE_OR_MORE:
			context = regex_execute_single(regex, op, input, inoffset, inlength, captures, maxcaptures);
			if (context.inoffset > inlength)
				return context;
			context = regex_consume_longest(regex, op, input, context.inoffset, inlength, captures, maxcaptures);
			inoffset = context.inoffset;
			op = context.op;
			break;

		case REGEXOP_ZERO_OR_MORE_SHORTEST:
			context = regex_consume_shortest(regex, op, input, inoffset, inlength, captures, maxcaptures);
			inoffset = context.inoffset;
			op = context.op;
			break;

		case REGEXOP_ONE_OR_MORE_SHORTEST:
			context = regex_execute_single(regex, op, input, inoffset, inlength, captures, maxcaptures);
			if (context.inoffset > inlength)
				return context;
			context = regex_consume_shortest(regex, op, input, context.inoffset, inlength, captures, maxcaptures);
			inoffset = context.inoffset;
			op = context.op;
			break;

		case REGEXOP_ZERO_OR_ONE:
			// Try matching with one, and if that succeeds verify complete match of remaining data
			context = regex_execute_single(regex, op, input, inoffset, inlength, captures, maxcaptures);
			if (context.inoffset <= inlength) {
				regex_context_t maybe_context =
				    regex_execute(regex, context.op, input, context.inoffset, inlength, captures, maxcaptures);
				if (maybe_context.inoffset <= inlength) {
					op = maybe_context.op;
					inoffset = maybe_context.inoffset;
					break;  // Matched with one
				}
			}

			// Failed, try matching remainder with next op (now stored in context.op by
			// regex_execute_single above)
			next_op = context.op;
			context = regex_execute(regex, next_op, input, inoffset, inlength, captures, maxcaptures);
			if (context.inoffset <= inlength) {
				op = context.op;
				inoffset = context.inoffset;
				break;  // Matched with zero
			}
			return regex_context_nomatch(next_op);

		case REGEXOP_BRANCH:
			skip = regex->code[op++];
			context = regex_execute(regex, op, input, inoffset, inlength, captures, maxcaptures);
			if (context.inoffset <= inlength) {
				inoffset = context.inoffset;
				op = context.op;
			} else {
				context = regex_execute(regex, op + skip, input, inoffset, inlength, captures, maxcaptures);
				inoffset = context.inoffset;
				op = context.op;
			}
			break;

		case REGEXOP_BRANCH_END:
			skip = regex->code[op++];
			op += skip;
			break;

		default:
			log_errorf(0, ERROR_INTERNAL_FAILURE, STRING_CONST("Regex encountered an unsupported op: %02x"),
			           (unsigned int)regex->code[op]);
			return regex_context_internal_failure(op);
	}

	context.op = op;
	context.inoffset = inoffset;

	return context;
}

static regex_context_t
regex_execute(regex_t* regex, size_t op, const char* input, size_t inoffset, size_t inlength, string_const_t* captures,
              size_t maxcaptures) {
	regex_context_t context = {op, inoffset};
	while (context.op < regex->code_length) {
		context = regex_execute_single(regex, context.op, input, context.inoffset, inlength, captures, maxcaptures);
		if (context.inoffset > inlength)
			break;
	}
	return context;
}

regex_t*
regex_compile(const char* pattern, size_t pattern_length) {
	regex_t* compiled;
	compiled = memory_allocate(HASH_STRING, sizeof(regex_t) + pattern_length + 16, 0, MEMORY_PERSISTENT);
	compiled->capture_count = 0;
	compiled->code_length = 0;
	compiled->code_allocated = pattern_length + 16;

	if (regex_parser(&compiled, pattern, 0, pattern_length, true, 0) == pattern_length)
		return compiled;

	memory_deallocate(compiled);
	return 0;
}

bool
regex_parse(regex_t* regex, const char* pattern, size_t pattern_length) {
	regex_t* result = regex;
	return (regex_parser(&result, pattern, 0, pattern_length, false, 0) == pattern_length);
}

bool
regex_match(regex_t* regex, const char* input, size_t inlength, string_const_t* captures, size_t maxcaptures) {
	size_t iin;

	if (!regex || !regex->code_length)
		return true;

	if (regex->code[0] == REGEXOP_BEGINNING_OF_LINE) {
		regex_context_t context = regex_execute(regex, 0, input, 0, inlength, captures, maxcaptures);
		if (context.inoffset <= inlength)
			return true;
	} else {
		for (iin = 0; iin < inlength; ++iin) {
			regex_context_t context = regex_execute(regex, 0, input, iin, inlength, captures, maxcaptures);
			if (context.inoffset <= inlength)
				return true;
			if (context.inoffset == (size_t)REGEXRES_INTERNAL_FAILURE)
				return false;
		}
	}

	return false;
}

void
regex_deallocate(regex_t* regex) {
	memory_deallocate(regex);
}
