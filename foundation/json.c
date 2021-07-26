/* json.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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

static json_token_t*
get_token(json_token_t* tokens, size_t capacity, unsigned int index) {
	return index < capacity ? tokens + index : nullptr;
}

static bool
is_valid_token(json_token_t* tokens, size_t capacity, unsigned int index) {
	json_token_t* token = get_token(tokens, capacity, index);
	return token ? (token->type != JSON_UNDEFINED) : true;
}

static void
set_token_primitive(json_token_t* tokens, size_t capacity, unsigned int current, json_type_t type, size_t value,
                    size_t value_length) {
	json_token_t* token = get_token(tokens, capacity, current);
	if (token) {
		token->type = type;
		token->child = 0;
		token->sibling = 0;
		token->value = (unsigned int)value;
		token->value_length = (unsigned int)value_length;
	}
}

static struct json_token_t*
set_token_complex(json_token_t* tokens, size_t capacity, unsigned int current, json_type_t type, size_t pos) {
	json_token_t* token = get_token(tokens, capacity, current);
	if (token) {
		token->type = type;
		token->child = current + 1;
		token->sibling = 0;
		token->value = (unsigned int)pos;
		token->value_length = 0;
	}
	return token;
}

static void
set_token_id(json_token_t* tokens, size_t capacity, unsigned int current, size_t id, size_t id_length) {
	json_token_t* token = get_token(tokens, capacity, current);
	if (token) {
		token->id = (unsigned int)id;
		token->id_length = (unsigned int)id_length;
	}
}

static bool
is_whitespace(char c) {
	return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r');
}

static bool
is_token_delimiter(char c) {
	return is_whitespace(c) || (c == ']') || (c == '}') || (c == ',');
}

static size_t
skip_whitespace(const char* buffer, size_t length, size_t pos) {
	while (pos < length) {
		if (!is_whitespace(buffer[pos]))
			return pos;
		++pos;
	}
	return pos;
}

static char
hex_char(unsigned char val) {
	if (val < 10)
		return '0' + (char)val;
	else if (val < 16)
		return 'a' + (char)(val - 10);
	return '0';
}

static size_t
parse_string(const char* buffer, size_t length, size_t pos, bool key, bool simple) {
	size_t start = pos;
	size_t esc;
	while (pos < length) {
		char c = buffer[pos];
		if (simple && (is_token_delimiter(c) || (key && ((c == '=') || (c == ':')))))
			return pos - start;
		if (c == '"')
			return pos - start;
		++pos;
		if (c == '\\' && (pos < length)) {
			switch (buffer[pos]) {
				// Escaped symbols \X
				case '\"':
				case '/':
				case '\\':
				case 'b':
				case 'f':
				case 'r':
				case 'n':
				case 't':
					break;
				// Escaped symbol \uXXXX
				case 'u':
					for (esc = 0; esc < 4 && pos < length; ++esc) {
						++pos;
						if (!((buffer[pos] >= 48 && buffer[pos] <= 57) ||  // 0-9
						      (buffer[pos] >= 65 && buffer[pos] <= 70) ||  // A-F
						      (buffer[pos] >= 97 && buffer[pos] <= 102)))  // a-f
							return STRING_NPOS;
					}
					break;
				default:
					return STRING_NPOS;
			}
			++pos;
		}
	}
	return simple ? pos - start : STRING_NPOS;
}

static size_t
parse_number(const char* buffer, size_t length, size_t pos) {
	size_t start = pos;
	bool has_dot = false;
	bool has_digit = false;
	bool has_exp = false;
	while (pos < length) {
		char c = buffer[pos];
		if (is_token_delimiter(c))
			break;
		if (c == '-') {
			if (start != pos)
				return STRING_NPOS;
		} else if (c == '.') {
			if (has_dot || has_exp)
				return STRING_NPOS;
			has_dot = true;
		} else if ((c == 'e') || (c == 'E')) {
			if (!has_digit || has_exp)
				return STRING_NPOS;
			has_exp = true;
			if ((pos + 1) < length) {
				if ((buffer[pos + 1] == '+') || (buffer[pos + 1] == '-'))
					++pos;
			}
		} else if ((c < '0') || (c > '9'))
			return STRING_NPOS;
		else
			has_digit = true;
		++pos;
	}
	return has_digit ? (pos - start) : STRING_NPOS;
}

static size_t
parse_object(const char* buffer, size_t length, size_t pos, json_token_t* tokens, size_t capacity,
             unsigned int* current, bool simple);

static size_t
parse_value(const char* buffer, size_t length, size_t pos, json_token_t* tokens, size_t capacity, unsigned int* current,
            bool simple);

static size_t
parse_array(const char* buffer, size_t length, size_t pos, json_token_t* tokens, size_t capacity, unsigned int owner,
            unsigned int* current, bool simple);

static size_t
parse_object(const char* buffer, size_t length, size_t pos, json_token_t* tokens, size_t capacity,
             unsigned int* current, bool simple) {
	json_token_t* token;
	size_t string;
	bool simple_string;
	unsigned int last = 0;

	pos = skip_whitespace(buffer, length, pos);
	while (pos < length) {
		char c = buffer[pos++];

		switch (c) {
			case '}':
				if (last && !is_valid_token(tokens, capacity, last))
					return STRING_NPOS;
				return pos;

			case ',':
				if (!last || !is_valid_token(tokens, capacity, last))
					return STRING_NPOS;
				if ((token = get_token(tokens, capacity, last)))
					token->sibling = *current;
				last = 0;
				pos = skip_whitespace(buffer, length, pos);
				break;

			case '"':
			default:
				if (last)
					return STRING_NPOS;
				if (c != '"') {
					if (!simple)
						return STRING_NPOS;
					simple_string = true;
					--pos;
				} else {
					simple_string = false;
				}

				string = parse_string(buffer, length, pos, true, simple_string);
				if (string == STRING_NPOS)
					return STRING_NPOS;

				last = *current;
				set_token_id(tokens, capacity, *current, pos, string);
				// Skip terminating '"' (optional for simplified)
				if (!simple || ((pos + string < length) && (buffer[pos + string] == '"')))
					++string;
				pos += string;

				pos = skip_whitespace(buffer, length, pos);
				if ((buffer[pos] != ':') && (!simple || (buffer[pos] != '=')))
					return STRING_NPOS;
				pos = parse_value(buffer, length, pos + 1, tokens, capacity, current, simple);
				pos = skip_whitespace(buffer, length, pos);
				if (simple_string && ((pos < length) && (buffer[pos] != ',') && (buffer[pos] != '}'))) {
					token = get_token(tokens, capacity, last);
					if (token)
						token->sibling = *current;
					last = 0;
				}
				break;
		}
	}

	return simple ? pos : STRING_NPOS;
}

static size_t
parse_array(const char* buffer, size_t length, size_t pos, json_token_t* tokens, size_t capacity, unsigned int owner,
            unsigned int* current, bool simple) {
	json_token_t* parent = get_token(tokens, capacity, owner);
	json_token_t* token;
	unsigned int now;
	unsigned int last = 0;

	pos = skip_whitespace(buffer, length, pos);
	if (buffer[pos] == ']') {
		if (parent)
			parent->child = 0;
		return skip_whitespace(buffer, length, ++pos);
	}

	while (pos < length) {
		now = *current;
		set_token_id(tokens, capacity, now, 0, 0);
		pos = parse_value(buffer, length, pos, tokens, capacity, current, simple);
		if (pos == STRING_NPOS)
			return STRING_NPOS;
		if (parent)
			parent->value_length++;
		if (last) {
			token = get_token(tokens, capacity, last);
			if (token)
				token->sibling = now;
		}
		last = now;
		pos = skip_whitespace(buffer, length, pos);
		if (buffer[pos] == ',')
			++pos;
		else if (buffer[pos] == ']')
			return ++pos;
		else if (!simple || buffer[pos] == '}')
			return STRING_NPOS;
	}

	return STRING_NPOS;
}

static size_t
parse_value(const char* buffer, size_t length, size_t pos, json_token_t* tokens, size_t capacity, unsigned int* current,
            bool simple) {
	json_token_t* subtoken;
	unsigned int owner;
	size_t string;
	bool simple_string;

	pos = skip_whitespace(buffer, length, pos);
	while (pos < length) {
		char c = buffer[pos++];
		switch (c) {
			case '{':
				subtoken = set_token_complex(tokens, capacity, *current, JSON_OBJECT, pos - 1);
				++(*current);
				pos = parse_object(buffer, length, pos, tokens, capacity, current, simple);
				if (subtoken) {
					if (pos != STRING_NPOS)
						subtoken->value_length = (unsigned int)(pos - subtoken->value);
					if (subtoken->child == *current)
						subtoken->child = 0;
				}
				return pos;

			case '[':
				owner = *current;
				set_token_complex(tokens, capacity, *current, JSON_ARRAY, 0);
				++(*current);
				pos = parse_array(buffer, length, pos, tokens, capacity, owner, current, simple);
				return pos;

			case '-':
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '.':
				string = parse_number(buffer, length, pos - 1);
				if (string == STRING_NPOS)
					return STRING_NPOS;
				set_token_primitive(tokens, capacity, *current, JSON_PRIMITIVE, pos - 1, string);
				++(*current);
				return pos + string - 1;

			case 't':
			case 'f':
			case 'n':
				if ((c == 't') && (length - pos >= 4) && string_equal(buffer + pos, 3, STRING_CONST("rue")) &&
				    is_token_delimiter(buffer[pos + 3])) {
					set_token_primitive(tokens, capacity, *current, JSON_PRIMITIVE, pos - 1, 4);
					++(*current);
					return pos + 3;
				}
				if ((c == 'f') && (length - pos >= 5) && string_equal(buffer + pos, 4, STRING_CONST("alse")) &&
				    is_token_delimiter(buffer[pos + 4])) {
					set_token_primitive(tokens, capacity, *current, JSON_PRIMITIVE, pos - 1, 5);
					++(*current);
					return pos + 4;
				}
				if ((c == 'n') && (length - pos >= 4) && string_equal(buffer + pos, 3, STRING_CONST("ull")) &&
				    is_token_delimiter(buffer[pos + 3])) {
					set_token_primitive(tokens, capacity, *current, JSON_PRIMITIVE, pos - 1, 4);
					++(*current);
					return pos + 3;
				}
				if (!simple)
					return STRING_NPOS;
				// Fall through to string handling

			case '"':
			default:
				if (c != '"') {
					if (!simple)
						return STRING_NPOS;
					simple_string = true;
					--pos;
				} else {
					simple_string = false;
				}
				string = parse_string(buffer, length, pos, false, simple_string);
				if (string == STRING_NPOS)
					return STRING_NPOS;
				set_token_primitive(tokens, capacity, *current, JSON_STRING, pos, string);
				++(*current);
				// Skip terminating '"' (optional for simplified)
				if (!simple_string || ((pos + string < length) && (buffer[pos + string] == '"')))
					++string;
				return pos + string;
		}
	}

	return STRING_NPOS;
}

size_t
json_parse(const char* buffer, size_t size, json_token_t* tokens, size_t capacity) {
	unsigned int current = 0;
	set_token_id(tokens, capacity, current, 0, 0);
	set_token_primitive(tokens, capacity, current, JSON_UNDEFINED, 0, 0);
	if (parse_value(buffer, size, 0, tokens, capacity, &current, false) == STRING_NPOS)
		return 0;
	return current;
}

size_t
sjson_parse(const char* buffer, size_t size, json_token_t* tokens, size_t capacity) {
	unsigned int current = 0;
	size_t pos = skip_whitespace(buffer, size, 0);
	if ((pos < size) && (buffer[pos] != '{')) {
		set_token_id(tokens, capacity, current, 0, 0);
		set_token_complex(tokens, capacity, current, JSON_OBJECT, pos);
		++current;
		if (parse_object(buffer, size, pos, tokens, capacity, &current, true) == STRING_NPOS)
			current = 0;
		if (capacity)
			tokens[0].value_length = (unsigned int)(size - tokens[0].value);
		return current;
	}
	if (parse_value(buffer, size, pos, tokens, capacity, &current, true) == STRING_NPOS)
		return 0;
	return current;
}

string_t
json_escape(char* buffer, size_t capacity, const char* string, size_t length) {
	size_t i;
	size_t outlength = 0;
	for (i = 0; (i < length) && (outlength < capacity); ++i) {
		char c = string[i];
		if ((c == '\"') || (c == '\\')) {
			buffer[outlength++] = '\\';
			if (outlength < capacity)
				buffer[outlength++] = c;
		} else if (c == '\b') {
			buffer[outlength++] = '\\';
			if (outlength < capacity)
				buffer[outlength++] = 'b';
		} else if (c == '\f') {
			buffer[outlength++] = '\\';
			if (outlength < capacity)
				buffer[outlength++] = 'f';
		} else if (c == '\r') {
			buffer[outlength++] = '\\';
			if (outlength < capacity)
				buffer[outlength++] = 'r';
		} else if (c == '\n') {
			buffer[outlength++] = '\\';
			if (outlength < capacity)
				buffer[outlength++] = 'n';
		} else if (c == '\t') {
			buffer[outlength++] = '\\';
			if (outlength < capacity)
				buffer[outlength++] = 't';
		} else if (c < 0x20) {
			buffer[outlength++] = '\\';
			if (outlength < capacity)
				buffer[outlength++] = 'u';
			if (outlength < capacity)
				buffer[outlength++] = '0';
			if (outlength < capacity)
				buffer[outlength++] = '0';
			if (outlength < capacity)
				buffer[outlength++] = hex_char((unsigned char)(c >> 4) & 0xf);
			if (outlength < capacity)
				buffer[outlength++] = hex_char((unsigned char)c & 0xf);
		} else {
			buffer[outlength++] = c;
		}
	}
	return (string_t){buffer, outlength};
}

string_t
json_unescape(char* buffer, size_t capacity, const char* string, size_t length) {
	size_t i;
	size_t outlength = 0;
	for (i = 0; (i < length) && (outlength < capacity); ++i) {
		char c = string[i];
		if ((c == '\\') && (i + 1 < length)) {
			c = string[++i];
			switch (c) {
				case '\"':
				case '/':
				case '\\':
					buffer[outlength++] = c;
					break;

				case 'b':
					buffer[outlength++] = '\b';
					break;
				case 'f':
					buffer[outlength++] = '\f';
					break;
				case 'r':
					buffer[outlength++] = '\r';
					break;
				case 'n':
					buffer[outlength++] = '\n';
					break;
				case 't':
					buffer[outlength++] = '\t';
					break;

				case 'u':
					if (i + 4 < length) {
						uint16_t val = (uint16_t)string_to_uint(string + i + 1, 4, true);
						string_t conv = string_convert_utf16(buffer + outlength, capacity - outlength, &val, 1);
						outlength += conv.length;
						i += 4;
					}
					break;

				default:
					break;
			}
		} else {
			buffer[outlength++] = c;
		}
	}
	return (string_t){buffer, outlength};
}

static size_t
sjson_parse_stream(const char* path, size_t length, json_handler_fn handler) {
	json_token_t localtokens[64];
	json_token_t* tokens = localtokens;
	size_t capacity = sizeof(localtokens) / sizeof(localtokens[0]);
	size_t num = 0;

	stream_t* configfile = stream_open(path, length, STREAM_IN);
	if (!configfile)
		return 0;

	size_t size = stream_size(configfile);
	char* buffer = memory_allocate(0, size, 0, (size < 1024) ? MEMORY_TEMPORARY : MEMORY_PERSISTENT);

	stream_read(configfile, buffer, size);
	stream_deallocate(configfile);

	num = sjson_parse(buffer, size, tokens, capacity);
	if (num > capacity) {
		capacity = num;
		tokens = memory_allocate(0, sizeof(json_token_t) * capacity, 0, MEMORY_PERSISTENT);
		num = sjson_parse(buffer, size, tokens, capacity);
	}
	if (num && (tokens[0].type == JSON_OBJECT))
		handler(path, length, buffer, size, tokens, num);

	memory_deallocate(buffer);
	if (tokens != localtokens)
		memory_deallocate(tokens);

	return num;
}

size_t
sjson_parse_path(const char* path, size_t length, json_handler_fn handler) {
	if (fs_is_directory(path, length)) {
		size_t num = 0;
		char pathbuf[BUILD_MAX_PATHLEN];
		string_t* files = fs_files(path, length);
		for (size_t ifile = 0, fsize = array_size(files); ifile < fsize; ++ifile) {
			string_const_t ext = path_file_extension(STRING_ARGS(files[ifile]));
			if (string_equal(STRING_ARGS(ext), STRING_CONST("json")) ||
			    string_equal(STRING_ARGS(ext), STRING_CONST("sjson"))) {
				string_t fullpath = path_concat(pathbuf, sizeof(pathbuf), path, length, STRING_ARGS(files[ifile]));
				num += sjson_parse_stream(STRING_ARGS(fullpath), handler);
			}
		}
		string_array_deallocate(files);
		return num;
	}

	return sjson_parse_stream(path, length, handler);
}
