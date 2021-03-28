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
        // try to read another 64 bytes from file
        numBytes = fread(M->bytes, 1, 64, f);

        // update number of bits read
        *numBits = *numBits + (8 * numBytes);

        if(numBytes == 64){
            //do nothing
        } else if(numBytes < 56) {
            // enough room for padding 64 bytes -> 55 bytes
            // append a 1 bit (and seven 0 bits to make a full byte)
            M->bytes[numBytes] = 0x80; // in bits : 10000000
            for (numBytes++; numBytes < 56; numBytes++) {
                // append enough 0 bits, leaving 64 at the end
                M->bytes[numBytes] = 0x00; // in bits : 00000000
            }

            // append length of original input
            // check ENDIANESS
            M->sixf[7] = (is_lilend() ? bswap_64(*numBits) : *numBits);

            // set status as the last block
            *S = END;
            
        } else {
            // got to end of input message
            // not enough room in this block for padding
            // append a 1 bit (and seven 0 bits to make a full byte)
            M->bytes[numBytes] = 0x80; // in bits : 10000000
            // append 0 bits
            for (numBytes++; numBytes < 64; numBytes++) {
                // append enough 0 bits, leaving 64 at the end
                M->bytes[numBytes] = 0x00; // in bits : 00000000
            }

            // set status to PAD
            *S = PAD;
        }
    } else if (*S == PAD) {
        // append 0 bits
        for (numBytes = 0; numBytes < 56; numBytes++) {
            // append enough 0 bits, leaving 64 at the end
            M->bytes[numBytes] = 0x00; // in bits : 0000000
        }

        // append numBits as a big endian integer
        // check ENDIAN
        M->sixf[7] = (is_lilend() ? bswap_64(*numBits) : *numBits);

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

int next_hash(union Block *M, WORD H[]){
    // message schedule
    WORD W[64];

    // iterator for W
    int t;

    // temp variables
    WORD a, b, c, d, e ,f, g, h, T1, T2;

    // section 6.2.2 (part1)
    // values 1 to 16
    for (t = 0; t < 16; t++){
        W[t] = M->words[t];
    }

    // values 16 to 64
    for (t = 16; t < 64; t++){
        W[t] = Sig1(W[t-2]) + W[t-7] + Sig0(W[t-15]) + W[t-16];
    }

    // section 6.2.2 (part2)
    a = H[0];
    b = H[1];
    c = H[2];
    d = H[3];
    e = H[4];
    f = H[5];
    g = H[6];
    h = H[7];

    // section 6.2.2 (part3)
    for (t = 0; t < 64; t++){
        T1 = h + SIG1(e) + CH(e, f, g) + K[t] + W[t];
        T2 = SIG0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }

    // section 6.2.2 (part4)
    H[0] = a + H[0];
    H[1] = b + H[1];
    H[2] = c + H[2];
    H[3] = d + H[3];
    H[4] = e + H[4];
    H[5] = f + H[5];
    H[6] = g + H[6];
    H[7] = h + H[7];
}

int sha256(FILE *f, WORD H[]){
    
    // The current block
    union Block M;

    // total number of bits read
    uint64_t numBits = 0;

    // current status of reading input
    enum Status S = READ;

    // function that performs the SHA256 algorithm on the message f
    // keep reading preprocessed blocks until empty
    while(next_block(f, &M, &S, &numBits)){
       next_hash(&M, H);
    }

    return 0;
}

int main(int argc, char *argv[]){
    // section 5.3.4
    WORD H[] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };


    // file pointer
    FILE *f;

    // TO-DO.......
    // NB - do error checking on file opening
    // open file from command line for reading

    // read file
    f = fopen(argv[1], "r");

    // calculate the sha256 of FILE f
    sha256(f, H);

    // print final SHA256 hash
    for (int i = 0; i < 8; i++){
        printf("%08" PF, H[i]);
    }
    printf("\n");

    // close file
    fclose(f);
    
    return 0;
}