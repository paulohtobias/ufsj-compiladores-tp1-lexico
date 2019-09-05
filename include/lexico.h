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

#include "token.h"

int lexico_init();

int lexico_parse(const char *src);

#endif // LEXICO_H
