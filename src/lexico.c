/**
 * PCC - Paulo C Compiler
 *
 * lexico.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <string.h>
#include <ctype.h>
#include "lexico.h"
#include "utils.h"

afd_t *afd_lexico = NULL;
int lexico_init() {
	// TODO: liberar o afd caso aconteça algum erro.
	int res = 0;

	PMALLOC(afd_lexico, 1);

	if ((res = afd_init(afd_lexico, 1)) != AFD_OK) {
		LOG_PCC_ERRO(res, NULL, "Erro ao inicializar o AFD: %s\n", afd_get_erro(res));
		return res;
	}

	// Inicializando os módulos de token.
	if ((res = token_init(afd_lexico)) != 0) {
		return 1;
	}

	// DELETE-ME
	//afd_print(afd_lexico);
	//exit(0);

	return 0;
}

/// Avança no código fonte.
static int avancar_cursor(char **src, int8_t comprimento, int32_t *linha, int32_t *coluna, token_contexto_t *contexto) {
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

static void ignorar_espacos(char **src, int32_t *linha, int32_t *coluna) {
	while (isspace((unsigned int) **src)) {
		avancar_cursor(src, -1, linha, coluna, NULL);
	}
}

int lexico_parse(const char *nome_arquivo) {
	// Carregando o arquivo para a memória.
	size_t comprimento;
	char *codigo_fonte = file_to_str(nome_arquivo, &comprimento);
	if (codigo_fonte == NULL) {
		LOG_PCC_ERRO(0, "", "Não foi possível abrir o arquivo '%s': ", nome_arquivo);
		return 1;
	}

	char *src = codigo_fonte;

	// Contadores de linha e coluna.
	int32_t linha = 1;
	int32_t coluna = 1;

	// Ignorando espaços iniciais.
	ignorar_espacos(&src, &linha, &coluna);

	// Variáveis de contexto.
	token_contexto_t contexto;
	contexto.arquivo = strdup(nome_arquivo);
	contexto.lexema = src;
	contexto.comprimento = 0;
	contexto.posicao.linha = linha;
	contexto.posicao.coluna = coluna;

	// Estado inicial.
	afd_estado_t *estado_atual = afd_lexico->estados;

	int8_t simbolo_comprimento = 1;
	bool moveu = false;
	while (*src != '\0') {
		simbolo_comprimento = utf8_simbolo_comprimento(src);
		int32_t estado_indice = afd_estado_get_proximo_estado(estado_atual, src, simbolo_comprimento);

		if (estado_indice >= 0) {
			estado_atual = &afd_lexico->estados[estado_indice];
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
					contexto.arquivo,
					contexto.lexema, contexto.comprimento,
					contexto.posicao.linha, contexto.posicao.coluna
				);
			} else {
				if (!moveu) {
					LOG_ERRO(contexto.arquivo, contexto.posicao.linha, contexto.posicao.coluna, contexto.lexema, contexto.comprimento, "símbolo '%.*s' inválido", simbolo_comprimento, contexto.lexema);
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
			estado_atual = afd_lexico->estados;

			moveu = false;
		}
	}

	// Se existia alguma ação a ser executada.
	if (estado_atual->acao != NULL) {
		estado_atual->acao(
			contexto.arquivo,
			contexto.lexema, contexto.comprimento,
			contexto.posicao.linha, contexto.posicao.coluna
		);
	}
	free(codigo_fonte);
	free(contexto.arquivo);

	return 0;
}
