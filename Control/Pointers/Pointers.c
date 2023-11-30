#include <stdio.h>
#include <stdlib.h>



int main(void) {

    int a = 5;
    int *pa = &a;

    printf("pa = %p (a : %d)", pa, *pa);

    return 0;
}