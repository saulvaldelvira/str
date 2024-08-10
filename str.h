/*
 * str.h - String definition.
 * Author: Saúl Valdelvira (2023)
 */
#pragma once
#ifndef STR_H
#define STR_H

#include <stddef.h> // size_t

typedef struct String String;

/**
 * Builds an empty String
 */
String* str_empty(void);

/**
 * Builds a String with the given initial size
 */
String* str_init(unsigned initial_size);

/**
 * Builds a String, from the given source cstring.
 * @param n max length of src
 */
String* str_from_cstr(const char *src, unsigned n);

/**
 * Reserves space in the String for n characters
 * @note n characters including the ones already in the String,
 *       it does not reserve space for n more characters.
 */
void str_reserve(String *str, unsigned n);

/**
 * Concatenates the given cstring at the end of the String
 * @param n max length of cat
 */
int str_concat_cstr(String *str, const char *cat, unsigned n);

/**
 * Concatenates a String at the end of another
 * @cat source String
 * @str dest String
 */
int str_concat_str(String *str, String *cat);

/**
 * Puts a char at the end of the String
 */
int str_push_char(String *str, char c);

/**
 * Removes the last character in the String
*/
int str_pop(String *str);

/**
 * Removes the characater at the given index
 */
int str_remove_at(String *str, unsigned index);

/**
 * Removes the range [start, end) from the String
*/
int str_remove_range(String *str, unsigned start, unsigned end);

/**
 * Gets the character at the given index
 */
char str_get_at(String *str, unsigned index);

/**
 * Sets the character at the given index
 */
int str_set_at(String *str, unsigned index, char c);

/**
 * Inserts the given cstring at the given index.
 * @param n, max length of the insert string
 */
int str_insert_cstr(String *str, const char *insert, unsigned n, unsigned index);

/**
 * Inserts char at the given index.
 * @param n, max length of the insert string
 */
int str_insert(String *str, char c, unsigned index);

/**
 * Returns a ctring copy of the given String.
 * @note The cstring is allocated using malloc
 */
char* str_to_cstr(String *str);

/**
 * Returns a pointer to the internal buffer of the String.
 * A null terminator will be appended.
 */
const char* str_get_buffer(String *str);

/**
 * Returns a substring of the String in the range [start, end)
 */
char* str_substring(String *str, unsigned start, unsigned end);

/**
 * Creates a copy of the given String
 */
String* str_dup(String *str);

/**
 * Returns the length of the String
 */
size_t str_length(String *str);

/**
 * Splits the String into substrings, using the characters
 * in tokens as dividers.
 * - If str is NULL, it continues on the string passed on the
 *   previous call.
 * - The returned cstring is malloc'd, but it's saved as a static variable,
 *   and will be freed on the next call, or when the function it's called with
 *   both str and tokens as NULL.
 * - When it reaches the end of the string, returns the remaining. After that, it
 *   returns NULL until a new string is provided.
 */
char* str_tok(String *str, char *tokens);

/**
 * Splits the String into an array of cstr, using delim
 * as a delimiter.
 * @param str the string to split
 * @param delim the delimiter
 * @return an array with the string split by delim.
 *         The last element is NULL.
*/
char** str_split(String *str, char *delim);

/**
 * Finds the first occurence of substr, starting at index [start_at]
 * @param substr string to search
 * @param start_at index of the String to start the search
 * @return Index of the first occurence of substr, or -1 if there isn't any
 */
int str_find_substring(String *str, const char *substr, unsigned start_at);

/**
 * Replaces any occurence of substr with replacement
 * @param substr string to replace
 * @param replacement replacement for substr
 * @return the number of matches
*/
int str_replace(String *wstr, const char *substr, const char *replacement);

/**
 * Transforms all the charcters in the String, one by one,
 * using the given function.
 */
int str_transform(String *str, char(*func)(char));

/**
 * Shrinks the given string to fit it's content
 */
void str_shrink(String *str);

/**
 * Clears the String, without shrinking or freeing the buffer.
 */
void str_clear(String *str);

/**
 * Frees all the memory allocated for the string
 */
void str_free(String *str, ...);

#define str_free(...) str_free(__VA_ARGS__, NULL)

/**
 * Frees multiple Strings at once
 * @param n number of String to free
 * @param ... all the Strings to free
*/
void str_free_all(unsigned int n, ...);

#endif // STR_H
