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

#define ARR_TAMANHO(arr) (sizeof arr / sizeof arr[0])

#define PMALLOC(arr, quantidade) arr = malloc(quantidade * sizeof *arr)
#define PCALLOC(arr, quantidade) arr = calloc(quantidade, sizeof *arr)
#define PREALLOC(arr, quantidade) arr = realloc(arr, quantidade * sizeof *arr)

#define regex_unicode_letter "\\p{Ll}\\p{Lu}\\p{Lo}_$"
#define REGEX_OPTIONS PCRE2_UTF | PCRE2_ANCHORED | PCRE2_NO_AUTO_CAPTURE

/// Cores
#define _COR(codigo, negrito) "\033[" #negrito codigo "m"
#define COR(codigo) _COR(codigo, 0)
#define COR_NEGRITO(codigo) _COR(codigo, 1)
#define _VERMELHO ";31"
#define _VERDE    ";32"
#define _AMARELO  ";33"
#define _AZUL     ";34"
#define _MAGENTA  ";35"
#define _CIANO    ";36"
#define _RESET    ""


char *file_to_str(const char *filename, size_t *len);

int8_t utf8_simbolo_comprimento(const char *str);

pcre2_code *regex_compile(const char *pattern, PCRE2_SIZE tamanho);

/// Code adapted from https://github.com/luvit/pcre2/blob/a677f5b51bac251082856d35a48a01670e2fd4a7/src/pcre2demo.c#L61
size_t regex_match(const char *str, pcre2_code *re, size_t subject_length);

#endif // UTILS_H
