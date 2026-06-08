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

void fusible_symbolic_start(int N, int M) {
    int A[100];
    int B[100];
    if (N < M) {
        for (int i = N; i < M; i++) {
            A[i] = i;
        }

        for (int j = N; j < M; j++) {
            B[j] = A[j] + 1;
        }
    }
}

void negative_decreasing(int N) {
    int A[11];
    int B[11];

    if (N > 10) {
        for (int i = 10; i > 0; i--) {
            A[i] = i;
        }

        for (int j = 10; j > 0; j--) {
            B[j] = A[j - 1] + 1;
        }
    }
}