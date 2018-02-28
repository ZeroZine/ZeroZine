#include <iostream>

int main()
{
  //Reservo espacio para un char
  char* n = new char;
  
  //Uso el char
  std::cin >> *n;
  std::cout << *n;

  //Ya no lo necesito y lo borro
  delete n;
  n = NULL;

  //Se deferencia un puntero nulo
  std::cout << *n << std::endl;
   
}
