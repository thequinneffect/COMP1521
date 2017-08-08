#include <stdio.h>
#include <math.h>
#include <string.h>
#include <limits.h>

int main(void)
{
	char *string = "hello";
	int len = strlen(string); 
	printf("%d\n", len);
	char one = '1';
	char nine = '9';

	int num_one = one - '0';
	int num_nine = nine - '0';

	double dec = pow(2,32);

	printf("%d %d\n", num_one, num_nine);
	printf("%lf\n", dec);

	unsigned int un_int = 0;
	for (int power = 0; power < 32; power++)
	{
		un_int += pow(2, power);
	}

	printf("%u\n", un_int);

	double double_dec = 9223372036854775808.0 + 12345;
	printf("%lf\n", double_dec); 

	printf("int max %d\n", INT_MAX);
	printf("uint max %u\n", UINT_MAX);
	printf("long max %ld\n", LONG_MAX);
	printf("ulong max %lu\n", ULONG_MAX);

	return 0;
}