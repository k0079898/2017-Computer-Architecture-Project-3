#include "cmp.h"

void initCMP()
{
    trace = fopen("trace.rpt","w");
    initPTE();
    initTLB();
    initCache();
    initMem();
}

void initPTE()
{
    //i-PTE
    iPTE_entries = DISK_SIZE / I_var.pageSize;
    iPTE = (PTE*)malloc(iPTE_entries * sizeof(PTE));
    for(int m=0 ; m<iPTE_entries ; m++)
    {
        iPTE[m].ValidBit = 0;
        iPTE[m].PhysicalPageNumber = 0;
    }
    iPTE_HIT = 0;
    iPTE_MISS = 0;
    //d-PTE
    dPTE_entries = DISK_SIZE / D_var.pageSize;
    dPTE = (PTE*)malloc(dPTE_entries * sizeof(PTE));
    for(int n=0 ; n<dPTE_entries ; n++)
    {
        dPTE[n].ValidBit = 0;
        dPTE[n].PhysicalPageNumber = 0;
    }
    dPTE_HIT = 0;
    dPTE_MISS = 0;
}

void initTLB()
{
    //i-TLB
    iTLB_entries = iPTE_entries / 4;
  	iTLB = (TLB*)malloc(iTLB_entries * sizeof(TLB));
  	for(int m=0 ; m<iTLB_entries ; m++)
  	{
    		iTLB[m].ValidBit = 0;
    		iTLB[m].VirtualPageNumber = 0;
        iTLB[m].PhysicalPageNumber = 0;
    		iTLB[m].Cycle = 0;
  	}
  	iTLB_HIT = 0;
  	iTLB_MISS = 0;
    //d-TLB
    dTLB_entries = dPTE_entries / 4;
    dTLB = (TLB*)malloc(dTLB_entries * sizeof(TLB));
    for(int n=0 ; n<dTLB_entries ; n++)
    {
        dTLB[n].ValidBit = 0;
        dTLB[n].VirtualPageNumber = 0;
        dTLB[n].PhysicalPageNumber = 0;
        dTLB[n].Cycle = 0;
    }
    dTLB_HIT = 0;
    dTLB_MISS = 0;
}

void initCache()
{
    int i, j;
    //i-Cache
    iCache_entries = I_var.cacheSize / I_var.blockSize / I_var.set;
  	iCache = (Cache**)malloc(iCache_entries * sizeof(Cache*));
  	for(i=0 ; i<iCache_entries ; i++)
    {
  		  iCache[i] = (Cache*)malloc(I_var.set * sizeof(Cache));
  	}
  	for(i=0 ; i<iCache_entries ; i++)
    {
    		for(j=0 ; j<I_var.set ; j++)
        {
      			iCache[i][j].ValidBit = 0;
      			iCache[i][j].Tag = 0;
      			iCache[i][j].MRU = 0;
    		}
  	}
  	iCache_HIT = 0;
  	iCache_MISS = 0;
    //d-Cache
    dCache_entries = D_var.cacheSize / D_var.blockSize / D_var.set;
    dCache = (Cache**)malloc(dCache_entries * sizeof(Cache*));
    for(i=0 ; i<dCache_entries ; i++)
    {
        dCache[i] = (Cache*)malloc(D_var.set * sizeof(Cache));
    }
    for(i=0 ; i<dCache_entries ; i++)
    {
        for(j=0 ; j<D_var.set ; j++)
        {
            dCache[i][j].ValidBit = 0;
            dCache[i][j].Tag = 0;
            dCache[i][j].MRU = 0;
        }
    }
    dCache_HIT = 0;
    dCache_MISS = 0;
}

void initMem()
{
    //i-Mem
    iMem_entries = I_var.memSize / I_var.pageSize;
    iMem = (Memory*)malloc(iMem_entries * sizeof(Memory));
    for(int m=0 ; m<iMem_entries ; m++)
    {
    		iMem[m].ValidBit = 0;
    		iMem[m].Cycle = 0;
	  }
    //d-Mem
    dMem_entries = D_var.memSize / D_var.pageSize;
    dMem = (Memory*)malloc(dMem_entries * sizeof(Memory));
    for(int n=0 ; n<dMem_entries ; n++)
    {
        dMem[n].ValidBit = 0;
        dMem[n].Cycle = 0;
    }
}

void checkITLB(unsigned int Vaddr)
{
    fprintf( trace, "\n\nCycle %d:\n", cycles);
    int find = 0, m;
    unsigned int VPN, PPN, offset, PhysicalAddr;
    VPN = Vaddr / I_var.pageSize;
    offset = Vaddr % I_var.pageSize;
    for(m=0 ; m<iTLB_entries ; m++)  //i-TLB HIT
    {
        if( (iTLB[m].VirtualPageNumber == VPN) && (iTLB[m].ValidBit == 1) && (iTLB[m].Cycle>0) )
        {
            find = 1;
            PPN = iTLB[m].PhysicalPageNumber;
            iTLB[m].Cycle = cycles;
            iTLB_HIT++;
            printf("ITLB HIT\n");
            fprintf( trace, "ITLB ");
            PhysicalAddr = (PPN * I_var.pageSize) + offset;  //Move on to check i-Cache
            checkICache(PhysicalAddr);
            break;
        }
    }
    if(find == 0)  //i-TLB MISS
    {
        iTLB_MISS++;
        //printf("ITLB MISS\n");
        int IPTEisHIT = checkIPTE(VPN);  //Move on to check i-PTE
        if(IPTEisHIT == 1)  //i-PTE HIT, update i-TLB & check i-Cache
        {
            updateILTB(VPN);  //update i-LTB
            for(m=0 ; m<iTLB_entries ; m++)    //Move on to check i-Cache
            {
                if( (iTLB[m].VirtualPageNumber == VPN) && (iTLB[m].ValidBit == 1) && (iTLB[m].Cycle>0) )
                {
                    PPN = iTLB[m].PhysicalPageNumber;
                    iTLB[m].Cycle = cycles;
                    PhysicalAddr = (PPN * I_var.pageSize) + offset;
                    checkICache(PhysicalAddr);
                    break;
                }
            }
        }else  //i-PTE MISS, update i-PTE & i-TLB & i-Cache
        {
            updateIPTE(VPN);  //update i-PTE
            printf("IDisk HIT\n");
            fprintf( trace, "iDisk");
            updateILTB(VPN);  //update i-LTB
            for(m=0 ; m<iTLB_entries ; m++)  //i-TLB HIT
            {
                if( (iTLB[m].VirtualPageNumber == VPN) && (iTLB[m].ValidBit == 1) && (iTLB[m].Cycle>0) )
                {
                    PPN = iTLB[m].PhysicalPageNumber;
                    iTLB[m].Cycle = cycles;
                    PhysicalAddr = (PPN * I_var.pageSize) + offset;  //Move on to check i-Cache
                    checkICache(PhysicalAddr);
                    break;
                }
            }
        }
    }
}

void updateILTB(unsigned int VPN)
{
    unsigned int PPN;
    int index, minC = 555555;
    for(int m=0 ; m<iTLB_entries ; m++)
    {
        if(iTLB[m].ValidBit == 0)
        {
            index = m;
            break;
        }else
        {
            if(iTLB[m].Cycle < minC)
            {
                minC = iTLB[m].Cycle;
                index = m;
            }
        }
    }
    PPN = iPTE[VPN].PhysicalPageNumber;

    iTLB[index].ValidBit = 1;
    iTLB[index].VirtualPageNumber = VPN;
    iTLB[index].PhysicalPageNumber = PPN;
    iTLB[index].Cycle = cycles;
}

int checkIPTE(unsigned int VPN)
{
    if(iPTE[VPN].ValidBit == 1)  //i-PTE HIT
    {
        iPTE_HIT++;
        printf("IPTE HIT\n");
        return 1;
    }else  //i-PTE MISS
    {
        iPTE_MISS++;
        //printf("IPTE MISS\n");
        return 0;
    }
}

void updateIPTE(unsigned int VPN)
{
    unsigned int PPN;
    int min = 555555, i, j;
    int Case = 0;
    for(i = 0 ; i<iMem_entries ; i++)
    {
  		  if(iMem[i].ValidBit == 0)
        {
            Case = 1;
  			    PPN = i;
  			    break;
		    }else
        {
            if (iMem[i].Cycle < min)
            {
              PPN = i;
              min = iMem[i].Cycle;
            }
        }
		}
    iMem[PPN].ValidBit = 1;
	  iMem[PPN].Cycle = cycles;
    if(Case == 1)
    {
        iPTE[VPN].ValidBit = 1;
        iPTE[VPN].PhysicalPageNumber = PPN;
    }else
    {
        for(i=0 ; i<iPTE_entries ; i++)
        {
            if( iPTE[i].PhysicalPageNumber == PPN )
            {
                iPTE[i].ValidBit = 0;
                iPTE[i].PhysicalPageNumber = 0;
            }
        }
        iPTE[VPN].ValidBit = 1;
        iPTE[VPN].PhysicalPageNumber = PPN;
        for(i=0 ; i<iTLB_entries ; i++)
        {
            if( iTLB[i].PhysicalPageNumber == PPN )
            {
                iTLB[i].ValidBit = 0;
                iTLB[i].VirtualPageNumber = 0;
                iTLB[i].PhysicalPageNumber = 0;
                iTLB[i].Cycle = 0;
            }
        }
        unsigned int PhysicalAddr;
        int index, tag;
        for(i=0 ; i<I_var.pageSize ; i++)
        {
            PhysicalAddr = (PPN * I_var.pageSize) + i;
            index = (PhysicalAddr / I_var.blockSize) % iCache_entries;
            tag = (PhysicalAddr / I_var.blockSize) / iCache_entries;
            if(I_var.set == 1)
            {
                if(iCache[index][0].Tag == tag)
                {
                    iCache[index][0].ValidBit = 0;
                }
            }else
            {
                for(j=0 ; j<I_var.set ; j++)
                {
                    if( (iCache[index][j].Tag == tag) && (iCache[index][j].ValidBit == 1) )
                    {
                        iCache[index][j].ValidBit = 0;
                        iCache[index][j].MRU = 0;
                    }
                }
            }
        }
    }
}

void checkICache(unsigned int PhysicalAddr)
{

    unsigned int index = (PhysicalAddr / I_var.blockSize) % iCache_entries;
    unsigned int tag = (PhysicalAddr / I_var.blockSize) / iCache_entries;
    if(I_var.set == 1)
    {
        if( (iCache[index][0].Tag == tag) && (iCache[index][0].ValidBit == 1) )  //i-Cache HIT
        {
            iCache_HIT++;
            printf("ICACHE HIT\n");
            fprintf( trace, "ICache ");
        }else  //i-Cache MISS, update i-Cache
        {
            iCache_MISS++;
            //printf("ICACHE MISS\n");
            iCache[index][0].ValidBit = 1;
            iCache[index][0].Tag = tag;
            iCache[index][0].MRU = 0;
        }
    }else
    {
        int find = 0, i, j, k;
        int MRU_FULL = 1;
        for(i=0 ; i<I_var.set ; i++)
        {
            //printf("%d %d %d\n", iCache[index][i].Tag, tag, iCache[index][i].ValidBit);
            if( (iCache[index][i].Tag == tag) && (iCache[index][i].ValidBit == 1) )  //i-Cache HIT
            {
                find = 1;
                iCache_HIT++;
                printf("ICACHE HIT\n");
                iCache[index][i].MRU = 1;
                for(j=0 ; j<I_var.set ; j++)
                {
                    MRU_FULL = MRU_FULL & iCache[index][j].MRU;
                }
                if(MRU_FULL == 1)
                {
                    for(k=0 ; k<I_var.set ; k++)
                    {
                        if(k != i) iCache[index][k].MRU = 0;
                    }
                }
                break;
            }
        }
        if(find == 0)  //i-Cache MISS, update i-Cache
        {
            iCache_MISS++;
            //printf("ICACHE MISS\n");
            for(i=0 ; i<I_var.set ; i++)  //Find from valid bit
            {
                //printf("%d %d %d\n", iCache[index][i].Tag, tag, iCache[index][i].ValidBit);
                if(iCache[index][i].ValidBit == 0)
                {
                    iCache[index][i].ValidBit = 1;
                    iCache[index][i].Tag = tag;
                    iCache[index][i].MRU = 1;
                    for(j=0 ; j<I_var.set ; j++)
                    {
                        MRU_FULL = MRU_FULL & iCache[index][j].MRU;
                    }
                    if(MRU_FULL == 1)
                    {
                        for(k=0 ; k<I_var.set ; k++)
                        {
                            if(k != i) iCache[index][k].MRU = 0;
                        }
                    }
                    return;
                }
            }
            for(i=0 ; i<I_var.set ; i++)  //Find from MRU
            {
                //printf("%d %d %d\n", iCache[index][i].Tag, tag, iCache[index][i].MRU);
                if(iCache[index][i].MRU == 0)
                {
                    iCache[index][i].ValidBit = 1;
                    iCache[index][i].Tag = tag;
                    iCache[index][i].MRU = 1;
                    for(j=0 ; j<I_var.set ; j++)
                    {
                        MRU_FULL = MRU_FULL & iCache[index][j].MRU;
                    }
                    if(MRU_FULL == 1)
                    {
                        for(k=0 ; k<I_var.set ; k++)
                        {
                            if(k != i) iCache[index][k].MRU = 0;
                        }
                    }
                    return;
                }
            }
        }
    }
}

void checkDTLB(unsigned int Vaddr)
{
    fprintf( trace, "\n");
    int find = 0, m;
    unsigned int VPN, PPN, offset, PhysicalAddr;
    VPN = Vaddr / D_var.pageSize;
    offset = Vaddr % D_var.pageSize;
    for(m=0 ; m<dTLB_entries ; m++)  //d-TLB HIT
    {
        if( (dTLB[m].VirtualPageNumber == VPN) && (dTLB[m].ValidBit == 1) && (dTLB[m].Cycle>0) )
        {
            find = 1;
            PPN = dTLB[m].PhysicalPageNumber;
            dTLB[m].Cycle = cycles;
            dTLB_HIT++;
            printf("DTLB HIT\n");
            fprintf( trace, "DTLB ");
            PhysicalAddr = (PPN * D_var.pageSize) + offset;  //Move on to check d-Cache
            checkDCache(PhysicalAddr);
            break;
        }
    }
    if(find == 0)  //d-TLB MISS
    {
        dTLB_MISS++;
        //printf("DTLB MISS\n");
        int DPTEisHIT = checkDPTE(VPN);  //Move on to check d-PTE
        if(DPTEisHIT == 1)  //d-PTE HIT, update d-TLB & check d-Cache
        {
            updateDLTB(VPN);  //update d-LTB
            for(m=0 ; m<dTLB_entries ; m++)    //Move on to check d-Cache
            {
                if( (dTLB[m].VirtualPageNumber == VPN) && (dTLB[m].ValidBit == 1) && (dTLB[m].Cycle>0) )
                {
                    PPN = dTLB[m].PhysicalPageNumber;
                    dTLB[m].Cycle = cycles;
                    PhysicalAddr = (PPN * D_var.pageSize) + offset;
                    checkDCache(PhysicalAddr);
                    break;
                }
            }
        }else  //d-PTE MISS, update d-PTE & d-TLB & d-Cache
        {
            updateDPTE(VPN);  //update d-PTE
            printf("DDisk HIT\n");
            fprintf( trace, "dDisk");
            updateDLTB(VPN);  //update d-LTB
            for(m=0 ; m<dTLB_entries ; m++)  //d-TLB HIT
            {
                if( (dTLB[m].VirtualPageNumber == VPN) && (dTLB[m].ValidBit == 1) && (dTLB[m].Cycle>0) )
                {
                    PPN = dTLB[m].PhysicalPageNumber;
                    dTLB[m].Cycle = cycles;
                    PhysicalAddr = (PPN * D_var.pageSize) + offset;  //Move on to check d-Cache
                    checkDCache(PhysicalAddr);
                    break;
                }
            }
        }
    }
}

void updateDLTB(unsigned int VPN)
{
    unsigned int PPN;
    int index, minC = 555555;
    for(int m=0 ; m<dTLB_entries ; m++)
    {
        if(dTLB[m].ValidBit == 0)
        {
            index = m;
            break;
        }else
        {
            if(dTLB[m].Cycle < minC)
            {
                minC = dTLB[m].Cycle;
                index = m;
            }
        }
    }
    PPN = dPTE[VPN].PhysicalPageNumber;

    dTLB[index].ValidBit = 1;
    dTLB[index].VirtualPageNumber = VPN;
    dTLB[index].PhysicalPageNumber = PPN;
    dTLB[index].Cycle = cycles;
}

int checkDPTE(unsigned int VPN)
{
    if(dPTE[VPN].ValidBit == 1)  //d-PTE HIT
    {
        dPTE_HIT++;
        printf("DPTE HIT\n");
        return 1;
    }else  //d-PTE MISS
    {
        dPTE_MISS++;
        //printf("DPTE MISS\n");
        return 0;
    }
}

void updateDPTE(unsigned int VPN)
{
    unsigned int PPN;
    int min = 555555, i, j;
    int Case = 0;
    for(i = 0 ; i<dMem_entries ; i++)
    {
  		  if(dMem[i].ValidBit == 0)
        {
            Case = 1;
  			    PPN = i;
  			    break;
		    }else
        {
            if (dMem[i].Cycle < min)
            {
              PPN = i;
              min = dMem[i].Cycle;
            }
        }
		}
    dMem[PPN].ValidBit = 1;
	  dMem[PPN].Cycle = cycles;
    if(Case == 1)
    {
        dPTE[VPN].ValidBit = 1;
        dPTE[VPN].PhysicalPageNumber = PPN;
    }else
    {
        for(i=0 ; i<dPTE_entries ; i++)
        {
            if( dPTE[i].PhysicalPageNumber == PPN )
            {
                dPTE[i].ValidBit = 0;
                dPTE[i].PhysicalPageNumber = 0;
            }
        }
        dPTE[VPN].ValidBit = 1;
        dPTE[VPN].PhysicalPageNumber = PPN;
        for(i=0 ; i<dTLB_entries ; i++)
        {
            if( dTLB[i].PhysicalPageNumber == PPN )
            {
                dTLB[i].ValidBit = 0;
                dTLB[i].VirtualPageNumber = 0;
                dTLB[i].PhysicalPageNumber = 0;
                dTLB[i].Cycle = 0;
            }
        }
        unsigned int PhysicalAddr;
        unsigned int index, tag;
        for(i=0 ; i<D_var.pageSize ; i++)
        {
            PhysicalAddr = (PPN * D_var.pageSize) + i;
            index = (PhysicalAddr / D_var.blockSize) % dCache_entries;
            tag = (PhysicalAddr / D_var.blockSize) / dCache_entries;
            if(D_var.set == 1)
            {
                if(dCache[index][0].Tag == tag)
                {
                    dCache[index][0].ValidBit = 0;
                }
            }else
            {
                for(j=0 ; j<D_var.set ; j++)
                {
                    if( (dCache[index][j].Tag == tag) && (dCache[index][j].ValidBit == 1) )
                    {
                        dCache[index][j].ValidBit = 0;
                        dCache[index][j].MRU = 0;
                    }
                }
            }
        }
    }
}

void checkDCache(unsigned int PhysicalAddr)
{

    unsigned int index = (PhysicalAddr / D_var.blockSize) % dCache_entries;
    unsigned int tag = (PhysicalAddr / D_var.blockSize) / dCache_entries;
    if(D_var.set == 1)
    {
        printf("%d %d %u %u\n", index, dCache[index][0].ValidBit, dCache[index][0].Tag, tag);
        if( (dCache[index][0].Tag == tag) && (dCache[index][0].ValidBit == 1) )  //d-Cache HIT
        {
            dCache_HIT++;
            printf("DCACHE HIT\n");
            fprintf( trace, "DCache");
        }else  //d-Cache MISS, update d-Cache
        {
            dCache_MISS++;
            //printf("DCACHE MISS\n");
            dCache[index][0].ValidBit = 1;
            dCache[index][0].Tag = tag;
            dCache[index][0].MRU = 0;
        }
    }else
    {
        int find = 0, i, j, k;
        int MRU_FULL = 1;
        for(i=0 ; i<D_var.set ; i++)
        {
            //printf("%d %d %d\n", dCache[index][i].Tag, tag, dCache[index][i].ValidBit);
            if( (dCache[index][i].Tag == tag) && (dCache[index][i].ValidBit == 1) )  //d-Cache HIT
            {
                find = 1;
                dCache_HIT++;
                printf("DCACHE HIT\n");
                fprintf( trace, "DCache");
                dCache[index][i].MRU = 1;
                for(j=0 ; j<D_var.set ; j++)
                {
                    MRU_FULL = MRU_FULL & dCache[index][j].MRU;
                }
                if(MRU_FULL == 1)
                {
                    for(k=0 ; k<D_var.set ; k++)
                    {
                        if(k != i) dCache[index][k].MRU = 0;
                    }
                }
                break;
            }
        }
        if(find == 0)  //d-Cache MISS, update d-Cache
        {
            dCache_MISS++;
            //printf("DCACHE MISS\n");
            for(i=0 ; i<D_var.set ; i++)  //Find from valid bit
            {
                //printf("%d %d %d\n", dCache[index][i].Tag, tag, dCache[index][i].ValidBit);
                if(dCache[index][i].ValidBit == 0)
                {
                    dCache[index][i].ValidBit = 1;
                    dCache[index][i].Tag = tag;
                    dCache[index][i].MRU = 1;
                    for(j=0 ; j<I_var.set ; j++)
                    {
                        MRU_FULL = MRU_FULL & dCache[index][j].MRU;
                    }
                    if(MRU_FULL == 1)
                    {
                        for(k=0 ; k<D_var.set ; k++)
                        {
                            if(k != i) dCache[index][k].MRU = 0;
                        }
                    }
                    return;
                }
            }
            for(i=0 ; i<D_var.set ; i++)  //Find from MRU
            {
                //printf("%d %d %d\n", dCache[index][i].Tag, tag, dCache[index][i].MRU);
                if(dCache[index][i].MRU == 0)
                {
                    dCache[index][i].ValidBit = 1;
                    dCache[index][i].Tag = tag;
                    dCache[index][i].MRU = 1;
                    for(j=0 ; j<D_var.set ; j++)
                    {
                        MRU_FULL = MRU_FULL & dCache[index][j].MRU;
                    }
                    if(MRU_FULL == 1)
                    {
                        for(k=0 ; k<I_var.set ; k++)
                        {
                            if(k != i) dCache[index][k].MRU = 0;
                        }
                    }
                    return;
                }
            }
        }
    }
}

void writeReport()
{
    report = fopen("report.rpt","w");
    fprintf( report, "ICache :\n");
    fprintf( report, "# hits: %u\n", iCache_HIT);
    fprintf( report, "# misses: %u\n\n", iCache_MISS);
    fprintf( report, "DCache :\n");
    fprintf( report, "# hits: %u\n", dCache_HIT);
    fprintf( report, "# misses: %u\n\n", dCache_MISS);
    fprintf( report, "ITLB :\n");
    fprintf( report, "# hits: %u\n", iTLB_HIT);
    fprintf( report, "# misses: %u\n\n", iTLB_MISS);
    fprintf( report, "DTLB :\n");
    fprintf( report, "# hits: %u\n", dTLB_HIT);
    fprintf( report, "# misses: %u\n\n", dTLB_MISS);
    fprintf( report, "IPageTable :\n");
    fprintf( report, "# hits: %u\n", iPTE_HIT);
    fprintf( report, "# misses: %u\n\n", iPTE_MISS);
    fprintf( report, "DPageTable :\n");
    fprintf( report, "# hits: %u\n", dPTE_HIT);
    fprintf( report, "# misses: %u\n\n", dPTE_MISS);
    fclose(report);
    fclose(trace);
}
