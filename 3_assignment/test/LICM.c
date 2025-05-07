double calculate_sum_optimized(int n, double a) {
    double sum = 0.0;
    a = 2;

    for (int i = 0; i < n; i++) {
        double invariant_value = a*2;
        sum += invariant_value * i;  // Usa il valore precalcolato
    }

    return sum;
}

