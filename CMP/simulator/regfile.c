#include "regfile.h"

void initREG() //Initial the register
{
    for(int i=0; i<REG_SIZE; i++) REG[i] = 0x00000000;
    PC = 0x00000000;
    HI = 0x00000000;
    LO = 0x00000000;
}
