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
#include "afd.h"

// X-macro para gerar os códigos de token.
#define TOKEN_CODIGOS \
	TOKEN_CODIGO(TK_ID, identificador, "Identificador") \

/// Tipos básicos de token.
#define TOKEN_CODIGO(cod, nome, descricao) cod,
enum {
	TOKEN_CODIGOS

	TK_COUNT // Quantidade de tipos de token.
};
#undef TOKEN_CODIGO

typedef struct token_t {
	uint32_t tipo;
	uint32_t subtipo;

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

		char *(*to_str)(void *, size_t);
	} valor;

} token_t;


/// Lista de tokens.
extern token_t *lista_tokens;

/**
 * Tabela de símbolos. Armazena o índice do token na
 * lista de tokens para evitar redundância e reduzir
 * o gasto de memória.
 */
size_t *tabela_simbolos[TK_COUNT];


/**
 * Cria o autômato léxico para todos os tipos e sub-tipos
 * de token existente.
 */
int token_init(afd_t *afd);

/**
 * Função genérica para criar um novo token.
 */
token_t token_criar(uint32_t tipo, uint32_t subtipo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);

/**
 * Adiciona o token na lista de tokens bem como na
 * tabela de símbolos correspondente.
 */
void token_adicionar(const token_t *token);

/**
 * TODO
 */
void token_print(const token_t *token);

#endif // TOKEN_H
