/**
 * Copyright (C) 2023 - Saúl Valdelvira
 * License: MIT
 * Email: saulvaldelvira@gmail.com
 */
#pragma once
#ifndef WSTR_H
#define WSTR_H

#include <stddef.h> // size_t, wchar_t

typedef struct WString WString;

/**
 * Builds an empty WString
 */
WString* wstr_empty(void);

/**
 * Builds a WString, with the given initial size
 */
WString* wstr_init(unsigned initial_size);

/**
 * Builds a WString, from the given source cwstring.
 * @param n max length of src
 */
WString* wstr_from_cwstr(const wchar_t *src, unsigned n);

/**
 * Reserves space in the WString for n characters
 */
void wstr_reserve(WString *wstr, unsigned n);

/**
 * Concatenates the given cwstring at the end of the WString
 * @param n max length of cat
 */
int wstr_concat_cwstr(WString *wstr, const wchar_t *cat, unsigned n);

/**
 * Concatenates a WString at the end of another
 * @cat source WString
 * @str dest WString
 */
int wstr_concat_wstr(WString *wstr, WString *cat);

/**
 * Puts a char at the end of the WString
 */
int wstr_push_char(WString *wstr, wchar_t c);

/**
 * Removes the characater at the given index
 */
int wstr_remove_at(WString *str, unsigned index);

/**
 * Removes the range [start, end) from the WString
*/
int wstr_remove_range(WString *str, unsigned start, unsigned end);

/**
 * Gets the character at the given index
 */
wchar_t wstr_get_at(WString *wstr, unsigned index);

/**
 * Sets the character at the given index
 */
int wstr_set_at(WString *wstr, unsigned index, wchar_t c);

/**
 * Inserts the given cwstring at the given index.
 * @param n, max length of the insert string
 */
int wstr_insert(WString *wstr, const wchar_t *insert, unsigned n, unsigned index);

/**
 * Returns a cwtring copy of the given WString.
 * @note The cwstring is allocated using malloc
 */
wchar_t* wstr_to_cwstr(WString *wstr);

/**
 * Returns a pointer to the internal buffer of the WString.
 * A null terminator will be appended.
 */
const wchar_t* wstr_get_buffer(WString *wstr);

/**
 * Returns a substring of the WString in the range [start, end)
 */
wchar_t* wstr_substring(WString *wstr, unsigned start, unsigned end);

/**
 * Creates a copy of the given WString
 */
WString* wstr_dup(WString *wstr);

/**
 * Returns the length of the WString
 */
size_t wstr_length(WString *wstr);

/**
 * Splits the WString into substrings, using the characters
 * in tokens as dividers.
 * - If str is NULL, it continues on the string passed on the
 *   previous call.
 * - The returned cstring is malloc'd, but it's saved as a static variable,
 *   and will be freed on the next call, or when the function it's called with
 *   both str and tokens as NULL.
 * - When it reaches the end of the string, returns the remaining. After that, it
 *   returns NULL until a new string is provided.
 */
wchar_t* wstr_tok(WString *wstr, wchar_t *tokens);

/**
 * Transforms all the charcters in the WString, one by one,
 * using the given function.
 */
int wstr_transform(WString *wstr, wchar_t(*func)(wchar_t));

/**
 * Shrinks the given string to fit it's content
 */
void wstr_shrink(WString *wstr);

/**
 * Clears the WString, without shrinking or freeing the buffer.
 */
void wstr_clear(WString *wstr);

/**
 * Frees all the memory allocated for the string
 */
void wstr_free(WString *wstr);

#endif // WSTR_H
