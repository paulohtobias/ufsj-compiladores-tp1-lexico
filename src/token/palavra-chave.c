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
#define PALAVRA_CHAVE_SUBTIPOS \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_AUTO, "auto") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_BREAK, "break") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_CASE, "case") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_CHAR, "char") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_CONST, "const") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_CONTINUE, "continue") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_DEFAULT, "default") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_DO, "do") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_DOUBLE, "double") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_ELSE, "else") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_ENUM, "enum") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_EXTERN, "extern") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_FLOAT, "float") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_FOR, "for") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_GOTO, "goto") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_IF, "if") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_INT, "int") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_LONG, "long") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_REGISTER, "register") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_RETURN, "return") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_SHORT, "short") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_SIGNED, "signed") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_SIZEOF, "sizeof") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_STATIC, "static") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_STRUCT, "struct") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_SWITCH, "switch") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_TYPEDEF, "typedef") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_UNION, "union") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_UNSIGNED, "unsigned") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_VOID, "void") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_VOLATILE, "volatile") \
	PALAVRA_CHAVE_SUBTIPO(TK_KW_WHILE, "while")

/// Tipos de palavra-chave.
#define PALAVRA_CHAVE_SUBTIPO(cod, str) cod,
enum {
	PALAVRA_CHAVE_SUBTIPOS
};
#undef PALAVRA_CHAVE_SUBTIPO

/// Tipos de palavra-chave em string.
#define PALAVRA_CHAVE_SUBTIPO(cod, str) str,
const char __palavras_chave[][16] = {
	PALAVRA_CHAVE_SUBTIPOS
};
#undef PALAVRA_CHAVE_SUBTIPO
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
