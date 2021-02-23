#include <stdio.h>
#include <inttypes.h>

// local macros
#define WORD uint32_t
#define PF PRIX32

WORD Ch(WORD x, WORD y, WORD z){
    // use bit masking
    // if is set (eg. x == F) take value from y
    // if X is unset (eg. x == 0) take value from z
    return (x & y) ^ (~x & z);
}

int main(int argc, char *argv[]){

    WORD x = 0xF0F0F0F0;
    WORD y = 0x0A0A0A0A;
    WORD z = 0x0B0B0B0B;

    WORD ans = Ch(x, y, z);

    printf("Ch(%08" PF ",%08" PF ",%08" PF ")=%" PF "\n", x, y, z, ans);

    return 0;
}