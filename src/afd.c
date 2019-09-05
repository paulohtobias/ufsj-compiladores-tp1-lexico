/**
 * PCC - Paulo C Compiler
 *
 * afd.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <stdio.h>
#include <string.h>
#include "afd.h"
#include "utils.h"
#include "plist.h"

#define AFD_CODIGO(cod, msg) msg,
const char __afd_msgs[][128] = {
	AFD_CODIGOS
};
#undef AFD_CODIGO

const char *afd_get_erro(int cod) {
	if (cod >= 0 && cod < ARR_TAMANHO(__afd_msgs)) {
		return __afd_msgs[cod];
	} else {
		return "Código de erro inválido.";
	}
}

int afd_init(afd_t *afd, size_t quantidade_estados) {
	afd->estados = NULL;

	plist_create(afd->estados, quantidade_estados);

	// Zerando todos os estados.
	memset(afd->estados, 0, quantidade_estados * sizeof *afd->estados);

	// Inicializando o estado inicial
	plist_append(
		afd->estados,
		afd_criar_estado(
			NULL, 0, false, NULL
		)
	);

	return AFD_OK;
}

afd_estado_t afd_criar_estado(afd_transicao_t *transicoes, size_t transicoes_quantidade, bool final, void (*acao)(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna)) {
	afd_estado_t estado;

	estado.final = final;
	estado.acao = acao;

	// Transições.
	estado.transicoes = NULL;
	if (transicoes != NULL && transicoes_quantidade > 0) {
		plist_create(estado.transicoes, transicoes_quantidade);

		for (size_t i = 0; i < transicoes_quantidade; i++) {
			afd_transicao_t transicao;

			transicao.estado_indice = transicoes[i].estado_indice;
			transicao.pattern.name = strdup(transicoes[i].pattern.name);
			transicao.pattern.compiled = regex_compile(transicoes[i].pattern.str, PCRE2_ZERO_TERMINATED);

			plist_insert(estado.transicoes, transicao, i);
		}
	}

	return estado;
}

void afd_liberar_estado(afd_estado_t *estado) {
	for (size_t i = 0; i < plist_len(estado->transicoes); i++) {
		free(estado->transicoes[i].pattern.name);
		pcre2_code_free(estado->transicoes[i].pattern.compiled);
	}
	free(estado->transicoes);
}

int32_t afd_estado_get_proximo_estado(const afd_estado_t *estado, const char *simbolo, int8_t simbolo_comprimento) {
	for (size_t i = 0; i < plist_len(estado->transicoes); i++) {
		const afd_transicao_t *transicao = &estado->transicoes[i];

		// Faz o casamento e anda no texto.
		size_t offset = regex_match(simbolo, transicao->pattern.compiled, simbolo_comprimento);

		// Se houve um casamento, então retornamos o estado.
		if (offset > 0) {
			return transicao->estado_indice;
		}
	}

	// Nenhuma transição foi encontrada.
	return -1;
}

afd_transicao_t *afd_estado_get_transicao(const afd_estado_t *estado, const char *transicao_id) {
	for (int32_t i = 0; i < plist_len(estado->transicoes); i++) {
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

	// Estado inicial.
	afd_estado_t *estado_atual = afd->estados;

	int32_t i = 0;
	for (i = 0; i < transicoes_quantidade - 1; i++) {
		size_t estado_atual_indice = estado_atual - afd->estados;
		transicao = afd_estado_get_transicao(estado_atual, transicoes[i].name);

		// Se a transição intermediária não existir, então criamos um novo estado.
		if (transicao == NULL) {
			afd_t novo_sub;
			afd_init(&novo_sub, 1);
			plist_create(novo_sub.estados[0].transicoes, 1);
			memset(novo_sub.estados[0].transicoes, 0, sizeof *novo_sub.estados[0].transicoes);

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
	transicao->estado_indice = plist_len(afd->estados);

	// Adicionamos a nova transição no último estado.
	plist_append(estado_atual->transicoes, *transicao);

	// Os novos estados são adicionados enquanto o índice de
	// suas transições é consertado.
	for (i = 0; i < plist_len(sub->estados); i++) {
		for (int32_t j = 0; j < plist_len(sub->estados[i].transicoes); j++) {
			sub->estados[i].transicoes[j].estado_indice += transicao->estado_indice;
		}

		plist_insert(afd->estados, sub->estados[i], transicao->estado_indice + i);
	}

	// Como o valor da nova transição foi copiado para o vetor do estado,
	// podemos librerar a memória.
	free(transicao);

fim:
	afd_print(afd);

	return res;
}

void afd_print(const afd_t *afd) {
	printf("============ AFD ===========\n");
	for (int i = 0; i < plist_len(afd->estados); i++) {
		const afd_estado_t *estado = &afd->estados[i];

		printf("ESTADO %d/%zu (%zu transições) %s\n",
			i + 1, plist_len(afd->estados), plist_len(estado->transicoes),
			estado->final ? "FINAL" : "");

		for (int j = 0; j < plist_len(estado->transicoes); j++) {
			const afd_transicao_t *transicao = &estado->transicoes[j];

			printf("\t%d/%zu \t %d => %d \t %s\n", j + 1, plist_len(estado->transicoes), i + 1, transicao->estado_indice + 1, transicao->pattern.name);
		}

		printf("\n");
	}
	printf("\n");
}

#undef AFD_CODIGOS
