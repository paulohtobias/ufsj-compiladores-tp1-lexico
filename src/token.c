/**
 * PCC - Paulo C Compiler
 *
 * token.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <stdlib.h>
#include <string.h>
#include "token.h"

void token_free(token_t *token) {
	if (token == NULL) {
		return;
	}

	free(token->str);
	free(token->valor);
	free(token);
}

static token_t *token_init(const char *tipo, const char *src, size_t comprimento, int32_t linha, int32_t coluna) {
	token_t *token = malloc(sizeof *token);

	token->tipo = tipo;

	token->str = malloc(comprimento + 1);
	strncpy(token->str, src, comprimento);
	token->str[comprimento] = '\0';

	token->linha = linha;
	token->coluna = coluna;

	token->valor = NULL;
	token->tamanho = 0;

	return token;
}

token_t *token_string_parse(const char *src, size_t comprimento, int32_t linha, int32_t coluna) {
	token_t *token = token_init("string", src, comprimento, linha, coluna);

	token->tamanho = comprimento - 1;
	token->valor = malloc(token->tamanho);
	strncpy((char *) token->valor, token->str + 1, token->tamanho - 1);
	((char *) token->valor)[token->tamanho - 1] = '\0';

	return token;
}
