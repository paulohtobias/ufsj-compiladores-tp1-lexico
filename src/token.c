/**
 * PCC - Paulo C Compiler
 *
 * token.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

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


/// Lista de tokens.
token_t *lista_tokens = NULL;

/// Tabela de símbolos.
size_t **tabela_simbolos[TK_COUNT];


int token_init(afd_t *afd) {
	#define TOKEN_CODIGO(cod, nome, descricao) token_ ## nome ## _init(afd);
	TOKEN_CODIGOS
	#undef TOKEN_CODIGO

	return 0;
}

void token_finalizar() {
	for (size_t i = 0; i < plist_len(lista_tokens); i++) {
		token_liberar(lista_tokens + i);
	}

	for (size_t i = 0; i < TK_COUNT; i++) {
		for (int j = 0; j < plist_cap(tabela_simbolos[i]); j++) {
			plist_free(tabela_simbolos[i][j]);
		}
		plist_free(tabela_simbolos[i]);
	}
}

const char *token_tipo_str(const token_t *token) {
	if (token->tipo < TK_COUNT) {
		return __token_codigo_str[token->tipo];
	}

	return "";
}

const char *token_subtipo_str(const token_t *token) {
	if (token->subtipo_to_str != NULL) {
		return token->subtipo_to_str(token->subtipo);
	}

	return "";
}

token_t token_criar(uint32_t tipo, uint32_t subtipo, const char *arquivo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	token_t token;

	// Tipo e sub-tipo.
	token.tipo = tipo;
	token.subtipo = subtipo;
	token.subtipo_to_str = NULL;

	// Arquivo.
	token.contexto.arquivo = strdup(arquivo);

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

void token_liberar(token_t *token) {
	free(token->contexto.arquivo);
	free(token->contexto.lexema);
	free(token->valor.dados);
}

void token_adicionar(const token_t *token) {
	// Adiciona na lista de tokens.
	plist_append(lista_tokens, *token);

	// Adiciona na tabela de símbolos correspondente.
	plist_append(tabela_simbolos[token->tipo][token->subtipo], plist_len(lista_tokens) - 1);
}

void token_print(FILE *out, const token_t *token) {
	const char * subtipo = token_subtipo_str(token);
	if (*subtipo == '\0') {
		subtipo = "N/A";
	}

	char *_valor = NULL;
	if (token->valor.to_str != NULL) {
		_valor = token->valor.to_str(token->valor.dados, token->valor.tamanho);
	}

	fprintf(out,
		"TOKEN: %s\n"
		"\tSubtipo: %s\n"
		"\tArquivo: %s\n"
		"\tLinha: %u | Coluna: %u\n"
		"\tLexema: %s\n",
		__token_codigo_str[token->tipo],
		subtipo,
		token->contexto.arquivo,
		token->contexto.posicao.linha, token->contexto.posicao.coluna,
		token->contexto.lexema
	);
	if (_valor != NULL) {
		fprintf(out, "\tValor: %s\n\n", _valor);
	} else {
		fprintf(out, "\n");
	}

	free(_valor);
}
