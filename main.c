   #include <stdio.h>
#include "lexico.h"
#include "utils.h"
#include "plist.h"

int main(int argc, char const *argv[]) {
	// Nome do arquivo.
	const char *filename = "main.c";
	if (argc >= 2) {
		filename = argv[1];
	}

	lexico_init();

	// Análise.
	lexico_parse(filename);

	// Exibindo a lista de tokens.
	for (size_t i = 0; i < plist_len(lista_tokens); i++) {
		token_print(lista_tokens + i);
	}

	// Exibindo as tabelas de símbolos.
	puts("TABLEA DE SÍMBOLOS\n");
	for (int i = 0; i < TK_COUNT; i++) {
		for (size_t j = 0; j < plist_len(tabela_simbolos[i]); j++) {
			token_print(&lista_tokens[tabela_simbolos[i][j]]);
		}
		puts("==========\n");
	}

	return 0;
}
