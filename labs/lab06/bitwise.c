#include <stdio.h>

int main(void)
{
	int a = 1, b = 3, c = 2, d = 4, e = 128, f = 128;
	/*
		a = 00000001
		b = 00000011
		c = 00000010
		d = 00000100
		e = 10000000
		f = 10000000
	 */
	
	printf("a & b is %d\n", (a & b));
	printf("b & b is %d\n", (b & b));
	printf("b & c is %d\n", (b & c));
	printf("b & d is %d\n", (b & d));
	printf("e & f is %d\n", (e & f));

	return 0;
}