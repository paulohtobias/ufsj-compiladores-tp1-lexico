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

#include <stdbool.h>
#include "token.h"

typedef struct afd_transicao_t {
	int32_t (*match)(const struct afd_transicao_t *, int);
	const void *dados;
	int32_t estado;
} afd_transicao_t;

typedef struct afd_estado_t {
	afd_transicao_t *transicoes;
	int32_t transicoes_quantidade;

	bool final;
	token_t *(*parse)(const char *, size_t, int32_t, int32_t);
} afd_estado_t;

typedef struct afd_t {
	afd_estado_t *estados;
	int32_t estados_quantidade;

	const afd_estado_t *estado_inicial;
} afd_t;

#define AFD_GET_ESTADO(afd, indice) ((afd)->estados[indice])

int32_t afd_transicao_match_interval(const struct afd_transicao_t *transicao, int simbolo);
int32_t afd_transicao_match_interval_not(const struct afd_transicao_t *transicao, int simbolo);

token_t *afd_get_token(const afd_t *afd, const char **pstr);

#endif // AFD_H
