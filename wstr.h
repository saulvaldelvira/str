/*
 * wstr.h - string_t implementation.
 * Author: Saúl Valdelvira (2023)
 */
#pragma once
#include <wchar.h>
#ifndef WSTR_H
#define WSTR_H

#include <stddef.h> // size_t, wchar_t

typedef struct wstring wstring_t;

/**
 * Builds an empty wstring_t
 */
wstring_t* wstr_empty(void);

/**
 * Builds a wstring_t with the given initial size
 */
wstring_t* wstr_init(unsigned initial_size);

/**
 * Builds a wstring_t, from the given source cwstring.
 * @param n max length of src
 */
wstring_t* wstr_from_cwstr(const wchar_t *src, unsigned n);
wstring_t* wstr_from_cstr(const char *src, unsigned n);

/**
 * Reserves space in the wstring_t for n characters
 * @note n characters including the ones already in the string_t,
 *       it does not reserve space for n more characters.
 */
void wstr_reserve(wstring_t *wstr, unsigned n);

/**
 * Concatenates the given cwstring at the end of the wstring_t
 * @param n max length of cat
 */
int wstr_concat_cwstr(wstring_t *wstr, const wchar_t *cat, unsigned n);

/**
 * Concatenates the given cstring at the end of the wstring_t
 * @param n max length of cat
 */
int wstr_concat_cstr(wstring_t *wstr, const char *cat, unsigned n);

/**
 * Concatenates a wstring_t at the end of another
 * @cat source wstring_t
 * @str dest wstring_t
 */
int wstr_concat_wstr(wstring_t *wstr, wstring_t *cat);

/**
 * Puts a char at the end of the wstring_t
 */
int wstr_push_char(wstring_t *wstr, wchar_t c);

/**
 * Removes the last character in the wstring_t
*/
int wstr_pop(wstring_t *wstr);

/**
 * Removes the characater at the given index
 */
int wstr_remove_at(wstring_t *str, unsigned index);

/**
 * Removes the range [start, end) from the wstring_t
*/
int wstr_remove_range(wstring_t *wstr, unsigned start, unsigned end);

/**
 * Gets the character at the given index
 */
wchar_t wstr_get_at(const wstring_t *wstr, unsigned index);

/**
 * Sets the character at the given index
 */
int wstr_set_at(wstring_t *wstr, unsigned index, wchar_t c);

/**
 * Inserts the given cwstring at the given index.
 * @param n, max length of the insert string
 */
int wstr_insert_cwstr(wstring_t *wstr, const wchar_t *insert, unsigned n, unsigned index);
int wstr_insert_cstr(wstring_t *wstr, const char *insert, unsigned n, unsigned index);

/**
 * Inserts the given wchar at the given index.
 * @param n, max length of the insert string
 */
int wstr_insert(wstring_t *wstr, wchar_t c, unsigned index);

/**
 * Returns a cwtring copy of the given wstring_t.
 * @note The cwstring is allocated using malloc
 */
wchar_t* wstr_to_cwstr(const wstring_t *wstr);

/**
 * Returns a pointer to the internal buffer of the wstring_t.
 * A null terminator will be appended.
 */
const wchar_t* wstr_get_buffer(wstring_t *wstr);

/**
 * Returns a substring of the wstring_t in the range [start, end)
 */
wchar_t* wstr_substring(wstring_t *wstr, unsigned start, unsigned end);

/**
 * Creates a copy of the given wstring_t
 */
wstring_t* wstr_dup(wstring_t *wstr);

int wstr_cmp_cwstr(const wstring_t *wstr, const wchar_t *cwstr);

/**
 * Returns the length of the wstring_t
 */
size_t wstr_length(const wstring_t *wstr);

/**
 * Splits the wstring_t into substrings, using the characters
 * in tokens as dividers.
 * - If str is NULL, it continues on the string passed on the
 *   previous call.
 * - The returned cstring is malloc'd, but it's saved as a static variable,
 *   and will be freed on the next call, or when the function it's called with
 *   both str and tokens as NULL.
 * - When it reaches the end of the string, returns the remaining. After that, it
 *   returns NULL until a new string is provided.
 */
wchar_t* wstr_tok(wstring_t *wstr, wchar_t *tokens);

/**
 * Splits the wstring_t into an array of cwstr, using delim
 * as a delimiter.
 * @param wstr the string to split
 * @param delim the delimiter
 * @return an array with the string split by delim.
 *         The last element is NULL.
*/
wchar_t** wstr_split(wstring_t *wstr, wchar_t *delim);

/**
 * Finds the first occurence of substr, starting at index [start_at]
 * @param substr string to search
 * @param start_at index of the wstring_t to start the search
 * @return Index of the first occurence of substr, or -1 if there isn't any
 */
int wstr_find_substring(wstring_t *wstr, const wchar_t *substr, unsigned start_at);

/**
 * Replaces any occurence of substr with replacement
 * @param substr string to replace
 * @param replacement replacement for substr
 * @return the number of matches
*/
int wstr_replace(wstring_t *wstr, const wchar_t *substr, const wchar_t *replacement);

/**
 * Transforms all the charcters in the wstring_t, one by one,
 * using the given function.
 */
int wstr_transform(wstring_t *wstr, wchar_t(*func)(wchar_t));

/**
 * Shrinks the given string to fit it's content
 */
void wstr_shrink(wstring_t *wstr);

/**
 * Converts the given wstring_t into a regular wchar_t*
 * - The resulting string is properly NULL terminated
 * - The original wstring_t is not usable anymore
 * */
wchar_t* wstr_into_cwstr(wstring_t *wstr);

/**
 * Clones the given wstring_t into a regular wchar_t*
 * - The resulting string is properly NULL terminated
 * */
wchar_t* wstr_cloned_cwstr(wstring_t *wstr);

/**
 * Clears the wstring_t, without shrinking or freeing the buffer.
 */
void wstr_clear(wstring_t *wstr);

/**
 * Frees all the memory allocated for the string
 */
void wstr_free(wstring_t *wstr, ...);

#define wstr_free(...) wstr_free(__VA_ARGS__, NULL)

/**
 * Frees multiple wstring_ts at once
 * @param n number of wstring_t to free
 * @param ... all the wstring_ts to free
*/
void wstr_free_all(unsigned int n, ...);

#endif // WSTR_H
