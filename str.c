/**
 * Copyright (C) 2023 - Saúl Valdelvira
 * License: MIT
 * Email: saulvaldelvira@gmail.com
 */
#include "str.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h> // memcpy, strnlen

#define INITIAL_SIZE 16

struct String{
        char    *buffer;
        size_t  length;
        size_t  buffer_size;
};

String* str_empty(void){
        return str_init(INITIAL_SIZE);
}

String* str_init(unsigned initial_size){
        String *str = malloc(sizeof(*str));
	assert(str);
        str->buffer = malloc(initial_size * sizeof(char));
	assert(str->buffer);
        str->length = 0;
        str->buffer_size = initial_size;
        return str;
}

String* str_from_cstr(char *src, unsigned n){
	if (!src)
		return NULL;
	size_t len = strnlen(src, n);
	String *str = str_init(len);
	str_concat_cstr(str, src, n);
	return str;
}

static void resize_buffer(String *str, size_t new_size){
	str->buffer_size = new_size;	
	str->buffer = realloc(str->buffer, str->buffer_size * sizeof(char));
	assert(str->buffer);
}

void str_reserve(String *str, unsigned n){
	if (str && str->buffer_size < n)
		resize_buffer(str, n);
}

int str_concat_cstr(String *str, char *cat, unsigned n){
	if (!str || !cat)
		return -1;
	size_t len = strnlen(cat, n);
	if (str->buffer_size - str->length < len){
		size_t new_size = str->buffer_size * 2;
		if (new_size - str->length < len)
			new_size += len;
		resize_buffer(str, new_size);
	}
        memcpy(&str->buffer[str->length], cat, len * sizeof(char));
	str->length += len;
	return 1;
}

int str_concat_str(String *str, String *cat){
	if (!str || !cat)
		return -1;
	return str_concat_cstr(str, cat->buffer, cat->length);
}

int str_push_char(String *str, char c){
	return str_concat_cstr(str, (char[]){c,'\0'}, 2);
}

char str_get_at(String *str, unsigned index){
	if (!str)
		return -1;
	else if (index >= str->length)
		return -2;
	return str->buffer[index];
}

int str_set_at(String *str, unsigned index, char c){
	if (!str)
		return -1;
	else if (index >= str->length)
		return -2;
	return str->buffer[index] = c;
}

int str_insert(String *str, char *insert, unsigned n, unsigned index){
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

char* str_to_cstr(String *str){
	if (!str)
		return NULL;
	char *cstr = malloc(str->length + 1);
	memcpy(cstr, str->buffer, str->length * sizeof(char));
	cstr[str->length] = '\0';
	return cstr;
}

char* str_substring(String *str, unsigned start, unsigned end){
	if (!str || end >= str->length || end < start)
		return NULL;
	size_t len = end - start;
	char *substring = malloc((len + 1) * sizeof(char));
	assert(substring);
	memcpy(substring, &str->buffer[start], len * sizeof(char));
	substring[len] = '\0';
	return substring;
}

int str_transform(String *str, char(*func)(char)){
	if (!str || !func)
		return -1;
	for (size_t i = 0; i < str->length; i++)
		str->buffer[i] = func(str->buffer[i]);
	return 1;
}

String* str_dup(String *str){
	if (!str)
		return NULL;
	String *dup = str_init(str->length);	
	memcpy(dup->buffer, str->buffer, str->length * sizeof(char));
        dup->length = str->length;
	return dup;
}

size_t str_length(String *str){
	return str->length;
}

char* str_tok(String *str, char *tokens){
	static char *prev_tok = NULL;
	static size_t pos = 0;
	static String *curr_str = NULL;
	free(prev_tok);
	if (str != NULL){
		pos = 0;
		curr_str = str;
	}
	if (!curr_str || !tokens || pos == curr_str->length)
		return NULL;
	for (size_t i = pos + 1; i < curr_str->length; i++){
		for (char *t = tokens; *t != '\0'; t++){
			if (curr_str->buffer[i] == *t){
				size_t len = i - pos;
				prev_tok = malloc(len + 1);
				memcpy(prev_tok, &curr_str->buffer[pos], len * sizeof(char));
				prev_tok[len] = '\0';
				pos = i + 1;
				return prev_tok;
			}
		}
	}
	size_t len = curr_str->length - pos;
	prev_tok = malloc(len + 1);
	memcpy(prev_tok, &curr_str->buffer[pos], len);
	prev_tok[len] = '\0';
	pos = curr_str->length;
	return prev_tok;
}

void str_shrink(String *str){
	if (str && str->buffer_size > str->length)
		resize_buffer(str, str->length);
}

void str_clear(String *str){
	if (str)
		str->length = 0;
}

void str_free(String *str){
	if (str){
		free(str->buffer);
		free(str);
	}
}
