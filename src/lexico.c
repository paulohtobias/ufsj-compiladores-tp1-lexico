/**
 * PCC - Paulo C Compiler
 *
 * lexico.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include "lexico.h"
#include <stdio.h>

static int avancar_cursor(const char **src, int32_t *linha, int32_t *coluna, token_contexto_t *contexto) {
	// Avança no código fonte.
	int utf8_incremento = 1; // TODO
	if (**src == '\n') {
		(*linha)++;
		(*coluna) = 1;
	} else {
		(*coluna) += utf8_incremento;
	}
	(*src) += utf8_incremento;
	contexto->comprimento += utf8_incremento;

	return utf8_incremento;
}

int lexico_parse(const char *src) {
	// Inicializando o AFD.
	afd_t afd;
	afd_init(&afd, 1);

	// Inicializando os módulos de token.
	token_init(&afd);

	// Contadores de linha e coluna.
	int32_t linha = 1;
	int32_t coluna = 1;

	// Variáveis de contexto.
	token_contexto_t contexto;
	contexto.lexema = src;
	contexto.comprimento = 0;
	contexto.posicao.linha = 1;
	contexto.posicao.coluna = 1;

	// DELETE-ME
	afd_print(&afd);

	afd_estado_t *estado_atual = afd.estados;
	while (*src != '\0') {
		int32_t estado_indice = afd_estado_get_proximo_estado(estado_atual, &src);

		if (estado_indice >= 0) {
			avancar_cursor(&src, &linha, &coluna, &contexto);
			estado_atual = &afd.estados[estado_indice];
		} else {
			/*
			 * Existem duas possibilidades quando uma transição não
			 * existe para um símbolo. Se o estado for final, então
			 * o símbolo age como um separador e, por isso, não avançamos
			 * no código, pois este "separador" pode fazer parte do próximo
			 * token. Se o estado não for final
			*/
			if (!estado_atual->final) {
				avancar_cursor(&src, &linha, &coluna, &contexto);
			}

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
				estado_atual->acao(
					contexto.lexema, contexto.comprimento,
					contexto.posicao.linha, contexto.posicao.coluna
				);
			} else {
				fprintf(stderr, "Símbolo '%.*s' inválido na linha %d coluna %d.\n", (int) contexto.comprimento, contexto.lexema, contexto.posicao.linha, contexto.posicao.coluna);
			}

			// Se o estado for final, precisamos ignorar o avanço no texto
			// pois o símbolo não foi de fato consumido.


			// Resetando o contexto.
			contexto.lexema = src;
			contexto.comprimento = 0;
			contexto.posicao.linha = linha;
			contexto.posicao.coluna = coluna;

			// Estado atual volta a ser o inicial.
			estado_atual = afd.estados;
		}
	}
	return 0;
}
