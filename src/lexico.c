/**
 * PCC - Paulo C Compiler
 *
 * lexico.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <stdio.h>
#include <ctype.h>
#include "lexico.h"
#include "utils.h"

/// Avança no código fonte.
static int avancar_cursor(const char **src, int8_t comprimento, int32_t *linha, int32_t *coluna, token_contexto_t *contexto) {
	if (comprimento <= 0) {
		comprimento = utf8_simbolo_comprimento(*src);
	}

	if (**src == '\n') {
		(*linha)++;
		(*coluna) = 1;
	} else {
		(*coluna) += comprimento;
	}

	(*src) += comprimento;

	if (contexto != NULL) {
		contexto->comprimento += comprimento;
	}

	return comprimento;
}

static void ignorar_espacos(const char **src, int32_t *linha, int32_t *coluna) {
	while (isspace((unsigned int) **src)) {
		avancar_cursor(src, -1, linha, coluna, NULL);
	}
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

	// Ignorando espaços iniciais.
	ignorar_espacos(&src, &linha, &coluna);

	// Variáveis de contexto.
	token_contexto_t contexto;
	contexto.lexema = src;
	contexto.comprimento = 0;
	contexto.posicao.linha = linha;
	contexto.posicao.coluna = coluna;

	// DELETE-ME
	afd_print(&afd);

	// Estado inicial.
	afd_estado_t *estado_atual = afd.estados;

	int8_t simbolo_comprimento = 1;
	bool moveu = false;
	while (*src != '\0') {
		simbolo_comprimento = utf8_simbolo_comprimento(src);
		int32_t estado_indice = afd_estado_get_proximo_estado(estado_atual, &src);

		if (estado_indice >= 0) {
			estado_atual = &afd.estados[estado_indice];
			avancar_cursor(&src, simbolo_comprimento, &linha, &coluna, &contexto);
			moveu = true;
		} else {
			/*
			 * estado_atual->acao != NULL pode significar duas coisas:
			 *   1: estado é final e um novo token será criado.
			 *   2: estado não é final mas possui mensagem de erro contextualizada.
			 *
			 * Ambas funções (criar novo estado e logar erro) possuem mesma assinatura.
			 * Portanto, não é necessário verificar se o estado é final.
			 */
			if (estado_atual->acao != NULL) {
				estado_atual->acao(
					contexto.lexema, contexto.comprimento,
					contexto.posicao.linha, contexto.posicao.coluna
				);
			} else {
				if (!moveu) {
					fprintf(stderr, "Símbolo '%.*s' inválido na linha %d coluna %d.\n", simbolo_comprimento, contexto.lexema, contexto.posicao.linha, contexto.posicao.coluna);
				}
			}

			/*
			 * Se houve uma transição antes do erro, então o símbolo é
			 * considerado como um separador e não será consumido.
			 */
			if (!moveu) {
				avancar_cursor(&src, simbolo_comprimento, &linha, &coluna, &contexto);
			}

			ignorar_espacos(&src, &linha, &coluna);

			// Resetando o contexto.
			contexto.lexema = src;
			contexto.comprimento = 0;
			contexto.posicao.linha = linha;
			contexto.posicao.coluna = coluna;

			// Estado atual volta a ser o inicial.
			estado_atual = afd.estados;

			moveu = false;
		}
	}
	return 0;
}
