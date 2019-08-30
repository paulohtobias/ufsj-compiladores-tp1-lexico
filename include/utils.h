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

#define PMALLOC(arr, size) arr = malloc(size * sizeof *arr)
#define PCALLOC(arr, size) arr = calloc(size, sizeof *arr)

char *file_to_str(const char *filename, size_t *len);

size_t regex_match(const char *str, const char *pattern);

#endif // UTILS_H
