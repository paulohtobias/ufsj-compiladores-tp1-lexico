/**
 * PCC - Paulo C Compiler
 *
 * token/extra.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <string.h>
#include "token.h"
#include "utils.h"
#include "plist.h"

// X-Macro pra cada tipo de extra.
#define SUBTIPOS \
	SUBTIPO(TK_EXT_PAR_A, "(", "parenteses-abrir") \
	SUBTIPO(TK_EXT_PAR_F, ")", "parenteses-fechar") \
	SUBTIPO(TK_EXT_COL_A, "[", "colchete-abrir") \
	SUBTIPO(TK_EXT_COL_F, "]", "colchete-fechar") \
	SUBTIPO(TK_EXT_CHA_A, "{", "chave-abrir") \
	SUBTIPO(TK_EXT_CHA_F, "}", "chave-fechar") \
	SUBTIPO(TK_EXT_VIRGULA, ",", "virgula") \
	SUBTIPO(TK_EXT_DOIS_PONTOS, ":", "dois-pontos") \
	SUBTIPO(TK_EXT_PT_VIRGULA, ";", "ponto-virgula")

/// Tipos de extra.
#define SUBTIPO(cod, lex, str) cod,
enum {
	SUBTIPOS
};
#undef SUBTIPO

/// Tipos de extra em string.
#define SUBTIPO(cod, lex, str) str,
const char __extras[][32] = {
	SUBTIPOS
};
#undef SUBTIPO
size_t __extras_quantidade = ARR_TAMANHO(__extras);

/// Vetor com os lexemas de extra.
#define SUBTIPO(cod, lex, str) lex,
const char __extras_lexemas[][32] = {
	SUBTIPOS
};
#undef SUBTIPO

/// Função adicionar
static void extra_adicionar(const char *arquivo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);

/// Função to_str
const char *extra_str(uint32_t subtipo);

int token_extra_init(afd_t *afd) {
	int res = 0;

	afd_t afd_ext;
	afd_init(&afd_ext, 8);

	// Estado 0.
	#define SUBTIPO(cod, lex, str) {cod + 1, {"\\" lex, "\\" lex, NULL}},
	afd_transicao_t transicoes_0[] = {
		SUBTIPOS
	};
	#undef SUBTIPO
	afd_ext.estados[0] = afd_criar_estado(transicoes_0, ARR_TAMANHO(transicoes_0), false, NULL);

	// Outros estados.
	for (size_t i = 0; i < __extras_quantidade; i++) {
		plist_append(afd_ext.estados, afd_criar_estado(NULL, 0, true, extra_adicionar));
	}

	if ((res = afd_mesclar_automatos(afd, &afd_ext)) != AFD_OK) {
		LOG_PCC_ERRO(0, NULL, "%s: Não foi possível iniciar: %s\n", __FUNCTION__, afd_get_erro(res));
		goto fim;
	}

	afd_liberar(&afd_ext);

	// Inicializando a tabela de símbolos de extra.
	plist_create(tabela_simbolos[TK_EXT], __extras_quantidade);
	memset(tabela_simbolos[TK_EXT], 0, __extras_quantidade * sizeof *tabela_simbolos[TK_EXT]);

fim:
	return res;
}

static void extra_adicionar(const char *arquivo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	// subtipo é o índice do lexema na tabela.
	int subtipo;
	for (subtipo = 0; subtipo < __extras_quantidade; subtipo++) {
		size_t i;
		for (i = 0; __extras_lexemas[subtipo][i] != '\0' && i < comprimento; i++) {
			if (__extras_lexemas[subtipo][i] != lexema[i]) {
				break;
			}
		}

		// Houve casamento.
		if (__extras_lexemas[subtipo][i] == '\0' && i == comprimento) {
			break;
		}
	}

	// Deve ser sempre verdadeiro.
	if (subtipo < __extras_quantidade) {
		token_t token = token_criar(TK_EXT, subtipo, arquivo, lexema, comprimento, linha, coluna);
		token.subtipo_to_str = extra_str;

		token_adicionar(&token);

	} else {
		LOG_PCC_ERRO(0, NULL, "%s: extra \"%.*s\" não reconhecido\n", __FUNCTION__, (int) comprimento, lexema);
	}
}

const char *extra_str(uint32_t subtipo) {
	if (subtipo < __extras_quantidade) {
		return __extras[subtipo];
	}

	return "Erro: subtipo de extra inválido";
}
