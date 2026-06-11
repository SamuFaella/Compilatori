#include <stdio.h>


/*
 * ============================================================
 * TEST 1: loop semplici fondibili
 * Atteso: FONDIBILE
 * ============================================================
 */
void simple_fusible_local(void) {
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
 * ============================================================
 * TEST 2: dipendenza a distanza zero
 * L0 scrive A[i], L1 legge A[j].
 * Atteso: FONDIBILE
 * ============================================================
 */
void zero_distance_local(void) {
    int A[10];
    int B[10];

    for (int i = 0; i < 10; i++) {
        A[i] = i;
    }

    for (int j = 0; j < 10; j++) {
        B[j] = A[j] + 1;
    }
}

/*
 * ============================================================
 * TEST 3: dipendenza negativa con offset positivo
 * L1 legge A[j + 1].
 * Atteso: NON FONDIBILE
 * ============================================================
 */
void negative_offset_plus_one(void) {
    int A[11];
    int B[10];

    for (int i = 0; i < 10; i++) {
        A[i] = i;
    }

    for (int j = 0; j < 10; j++) {
        B[j] = A[j + 1] + 1;
    }
}

/*
 * ============================================================
 * TEST 4: trip count diverso
 * Primo loop: 10 iterazioni.
 * Secondo loop: 8 iterazioni.
 * Atteso: NON FONDIBILE
 * ============================================================
 */
void different_trip_count_simple(void) {
    int A[10];
    int B[10];

    for (int i = 0; i < 10; i++) {
        A[i] = i;
    }

    for (int j = 0; j < 8; j++) {
        B[j] = j + 1;
    }
}

/*
 * ============================================================
 * TEST 5: loop non adiacenti
 * C'è una printf tra i due loop.
 * Atteso: NON FONDIBILE
 * ============================================================
 */
void not_adjacent_printf(void) {
    int A[10];
    int B[10];

    for (int i = 0; i < 10; i++) {
        A[i] = i;
    }

    printf("between\n");

    for (int j = 0; j < 10; j++) {
        B[j] = j + 1;
    }
}

/*
 * ============================================================
 * TEST 6: non control-flow equivalent
 * Il secondo loop può eseguire anche se il primo viene saltato.
 * Atteso: NON FONDIBILE
 * ============================================================
 */
void not_cfe_simple(int N) {
    int A[10];
    int B[10];

    if (N > 0) {
        for (int i = 0; i < 10; i++) {
            A[i] = i;
        }
    }

    for (int j = 0; j < 10; j++) {
        B[j] = j + 1;
    }
}

/*
 * ============================================================
 * TEST 7: due guard separati con stessa condizione
 * Le condizioni sono uguali, ma il CFG ha due branch distinti.
 * Atteso: NON FONDIBILE
 * ============================================================
 */
void two_guards_same_condition(int N) {
    int A[10];
    int B[10];

    if (N > 0) {
        for (int i = 0; i < 10; i++) {
            A[i] = i;
        }
    }

    if (N > 0) {
        for (int j = 0; j < 10; j++) {
            B[j] = j + 1;
        }
    }
}

/*
 * ============================================================
 * TEST 8: loop decrescenti fondibili
 * Dipendenza a distanza zero con step -1.
 * Atteso: FONDIBILE
 * ============================================================
 */
void decreasing_fusible(void) {
    int A[11];
    int B[11];

    for (int i = 10; i > 0; i--) {
        A[i] = i;
    }

    for (int j = 10; j > 0; j--) {
        B[j] = A[j] + 1;
    }
}

/*
 * ============================================================
 * TEST 9: loop decrescenti con dipendenza negativa
 * L1 legge A[j - 1].
 * Con step -1 è una lettura verso una futura iterazione.
 * Atteso: NON FONDIBILE
 * ============================================================
 */
void decreasing_negative(void) {
    int A[11];
    int B[11];

    for (int i = 10; i > 0; i--) {
        A[i] = i;
    }

    for (int j = 10; j > 0; j--) {
        B[j] = A[j - 1] + 1;
    }
}

/*
 * ============================================================
 * TEST 10: start simbolico con dipendenza negativa
 * L1 legge A[j + 1].
 * Atteso: NON FONDIBILE
 * ============================================================
 */
void negative_symbolic_start(int *A, int *B, int N, int M) {
    if (N < M) {
        for (int i = N; i < M; i++) {
            A[i] = i;
        }

        for (int j = N; j < M; j++) {
            B[j] = A[j + 1] + 1;
        }
    }
}

/*
 * ============================================================
 * TEST 11: start simbolico fondibile con array locali
 * L0 scrive A[i], L1 legge A[j].
 * Atteso: FONDIBILE
 * ============================================================
 */
void fusible_symbolic_start(int N, int M) {
    int A[100];
    int B[100];

    if (0 <= N && N < M && M <= 100) {
        for (int i = N; i < M; i++) {
            A[i] = i;
        }

        for (int j = N; j < M; j++) {
            B[j] = A[j] + 1;
        }
    }
}

/*
 * ============================================================
 * TEST 12: puntatori potenzialmente aliasanti
 * A e B sono parametri, quindi potrebbero aliasare.
 * Atteso: NON FONDIBILE o UnknownUnsafe
 * ============================================================
 */
void pointer_may_alias_unknown(int *A, int *B, int N) {
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
 * ============================================================
 * TEST 13: loop nest fondibile
 * Prima dovrebbero fondersi i loop esterni, poi gli interni.
 * Atteso: FONDIBILE dopo più esecuzioni del pass
 * ============================================================
 */
void nest_fusible(int N) {
    int A[100][100];
    int B[100][100];
    int C[100][100];

    if (N > 0 && N <= 100) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = i + j;
            }
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                B[i][j] = A[i][j] + C[i][j];
            }
        }
    }
}

/*
 * ============================================================
 * TEST 14: loop nest con dipendenza negativa sulla dimensione interna
 * L1 legge A[i][j + 1].
 * Atteso: NON FONDIBILE
 * ============================================================
 */
void nest_negative_inner_dimension(int N) {
    int A[100][101];
    int B[100][100];

    if (N > 0 && N <= 100) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = i + j;
            }
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                B[i][j] = A[i][j + 1] + 1;
            }
        }
    }
}

/*
 * ============================================================
 * TEST 15: loop nest con dipendenza negativa sulla dimensione esterna
 * L1 legge A[i + 1][j].
 * Atteso: NON FONDIBILE
 * ============================================================
 */
void nest_negative_outer_dimension(int N) {
    int A[101][100];
    int B[100][100];

    if (N > 0 && N <= 100) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = i + j;
            }
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                B[i][j] = A[i + 1][j] + 1;
            }
        }
    }
}

/*
 * ============================================================
 * TEST 16: loop nest con trip count interno diverso
 * Interno L0: j < N
 * Interno L1: j < N - 1
 * Atteso: NON FONDIBILE
 * ============================================================
 */
void nest_different_inner_trip_count(int N) {
    int A[100][100];
    int B[100][100];

    if (N > 1 && N <= 100) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = i + j;
            }
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N - 1; j++) {
                B[i][j] = j + 1;
            }
        }
    }
}

/*
 * ============================================================
 * TEST 17: loop nest non adiacente
 * C'è una printf tra i due nest.
 * Atteso: NON FONDIBILE
 * ============================================================
 */
void nest_not_adjacent(int N) {
    int A[100][100];
    int B[100][100];

    if (N > 0 && N <= 100) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = i + j;
            }
        }

        printf("between nests\n");

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                B[i][j] = j + 1;
            }
        }
    }
}

/*
 * ============================================================
 * TEST 18: loop interni fratelli dentro un loop esterno
 * I due loop interni hanno stesso parent.
 * Atteso: FONDIBILE
 * ============================================================
 */
void inner_sibling_fusible(int N, int M) {
    int A[100][100];
    int B[100][100];

    if (N > 0 && N <= 100 && M > 0 && M <= 100) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                A[i][j] = j;
            }

            for (int k = 0; k < M; k++) {
                B[i][k] = k + 1;
            }
        }
    }
}

/*
 * ============================================================
 * TEST 19: loop interni fratelli con dipendenza negativa
 * L1 legge A[i][k + 1].
 * Atteso: NON FONDIBILE
 * ============================================================
 */
void inner_sibling_negative(int N, int M) {
    int A[100][101];
    int B[100][100];

    if (N > 0 && N <= 100 && M > 0 && M <= 100) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                A[i][j] = j;
            }

            for (int k = 0; k < M; k++) {
                B[i][k] = A[i][k + 1] + 1;
            }
        }
    }
}

/*
 * ============================================================
 * TEST 20: loop nest con offset sicuro verso il passato
 * Gli accessi leggono valori prodotti da iterazioni precedenti.
 * Atteso: FONDIBILE oppure safe, se il fallback lo riconosce
 * ============================================================
 */
void nest_safe_offset_backward(int N) {
    int A[101][101];
    int B[100][100];

    if (N > 1 && N <= 100) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i + 1][j + 1] = i + j;
            }
        }

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                B[i][j] = A[i][j] + 1;
            }
        }
    }
}

/*
 * ============================================================
 * TEST 21: loop nest con due guard separati
 * Stessa condizione, ma branch separati.
 * Atteso: NON FONDIBILE per CFE
 * ============================================================
 */
void nest_two_guards_same_condition(int N) {
    int A[100][100];
    int B[100][100];

    if (N > 0 && N <= 100) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = i + j;
            }
        }
    }

    if (N > 0 && N <= 100) {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                B[i][j] = j + 1;
            }
        }
    }
}

/*
 * ============================================================
 * TEST 22: loop nest decrescente fondibile
 * Entrambi i nest usano i-- e j--.
 * Atteso: FONDIBILE, se il passo supporta step -1 anche nei nest
 * ============================================================
 */
void nest_decreasing_fusible(int N) {
    int A[101][101];
    int B[101][101];

    if (N > 0 && N <= 100) {
        for (int i = N; i > 0; i--) {
            for (int j = N; j > 0; j--) {
                A[i][j] = i + j;
            }
        }

        for (int i = N; i > 0; i--) {
            for (int j = N; j > 0; j--) {
                B[i][j] = A[i][j] + 1;
            }
        }
    }
}

/*
 * ============================================================
 * TEST 23: loop nest decrescente con dipendenza negativa
 * L1 legge A[i][j - 1].
 * Con j-- è verso una futura iterazione interna.
 * Atteso: NON FONDIBILE
 * ============================================================
 */
void nest_decreasing_negative_inner(int N) {
    int A[101][101];
    int B[101][101];

    if (N > 1 && N <= 100) {
        for (int i = N; i > 0; i--) {
            for (int j = N; j > 0; j--) {
                A[i][j] = i + j;
            }
        }

        for (int i = N; i > 0; i--) {
            for (int j = N; j > 0; j--) {
                B[i][j] = A[i][j - 1] + 1;
            }
        }
    }
}

/*
 * ============================================================
 * TEST 24: tre loop consecutivi fondibili
 * Serve a vedere se, rilanciando il pass, fonde più coppie.
 * Atteso: più fusion successive
 * ============================================================
 */
void three_consecutive_fusible(void) {
    int A[10];
    int B[10];
    int C[10];

    for (int i = 0; i < 10; i++) {
        A[i] = i;
    }

    for (int j = 0; j < 10; j++) {
        B[j] = j + 1;
    }

    for (int k = 0; k < 10; k++) {
        C[k] = k + 2;
    }
}