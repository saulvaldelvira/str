/**
 * Copyright (C) 2023 - Saúl Valdelvira
 * License: MIT
 * Email: saul@saulv.es
 */
#include "wstr.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h> // memcpy

#define INITIAL_SIZE 16

/**
 * %WString struct
 * @see wstr.h
*/
struct WString{
		wchar_t* buffer;
		size_t   length;
		size_t   buffer_size;
};

static void resize_buffer(WString *wstr, size_t new_size){
	if (new_size == 0)
		new_size = 1;
	wstr->buffer_size = new_size;
	wstr->buffer = realloc(wstr->buffer, wstr->buffer_size * sizeof(wchar_t));
	assert(wstr->buffer);
}

WString* wstr_empty(void){
	return wstr_init(INITIAL_SIZE);
}

WString* wstr_init(unsigned initial_size){
	WString *wstr = malloc(sizeof(*wstr));
	assert(wstr);
	wstr->buffer = NULL;
	resize_buffer(wstr, initial_size);
	wstr->length = 0;
	return wstr;
}

static size_t wstrnlen(const wchar_t *str, unsigned n){
	size_t len = 0;
	while (*str++ != L'\0' && n-- > 0)
		len++;
	return len;
}

WString* wstr_from_cwstr(const wchar_t *src, unsigned n){
	if (!src)
		return NULL;
	size_t len = wstrnlen(src, n);
	WString *wstr = wstr_init(len);
	wstr_concat_cwstr(wstr, src, n);
	return wstr;
}

void wstr_reserve(WString *wstr, unsigned n){
	if (wstr && wstr->buffer_size < n)
		resize_buffer(wstr, n);
}

int wstr_concat_cwstr(WString *wstr, const wchar_t *cat, unsigned n){
	if (!wstr || !cat)
		return -1;
	size_t len = wstrnlen(cat, n);
	if (wstr->length + len > wstr->buffer_size){
		size_t new_size = wstr->buffer_size * 2;
		if (wstr->length + len > new_size)
			new_size += len;
		resize_buffer(wstr, new_size);
	}
	memcpy(&wstr->buffer[wstr->length], cat, len * sizeof(wchar_t));
	wstr->length += len;
	return 1;
}

int wstr_concat_wstr(WString *wstr, WString *cat){
	if (!wstr || !cat)
		return -1;
	return wstr_concat_cwstr(wstr, cat->buffer, cat->length);
}

int wstr_push_char(WString *wstr, wchar_t c){
	return wstr_concat_cwstr(wstr, (wchar_t[]){c, L'\0'}, 2);
}

int wstr_pop(WString *wstr){
	if (!wstr)
		return -1;
	return wstr_remove_at(wstr, wstr->length - 1);
}

int wstr_remove_at(WString *wstr, unsigned index){
	if (!wstr)
		return -1;
	if (index >= wstr->length)
		return -2;
	if (index < wstr->length - 1)
		memcpy(&wstr->buffer[index], &wstr->buffer[index + 1], (wstr->length - index - 1) * sizeof(wchar_t));
	wstr->length--;
	return 1;
}

int wstr_remove_range(WString *wstr, unsigned start, unsigned end){
	if (!wstr)
		return -1;
	if (end < start)
		return -2;
	if (end > wstr->length)
		end = wstr->length;
	size_t len = wstr->length - end;
	memmove(&wstr->buffer[start], &wstr->buffer[end], len * sizeof(wchar_t));
	wstr->length -= end - start;
	return 1;
}

wchar_t wstr_get_at(WString *wstr, unsigned index){
	if (!wstr)
		return -1;
	else if (index >= wstr->length)
		return -2;
	return wstr->buffer[index];
}

int wstr_set_at(WString *wstr, unsigned index, wchar_t c){
	if (!wstr)
		return -1;
	else if (index >= wstr->length)
		return -2;
	return wstr->buffer[index] = c;
}

int wstr_insert(WString *wstr, const wchar_t *insert, unsigned n, unsigned index){
	if (!wstr || !insert)
		return -1;
	if (index > wstr->length)
		return -2;
	size_t len = wstrnlen(insert, n);
	if (wstr->length + len  > wstr->buffer_size){
		size_t new_size = wstr->buffer_size * 2;
		if (wstr->length + len > new_size)
			new_size += len;
		resize_buffer(wstr, new_size);
	}
	memmove(&wstr->buffer[index + len], &wstr->buffer[index], (wstr->length - index) * sizeof(wchar_t));
	memcpy(&wstr->buffer[index], insert, len * sizeof(wchar_t));
	wstr->length += len;
	return 1;
}

wchar_t* wstr_to_cwstr(WString *wstr){
	if (!wstr)
		return NULL;
	wchar_t *cwstr = malloc((wstr->length + 1) * sizeof(wchar_t));
	memcpy(cwstr, wstr->buffer, wstr->length * sizeof(wchar_t));
	cwstr[wstr->length] = '\0';
	return cwstr;
}

const wchar_t* wstr_get_buffer(WString *wstr){
	if (!wstr)
		return NULL;
	if (wstr->length == wstr->buffer_size)
		resize_buffer(wstr, wstr->buffer_size * 2);
	wstr->buffer[wstr->length] = '\0';
	return wstr->buffer;
}

wchar_t* wstr_substring(WString *wstr, unsigned start, unsigned end){
	if (!wstr || end < start)
		return NULL;
	if (end > wstr->length)
		end = wstr->length;
	size_t len = end - start;
	wchar_t *substring = malloc((len + 1) * sizeof(wchar_t));
	assert(substring);
	memcpy(substring, &wstr->buffer[start], len * sizeof(wchar_t));
	substring[len] = '\0';
	return substring;
}

WString* wstr_dup(WString *wstr){
	if (!wstr)
		return NULL;
	WString *dup = wstr_init(wstr->length);
	memcpy(dup->buffer, wstr->buffer, wstr->length * sizeof(wchar_t));
		dup->length = wstr->length;
	return dup;
}

size_t wstr_length(WString *wstr){
	if (!wstr)
		return 0;
	return wstr->length;
}

wchar_t* wstr_tok(WString *wstr, wchar_t *tokens){
	static wchar_t *prev_tok = NULL;
	static size_t pos = 0;
	static WString *curr_str = NULL;
	free(prev_tok);
	if (wstr != NULL){
		pos = 0;
		curr_str = wstr;
	}
	if (!curr_str || !tokens || pos == curr_str->length)
		return NULL;
	for (size_t i = pos + 1; i < curr_str->length; i++){
		for (wchar_t *t = tokens; *t != '\0'; t++){
			if (curr_str->buffer[i] == *t){
				size_t len = i - pos;
				prev_tok = malloc(len + 1);
				memcpy(prev_tok, &curr_str->buffer[pos], len * sizeof(wchar_t));
				prev_tok[len] = '\0';
				pos = i + 1;
				return prev_tok;
			}
		}
	}
	size_t len = curr_str->length - pos;
	prev_tok = malloc((len + 1) * sizeof(wchar_t));
	memcpy(prev_tok, &curr_str->buffer[pos], len);
	prev_tok[len] = '\0';
	pos = curr_str->length;
	return prev_tok;
}

int wstr_transform(WString *wstr, wchar_t(*func)(wchar_t)){
	if (!wstr || !func)
		return -1;
	for (size_t i = 0; i < wstr->length; i++)
		wstr->buffer[i] = func(wstr->buffer[i]);
	return 1;
}

void wstr_shrink(WString *wstr){
	if (wstr && wstr->buffer_size > wstr->length)
		resize_buffer(wstr, wstr->length);
}

void wstr_clear(WString *wstr){
	if (wstr)
		wstr->length = 0;
}

void wstr_free(WString *wstr){
	if (wstr){
		free(wstr->buffer);
		free(wstr);
	}
}
