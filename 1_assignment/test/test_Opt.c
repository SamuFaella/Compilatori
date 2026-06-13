int main() {
  int x = 42;
  int y = x + 0;
  int z = 1 * x;
  int a = x * 16;
  int e = x * 15;
  int b = x / 8;
  int c = x + 1;
  int d = c - 1;
  return y + z + a + b + d + e;
}

// Controllo per il funzionamento della multi-instruction optimization
int example(int b) {
    int a = b + 5;
    int c = a - 5;
    return c;
}

// test per la stregth reduction
int test_strength(int x) {
    int a = x * 16;
    int b = x * 15;
    int c = x / 8;
    return a + b + c;
}