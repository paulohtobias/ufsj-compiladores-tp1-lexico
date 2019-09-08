/**
 * PCC - Paulo C Compiler
 *
 * log.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include "log.h"

uint32_t _log_warnings = 0;
uint32_t _log_erros = 0;

void log_print_linha(const char *src, size_t comprimento, const char *cor_tipo, int32_t coluna) {
	// TODO

	fprintf(stderr, "\n%.*s\n", (int) comprimento, src);
}
