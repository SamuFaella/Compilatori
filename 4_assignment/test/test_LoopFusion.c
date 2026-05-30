#include <stdio.h>

void test(int N1, int N2) {
    // Primo guard loop
    /*
    if (N1 > 0) {
        for (int i = 0; i < N1; i++) {
            printf("Loop 1: i = %d\n", i);
        }
    }

    // Secondo guard loop
    if (N2 > 0) {
        for (int j = 0; j < N2; j++) {
            printf("Loop 2: j = %d\n", j);
        }
    }
    int i = 0;
    int j = 0;
    do {
        printf("Loop 1: i = %d\n", i);
        i++;
    } while (N1 > 0);

    do{
        printf("Loop 2: j = %d\n", j);
        j++;
    } while (N2 > 0);*/
    
    for(int i=0; i < 10; i++){
        printf("Loop 1: i = %d\n", i);
    }
    for(int j=0; j < 10; j++){
        printf("Loop 2: j = %d\n", j);
    }
}

