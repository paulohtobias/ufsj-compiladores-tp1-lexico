/**
 * PCC - Paulo C Compiler
 *
 * token/comentario.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <string.h>
#include "token.h"
#include "utils.h"
#include "plist.h"

/**
 * Controla se os comentários serão salvos na lista de tokens
 * ou se serão descartados.
 */
bool salvar_comentarios = false;

/// Função adicionar
static void comentario_adicionar(const char *arquivo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);

/// Função de erro.
static void comentario_incompleto(const char *arquivo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);

int token_comentario_init(afd_t *afd) {
	int res = 0;

	afd_t afd_cmt;
	afd_init(&afd_cmt, 6);

	// Estado 0.
	afd_transicao_t transicoes_0[] = {
		{1, {"/", "/", NULL}},
	};
	afd_cmt.estados[0] = afd_criar_estado(transicoes_0, ARR_TAMANHO(transicoes_0), false, NULL);

	// Estado 1
	afd_transicao_t transicoes_1[] = {
		{2, {"/", "/", NULL}},
		{3, {"\\*", "\\*", NULL}},
	};
	plist_append(
		afd_cmt.estados,
		afd_criar_estado(transicoes_1, ARR_TAMANHO(transicoes_1), false, NULL)
	);

	// Estado 2
	afd_transicao_t transicoes_2[] = {
		{2, {"[^\\n]", "[^\\n]", NULL}},
		{5, {"\\n", "\\n", NULL}},
	};
	plist_append(
		afd_cmt.estados,
		afd_criar_estado(transicoes_2, ARR_TAMANHO(transicoes_2), false, NULL)
	);

	// Estado 3
	afd_transicao_t transicoes_3[] = {
		{3, {"[^\\*]", "[^\\*]", NULL}},
		{4, {"\\*", "\\*", NULL}},
	};
	plist_append(
		afd_cmt.estados,
		afd_criar_estado(transicoes_3, ARR_TAMANHO(transicoes_3), false, comentario_incompleto)
	);

	// Estado 4
	afd_transicao_t transicoes_4[] = {
		{3, {"[^/]", "[^/]", NULL}},
		{5, {"/", "/", NULL}},
	};
	plist_append(
		afd_cmt.estados,
		afd_criar_estado(transicoes_4, ARR_TAMANHO(transicoes_4), false, comentario_incompleto)
	);

	// Estado Final
	plist_append(afd_cmt.estados, afd_criar_estado(NULL, 0, true, comentario_adicionar));

	if ((res = afd_mesclar_automatos(afd, &afd_cmt)) != AFD_OK) {
		LOG_PCC_ERRO(0, NULL, "%s: Não foi possível iniciar: %s\n", __FUNCTION__, afd_get_erro(res));
		goto fim;
	}

	// Inicializando a tabela de símbolos de comentario.
	plist_create(tabela_simbolos[TK_CMT], 1);
	tabela_simbolos[TK_CMT][0] = NULL;

fim:
	return res;
}

static void comentario_adicionar(const char *arquivo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	if (salvar_comentarios) {
		token_t token = token_criar(TK_CMT, 0, arquivo, lexema, comprimento, linha, coluna);

		token_adicionar(&token);
	}
}

static void comentario_incompleto(const char *arquivo, const char *lexema, size_t comprimento, int32_t linha, int32_t coluna) {
	LOG_ERRO(arquivo, linha, coluna, lexema, comprimento, "comentário incompleto.");
}
