#include <thread>
#include <iostream>
#include <chrono>

using namespace std;

volatile int global;

void f1() {
  while(global != 1)
    this_thread::sleep_for(chrono::milliseconds(500));
}
void f2() {
  while(global != 2)
    this_thread::sleep_for(chrono::milliseconds(500));
}

int main() {
  thread t1(f1);
  thread t2(f2); 
  global=2;
  global=1;
  t1.join();
  t2.join();
}
