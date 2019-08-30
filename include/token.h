/**
 * PCC - Paulo C Compiler
 *
 * token.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>
#include <stddef.h>
#include "utils.h"

enum {
	TK_ID,
	TK_KW,
	TK_CNST,
	TK_STR,
	TK_OP,
	TK_EXT
};

typedef struct token_t {
	int tipo;
	int subtipo;

	struct {
		char *lexema;
		size_t comprimento;

		struct {
			int32_t linha;
			int32_t coluna;
		} posicao;
	} contexto;

	struct {
		void *dados;
		size_t tamanho;
	} valor;
} token_t;


#endif // TOKEN_H
