#include "token/identificador.h"

int main(int argc, char const *argv[]) {
	afd_t afd_lexico;
	afd_init(&afd_lexico, 1, 0);

	token_init(&afd_lexico);

	afd_print(&afd_lexico);
	return 0;
}
