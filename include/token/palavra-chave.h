/**
 * PCC - Paulo C Compiler
 *
 * token/palavra-chave.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef TOKEN_PALAVRA_CHAVE_H
#define TOKEN_PALAVRA_CHAVE_H

#include "token.h"

// TODO: x-macro pra cada KW

int token_palavra_chave_init();

int token_palavra_chave_buscar(const token_t *token);

#endif // TOKEN_PALAVRA_CHAVE_H
