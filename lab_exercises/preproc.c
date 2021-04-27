#include <stdio.h>
#include <inttypes.h>

// local macros (preprocess directives)
#define W 32
#define WORD uint32_t
#define PF PRIX32
#define BYTE uint8_t

// SHA256 works on blocks of 512 bits
union Block {
    // 8 x 64 = 512 (block of Bytes)
    BYTE bytes[64];
    // 32 x 16 = 512 (block of words)
    WORD words[16];
    // 64 x 8 = 512 (last 64 bits of last block)
    uint64_t sixf[8];
};

// keeping track of input message/padding
enum Status{
    READ, PAD, END
};

// returns 1 if it created a new block - origional message or padding
// returns 0 if all padded message has already been consumed
int next_block(FILE *f, union Block *B, enum Status *S, uint64_t *numBits){
    
    // number of bytes read
    size_t numBytes;

    // stop if status is END
    if (*S == END) {
        return 0;

    } else if (*S == READ) {
        // try to read another 64 bytes from file
        numBytes = fread(B->bytes, 1, 64, f);

        // update number of bits read
        *numBits = *numBits + (8 * numBytes);

        if(numBytes == 64){
            return 1;

        } else if(numBytes < 56) {
            // enough room for padding 64 bytes -> 55 bytes
            // append a 1 bit (and seven 0 bits to make a full byte)
            B->bytes[numBytes] = 0x80; // in bits : 10000000
            for (numBytes++; numBytes < 56; numBytes++) {
                // append enough 0 bits, leaving 64 at the end
                B->bytes[numBytes] = 0x00; // in bits : 00000000
            }

            // append length of original input
            // check ENDIANESS
            B->sixf[7] = *numBits;

            // set status as the last block
            *S = END;
            
        } else {
            // got to end of input message
            // not enough room in this block for padding
            // append a 1 bit (and seven 0 bits to make a full byte)
            B->bytes[numBytes] = 0x80; // in bits : 10000000
            // append 0 bits
            for (numBytes++; numBytes < 64; numBytes++) {
                // append enough 0 bits, leaving 64 at the end
                B->bytes[numBytes] = 0x00; // in bits : 00000000
            }

            // set status to PAD
            *S = PAD;
        }
    } else if (*S == PAD) {
        // append 0 bits
        for (numBytes = 0; numBytes < 56; numBytes++) {
            // append enough 0 bits, leaving 64 at the end
            B->bytes[numBytes] = 0x00; // in bits : 0000000
        }

        // append numBits as an integer
        // check ENDIAN
        B->sixf[7] = *numBits;

        // set status to END
        *S = END;
    }
    
    return 1;
};

int main(int argc, char *argv[]){

    // iterator
    int i;

    // The current block
    union Block B;

    // total number of bits read
    uint64_t numBits = 0;

    // current status of reading input
    enum Status S = READ;

    // file pointer
    FILE *f;

    // NB - do error checking on file opening
    // TO-DO.......

    // open file from command line for reading
    f = fopen(argv[1], "r");

    // keep reading preprocessed blocks until empty
    while(next_block(f, &B, &S, &numBits))
    {
        // print the 16 32-bit words to the console
        for(i = 0; i < 16; i++){
            printf("%08" PF " ", B.words[i]);
        } 
        printf("\n");
    }

    // close file
    fclose(f);

    printf("Total bits read: %lu. \n", numBits);
    
    return 0;
}