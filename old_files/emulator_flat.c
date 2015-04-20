/* ----------------------------------------

* File Name : emulator.c

* Created on : 05-01-2015

* Last Modified on : Tue 03 Feb 2015 01:57:01 PM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include "config.h"
#include <string.h>
#include <stdlib.h>


#if 0
#include "fibcall0.h"
#include "fibcall1.h"
#include "fibcall2.h"
#include "fibcall3.h"
#endif

#include "fibcall.h"

//#include "decode.c"


uint emulator (unsigned int);
void decode(uint, uchar*, uchar*, uchar*, uchar*, uchar*, ushort*, uchar*, uint*, unsigned int*);
unsigned int get_value(unsigned int);
void write_value(int, unsigned int);
void add ();
int NEW_ADD(int, int);
int NEW_ADD2(int, int);


int main(int argc, char **argv){
  unsigned int prog_count = 0x100;
  unsigned int STATUS = emulator(prog_count >> 2);
  int i;
  for (i = 0; i < 32; i++){
    if ((i%4) == 0)
      printf("\n");
    printf("%d\t",get_value(i));
  }
  printf("\n");
  return STATUS;
}





uint emulator (unsigned int prog_count){
  uint EMULATOR_STATUS = NORMAL;
  //uint prog_count = START_PC;
  unsigned int prog_count_1 = prog_count;
  //unsigned int temp = prog_count;
  unsigned int instruction;

  write_value(0x0000800, 29);

  uchar opcode;
  uchar funct;
  uchar rs;
  uchar rt;
  uchar rd;
  ushort imm;
  uchar sa;

  unsigned int test = 0;
  
  while (EMULATOR_STATUS == NORMAL){
    /*fetch instructions*/
    instruction = get_value (prog_count);//MEM[prog_count];
    prog_count_1 = prog_count_1 + 1;
    prog_count = prog_count_1;
    //if (instruction != 0){
      decode(instruction, &opcode, &funct, &rs, &rt, &rd, &imm, &sa, &prog_count_1, &EMULATOR_STATUS);
    //}
  }
#if 0
  if (EMULATOR_STATUS == FINISH){
    printf("END OF SIMULATION\n");
  }
  else{
    printf("FORCE QUIT\n");
  }
  return EMULATOR_STATUS;
#endif
}

void decode(uint instruction, uchar *opcode, uchar *funct, uchar *rs, uchar *rt, uchar *rd, ushort *imm, uchar *sa, uint *prog_count, unsigned int *EMULATOR_STATUS){
  /*decode instructions*/
  *opcode = instruction >> 26;
  *funct = instruction & 0X0000003F;
  *rs = (instruction >> 21) & 0X0000001F;
  *rt = (instruction >> 16) & 0X0000001F;
  *rd = (instruction >> 11) & 0X0000001F;
  *imm = instruction & 0xFFFF;
  *sa = (instruction >> 6) & 0X0000001F;


  int SRC1 = get_value (*rs);
  int SRC2 = get_value (*rt);
  int SRC3 = get_value (*rd);
  int DEST = 0x0;
  unsigned int RET_ADD = *prog_count;
  unsigned int ST_LOC;
  unsigned int WB_LOC = TEMP_REG;
  
  if (*opcode == 0x00 && *funct == 0x08) {/*jr*/
    SRC2 = -1;
    DEST = NEW_ADD2 (SRC1, SRC2);
    RET_ADD = DEST;
  }
  else if (*opcode == 0x00 && *funct == 0x0C){/*syscall*/
    if ((get_value (2) == 10)){
      *EMULATOR_STATUS = FINISH;
    }
  }
  else if (*opcode == 0x00 && *funct == 0x21){/*addu*/
    DEST = NEW_ADD (SRC1, SRC2);
    //WB_LOC = *rd;
    //write_value(DEST, *rd);
  }
  else if (*opcode == 0x03) {/*jal*/
    DEST = NEW_ADD2 (RET_ADD, 1);
    //WB_LOC = 31;
    //write_value (TEMP, 31);
    unsigned int TEMP = NEW_ADD2 (((((*rs << 5) | *rt) << 16) | *imm), -1);
    RET_ADD = TEMP;
  }
  if (*opcode == 0x05){/*bne*/
    if (SRC1 != SRC2){
      DEST = NEW_ADD2 (RET_ADD, (signed short) *imm);
      DEST = NEW_ADD2 (DEST, -1);
      RET_ADD = DEST;
    }
  }
  else if (*opcode == 0x09){/*addiu*/
    DEST = NEW_ADD (SRC1, (signed short) *imm);
    //WB_LOC = *rt;
    //write_value (DEST, *rt);
  }
  else if (*opcode == 0x0F){
    DEST = (*imm << 16) | 0x0000;
    //WB_LOC = *rt;
  }
  else if (*opcode == 0x2B){/*sw*/
    //DEST = NEW_ADD2 (SRC1, (signed short) *imm);
    //WB_LOC = NEW_ADD2 (SRC1, (signed short) *imm);
    //WB_LOC = WB_LOC >> 2;
    ST_LOC = NEW_ADD2 (SRC1, (signed short) *imm);
    ST_LOC = WB_LOC >> 2;
    //write_value (SRC2, DEST >> 2);
    DEST = SRC2;
  }
  WB_LOC = ((*opcode ==  0x00 && *funct == 0x08) ? TEMP_REG:0x0) | ((*opcode == 0x00 && *funct == 0x0C) ? TEMP_REG : 0x0) | ((*opcode == 0x00 && *funct == 0x21) ? *rd: 0x0) | ((*opcode == 0x03) ? 31 : 0x0) | ((*opcode == 0x05) ? DEST : 0x0) | ((*opcode == 0x09) ? *rt : 0x0) | ((*opcode == 0x0F) ? *rt : 0) | ((*opcode == 0x2B) ? ST_LOC : 0x0);
  write_value (DEST, WB_LOC);
  *prog_count = RET_ADD;
}


unsigned int get_value(unsigned int location){
  return MEM[location];
}

void write_value(int value, unsigned int location){
  MEM[location] = value;
}


void add (){
  unsigned int a = get_value(TEMP0);
  unsigned int b = get_value(TEMP1);
  unsigned int c = a + b;
  write_value (c,TEMP2);
}

int NEW_ADD (int a, int b){
  return a + b;
}

int NEW_ADD2 (int a, int b){
  return (a+b) & 0x7FF;
}
