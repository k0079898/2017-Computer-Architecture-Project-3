#ifndef memory_h
#define memory_h

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "regfile.h"

#define DISK_SIZE 1024

FILE *iimage, *dimage;
unsigned char *iBuffer, *dBuffer;

//Disk
unsigned char iDisk[DISK_SIZE];
unsigned char dDisk[DISK_SIZE];

void readBin();   //Read iimage.bin & dimage.bin into buffer
void writeDisk();  //Write data into memory & free the buffer
void memDebug();  //Memory debug function


#endif
