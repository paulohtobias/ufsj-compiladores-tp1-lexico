#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char const *argv[]) {
	char str[] = "\xc2\xa9";//{0xc2, 0xa9, 0};//"\xc2\xa9"; //áやきゅられç";
	char str2[] = "\5";
	int d = 0xa9c2;

	const char *c = str;
	printf("<%s>\n", c);

	int pa = 'ab';
	int pa2 = 0x6162;
	printf("%d\n", pa);
	printf("%d\n", pa2);
	return 0;


	while(*c != '\0') {
		printf("%x\n", (unsigned char) *c++);
	}

	int a = 1;

	int b = a++ + ++a;
	printf("a:%d\nb:%d\n", a, b);
	return 0;
}
