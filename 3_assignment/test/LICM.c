double complex_test(int n, double a, double b, double *arr) {
    double sum = 0.0;
    double result = 1.0;

    for (int i = 0; i < n; i++) {
        double inv1 = a * 2.0;          // loop-invariant, spostabile
        double inv2 = b + 3.0;          // loop-invariant, spostabile
        double inv3 = inv1 + inv2;      // loop-invariant, spostabile solo dopo inv1 e inv2

        double not_inv1 = i * 4.0;      // NON invariant, dipende da i
        double not_inv2 = inv3 * i;     // NON invariant, dipende da i

        arr[i] = not_inv2;              // NON spostabile, scrive memoria
        sum += arr[i] + inv3;           // NON spostabile, legge memoria e dipende dal loop

        result = inv3;                  // attenzione: assegnazione nel loop
    }

    return sum + result;
}