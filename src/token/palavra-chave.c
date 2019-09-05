/**
 * PCC - Paulo C Compiler
 *
 * token/palavra-chave.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include "token/palavra-chave.h"

int token_palavra_chave_init() {
	return 0;
}

int token_palavra_chave_buscar(const token_t *token) {
	static const char palavras_chave[][16] = {
		"auto",
		"break",
		"case",
		"char",
		"const",
		"continue",
		"default",
		"do",
		"double",
		"else",
		"enum",
		"extern",
		"float",
		"for",
		"goto",
		"if",
		"int",
		"long",
		"register",
		"return",
		"short",
		"signed",
		"sizeof",
		"static",
		"struct",
		"switch",
		"typedef",
		"union",
		"unsigned",
		"void",
		"volatile",
		"while"
	};
	static size_t palavras_chave_quantidade = sizeof palavras_chave / sizeof palavras_chave[0];

	int inicio = 0;
	int fim = palavras_chave_quantidade;
	int index;

	while (1) {
		int novo_index = (int) ((inicio + fim) / 2);
		if (novo_index == index) {
			return -1;
		}

		index = novo_index;

		int i;
		for (i = 0; token->contexto.lexema[i] != '\0' && palavras_chave[index][i] != '\0'; i++) {
			if (token->contexto.lexema[i] < palavras_chave[index][i]) {
				fim = index;
				break;
			} else if (token->contexto.lexema[i] > palavras_chave[index][i]) {
				inicio = index;
				break;
			}
		}

		if (token->contexto.lexema[i] == '\0' && palavras_chave[index][i] == '\0') {
			return index;
		}
	}
}
