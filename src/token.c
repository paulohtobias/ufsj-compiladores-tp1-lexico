/**
 * PCC - Paulo C Compiler
 *
 * token.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <stdio.h>
#include <string.h>
#include "token.h"
#include "utils.h"
#include "plist.h"

/// Descrição dos códigos.
#define TOKEN_CODIGO(cod, nome, descricao) descricao,
const char __token_codigo_str[][64] = {
	TOKEN_CODIGOS
};
#undef TOKEN_CODIGO

/// Funções init.
#define TOKEN_CODIGO(cod, nome, descricao) extern int token_ ## nome ## _init(afd_t *afd);
TOKEN_CODIGOS
#undef TOKEN_CODIGO


/// Lista de tokens.
token_t *lista_tokens = NULL;

/// Tabela de símbolos.
size_t *tabela_simbolos[TK_COUNT];


int token_init(afd_t *afd) {
	// Inicializando a tabela de símbolos.
	for (int i = 0; i < TK_COUNT; i++) {
		tabela_simbolos[i] = NULL;
	}


	#define TOKEN_CODIGO(cod, nome, descricao) token_ ## nome ## _init(afd);
	TOKEN_CODIGOS
	#undef TOKEN_CODIGO

	return 0;
}

/// FUNÇÕES
token_t token_criar(uint32_t tipo, uint32_t subtipo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	token_t token;

	// Tipo e sub-tipo.
	token.tipo = tipo;
	token.subtipo = subtipo;

	// Lexema.
	token.contexto.comprimento = comprimento;
	PMALLOC(token.contexto.lexema, token.contexto.comprimento + 1);
	strncpy(token.contexto.lexema, lexema, comprimento);
	token.contexto.lexema[comprimento] = '\0';

	// Posição.
	token.contexto.posicao.linha = linha;
	token.contexto.posicao.coluna = coluna;

	// Valor.
	token.valor.dados = NULL;
	token.valor.tamanho = 0;
	token.valor.to_str = NULL;

	return token;
}

void token_adicionar(const token_t *token) {
	// Adiciona na lista de tokens.
	plist_append(lista_tokens, *token);

	// Adiciona na tabela de símbolos correspondente.
	plist_append(tabela_simbolos[token->tipo], plist_len(lista_tokens) - 1);
}

void token_print(const token_t *token) {
	char *valor = NULL;
	if (token->valor.to_str != NULL) {
		// TODO
	}
	printf(
		"TOKEN: %s\n"
		"\tlinha: %u | coluna: %u\n"
		"\tlexema: %s\n\n",
		__token_codigo_str[token->tipo],
		token->contexto.posicao.linha, token->contexto.posicao.coluna,
		token->contexto.lexema
	);
}
