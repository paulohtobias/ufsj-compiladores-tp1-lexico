#include <stdio.h>
#include "lexico.h"
#include "utils.h"

int main(int argc, char const *argv[]) {
	const char *filename = "main.c";
	if (argc >= 2) {
		filename = argv[1];
	}

	size_t comprimento;
	char *emoji_💩_no_meio = file_to_str(filename, &comprimento);

	lexico_parse(emoji_💩_no_meio);

	free(emoji_💩_no_meio);

	return 0;
}
