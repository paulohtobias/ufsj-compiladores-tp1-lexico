#include <stdio.h>
#include <stdlib.h>
#include "lexico.h"
#include "utils.h"

int main(int argc, char const *argv[]) {
	size_t codigo_fonte_len;
	char *codigo_fonte = file_to_str(argv[1], &codigo_fonte_len);

	lexico_init();

	const char *carriage = codigo_fonte;

	while (*carriage != '\0') {
		token_t *token = afd_get_token(&lexico_afd, &carriage);

		if (token != NULL) {
			printf("%s\n", token->tipo);
			printf("\tsrc:%s\n", token->src);
			printf("\tlinha: %d | coluna: %d\n", token->linha, token->coluna);
			printf("\tvalor: %s\n\n", (char *) token->valor);

			token_free(token);

			//break;
		}
	}

	free(codigo_fonte);

	puts("hex: \xc2\xa9");
	puts("oct: \302\251");

	return 0;
}
