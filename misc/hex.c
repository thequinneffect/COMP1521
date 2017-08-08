#include <stdio.h>
 
int main()
{
    long int binaryval, hexadecimalval = 0, i = 1, remainder;
 
    printf("Enter the binary number: ");
    scanf("%ld", &binaryval);
    printf("%ld entered\n", binaryval);
    while (binaryval != 0)
    {
        remainder = binaryval % 10;
        hexadecimalval = hexadecimalval + remainder * i;
        i = i * 2;
        binaryval = binaryval / 10;
    }
    printf("hex value is %ld\n", hexadecimalval);
    printf("Equivalent hexadecimal value: %lX\n", hexadecimalval);
    return 0;
}