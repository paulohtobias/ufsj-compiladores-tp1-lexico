/**
 * PCC - Paulo C Compiler
 *
 * afd.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include "afd.h"

#define AFD_CODIGO(cod, msg) msg,
const char __afd_msgs[][128] = {
	AFD_CODIGOS
};
#undef AFD_CODIGO

#undef AFD_CODIGOS
