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

/// Code adapted from https://github.com/luvit/pcre2/blob/a677f5b51bac251082856d35a48a01670e2fd4a7/src/pcre2demo.c#L61
size_t regex_match(const char *str, pcre2_code *re);

#endif // UTILS_H
