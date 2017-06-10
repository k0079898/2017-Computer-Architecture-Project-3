#include "processor.h"

void doInstruction()
{
  int i;
  for(i=0; i<4; i++) instruction = (instruction<<8) + (unsigned char)iDisk[PC+i];
  opcode = instruction >> 26;  //Get opcode in 31~26
  //printf("%08X ", instruction);
  decode();
}

void decode()  //Decode the instruction
{
  switch (opcode) {
    case 0x00: //R-Type
        funct = instruction << 26 >> 26;  //5~0
        switch (funct) {
          case 0x20: rType("add  ");  break;
          case 0x21: rType("addu ");  break;
          case 0x22: rType("sub  ");  break;
          case 0x24: rType("and  ");  break;
          case 0x25: rType("or   ");  break;
          case 0x26: rType("xor  ");  break;
          case 0x27: rType("nor  ");  break;
          case 0x28: rType("nand ");  break;
          case 0x2A: rType("slt  ");  break;
          case 0x00: rType("sll  ");  break;
          case 0x02: rType("srl  ");  break;
          case 0x03: rType("sra  ");  break;
          case 0x08: rType("jr   ");  break;
          case 0x18: rType("mult ");  break;
          case 0x19: rType("multu");  break;
          case 0x10: rType("mfhi ");  break;
          case 0x12: rType("mflo ");  break;
        }
        break;
    //I-Type
    case 0x08: iType("addi "); break;
    case 0x09: iType("addiu"); break;
    case 0x23: iType("lw   "); break;
    case 0x21: iType("lh   "); break;
    case 0x25: iType("lhu  "); break;
    case 0x20: iType("lb   "); break;
    case 0x24: iType("lbu  "); break;
    case 0x2B: iType("sw   "); break;
    case 0x29: iType("sh   "); break;
    case 0x28: iType("sb   "); break;
    case 0x0F: iType("lui  "); break;
    case 0x0C: iType("andi "); break;
    case 0x0D: iType("ori  "); break;
    case 0x0E: iType("nori "); break;
    case 0x0A: iType("slti "); break;
    case 0x04: iType("beq  "); break;
    case 0x05: iType("bne  "); break;
    case 0x07: iType("bgtz "); break;
    //J-Type
    case 0x02: jType("j    "); break;
    case 0x03: jType("jal  "); break;
    case 0x3F:
        halt = 1;
        strcpy(iName, "halt ");
        checkITLB(PC);
        break;
    default:
        printf("illegal instruction found at 0x%08X\n", PC);
        halt = 1;
  }
}

void rType(char *command)  //Deal with R-Type instruction
{
  strcpy(iName, command);
  checkITLB(PC);
  rs = instruction << 6 >> 27;
  rt = instruction << 11 >> 27;
  rd = instruction << 16 >> 27;
  shamt = instruction << 21 >> 27;
  //printf("R-Type\n");
  if(strcmp(command, "add  ")==0)
  {
    //printf("0x%08X: add $%u, $%u, $%u\n", PC, rd, rs, rt);
    if(rd==0) writeToRegZero = 1;
    unsigned int result = (int32_t)REG[rs] + (int32_t)REG[rt];
    NumberOverflowDetection(REG[rs], REG[rt], result);
    if(writeToRegZero!=1) REG[rd] = result;
  }else if(strcmp(command, "addu ")==0)
  {
    //printf("0x%08X: addu $%u, $%u, $%u\n", PC, rd, rs, rt);
    if(rd==0) writeToRegZero = 1;
    else REG[rd] = REG[rs] + REG[rt];
  }else if(strcmp(command, "sub  ")==0)
  {
    //printf("0x%08X: sub $%u, $%u, $%u\n", PC, rd, rs, rt);
    if(rd==0) writeToRegZero = 1;
    unsigned int result = (int32_t)REG[rs] - (int32_t)REG[rt];
    NumberOverflowDetection(REG[rs], REG[rt]*(-1), result);  //a - b = a + (-b)
    if(writeToRegZero!=1) REG[rd] = result;
  }else if(strcmp(command, "and  ")==0)
  {
    //printf("0x%08X: and $%u, $%u, $%u\n", PC, rd, rs, rt);
    if(rd==0) writeToRegZero = 1;
    else REG[rd] = REG[rs] & REG[rt];
  }else if(strcmp(command, "or   ")==0)
  {
    //printf("0x%08X: or $%u, $%u, $%u\n", PC, rd, rs, rt);
    if(rd==0) writeToRegZero = 1;
    else REG[rd] = REG[rs] | REG[rt];
  }else if(strcmp(command, "xor  ")==0)
  {
    //printf("0x%08X: xor $%u, $%u, $%u\n", PC, rd, rs, rt);
    if(rd==0) writeToRegZero = 1;
    else REG[rd] = REG[rs] ^ REG[rt];
  }else if(strcmp(command, "nor  ")==0)
  {
    //printf("0x%08X: nor $%u, $%u, $%u\n", PC, rd, rs, rt);
    if(rd==0) writeToRegZero = 1;
    else REG[rd] = ~(REG[rs] | REG[rt]);
  }else if(strcmp(command, "nand ")==0)
  {
    //printf("0x%08X: nand $%u, $%u, $%u\n", PC, rd, rs, rt);
    if(rd==0) writeToRegZero = 1;
    else REG[rd] = ~(REG[rs] & REG[rt]);
  }else if(strcmp(command, "slt  ")==0)
  {
    //printf("0x%08X: slt $%u, $%u, $%u\n", PC, rd, rs, rt);
    if(rd==0) writeToRegZero = 1;
    else REG[rd] = ((int32_t)REG[rs] < (int32_t)REG[rt]) ? 1:0;
  }else if(strcmp(command, "sll  ")==0)
  {
    //printf("0x%08X: sll $%u, $%u, $%u\n", PC, rd, rt, shamt);
    if(rd==0 && instruction!=0x00000000)
    {
        if(rd==0 && rt==0 && shamt==0)
        {
          writeToRegZero = 0;
        }else
        {
          writeToRegZero = 1;
        }
    }else REG[rd] = REG[rt] << shamt;
  }else if(strcmp(command, "srl  ")==0)
  {
    //printf("0x%08X: srl $%u, $%u, $%u\n", PC, rd, rt, shamt);
    if(rd==0) writeToRegZero = 1;
    else REG[rd] = REG[rt] >> shamt;
  }else if(strcmp(command, "sra  ")==0)
  {
    //printf("0x%08X: sra $%u, $%u, $%u\n", PC, rd, rt, shamt);
    if(rd==0) writeToRegZero = 1;
    else REG[rd] = (int32_t)REG[rt] >> shamt;
  }else if(strcmp(command, "jr   ")==0)
  {
    //printf("0x%08X: jr $%u\n", PC, rs);
    PC = REG[rs];
    return;  //Witout PC = PC + 4;
  }else if(strcmp(command, "mult ")==0)
  {
    if(need_mfHILO==1) overwriteHILO = 1;
    //printf("0x%08X: mult $%u, $%u\n", PC, rs, rt);
    int64_t R_rs = (int32_t)REG[rs];
    int64_t R_rt = (int32_t)REG[rt];
    uint64_t result = R_rs * R_rt;
    //printf("%lld * %lld = %lld\n", R_rs, R_rt, result);
    HI = result >> 32;
    LO = result << 32 >> 32;
    need_mfHILO = 1;
  }else if(strcmp(command, "multu")==0)
  {
    if(need_mfHILO==1) overwriteHILO = 1;
    //printf("0x%08X: multu $%u, $%u\n", PC, rs, rt);
    uint64_t R_rs = (uint32_t)REG[rs];
    uint64_t R_rt = (uint32_t)REG[rt];
    uint64_t result = R_rs * R_rt;
    //printf("%llu * %llu = %llu\n", R_rs, R_rt, result);
    HI = result >> 32;
    LO = result << 32 >> 32;
    need_mfHILO = 1;
  }else if(strcmp(command, "mfhi ")==0)
  {
    //printf("0x%08X: mfhi $%u\n", PC, rd);
    if(rd==0) writeToRegZero = 1;
    else REG[rd] = HI;
    need_mfHILO = 0;
  }else if(strcmp(command, "mflo ")==0)
  {
    //printf("0x%08X: mflo $%u\n", PC, rd);
    if(rd==0) writeToRegZero = 1;
    else REG[rd] = LO;
    need_mfHILO = 0;
  }else halt = 1;
  PC = PC + 4;
}

void iType(char *command)  //Deal with I-Type instruction
{
  strcpy(iName, command);
  checkITLB(PC);
  rs = instruction << 6 >> 27;
  rt = instruction << 11 >> 27;
  //printf("I-Type\n");
  immediate = (short int)instruction << 16 >> 16;  //Immediate is not unsigned int!!!!
  if(strcmp(command, "addi ")==0)
  {
    //printf("0x%08X: addi $%u, $%u, %d\n", PC, rt, rs, immediate);
    if(rt==0) writeToRegZero = 1;
    unsigned int result = (int32_t)REG[rs] + (int32_t)immediate;
    NumberOverflowDetection(REG[rs], immediate, result);
    if(writeToRegZero!=1) REG[rt] = result;
  }else if(strcmp(command, "addiu")==0)
  {
    //printf("0x%08X: addiu $%u, $%u, %d\n", PC, rs, rt, immediate);
    if(rt==0) writeToRegZero = 1;
    else REG[rt] = REG[rs] + immediate;
  }else if(strcmp(command, "lw   ")==0)
  {
    //printf("0x%08X: lw $%u, %04X($%u)\n", PC, rt, immediate, rs);
    unsigned int addr = REG[rs] + (int32_t)immediate;
    checkDTLB(addr);
    if(rt==0) writeToRegZero = 1;
    NumberOverflowDetection(REG[rs], immediate, addr);
    MemAddOverflowDetection(addr, 3);
    DataMisalignedDetection(addr, 4);
    if(memAddOverflow==1 || dataMisaligned==1)
    {
        halt = 1;
        return;
    }
    if(writeToRegZero!=1) REG[rt] = (int32_t)(dDisk[addr] << 24 | dDisk[addr+1] << 16 | dDisk[addr+2] << 8 | dDisk[addr+3]);
  }else if(strcmp(command, "lh   ")==0)
  {
    //printf("0x%08X: lh $%u, %04X($%u)\n", PC, rt, immediate, rs);
    unsigned int addr = REG[rs] + (int32_t)immediate;
    checkDTLB(addr);
    if(rt==0) writeToRegZero = 1;
    NumberOverflowDetection(REG[rs], immediate, addr);
    MemAddOverflowDetection(addr, 1);
    DataMisalignedDetection(addr, 2);
    if(memAddOverflow==1 || dataMisaligned==1)
    {
        halt = 1;
        return;
    }
    if(writeToRegZero!=1) REG[rt] = (int16_t)(dDisk[addr] << 8 | dDisk[addr+1]);
  }else if(strcmp(command, "lhu  ")==0)
  {
    //printf("0x%08X: lhu $%u, %04X($%u)\n", PC, rt, immediate, rs);
    unsigned int addr = REG[rs] + (int32_t)immediate;
    checkDTLB(addr);
    if(rt==0) writeToRegZero = 1;
    NumberOverflowDetection(REG[rs], immediate, addr);
    MemAddOverflowDetection(addr, 1);
    DataMisalignedDetection(addr, 2);
    if(memAddOverflow==1 || dataMisaligned==1)
    {
        halt = 1;
        return;
    }
    if(writeToRegZero!=1) REG[rt] = (uint16_t)(dDisk[addr] << 8 | dDisk[addr+1]);
  }else if(strcmp(command, "lb   ")==0)
  {
    //printf("0x%08X: lb $%u, %04X($%u)\n", PC, rt, immediate, rs);
    unsigned int addr = REG[rs] + (int32_t)immediate;
    checkDTLB(addr);
    if(rt==0) writeToRegZero = 1;
    NumberOverflowDetection(REG[rs], immediate, addr);
    MemAddOverflowDetection(addr, 0);
    DataMisalignedDetection(addr, 1);
    if(memAddOverflow==1 || dataMisaligned==1)
    {
        halt = 1;
        return;
    }
    if(writeToRegZero!=1) REG[rt] = (int8_t)dDisk[addr];
  }else if(strcmp(command, "lbu  ")==0)
  {
    //printf("0x%08X: lbu $%u, %04X($%u)\n", PC, rt, immediate, rs);
    unsigned int addr = REG[rs] + (int32_t)immediate;
    checkDTLB(addr);
    if(rt==0) writeToRegZero = 1;
    NumberOverflowDetection(REG[rs], immediate, addr);
    MemAddOverflowDetection(addr, 0);
    DataMisalignedDetection(addr, 1);
    if(memAddOverflow==1 || dataMisaligned==1)
    {
        halt = 1;
        return;
    }
    if(writeToRegZero!=1) REG[rt] = (uint8_t)dDisk[addr];
    NumberOverflowDetection(REG[rs], immediate, addr);
  }else if(strcmp(command, "sw   ")==0)
  {
    //printf("0x%08X: sw $%u, %04X($%u)\n", PC, rt, immediate, rs);
    unsigned int addr = REG[rs] + (int32_t)immediate;
    checkDTLB(addr);
    NumberOverflowDetection(REG[rs], immediate, addr);
    MemAddOverflowDetection(addr, 3);
    DataMisalignedDetection(addr, 4);
    if(memAddOverflow==1 || dataMisaligned==1)
    {
        halt = 1;
        return;
    }
    dDisk[addr] = REG[rt] >> 24;
    dDisk[addr+1] = REG[rt] >> 16;
    dDisk[addr+2] = REG[rt] >> 8;
    dDisk[addr+3] = REG[rt];
  }else if(strcmp(command, "sh   ")==0)
  {
    //printf("0x%08X: sh $%u, %04X($%u)\n", PC, rt, immediate, rs);
    unsigned int addr = REG[rs] + (int32_t)immediate;
    checkDTLB(addr);
    NumberOverflowDetection(REG[rs], immediate, addr);
    MemAddOverflowDetection(addr, 1);
    DataMisalignedDetection(addr, 2);
    if(memAddOverflow==1 || dataMisaligned==1)
    {
        halt = 1;
        return;
    }
    dDisk[addr] = (REG[rt] >> 8) & 0xFF;
    dDisk[addr+1] = REG[rt] & 0xFF;
  }else if(strcmp(command, "sb   ")==0)
  {
    //printf("0x%08X: sb $%u, %04X($%u)\n", PC, rt, immediate, rs);
    unsigned int addr = REG[rs] + (int32_t)immediate;
    checkDTLB(addr);
    NumberOverflowDetection(REG[rs], immediate, addr);
    MemAddOverflowDetection(addr, 0);
    DataMisalignedDetection(addr, 1);
    if(memAddOverflow==1 || dataMisaligned==1) halt = 1;
    dDisk[addr] = REG[rt] & 0xFF;
  }else if(strcmp(command, "lui  ")==0)
  {
    //printf("0x%08X: lui $%u, %04X\n", PC, rt, immediate);
    if(rt==0) writeToRegZero = 1;
    else REG[rt] = immediate << 16;
  }else if(strcmp(command, "andi ")==0)
  {
    //printf("0x%08X: andi $%u, $%u, %04X\n", PC, rt, rs, immediate);
    if(rt==0) writeToRegZero = 1;
    else REG[rt] = REG[rs] & (uint16_t)immediate;
  }else if(strcmp(command, "ori  ")==0)
  {
    //printf("0x%08X: ori $%u, $%u, %04X\n", PC, rt, rs, immediate);
    if(rt==0) writeToRegZero = 1;
    else REG[rt] = REG[rs] | (uint16_t)immediate;
  }else if(strcmp(command, "nori ")==0)
  {
    //printf("0x%08X: nori $%u, $%u, %04X\n", PC, rt, rs, immediate);
    if(rt==0) writeToRegZero = 1;
    else REG[rt] = ~(REG[rs] | (uint16_t)immediate);
  }else if(strcmp(command, "slti ")==0)
  {
    //printf("0x%08X: slti $%u, $%u, %04X\n", PC, rt, rs, immediate);
    if(rt==0) writeToRegZero = 1;
    else REG[rt] = ((int32_t)REG[rs] < (int32_t)immediate) ? 1:0;
  }else if(strcmp(command, "beq  ")==0)
  {
    //printf("0x%08X: beq $%u, $%u, %04X\n", PC, rs, rt, immediate);
    if(REG[rs]==REG[rt])
    {
      PC = (PC + 4) + (4 * immediate);
      return;    //Witout PC = PC + 4;
    }
  }else if(strcmp(command, "bne  ")==0)
  {
    //printf("0x%08X: bne $%u, $%u, %04X\n", PC, rs, rt, immediate);
    if(REG[rs]!=REG[rt])
    {
      PC = (PC + 4) + (4 * immediate);
      return;    //Witout PC = PC + 4;
    }
  }else if(strcmp(command, "bgtz ")==0)
  {
    //printf("0x%08X: bgtz $%u, %04X\n", PC, rs, immediate);
    if((int32_t)REG[rs]>0)
    {
      PC = (PC + 4) + (4 * immediate);
      return;    //Witout PC = PC + 4;
    }
  }else halt = 1;
  PC = PC + 4;
}

void jType(char *command)  //Deal with J-type instruction
{
  strcpy(iName, command);
  checkITLB(PC);
  address = instruction << 6 >> 6;
  //printf("J-Type\n");
  if(strcmp(command, "j    ")==0)
  {
    //printf("0x%08X: j %08X\n", PC, address);
    PC = ((PC+4) >> 28 << 28) | (4*address);
  }else if(strcmp(command, "jal  ")==0)
  {
    //printf("0x%08X: jal %08X\n", PC, address);
    REG[31] = PC + 4;
    PC = ((PC+4) >> 28 << 28) | (4*address);
  }else halt = 1;
}

void NumberOverflowDetection(int in1, int in2, int out)
{
  if((in1>=0 && in2>=0 && out<0) || (in1<0 && in2<0 && out>=0)) numberOverflow = 1;
}

void MemAddOverflowDetection(int addr, int size)
{
  if(addr < 0 || addr >= DISK_SIZE || addr+size >= DISK_SIZE) memAddOverflow = 1;
}

void DataMisalignedDetection(int words, int size)
{
  if(words%size!=0) dataMisaligned = 1;
}
