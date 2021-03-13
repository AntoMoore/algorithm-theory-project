#include <stdio.h>
#include <inttypes.h>

// local macros
#define W 32
#define WORD uint32_t
#define PF PRIX32
#define BYTE uint8_t

union Block {
    BYTE bytes[64];
    WORD words[16];
};

int main(int argc, char *argv[]){
    // The current block
    union Block B;

    // total number of bits read
    uint64_t numBits = 0;

    // file pointer
    FILE *f;

    // NB - do error checking on file opening
    // TO-DO.......

    // open file from command line for reading
    f = fopen(argv[1], "r");

    // try to read 64 bytes from file
    size_t numBytes;
    numBytes = fread(&B.bytes, 1, 64, f);
    printf("Read %lu bytes. \n", numBytes);

    // update number of bits read
    numBits += (8 * numBytes);

    // print the 16 32-bit words to the console
    for(int i = 0; i < 16; i++){
        printf("%08" PF " ", B.words[i]);
        if((i + 1) % 8 == 0){
            printf("\n");
        }
    }   

    // keep reading until file is empyty
    while (!feof(f))
    {
        // try to read another 64 bytes from file
        numBytes = fread(&B.bytes, 1, 64, f);
        printf("Read %lu bytes. \n", numBytes);

        // update number of bits read
        numBits += (8 * numBytes);

        // print the 16 32-bit words to the console
        for(int i = 0; i < 16; i++){
            printf("%08" PF " ", B.words[i]);
            if((i + 1) % 8 == 0){
                printf("\n");
            }
        } 
    }

    // close file
    fclose(f);

    printf("Total bits read: %lu. \n", numBits);
    
    return 0;
}