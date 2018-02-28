#include <stdio.h>
#include <unistd.h>

int main(void)
{
  printf("My PID is: %u\n", getpid());
  scanf("%s");
  puts("Bye");
}
