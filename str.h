/*
 * str.h - string_t definition.
 * Author: Saúl Valdelvira (2023)
 */
#pragma once
#ifndef STR_H
#define STR_H

#include <stddef.h> // size_t

typedef struct string string_t;

/**
 * Builds an empty string_t
 */
string_t* str_empty(void);

/**
 * Builds a string_t with the given initial size
 */
string_t* str_init(unsigned initial_size);

/**
 * Builds a string_t, from the given source cstring.
 * @param n max length of src
 */
string_t* str_from_cstr(const char *src, unsigned n);

/**
 * Reserves space in the string_t for n characters
 * @note n characters including the ones already in the string_t,
 *       it does not reserve space for n more characters.
 */
void str_reserve(string_t *str, unsigned n);

/**
 * Concatenates the given cstring at the end of the string_t
 * @param n max length of cat
 */
int str_concat_cstr(string_t *str, const char *cat, unsigned n);

/**
 * Concatenates a string_t at the end of another
 * @cat source string_t
 * @str dest string_t
 */
int str_concat_str(string_t *str, string_t *cat);

/**
 * Puts a char at the end of the string_t
 */
int str_push_char(string_t *str, char c);

/**
 * Removes the last character in the string_t
*/
int str_pop(string_t *str);

/**
 * Removes the characater at the given index
 */
int str_remove_at(string_t *str, unsigned index);

/**
 * Removes the range [start, end) from the string_t
*/
int str_remove_range(string_t *str, unsigned start, unsigned end);

/**
 * Gets the character at the given index
 */
wchar_t str_get_at(string_t *str, unsigned index);

/**
 * Sets the character at the given index
 */
int str_set_at(string_t *str, unsigned index, wchar_t c);

/**
 * Inserts the given cstring at the given index.
 * @param n, max length of the insert string
 */
int str_insert_cstr(string_t *str, const char *insert, unsigned n, unsigned index);

/**
 * Inserts char at the given index.
 * @param n, max length of the insert string
 */
int str_insert(string_t *str, char c, unsigned index);

/**
 * Returns a ctring copy of the given string_t.
 * @note The cstring is allocated using malloc
 */
char* str_to_cstr(string_t *str);

/**
 * Returns a pointer to the internal buffer of the string_t.
 * A null terminator will be appended.
 */
const char* str_get_buffer(string_t *str);

const char* str_get_buffer_at(string_t *str, unsigned i);

size_t str_bytes_between(string_t *str, unsigned s, unsigned e);

/**
 * Returns a substring of the string_t in the range [start, end)
 */
char* str_substring(string_t *str, unsigned start, unsigned end);

/**
 * Creates a copy of the given string_t
 */
string_t* str_dup(string_t *str);

/**
 * Returns the length of the string_t
 */
/* size_t str_length(string_t *str); */

size_t str_length_utf8(string_t *str);

/**
 * Splits the string_t into substrings, using the characters
 * in tokens as dividers.
 * - If str is NULL, it continues on the string passed on the
 *   previous call.
 * - The returned cstring is malloc'd, but it's saved as a static variable,
 *   and will be freed on the next call, or when the function it's called with
 *   both str and tokens as NULL.
 * - When it reaches the end of the string, returns the remaining. After that, it
 *   returns NULL until a new string is provided.
 */
char* str_tok(string_t *str, char *tokens);

/**
 * Splits the string_t into an array of cstr, using delim
 * as a delimiter.
 * @param str the string to split
 * @param delim the delimiter
 * @return an array with the string split by delim.
 *         The last element is NULL.
*/
char** str_split(string_t *str, char *delim);

/**
 * Finds the first occurence of substr, starting at index [start_at]
 * @param substr string to search
 * @param start_at index of the string_t to start the search
 * @return Index of the first occurence of substr, or -1 if there isn't any
 */
int str_find_substring(string_t *str, const char *substr, unsigned start_at);

/**
 * Replaces any occurence of substr with replacement
 * @param substr string to replace
 * @param replacement replacement for substr
 * @return the number of matches
*/
int str_replace(string_t *wstr, const char *substr, const char *replacement);

char* str_cloned_cstr(string_t *str);

/**
 * Transforms all the charcters in the string_t, one by one,
 * using the given function.
 */
int str_transform(string_t *str, char(*func)(char));

int str_cmp_cstr(const string_t *str, const char *cstr);

/**
 * Shrinks the given string to fit it's content
 */
void str_shrink(string_t *str);

/**
 * Clears the string_t, without shrinking or freeing the buffer.
 */
void str_clear(string_t *str);

/**
 * Frees all the memory allocated for the string
 */
void str_free(string_t *str, ...);

#define str_free(...) str_free(__VA_ARGS__, NULL)

/**
 * Frees multiple string_ts at once
 * @param n number of string_t to free
 * @param ... all the string_ts to free
*/
void str_free_all(unsigned int n, ...);

#endif // STR_H
