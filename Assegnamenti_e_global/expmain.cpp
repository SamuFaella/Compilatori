#include <iostream>
#include <stdint.h>

extern "C" {
    int32_t cout(int x);
}

int cout(int32_t v) {
    std::cout << v << std::endl;
    return 0;
}


