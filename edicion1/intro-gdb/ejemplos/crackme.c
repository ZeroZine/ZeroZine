#include <stdio.h>

int main(void)
{
  char s[6];
  puts("Introduzca el serial: ");
  scanf("%s", s);
  if(*s == '5' && *(s+1) == 'a' && *(s+2) == '7' && *(s+3) == *(s+4))
    puts("Correcto");
  else
    puts("Incorrecto");
}
