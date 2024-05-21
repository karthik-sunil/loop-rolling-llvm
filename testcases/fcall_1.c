#include <stdio.h>

int foo(int x){
    x++; 
    return x;
}

int bar(int x,int y){
    x++; 
    y++;
    return x;
}

int a[100];
int b[100];

int main()
{

 bar(0,1);

 return 5;
}
