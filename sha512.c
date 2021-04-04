#include <stdio.h>
#include <inttypes.h>
#include <byteswap.h>

// Endianess RE: https://developer.ibm.com/technologies/systems/articles/au-endianc/
const int _i = 1;
#define is_lilend() ((*(char*)&_i) != 0)

// local macros (preprocess directives)
#define WLEN 32
#define WORD uint32_t
#define PF PRIx32
#define BYTE uint8_t

// NB - lookup methods for 512
/*
// Page 5 of secure hash standard
#define ROTL(x,n) ((x<<n)|(x>>(WLEN-n))) // rotate left
#define ROTR(x,n) ((x>>n)|(x<<(WLEN-n))) // rotate right
#define SHR(x,n) (x>>n) // shift right

// Page 10 of secure hash standard
#define CH(x,y,z) ((x&y)^(~x&z)) // choose bits
#define MAJ(x,y,z) ((x&y)^(x&z)^(y&z)) // get majority vote

#define SIG0(x) (ROTR(x,2)^ROTR(x,13)^ROTR(x,22)) // scramble 
#define SIG1(x) (ROTR(x,6)^ROTR(x,11)^ROTR(x,25)) // scramble
#define Sig0(x) (ROTR(x,7)^ROTR(x,18)^SHR(x,3)) // scramble 
#define Sig1(x) (ROTR(x,17)^ROTR(x,19)^SHR(x,10)) // scramble 

*/

// SHA512 works on blocks of 1024 bits
union Block {
    // 8 x 128 = 1024 (block of Bytes)
    BYTE bytes[128];
    // 32 x 32 = 1024 (block of words)
    WORD words[32];
    // 64 x 16 = 1024 (last 128 bits of last block)
    uint64_t lblock[16];
};

// keeping track of input message/padding
enum Status{
    READ, PAD, END
};

// NB - lookup constants for 512
/*
// section 4.2.2
const WORD K[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};
*/

// returns 1 if it created a new block - origional message or padding
// returns 0 if all padded message has already been consumed
int next_block(FILE *f, union Block *M, enum Status *S, uint64_t *numBits){
    
    // number of bytes read
    size_t numBytes;

    // stop if status is END
    if (*S == END) {
        //finish
        return 0;

    } else if (*S == READ) {
        // try to read another 128 bytes from file
        numBytes = fread(M->bytes, 1, 128, f);

        // update number of bits read
        *numBits = *numBits + (8 * numBytes);

        if(numBytes == 128){
            //do nothing
        } else if(numBytes < 112) {
            // enough room for padding 64 bytes -> 55 bytes
            // append a 1 bit (and seven 0 bits to make a full byte)
            M->bytes[numBytes] = 0x80; // in bits : 10000000
            for (numBytes++; numBytes < 112; numBytes++) {
                // append enough 0 bits, leaving 64 at the end
                M->bytes[numBytes] = 0x00; // in bits : 00000000
            }

            // append length of original input
            // check ENDIANESS
            M->lblock[15] = (is_lilend() ? bswap_64(*numBits) : *numBits);

            // set status as the last block
            *S = END;
            
        } else {
            // got to end of input message
            // not enough room in this block for padding
            // append a 1 bit (and seven 0 bits to make a full byte)
            M->bytes[numBytes] = 0x80; // in bits : 10000000
            // append 0 bits
            for (numBytes++; numBytes < 112; numBytes++) {
                // append enough 0 bits, leaving 64 at the end
                M->bytes[numBytes] = 0x00; // in bits : 00000000
            }

            // set status to PAD
            *S = PAD;
        }
    } else if (*S == PAD) {
        // append 0 bits
        for (numBytes = 0; numBytes < 112; numBytes++) {
            // append enough 0 bits, leaving 64 at the end
            M->bytes[numBytes] = 0x00; // in bits : 0000000
        }

        // append numBits as a big endian integer
        // check ENDIAN
        M->lblock[15] = (is_lilend() ? bswap_64(*numBits) : *numBits);

        // set status to END
        *S = END;
    }
    
    // if little Endian, reverse the byte order of words
    if(is_lilend()){
        for(int i = 0; i < 16; i++){
            M->words[i] = bswap_32(M->words[i]);
        }
    }
    return 1;
};