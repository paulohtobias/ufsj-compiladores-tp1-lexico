/**
 * PCC - Paulo C Compiler
 *
 * afd.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include "afd.h"

// Funções de transição.
int32_t afd_transicao_match_interval(const struct afd_transicao_t *transicao, int simbolo) {
	const char *intervalo = transicao->dados;

	// intervalo NULL: casa com qualquer símbolo.
	if (intervalo == NULL) {
		return transicao->estado;
	}

	while (*intervalo != '\0') {
		if (*intervalo == simbolo) {
			return transicao->estado;
		}

		// TODO: próximo char utf-8.
		intervalo++;
	}

	return -1;
}

int32_t afd_transicao_match_interval_not(const struct afd_transicao_t *transicao, int simbolo) {
	return afd_transicao_match_interval(transicao, simbolo) == -1 ? transicao->estado : -1;
}


token_t *afd_get_token(const afd_t *afd, const char **pstr) {
	static int32_t linha = 1;
	static int32_t coluna = 1;

	const char *str = *pstr;

	const afd_estado_t *estado_atual = afd->estado_inicial;
	token_t *token = NULL; // TODO: talvez criar tipo erro?

	while (**pstr != '\0') {
		//printf("%s\n", *pstr);
		//printf("%c\n", **pstr);

		bool mudanca = false;
		for (int32_t i = 0; i < estado_atual->transicoes_quantidade; i++) {
			const afd_transicao_t *transicao = &estado_atual->transicoes[i];

			if (transicao->match(transicao, **pstr) != -1) {
				//printf("Indo para o estado %d\n", transicao->estado);
				estado_atual = &AFD_GET_ESTADO(afd, transicao->estado);
				mudanca = true;
				break;
			}
		}

		//getchar();

		//printf("EA: %d/%d\n", mudanca, estado_atual->transicoes_quantidade);
		//getchar();

		// Se o símbolo não leva a nenhum estado.
		if (!mudanca) {
			// TODO: Pensar num jeito de logar erros contextuais
			// caso o estado não seja final.
			if (estado_atual->final) {
				token_free(token);

				token = estado_atual->parse(str, *pstr - str, linha, coluna);
			}

			// TODO: atualizar linha e coluna.
			if (**pstr == '\n') {
				linha++;
				coluna = 0;
			} else {
				coluna++;
			}

			// TODO: avançar em modo utf-8.
			(*pstr)++;

			// token será NULL se o estado não for final.
			return token;
		}

		// TODO: avançar em modo utf-8.
		(*pstr)++;

	}

	return token;
}
