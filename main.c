#include <stdio.h>
#include <string.h>
#include "lexico.h"
#include "utils.h"
#include "plist.h"
#include "opcoes.h"

#define ABRIR_SAIDA(out, nome, modo) \
	if (out != stdout) { \
	do { \
		out = fopen(nome, modo); \
		if (out == NULL) { \
			LOG_PCC_ERRO(EXIT_FAILURE, "", "Erro ao abrir o arquivo de saída %s: ", nome); \
		} \
		} while(0); \
	}

#define FECHAR_SAIDA(out) \
	if (out != stdout) { \
		fclose(out); \
	}

int main(int argc, char * const argv[]) {
	// Opções.
	extern bool salvar_comentarios;
	FILE *out = NULL;
	char *pasta_saida = NULL;
	char arquivo_saida[300];

	opcao_t opcoes[] = {
		OPCAO_INIT('C', tipo_bool, &salvar_comentarios, "0", "Controla se os comentários serão salvos na lista de tokens ou se serão descartados"),
		OPCAO_INIT('O', tipo_str(0), &pasta_saida, "FILE=stdout", "Diretório de saída para os arquivos"),
	};

	int argi;
	if ((argi = parse_args(argc, argv, opcoes, ARR_TAMANHO(opcoes))) >= argc) {
		LOG_PCC_ERRO(1, NULL, "Código fonte não informado");
	}

	const char *filename = argv[argi];
	if (strcmp(pasta_saida, "stdout") == 0) {
		out = stdout;
	}

	// Inicializando o módulo léxico.
	lexico_init();

	// Fazendo a análise do código.
	lexico_parse(filename);

	// Exibindo a lista de tokens.
	sprintf(arquivo_saida, "%s/Token.txt", pasta_saida);
	ABRIR_SAIDA(out, arquivo_saida, "wb");
	for (size_t i = 0; i < plist_len(lista_tokens); i++) {
		token_print(out, lista_tokens + i);
	}
	FECHAR_SAIDA(out);

	if (out != stdout) {
	// Exibindo as tabelas de símbolos.
	for (int i = 0; i < TK_COUNT; i++) {
		for (int j = 0; j < plist_cap(tabela_simbolos[i]); j++) {
			for (int k = 0; k < plist_len(tabela_simbolos[i][j]); k++) {
				const token_t *token = &lista_tokens[tabela_simbolos[i][j][k]];

				const char *modo = k == 0 ? "wb" : "ab";
				sprintf(arquivo_saida, "saida/%s-%s.txt", token_tipo_str(token), token_subtipo_str(token));
				ABRIR_SAIDA(out, arquivo_saida, modo);

				token_print(out, token);

					FECHAR_SAIDA(out);
				}
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
