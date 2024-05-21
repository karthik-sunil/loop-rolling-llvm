#include <stdio.h>

int foo(int x){
    x++; 
    return x;
}

int a[7];

int main()
{


   // unrollable
   foo(7);
   foo(9);
   foo(19);
   
   // rollable
   foo(2);
   foo(3);
   foo(4);
   foo(5);
   foo(6);
   foo(7);
   

    return 5;
}
