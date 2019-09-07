/**
 * PCC - Paulo C Compiler
 *
 * token/operador.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <string.h>
#include "token.h"
#include "utils.h"
#include "plist.h"

// X-Macro pra cada tipo de operador.
#define SUBTIPOS \
	SUBTIPO(TK_OP_SETA, "->", "seta") \
	SUBTIPO(TK_OP_PONTO, ".", "ponto") \
	SUBTIPO(TK_OP_NOTL, "!", "notl") \
	SUBTIPO(TK_OP_NOTB, "~", "notb") \
	SUBTIPO(TK_OP_INC, "++", "inc") \
	SUBTIPO(TK_OP_DEC, "--", "dec") \
	SUBTIPO(TK_OP_AST, "*", "ast") \
	SUBTIPO(TK_OP_EC, "&", "ec") \
	SUBTIPO(TK_OP_BARRA, "/", "barra") \
	SUBTIPO(TK_OP_MOD, "%", "mod") \
	SUBTIPO(TK_OP_MAIS, "+", "mais") \
	SUBTIPO(TK_OP_MENOS, "-", "menos") \
	SUBTIPO(TK_OP_SHL, "<<", "shl") \
	SUBTIPO(TK_OP_SHR, ">>", "shr") \
	SUBTIPO(TK_OP_MENOR, "<", "menor") \
	SUBTIPO(TK_OP_MENOR_IGUAL, "<=", "menor_igual") \
	SUBTIPO(TK_OP_MAIOR, ">", "maior") \
	SUBTIPO(TK_OP_MAIOR_IGUAL, ">=", "maior_igual") \
	SUBTIPO(TK_OP_IGUAL, "==", "igual") \
	SUBTIPO(TK_OP_DIF, "!=", "dif") \
	SUBTIPO(TK_OP_XOR, "^", "xor") \
	SUBTIPO(TK_OP_ORB, "|", "orb") \
	SUBTIPO(TK_OP_EL, "&&", "el") \
	SUBTIPO(TK_OP_ORL, "||", "orl") \
	SUBTIPO(TK_OP_TER_C, "?", "ter_c") \
	SUBTIPO(TK_OP_ATRIB, "=", "atrib") \
	SUBTIPO(TK_OP_AUTO_INC, "+=", "auto_inc") \
	SUBTIPO(TK_OP_AUTO_DEC, "-=", "auto_dec") \
	SUBTIPO(TK_OP_AUTO_MUL, "*=", "auto_mul") \
	SUBTIPO(TK_OP_AUTO_DIV, "/=", "auto_div") \
	SUBTIPO(TK_OP_AUTO_MOD, "%=", "auto_mod") \
	SUBTIPO(TK_OP_AUTO_EB, "&=", "auto_eb") \
	SUBTIPO(TK_OP_AUTO_XORB, "^=", "auto_xorb") \
	SUBTIPO(TK_OP_AUTO_ORB, "|=", "auto_orb") \
	SUBTIPO(TK_OP_AUTO_SHL, "<<=", "auto_shl") \
	SUBTIPO(TK_OP_AUTO_SHR, ">>=", "auto_shr")


/// Tipos de operador.
#define SUBTIPO(cod, lex, str) cod,
enum {
	SUBTIPOS
};
#undef SUBTIPO

/// Tipos de operador em string.
#define SUBTIPO(cod, lex, str) str,
const char __operadores[][32] = {
	SUBTIPOS
};
#undef SUBTIPO
size_t __operadores_quantidade = ARR_TAMANHO(__operadores);

/// Vetor com os lexemas de operador.
#define SUBTIPO(cod, lex, str) lex,
const char __operadores_lexemas[][32] = {
	SUBTIPOS
};
#undef SUBTIPO

/// Função adicionar
static void operador_adicionar(const char *arquivo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);

/// Função to_str
const char *operador_str(uint32_t subtipo);

int token_operador_init(afd_t *afd) {
	int res = 0;

	afd_t afd_op;
	afd_init(&afd_op, 37);

	// Estado 0.
	afd_transicao_t transicoes_0[] = {
		{1, {"\\.", "\\.", NULL}},
		{2, {"-", "-", NULL}},
		{6, {"!", "!", NULL}},
		{8, {"\\+", "\\+", NULL}},
		{11, {"\\*", "\\*", NULL}},
		{13, {"&", "&", NULL}},
		{16, {"/", "/", NULL}},
		{18, {"%", "%", NULL}},
		{20, {"<", "<", NULL}},
		{24, {">", ">", NULL}},
		{28, {"=", "=", NULL}},
		{30, {"\\^", "\\^", NULL}},
		{32, {"\\|", "\\|", NULL}},
		{35, {"~", "~", NULL}},
		{36, {"\\?", "\\?", NULL}},
	};
	afd_op.estados[0] = afd_criar_estado(transicoes_0, ARR_TAMANHO(transicoes_0), false, NULL);

	// Estado 1
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));

	// Estado 2
	afd_transicao_t transicoes_2[] = {
		{3, {">", ">", NULL}},
		{4, {"-", "-", NULL}},
		{5, {"=", "=", NULL}},
	};
	plist_append(
		afd_op.estados,
		afd_criar_estado(transicoes_2, ARR_TAMANHO(transicoes_2), true, operador_adicionar)
	);
	// Estado 3
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));
	// Estado 4
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));
	// Estado 5
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));

	// Estado 6
	afd_transicao_t transicoes_6[] = {
		{7, {"=", "=", NULL}},
	};
	plist_append(
		afd_op.estados,
		afd_criar_estado(transicoes_6, ARR_TAMANHO(transicoes_6), true, operador_adicionar)
	);
	// Estado 7
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));

	// Estado 8
	afd_transicao_t transicoes_8[] = {
		{9, {"\\+", "\\+", NULL}},
		{10, {"=", "=", NULL}},
	};
	plist_append(
		afd_op.estados,
		afd_criar_estado(transicoes_8, ARR_TAMANHO(transicoes_8), true, operador_adicionar)
	);
	// Estado 9
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));
	// Estado 10
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));

	// Estado 11
	afd_transicao_t transicoes_11[] = {
		{12, {"=", "=", NULL}},
	};
	plist_append(
		afd_op.estados,
		afd_criar_estado(transicoes_11, ARR_TAMANHO(transicoes_11), true, operador_adicionar)
	);
	// Estado 12
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));

	// Estado 13
	afd_transicao_t transicoes_13[] = {
		{14, {"&", "&", NULL}},
		{15, {"=", "=", NULL}},
	};
	plist_append(
		afd_op.estados,
		afd_criar_estado(transicoes_13, ARR_TAMANHO(transicoes_13), true, operador_adicionar)
	);
	// Estado 14
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));
	// Estado 15
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));

	// Estado 16
	afd_transicao_t transicoes_16[] = {
		{17, {"=", "=", NULL}},
	};
	plist_append(
		afd_op.estados,
		afd_criar_estado(transicoes_16, ARR_TAMANHO(transicoes_16), true, operador_adicionar)
	);
	// Estado 17
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));

	// Estado 18
	afd_transicao_t transicoes_18[] = {
		{19, {"=", "=", NULL}},
	};
	plist_append(
		afd_op.estados,
		afd_criar_estado(transicoes_18, ARR_TAMANHO(transicoes_18), true, operador_adicionar)
	);
	// Estado 19
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));

	// Estado 20
	afd_transicao_t transicoes_20[] = {
		{21, {"<", "<", NULL}},
		{23, {"=", "=", NULL}},
	};
	plist_append(
		afd_op.estados,
		afd_criar_estado(transicoes_20, ARR_TAMANHO(transicoes_20), true, operador_adicionar)
	);
	// Estado 21
	afd_transicao_t transicoes_21[] = {
		{22, {"=", "=", NULL}},
	};
	plist_append(
		afd_op.estados,
		afd_criar_estado(transicoes_21, ARR_TAMANHO(transicoes_21), true, operador_adicionar)
	);
	// Estado 22
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));
	// Estado 23
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));

	// Estado 24
	afd_transicao_t transicoes_24[] = {
		{25, {">", ">", NULL}},
		{27, {"=", "=", NULL}},
	};
	plist_append(
		afd_op.estados,
		afd_criar_estado(transicoes_24, ARR_TAMANHO(transicoes_24), true, operador_adicionar)
	);
	// Estado 25
	afd_transicao_t transicoes_25[] = {
		{26, {"=", "=", NULL}},
	};
	plist_append(
		afd_op.estados,
		afd_criar_estado(transicoes_25, ARR_TAMANHO(transicoes_25), true, operador_adicionar)
	);
	// Estado 26
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));
	// Estado 27
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));

	// Estado 28
	afd_transicao_t transicoes_28[] = {
		{29, {"=", "=", NULL}},
	};
	plist_append(
		afd_op.estados,
		afd_criar_estado(transicoes_28, ARR_TAMANHO(transicoes_28), true, operador_adicionar)
	);
	// Estado 29
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));

	// Estado 30
	afd_transicao_t transicoes_30[] = {
		{31, {"=", "=", NULL}},
	};
	plist_append(
		afd_op.estados,
		afd_criar_estado(transicoes_30, ARR_TAMANHO(transicoes_30), true, operador_adicionar)
	);
	// Estado 31
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));

	// Estado 32
	afd_transicao_t transicoes_32[] = {
		{33, {"\\|", "\\|", NULL}},
		{34, {"=", "=", NULL}},
	};
	plist_append(
		afd_op.estados,
		afd_criar_estado(transicoes_32, ARR_TAMANHO(transicoes_32), true, operador_adicionar)
	);
	// Estado 33
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));
	// Estado 34
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));

	// Estado 35
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));
	// Estado 36
	plist_append(afd_op.estados, afd_criar_estado(NULL, 0, true, operador_adicionar));

	if ((res = afd_mesclar_automatos(afd, &afd_op)) != AFD_OK) {
		LOG_PCC_ERRO(0, NULL, "%s: Não foi possível iniciar: %s\n", __FUNCTION__, afd_get_erro(res));
		goto fim;
	}

	// Inicializando a tabela de símbolos de operador.
	plist_create(tabela_simbolos[TK_OP], __operadores_quantidade);
	memset(tabela_simbolos[TK_OP], 0, __operadores_quantidade * sizeof *tabela_simbolos[TK_OP]);

fim:
	return res;
}

static void operador_adicionar(const char *arquivo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	// subtipo é o índice do lexema na tabela.
	int subtipo;
	for (subtipo = 0; subtipo < __operadores_quantidade; subtipo++) {
		size_t i;
		for (i = 0; __operadores_lexemas[subtipo][i] != '\0' && lexema[i] != '0'; i++) {
			if (__operadores_lexemas[subtipo][i] != lexema[i]) {
				break;
			}
		}

		// Houve casamento.
		if (__operadores_lexemas[subtipo][i] == '\0') {
			break;
		}
	}

	// Deve ser sempre verdadeiro.
	if (subtipo < __operadores_quantidade) {
		token_t token = token_criar(TK_OP, subtipo, arquivo, lexema, comprimento, linha, coluna);
		token.subtipo_to_str = operador_str;

		token_adicionar(&token);
	} else {
		LOG_PCC_ERRO(0, NULL, "%s: operador \"%.*s\" não reconhecido\n", __FUNCTION__, (int) comprimento, lexema);
	}
}

const char *operador_str(uint32_t subtipo) {
	if (subtipo < __operadores_quantidade) {
		return __operadores[subtipo];
	}

	// TODO: warning
	return "Erro: subtipo de operador inválido";
}
