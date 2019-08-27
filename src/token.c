/**
 * PCC - Paulo C Compiler
 *
 * token.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"
#include "utils.h"

static char parse_escape_sequence(const char **src);

void token_free(token_t *token) {
	if (token == NULL) {
		return;
	}

	free(token->src);
	free(token->valor);
	free(token);
}

static token_t *token_init(const char *tipo, const char *src, size_t comprimento, int32_t linha, int32_t coluna) {
	token_t *token = malloc(sizeof *token);

	token->tipo = tipo;

	token->src = malloc(comprimento + 1);
	strncpy(token->src, src, comprimento);
	token->src[comprimento] = '\0';

	token->linha = linha;
	token->coluna = coluna;

	token->valor = NULL;
	token->tamanho = 0;

	return token;
}

token_t *token_char_parse(const char *src, size_t comprimento, int32_t linha, int32_t coluna) {
	token_t *token = token_init("constant:character", src, comprimento, linha, coluna);

	token->tamanho = 1; // sizeof(char) é sempre 1.
	token->valor = malloc(1);

	src++;
	if (*src == '\\') {
		*((char *) token->valor) = parse_escape_sequence(&src);
	} else {
		*((char *) token->valor) = *src;
	}

	return token;
}

token_t *token_string_parse(const char *src, size_t comprimento, int32_t linha, int32_t coluna) {
	token_t *token = token_init("string", src, comprimento, linha, coluna);

	// Tamanho conta com o '\0' no final.
	token->tamanho = comprimento - 1;
	token->valor = malloc(token->tamanho);

	src++;
	size_t i;
	for (i = 0; *src != '\"'; i++) {
		if (*src == '\\') {
			((char *) token->valor)[i] = parse_escape_sequence(&src);
		} else {
			((char *) token->valor)[i] = *src;
		}

		// TODO: avançar em modo UTF-8
		src++;
	}
	((char *) token->valor)[i] = '\0';
	// TODO: realloc em token->valor ?

	return token;
}


/* FUNÇÕES ESTÁTICAS */
static char parse_escape_sequence(const char **src) {
	static char escape_table[][2] = {
		{'n', '\n'},
		{'t', '\t'},
		{'v', '\v'},
		{'b', '\b'},
		{'r', '\r'},
		{'f', '\f'},
		{'a', '\a'},
		{'\\', '\\'},
		{'?', '\?'},
		{'\'', '\''},
		{'\"', '\"'}
	};
	static const size_t escape_table_tamanho = ARR_SIZE(escape_table);

	(*src)++;
	// Procura na tabela pelos símbolos.
	for (int i = 0; i < escape_table_tamanho; i++) {
		if (escape_table[i][0] == **src) {
			return escape_table[i][1];
		}
	}

	// Identifica numerais.
	int base = -1;
	if (isdigit((unsigned char) **src)) {
		base = 8;
	} else if (**src == 'x') {
		(*src)++;
		base = 16;
	}

	if (base == -1) {
		// TODO: warning ou erro.
		return **src;
	}

	char *end = NULL;
	char c = strtol(*src, &end, base);

	// TODO: checar por overflow e dar warnings.

	*src = end - 1;

	return c;
}
