/**
 * PCC - Paulo C Compiler
 *
 * token/constante.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <errno.h>
#include "token.h"
#include "utils.h"
#include "plist.h"

// X-Macro pra cada tipo de constante.
#define SUBTIPOS \
	SUBTIPO(TK_CNST_STR, str, "string-literal") \
	SUBTIPO(TK_CNST_CHAR, char, "char") \
	SUBTIPO(TK_CNST_INT, int, "inteiro") \
	SUBTIPO(TK_CNST_UINT, uint, "inteiro-unsigned") \
	SUBTIPO(TK_CNST_LINT, lint, "inteiro-long") \
	SUBTIPO(TK_CNST_ULINT, ulint, "inteiro-unsigned_long") \

	/*
	SUBTIPO(TK_CNST_FLT, float, "float")
	*/

/// Tipos de constante.
#define SUBTIPO(cod, nome, str) cod,
enum {
	SUBTIPOS
};
#undef SUBTIPO

/// Tipos de palavra-chave em string.
#define SUBTIPO(cod, nome, str) str,
const char __constantes[][32] = {
	SUBTIPOS
};
#undef SUBTIPO
size_t __constantes_quantidade = ARR_TAMANHO(__constantes);

/// Funções init.
#define SUBTIPO(cod, nome, str) static int nome ## _init(afd_t *afd);
SUBTIPOS
#undef SUBTIPO

/// Funções adicionar.
static void str_adicionar(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);
static void char_adicionar(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);
static void int_adicionar(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);

/// Funções to_str
static char *str_to_str(const void *dados, size_t comprimento);
static char *char_to_str(const void *dados, size_t comprimento);
static char *int_to_str(const void *dados, size_t comprimento);

/// Funções de erro.
static void str_incompleta(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);
static void char_incompleto(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);

/// Outras funções.
static const char *subtipo_str(uint32_t subtipo);
static char parse_escape_sequence(const char **src);
static char *int_to_str(const void *dados, size_t comprimento);


/// init
int token_constante_init(afd_t *afd) {
	int res;

	#define SUBTIPO(cod, nome, str) \
		if ((res = nome ## _init(afd)) != 0) { \
			return 1; \
		}

	SUBTIPOS
	#undef SUBTIPO

	// Inicializando a tabela de símbolos de identificador.
	plist_create(tabela_simbolos[TK_CNST], __constantes_quantidade);
	memset(tabela_simbolos[TK_CNST], 0, __constantes_quantidade * sizeof *tabela_simbolos[TK_CNST]);

	return 0;
}
static int str_init(afd_t *afd) {
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
	afd_str.estados[0] = afd_criar_estado(transicoes_0, ARR_TAMANHO(transicoes_0), false, str_incompleta);

	// Estado 1.
	afd_transicao_t transicoes_1[] = {
		{0, {".", "."}}
	};
	plist_append(
		afd_str.estados,
		afd_criar_estado(transicoes_1, ARR_TAMANHO(transicoes_1), false, str_incompleta)
	)

	// Estado Final.
	plist_append(
		afd_str.estados,
		afd_criar_estado(NULL, 0, true, str_adicionar)
	);

	if ((res = afd_add_subautomato(afd, &transicao_inicial, 1, &afd_str)) != AFD_OK) {
		fprintf(stderr, "%s: %s.\n Não foi possível iniciar.\n", __FUNCTION__, afd_get_erro(res));
	}

	return res;
}
static int char_init(afd_t *afd) {
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
	afd_char.estados[0] = afd_criar_estado(transicoes_0, ARR_TAMANHO(transicoes_0), false, char_incompleto);

	// Estado 1.
	afd_transicao_t transicoes_1[] = {
		{3, {"'", "'"}}
	};
	plist_append(
		afd_char.estados,
		afd_criar_estado(transicoes_1, ARR_TAMANHO(transicoes_1), false, char_incompleto)
	)

	// Estado 2.
	afd_transicao_t transicoes_2[] = {
		{2, {"[^'\\n]", "[^'\n]"}},
		{3, {"'", "'"}}
	};
	plist_append(
		afd_char.estados,
		afd_criar_estado(transicoes_2, ARR_TAMANHO(transicoes_2), false, char_incompleto)
	)

	// Estado Final.
	plist_append(
		afd_char.estados,
		afd_criar_estado(NULL, 0, true, char_adicionar)
	);

	if ((res = afd_add_subautomato(afd, &transicao_inicial, 1, &afd_char)) != AFD_OK) {
		fprintf(stderr, "%s: %s.\n Não foi possível iniciar.\n", __FUNCTION__, afd_get_erro(res));
	}

	return res;
}
static int int_init(afd_t *afd) {
	int res;

	afd_t afd_int;
	afd_init(&afd_int, 2);

	// Transição de ligação. Deve começar com número.
	afd_transicao_pattern_t transicao_inicial = {"\\d", "\\d", NULL};

	// Estado 0.
	afd_transicao_t transicoes_0[] = {
		{0, {"\\d", "\\d", NULL}},
		{1, {"\\w", "[" regex_unicode_letter "]", NULL}}
	};
	afd_int.estados[0] = afd_criar_estado(transicoes_0, ARR_TAMANHO(transicoes_0), true, int_adicionar);

	// Estado Final.
	afd_transicao_t transicoes_1[] = {
		{1, {"[\\d\\w\\.]", "[\\d\\." regex_unicode_letter "]", NULL}}
	};
	plist_append(
		afd_int.estados,
		afd_criar_estado(transicoes_1, ARR_TAMANHO(transicoes_1), true, int_adicionar)
	)

	puts("KAMIKAZE");
	afd_print(&afd_int);

	if ((res = afd_add_subautomato(afd, &transicao_inicial, 1, &afd_int)) != AFD_OK) {
		fprintf(stderr, "%s: %s.\n Não foi possível iniciar.\n", __FUNCTION__, afd_get_erro(res));
	}

	return res;
}
static int uint_init(afd_t *afd) {
	return 0;
}
static int lint_init(afd_t *afd) {
	return 0;
}
static int ulint_init(afd_t *afd) {
	return 0;
}

/// adicionar
static void str_adicionar(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	token_t token = token_criar(TK_CNST, TK_CNST_STR, lexema, comprimento, linha, coluna);
	token.subtipo_to_str = subtipo_str;

	token.valor.tamanho = comprimento -1;
	PMALLOC(token.valor.dados, token.valor.tamanho);
	token.valor.to_str = str_to_str;

	// Ignorando as aspas iniciais.
	lexema++;
	size_t i;
	for (i = 0; *lexema != '\"'; i++) {
		if (*lexema == '\\') {
			((char *) token.valor.dados)[i] = parse_escape_sequence(&lexema);
		} else {
			((char *) token.valor.dados)[i] = *lexema;
		}

		lexema++;
	}
	((char *) token.valor.dados)[i++] = '\0';

	if (i < token.valor.tamanho) {
		token.valor.tamanho = i;
		PREALLOC(token.valor.dados, token.valor.tamanho);
	}

	token_adicionar(&token);
}
static void char_adicionar(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	token_t token = token_criar(TK_CNST, TK_CNST_CHAR, lexema, comprimento, linha, coluna);
	token.subtipo_to_str = subtipo_str;

	token.valor.tamanho = 1; // sizeof(char) é sempre 1.
	PMALLOC(token.valor.dados, 1);
	token.valor.to_str = char_to_str;

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
static void int_adicionar(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	token_t token = token_criar(TK_CNST, TK_CNST_INT, lexema, comprimento, linha, coluna);
	token.subtipo_to_str = subtipo_str;

	// Convertendo o lexema para inteiro.
	char *fim = NULL;
	uintmax_t valor = strtoumax(token.contexto.lexema, &fim, 0);
	size_t tamanho = sizeof valor;

	// Verificando se o sufixo é válido.
	bool u = false, l = false;
	bool sufixo_valido = false;
	while (*fim != '\0') {
		char sufixo = tolower((unsigned char) *fim);

		sufixo_valido = false;
		if (!u && sufixo == 'u') {
			fim++;
			u = sufixo_valido = true;

			// Alterando o subtipo.
			token.subtipo = l ? TK_CNST_ULINT : TK_CNST_UINT;
		}
		if (!l && sufixo == 'l') {
			fim++;
			l = sufixo_valido = true;

			// Alterando o subtipo.
			token.subtipo = u ? TK_CNST_ULINT : TK_CNST_LINT;
		}

		if (!sufixo_valido) {
			// TODO: warning: também melhorar mensagem.
			fprintf(stderr, "sufixo %s inválido (%d, %d)\n%s\n", fim, linha, coluna, token.contexto.lexema);
			break;
		}
	}

	// Verificando se houve overflow.
	if (valor == UINTMAX_MAX && errno == ERANGE) {
		// TODO: warning
		fprintf(stderr, "constante inteira é grande demais.\n");
	}

	token.valor.tamanho = tamanho;
	PMALLOC(token.valor.dados, token.valor.tamanho);
	memcpy(token.valor.dados, &valor, sizeof valor);
	token.valor.to_str = int_to_str;

	token_adicionar(&token);
}

/// to_str
static char *str_to_str(const void *dados, size_t comprimento) {
	return strdup(dados);
}
static char *char_to_str(const void *dados, size_t comprimento) {
	char *str = malloc(2);

	str[0] = *((char *) dados);
	str[1] = '\0';

	return str;
}
static char *int_to_str(const void *dados, size_t comprimento) {
	char *str = malloc(64);

	long valor;
	memcpy(&valor, dados, sizeof valor);

	sprintf(str, "%"PRIuMAX, valor);

	return str;
}


static void incompleto(char simbolo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	fprintf(stderr, "Faltando '%c' na linha %d coluna %d\n", simbolo, linha, coluna);
}
static void str_incompleta(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	incompleto('"', lexema, comprimento, linha, coluna);
}
static void char_incompleto(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	incompleto('\'', lexema, comprimento, linha, coluna);
}


static const char *subtipo_str(uint32_t subtipo) {
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
