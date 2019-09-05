/**
 * PCC - Paulo C Compiler
 *
 * token/palavra-chave.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include "token/palavra-chave.h"
#define PALAVRA_CHAVE_SUBTIPO(cod, str) str,
const char __palavras_chave[][16] = {
	PALAVRA_CHAVE_SUBTIPOS
};
#undef PALAVRA_CHAVE_SUBTIPO
size_t __palavras_chave_quantidade = sizeof __palavras_chave / sizeof __palavras_chave[0];

int token_palavra_chave_init() {
	return 0;
}

int token_palavra_chave_buscar(const token_t *token) {
	int inicio = 0;
	int fim = __palavras_chave_quantidade;
	int index;

	while (1) {
		int novo_index = (int) ((inicio + fim) / 2);
		if (novo_index == index) {
			return -1;
		}

		index = novo_index;

		int i;
		for (i = 0; token->contexto.lexema[i] != '\0' && __palavras_chave[index][i] != '\0'; i++) {
			if (token->contexto.lexema[i] < __palavras_chave[index][i]) {
				fim = index;
				break;
			} else if (token->contexto.lexema[i] > __palavras_chave[index][i]) {
				inicio = index;
				break;
			}
		}

		if (token->contexto.lexema[i] == '\0' && __palavras_chave[index][i] == '\0') {
			return index;
		}
	}
}

const char *token_palavra_chave_subtipo_str(uint32_t subtipo) {
	if (subtipo < __palavras_chave_quantidade) {
		return __palavras_chave[subtipo];
	}

	// TODO: warning
	return "Erro: subtipo de palavra-chave inválido";
}
