#include <stdio.h>

int main(void) {

	int max = 3, N = 10;
	int o = 1, m = 0, i = 0;

	while (o <= max) {
		m = 0;
		while (m < N) {
			i = 0;
			while (i < N) {
				printf("code\n");
				i++;
			}
			m++;
		}
		o++;
	}

	printf("\n");

	for (o = 1; o <= max; o++) {
		for (m = 0; m < N; m++) {
			for (i = 0; i < N; i++) {
				printf("code2\n");
			}
		}
	}

	return 0;
}