/* ----------------------------------------

* File Name : emulator.c

* Created on : 05-01-2015

* Last Modified on : Mon 20 Apr 2015 04:51:09 PM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include "../config/config.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

//#include "../benchmarks/adpcm.h"
//#include "../benchmarks/bf.h"
//#include "../benchmarks/bs.h"
//#include "../benchmarks/bubble.h"
//#include "../benchmarks/crc.h"
//#include "../benchmarks/fibcall.h"
//#include "../benchmarks/gsm.h"
//#include "../benchmarks/intmm.h"
//#include "../benchmarks/insertsort.h"
//#include "../benchmarks/jfdctint.h"
//#include "../benchmarks/mpeg2.h"
#include "../benchmarks/vec_add.h"

#ifdef PROFILE
int add = 0;
int sub = 0;
int shift = 0;
int logic = 0;
int mult = 0;
int slt_u = 0;
int jump = 0;
int load = 0;
int store = 0;
int branch = 0;
#endif

uint emulator (uint);
void exec (uint, uchar*, uchar*, uchar*, uchar*, uchar*, ushort*, uchar*, uint*, uint*);
uint get_value(uint);
void write_value(int, uint);
//void add ();
int NEW_ADD(int, int);
int NEW_ADD2(int, int);
int NEW_ADD3(int, int);
uint SUB (uint, uint);
int SLL (int, int);
int SRL (int, int);
int SRA (int, int);
uint OR (uint, uint);
uint AND (uint, uint);
uint XOR (uint, uint);
int64_t MULT (int, int);
void subleq_machine(ushort prog_count);


int main(int argc, char **argv){
  uint prog_count = 0x2000;
  uint STATUS = emulator(prog_count >> 2);
#ifdef PRINT
  int i;
  printf("========\nREG FILE\n========\n");
  for (i = 0; i < 32; i++){
    printf("%d\t",get_value(i));
    if (((i+1) % 4) == 0)
      printf("\n");
  }
#endif

#ifdef PROFILE
  printf("Addition: %d\n", add);
  printf("Subtraction: %d\n", sub);
  printf("All shift: %d\n", shift);
  printf("All logic: %d\n", logic);
  printf("All multiplication: %d\n", mult);
  printf("All slt and sltu: %d\n", slt_u);
  printf("All jump: %d\n", jump);
  printf("All load from memory: %d\n", load);
  printf("All store to memory: %d\n", store);
  printf("All conditional branch: %d\n", branch);
#endif


#if 0
  printf("=========\nPRINT MEM\n=========\n");
  for (i = 0; i < 15; i++)
    printf("%d\t", ADD[i]);
  printf("\n");
  for (i = ADD_ROUTINE; i < ADD_ROUTINE + 15; i++)
    printf("%d\t", MEM[i]);
  printf("\n");
#endif
#if 0
  printf("=========\nPRINT MEM\n=========\n");
  for (i = 0x17f5; i < 0x17f5 + 10; i++)
    printf("%d\t", MEM[i]);
  printf("\n");
#endif
  return STATUS;
}


uint emulator (uint prog_count){
  uint EMULATOR_STATUS = NORMAL;
  uint prog_count_1 = prog_count;
  uint instruction;

  uchar opcode;
  uchar funct;
  uchar rs;
  uchar rt;
  uchar rd;
  ushort imm;
  uchar sa;

#ifdef PRINT
  uint i = 0;
#endif

  while (EMULATOR_STATUS == NORMAL){
    /*fetch instructions*/
#ifdef PRINT
    printf("%d: %3.3x, INST: %8.8x", i, prog_count << 2, get_value(prog_count));
#endif
    instruction = get_value (prog_count);
    prog_count_1 = prog_count_1 + 1;
    prog_count = prog_count_1;
    exec (instruction, &opcode, &funct, &rs, &rt, &rd, &imm, &sa, &prog_count_1, &EMULATOR_STATUS);
#ifdef PRINT
    i++;
#endif
  }
  prog_count_1 = prog_count_1 & 0x1; 
  return prog_count_1;//EMULATOR_STATUS;
}

void exec (uint instruction, uchar *opcode, uchar *funct, uchar *rs, uchar *rt, uchar *rd, ushort *imm, uchar *sa, uint *prog_count, uint *EMULATOR_STATUS){
  /*decode instructions*/
  *opcode = instruction >> 26;
  *rs = (instruction >> 21) & 0X0000001F;
  *rt = (instruction >> 16) & 0X0000001F;
  *rd = (instruction >> 11) & 0X0000001F;
  *sa = (instruction >> 6) & 0X0000001F;
  *funct = instruction & 0X0000003F;
  *imm = instruction & 0xFFFF;


  int SRC1 = get_value (*rs);
  int SRC2 = get_value (*rt);
  int RESULT = 0;//NEW_ADD (SRC1, SRC2); /*addu*/
  uint RET_ADD = *prog_count;// = *prog_count;
  uint WB_LOC = TEMP_REG;

  bool R_TYPE = (*opcode == 0x00);
  bool NOP_COND = (instruction == 0x0);
  bool SLL_COND = (R_TYPE & !NOP_COND & (*funct == 0x00));
  bool SRL_COND = (R_TYPE & (*funct == 0x02));
  bool SRA_COND = (R_TYPE & (*funct == 0x03));
  bool SLLV_COND = (R_TYPE & (*funct == 0x04));
  bool SRLV_COND = (R_TYPE & (*funct == 0x06));
  bool SRAV_COND = (R_TYPE & (*funct == 0x07));
  bool JR_COND = (R_TYPE & (*funct == 0x08));
  bool SYS_COND = (R_TYPE & (*funct == 0x0C));
  bool MFHI_COND = (R_TYPE & (*funct == 0x10));
  bool MFLO_COND = (R_TYPE & (*funct == 0x12));
  bool MULT_COND = (R_TYPE & (*funct == 0x18));
  //bool DIV_COND =
  bool ADDU_COND = (R_TYPE & (*funct == 0x21));
  bool SUBU_COND = (R_TYPE & (*funct == 0x23));
  bool AND_COND = (R_TYPE & (*funct == 0x24));
  bool OR_COND = (R_TYPE & (*funct == 0x25));
  bool XOR_COND = (R_TYPE & (*funct == 0x26));
  bool NOR_COND = (R_TYPE & (*funct == 0x27));
  bool SLT_COND = (R_TYPE & (*funct == 0x2A));
  bool SLTU_COND = (R_TYPE & (*funct == 0x2B));
  //
  bool J_COND = (*opcode == 0x02);
  bool JAL_COND = (*opcode == 0x03);
  //
  bool BLTZ_COND = ((*opcode == 0x01) & (*rt == 0x0));
  bool BGEZ_COND = ((*opcode == 0x01) & (*rt == 0x1));
  bool BEQ_COND = ((*opcode == 0x04) & (*rt != 0x0));
  bool BEQZ_COND = ((*opcode == 0x04) & (*rt == 0x0));
  bool BNE_COND = (*opcode == 0x05);
  bool BLEZ_COND = (*opcode == 0x06);
  bool BGTZ_COND = (*opcode == 0x07);
  bool ADDIU_COND = (*opcode == 0x09);
  bool SLTI_COND = (*opcode == 0x0A);
  bool SLTIU_COND = (*opcode == 0x0B);
  bool ANDI_COND = (*opcode == 0x0C);
  bool ORI_COND = (*opcode == 0x0D);
  bool XORI_COND = (*opcode == 0x0E);
  bool LUI_COND = (*opcode == 0x0F);
  bool LB_COND = (*opcode == 0x20);
  bool LH_COND = (*opcode == 0x21);
  bool LW_COND = (*opcode == 0x23);
  bool LBU_COND = (*opcode == 0x24);
  bool LHU_COND = (*opcode == 0x25);
  bool SB_COND = (*opcode == 0x28);
  bool SH_COND = (*opcode == 0x29);
  bool SW_COND = (*opcode == 0x2B);


#ifdef PROFILE
  if (ADDU_COND | ADDIU_COND) add++;
  if (SUBU_COND) sub++;
  if (AND_COND | ANDI_COND | OR_COND | ORI_COND | XOR_COND | XORI_COND | NOR_COND) logic++;
  if (SLL_COND | SLLV_COND | SRL_COND | SRLV_COND | SRA_COND | SRAV_COND) shift++;
  if (MULT_COND | MFHI_COND | MFLO_COND) mult++;
  if (SLT_COND | SLTU_COND | SLTI_COND | SLTIU_COND) slt_u++;
  if (JR_COND | J_COND | SYS_COND | JAL_COND) jump++;
  if (LB_COND | LH_COND | LW_COND | LBU_COND | LHU_COND) load++;
  if (SB_COND | SH_COND | SW_COND) store++;
  if (BLTZ_COND | BGEZ_COND | BEQ_COND | BEQZ_COND | BNE_COND | BLEZ_COND | BGTZ_COND) branch++;
#endif

  /*AND ANDI OR ORI XOR XORI*/
  bool LOGIC_IMM_COND = ANDI_COND | ORI_COND | XORI_COND; 

  uint LOGIC_IMM_INP = (LOGIC_IMM_COND) ? (ushort) *imm : SRC2;
  uint RES_AND = AND (SRC1, LOGIC_IMM_INP);
  uint RES_OR = OR (SRC1, LOGIC_IMM_INP);
  uint RES_XOR = XOR (SRC1, LOGIC_IMM_INP);
  /*NOR*/
  uint RES_NOR = ~RES_OR;

  uint RES_LOGIC = ((AND_COND | ANDI_COND) ? RES_AND : 0x0) |
		   ((OR_COND | ORI_COND) ? RES_OR : 0x0) |
		   ((XOR_COND | XORI_COND) ? RES_XOR : 0x0) |
		   ((NOR_COND) ? RES_NOR : 0x0);

  bool SUBLEQ_COND = false 
#ifndef USE_ADDER
		     | ADDU_COND | ADDIU_COND
#endif
#ifndef USE_SUB
		     | SUBU_COND 
#endif
#ifndef USE_MULT
		     | MULT_COND 
#endif
#ifndef USE_MFHI
		     | MFHI_COND 
#endif
#ifndef USE_MFLO
		     | MFLO_COND
#endif
#ifndef USE_SHIFTER
		     | SLL_COND | SLLV_COND | SRL_COND | SRLV_COND | SRA_COND | SRAV_COND
#endif
#ifndef USE_SYS
		     | SYS_COND
#endif
#ifndef USE_SET_LESS_THAN
		     | SLT_COND | SLTI_COND | SLTU_COND | SLTIU_COND
#endif
		     ; 
  
  /*MFHI*//**opcode == 0x00 & *funct == 0x10*/
#ifdef USE_MFHI
  uint RES_MFHI = get_value (HI);
#endif
#ifdef USE_MFLO
  uint RES_MFLO = get_value (LO);
#endif


#ifdef USE_MULT
  /*MULT*/ /**opcode == 0x00 & *funct == 0x18*/
  int64_t RES_MULT_TEMP = MULT (SRC1, SRC2);
  int RES_MULT_LO = (MULT_COND)? RES_MULT_TEMP & 0xFFFFFFFF : RES_MFLO;
  int RES_MULT_HI = (RES_MULT_TEMP >> 32);
#endif


#ifdef USE_SUB  
  /*SUBU*/ /**opcode == 0x00 & *funct == 0x23*/
  uint RES_SUBU = SUB ((uint) SRC1, (uint) SRC2);
#endif

  /*SRA*//*SRAV*//*SLL*//*SLLV*//*SRL*//*SRLV*/
  int SHIFT_INP = (SRAV_COND | SLLV_COND | SRLV_COND) ? SRC1 : *sa;
#ifdef USE_SHIFTER
  int RES_SRA_V = SRA (SRC2, SHIFT_INP);
  int RES_SLLV = SLL (SRC2, SHIFT_INP);
  int RES_SRLV = SRL (SRC2, SHIFT_INP);
#endif

  int ADD_INP = (ADDIU_COND)? (sshort)*imm : SRC2;
#ifdef USE_ADDER
  /*addu addiu*/
  uint RES_ADDU = NEW_ADD (SRC1, ADD_INP);
#endif  

  int SLT_INP = (SLTI_COND | SLTIU_COND) ? (sshort) *imm : SRC2;
#ifdef	USE_SET_LESS_THAN
  /*SLT*//*SLTI*/
  int RES_SLT_I = (SRC1 < SLT_INP) ? 1 : 0;
  /*SLTU*//*SLTIU*/
  int RES_SLTU_I = ((uint) SRC1 < (uint) SLT_INP) ? 1 : 0;
#endif



#if !defined(USE_ADDER) || !defined(USE_MULT) || !defined(USE_SUB) || !defined(USE_MFLO) || !defined(USE_MFHI) || !defined(USE_SHIFTER) || !defined(USE_SYS) || !defined(USE_SET_LESS_THAN)
  uint RES_SUBLEQ = 0;
  uint ROUTINE_ADD = 0x0
#ifndef	USE_ADDER
			     | ((ADDU_COND | ADDIU_COND) ? ADD_ROUTINE : 0x0)
#endif
#ifndef	USE_MULT
			     | ((MULT_COND) ? MUL_ROUTINE : 0x0)
#endif
#ifndef USE_SUB
			     | ((SUBU_COND) ? SUB_ROUTINE : 0x0)
#endif
#ifndef	USE_MFLO
			     | ((MFLO_COND) ? MFLO_ROUTINE : 0x0)
#endif
#ifndef	USE_MFHI
			     | ((MFHI_COND) ? MFHI_ROUTINE : 0x0)
#endif
#ifndef USE_SHIFTER
			     | ((SLLV_COND | SLL_COND) ? SLL_ROUTINE : 0x0)
			     | ((SRLV_COND | SRL_COND) ? SRL_ROUTINE : 0x0)
			     | ((SRAV_COND | SRA_COND) ? SRA_ROUTINE : 0x0)
#endif
#ifndef	USE_SYS
			     | ((SYS_COND) ? SYS_ROUTINE : 0x0)
#endif
#ifndef USE_SET_LESS_THAN
			     | ((SLT_COND | SLTI_COND) ? SLT_ROUTINE : 0x0)
			     | ((SLTU_COND | SLTIU_COND) ? SLTU_ROUTINE : 0x0)
#endif
			     ;

  int SUBLEQ_INP1 = 0x0
#ifndef USE_ADDER
		   | ((ADDIU_COND | ADDU_COND) ? SRC1 : 0x0)
#endif
#ifndef USE_MULT
		   | ((MULT_COND) ? SRC1 : 0x0)
#endif
#ifndef USE_SUB
		   | ((SUBU_COND) ? SRC1 : 0x0) 
#endif
#ifndef	USE_SHIFTER
		   | ((SLL_COND | SLLV_COND | SRL_COND | SRLV_COND | SRA_COND | SRAV_COND) ? SRC2 : 0x0)
#endif
#ifndef USE_SYS
		   | ((SYS_COND) ? SRC1 : 0x0)
#endif
#ifndef	USE_SET_LESS_THAN
		   | ((SLT_COND | SLTI_COND | SLTU_COND | SLTIU_COND) ? SRC1 : 0x0)
#endif
		   ; 


  int SUBLEQ_INP2 = 0x0
#ifndef	USE_ADDER
		   | ((ADDIU_COND | ADDU_COND) ? ADD_INP : 0x0)
#endif
#ifndef	USE_MULT
		   | ((MULT_COND) ? SRC2 : 0x0)
#endif
#ifndef USE_SUB
		   | ((SUBU_COND) ? SRC2 : 0x0)
#endif
#ifndef	USE_SHIFTER
		   | ((SLL_COND | SLLV_COND | SRL_COND | SRLV_COND | SRA_COND | SRAV_COND) ? SHIFT_INP : 0x0)
#endif
#ifndef USE_SYS
		   | ((SYS_COND) ? SRC2 : 0x0)
#endif
#ifndef	USE_SET_LESS_THAN
		   | ((SLT_COND | SLTI_COND | SLTU_COND | SLTIU_COND) ? SLT_INP : 0x0)
		   /*| ((SLTU_COND | SLTIU_COND) ? SLT_INP : 0x0)*/
#endif
		   ; 

  if (SUBLEQ_COND) {
    write_value (SUBLEQ_INP1, SRC1_LOC);
    write_value (SUBLEQ_INP2, SRC2_LOC);
    subleq_machine(ROUTINE_ADD);
    RES_SUBLEQ = get_value(DEST_LOC);
  }
#endif


#ifdef DEBUG_SUB
  if (SUBU_COND){
    printf("SUB: N:(%d,%d,%d), S:(%d,%d,%d)\n",SRC1,SRC2,(SRC1-SRC2),SUBLEQ_INP1,SUBLEQ_INP2,RES_SUBLEQ);
  }
#endif
#ifdef DEBUG_ADD
  if (ADDU_COND){
    printf("ADDU: N:(%d,%d,%d), S:(%d,%d,%d)\n",SRC1,ADD_INP,(SRC1+ADD_INP),SUBLEQ_INP1,SUBLEQ_INP2,RES_SUBLEQ);
  }
  else if (ADDIU_COND){
    printf("ADDIU: N:(%d,%d,%d), S:(%d,%d,%d)\n",SRC1,ADD_INP,(SRC1+ADD_INP),SUBLEQ_INP1,SUBLEQ_INP2,RES_SUBLEQ);
  }
#endif
#ifdef DEBUG_MULT
  if (MULT_COND){
    if ((((int64_t)SRC1) * SRC2) != (int64_t)(((uint64_t)get_value(HI) << 32) | (uint64_t)get_value(LO))) {
      printf("MULT: N:(%d,%d,%d,%d), S:(%d,%d,%d,%d)\n",SRC1,SRC2,(int)((((int64_t)SRC1)*SRC2)>>32), (int)((((int64_t)SRC1)*SRC2)&0xFFFFFFFF),SUBLEQ_INP1,SUBLEQ_INP2,get_value(HI),get_value(LO));
    }
  }
#endif
#ifdef DEBUG_MFLO
#endif
#ifdef DEBUG_MFHI
#endif
#ifdef DEBUG_SHIFTER
  if (SLL_COND){
    printf("SLL: N:(%x,%x,%x), S:(%x,%x,%x)\n",SRC2,SHIFT_INP,SLL(SRC2,SHIFT_INP),SUBLEQ_INP1,SUBLEQ_INP2,RES_SUBLEQ);
  }
  else if (SLLV_COND){
    printf("SLLV: N:(%x,%x,%x), S:(%x,%x,%x)\n",SRC2,SHIFT_INP,SLL(SRC2,SHIFT_INP),SUBLEQ_INP1,SUBLEQ_INP2,RES_SUBLEQ);
  }
  else if (SRL_COND){
    printf("SRL: N:(%x,%x,%x), S:(%x,%x,%x)\n",SRC2,SHIFT_INP,SRL(SRC2,SHIFT_INP),SUBLEQ_INP1,SUBLEQ_INP2,RES_SUBLEQ);
  }
  else if (SRLV_COND){
    printf("SRLV: N:(%x,%x,%x), S:(%x,%x,%x)\n",SRC2,SHIFT_INP,SRL(SRC2,SHIFT_INP),SUBLEQ_INP1,SUBLEQ_INP2,RES_SUBLEQ);
  }
  else if (SRA_COND){
    printf("SRA: N:(%x,%x,%x), S:(%x,%x,%x)\n",SRC2,SHIFT_INP,SRA(SRC2,SHIFT_INP),SUBLEQ_INP1,SUBLEQ_INP2,RES_SUBLEQ);
  }
  else if (SRAV_COND){
    printf("SRAV: N:(%x,%x,%x), S:(%x,%x,%x)\n",SRC2,SHIFT_INP,SRA(SRC2,SHIFT_INP),SUBLEQ_INP1,SUBLEQ_INP2,RES_SUBLEQ);
  }
#endif
#ifdef DEBUG_SET_LESS_THAN
  if (SLT_COND){
    printf("SLT: N:(%d,%d,%d), S:(%d,%d,%d)\n",SRC1,SLT_INP,((SRC1 < SLT_INP)?1:0),SUBLEQ_INP1,SUBLEQ_INP2,RES_SUBLEQ);
  }
  else if (SLTU_COND){
    printf("SLTU: N:(%d,%d,%d), S:(%d,%d,%d)\n",SRC1,SLT_INP,(((uint) SRC1 < (uint) SLT_INP)?1:0),SUBLEQ_INP1,SUBLEQ_INP2,RES_SUBLEQ);
  }
  else if (SLTI_COND){
    printf("SLTI: N:(%d,%d,%d), S:(%d,%d,%d)\n",SRC1,SLT_INP,((SRC1 < SLT_INP)?1:0),SUBLEQ_INP1,SUBLEQ_INP2,RES_SUBLEQ);
  }
  else if (SLTIU_COND){
    printf("SLTIU: N:(%d,%d,%d), S:(%d,%d,%d)\n",SRC1,SLT_INP,(((uint) SRC1 < (uint) SLT_INP)?1:0),SUBLEQ_INP1,SUBLEQ_INP2,RES_SUBLEQ);
  } 
#endif



  /*JR*/
  uint RET_ADD_JR = NEW_ADD2 (SRC1, -1);

  /*jal, j*/
  uint RES_JAL = NEW_ADD2 (RET_ADD, 1);
  uint RET_ADD_J = NEW_ADD2 (((((*rs << 5) | *rt) << 16) | *imm), -1);


  /*BNE BEQZ BLEZ*/
  uint COND_BR_ADD = NEW_ADD2 (NEW_ADD2 (RET_ADD, (sshort)*imm), -1);
  bool CHECK_EQ = (SRC1 == SRC2)? true : false;
  bool SRC_EQ_0 = (SRC1 == 0) ? true : false;
  bool SRC_GT_0 = (SRC1 > 0) ? true : false;

  bool BNE_COND_TEMP = (BNE_COND & !CHECK_EQ);
  bool BEQ_COND_TEMP = (BEQ_COND & CHECK_EQ);
  bool BEQZ_COND_TEMP = (BEQZ_COND & (SRC_EQ_0));
  bool BGEZ_COND_TEMP = (BGEZ_COND & (SRC_EQ_0 | SRC_GT_0));
  bool BLEZ_COND_TEMP = (BLEZ_COND & (SRC_EQ_0 | !SRC_GT_0));
  bool BLTZ_COND_TEMP = (BLTZ_COND & (!SRC_GT_0));
  bool BGTZ_COND_TEMP = (BGTZ_COND & (SRC_GT_0));

  bool BR_COND_ALL = (BNE_COND_TEMP | BEQ_COND_TEMP | BEQZ_COND_TEMP | BGEZ_COND_TEMP | BLEZ_COND_TEMP | BLTZ_COND_TEMP | BGTZ_COND_TEMP);

  uint RET_ADD_COND_BRANCH = (BR_COND_ALL) ? COND_BR_ADD : RET_ADD;

  /*lui*/
  int RES_LUI = (*imm << 16) | 0x0000;
  
  int MEM_ADD_TEMP = NEW_ADD3 (SRC1, (sshort) *imm);
  /*sw*/
  uint MEM_ADD = (MEM_ADD_TEMP >> 2);
  int RES_SW = SRC2;
  /*LW*/
  int RES_LW = get_value(MEM_ADD);//SRC2;
  /*SB*/
  uchar BYTE_CHECK = MEM_ADD_TEMP & 0x2;

  uint RES_SB0 = (RES_LW & 0xFFFFFF00) | (0xFF & (SRC2 << 0));
  uint RES_SB1 = (RES_LW & 0xFFFF00FF) | (0xFF00 & (SRC2 << 8));
  uint RES_SB2 = (RES_LW & 0xFF00FFFF) | (0xFF0000 & (SRC2 << 16));
  uint RES_SB3 = (RES_LW & 0x00FFFFFF) | (0xFF000000 & (SRC2 << 24));

  uint RES_SB = (BYTE_CHECK >> 1) ? ((BYTE_CHECK & 0x1)? RES_SB3 : RES_SB2) : ((BYTE_CHECK & 0x1)? RES_SB1 : RES_SB0);

  /*LBU, LB*/
  uint RES_LBU0 = (RES_LW & 0xFF) >> 0;
  uint RES_LBU1 = (RES_LW & 0xFF00) >> 8;
  uint RES_LBU2 = (RES_LW & 0xFF0000) >> 16;
  uint RES_LBU3 = (RES_LW & 0xFF000000) >> 24;

  uint RES_LBU = (BYTE_CHECK >> 1) ? ((BYTE_CHECK & 0x1)? RES_LBU3 : RES_LBU2) : ((BYTE_CHECK & 0x1)? RES_LBU1 : RES_LBU0);

  /*SH*/
  uchar BIT_CHECK = MEM_ADD_TEMP & 0x1;
  uint RES_SH0 = (RES_LW & 0xFFFF0000) | (0xFFFF & (SRC2 << 0));
  uint RES_SH1 = (RES_LW & 0x0000FFFF) | (0xFFFF0000 & (SRC2 << 16));
  uint RES_SH = (BIT_CHECK)? RES_SH1 : RES_SH0;

  /*LHU, LH*/
  uint RES_LHU0 = (RES_LW & 0xFFFF);
  uint RES_LHU1 = (RES_LW & 0xFFFF0000) >> 16;
  uint RES_LHU = (BIT_CHECK)? RES_LHU1 : RES_LHU0;

  bool RET_ADD_EXP = (J_COND | JR_COND | JAL_COND | BNE_COND | BEQZ_COND | BEQ_COND | BLEZ_COND | BLTZ_COND | BGEZ_COND | BGTZ_COND);
  RET_ADD = ((J_COND)? RET_ADD_J : 0x0) |/*J*/
	    ((JR_COND)? RET_ADD_JR : 0x0) |/*JR*/
	    ((JAL_COND)? RET_ADD_J : 0x0) |/*JAL*/
	    ((BNE_COND | BEQZ_COND | BEQ_COND | BLEZ_COND | BLTZ_COND | BGEZ_COND | BGTZ_COND) ? RET_ADD_COND_BRANCH : 0x0) |
#if 0
	    ((BNE_COND)? RET_ADD_BNE : 0x0) |/*BNE*/
	    ((BEQZ_COND)? RET_ADD_BEQZ : 0x0) |/*BEQZ*/
	    ((BEQ_COND)? RET_ADD_BEQ : 0x0) |/*BEQ*/
	    ((BLEZ_COND)? RET_ADD_BLEZ : 0x0) |/*BLEZ*/
	    ((BLTZ_COND)? RET_ADD_BLTZ : 0x0) |/*BLTZ*/
	    ((BGEZ_COND)? RET_ADD_BGEZ : 0x0) |/*BGEZ*/
	    ((BGTZ_COND)? RET_ADD_BGTZ : 0x0) |/*BGTZ*/
#endif
	    ((RET_ADD_EXP)? 0x0 : RET_ADD);

  bool RESULT_EXP = (J_COND | JR_COND
#ifndef USE_SYS
		    | SYS_COND 
#endif
		    | BNE_COND | BEQZ_COND | BLEZ_COND | BLTZ_COND | BGEZ_COND | BGTZ_COND | NOP_COND
#ifndef USE_MULT
		    | MULT_COND
#endif
		    );

  RESULT = (AND_COND | ANDI_COND | OR_COND | ORI_COND | XOR_COND | XORI_COND | NOR_COND) ? RES_LOGIC : 0x0 |
#if 0
  RESULT = ((AND_COND | ANDI_COND)? RES_AND : 0x0) | /*AND*/
	   ((OR_COND | ORI_COND)? RES_OR : 0x0) | /*OR*/
	   ((XOR_COND | XORI_COND)? RES_XOR : 0x0) | /*XOR*/
	   ((NOR_COND)? RES_NOR : 0x0) | /*NOR*/
#endif
#ifdef USE_SET_LESS_THAN
	   ((SLT_COND | SLTI_COND)? RES_SLT_I : 0x0) |/*SLT*//*SLTI*/
	   ((SLTU_COND | SLTIU_COND)? RES_SLTU_I : 0x0) |/*SLTU*/
#endif
	   ((LW_COND)? RES_LW : 0x0) |/*LW*/
	   ((LBU_COND)? RES_LBU : 0x0) | /*LBU*/
	   ((LB_COND)? (sint) RES_LBU : 0x0) | /*LB*/
	   ((LH_COND)? (sint) RES_LHU : 0x0) | /*LH*/
	   ((LHU_COND)? RES_LHU : 0x0) | /*LHU*/
#if !defined(USE_ADDER) || !defined(USE_SUB) || !defined(USE_MULT) || !defined(USE_MFLO) || !defined(USE_MFHI) || !defined(USE_SHIFTER) || !defined(USE_SET_LESS_THAN) || !defined (USE_SYS)
	   ((SUBLEQ_COND)? RES_SUBLEQ : 0x0) |/*ADDU*/
#endif
#ifdef USE_ADDER
	   ((ADDU_COND | ADDIU_COND)? RES_ADDU : 0x0)|
	   //((ADDIU_COND)? RES_ADDIU : 0x0)|
#endif
#ifdef USE_SUB
	   ((SUBU_COND)? RES_SUBU : 0x0) | /*SUBU*/
#endif
#ifdef USE_MULT
	   ((MULT_COND)? RES_MULT_HI : 0x0) | /*MULT*/
#endif
#ifdef USE_MFHI
	   ((MFHI_COND)? RES_MFHI : 0x0) | /*MFHI*/
#endif
#ifdef USE_MFLO
	   ((MFLO_COND)? RES_MFLO : 0x0) | /*MFLO*/
#endif
#ifdef USE_SHIFTER
           ((SLL_COND | SLLV_COND)? RES_SLLV : 0x0) |/*SLL*/
	   ((SRA_COND | SRAV_COND)? RES_SRA_V : 0x0) | /*SRA*//*SRAV*/
	   ((SRL_COND | SRLV_COND)? RES_SRLV : 0x0) | /*SRL*/
#endif
	   ((JAL_COND)? RES_JAL : 0x0) |/*JAL*/
	   ((LUI_COND)? RES_LUI : 0x0) |/*LUI*/
	   ((SW_COND)? RES_SW : 0x0) |/*SW*/
	   ((SB_COND)? RES_SB : 0x0) | /*SB*/
	   ((SH_COND)? RES_SH : 0x0) | /*SH*/
	   ((RESULT_EXP)? 0x0 : 0x0);/*EXPECTION*/



  bool WB_TEMP_REG = (J_COND | BEQZ_COND | JR_COND | SYS_COND | BNE_COND | BEQZ_COND | BLEZ_COND | BLTZ_COND | BGEZ_COND | BGTZ_COND | NOP_COND
#ifndef USE_MULT
		     | MULT_COND
#endif
		     );
  bool WB_RD = (SLL_COND | SLLV_COND | SRA_COND | SRAV_COND | SRL_COND | SRLV_COND | AND_COND | OR_COND | XOR_COND | NOR_COND | SLT_COND | SLTU_COND | ADDU_COND | SUBU_COND | MFHI_COND | MFLO_COND);  
  bool WB_RT = (SLTI_COND | SLTIU_COND | LW_COND | LBU_COND | LB_COND | LH_COND | LHU_COND | ORI_COND | ANDI_COND | XORI_COND | ADDIU_COND | LUI_COND); 
  bool WB_MEM_ADD = (SW_COND | SB_COND | SH_COND);

  WB_LOC = ((WB_TEMP_REG)? TEMP_REG : 0x0) | 
	   ((WB_RD)? *rd : 0x0) |
	   ((WB_RT)? *rt : 0x0) |
	   ((WB_MEM_ADD)? MEM_ADD : 0x0) |
#ifdef USE_MULT 
	   ((MULT_COND)? HI : 0x0) | /*MULT*/
#endif
	   ((JAL_COND)? 31 : 0x0);/*JAL*/

  /*syscall*/
#ifdef USE_SYS
  int SYSCALL_EXIT = get_value(2);
  *EMULATOR_STATUS = (SYS_COND & (SYSCALL_EXIT == 10)) ? FINISH : NORMAL;
#else
  *EMULATOR_STATUS = (SYS_COND & (RES_SUBLEQ == 10)) ? FINISH : NORMAL;
#endif 

#ifdef USE_MULT
  write_value (RES_MULT_LO, LO);
#endif
  write_value (RESULT, WB_LOC);
  *prog_count = RET_ADD;
#ifdef PRINT
  printf(" RET_ADD: %x\n", *prog_count << 2);
#endif
#if 0
  if (*opcode == 0x00 & *funct == 0x00 & instruction != 0x00){
    printf("Instruction: %8.8x\n", instruction);
  }
#endif
}


uint get_value(uint location){
  return MEM[location];
}

void write_value(int value, uint location){
  MEM[location] = value;
}


int NEW_ADD_prev (int a, int b){
  return a + b;
}

int NEW_ADD (int a, int b){
  return a + b;
}

int NEW_ADD2 (int a, int b){
  return (a+b) & 0xFFFF;
}

int NEW_ADD3 (int a, int b){
  return (a+b) & 0xFFFF;
}

uint SUB (uint a, uint b){
  return (a - b);
}

int SRA (int a, int b){
  uchar check = a >> 31;
  uint TEMP = (check == 1)? (a >> b) | (0xFFFFFFFF << (32 -a)): (a >> b);
  return TEMP;
}

int SRL (int a, int b){
  return (a >> b);
}

int SLL (int a, int b){
  return (a << b);
}

uint OR (uint a, uint b){
  return (a | b);
}

uint XOR (uint a, uint b){
  return (a ^ b);
}

uint AND (uint a, uint b){
  return (a & b);
}
int64_t MULT (int a, int b){
  return (a * b);
}

void subleq_machine(ushort prog_count) {
  while(prog_count != 999) {
    //fprintf(stderr, "subleq: %d: ", prog_count);
    ushort a = get_value (prog_count);
    ushort b = get_value (prog_count + 0x1);
    ushort c = get_value (prog_count + 0x2);
    //fprintf(stderr, "%d, %d, %d:", a, b, c);

    sint src1 = get_value (a);
    sint src2 = get_value (b);

    //fprintf(stderr, "subleq: %d, %d, %d: %d, %d -> %d\n", a, b, c, src1, src2, src2 - src1);
    src2 = src2 - src1;

    write_value (src2, b);
    prog_count = 0x7FF & ((src2 > 0) ? prog_count + 0x3 : c);
  }
}
