#include <stdio.h>

/*
 * CASO 1: completamente fondibile
 *
 * Atteso:
 * - adiacenza: yes
 * - trip count: yes
 * - control-flow equivalence: yes
 * - dependence: safe
 * - risultato: fondibile
 */
void fusible_test(int N, int M) {
    int *A;
    int *B;

    if (N < M) {
        for (int i = N; i < M; i++) {
            A[i] = i;
        }

        for (int j = N; j < M; j++) {
            B[j] = j + 1;
        }
    }
}

/*
 * CASO 2: dipendenza negativa
 *
 * L0 scrive A[i]
 * L1 legge A[j + 1]
 *
 * Dopo fusion, alla iterazione j, L1 leggerebbe un valore prodotto
 * da una futura iterazione di L0.
 *
 * Atteso:
 * - adiacenza: yes
 * - trip count: yes
 * - control-flow equivalence: yes
 * - dependence: negative
 * - risultato: NON fondibile
 */
void negative_dep_test(int N) {
    int A[11];
    int B[10];

    if (N > 10) {
        for (int i = 0; i < 10; i++) {
            A[i] = i;
        }

        for (int j = 0; j < 10; j++) {
            B[j] = A[j + 1] + 1;
        }
    }
}

/*
 * CASO 3: dipendenza a distanza zero, generalmente sicura
 *
 * L0 scrive A[i]
 * L1 legge A[j]
 *
 * Dopo fusion:
 *   A[i] = i;
 *   B[i] = A[i] + 1;
 *
 * Il valore letto è prodotto nella stessa iterazione.
 *
 * Atteso:
 * - adiacenza: yes
 * - trip count: yes
 * - control-flow equivalence: yes
 * - dependence: safe oppure classificata sicura dall'analisi semplice
 * - risultato: fondibile
 */
void zero_distance_dep_test(int N) {
    int A[10];
    int B[10];

    if (N > 10) {
        for (int i = 0; i < 10; i++) {
            A[i] = i;
        }

        for (int j = 0; j < 10; j++) {
            B[j] = A[j] + 1;
        }
    }
}

/*
 * CASO 4: non adiacenti
 *
 * C'è uno statement reale tra i due loop.
 *
 * Atteso:
 * - adiacenza: no
 * - risultato: NON fondibile subito
 */
void not_adjacent_test(int N) {
    int A[10];
    int B[10];

    if (N > 10) {
        for (int i = 0; i < 10; i++) {
            A[i] = i;
        }

        printf("Between loops\n");

        for (int j = 0; j < 10; j++) {
            B[j] = j + 1;
        }
    }
}

/*
 * CASO 5: trip count diverso
 *
 * Primo loop: 10 iterazioni.
 * Secondo loop: 8 iterazioni.
 *
 * Atteso:
 * - adiacenza: yes
 * - trip count: no
 * - risultato: NON fondibile
 */
void different_trip_count_test(int N) {
    int A[10];
    int B[10];

    if (N > 10) {
        for (int i = 0; i < 10; i++) {
            A[i] = i;
        }

        for (int j = 0; j < 8; j++) {
            B[j] = j + 1;
        }
    }
}

/*
 * CASO 6: non control-flow equivalent
 *
 * L1 può essere eseguito anche quando L0 viene saltato.
 *
 * Atteso:
 * - adiacenza: può risultare yes o no a seconda del CFG
 * - trip count: yes
 * - control-flow equivalence: no
 * - risultato: NON fondibile
 */
void not_control_flow_equivalent_test(int N) {
    int A[10];
    int B[10];

    if (N > 10) {
        for (int i = 0; i < 10; i++) {
            A[i] = i;
        }
    }

    for (int j = 0; j < 10; j++) {
        B[j] = j + 1;
    }
}

/*
 * CASO 7: due guard separati con stessa condizione
 *
 * Semanticamente sembrano legati, ma a livello CFG sono due branch distinti.
 * DominatorTree/PostDominatorTree non ragionano sul fatto che le condizioni
 * siano uguali.
 *
 * Atteso:
 * - adiacenza: yes, se non c'è codice tra i due if
 * - trip count: yes
 * - control-flow equivalence: no
 * - risultato: NON fondibile
 */
void two_separate_guards_same_condition_test(int N) {
    int A[10];
    int B[10];

    if (N > 10) {
        for (int i = 0; i < 10; i++) {
            A[i] = i;
        }
    }

    if (N > 10) {
        for (int j = 0; j < 10; j++) {
            B[j] = j + 1;
        }
    }
}

/*
 * CASO 8: due guard separati con condizioni diverse
 *
 * Chiaramente non control-flow equivalent:
 * L0 può eseguire senza L1 e viceversa.
 *
 * Atteso:
 * - control-flow equivalence: no
 * - risultato: NON fondibile
 */
void two_separate_guards_different_conditions_test(int N, int M) {
    int A[10];
    int B[10];

    if (N > 10) {
        for (int i = 0; i < 10; i++) {
            A[i] = i;
        }
    }

    if (M > 10) {
        for (int j = 0; j < 10; j++) {
            B[j] = j + 1;
        }
    }
}

/*
 * CASO 9: dipendenza non negativa, offset indietro
 *
 * L0 scrive A[i]
 * L1 legge A[j - 1]
 *
 * Per j = 1 legge A[0], che è già stato prodotto da una iterazione precedente.
 * Dal punto di vista della fusion non è una dipendenza negativa.
 *
 * Nota: j parte da 1 per evitare A[-1].
 *
 * Atteso:
 * - trip count potrebbe essere diverso se il tuo SE vede 9 vs 10
 * - utile soprattutto per testare l'analisi degli offset
 */
void backward_read_not_negative_test(int N) {
    int A[10];
    int B[10];

    if (N > 10) {
        for (int i = 0; i < 9; i++) {
            A[i] = i;
        }

        for (int j = 0; j < 9; j++) {
            B[j] = A[j] + 1;
        }
    }
}

/*
 * CASO 10: accesso con offset positivo maggiore
 *
 * L0 scrive A[i]
 * L1 legge A[j + 3]
 *
 * È simile all'esempio delle slide.
 *
 * Atteso:
 * - dependence: negative
 * - risultato: NON fondibile
 */
void negative_dep_plus_three_test(int N) {
    int A[13];
    int B[10];

    if (N > 10) {
        for (int i = 0; i < 10; i++) {
            A[i] = i;
        }

        for (int j = 0; j < 10; j++) {
            B[j] = A[j + 3] + 1;
        }
    }
}

/*
 * CASO 11: puntatori potenzialmente aliasanti
 *
 * A e B sono parametri. Senza restrict, LLVM può non sapere se aliasano.
 *
 * Atteso:
 * - può diventare UnknownUnsafe
 * - utile per vedere la parte conservativa del passo
 */
void pointer_may_alias_unknown_test(int *A, int *B, int N) {
    if (N > 10) {
        for (int i = 0; i < 10; i++) {
            A[i] = i;
        }

        for (int j = 0; j < 10; j++) {
            B[j] = B[j] + 1;
        }
    }
}

/*
 * CASO 12: loop non guarded, adiacenti, sicuri
 *
 * Caso base senza if.
 *
 * Atteso:
 * - L0 guarded? no
 * - L1 guarded? no
 * - adiacenza: yes
 * - trip count: yes
 * - control-flow equivalence: yes
 * - dependence: safe
 */
void non_guarded_fusible_test(void) {
    int A[10];
    int B[10];

    for (int i = 0; i < 10; i++) {
        A[i] = i;
    }

    for (int j = 0; j < 10; j++) {
        B[j] = j + 1;
    }
}

/*
 * CASO 13: loop non guarded ma non adiacenti
 *
 * Atteso:
 * - L0 guarded? no
 * - L1 guarded? no
 * - adiacenza: no
 */
void non_guarded_not_adjacent_test(void) {
    int A[10];
    int B[10];

    for (int i = 0; i < 10; i++) {
        A[i] = i;
    }

    printf("Between loops\n");

    for (int j = 0; j < 10; j++) {
        B[j] = j + 1;
    }
}