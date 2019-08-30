#include "afd.h"
#include "utils.h"
#include <stdio.h>

extern void print_afd(const afd_t *afd);

int main(int argc, char const *argv[]) {
	afd_transicao_pattern_t transicoes[] = {
		{"maior", ">", NULL},
		{"shift-right", ">", NULL},
		{"self-shift-right", "=", NULL}
	};

	afd_t afd_lexico;
	afd_init(&afd_lexico, 1, 0);

	afd_estado_t estado_neg;
	estado_neg.transicoes_quantidade = 0;
	estado_neg.transicoes = NULL;
	estado_neg.gerar_dado = NULL;

	afd_add_estado(&afd_lexico, transicoes, sizeof transicoes / sizeof transicoes[0], &estado_neg);

	print_afd(&afd_lexico);

	return 0;
}
