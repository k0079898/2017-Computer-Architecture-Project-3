#ifndef processor_h
#define processor_h

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "cmp.h"
#include "memory.h"
#include "regfile.h"

char iName[5];
int cycles, halt;
unsigned int instruction, opcode, rs, rt, rd, shamt, funct, immediate, address;
int writeToRegZero, numberOverflow, overwriteHILO, memAddOverflow, dataMisaligned;
int need_mfHILO;  //For the HI & LO overwrite detection

void doInstruction();  //If detected halt, return 1
void decode();         //Decode the instruction, if detected halt, return 1
void rType(char *command);  //Deal with R-Type instruction, if detected halt, return 1
void iType(char *command);  //Deal with I-Type instruction, if detected halt, return 1
void jType(char *command);  //Deal with J-type instruction, if detected halt, return 1
void NumberOverflowDetection(int in1, int in2, int out);  //Detect number overflow
void MemAddOverflowDetection(int addr, int size);         //Detect memory address overflow
void DataMisalignedDetection(int words, int size);        //Detect data misaligned

#endif
