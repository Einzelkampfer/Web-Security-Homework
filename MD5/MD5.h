#ifndef MD5_H
#define MD5_H

#include <cstdio>
#include <cstring>

using namespace std;
// define the 1 byte memory
typedef unsigned char bit8;
// define the 4 bytes memory
typedef unsigned int bit32;
// define the 8 bytes memory
typedef unsigned long long bit64;
// the four magic number
bit32 magics[] = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
//bit need to shift for each round
bit32 shift[4][4] = {{7, 12, 17, 22}, {5, 9, 14, 20}, {4, 11, 16, 23}, {6, 10, 15, 21}};
//Use binary integer part of the sines of integers as constants
bit32 K[64] = {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8, 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665, 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};
// four non-linear functions
bit32 F(bit32 x, bit32 y, bit32 z) {
	return (x & y) | ((~x) & z);
}
bit32 G(bit32 x, bit32 y, bit32 z) {
	return (x & z) | (y & ~z);
}
bit32 H(bit32 x, bit32 y, bit32 z) {
	return (x ^ y) ^ z;
}
bit32 I(bit32 x, bit32 y, bit32 z) {
	return y ^ (x | ~z);
}
// rotate left function
bit32 rotateLeft(bit32 x, int n) {
	return (x >> (32 - n)) | (x << n);
}
/***preprocessing:
	pad the data till the bit number be a mulitple of 512
***/
void preProcess(bit8* str, bit64& len) {
	// pad a 1000 0000 byte
	bit8 pad = 0x80;
	bit64 tempLen = len * 8;
	str[len++] = pad;

	// pad as many zero till len % 512 = 448
	bit64 bitLen = len * 8;
	while (bitLen % 512 != 448) {
		// the 0000 0000 byte to be pad
		pad = 0x00;
		str[len++] = pad;
		bitLen += 8;
	}

	// pad the 64 bit(8 byte) length of the original string in little endian format
	int i = 8;
	while (i--) {
		// get the low byte and pad first
		bit8 byte = (tempLen & 0x00000000000000FFLL);
		str[len++] = byte;
		tempLen = tempLen >> 8;
	}
}
/***preprocessing:
	convert char* to unsigned char*
***/
void preProcess(char* str, bit64& len) {
	preProcess((unsigned char*)str, len);
}
/***operation:
	divide the preprocessed data into groups
	for each groups make four round to get them md5-encoded
***/
void operate(bit8* str, const bit64 len) {
	// number of groups, each group is 512 bits(64 bytes)
	bit64 groupNum = len / 64LL;
	// for each 512 bits as a group
	for (bit64 i = 0; i < groupNum; ++i) {
		// an array of 32 bits (4 bytes) type
		bit32 x[16];
		bit32 a = magics[0], b = magics[1], c = magics[2], d = magics[3];
		// divide the 512 bits (64 bytes) into 16 section, each section is 32 bits(4 bytes)
		for (bit64 j = i * 64LL; j < (i + 1) * 64LL; j += 4LL) {
			bit64 start = i * 64LL + j;
			// merge the four 8-bit data into a 32-bit one
			x[j / 4] = ((bit32)str[start]) | ((bit32)str[start + 1] << 8) | ((bit32)str[start + 2] << 16) | ((bit32)str[start + 3] << 24);
		}
		//Round 1
		for (int j = 0; j < 64; ++j) {
			bit32 fun, g;
			if (j >= 0 && j < 16) {
				fun = F(b, c, d);
				g = j;
			} else if (j >= 16 && j < 32) {
				fun = G(b, c, d);
				g = (5 * j + 1) % 16;
			} else if (j >= 32 && j < 48) {
				fun = H(b, c, d);
				g = (3 * j + 5) % 16;
			} else if (j >= 48 && j < 64) {
				fun = I(b, c, d);
				g = (7 * j) % 16;
			}
			bit32 dTemp = d;
			d = c;
			c = b;
			b = b + rotateLeft(a + fun + K[j] + x[g], shift[j / 16][j % 4]);
			a = dTemp;
		}
		// update the four magic number
		magics[0] += a;
		magics[1] += b;
		magics[2] += c;
		magics[3] += d;
	}
}
/***operation:
	convert char* to unsigned char*
***/
void operate(char* str, bit64& len) {
	operate((unsigned char*)str, len);
}
/***Print the data in binary format
	function only use for early stage testing
***/
void printBin(char* str, const bit64 len) {
	for (int i = 0; i < len; ++i) {
		bit8 c = str[i];
		int j = 8;
		while (j--) {
			bit8 t = (c >> j) & 0x01;
			printf("%d", t);
		}
    }
	printf("\n");
}
/***Print the md5-data:
	print the four magic numbers in little endian rules
***/
void printMD5() {
	// for each of the magic numbers
	for (int i = 0; i < 4; ++i) {
		bit8 byte;
		bit32 block = magics[i];
		int j = 4;
		// print the lowest byte first
		while (j--) {
			byte = (bit8)(block & 0x000000FF);
			printf("%02x", byte);
			block = block >> 8;
		}
		printf(" ");
	}
	printf("\n");
	// reset the magic numbers
	magics[0] = 0x67452301;
	magics[1] = 0xefcdab89;
	magics[2] = 0x98badcfe;
	magics[3] = 0x10325476;
}
/***md5
	the complete function for convert and output
***/
void md5(char str[]) {
	bit64 len = strlen(str);
	printf("%s in md5 format:\n", str);
	preProcess(str, len);
	// printf("%d byte\n", len);
	// printf("len %d bit\n", len * 8);
	// printBin(str, len);
	operate(str, len);

	printMD5();
}

#endif
