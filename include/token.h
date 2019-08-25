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

#include <stddef.h>
#include <stdint.h>

struct token_vtable {
};

typedef struct token_t {
	/// Tipo do token. TODO: talvez mudar pra um enum?
	const char *tipo;


	/// Cópia do texto onde o token foi encontrado.
	char *str;

	/// Linha do arquivo onde o token foi encontrado.
	int32_t linha;

	/// Coluna do arquivo onde o token foi encontrado.
	int32_t coluna;


	/// Representação binária de `str`.
	void *valor;

	/// Tamanho em bytes de `valor`.
	size_t tamanho;


	/// Métodos.
	struct token_vtable *metodos;
} token_t;

void token_free(token_t *token);

token_t *token_string_parse(const char *src, size_t comprimento, int32_t linha, int32_t coluna);

#endif // TOKEN_H
