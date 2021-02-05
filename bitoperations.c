#include <stdio.h>

// function for printing bits
void bin_print(int x) {

    // Number of bits in an integer.
    int y = sizeof(int) * 8;

    // temp variable.
    int z;

    for (y-- ; y >= 0; y--) {
        // print bits
        z = ((1 << y) & x) ? 1 : 0;
        printf("%d", z);
    }
}

// main function
int main(int argc, char *argv[]) {
    
    // set int to 241 using hex value
    int i = 0xf1;

    // print starting value
    printf("Original:  ");
    bin_print(i);
    printf("\n");

    // shift bits to the left 40 times
    for (int j = 0; j < 40; j++) {
        printf("%3d << %2d: ", i, j);
        // call bin_print shifted to the left by j
        bin_print(i << j);
        printf("\n");
    }

    return 0;
}