#include <stdio.h>
#include "lexico.h"
#include "utils.h"
#include "plist.h"

#define ABRIR_SAIDA(out, nome, modo) \
	do { \
		out = fopen(nome, modo); \
		if (out == NULL) { \
			LOG_PCC_ERRO(EXIT_FAILURE, "", "Erro ao abrir o arquivo de saída %s: ", nome); \
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

	// Exibindo a lista de tokens.
	FILE *out = NULL;
	ABRIR_SAIDA(out, "saida/Token.txt", "wb");
	for (size_t i = 0; i < plist_len(lista_tokens); i++) {
		token_print(out, lista_tokens + i);
	}
	fclose(out);

	// Exibindo as tabelas de símbolos.
	char arquivo_saida[300];
	for (int i = 0; i < TK_COUNT; i++) {
		for (int j = 0; j < plist_cap(tabela_simbolos[i]); j++) {
			for (int k = 0; k < plist_len(tabela_simbolos[i][j]); k++) {
				const token_t *token = &lista_tokens[tabela_simbolos[i][j][k]];

				const char *modo = k == 0 ? "wb" : "ab";

				sprintf(arquivo_saida, "saida/%s-%s.txt", token_tipo_str(token), token_subtipo_str(token));
				ABRIR_SAIDA(out, arquivo_saida, modo);

				token_print(out, token);

				fclose(out);
			}
		}
	}

	// Exibe "Fim" na tela.
	putchar('\x46');
	putchar('\151');
	putchar('m');
	putchar('\n');

	printf("Warnings: %u\n", _log_warnings);
	printf("Erros: %u\n", _log_erros);

	// TODO: lexico_finalizar();
	return 0;
}
