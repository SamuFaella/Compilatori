#include <iostream>

extern "C" {

// Funzione IN: legge un intero da standard input
int IN(int verbose) {
    int value;
    if (verbose)
        std::cout << "Inserisci un numero: ";
    std::cin >> value;
    return value;
}

// Funzione OUT: stampa un intero su standard output
void OUT(int x) {
    std::cout << x << std::endl;
}

}
