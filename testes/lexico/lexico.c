#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[]) {
	char str[] = "\xc2\xa9";//{0xc2, 0xa9, 0};//"\xc2\xa9"; //áやきゅられç";
	char str2[] = "\5";
	int d = 0xa9c2;

	const char *c = str;
	printf("<%s>\n", c);

	while(*c != '\0') {
		printf("%x\n", (unsigned char) *c++);
	}
	return 0;
}
