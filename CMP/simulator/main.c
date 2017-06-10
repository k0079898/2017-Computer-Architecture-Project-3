#include <stdio.h>
#include "cmp.h"
#include "memory.h"
#include "regfile.h"
#include "processor.h"

FILE *snapshot;
void writeSnapshot(unsigned int cycles);

int main(int argc , char *argv[])
{
  	//Initial the simlator
    if(argc == 1)
    {
        I_var.memSize = 64;
        D_var.memSize = 32;
        I_var.pageSize = 8;
        D_var.pageSize = 16;
        I_var.cacheSize = 16;
        I_var.blockSize = 4;
        I_var.set = 4;
        D_var.cacheSize = 16;
        D_var.blockSize = 4;
        D_var.set = 1;
    }else if(argc == 11) //2. 256 256 32 32 16 4 4 16 4 4, 3.512 1024 128 64 64 4 8 32 4 4
    {
        I_var.memSize = atoi(argv[1]);
        D_var.memSize = atoi(argv[2]);
        I_var.pageSize = atoi(argv[3]);
        D_var.pageSize = atoi(argv[4]);
        I_var.cacheSize = atoi(argv[5]);
        I_var.blockSize = atoi(argv[6]);
        I_var.set = atoi(argv[7]);
        D_var.cacheSize = atoi(argv[8]);
        D_var.blockSize = atoi(argv[9]);
        D_var.set = atoi(argv[10]);
    }else
    {
        printf("Error config value!!");
        exit(1);
    }
  	snapshot = fopen("snapshot.rpt", "w");
    initREG();
  	readBin();
  	writeDisk();
  	cycles = 0;
  	halt = 0;
    initCMP();
    //memDebug();
  	writeSnapshot(cycles);
  	//Start the simulation
  	while(halt!=1)
  	{
    		cycles ++;
        //printf("Cycle %d:\n", cycles);
    		writeToRegZero = 0;
    		numberOverflow = 0;
    		overwriteHILO = 0;
    		memAddOverflow = 0;
    		dataMisaligned = 0;
    		doInstruction();
    		if(halt!=1) writeSnapshot(cycles);
        //printf("\n");
        fprintf( trace, "\n");
  	}
  	fclose(snapshot);
    writeReport();
  	return 0;
}

void writeSnapshot(unsigned int cycles)
{
    int i;
    fprintf(snapshot, "cycle %d\n", cycles);
    //printf("cycle %d\n", cycles);
    if(cycles==0)  //Initial Print
    {
        for(i=0; i<REG_SIZE; i++)
        {
            fprintf(snapshot, "$%02d: ", i);
            fprintf(snapshot, "0x%08X\n", REG[i]);
            //printf("$%02d: ", i);
            //printf("0x%08X\n", REG[i]);
            L_REG[i] = REG[i];
        }
        fprintf(snapshot, "$HI: 0x%08X\n", HI);
        //printf("$HI: 0x%08X\n", HI);
        L_HI = HI;
        fprintf(snapshot, "$LO: 0x%08X\n", LO);
        //printf("$LO: 0x%08X\n", LO);
        L_LO = LO;
        fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
        //printf("PC: 0x%08X\n\n\n", PC);
        L_PC = PC;
    }else //Simulaing Print
    {
        for(i=0; i<REG_SIZE; i++)
        {
          if(L_REG[i] != REG[i])
          {
              fprintf(snapshot, "$%02d: ", i);
              fprintf(snapshot, "0x%08X\n", REG[i]);
              //printf("$%02d: ", i);
              //printf("0x%08X\n", REG[i]);
              L_REG[i] = REG[i];
          }
        }
        if(L_HI != HI)
        {
            fprintf(snapshot, "$HI: 0x%08X\n", HI);
            //printf("$HI: 0x%08X\n", HI);
            L_HI = HI;
        }
        if(L_LO != LO)
        {
            fprintf(snapshot, "$LO: 0x%08X\n", LO);
            //printf("$LO: 0x%08X\n", LO);
            L_LO = LO;
        }
        if(L_PC != PC)
        {
            fprintf(snapshot, "PC: 0x%08X\n\n\n", PC);
            //printf("PC: 0x%08X\n\n\n", PC);
            L_PC = PC;
        }
    }
}
