/**
 * PCC - Paulo C Compiler
 *
 * afd.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <string.h>
#include "afd.h"
#include "utils.h"

#define AFD_CODIGO(cod, msg) msg,
const char __afd_msgs[][128] = {
	AFD_CODIGOS
};
#undef AFD_CODIGO

//afd_t *g_afd_lexico = NULL;

int afd_init(afd_t *afd, size_t quantidade_estados, int32_t estado_inicial) {
	if (estado_inicial >= quantidade_estados) {
		return AFD_EINVAL;
	}

	afd->estados_quantidade = 0;
	afd->estados_capacidade = quantidade_estados;
	PCALLOC(afd->estados, afd->estados_capacidade);
	afd->estado_inicial = afd->estados + estado_inicial;

	return AFD_OK;
}

afd_transicao_t *afd_estado_get_transicao(const afd_estado_t *estado, const char *transicao_id) {
	for (int32_t i = 0; i < estado->transicoes_quantidade; i++) {
		afd_transicao_t *transicao = &estado->transicoes[i];

		if (strcmp(transicao->pattern.name, transicao_id) == 0) {
			return transicao;
		}
	}

	return NULL;
}

int afd_add_estado(afd_t *afd, const afd_transicao_pattern_t *transicoes, int32_t transicoes_quantidade, afd_estado_t *estado_ligacao) {
	afd_estado_t *estado = NULL;
	const char *transicao_id = NULL;
	const char *pattern = NULL;

	int32_t i;
	for (i = 0; i < transicoes_quantidade - 1; i++) {
		transicao_id = transicoes[i].name;
		// TODO
	}
	transicao_id = transicoes[i].name;
	pattern = transicoes[i].str;


	afd_transicao_t *transicao = afd_estado_get_transicao(afd->estado_inicial, transicao_id);
	if (transicao != NULL) {
		// TODO: Tentar fazer o merge das transições de estado com as do estado de ligação.

		return AFD_EEXISTE;
	} else {
		estado = estado_ligacao;

		// Cria uma nova transição.
		PCALLOC(transicao, 1);
		transicao->pattern.name = transicao_id; // Talvez usar strdup ?
		transicao->pattern.str = pattern;
		transicao->pattern.compiled = regex_compile(transicao->pattern.str, 0);
		transicao->estado_indice = afd->estados_quantidade++;

		// Aumenta o vetor de estados caso a quantidade ultrapasse a capacidade.
		if (afd->estados_quantidade >= afd->estados_capacidade) {
			afd->estados_capacidade += AFD_CAPACIDADE_INC;
			PREALLOC(afd->estados, afd->estados_capacidade);
		}
	}

	afd->estados[transicao->estado_indice] = *estado;

	return AFD_OK;
}

#undef AFD_CODIGOS
