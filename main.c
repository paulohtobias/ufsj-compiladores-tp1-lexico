#include <stdio.h>
#include "lexico.h"
#include "utils.h"
#include "plist.h"

#define ABRIR_SAIDA(out, nome) \
	do { \
		out = fopen(nome, "wb"); \
		if (out == NULL) { \
			fprintf(stderr, "Erro ao abrir o arquivo de saída %s. ", nome); \
			perror(""); \
			retval = EXIT_FAILURE; \
			goto fim; \
		} \
	} while(0);

int main(int argc, char const *argv[]) {
	// Nome do arquivo.
	const char *filename = "main.c";
	if (argc >= 2) {
		filename = argv[1];
	}


	// Inicializando o módulo léxico.
	lexico_init();

	// Fazendo a análise do código.
	lexico_parse(filename);

	int retval = EXIT_SUCCESS;
	FILE *out = NULL;

	// Exibindo a lista de tokens.
	ABRIR_SAIDA(out, "saida/Token.txt");
	for (size_t i = 0; i < plist_len(lista_tokens); i++) {
		token_print(out, lista_tokens + i);
	}
	fclose(out);

	// Exibindo as tabelas de símbolos.
	for (int i = 0; i < TK_COUNT; i++) {
		char arquivo_saida[300];
		sprintf(arquivo_saida, "saida/%s.txt", token_tipo_str(i));
		ABRIR_SAIDA(out, arquivo_saida);
		for (size_t j = 0; j < plist_len(tabela_simbolos[i]); j++) {
			token_print(out, &lista_tokens[tabela_simbolos[i][j]]);
		}
		fclose(out);
	}

fim:
	// TODO: lexico_finalizar();

	return retval;
}
