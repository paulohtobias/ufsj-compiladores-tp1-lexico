/**
 * PCC - Paulo C Compiler
 *
 * utils.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <stdio.h>
#include "utils.h"

char *file_to_str(const char *filename, size_t *len) {
	*len = 0;

	FILE *in = fopen(filename, "rb");
	if (in == NULL) {
		return NULL;
	}

	fseek(in, 0, SEEK_END);
	*len = ftell(in);
	fseek(in, 0, SEEK_SET);

	char *str = malloc(*len + 1);
	fread(str, *len, 1, in);
	str[*len] = '\0';

	fclose(in);

	return str;
}
