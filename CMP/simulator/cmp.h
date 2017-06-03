#ifndef cmp_h
#define cmp_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "memory.h"
#include "processor.h"

FILE *report, *trace;

//Cache
typedef struct _variable {
    int memSize;
    int pageSize;
    int cacheSize;
    int blockSize;
    int set;
} variable;

//PTE
typedef struct _PTE {
    int ValidBit;
    unsigned int PhysicalPageNumber;
} PTE;

//TLB
typedef struct _TLB {
    int ValidBit;
    unsigned int VirtualPageNumber;
    unsigned int PhysicalPageNumber;
    int Cycle;
} TLB;

typedef struct _Cache {
    int ValidBit;
  	unsigned int Tag;
  	int MRU;
} Cache;

typedef struct _Memory {
  	int ValidBit;
    int Cycle;
} Memory;

variable I_var, D_var;
PTE *iPTE, *dPTE;
TLB *iTLB, *dTLB;
Cache **iCache, **dCache;
Memory *iMem, *dMem;
unsigned int iPTE_entries, dPTE_entries;
int iPTE_HIT, iPTE_MISS, dPTE_HIT, dPTE_MISS;
unsigned int iTLB_entries, dTLB_entries;
int iTLB_HIT, iTLB_MISS, dTLB_HIT, dTLB_MISS;
unsigned int iCache_entries, dCache_entries;
int iCache_HIT, iCache_MISS, dCache_HIT, dCache_MISS;
unsigned int iMem_entries, dMem_entries;

void initCMP();
void initPTE();
void initTLB();
void initCache();
void initMem();
void checkITLB(unsigned int Vaddr);  //I
void updateILTB(unsigned int VPN);
int checkIPTE(unsigned int VPN);
void updateIPTE(unsigned int VPN);
void checkICache(unsigned int PhysicalAddr);
void checkDTLB(unsigned int Vaddr); //D
void checkDTLB(unsigned int Vaddr);
void updateDLTB(unsigned int VPN);
int checkDPTE(unsigned int VPN);
void updateDPTE(unsigned int VPN);
void checkDCache(unsigned int PhysicalAddr);
void writeReport();

#endif
