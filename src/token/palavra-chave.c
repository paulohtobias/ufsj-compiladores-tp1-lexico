/**
 * PCC - Paulo C Compiler
 *
 * token/palavra-chave.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include "token/palavra-chave.h"
#include "utils.h"

// X-Macro pra cada palavra chave.
#define SUBTIPOS \
	SUBTIPO(TK_KW_AUTO, "auto") \
	SUBTIPO(TK_KW_BREAK, "break") \
	SUBTIPO(TK_KW_CASE, "case") \
	SUBTIPO(TK_KW_CHAR, "char") \
	SUBTIPO(TK_KW_CONST, "const") \
	SUBTIPO(TK_KW_CONTINUE, "continue") \
	SUBTIPO(TK_KW_DEFAULT, "default") \
	SUBTIPO(TK_KW_DO, "do") \
	SUBTIPO(TK_KW_DOUBLE, "double") \
	SUBTIPO(TK_KW_ELSE, "else") \
	SUBTIPO(TK_KW_ENUM, "enum") \
	SUBTIPO(TK_KW_EXTERN, "extern") \
	SUBTIPO(TK_KW_FLOAT, "float") \
	SUBTIPO(TK_KW_FOR, "for") \
	SUBTIPO(TK_KW_GOTO, "goto") \
	SUBTIPO(TK_KW_IF, "if") \
	SUBTIPO(TK_KW_INT, "int") \
	SUBTIPO(TK_KW_LONG, "long") \
	SUBTIPO(TK_KW_REGISTER, "register") \
	SUBTIPO(TK_KW_RETURN, "return") \
	SUBTIPO(TK_KW_SHORT, "short") \
	SUBTIPO(TK_KW_SIGNED, "signed") \
	SUBTIPO(TK_KW_SIZEOF, "sizeof") \
	SUBTIPO(TK_KW_STATIC, "static") \
	SUBTIPO(TK_KW_STRUCT, "struct") \
	SUBTIPO(TK_KW_SWITCH, "switch") \
	SUBTIPO(TK_KW_TYPEDEF, "typedef") \
	SUBTIPO(TK_KW_UNION, "union") \
	SUBTIPO(TK_KW_UNSIGNED, "unsigned") \
	SUBTIPO(TK_KW_VOID, "void") \
	SUBTIPO(TK_KW_VOLATILE, "volatile") \
	SUBTIPO(TK_KW_WHILE, "while")

/// Tipos de palavra-chave.
#define SUBTIPO(cod, str) cod,
enum {
	SUBTIPOS
};
#undef SUBTIPO

/// Tipos de palavra-chave em string.
#define SUBTIPO(cod, str) str,
const char __palavras_chave[][16] = {
	SUBTIPOS
};
#undef SUBTIPO
size_t __palavras_chave_quantidade = ARR_TAMANHO(__palavras_chave);

int token_palavra_chave_init(afd_t *afd) {
	return 0;
}

int token_palavra_chave_buscar(const token_t *token) {
	int inicio = 0;
	int fim = __palavras_chave_quantidade;
	int index;

	while (1) {
		int novo_index = (int) ((inicio + fim) / 2);
		if (novo_index == index) {
			return -1;
		}

		index = novo_index;

		int i;
		for (i = 0; token->contexto.lexema[i] != '\0' && __palavras_chave[index][i] != '\0'; i++) {
			if (token->contexto.lexema[i] < __palavras_chave[index][i]) {
				fim = index;
				break;
			} else if (token->contexto.lexema[i] > __palavras_chave[index][i]) {
				inicio = index;
				break;
			}
		}

		if (token->contexto.lexema[i] == '\0' && __palavras_chave[index][i] == '\0') {
			return index;
		}
	}
}

const char *token_palavra_chave_subtipo_str(uint32_t subtipo) {
	if (subtipo < __palavras_chave_quantidade) {
		return __palavras_chave[subtipo];
	}

	// TODO: warning
	return "Erro: subtipo de palavra-chave inválido";
}
