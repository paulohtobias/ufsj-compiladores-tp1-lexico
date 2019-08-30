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

#include <stdio.h>
void print_afd(const afd_t *afd);

int afd_init(afd_t *afd, size_t quantidade_estados, int32_t estado_inicial) {
	if (estado_inicial >= quantidade_estados) {
		return AFD_EINVAL;
	}

	afd->estados_quantidade = 1; // Estado inicial.
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
	int res = AFD_OK;

	afd_transicao_t *transicao = NULL;

	// Pegando o estado inicial usando aritmética de ponteiros.
	afd_estado_t *estado_atual = &afd->estados[afd->estado_inicial - afd->estados];

	int32_t i = 0;
	for (i = 0; i < transicoes_quantidade - 1; i++) {
		size_t estado_atual_indice = estado_atual - afd->estados;
		transicao = afd_estado_get_transicao(estado_atual, transicoes[i].name);

		// Se a transição intermediária não existir, então criamos um novo estado.
		if (transicao == NULL) {
			afd_estado_t *novo_estado = NULL;
			PCALLOC(novo_estado, 1);
			novo_estado->transicoes_capacidade = 1;
			PCALLOC(novo_estado->transicoes, novo_estado->transicoes_capacidade);

			res = afd_add_estado(afd, transicoes, i + 1, novo_estado);

			// Como o valor do novo estado foi copiado para o vetor do automato,
			// podemos librerar a memória.
			free(novo_estado);

			// Um realloc pode ocorrer em afd_add_estado que fará com que o ponteiro
			// estado_atual esteja desatualizado.
			estado_atual = &afd->estados[estado_atual_indice];

			// Agora procuramos novamente a transição. Dessa vez não deve ser NULL.
			transicao = afd_estado_get_transicao(estado_atual, transicoes[i].name);
		}

		estado_atual = &afd->estados[transicao->estado_indice];
	}

	transicao = afd_estado_get_transicao(estado_atual, transicoes[i].name);

	if (transicao != NULL) {
		// TODO: tentar fundir.
		res = AFD_EEXISTE;
		goto fim;
	}

	// Efim é criada uma transição do estado atual para o novo estado.
	// transicoes[i] se refere à ultima instrução do vetor.
	PCALLOC(transicao, 1);
	transicao->pattern.name = transicoes[i].name;
	transicao->pattern.str = transicoes[i].str;
	transicao->pattern.compiled = regex_compile(transicao->pattern.str, PCRE2_ZERO_TERMINATED);
	transicao->estado_indice = afd->estados_quantidade++;

	// Adicionamos a nova transição no último estado.
	estado_atual->transicoes_quantidade++;
	// Alocamos mais memória para as transições, se necessário.
	if (estado_atual->transicoes_quantidade > estado_atual->transicoes_capacidade) {
		estado_atual->transicoes_capacidade += AFD_CAPACIDADE_INC;
		PREALLOC(estado_atual->transicoes, estado_atual->transicoes_capacidade);
	}
	estado_atual->transicoes[estado_atual->transicoes_quantidade - 1] = *transicao;

	// Alocamos mais memória para os estados, se necessário.
	if (afd->estados_quantidade >= afd->estados_capacidade) {
		afd->estados_capacidade += AFD_CAPACIDADE_INC;

		// Como o endereço do vetor de estados pode mudar, precisamos manter
		// o estado_inicial atualizado.
		size_t estado_inicial_indice = afd->estado_inicial - afd->estados;
		PREALLOC(afd->estados, afd->estados_capacidade);
		afd->estado_inicial = &afd->estados[estado_inicial_indice];
	}

	afd->estados[transicao->estado_indice] = *estado_ligacao;

	// Como o valor da nova transição foi copiado para o vetor do estado,
	// podemos librerar a memória.
	free(transicao);

fim:
	print_afd(afd);

	return res;
}

void print_afd(const afd_t *afd) {
	printf("============ AFD ===========\n");
	for (int i = 0; i < afd->estados_quantidade; i++) {
		const afd_estado_t *estado = &afd->estados[i];

		printf("ESTADO %d/%d (%d transições)\n", i + 1, afd->estados_quantidade, estado->transicoes_quantidade);

		for (int j = 0; j < estado->transicoes_quantidade; j++) {
			const afd_transicao_t *transicao = &estado->transicoes[j];

			printf("\t%d/%d: %s => %d\n", j + 1, estado->transicoes_quantidade, transicao->pattern.name, transicao->estado_indice + 1);
		}

		printf("\n");
	}
	printf("\n");
}

#undef AFD_CODIGOS
