// where_are_the_bits.c ... determine bit-field order
// COMP1521 Lab 03 Exercise
// Written by ...

#include <stdio.h>
#include <stdlib.h>

#define FORCE_CAST *(int*)&word

struct _bit_fields {
   unsigned int a : 4,
                b : 8,
                c : 20;
};

int main(void)
{
	struct _bit_fields word;

	printf("The struct is %lu bytes long\n",sizeof(word));

	word.a = 1, word.b = 3, word.c = 7;

	for (int i = 31; i >= 0; i--) {
		if ((FORCE_CAST >> i) & 1) {
			printf("1");
		} else {
			printf("0");
		}
	}
	printf("\n");

	return 0;
}