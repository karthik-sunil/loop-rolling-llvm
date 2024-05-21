#include <stdio.h>

int main()
{
int a[100];

    // unrollable
    a[50] = 1;
    a[59] = 1;
    a[67] = 1; 
    
    // rollable
    a[0] = 1;
    a[1] = 1;
    a[2] = 1;
    a[3] = 1;
    a[4] = 1;
    a[5] = 1; 
    a[6] = 1; 


    return 5;
}
