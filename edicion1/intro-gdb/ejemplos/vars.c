#include <stdio.h>

int sum(int a, int b)
{
  return a+b;
}

int main(int argc, char* argv[])
{
  unsigned int x,*y;
  scanf("%d", &x);
  scanf("%d", y);
  
  printf("Suma: %d\n", sum(x,*y));
  return 0;
}
