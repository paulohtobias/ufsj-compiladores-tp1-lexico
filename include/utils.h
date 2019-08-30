/**
 * PCC - Paulo C Compiler
 *
 * utils.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdbool.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#define REGEX_OPTIONS PCRE2_UTF | PCRE2_ANCHORED | PCRE2_NO_AUTO_CAPTURE

#define PMALLOC(arr, quantidade) arr = malloc(quantidade * sizeof *arr)
#define PCALLOC(arr, quantidade) arr = calloc(quantidade, sizeof *arr)
#define PREALLOC(arr, quantidade) arr = realloc(arr, quantidade * sizeof *arr)

char *file_to_str(const char *filename, size_t *len);

pcre2_code *regex_compile(const char *pattern, PCRE2_SIZE tamanho);

size_t regex_match(const char *str, const char *pattern);

#endif // UTILS_H
