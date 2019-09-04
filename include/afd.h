/**
 * PCC - Paulo C Compiler
 *
 * afd.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef AFD_H
#define AFD_H

#include <stdbool.h>
#include <stdint.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <stddef.h>

/// Incremento na capacidade.
#ifndef AFD_CAPACIDADE_INC
#define AFD_CAPACIDADE_INC 10
#endif

// X-macro para gerar os códigos e mensagens de erro.
#define AFD_CODIGOS \
	AFD_CODIGO(AFD_OK, "Nenhum erro") \
	AFD_CODIGO(AFD_EINVAL, "Argumento inválido") \
	AFD_CODIGO(AFD_EEXISTE, "Estado já existe e junção não foi possível")

#define AFD_CODIGO(cod, msg) cod,
enum {
	AFD_CODIGOS
};
#undef AFD_CODIGO

const char *afd_get_erro(int cod);


/// Definindo structs.

typedef struct afd_transicao_pattern_t {
	char *name;
	const char *str;
	pcre2_code *compiled;
} afd_transicao_pattern_t;

typedef struct afd_transicao_t {
	afd_transicao_pattern_t pattern;
	int32_t estado_indice;
} afd_transicao_t;

typedef struct afd_estado_t {
	afd_transicao_t *transicoes;

	bool final;

	void (*acao)(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna);
} afd_estado_t;

typedef struct afd_t {
	afd_estado_t *estados;
} afd_t;

/**
 * TODO
 */
int afd_init(afd_t *afd, size_t quantidade_estados);

/**
 * TODO
 */
afd_estado_t afd_criar_estado(afd_transicao_t *transicoes, size_t transicoes_quantidade, bool final, void (*acao)(const char *lexema, size_t comprimento, int32_t linha, int32_t coluna));

/**
 * TODO
 */
void afd_liberar_estado(afd_estado_t *estado);

/**
 * TODO: Retorna o índice do estado...
 */
int32_t afd_estado_get_proximo_estado(const afd_estado_t *estado, const char **cursor);

/**
 * Procura a transição na lista de transições do estado.
 *
 * PARAMETROS:
 *     - estado: ponteiro pra um `afd_estado_t`.
 *     - transicao_id: string que será comparada com o atributo `id`
 *       das transições de `estado`.
 *
 * RETORNO:
 *     Se a transição existir, retorna o endereço dela.
 *     Caso contrário, retorna NULL.
 */
afd_transicao_t *afd_estado_get_transicao(const afd_estado_t *estado, const char *transicao_id);

/**
 * TODO
 */
int afd_add_subautomato(afd_t *afd, const afd_transicao_pattern_t *transicoes, int32_t transicoes_quantidade, afd_t *sub);

void afd_print(const afd_t *afd);

#endif // AFD_H
