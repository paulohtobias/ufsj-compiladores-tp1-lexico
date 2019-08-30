/**
 * PCC - Paulo C Compiler
 *
 * utils.c
 *
 * Paulo Tobias
 * paulohtobias@outlook.com
 */

#include <stdio.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
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

size_t regex_match(const char *str, const char *pattern_str) {
	size_t offset = 0;

	// Handling NULL strings.
	if (str == NULL || pattern_str == NULL) {
		return 0;
	}

	// Code adapted from https://github.com/luvit/pcre2/blob/a677f5b51bac251082856d35a48a01670e2fd4a7/src/pcre2demo.c#L61
	pcre2_code *re;
	PCRE2_SPTR pattern;     /* PCRE2_SPTR is a pointer to unsigned code units of */
	PCRE2_SPTR subject;     /* the appropriate width (8, 16, or 32 bits). */

	int errornumber;
	int rc;

	PCRE2_SIZE erroroffset;

	size_t subject_length;
	pcre2_match_data *match_data;


	/* As pattern and subject are char arguments, they can be straightforwardly
	cast to PCRE2_SPTR as we are working in 8-bit code units. */

	pattern = (PCRE2_SPTR)pattern_str;
	subject = (PCRE2_SPTR)str;
	subject_length = strlen((char *)subject);


	/*************************************************************************
	* Now we are going to compile the regular expression pattern, and handle *
	* any errors that are detected.                                          *
	*************************************************************************/

	re = pcre2_compile(
		pattern,                     /* the pattern */
		PCRE2_ZERO_TERMINATED,       /* indicates pattern is zero-terminated */
		PCRE2_UTF | PCRE2_ANCHORED | /* flags */
		PCRE2_NO_AUTO_CAPTURE,
		&errornumber,                /* for error number */
		&erroroffset,                /* for error offset */
		NULL                         /* use default compile context */
	);

	/* Compilation failed: print the error message and exit. */

	if (re == NULL) {
		PCRE2_UCHAR buffer[256];
		pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
		printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset,
			buffer);
		return 0;
	}


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

	printf("\nMatch succeeded at offset %d\n", offset);

exit:
	pcre2_match_data_free(match_data);   /* Release memory used for the match */
	pcre2_code_free(re);                 /* data and the compiled pattern. */
	return offset;
}
