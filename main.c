#include "afd.h"
#include "utils.h"
#include <stdio.h>

extern void print_afd(const afd_t *afd);

int main(int argc, char const *argv[]) {
	afd_t afd_lexico;
	afd_init(&afd_lexico, 1, 0);

	int res;

	{
		afd_transicao_pattern_t transicoes[] = {
			{">", ">", NULL},
			{">>", ">", NULL},
			{">>=", "=", NULL}
		};

		afd_estado_t estado_neg;
		estado_neg.transicoes_quantidade = 0;
		estado_neg.transicoes = NULL;
		estado_neg.gerar_dado = NULL;

		res = afd_add_estado(&afd_lexico, transicoes, sizeof transicoes / sizeof transicoes[0], &estado_neg);
		if (res != AFD_OK) {
			printf("Erro ao adicionar estado: %s\n", afd_get_erro(res));
		}
	}

	print_afd(&afd_lexico);
	getchar();

	{
		afd_transicao_pattern_t transicoes[] = {
			{">", ">", NULL},
			{">=", ">", NULL},
		};

		afd_estado_t estado_neg;
		estado_neg.transicoes_quantidade = 0;
		estado_neg.transicoes = NULL;
		estado_neg.gerar_dado = NULL;

		res = afd_add_estado(&afd_lexico, transicoes, sizeof transicoes / sizeof transicoes[0], &estado_neg);
		if (res != AFD_OK) {
			printf("Erro ao adicionar estado: %s\n", afd_get_erro(res));
		}
	}

	print_afd(&afd_lexico);

	return 0;
}
