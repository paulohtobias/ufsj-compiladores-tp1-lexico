/**
 * PCC - Paulo C Compiler
 *
 * lexico.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include "lexico.h"

int lexico_parse(const char *src) {
	// Inicializando o AFD.
	afd_t afd;
	afd_init(&afd, 1, 0);

	// Inicializando os módulos de token.
	token_init(&afd);

	// Variáveis de contexto.
	const char *sentinela = src;
	int32_t linha = 1;
	int32_t coluna = 1;

	// DELETE-ME
	afd_print(&afd);

	afd_estado_t *estado_atual = &afd.estados[afd.estado_inicial - afd.estados];
	while (*src != '\0') {
		int32_t estado_indice = afd_estado_get_proximo_estado(estado_atual, &src);
		if (estado_indice >= 0) {
			estado_atual = &afd.estados[estado_indice];
		} else {
			/*
			 * estado_atual->acao != NULL pode significar duas coisas:
			 *   1: estado é final e um novo token será criado.
			 *   2: estado não é final mas possui mensagem de erro contextualizada.
			 *
			 * Ambas funções (criar novo estado e logar erro) possuem mesma assinatura.
			 * Portanto, não é necessário verificar nada além da existência
			 * da função.
			 */
			if (estado_atual->acao != NULL) {
				estado_atual->acao(sentinela, src - sentinela, linha, coluna);
			}

			if (*src == '\n') {
				linha++;
				coluna = 0;
			} else {
				coluna++;
			}
			// TODO: avançar em modo utf8
			src++;

			sentinela = src;

			// Estado atual volta a ser o inicial.
			estado_atual = &afd.estados[afd.estado_inicial - afd.estados];
		}
	}
	return 0;
}
