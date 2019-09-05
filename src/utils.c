/**
 * PCC - Paulo C Compiler
 *
 * utils.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <stdio.h>
#include <string.h>
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

int8_t utf8_simbolo_comprimento(const char *str) {
	uint8_t fb = (unsigned char) *str;

	if (fb < 0x7f) {
		return 1;
	} else if (fb < 0xe0) {
		return 2;
	} else if (fb < 0xf0) {
		return 3;
	} else if (fb < 0xf8) {
		return 4;
	} else {
		printf("This shouldn't happen.\n");
		return -1;
	}
}

pcre2_code *regex_compile(const char *pattern, PCRE2_SIZE tamanho) {
	int errornumber;
	PCRE2_SIZE erroroffset;

	if (tamanho == 0) {
		tamanho = PCRE2_ZERO_TERMINATED;
	}

	return pcre2_compile(
		(PCRE2_SPTR) pattern, /* the pattern */
		tamanho,              /* indicates pattern is zero-terminated */
		REGEX_OPTIONS,        /* default flags */
		&errornumber,         /* for error number */
		&erroroffset,         /* for error offset */
		NULL                  /* use default compile context */
	);
}

size_t regex_match(const char *str, pcre2_code *re) {
	size_t offset = 0;

	// Handling NULL strings.
	if (str == NULL || re == NULL) {
		return 0;
	}

	PCRE2_SPTR subject;     /* the appropriate width (8, 16, or 32 bits). */

	int rc;

	size_t subject_length;
	pcre2_match_data *match_data;


	/* As subject is char argument, it can be straightforwardly
	cast to PCRE2_SPTR as we are working in 8-bit code units. */

	subject = (PCRE2_SPTR)str;
	subject_length = strlen((char *)subject);


	/*************************************************************************
	* If the compilation succeeded, we call PCRE again, in order to do a     *
	* pattern match against the subject string. This does just ONE match. If *
	* further matching is needed, it will be done below. Before running the  *
	* match we must set up a match_data block for holding the result.        *
	*************************************************************************/

	/* Using this function ensures that the block is exactly the right size for
	the number of capturing parentheses in the pattern. */

	match_data = pcre2_match_data_create_from_pattern(re, NULL);

	rc = pcre2_match(
		re,                   /* the compiled pattern */
		subject,              /* the subject string */
		subject_length,       /* the length of the subject */
		0,                    /* start at offset 0 in the subject */
		PCRE2_ANCHORED,       /* default options */
		match_data,           /* block for storing the result */
		NULL                  /* use default match context */
	);

	/* Matching failed: handle error cases */

	if (rc <= 0) {
		goto exit;
	}

	/* Match succeded. Get a pointer to the offset vector, where string offsets are
	stored. */

	PCRE2_SIZE *ovector = pcre2_get_ovector_pointer(match_data);
	offset = ovector[1];

exit:
	pcre2_match_data_free(match_data);   /* Release memory used for the match */
	return offset;
}
