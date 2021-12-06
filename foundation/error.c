/* error.c  -  Foundation library  -  Public Domain  -  2013 Mattias Jansson
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
#include <foundation/internal.h>

#include <stdarg.h>

FOUNDATION_DECLARE_THREAD_LOCAL(error_t, error, 0)  // 0 = ERROR_NONE
FOUNDATION_DECLARE_THREAD_LOCAL(error_handler_fn, error_handler, 0)

error_t
error(void) {
	error_t err = get_thread_error();
	set_thread_error(ERROR_NONE);
	return err;
}

int
error_report(error_level_t level, error_t err) {
	int result = 0;
	error_handler_fn handler = get_thread_error_handler();

	set_thread_error(err);
	if (handler)
		result = handler(level, err);

	return result;
}

error_handler_fn
error_handler(void) {
	return get_thread_error_handler();
}

void
error_set_handler(error_handler_fn handler) {
	set_thread_error_handler(handler);
}

#if BUILD_ENABLE_ERROR_CONTEXT

FOUNDATION_DECLARE_THREAD_LOCAL(error_context_t*, error_context, 0)

error_context_t*
error_context_clone(void) {
	error_context_t* context = get_thread_error_context();
	if (!context)
		return context;

	size_t capacity = sizeof(error_context_t) + (sizeof(error_frame_t) * foundation_config().error_context_depth);
	void* context_clone = memory_allocate(0, capacity, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
	memcpy(context_clone, context, capacity);
	return context_clone;
}

error_context_t*
error_context_set(error_context_t* context) {
	error_context_t* previous = get_thread_error_context();
	set_thread_error_context(context);
	return previous;
}

void
error_context_push_impl(const char* name, size_t name_length, const char* data, size_t data_length) {
	error_context_t* context = get_thread_error_context();
	if (!context) {
		size_t capacity = sizeof(error_context_t) + (sizeof(error_frame_t) * foundation_config().error_context_depth);
		context = memory_allocate(0, capacity, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		set_thread_error_context(context);
	}
	FOUNDATION_ASSERT_MSG(context->depth < foundation_config().error_context_depth, "Error context overflow");
	context->frame[context->depth].name.str = name ? name : "<unknown>";
	context->frame[context->depth].name.length = name ? name_length : 9;
	context->frame[context->depth].data.str = data;
	context->frame[context->depth].data.length = data_length;
	if (context->depth < foundation_config().error_context_depth - 1)
		++context->depth;
}

void
error_context_push_format_impl(const char* name, size_t name_length, char* data, size_t data_length,
                               const char* data_format, size_t data_format_length, ...) {
	error_context_t* context = get_thread_error_context();
	if (!context) {
		size_t capacity = sizeof(error_context_t) + (sizeof(error_frame_t) * foundation_config().error_context_depth);
		context = memory_allocate(0, capacity, 0, MEMORY_PERSISTENT | MEMORY_ZERO_INITIALIZED);
		set_thread_error_context(context);
	}
	// Format the data string
#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic push
#if __has_warning("-Wformat-nonliteral")
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#endif
#endif
	string_t data_string = string(0, 0);
	if (data_length && data_format_length) {
		va_list data_list;
		va_start(data_list, data_format_length);
		data_string = string_vformat(data, data_length, data_format, data_format_length, data_list);
		va_end(data_list);
	}
	context->frame[context->depth].name.str = name ? name : "<something>";
	context->frame[context->depth].name.length = name ? name_length : 11;
	context->frame[context->depth].data.str = data_string.length ? data_string.str : "<nothing>";
	context->frame[context->depth].data.length = data_string.length ? data_string.length : 9;
	if (context->depth < foundation_config().error_context_depth - 1)
		++context->depth;
#if FOUNDATION_COMPILER_CLANG
#pragma clang diagnostic pop
#endif
}

void
error_context_pop_impl(void) {
	error_context_t* context = get_thread_error_context();
	if (context && context->depth > 0)
		--context->depth;
}

void
error_context_clear_impl(void) {
	error_context_t* context = get_thread_error_context();
	if (context)
		context->depth = 0;
}

string_t
error_context_buffer_impl(char* buffer, size_t size) {
	error_context_t* context = get_thread_error_context();
	string_t result = {buffer, size};
	if (context) {
		size_t i;
		string_t line;
		error_frame_t* frame = context->frame;
		for (i = 0; (size > 1) && (i < context->depth); ++i, ++frame) {
			if (i) {
				*buffer++ = '\n';
				--size;
			}

			line = string_format(buffer, size, STRING_CONST("When %.*s:"), STRING_FORMAT(frame->name));
			buffer += line.length;
			size -= line.length;

			if (frame->data.length) {
				line = string_format(buffer, size, STRING_CONST(" %.*s"), STRING_FORMAT(frame->data));
				buffer += line.length;
				size -= line.length;
			}
		}
	}
	if (size)
		*buffer = 0;
	result.length = (size_t)pointer_diff(buffer, result.str);
	return result;
}

error_context_t*
error_context_impl(void) {
	return get_thread_error_context();
}

void
error_context_thread_finalize_impl(void) {
	error_context_t* context = get_thread_error_context();
	if (context) {
		FOUNDATION_ASSERT_MSG(!context->depth, "Error context thread exit with non-zero context stack");
		memory_deallocate(context);
		set_thread_error_context(0);
	}
}

#else

error_context_t*
error_context_clone(void) {
	return 0;
}

error_context_t*
error_context_set(error_context_t* context) {
	FOUNDATION_UNUSED(context);
	return 0;
}

#endif
