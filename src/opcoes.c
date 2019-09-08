/**
 * Option Parser
 *
 * opcoes.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <ctype.h>

#include "opcoes.h"

int flag_hash(int flag) {
	if (flag >= 'A' && flag <= 'Z') {
		return flag - 'A';
	}
	if (flag >= 'a' && flag <= 'z') {
		return flag - 'a' + 'Z' - 'A' + 1;
	}

	return -1;
}

int conversao_bool(opcao_t *opcao, const void *valor){
	int *v = opcao->buffer;
	if (valor == NULL) {
		*v = !(*v);
	} else {
		*v = atoi(valor);
	}
	return 1;
}

int conversao_char(opcao_t *opcao, const void *valor){
	*((char *) opcao->buffer) = *((char *) valor);
	return 1;
}

int conversao_int(opcao_t *opcao, const void *valor){
	*((int *) opcao->buffer) = atoi(valor);
	return 1;
}

int conversao_double(opcao_t *opcao, const void *valor){
	*((int *) opcao->buffer) = atof(valor);
	return 1;
}

int conversao_str(opcao_t *opcao, const void *valor){
	// Verifica se é pra alocar dinamicamente.
	if (opcao->tipo.tamanho < 1) {
		opcao->tipo.tamanho = strlen((char *) valor);
		*((char **) opcao->buffer) = malloc(opcao->tipo.tamanho + 1);
	}
	strncpy(*((char **) opcao->buffer), valor, opcao->tipo.tamanho);
	return 1;
}

int opcao_definir_valor(opcao_t *opcao, const void *valor) {
	opcao->tipo.conversao(opcao, valor);

	return 1;
}

int print_descricao(const char *descricao, int tamanho, int j) {
	int i = 0, quebrar_linha = 0;
	int c;
	while (i < tamanho) {
		j++;

		c = descricao[i++];
		if (c == '\n' || (quebrar_linha && isspace(c))) {
			printf("\n\t\t\t");
			quebrar_linha = 0;
			j = 1;
		}
		if (c != '\n') {
			putchar(c);
		}
		if (j % LARGURA_MAX == 0) {
			quebrar_linha = 1;
		}
	}

	return j;
}

void print_ajuda(const char *arg0, const opcao_t *opcoes, int qtd_opcoes) {
	int i, j;
	const opcao_t *opcao;
	char nome_valor[32];
	const char *valor_padrao;
	char descricao[DESCRICAO_MAX];
	int descricao_tamanho = 0;
	printf("Uso: %s\n", arg0);

	for (i = 0; i < qtd_opcoes; i++) {
		opcao = &opcoes[i];

		j = 0;
		valor_padrao = opcao->nome_valor;
		if (opcao->nome_valor != NULL && opcao->tipo.cod != FT_BOOL) {
			for (j = 0; opcao->nome_valor[j] != '\0' && opcao->nome_valor[j] != '='; nome_valor[j] = opcao->nome_valor[j], j++);
			if (opcao->nome_valor[j] != '\0') {
				valor_padrao = &opcao->nome_valor[j] + 1;
			} else {
				valor_padrao = NULL;
			}
		}
		nome_valor[j] = '\0';

		sprintf(descricao, opcao->descricao, nome_valor);
		descricao_tamanho = strlen(descricao);
		printf("  -%c\t%s\t\t", opcao->flag, nome_valor);

		//Descrição
		j = print_descricao(descricao, descricao_tamanho, 0);
		putchar('.');

		if (opcao->tipo.cod != FT_BOOL && valor_padrao != NULL) {
			const char *aspas = "";
			if (opcao->tipo.cod == FT_STR) {
				aspas = "'";
			}
			char padrao[12 + strlen(valor_padrao)];
			sprintf(padrao, " Padrão: %s%s%s", aspas, valor_padrao, aspas);
			print_descricao(padrao, strlen(padrao), j);
		}
		printf("\n");
	}
	exit(0);
}

int parse_args(int argc, char * const argv[], opcao_t *opcoes, int qtd_opcoes) {
	int i, j = 0, optstring_tamanho = qtd_opcoes * 2 + 1;
	char *optstring = calloc(optstring_tamanho, 1);
	int opcoes_obrigatorias = 0;
	opcao_t *opcao;

	//Para acessar as opções em O(1).
	opcao_t *opcao_hash[52] = {0};

	//Criando a string que será usada pelo getopt.
	//E inicializa opcao_hash.
	optstring[j++] = 'h';
	for (i = 0; i < qtd_opcoes; i++) {
		opcao = &opcoes[i];
		optstring[j++] = opcao->flag;
		if (opcao->tipo.cod != FT_BOOL) {
			optstring[j++] = ':';
		}

		//Definindo o valor padrão.
		const char *valor_padrao = opcao->nome_valor;
		if (opcao->tipo.cod != FT_BOOL) {
			valor_padrao = strstr(opcao->nome_valor, "=");
			if (valor_padrao != NULL) {
				valor_padrao++;
			}
		}

		opcao->__obrigatoria = -1;
		if (valor_padrao != NULL) {
			opcao_definir_valor(opcao, valor_padrao);
		} else if (opcao->tipo.cod != FT_BOOL) {
			opcao->__obrigatoria = i;
			opcoes_obrigatorias++;
		}

		//Copia a opção para o vetor de hash.
		opcao_hash[flag_hash(opcao->flag)] = opcao;
	}

	int flag, hash;
	opterr = 0;
	while ((flag = getopt(argc, argv, optstring)) != -1) {
		hash = flag_hash(flag);
		if (hash != -1) {
			opcao = opcao_hash[hash];

			if (flag == 'h') {
				print_ajuda(argv[0], opcoes, qtd_opcoes);
			} else {
				opcao_definir_valor(opcao, optarg);
				if (opcao->__obrigatoria != -1) {
					opcao->__obrigatoria = -1;
					opcoes_obrigatorias--;
				}
			}
		} else {
			//Flag inválida.
			if (flag == '?'){
				hash = flag_hash(optopt);
				printf("?: %d\n", hash);
				if (hash != -1 && opcao_hash[hash] != NULL) {
					fprintf(stderr, "Opção -%c precisa de argumento.\n", optopt);
				} else {
					printf("EI\n");
					if (isprint(optopt)) {
						fprintf(stderr, "Opção -%c desconhecida.\n", optopt);
					} else {
						fprintf(stderr, "Caractere '\\x%x' de opção desconhecido.\n", optopt);
					}
				}
				exit(1);
			}
		}
	}

	//Verifica se opções obrigatórias não foram definidas.
	if (opcoes_obrigatorias > 0) {
		for (i = 0; i < qtd_opcoes; i++) {
			if (opcoes[i].__obrigatoria != -1) {
				fprintf(stderr, "Opção -%c necessária\n", opcoes[i].flag);
			}
		}
		exit(1);
	}

	return optind;
}
