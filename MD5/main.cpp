#include "MD5.h"
#include <cstring>
#include <cstdio>
using namespace std;


int main() {
	char str[10000];
	printf("Enter the string you want to encode(EOF to end):\n");
	while (scanf("%s", str) != EOF) {
		md5(str);
		printf("Enter the string you want to encode(EOF to end):\n");
	}
	return 0;
}
