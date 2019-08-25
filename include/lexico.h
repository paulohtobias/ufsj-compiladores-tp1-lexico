/**
 * PCC - Paulo C Compiler
 *
 * lexico.h
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#ifndef LEXICO_H
#define LEXICO_H

#include "afd.h"

/**
 * Lista de tokens e suas expressões regulares.
 *
 * String /gm: ("([^"\\]|\\.)*"\s*)+
 * Char: '([^"\\]|\\.)'
 *
 *
 */

extern afd_t lexico_afd;

int lexico_init();

#endif // LEXICO_H
