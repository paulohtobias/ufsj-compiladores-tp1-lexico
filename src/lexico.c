/**
 * PCC - Paulo C Compiler
 *
 * lexico.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include "utils.h"
#include "lexico.h"

afd_t lexico_afd;

int lexico_init() {
	// Inicializando o automato.
	lexico_afd.estados_quantidade = 4;
	PCALLOC(lexico_afd.estados, lexico_afd.estados_quantidade);
	lexico_afd.estado_inicial = lexico_afd.estados;


	// Definindo cada estado.
	afd_estado_t *estado = lexico_afd.estados;
	estado->transicoes_quantidade = 1;
	PCALLOC(estado->transicoes, estado->transicoes_quantidade);
	estado->transicoes[0].match = afd_transicao_match_interval;
	estado->transicoes[0].dados = "\"";
	estado->transicoes[0].estado = 1;

	estado = lexico_afd.estados + 1;
	estado->transicoes_quantidade = 3;
	PCALLOC(estado->transicoes, estado->transicoes_quantidade);
	estado->transicoes[0].match = afd_transicao_match_interval_not;
	estado->transicoes[0].dados = "\"\\";
	estado->transicoes[0].estado = 1;
	estado->transicoes[1].match = afd_transicao_match_interval;
	estado->transicoes[1].dados = "\\";
	estado->transicoes[1].estado = 2;
	estado->transicoes[2].match = afd_transicao_match_interval;
	estado->transicoes[2].dados = "\"";
	estado->transicoes[2].estado = 3;

	estado = lexico_afd.estados + 2;
	estado->transicoes_quantidade = 1;
	PCALLOC(estado->transicoes, estado->transicoes_quantidade);
	estado->transicoes[0].match = afd_transicao_match_interval;
	estado->transicoes[0].dados = NULL;
	estado->transicoes[0].estado = 1;

	estado = lexico_afd.estados + 3;
	estado->transicoes_quantidade = 0;
	estado->final = true;
	estado->parse = token_string_parse;

	return 0;
}
