/**
 * Option Parser
 *
 * opcoes.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef OPCOES_H
#define OPCOES_H

#include <stddef.h>

#ifndef LARGURA_MAX
	#define LARGURA_MAX 40
#endif
#define DESCRICAO_MAX 256

typedef struct opcao_t {
	///Caractere usado na linha de comando para identificar esta opção.
	char flag;

	///Tipo da opção.
	struct {
		enum {FT_BOOL, FT_CHAR, FT_INT, FT_DOUBLE, FT_STR} cod;
		size_t tamanho;
		int (*conversao)(struct opcao_t *, const void *);
	} tipo;

	///Endereço da variável que armazenará o valor.
	void *buffer;

	///Afeta como a opção aparecerá na ajuda.
	///Caso nome_valor seja da forma nome=valor_padrao,
	///valor_padrao será convertido e atribuído em valor
	///inicialmente.
	///Caso o tipo seja FT_BOOL e nome_valor seja != NULL,
	///então é assumido que nome_valor é o valor padrão (0 ou 1).
	const char *nome_valor;

	///Indica o índice no vetor de opcções se a opção é obrigatória.
	int __obrigatoria;

	///Descrição da opção. Use %s ao se referir a nome_valor.
	const char *descricao;
} opcao_t;

#define OPCAO_INIT(flag, tipo, buffer, nome_valor, descricao)\
	{flag, tipo, buffer, nome_valor, -1, descricao}

#define tipo_bool {FT_BOOL, sizeof(int), conversao_bool}
#define tipo_char {FT_CHAR, sizeof(char), conversao_char}
#define tipo_int {FT_INT, sizeof(int), conversao_int}
#define tipo_double {FT_DOUBLE, sizeof(double), conversao_double}
#define tipo_str(max) {FT_STR, max, conversao_str}

int conversao_bool(opcao_t *opcao, const void *valor);

int conversao_char(opcao_t *opcao, const void *valor);

int conversao_int(opcao_t *opcao, const void *valor);

int conversao_double(opcao_t *opcao, const void *valor);

int conversao_str(opcao_t *opcao, const void *valor);

int parse_args(int argc, char * const argv[], opcao_t *opcoes, int qtd_opcoes);

#endif //OPCOES_H
