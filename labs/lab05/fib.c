#include <stdio.h>
#include <stdlib.h>
 
int fib(int);
 
int main(void)
{
   int n;
   printf("n = ");
   scanf("%d", &n);
   if (n >= 1) {
      printf("fib(n) = %d\n", fib(n));
      return 0;
   }
   else {
      printf("n must be > 0\n");
      return 1;
   }
}
 
int fib(int n)
{
   if (n < 1)
      return 0;
   else if (n == 1)
      return 1;
   else
      return fib(n-1) + fib(n-2);
}