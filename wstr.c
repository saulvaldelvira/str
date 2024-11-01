/*
 * wstr.c - wstring_t implementation.
 * Author: Saúl Valdelvira (2023)
 */
#define _POSIX_C_SOURCE 200809L
#include "wstr.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h> // memcpy
#include <stdarg.h>
#include <time.h>
#include <wchar.h>
#include "util.h"

#define INITIAL_SIZE 16
#ifndef GROW_FACTOR
#define GROW_FACTOR 2
#endif
static_assert(GROW_FACTOR > 1, "");

struct wstring {
		wchar_t* buffer;
		size_t   length;
		size_t   buffer_size;
};

static void __resize_buffer(wstring_t *wstr, size_t new_size){
        assert(wstr);
        if (new_size == 0)
                new_size = INITIAL_SIZE;
	wstr->buffer_size = new_size;
	wstr->buffer = realloc(wstr->buffer, wstr->buffer_size * sizeof(wchar_t));
	assert(wstr->buffer);
}

#define __wstr_init(initial_size) \
	wstring_t *wstr = malloc(sizeof(wstring_t)); \
	assert(wstr); \
        memset(wstr, 0, sizeof(wstring_t)); \
	__resize_buffer(wstr, initial_size); \
	return wstr;

wstring_t* wstr_empty(void){
        __wstr_init(INITIAL_SIZE);
}

wstring_t* wstr_init(unsigned initial_size){
	__wstr_init(initial_size);
}

static size_t __wstrnlen(const wchar_t *str, unsigned n){
        assert(str);
	size_t len = 0;
        while (*str != L'\0' && n > 0) {
                len++;
                str++;
                n--;
        }
	return len;
}

wstring_t* wstr_from_cwstr(const wchar_t *src, unsigned n){
	if (!src)
		return NULL;
	size_t len = __wstrnlen(src, n);
	wstring_t *wstr = wstr_init(len);
	wstr_concat_cwstr(wstr, src, n);
	return wstr;
}

wstring_t* wstr_from_cstr(const char *src, unsigned n){
	if (!src)
		return NULL;
	size_t len = strnlen(src, n);
	wstring_t *wstr = wstr_init(len);
	wstr_concat_cstr(wstr, src, n);
	return wstr;
}

void wstr_reserve(wstring_t *wstr, unsigned n){
	if (wstr && wstr->buffer_size < n)
		__resize_buffer(wstr, n);
}

static inline void resize_if_needed(wstring_t *wstr, size_t size){
	if (wstr->length + size > wstr->buffer_size){
		size_t new_size = wstr->buffer_size * GROW_FACTOR;
		if (wstr->length + size > new_size)
			new_size += size;
		__resize_buffer(wstr, new_size);
	}
}

int wstr_concat_cwstr(wstring_t *wstr, const wchar_t *cat, unsigned n){
	if (!wstr || !cat)
		return -1;
	size_t len = __wstrnlen(cat, n);
	resize_if_needed(wstr, len);
	memcpy(&wstr->buffer[wstr->length], cat, len * sizeof(wchar_t));
	wstr->length += len;
	return 1;
}

int wstr_concat_cstr(wstring_t *wstr, const char *cat, unsigned n){
	if (!wstr || !cat)
		return -1;
	size_t len = strnlen(cat, n);
	resize_if_needed(wstr, len);
	wchar_t *start = &wstr->buffer[wstr->length];
	while (--n > 0 && *cat)
		*start++ = *cat++;
	wstr->length += len;
	return 1;
}

int wstr_concat_wstr(wstring_t *wstr, wstring_t *cat){
	if (!wstr || !cat)
		return -1;
	return wstr_concat_cwstr(wstr, cat->buffer, cat->length);
}

int wstr_push_char(wstring_t *wstr, wchar_t c){
	return wstr_concat_cwstr(wstr, (wchar_t[]){c, L'\0'}, 2);
}

int wstr_pop(wstring_t *wstr){
	if (!wstr)
		return -1;
	return wstr_remove_at(wstr, wstr->length - 1);
}

int wstr_remove_at(wstring_t *wstr, unsigned index){
	if (!wstr)
		return -1;
	if (index >= wstr->length)
		return -2;
	if (index < wstr->length - 1)
		memcpy(&wstr->buffer[index], &wstr->buffer[index + 1], (wstr->length - index - 1) * sizeof(wchar_t));
	wstr->length--;
	return 1;
}

int wstr_remove_range(wstring_t *wstr, unsigned start, unsigned end){
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

wchar_t wstr_get_at(const wstring_t *wstr, unsigned index){
	if (!wstr)
		return -1;
	else if (index >= wstr->length)
		return -2;
	return wstr->buffer[index];
}

int wstr_set_at(wstring_t *wstr, unsigned index, wchar_t c){
	if (!wstr)
		return -1;
	else if (index >= wstr->length)
		return -2;
	return wstr->buffer[index] = c;
}

int wstr_insert_cwstr(wstring_t *wstr, const wchar_t *insert, unsigned n, unsigned index){
	if (!wstr || !insert)
		return -1;
	if (index > wstr->length)
		return -2;
	size_t len = __wstrnlen(insert, n);
	resize_if_needed(wstr, len);
	memmove(&wstr->buffer[index + len], &wstr->buffer[index], (wstr->length - index) * sizeof(wchar_t));
	memcpy(&wstr->buffer[index], insert, len * sizeof(wchar_t));
	wstr->length += len;
	return 1;
}

int wstr_insert_cstr(wstring_t *wstr, const char *insert, unsigned n, unsigned index){
	if (!wstr || !insert)
		return -1;
	if (index > wstr->length)
		return -2;
	size_t len = strnlen(insert, n);
	resize_if_needed(wstr, len);
	memmove(&wstr->buffer[index + len], &wstr->buffer[index], (wstr->length - index) * sizeof(char));
	memcpy(&wstr->buffer[index], insert, len * sizeof(char));
	wstr->length += len;
	return 1;
}

int wstr_insert(wstring_t *wstr, wchar_t c, unsigned index){
	return wstr_insert_cwstr(wstr, (wchar_t[]){c, L'\0'}, 2, index);
}

wchar_t* wstr_to_cwstr(const wstring_t *wstr){
	if (!wstr)
		return NULL;
	wchar_t *cwstr = malloc((wstr->length + 1) * sizeof(wchar_t));
	memcpy(cwstr, wstr->buffer, wstr->length * sizeof(wchar_t));
	cwstr[wstr->length] = '\0';
	return cwstr;
}

static void __add_null_term(wstring_t *wstr) {
        assert(wstr);
	if (wstr->length == wstr->buffer_size)
		__resize_buffer(wstr, wstr->buffer_size * GROW_FACTOR);
	wstr->buffer[wstr->length] = '\0';
}

const wchar_t* wstr_get_buffer(wstring_t *wstr){
	if (!wstr)
		return NULL;
        __add_null_term(wstr);
	return wstr->buffer;
}

wchar_t* wstr_substring(wstring_t *wstr, unsigned start, unsigned end){
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

wstring_t* wstr_dup(wstring_t *wstr){
	if (!wstr)
		return NULL;
	wstring_t *dup = wstr_init(wstr->length);
	memcpy(dup->buffer, wstr->buffer, wstr->length * sizeof(wchar_t));
		dup->length = wstr->length;
	return dup;
}

size_t wstr_length(const wstring_t *wstr){
	if (!wstr)
		return 0;
	return wstr->length;
}

wchar_t* wstr_tok(wstring_t *wstr, wchar_t *tokens){
	static wchar_t *prev_tok = NULL;
	static size_t pos = 0;
	static wstring_t *curr_str = NULL;
	free(prev_tok);
	prev_tok = NULL;
	if (wstr != NULL){
		pos = 0;
		curr_str = wstr;
	}
	if (!curr_str || !tokens || pos == curr_str->length)
		return NULL;
	for (size_t i = pos; i < curr_str->length; i++){
		for (wchar_t *t = tokens; *t != '\0'; t++){
			if (curr_str->buffer[i] == *t){
				size_t len = i - pos;
				prev_tok = malloc((len + 1) * sizeof(wchar_t));
				memcpy(prev_tok, &curr_str->buffer[pos], len * sizeof(wchar_t));
				prev_tok[len] = '\0';
				pos = i + 1;
				return prev_tok;
			}
		}
	}
	size_t len = curr_str->length - pos;
	prev_tok = malloc((len + 1) * sizeof(wchar_t));
	memcpy(prev_tok, &curr_str->buffer[pos], len * sizeof(wchar_t));
	prev_tok[len] = L'\0';
	pos = curr_str->length;
	return prev_tok;
}

wchar_t** wstr_split(wstring_t *wstr, wchar_t *delim){
	if (!wstr || !delim)
		return NULL;
	size_t delim_len = __wstrnlen(delim, -1);
	int count = 1;
	int i = wstr_find_substring(wstr, delim, 0);
	while (i >= 0){
		count++;
		i = wstr_find_substring(wstr, delim, i + delim_len);
	}
	count++; // For the NULL element at the end
	wchar_t **split = malloc(count * sizeof(wchar_t*));
	wchar_t **ptr = split;
	int prev_i = 0;
	i = wstr_find_substring(wstr, delim, 0);
	while (i >= 0){
		*ptr = wstr_substring(wstr, prev_i, i);
		if (**ptr == L'\0')
			free(*ptr);
		else
			ptr++;
		prev_i = i + delim_len;
		i = wstr_find_substring(wstr, delim, prev_i);
	}
	if ((size_t)prev_i < wstr->length)
		*ptr = wstr_substring(wstr, prev_i, wstr->length);
	split[count - 1] = NULL;
	return split;
}

int wstr_find_substring(wstring_t *wstr, const wchar_t *substr, unsigned start_at){
	if (!wstr || !substr)
		return -2;
	if (start_at >= wstr->length)
		return -3;
	for (size_t i = start_at; i < wstr->length; i++){
		const wchar_t *c = substr;
		for (size_t j = i; j < wstr->length; j++){
			if (*c != wstr->buffer[j])
				break;
			c++;
			if (*c == L'\0')
                                return i;
		}
	}
	return -1;
}

int wstr_replace(wstring_t *wstr, const wchar_t *substr, const wchar_t *replacement){
	size_t substr_len = __wstrnlen(substr, -1);
	size_t replacement_len = __wstrnlen(replacement, -1);
        int n_replacements = 0;
	int i = wstr_find_substring(wstr, substr, 0);
        while (i >= 0){
                n_replacements++;
		wstr_remove_range(wstr, i, i + substr_len);
		wstr_insert_cwstr(wstr, replacement, replacement_len, i);
		i = wstr_find_substring(wstr, substr, i + replacement_len);
	}
	return n_replacements;
}

int wstr_transform(wstring_t *wstr, wchar_t(*func)(wchar_t)){
	if (!wstr || !func)
		return -1;
	for (size_t i = 0; i < wstr->length; i++)
		wstr->buffer[i] = func(wstr->buffer[i]);
	return 1;
}

void wstr_shrink(wstring_t *wstr){
	if (wstr && wstr->buffer_size > wstr->length)
		__resize_buffer(wstr, wstr->length);
}

wchar_t* wstr_into_cwstr(wstring_t *wstr) {
        if (!wstr) return NULL;
        __add_null_term(wstr);
        wstr_shrink(wstr);
        wchar_t *buf = wstr->buffer;
        wstr->buffer = NULL;
        return buf;
}

wchar_t* wstr_cloned_cwstr(wstring_t *wstr) {
        if (!wstr) return NULL;
        wchar_t *result = malloc((wstr->length + 1) * sizeof(wchar_t));
        memcpy(result, wstr->buffer, wstr->length * sizeof(wchar_t));
        result[wstr->length] = L'\0';
        return result;
}

int wstr_cmp_cwstr(const wstring_t *wstr, const wchar_t *cwstr) {
        if (!wstr || !cwstr) return 0;
        for (size_t i = 0; i < wstr->length; i++) {
                int c = wstr->buffer[i] - cwstr[i];
                if (c != 0)
                        return c;
        }
        return 0;
}

void wstr_clear(wstring_t *wstr){
	if (wstr)
		wstr->length = 0;
}

static INLINE void __wstr__free(wstring_t *wstr) {
	if (wstr){
		free(wstr->buffer);
		free(wstr);
	}
}

void (wstr_free)(wstring_t *wstr, ...){
        if (!wstr)
                return;
        va_list arg;
        va_start(arg, wstr);
        do {
                __wstr__free(wstr);
                wstr = va_arg(arg, wstring_t*);
        } while (wstr);
        va_end(arg);
}

void wstr_free_all(unsigned int n, ...){
	va_list arg;
	va_start(arg, n);
	while (n-- > 0){
		wstring_t *wstr = va_arg(arg, wstring_t*);
		wstr_free(wstr);
	}
	va_end(arg);
}
