#include <stdio.h>
#include "lexico.h"
#include "utils.h"
#include "plist.h"

int main(int argc, char const *argv[]) {
	const char *filename = "main.c";
	if (argc >= 2) {
		filename = argv[1];
	}

	size_t comprimento;
	char *c2c = file_to_str(filename, &comprimento);

	lexico_parse(c2c);

	free(c2c);

	return 0;
}
