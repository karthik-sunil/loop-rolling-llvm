#include <stdio.h>

int foo(int z){
    for(int i=0; i<3; i++) {
        printf("%d",i);
    }
    printf("%d", z);
    return 0;
}

int bar(int z){
    printf("%d", z);
    return 0;
}

// int bar(int y){
//     printf("%d", y);
//     return 2;
// }

int main()
{
int a[15];
int b[6]; 

// int ptr = 0;
// foo(1);
// foo(2);
// foo(3);
// foo(4);

// bar(1);
// bar(2);
// bar(3);
// bar(4);


// a[ptr+0] = 1;
// a[ptr+1] = 1;
// a[ptr+2] = 1;
// a[ptr+3] = 1;

    a[0] = 1;
    a[1] = 1;
    a[2] = 1;
    a[3] = 1;
    a[4] = 1;
    a[5] = 1; 
    a[6] = 1; 

    b[0] = 2; 
    b[2] = 2; 
    b[4] = 2; 

    // a[4] = 1;

    // a[9] = 1;
    // a[11] = 1; 
    // a[13] = 1;




    return 5;
}
