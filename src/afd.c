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

afd_estado_t afd_criar_estado(afd_transicao_t *transicoes, size_t transicoes_quantidade, bool final, void (*acao)(const char *arquivo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna)) {
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

int afd_mesclar_automatos(afd_t *afd, const afd_t *afd_novo) {
	int res = AFD_OK;

	// Lista com os novos índices para cada estado do no AFD.
	int32_t *afd_novo_indices = NULL;
	PMALLOC(afd_novo_indices, plist_len(afd_novo->estados));
	memset(afd_novo_indices, -1, plist_len(afd_novo->estados) * sizeof *afd_novo_indices);
	afd_novo_indices[0] = 0; // Estados iniciais são iguais.

	// Filas de índices
	int32_t afd_estado_atual_fila_indice = 0;
	int32_t *afd_estado_atual_fila = NULL;
	plist_create(afd_estado_atual_fila, 1);
	plist_append(afd_estado_atual_fila, 0);

	int32_t afd_novo_estado_atual_fila_indice = 0;
	int32_t *afd_novo_estado_atual_fila = NULL;
	plist_create(afd_novo_estado_atual_fila, 1);
	plist_append(afd_novo_estado_atual_fila, 0);

	// Encontrando os novos índices dos estados do novo AFD.
	for (int32_t i = 0; i < plist_len(afd_novo->estados); i++) {
		int32_t afd_estado_atual_indice = afd_estado_atual_fila[afd_estado_atual_fila_indice++];
		int32_t afd_novo_estado_atual_indice = afd_novo_estado_atual_fila[afd_novo_estado_atual_fila_indice++];

		//printf("Estados atuais:\n\tDEST: %d\n\tNOV: %d\n", afd_estado_atual_indice + 1, afd_novo_estado_atual_indice + 1);

		afd_estado_t *afd_estado_atual = &afd->estados[afd_estado_atual_indice];
		const afd_estado_t *afd_novo_estado_atual = &afd_novo->estados[afd_novo_estado_atual_indice];

		for (int32_t j = 0; j < plist_len(afd_novo_estado_atual->transicoes); j++) {
			const afd_transicao_t *afd_novo_transicao = &afd_novo_estado_atual->transicoes[j];

			int32_t novo_indice = afd_novo_indices[afd_novo_transicao->estado_indice];
			// Procura pela transição do AFD novo nas transições do estado atual.
			if (novo_indice == -1) {
				// Primeiro verifica se já existe uma transição igual no AFD destino.
				// Se houver, o índice usado será esse.
				afd_transicao_t *afd_transicao = afd_estado_get_transicao(afd_estado_atual, afd_novo_transicao->pattern.name);

				if (afd_transicao == NULL) {
					// Seta o índice do estado.
					afd_novo_indices[afd_novo_transicao->estado_indice] = plist_len(afd->estados);
					novo_indice = afd_novo_indices[afd_novo_transicao->estado_indice];

					// Adiciona um novo estado no AFD destino.
					plist_append(afd->estados, afd_criar_estado(NULL, 0, false, NULL));

					// A inserção na lista pode ter causado um realloc.
					// Isso abre a possibilidade do endereço mudar. Portanto,
					// é necessário redefinir o valor de afd_estado_atual.
					afd_estado_atual = &afd->estados[afd_estado_atual_indice];

					//printf("AFD destino NÃO possui transição %s\n\tNovo índice: %d\n", afd_novo_transicao->pattern.name, novo_indice + 1);
				} else {
					// Seta o índice do estado.
					novo_indice = afd_transicao->estado_indice;
					afd_novo_indices[afd_novo_transicao->estado_indice] = novo_indice;

					//printf("AFD destino possui transição %s\n", afd_novo_transicao->pattern.name);
				}

				plist_append(afd_estado_atual_fila, novo_indice);
				plist_append(afd_novo_estado_atual_fila, afd_novo_transicao->estado_indice);
			}
		}
	}

	for (int i = 0; i < plist_len(afd_novo->estados); i++) {
		//printf("%d => %d\n", i + 1, afd_novo_indices[i] + 1);
		afd_estado_t *afd_estado_atual = &afd->estados[afd_novo_indices[i]];
		const afd_estado_t *afd_novo_estado_atual = &afd_novo->estados[i];

		// Tenta mesclar os dois estados. A única restrição é que pelo
		// menos um dos dois não deve ser final.
		if (afd_estado_atual->final && afd_novo_estado_atual->final) {
			res = AFD_EEXISTE;
			goto fim;
		}

		// Se o estado do AFD novo for final, então o do destino também
		// será final.
		if (afd_novo_estado_atual->final) {
			afd_estado_atual->final = true;
			afd_estado_atual->acao = afd_novo_estado_atual->acao;
		} else if (afd_estado_atual->acao == NULL) {
			afd_estado_atual->acao = afd_novo_estado_atual->acao;
		}

		// Adiciona as transições para o novo estado.
		for (int j = 0; j < plist_len(afd_novo_estado_atual->transicoes); j++) {
			const afd_transicao_t *afd_novo_transicao = &afd_novo_estado_atual->transicoes[j];

			afd_transicao_t *afd_transicao = afd_estado_get_transicao(afd_estado_atual, afd_novo_transicao->pattern.name);
			if (afd_transicao == NULL) {
				afd_transicao_t afd_transicao_nova = *afd_novo_transicao;
				afd_transicao_nova.estado_indice = afd_novo_indices[afd_novo_transicao->estado_indice];

				plist_append(afd_estado_atual->transicoes, afd_transicao_nova);
			}
		}
	}

	//printf("DEST\n");
	//afd_print(afd);
fim:
	return res;
}

void afd_print(const afd_t *afd) {
	printf("============ AFD (%zu estados) ===========\n", plist_len(afd->estados));
	for (int i = 0; i < plist_len(afd->estados); i++) {
		const afd_estado_t *estado = &afd->estados[i];

		printf("ESTADO %d (%zu transições) %s\n",
			i, plist_len(estado->transicoes),
			estado->final ? "FINAL" : "");

		for (int j = 0; j < plist_len(estado->transicoes); j++) {
			const afd_transicao_t *transicao = &estado->transicoes[j];

			printf("\ttr %d \t %d => %d \t %s\n", j, i, transicao->estado_indice, transicao->pattern.name);
		}

		printf("\n");
	}
	printf("\n");
}

#undef AFD_CODIGOS
