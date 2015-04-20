/* ----------------------------------------

* File Name : emulator.c

* Created on : 05-01-2015

* Last Modified on : Mon 02 Feb 2015 04:44:12 PM JST

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


int main(int argc, char **argv){
  unsigned int prog_count = 0x100;
  unsigned int STATUS = emulator(prog_count >> 2);

  int i;
  for (i = 0; i < 32; i++){
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
    if (instruction != 0){
      decode(instruction, &opcode, &funct, &rs, &rt, &rd, &imm, &sa, &prog_count_1, &EMULATOR_STATUS);
    }
  }

  if (EMULATOR_STATUS == FINISH){
    printf("END OF SIMULATION\n");
  }
  else{
    printf("FORCE QUIT\n");
  }
  return EMULATOR_STATUS;
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

  /*common for every thing*/
  //TEMP[4] = Y;
  //TEMP[5] = Z;
  //TEMP4 = Y;
  //TEMP5 = Z;
  
  if (*opcode == 0x00 || *opcode == 0x09 || *opcode == 0x2B) {
    if (*funct == 0x08){//:/*jr*/
      //TEMP[0] = *rs;
      write_value((-1), TEMP_REG);
      TEMP0 = *rs;
      TEMP1 = TEMP_REG;
      TEMP2 = TEMP_REG;
      add ();
      *prog_count = get_value(TEMP_REG);
      write_value (get_value(TEMP_REG), RETURN_PC);
    }
    else if (*funct == 0x0C){/*syscall*/
      if ((get_value (2) == 10)){
	*EMULATOR_STATUS = FINISH;
      }
    }
    else if (((*opcode == 0x00) && (*funct == 0x21)) || (*opcode == 0x09) || (*opcode == 0x2B)){/*addiu*/
      (*opcode == 0x09 || *opcode == 0x2B)? write_value ((signed short) *imm, TEMP_REG): write_value(0, TEMP_REG);
      TEMP0 = *rs;
/*
      if ((*opcode == 0x00) && (*funct == 0x21)){
	TEMP1 = *rt;
      }
      else {
	TEMP1 = TEMP_REG;
      }
      */
      TEMP1 = (*opcode == 0x00 && *funct ==  0x21) ? *rt : TEMP_REG;
      //
      /*
      if (*opcode == 0x09){
	TEMP2 = *rt;
      }
      else if (*opcode == 0x2B){
	TEMP2 = TEMP_REG;
      }
      else {
    	TEMP2 = *rd;
      }*/
      TEMP2 = (*opcode == 0x09)? *rt: (*opcode == 0x2B)? TEMP_REG : *rd;//     *rt;
      //
      add ();/*
      if (*opcode == 0x2B){
	write_value(get_value(*rt), get_value(TEMP2) >> 2);
      }*/
      (*opcode == 0x2B) ? write_value(get_value(*rt), get_value(TEMP2) >> 2) : write_value(get_value(get_value(TEMP2) >> 2), get_value(TEMP2) >> 2);
      //MEM[get_value(TEMP2) >> 2] = (*opcode == 0x2B)? get_value(*rt): MEMget_value(get_value(TEMP2) >> 2);
      //
    }
#if 0
    else if (*funct == 0x21){/*addu*/
      TEMP0 = *rs;
      TEMP1 = *rt;
      TEMP2 = *rd;
      add ();
    }
#endif
    else {
      printf("%x Unimplemented Instructions R\n", *prog_count << 2);
      *EMULATOR_STATUS = FINISH;
      *EMULATOR_STATUS = FINISH;
    }
  }
  else if (*opcode == 0x03 || *opcode == 0x02) {
    if (*opcode == 0x03){/*jal*/
      write_value (*prog_count, TEMP_REG);
      write_value (1, Z);
      TEMP0 = TEMP_REG;
      TEMP1 = Z;
      TEMP2 = TEMP_REG;
      add (); 
      write_value (get_value (TEMP_REG), 31);
      write_value (((((*rs << 5) | *rt) << 16) | *imm), TEMP_REG);
      write_value ((-1), Z);
      add ();
      *prog_count = get_value (TEMP_REG);
    }
    else {
      printf("%x Unimplemented Instructions J\n",*prog_count << 2);
      *EMULATOR_STATUS = FINISH;
    }
  }
  else {
    if (*opcode == 0x05){/*bne*/
      if (get_value(*rs) != get_value(*rt)){
        write_value (*prog_count, TEMP_REG);
	write_value (((signed short)*imm), Z);
	TEMP0 = TEMP_REG;
	TEMP1 = Z;
	TEMP2 = TEMP_REG;
	add ();
	write_value ((-1), Z);
	add ();
	*prog_count = get_value(TEMP_REG);
      }
    }
#if 0
    else if (((*opcode == 0x00) && (*funct == 0x21)) || (*opcode == 0x09) || (*opcode == 0x2B)){/*addiu*/
      if ((*opcode == 0x09) || (*opcode == 0x2B)){
        write_value((signed short)*imm, TEMP_REG);
      }

      TEMP0 = *rs;

      if ((*opcode == 0x00) && (*funct == 0x21)){
	TEMP1 = *rt;
      }
      else {
	TEMP1 = TEMP_REG;
      }
      //TEMP1 = (*opcode == 0x00 && *funct ==  0x21) ? *rt : TEMP_REG;
      //
      if (*opcode == 0x09){
	TEMP2 = *rt;
      }
      else if (*opcode == 0x2B){
	TEMP2 = TEMP_REG;
      }
      else {
    	TEMP2 = *rd;
      }
      //TEMP2 = (*opcode == 0x09)? *rt: (*opcode == 0x2B)? TEMP_REG : *rd;//     *rt;
      //
      add ();
      if (*opcode == 0x2B){
	write_value(get_value(*rt), get_value(TEMP2) >> 2);
      }
      //MEM[get_value(TEMP2) >> 2] = (*opcode == 0x2B)? get_value(*rt): MEMget_value(get_value(TEMP2) >> 2);
      //
    }
#endif 
    else if (*opcode == 0x0F){
      write_value((*imm << 16 | 0x0000), *rt);
    }
#if 0
    else if (*opcode == 0x2B){/*sw*/
      write_value((signed short)(*imm), TEMP_REG);
      TEMP0 = *rs;
      TEMP1 = TEMP_REG;
      TEMP2 = TEMP_REG;
      add ();//calculate address
      write_value (get_value(*rt), get_value(TEMP2) >> 2);
    }
#endif 
    else {
      printf("%x Unimplemented Instructions I\n",*prog_count << 2);
      *EMULATOR_STATUS = FINISH;
    }
  }
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

/*
void sub (){
  unsigned int a = get_value(TEMP0);
  unsigned int b = get_value(TEMP1);

  unsigned int c = a - b;

  write_value (c,TEMP2);
}*/
