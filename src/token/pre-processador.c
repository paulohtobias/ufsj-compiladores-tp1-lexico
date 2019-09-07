/**
 * PCC - Paulo C Compiler
 *
 * token/pre-processador.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <ctype.h>
#include <string.h>
#include "token.h"
#include "utils.h"
#include "plist.h"

// X-Macro pra cada tipo de pre-processador.
#define SUBTIPOS \
	SUBTIPO(TK_PP_IF, if, "if") \
	SUBTIPO(TK_PP_IFDEF, ifdef, "ifdef") \
	SUBTIPO(TK_PP_IFNDEF, ifndef, "ifndef") \
	SUBTIPO(TK_PP_ENDIF, endif, "endif") \
	SUBTIPO(TK_PP_DEFINE, define, "define") \
	SUBTIPO(TK_PP_INCLUDE, include, "include")

/// Tipos de pre-processador.
#define SUBTIPO(cod, nome, str) cod,
enum {
	SUBTIPOS
};
#undef SUBTIPO

/// Tipos de pre-processador em string.
#define SUBTIPO(cod, nome, str) str,
const char __preprocessadores[][32] = {
	SUBTIPOS
};
#undef SUBTIPO
size_t __preprocessadores_quantidade = ARR_TAMANHO(__preprocessadores);


/// Função adicionar
static void preprocessador_adicionar(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);

/// Função de erro.
static void preprocessador_incompleto(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);

/// Função subtipo_str.
const char *preprocessador_str(uint32_t subtipo);

/// Funções init.
int token_preprocessador_init(afd_t *afd) {
	int res = 0;

	afd_t afd_pp;
	afd_init(&afd_pp, 5);

	// Estado 0
	afd_transicao_t transicoes_0[] = {
		{1, {"#", "#", NULL}},
	};
	afd_pp.estados[0] = afd_criar_estado(transicoes_0, ARR_TAMANHO(transicoes_0), false, NULL);

	// Estado 1
	afd_transicao_t transicoes_1[] = {
		{1, {"[ \t]", "[ \t]", NULL}},
		{2, {"[a-z]", "[a-z]", NULL}},
	};
	plist_append(
		afd_pp.estados,
		afd_criar_estado(transicoes_1, ARR_TAMANHO(transicoes_1), false, NULL)
	);

	// Estado 2
	afd_transicao_t transicoes_2[] = {
		{2, {"[a-z]", "[a-z]", NULL}},
		{3, {"[^a-z]", "[^a-z]", NULL}},
	};
	plist_append(
		afd_pp.estados,
		afd_criar_estado(transicoes_2, ARR_TAMANHO(transicoes_2), false, NULL)
	);

	// Estado 3
	afd_transicao_t transicoes_3[] = {
		{3, {"[^\\\\\\n]", "[^\\\\\\n]", NULL}},
		{4, {"\\\\", "\\\\", NULL}},
		{5, {"\\n", "\\n", NULL}},
	};
	plist_append(
		afd_pp.estados,
		afd_criar_estado(transicoes_3, ARR_TAMANHO(transicoes_3), false, NULL)
	);

	// Estado 4
	afd_transicao_t transicoes_4[] = {
		{3, {"[\\S\\n]", "[\\S\\n]", NULL}},
		{4, {"\\s", "\\s", NULL}},
	};
	plist_append(
		afd_pp.estados,
		afd_criar_estado(transicoes_4, ARR_TAMANHO(transicoes_4), false, NULL)
	);

	// Estado Final
	plist_append(afd_pp.estados, afd_criar_estado(NULL, 0, true, preprocessador_adicionar));

	if ((res = afd_mesclar_automatos(afd, &afd_pp)) != AFD_OK) {
		fprintf(stderr, "%s: %s.\n Não foi possível iniciar.\n", __FUNCTION__, afd_get_erro(res));
		goto fim;
	}

	// Inicializando a tabela de símbolos de preprocessador.
	plist_create(tabela_simbolos[TK_PP], __preprocessadores_quantidade);
	memset(tabela_simbolos[TK_PP], 0, __preprocessadores_quantidade * sizeof *tabela_simbolos[TK_PP]);

fim:
	return res;
}

static void preprocessador_adicionar(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	// Pulando a # e possíveis espaços antes do nome da diretiva.
	int32_t lexema_offset = 1;
	while (isspace((unsigned char) lexema[lexema_offset])) {
		lexema_offset++;
	}

	// subtipo é o índice do lexema na tabela.
	int subtipo;
	for (subtipo = 0; subtipo < __preprocessadores_quantidade; subtipo++) {
		size_t i;
		for (i = 0; __preprocessadores[subtipo][i] != '\0' && lexema[i + lexema_offset] != '0'; i++) {
			if (__preprocessadores[subtipo][i] != lexema[i + lexema_offset]) {
				break;
			}
		}

		// Houve casamento.
		if (__preprocessadores[subtipo][i] == '\0') {
			break;
		}
	}

	if (subtipo < __preprocessadores_quantidade) {
		token_t token = token_criar(TK_PP, subtipo, lexema, comprimento, linha, coluna);
		token.subtipo_to_str = preprocessador_str;

		// Copiando a # inicial.
		token.valor.tamanho = 1;
		PMALLOC(token.valor.dados, token.valor.tamanho);
		((char *) token.valor.dados)[0] = '#';

		// Convertendo o lexema para uma única linha.
		int32_t posicao_inicial = lexema_offset;
		int32_t posicao_final = lexema_offset;
		int32_t posicao_barra = -1;

		for (int32_t i = 0; i < token.contexto.comprimento; i++) {
			if (token.contexto.lexema[i] == '\\') {
				posicao_barra = i;
			} else if (token.contexto.lexema[i] == '\n') {
				if (posicao_barra != -1) {
					if (posicao_barra + 1 < i) {
						// TODO: warning
						fprintf(stderr, "warning: backslash and newline separated by space linha %d coluna %d\n", linha, coluna);
					}

					posicao_final = posicao_barra;
				} else {
					posicao_final = i;
				}

				int32_t offset = token.valor.tamanho;
				token.valor.tamanho += posicao_final - posicao_inicial;
				PREALLOC(token.valor.dados, token.valor.tamanho + 1);
				strncpy(((char *) token.valor.dados) + offset, token.contexto.lexema + posicao_inicial, posicao_final - posicao_inicial);
				posicao_inicial = i + 1;
			} else if (!isspace((unsigned char) token.contexto.lexema[i]) && posicao_barra != -1) {
				posicao_barra = -1;
			}
		}
		((char *) token.valor.dados)[token.valor.tamanho] = '\0';
		token.valor.to_str = strdup;

		token_adicionar(&token);
	} else {
		// TODO: erro
		fprintf(stderr, "invalid preprocessing directive linha %d coluna %d\n", linha, coluna);
	}
}

static void preprocessador_incompleto(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	// TODO: warning
	fprintf(stderr, "diretiva pre-processador incompleta na linha %d coluna %d\n", linha, coluna);
}

const char *preprocessador_str(uint32_t subtipo) {
	if (subtipo < __preprocessadores_quantidade) {
		return __preprocessadores[subtipo];
	}

	// TODO: warning
	return "Erro: subtipo de preprocessador inválido";
}