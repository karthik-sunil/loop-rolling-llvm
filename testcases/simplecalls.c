
#include <stdio.h>


void foo(int w, int x){
    w++;
    x++;
}

void bar(int x){
    x++;
}

void spam(char x){
    x++;
}

int main()
{

int a[20];
int b[20];

foo(10,15);
foo(11,17);
foo(12,19);
foo(13,21);
foo(14,23);
foo(15,25);
foo(16,27);
foo(17,29);
foo(18,31);
foo(19,33);
foo(20,35);
foo(21,37);
foo(22,39);
foo(23,41);
foo(24,43);
foo(25,45);
foo(26,47);
foo(27,49);
foo(28,51);
foo(29,53);
bar(7);
bar(10);
bar(13);
bar(16);
bar(19);
bar(22);
bar(25);
bar(28);
bar(31);
bar(34);
bar(37);
bar(40);


return 0;
}

