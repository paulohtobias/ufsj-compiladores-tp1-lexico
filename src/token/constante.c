/**
 * PCC - Paulo C Compiler
 *
 * token/constante.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <ctype.h>
#include "token.h"
#include "utils.h"
#include "plist.h"

// X-Macro pra cada tipo de constante.
#define CONSTANTE_SUBTIPOS \
	CONSTANTE_SUBTIPO(TK_CNST_STR, str, "string-literal") \
	CONSTANTE_SUBTIPO(TK_CNST_CHAR, char, "char") \

	/*
	CONSTANTE_SUBTIPO(TK_CNST_OCT, octal, "octal") \
	CONSTANTE_SUBTIPO(TK_CNST_DEC, decimal, "decimal") \
	CONSTANTE_SUBTIPO(TK_CNST_HEX, hexadecimal, "hexadecimal") \
	CONSTANTE_SUBTIPO(TK_CNST_FLT, float, "float") \
	CONSTANTE_SUBTIPO(TK_CNST_DBL, double, "double")
	*/

/// Tipos de constante.
#define CONSTANTE_SUBTIPO(cod, nome, str) cod,
enum {
	CONSTANTE_SUBTIPOS
};
#undef CONSTANTE_SUBTIPO

/// Tipos de palavra-chave em string.
#define CONSTANTE_SUBTIPO(cod, nome, str) str,
const char __constantes[][16] = {
	CONSTANTE_SUBTIPOS
};
#undef CONSTANTE_SUBTIPO
size_t __constantes_quantidade = ARR_TAMANHO(__constantes);

/// Funções init.
#define CONSTANTE_SUBTIPO(cod, nome, str) int token_constante_ ## nome ## _init(afd_t *afd);
CONSTANTE_SUBTIPOS
#undef CONSTANTE_SUBTIPO

/// Funções adicionar.
#define CONSTANTE_SUBTIPO(cod, nome, str) void token_constante_ ## nome ## _adicionar(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);
CONSTANTE_SUBTIPOS
#undef CONSTANTE_SUBTIPO

/// Funções de erro.
void token_constante_str_incompleta(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);
void token_constante_char_incompleto(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);

/// Outras funções.
const char *token_constante_subtipo_str(uint32_t subtipo);
static char parse_escape_sequence(const char **src);


/// init
int token_constante_init(afd_t *afd) {
	int res;
	#define CONSTANTE_SUBTIPO(cod, nome, str) \
		if ((res = token_constante_ ## nome ## _init(afd)) != 0) { \
			return 1; \
		}

	CONSTANTE_SUBTIPOS
	#undef CONSTANTE_SUBTIPO

	return 0;
}

int token_constante_str_init(afd_t *afd) {
	int res;

	afd_t afd_str;
	afd_init(&afd_str, 3);

	// Transição de ligação. Deve começar com aspas duplas.
	afd_transicao_pattern_t transicao_inicial = {
		"\"",
		"\"",
		NULL
	};

	// Estado 0.
	afd_transicao_t transicoes_0[] = {
		{0, {"[^\"\\\\\\n]", "[^\"\\\\\\n]", NULL}},
		{1, {"\\", "\\\\", NULL}},
		{2, {"\"", "\"", NULL}}
	};
	afd_str.estados[0] = afd_criar_estado(transicoes_0, ARR_TAMANHO(transicoes_0), false, token_constante_str_incompleta);

	// Estado 1.
	afd_transicao_t transicoes_1[] = {
		{0, {".", "."}}
	};
	plist_append(
		afd_str.estados,
		afd_criar_estado(transicoes_1, ARR_TAMANHO(transicoes_1), false, token_constante_str_incompleta)
	)

	// Estado Final.
	plist_append(
		afd_str.estados,
		afd_criar_estado(NULL, 0, true, token_constante_str_adicionar)
	);

	if ((res = afd_add_subautomato(afd, &transicao_inicial, 1, &afd_str)) != AFD_OK) {
		fprintf(stderr, "%s: %s.\n Não foi possível iniciar.\n", __FUNCTION__, afd_get_erro(res));
	}

	return res;
}

int token_constante_char_init(afd_t *afd) {
	int res;

	afd_t afd_char;
	afd_init(&afd_char, 4);

	// Transição de ligação. Deve começar com aspas simples.
	afd_transicao_pattern_t transicao_inicial = {
		"'",
		"'",
		NULL
	};

	// Estado 0.
	afd_transicao_t transicoes_0[] = {
		{1, {"[^'\\\\\\n]", "[^\'\\\\\n]", NULL}},
		{2, {"\\", "\\\\", NULL}}
	};
	afd_char.estados[0] = afd_criar_estado(transicoes_0, ARR_TAMANHO(transicoes_0), false, token_constante_char_incompleto);

	// Estado 1.
	afd_transicao_t transicoes_1[] = {
		{3, {"'", "'"}}
	};
	plist_append(
		afd_char.estados,
		afd_criar_estado(transicoes_1, ARR_TAMANHO(transicoes_1), false, token_constante_char_incompleto)
	)

	// Estado 2.
	afd_transicao_t transicoes_2[] = {
		{2, {"[^'\\n]", "[^'\n]"}},
		{3, {"'", "'"}}
	};
	plist_append(
		afd_char.estados,
		afd_criar_estado(transicoes_2, ARR_TAMANHO(transicoes_2), false, token_constante_char_incompleto)
	)

	// Estado Final.
	plist_append(
		afd_char.estados,
		afd_criar_estado(NULL, 0, true, token_constante_char_adicionar)
	);

	if ((res = afd_add_subautomato(afd, &transicao_inicial, 1, &afd_char)) != AFD_OK) {
		fprintf(stderr, "%s: %s.\n Não foi possível iniciar.\n", __FUNCTION__, afd_get_erro(res));
	}

	return res;
}

/// adicionar
void token_constante_str_adicionar(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	token_t token = token_criar(TK_CNST, TK_CNST_STR, lexema, comprimento, linha, coluna);
	token.subtipo_to_str = token_constante_subtipo_str;

	token.valor.tamanho = comprimento -1;
	PMALLOC(token.valor.dados, token.valor.tamanho);

	// Ignorando as aspas iniciais.
	lexema++;
	size_t i;
	for (i = 0; *lexema != '\"'; i++) {
		if (*lexema == '\\') {
			((char *) token.valor.dados)[i] = parse_escape_sequence(&lexema);
		} else {
			((char *) token.valor.dados)[i] = *lexema;
		}

		lexema += utf8_simbolo_comprimento(lexema);
	}
	((char *) token.valor.dados)[i++] = '\0';

	if (i < token.valor.tamanho) {
		token.valor.tamanho = i;
		PMALLOC(token.valor.dados, token.valor.tamanho);
	}

	token_adicionar(&token);
}

void token_constante_char_adicionar(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	token_t token = token_criar(TK_CNST, TK_CNST_CHAR, lexema, comprimento, linha, coluna);
	token.subtipo_to_str = token_constante_subtipo_str;

	token.valor.tamanho = 1; // sizeof(char) é sempre 1.
	PMALLOC(token.valor.dados, 1);

	// Ignorando as aspas iniciais.
	lexema++;
	if (*lexema == '\\') {
		*((char *) token.valor.dados) = parse_escape_sequence(&lexema);
	} else {
		*((char *) token.valor.dados) = *lexema;
	}
	lexema++;

	if (*lexema != '\'') {
		// TODO: warning: 'multi-character constant'.
	}

	token_adicionar(&token);
}


static void incompleto(char simbolo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	fprintf(stderr, "Faltando '%c' na linha %d coluna %d\n", simbolo, linha, coluna);
}
void token_constante_str_incompleta(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	incompleto('"', lexema, comprimento, linha, coluna);
}
void token_constante_char_incompleto(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	incompleto('\'', lexema, comprimento, linha, coluna);
}


const char *token_constante_subtipo_str(uint32_t subtipo) {
	if (subtipo < __constantes_quantidade) {
		return __constantes[subtipo];
	}

	// TODO: warning
	return "Erro: subtipo de constante inválido";
}

static char parse_escape_sequence(const char **src) {
	static char escape_table[][2] = {
		{'n', '\n'},
		{'t', '\t'},
		{'v', '\v'},
		{'b', '\b'},
		{'r', '\r'},
		{'f', '\f'},
		{'a', '\a'},
		{'\\', '\\'},
		{'?', '\?'},
		{'\'', '\''},
		{'\"', '\"'}
	};
	static const size_t escape_table_tamanho = ARR_TAMANHO(escape_table);

	(*src)++;
	// Procura na tabela pelos símbolos.
	for (int i = 0; i < escape_table_tamanho; i++) {
		if (escape_table[i][0] == **src) {
			return escape_table[i][1];
		}
	}

	// Identifica numerais.
	int base = -1;
	if (isdigit((unsigned char) **src)) {
		base = 8;
	} else if (**src == 'x') {
		(*src)++;
		base = 16;
	}

	if (base == -1) {
		// TODO: warning ou erro.
		return **src;
	}

	char *end = NULL;
	char c = strtol(*src, &end, base);

	// TODO: checar por overflow e dar warnings.

	*src = end - 1;

	return c;
}
