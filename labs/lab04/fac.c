#include <stdio.h>
#include <stdlib.h>
 
int fac(int);
 
int main(void)
{
   int n;
   printf("n: ");
   scanf("%d", &n);
   printf("n! = %d\n", fac(n));
   return 0;
}
 
int fac(int k)
{
   int i, f = 1;
   for (i = 1; i <= k; i++) f = f * i;
   return f;
}