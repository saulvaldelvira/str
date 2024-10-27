/*
 * str.c - string_t implementation.
 * Author: Saúl Valdelvira (2023)
 */
#define _POSIX_C_SOURCE 200809L
#include "str.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h> // memcpy, strnlen
#include <stdarg.h>
#include "util.h"

#define INITIAL_SIZE 16
#ifndef GROW_FACTOR
#define GROW_FACTOR 2
#endif
static_assert(GROW_FACTOR > 1, "");

struct string {
        char    *buffer;
        size_t  length;
        size_t  buffer_size;
};

static void resize_buffer(string_t *str, size_t new_size){
	if (new_size == 0)
		new_size = 1;
	str->buffer_size = new_size;
	str->buffer = realloc(str->buffer, str->buffer_size * sizeof(char));
	assert(str->buffer);
}

static INLINE
string_t* __str_init(unsigned int initial_size) {
	string_t *str = malloc(sizeof(*str));
	assert(str);
	str->buffer = NULL;
	resize_buffer(str, initial_size);
	str->length = 0;
	return str;
}

string_t* str_empty(void){
        return __str_init(INITIAL_SIZE);
}

string_t* str_init(unsigned int initial_size){
        return __str_init(initial_size);
}

string_t* str_from_cstr(const char *src, unsigned n){
	if (!src)
		return NULL;
	size_t len = strnlen(src, n);
	string_t *str = str_init(len);
	str_concat_cstr(str, src, n);
	return str;
}

void str_reserve(string_t *str, unsigned n){
	if (str && str->buffer_size < n)
		resize_buffer(str, n);
}

int str_concat_cstr(string_t *str, const char *cat, unsigned n){
	if (!str || !cat)
		return -1;
	size_t len = strnlen(cat, n);
	if (str->buffer_size - str->length < len){
		size_t new_size = str->buffer_size * GROW_FACTOR;
		if (new_size - str->length < len)
			new_size += len;
		resize_buffer(str, new_size);
	}
        memcpy(&str->buffer[str->length], cat, len * sizeof(char));
	str->length += len;
	return 1;
}

int str_concat_str(string_t *str, string_t *cat){
	if (!str || !cat)
		return -1;
	return str_concat_cstr(str, cat->buffer, cat->length);
}

int str_push_char(string_t *str, char c){
	return str_concat_cstr(str, (char[]){c,'\0'}, 2);
}

int str_pop(string_t *str){
	if (!str)
		return -1;
	return str_remove_at(str, str->length - 1);
}

int str_remove_at(string_t *str, unsigned index){
	if (!str)
		return -1;
	if (index >= str->length)
		return -2;
	if (index < str->length - 1)
		memcpy(&str->buffer[index], &str->buffer[index + 1], (str->length - index - 1) * sizeof(char));
	str->length--;
	return 1;
}

int str_remove_range(string_t *str, unsigned start, unsigned end){
	if (!str)
		return -1;
	if (end < start)
		return -2;
	if (end > str->length)
		end = str->length;
	size_t len = str->length - end;
	memmove(&str->buffer[start], &str->buffer[end], len * sizeof(char));
	str->length -= end - start;
	return 1;
}

char str_get_at(string_t *str, unsigned index){
	if (!str)
		return -1;
	else if (index >= str->length)
		return -2;
	return str->buffer[index];
}

int str_set_at(string_t *str, unsigned index, char c){
	if (!str)
		return -1;
	else if (index >= str->length)
		return -2;
	return str->buffer[index] = c;
}

int str_insert_cstr(string_t *str, const char *insert, unsigned n, unsigned index){
	if (!str || !insert)
		return -1;
	if (index > str->length)
		return -2;
	size_t len = strnlen(insert, n);
	if (str->length + len  > str->buffer_size){
		size_t new_size = str->buffer_size * 2;
		if (str->length + len > new_size)
			new_size += len;
		resize_buffer(str, new_size);
	}
	memmove(&str->buffer[index + len], &str->buffer[index], (str->length - index) * sizeof(char));
	memcpy(&str->buffer[index], insert, len * sizeof(char));
	str->length += len;
	return 1;
}

int str_insert(string_t *str, char c, unsigned index){
	return str_insert_cstr(str, (char[]){c, '\0'}, 2, index);
}

char* str_to_cstr(string_t *str){
	if (!str)
		return NULL;
	char *cstr = malloc(str->length + 1);
	memcpy(cstr, str->buffer, str->length * sizeof(char));
	cstr[str->length] = '\0';
	return cstr;
}

const char* str_get_buffer(string_t *str){
	if (!str)
		return NULL;
	if (str->length == str->buffer_size)
		resize_buffer(str, str->buffer_size * GROW_FACTOR);
	str->buffer[str->length] = '\0';
	return str->buffer;
}

char* str_substring(string_t *str, unsigned start, unsigned end){
	if (!str || end < start)
		return NULL;
	if (end > str->length)
		end = str->length;
	size_t len = end - start;
	char *substring = malloc((len + 1) * sizeof(char));
	assert(substring);
	memcpy(substring, &str->buffer[start], len * sizeof(char));
	substring[len] = '\0';
	return substring;
}

int str_transform(string_t *str, char(*func)(char)){
	if (!str || !func)
		return -1;
	for (size_t i = 0; i < str->length; i++)
		str->buffer[i] = func(str->buffer[i]);
	return 1;
}

string_t* str_dup(string_t *str){
	if (!str)
		return NULL;
	string_t *dup = str_init(str->length);
	memcpy(dup->buffer, str->buffer, str->length * sizeof(char));
        dup->length = str->length;
	return dup;
}

size_t str_length(string_t *str){
	if (!str)
		return 0;
	return str->length;
}

char* str_tok(string_t *str, char *tokens){
	static char *prev_tok = NULL;
	static size_t pos = 0;
	static string_t *curr_str = NULL;
	free(prev_tok);
	prev_tok = NULL;
	if (str != NULL){
		pos = 0;
		curr_str = str;
	}
	if (!curr_str || !tokens || pos == curr_str->length)
		return NULL;
	for (size_t i = pos; i < curr_str->length; i++){
		for (char *t = tokens; *t != '\0'; t++){
			if (curr_str->buffer[i] == *t){
				size_t len = i - pos;
				prev_tok = malloc((len + 1) * sizeof(char));
				memcpy(prev_tok, &curr_str->buffer[pos], len * sizeof(char));
				prev_tok[len] = '\0';
				pos = i + 1;
				return prev_tok;
			}
		}
	}
	size_t len = curr_str->length - pos;
	prev_tok = malloc(len + 1);
	memcpy(prev_tok, &curr_str->buffer[pos], len * sizeof(char));
	prev_tok[len] = '\0';
	pos = curr_str->length;
	return prev_tok;
}

char** str_split(string_t *str, char *delim){
	if (!str || !delim)
		return NULL;
	size_t delim_len = strlen(delim);
	int count = 1;
	int i = str_find_substring(str, delim, 0);
	while (i >= 0){
		count++;
		i = str_find_substring(str, delim, i + delim_len);
	}
	count++; // For the NULL element at the end
	char **split = malloc(count * sizeof(char*));
	char **ptr = split;
	int prev_i = 0;
	i = str_find_substring(str, delim, 0);
	while (i >= 0){
		*ptr = str_substring(str, prev_i, i);
		if (**ptr == '\0')
			free(*ptr);
		else
			ptr++;
		prev_i = i + delim_len;
		i = str_find_substring(str, delim, prev_i);
	}
	if ((size_t)prev_i < str->length)
		*ptr = str_substring(str, prev_i, str->length);
	split[count - 1] = NULL;
	return split;
}

int str_find_substring(string_t *str, const char *substr, unsigned start_at){
        if (!str || !substr)
                return -2;
        if (start_at >= str->length)
                return -3;
        for (size_t i = start_at; i < str->length; i++){
                const char *c = substr;
                for (size_t j = i; j < str->length; j++){
                        if (*c != str->buffer[j])
                                break;
                        c++;
			if (*c == L'\0')
                                return i;
		}
        }
        return -1;
}

int str_replace(string_t *str, const char *substr, const char *replacement){
	size_t substr_len = strlen(substr);
	size_t replacement_len = strlen(replacement);
        int n_replacements = 0;
	int i = str_find_substring(str, substr, 0);
        while (i >= 0){
                n_replacements++;
		str_remove_range(str, i, i + substr_len);
		str_insert_cstr(str, replacement, replacement_len, i);
		i = str_find_substring(str, substr, i + replacement_len);
	}
	return n_replacements;
}

void str_shrink(string_t *str){
	if (str && str->buffer_size > str->length)
		resize_buffer(str, str->length);
}

void str_clear(string_t *str){
	if (str)
		str->length = 0;
}

static INLINE void __str__free(string_t *str) {
	if (str){
		free(str->buffer);
		free(str);
	}
}

void (str_free)(string_t *str, ...){
        if (!str)
                return;
        va_list arg;
        va_start(arg, str);
        do {
                __str__free(str);
                str = va_arg(arg, string_t*);
        } while (str);
        va_end(arg);
}

void str_free_all(unsigned int n, ...){
	va_list arg;
	va_start(arg, n);
	while (n-- > 0){
		string_t *str = va_arg(arg, string_t*);
		str_free(str);
	}
	va_end(arg);
}
