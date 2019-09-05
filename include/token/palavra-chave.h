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

int token_palavra_chave_buscar(const token_t *token);

const char *token_palavra_chave_subtipo_str(uint32_t subtipo);

#endif // TOKEN_PALAVRA_CHAVE_H
