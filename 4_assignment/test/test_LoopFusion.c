#include <stdio.h>

/*
 * Caso 1:
 * L0 guarded, L1 guarded, adiacenti.
 *
 * Struttura attesa:
 * if (...) { loop0 }
 * if (...) { loop1 }
 *
 * Se L0 viene saltato, si arriva direttamente al guard di L1.
 * Se L0 viene eseguito, all'uscita si arriva direttamente al guard di L1.
 */
void both_guarded_adjacent(int N, int M) {
    int t1 = 11;
    int t2 = 21;
    if (N > 0) {
        for (int i = 0; i < t1; i++) {
            printf("L0 guarded adjacent: %d\n", i);
        }
    }

    if (M > 0) {
        for (int j = 0; j < t2; j++) {
            printf("L1 guarded adjacent: %d\n", j);
        }
    }
}


/*
 * Caso 2:
 * L0 guarded, L1 non guarded, NON adiacenti.
 *
 * Tra L0 e L1 c'è una istruzione extra.
 
void l0_guarded_l1_not_guarded_not_adjacent(int N) {
    if (N > 0) {
        for (int i = 0; i < 10; i++) {
            printf("L0 guarded: %d\n", i);
        }
    }

    puts("statement between loops");

    for (int j = 0; j < 10; j++) {
        printf("L1 non guarded: %d\n", j);
    }
}
*/

/*
 * Caso 3:
 * L0 guarded, L1 non guarded, adiacenti.
 *
 * Se L0 viene saltato, si arriva direttamente al preheader di L1.
 * Se L0 viene eseguito, all'uscita si arriva direttamente al preheader di L1.
 
void l0_guarded_l1_not_guarded_adjacent(int N) {
    if (N > 0) {
        for (int i = 0; i < 10; i++) {
            printf("L0 guarded adjacent: %d\n", i);
        }
    }

    for (int j = 0; j < 10; j++) {
        printf("L1 non guarded adjacent: %d\n", j);
    }
}*/


/*
 * Caso 4:
 * L0 non guarded, L1 guarded, adiacenti.
 *
 * L'uscita di L0 arriva direttamente al guard di L1.

void l0_not_guarded_l1_guarded_adjacent(int M) {
    for (int i = 0; i < 10; i++) {
        printf("L0 non guarded adjacent: %d\n", i);
    }

    if (M > 0) {
        for (int j = 0; j < 10; j++) {
            printf("L1 guarded adjacent: %d\n", j);
        }
    }
}
 */

/*
 * Caso 5:
 * L0 non guarded, L1 guarded, NON adiacenti.
 *
 * Tra L0 e il guard di L1 c'è una istruzione extra.
 
void l0_not_guarded_l1_guarded_not_adjacent(int M) {
    for (int i = 0; i < 10; i++) {
        printf("L0 non guarded: %d\n", i);
    }

    puts("statement between loops");

    if (M > 0) {
        for (int j = 0; j < 10; j++) {
            printf("L1 guarded: %d\n", j);
        }
    }
}*/


/*
 * Caso 6:
 * L0 guarded, L1 guarded, NON adiacenti.
 *
 * Tra il primo if/loop e il guard del secondo loop c'è una istruzione extra.
 
void both_guarded_not_adjacent(int N, int M) {
    if (N > 0) {
        for (int i = 0; i < 10; i++) {
            printf("L0 guarded: %d\n", i);
        }
    }

    puts("statement between guarded loops");

    if (M > 0) {
        for (int j = 0; j < 10; j++) {
            printf("L1 guarded: %d\n", j);
        }
    }
}
*/

/*
 * Caso 7:
 * L0 non guarded, L1 non guarded, adiacenti.
 *
 * È il tuo caso base.
 
void both_not_guarded_adjacent(void) {
    for (int i = 0; i < 10; i++) {
        printf("L0 non guarded adjacent: %d\n", i);
    }

    for (int j = 0; j < 10; j++) {
        printf("L1 non guarded adjacent: %d\n", j);
    }
}
*/

/*
 * Caso 8:
 * L0 non guarded, L1 non guarded, NON adiacenti.
 *
 * Tra i due loop c'è una istruzione extra.
 
void both_not_guarded_not_adjacent(void) {
    for (int i = 0; i < 10; i++) {
        printf("L0 non guarded: %d\n", i);
    }

    puts("statement between loops");

    for (int j = 0; j < 10; j++) {
        printf("L1 non guarded: %d\n", j);
    }
}
*/
