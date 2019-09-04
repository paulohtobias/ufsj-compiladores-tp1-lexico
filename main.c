#include <stdio.h>
#include "lexico.h"
#include "utils.h"
#include "plist.h"

int main(int argc, char const *argv[]) {
	const char *filename = "main.c";
	if (argc >= 2) {
		filename = argv[1];
	}

	size_t comprimento;
	char *c2c = file_to_str(filename, &comprimento);

	lexico_parse(c2c);
	free(c2c);

	for (size_t i = 0; i < plist_len(lista_tokens); i++) {
		token_print(lista_tokens + i);
	}

	puts("TABLEA DE SÍMBOLOS\n");
	for (int i = 0; i < TK_COUNT; i++) {
		for (size_t j = 0; j < plist_len(tabela_simbolos[i]); j++) {
			token_print(&lista_tokens[tabela_simbolos[i][j]]);
		}
		puts("==========\n");
	}

	return 0;
}
