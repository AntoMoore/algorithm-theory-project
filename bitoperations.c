#include <stdio.h>

// function for printing bits
void bin_print(int x) {

    // Number of bits in an integer.
    int y = sizeof(int) * 8;

    // temp variable.
    int z;

    // print bits with loop
    for (y-- ; y >= 0; y--) {
        // ternary operator 
        // (expression) ? true : false
        // pick out the y^th bit of x
        z = ((1 << y) & x) ? 1 : 0;
        printf("%d", z);
    }
}

// main function
int main(int argc, char *argv[]) {
    
    // set int using hex value
    unsigned int i = 0x0f0f0f0f;

    // print starting value
    printf("Original:\t");
    // print i in binary
    bin_print(i);
    // End line
    printf("\t%x\t%u\n\n",i ,i);

    // set j to 32 bits
    int j = sizeof(unsigned int) * 8;

    // shift bits to the left
    for (j--; j >= 0; j--) {
        // 1 shifted to the left by j 
        bin_print(1 << j);
        // newline
        printf("\n");

        // call bin_print for i
        bin_print(i);
        // newline
        printf("\n");
        printf("-------------------------------- & \n");

        // 1 shifted to the left j times (bitwise locial and i)
        bin_print((1 << j) & i);
        printf("\n\n");
   
    }

    // everything is okay
    return 0;
}