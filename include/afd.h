/**
 * PCC - Paulo C Compiler
 *
 * afd.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef AFD_H
#define AFD_H

#include <stdint.h>
#include <stddef.h>

// X-macro para gerar os códigos e mensagens de erro.
#define AFD_CODIGOS \
	AFD_CODIGO(AFD_OK, "Nenhum erro") \
	AFD_CODIGO(AFD_EEXISTE, "Estado já existe e junção não foi possível")

#define AFD_CODIGO(cod, msg) cod,
enum {
	AFD_CODIGOS
};
#undef AFD_CODIGO

typedef struct afd_transicao_t {
	const char *pattern;
	int32_t estado_indice;
} afd_transicao_t;

typedef struct afd_estado_t {
	afd_transicao_t *transicoes;
	size_t transicoes_quantidade;

	void *(*gerar_dado)(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);
} afd_estado_t;

typedef struct afd_t {
	afd_estado_t *estados;
	size_t estados_quantidade;

	const afd_estado_t *estado_inicial;
} afd_t;

// TODO: funções básicas.

#endif // AFD_H
