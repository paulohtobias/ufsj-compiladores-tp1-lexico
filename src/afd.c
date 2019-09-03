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

const char *afd_get_erro(int cod) {
	if (cod >= 0 && cod < sizeof __afd_msgs / sizeof __afd_msgs[0]) {
		return __afd_msgs[cod];
	} else {
		return "Código de erro inválido.";
	}
}

//afd_t *g_afd_lexico = NULL;

#include <stdio.h>
void afd_print(const afd_t *afd);

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

afd_estado_t afd_criar_estado(afd_transicao_t *transicoes, size_t transicoes_quantidade, bool final, void (*acao)(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna)) {
	afd_estado_t estado;

	estado.final = final;
	estado.acao = acao;

	// Transições.
	estado.transicoes = NULL;
	estado.transicoes_capacidade = estado.transicoes_quantidade = transicoes_quantidade;
	if (transicoes != NULL && transicoes_quantidade > 0) {
		PMALLOC(estado.transicoes, estado.transicoes_capacidade);

		for (size_t i = 0; i < transicoes_quantidade; i++) {
			estado.transicoes[i].estado_indice = transicoes[i].estado_indice;
			estado.transicoes[i].pattern.name = strdup(transicoes[i].pattern.name);
			estado.transicoes[i].pattern.compiled = regex_compile(transicoes[i].pattern.str, PCRE2_ZERO_TERMINATED);
		}
	}

	return estado;
}

void afd_liberar_estado(afd_estado_t *estado) {
	for (size_t i = 0; i < estado->transicoes_quantidade; i++) {
		free(estado->transicoes[i].pattern.name);
		pcre2_code_free(estado->transicoes[i].pattern.compiled);
	}
	free(estado->transicoes);
}

int32_t afd_estado_get_proximo_estado(const afd_estado_t *estado, const char **cursor) {
	for (size_t i = 0; i < estado->transicoes_quantidade; i++) {
		const afd_transicao_t *transicao = &estado->transicoes[i];

		// Faz o casamento e anda no texto.
		size_t offset = regex_match(*cursor, transicao->pattern.compiled);
		(*cursor) += offset;

		// Se houve um casamento, então retornamos o estado.
		if (offset > 0) {
			return transicao->estado_indice;
		}
	}

	// Nenhuma transição foi encontrada.
	return -1;
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

int afd_add_subautomato(afd_t *afd, const afd_transicao_pattern_t *transicoes, int32_t transicoes_quantidade, afd_t *sub) {
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
			afd_t novo_sub;
			novo_sub.estados_capacidade = novo_sub.estados_quantidade = 1;
			PCALLOC(novo_sub.estados, novo_sub.estados_capacidade);
			novo_sub.estados[0].transicoes_capacidade = 1;
			PCALLOC(novo_sub.estados[0].transicoes, novo_sub.estados[0].transicoes_capacidade);

			res = afd_add_subautomato(afd, transicoes, i + 1, &novo_sub);

			// Um realloc pode ocorrer em afd_add_subautomato que fará com que o ponteiro
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
	transicao->pattern.name = strdup(transicoes[i].name);
	transicao->pattern.compiled = regex_compile(transicoes[i].str, PCRE2_ZERO_TERMINATED);
	transicao->estado_indice = afd->estados_quantidade;
	afd->estados_quantidade += sub->estados_quantidade;

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

	// Arruma o índice de todas as transições do sub-autômato.
	for (i = 0; i < sub->estados_quantidade; i++) {
		for (int32_t j = 0; j < sub->estados[i].transicoes_quantidade; j++) {
			sub->estados[i].transicoes[j].estado_indice += transicao->estado_indice;
		}
	}

	// Copia os estados do sub-afd para o afd principal.
	memcpy(&afd->estados[transicao->estado_indice], sub->estados, sub->estados_quantidade * sizeof *sub->estados);

	// Como o valor da nova transição foi copiado para o vetor do estado,
	// podemos librerar a memória.
	free(transicao);

fim:
	afd_print(afd);

	return res;
}

void afd_print(const afd_t *afd) {
	printf("============ AFD ===========\n");
	printf("Estado inicial: %ld\n", afd->estado_inicial - afd->estados + 1);
	for (int i = 0; i < afd->estados_quantidade; i++) {
		const afd_estado_t *estado = &afd->estados[i];

		printf("ESTADO %d/%zu (%zu transições) %s\n",
			i + 1, afd->estados_quantidade, estado->transicoes_quantidade,
			estado->final ? "FINAL" : "");

		for (int j = 0; j < estado->transicoes_quantidade; j++) {
			const afd_transicao_t *transicao = &estado->transicoes[j];

			printf("\t%d/%zu \t %d => %d \t %s\n", j + 1, estado->transicoes_quantidade, i + 1, transicao->estado_indice + 1, transicao->pattern.name);
		}

		printf("\n");
	}
	printf("\n");
}

#undef AFD_CODIGOS
