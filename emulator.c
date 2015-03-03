/* ----------------------------------------

* File Name : emulator.c

* Created on : 05-01-2015

* Last Modified on : Tue Mar  3 09:52:27 2015

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "bs.h"
//#include "isort.h"
//#include "fibcall.h"
//#include "bubble.h"
//#include "bf.h"
//#include "crc.h"
//#include "adpcm.h"
//#include "intmm.h"
//#include "jfdctint.h"
//#include "string_search.h"
//#include "gsm.h"
//#include "matmul.h"
//#include "mpeg.h"


uint emulator (unsigned int);
void exec (uint, uchar*, uchar*, uchar*, uchar*, uchar*, ushort*, uchar*, uint*, unsigned int*);
unsigned int get_value(unsigned int);
void write_value(int, unsigned int);
void add ();
int NEW_ADD(int, int);
int NEW_ADD2(int, int);
int SUB (int, int);
int SLL (int, int);
int SRL (int, int);
int SRA (int, int);
unsigned int OR (unsigned int, unsigned int);
unsigned int AND (unsigned int, unsigned int);
unsigned int XOR (unsigned int, unsigned int);
long MULT (int, int);
void subleq_machine(unsigned int prog_count);


int main(int argc, char **argv){
  int i;
  for (i = 0; i < 15; i++) {
    write_value(ADD[i], ADD_ROUTINE + i);
  }

  unsigned int prog_count = 0x100;
  unsigned int STATUS = emulator(prog_count >> 2);
#if 1
  printf("========\nREG FILE\n========\n");
  for (i = 0; i < 32; i++){
    printf("%d\t",get_value(i));
    if (((i+1) % 4) == 0)
      printf("\n");
  }
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
  return STATUS;
}


uint emulator (unsigned int prog_count){
  uint EMULATOR_STATUS = NORMAL;
  unsigned int prog_count_1 = prog_count;
  unsigned int instruction;

  write_value(0x000F000, 28);
  write_value(0x0006000, 29);

  uchar opcode;
  uchar funct;
  uchar rs;
  uchar rt;
  uchar rd;
  ushort imm;
  uchar sa;

  unsigned int test = 0;
  unsigned int i = 1;

  TEMP4 = Z;
  write_value(0, Z);
  
  while (EMULATOR_STATUS == NORMAL){
    /*fetch instructions*/
    printf("%d: %3.3x, INST: %8.8x", i, prog_count << 2, get_value(prog_count));
    instruction = get_value (prog_count);
    prog_count_1 = prog_count_1 + 1;
    prog_count = prog_count_1;
    exec (instruction, &opcode, &funct, &rs, &rt, &rd, &imm, &sa, &prog_count_1, &EMULATOR_STATUS);
    i++;
  }
  prog_count_1 = prog_count_1 & 0x1; 
  return prog_count_1;//EMULATOR_STATUS;
}

void exec (uint instruction, uchar *opcode, uchar *funct, uchar *rs, uchar *rt, uchar *rd, ushort *imm, uchar *sa, uint *prog_count, unsigned int *EMULATOR_STATUS){
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
  //int SRC3 = get_value (*rd);
  int RESULT = 0;//NEW_ADD (SRC1, SRC2); /*addu*/
  unsigned int RET_ADD = *prog_count;// = *prog_count;
  unsigned int ST_LOC = TEMP_REG;
  unsigned int WB_LOC = TEMP_REG;

  bool R_TYPE = (*opcode == 0x00);
  bool NOP_COND = (instruction == 0x0);
  bool SLL_COND = (R_TYPE & !NOP_COND & *funct == 0x00);
  bool SRL_COND = (R_TYPE & *funct == 0x02);
  bool SRA_COND = (R_TYPE & *funct == 0x03);
  bool SLLV_COND = (R_TYPE & *funct == 0x04);
  bool SRLV_COND = (R_TYPE & *funct == 0x06);
  bool SRAV_COND = (R_TYPE & *funct == 0x07);
  bool JR_COND = (R_TYPE & *funct == 0x08);
  bool SYS_COND = (R_TYPE & *funct == 0x0C);
  bool MFHI_COND = (R_TYPE & *funct == 0x10);
  bool MFLO_COND = (R_TYPE & *funct == 0x12);
  bool MULT_COND = (R_TYPE & *funct == 0x18);
  //bool DIV_COND =
  bool ADDU_COND = (R_TYPE & *funct == 0x21);
  bool SUBU_COND = (R_TYPE & *funct == 0x23);
  bool AND_COND = (R_TYPE & *funct == 0x24);
  bool OR_COND = (R_TYPE & *funct == 0x25);
  bool XOR_COND = (R_TYPE & *funct == 0x26);
  bool NOR_COND = (R_TYPE & *funct == 0x27);
  bool SLT_COND = (R_TYPE & *funct == 0x2A);
  bool SLTU_COND = (R_TYPE & *funct == 0x2B);
  //
  bool J_COND = (*opcode == 0x02);
  bool JAL_COND = (*opcode == 0x03);
  //
  bool BLTZ_COND = (*opcode == 0x01 & *rt == 0x0);
  bool BGEZ_COND = (*opcode == 0x01 & *rt == 0x1);
  bool BEQ_COND = (*opcode == 0x04 & *rt != 0x0);
  bool BEQZ_COND = (*opcode == 0x04 & *rt == 0x0);
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

  /*XORI*/
  unsigned int RES_XORI = XOR (SRC1, (unsigned short)*imm);
  /*XOR*/
  unsigned int RES_XOR = XOR (SRC1, SRC2);

  /*OR*/
  unsigned int RES_OR = OR (SRC1, SRC2);
  /*ORI*/ /**opcode == 0x0D*/
  unsigned int RES_ORI = OR (SRC1, (unsigned short)*imm);
  /*NOR*/
  unsigned int RES_NOR = ~RES_OR;

  /*MFHI*//**opcode == 0x00 & *funct == 0x10*/
  unsigned int RES_MFHI = get_value (HI);
  unsigned int RES_MFLO = get_value (LO);

  /*MULT*/ /**opcode == 0x00 & *funct == 0x18*/
  long RES_MULT_TEMP = MULT (SRC1, SRC2);
  int RES_MULT_LO = (*opcode == 0x00 & *funct == 0x18)? RES_MULT_TEMP & 0xFFFFFFFF : RES_MFLO;
  int RES_MULT_HI = (RES_MULT_TEMP >> 32);
  /*if mult ==  true
 * prepear for subleq
 * use_subleq = true*/
  
  /*SUBU*/ /**opcode == 0x00 & *funct == 0x23*/
  int RES_SUBU = SUB (SRC1, SRC2);

  /*ANDI*/ /**opcode == 0x0C*/
  unsigned int RES_ANDI = AND (SRC1, (unsigned short)*imm);
  /*AND*/
  unsigned int RES_AND = AND (SRC1, SRC2);

  /*SRA*//*SRAV*/
  int SRA_INP = ((SRA_COND) ? *sa : 0x0) |
                ((SRAV_COND) ? SRC1 : 0x0);
  int RES_SRA_V = SRA (SRC2, SRA_INP);

  /*SLL*/
  int RES_SLL = SLL (SRC2, *sa);
  /*SLLV*/
  int RES_SLLV = SLL (SRC2, SRC1);

  /*SRL*/
  int RES_SRL = SRL (SRC2, *sa);

  /*SRLV*/
  int RES_SRLV = SRL (SRC2, SRC1);

  /*SLT*/
  int RES_SLT = (SRC1 < SRC2)? 1 : 0;

  /*SLTI*/
  int RES_SLTI = (SRC1 < (signed short)*imm) ? 1 : 0;

  /*SLTU*/
  int RES_SLTU = ((unsigned int) SRC1 < (unsigned int) SRC2)? 1 : 0;

  /*SLTIU*/
  int RES_SLTIU = (SRC1 < (unsigned short)*imm) ? 1 : 0;

  /*JR*/
  unsigned int RET_ADD_JR = NEW_ADD2 (SRC1, -1);

  /*jal, j*/
  unsigned int RES_JAL = NEW_ADD2 (RET_ADD, 1);
  unsigned int RET_ADD_J = NEW_ADD2 (((((*rs << 5) | *rt) << 16) | *imm), -1);

  /*BNE BEQZ BLEZ*/
  unsigned int COND_BR_ADD = NEW_ADD2 (NEW_ADD2 (RET_ADD, (signed short)*imm), -1);
  bool CHECK_EQ = (SRC1 == SRC2)? true : false;
  unsigned int RET_ADD_BNE = (!CHECK_EQ) ? COND_BR_ADD : RET_ADD;
  unsigned int RET_ADD_BEQ = (CHECK_EQ) ? COND_BR_ADD : RET_ADD;

  //unsigned int RET_ADD_BNE = (SRC1 != SRC2) ? COND_BR_ADD : RET_ADD;
  //unsigned int RET_ADD_BEQ = (SRC1 == SRC2)? COND_BR_ADD : RET_ADD;

  unsigned int RET_ADD_BEQZ = (SRC1 == 0)? COND_BR_ADD : RET_ADD;
  unsigned int RET_ADD_BGEZ = (SRC1 >= 0)? COND_BR_ADD : RET_ADD;
  unsigned int RET_ADD_BLEZ = (SRC1 <= 0)? COND_BR_ADD : RET_ADD;
  unsigned int RET_ADD_BLTZ = (SRC1 < 0)? COND_BR_ADD : RET_ADD;
  unsigned int RET_ADD_BGTZ = (SRC1 > 0)? COND_BR_ADD : RET_ADD;

  /*addiu*/
  //int RES_ADDIU = NEW_ADD (SRC1, (signed short) *imm);
  /*addu*/
  // unsigned int RES_ADDU = NEW_ADD (SRC1, SRC2);
  //
  
  write_value ((signed short) *imm, TEMP_REG);
  TEMP0 = *rs;
  TEMP1 = (ADDU_COND) ? *rt : 0x0 |
	  (ADDIU_COND) ? TEMP_REG : 0x0;
  TEMP2 = (ADDU_COND) ? *rd : 0x0 |
	  (ADDIU_COND) ? *rt : 0x0;

  unsigned int ROUTINE_ADD = (ADDU_COND | ADDIU_COND) ? ADD_ROUTINE : 0x0;
  if (ADDU_COND | ADDIU_COND) {
    //printf("Use Subleq\n");
    //printf("addu $%d, $%d, $%d ; %d + %d\n", *rd, *rt, *rs, get_value(*rt), get_value(*rs));
    subleq_machine(ROUTINE_ADD);
    //printf("%d + %d = %d\n", get_value(*rt), get_value(*rs), get_value(*rd));
  }


  /*lui*/
  int RES_LUI = (*imm << 16) | 0x0000;
  

  int RES_ADDIU = NEW_ADD (SRC1, (signed short) *imm);

  /*sw*/
  unsigned int MEM_ADD = ((RES_ADDIU & 0x7FFF) >> 2);
  int RES_SW = SRC2;
  /*LW*/
  int RES_LW = get_value(MEM_ADD);//SRC2;
  /*SB*/
  unsigned char BYTE_CHECK = RES_ADDIU & 0x3;

  unsigned int RES_SB0 = (RES_LW & 0xFFFFFF00) | (0xFF & (SRC2 << 0));
  unsigned int RES_SB1 = (RES_LW & 0xFFFF00FF) | (0xFF00 & (SRC2 << 0));
  unsigned int RES_SB2 = (RES_LW & 0xFF00FFFF) | (0xFF0000 & (SRC2 << 0));
  unsigned int RES_SB3 = (RES_LW & 0x00FFFFFF) | (0xFF000000 & (SRC2 << 0));


  unsigned char BIT_CHECK = RES_ADDIU & 0x1;
  unsigned int RES_SH0 = (RES_LW & 0xFFFF0000) | (0xFFFF & (SRC2 << 0));
  unsigned int RES_SH1 = (RES_LW & 0x0000FFFF) | (0xFFFF0000 & (SRC2 << 16));
  
  /*LBU, LB*/
  unsigned int RES_LBU0 = (RES_LW & 0xFF) >> 0;
  unsigned int RES_LBU1 = (RES_LW & 0xFF00) >> 8;
  unsigned int RES_LBU2 = (RES_LW & 0xFF0000) >> 16;
  unsigned int RES_LBU3 = (RES_LW & 0xFF000000) >> 24;


  /*LHU, LH*/
  unsigned int RES_LHU0 = (RES_LW & 0xFFFF);
  unsigned int RES_LHU1 = (RES_LW & 0xFFFF0000) >> 16;



  /*subleq machine*/
  /*
 *   if subeq == true
 *   use subleq
 *
 */

  bool RET_ADD_EXP = (J_COND | JR_COND | JAL_COND | BNE_COND | BEQZ_COND | BEQ_COND | BLEZ_COND | BLTZ_COND | BGEZ_COND | BGTZ_COND);
  RET_ADD = ((J_COND)? RET_ADD_J : 0x0) |/*J*/
	    ((JR_COND)? RET_ADD_JR : 0x0) |/*JR*/
	    ((JAL_COND)? RET_ADD_J : 0x0) |/*JAL*/
	    ((BNE_COND)? RET_ADD_BNE : 0x0) |/*BNE*/
	    ((BEQZ_COND)? RET_ADD_BEQZ : 0x0) |/*BEQZ*/
	    ((BEQ_COND)? RET_ADD_BEQ : 0x0) |/*BEQ*/
	    ((BLEZ_COND)? RET_ADD_BLEZ : 0x0) |/*BLEZ*/
	    ((BLTZ_COND)? RET_ADD_BLTZ : 0x0) |/*BLTZ*/
	    ((BGEZ_COND)? RET_ADD_BGEZ : 0x0) |/*BGEZ*/
	    ((BGTZ_COND)? RET_ADD_BGTZ : 0x0) |/*BGTZ*/
	    ((RET_ADD_EXP)? 0x0 : RET_ADD);

  bool RESULT_EXP = (J_COND | JR_COND | SYS_COND | BNE_COND | BEQZ_COND | BLEZ_COND | BLTZ_COND | BGEZ_COND | BGTZ_COND | ADDU_COND | NOP_COND | ADDIU_COND);

  RESULT = ((SLL_COND)? RES_SLL : 0x0) |/*SLL*/
	   ((SLLV_COND)? RES_SLLV : 0x0) |/*SLLV*/
	   ((SRA_COND)? RES_SRA_V : 0x0) | /*SRA*/
	   ((SRAV_COND)? RES_SRA_V : 0x0) | /*SRAV*/
	   ((SRL_COND)? RES_SRL : 0x0) | /*SRL*/
	   ((SRLV_COND)? RES_SRLV : 0x0) | /*SRLV*/
	   ((AND_COND)? RES_AND : 0x0) | /*AND*/
	   ((OR_COND)? RES_OR : 0x0) | /*OR*/
	   ((XOR_COND)? RES_XOR : 0x0) | /*XOR*/
	   ((NOR_COND)? RES_NOR : 0x0) | /*NOR*/
	   ((SLT_COND)? RES_SLT : 0x0) |/*SLT*/
	   ((SLTU_COND)? RES_SLTU : 0x0) |/*SLTU*/
	   ((SLTI_COND)? RES_SLTI : 0x0) |/*SLTI*/
	   ((SLTIU_COND)? RES_SLTIU : 0x0) |/*SLTIU*/
	   ((LW_COND)? RES_LW : 0x0) |/*LW*/
	   ((LBU_COND & BYTE_CHECK == 0x0)? RES_LBU0 : 0x0) |/*LBU*/
	   ((LBU_COND & BYTE_CHECK == 0x1)? RES_LBU1 : 0x0) |/*LBU*/
	   ((LBU_COND & BYTE_CHECK == 0x2)? RES_LBU2 : 0x0) |/*LBU*/
	   ((LBU_COND & BYTE_CHECK == 0x3)? RES_LBU2 : 0x0) |/*LBU*/
	   ((LB_COND & BYTE_CHECK == 0x0)? (signed int) RES_LBU0 : 0x0) |/*LB*/
	   ((LB_COND & BYTE_CHECK == 0x1)? (signed int) RES_LBU1 : 0x0) |/*LB*/
	   ((LB_COND & BYTE_CHECK == 0x2)? (signed int) RES_LBU2 : 0x0) |/*LB*/
	   ((LB_COND & BYTE_CHECK == 0x3)? (signed int) RES_LBU2 : 0x0) |/*LB*/
	   ((LH_COND & BIT_CHECK == 0x0)? (signed int) RES_LHU0 : 0x0) |/*LH*/
	   ((LH_COND & BIT_CHECK == 0x1)? (signed int) RES_LHU1 : 0x0) |/*LH*/
	   ((LHU_COND & BIT_CHECK == 0x0)? RES_LHU0 : 0x0) |/*LHU*/
	   ((LHU_COND & BIT_CHECK == 0x1)? RES_LHU1 : 0x0) |/*LHU*/
	   ((ORI_COND)? RES_ORI : 0x0) | /*ORI*/
	   ((ANDI_COND)? RES_ANDI : 0x0) | /*ANDI*/
	   ((XORI_COND)? RES_XORI : 0x0) | /*XORI*/
	   // ((ADDU_COND)? RES_ADDU : 0x0) |/*ADDU*/
	   ((SUBU_COND)? RES_SUBU : 0x0) | /*SUBU*/
	   ((MULT_COND)? RES_MULT_HI : 0x0) | /*MULT*/
	   ((MFHI_COND)? RES_MFHI : 0x0) | /*MFHI*/
	   ((MFLO_COND)? RES_MFLO : 0x0) | /*MFLO*/
	   ((JAL_COND)? RES_JAL : 0x0) |/*JAL*/
	   //((ADDIU_COND)? RES_ADDIU : 0x0) |/*ADDIU*/
	   ((LUI_COND)? RES_LUI : 0x0) |/*LUI*/
	   ((SW_COND)? RES_SW : 0x0) |/*SW*/
	   ((SB_COND & BYTE_CHECK == 0x0)? RES_SB0 : 0x0) |/*SB*/
	   ((SB_COND & BYTE_CHECK == 0x1)? RES_SB1 : 0x0) |/*SB*/
	   ((SB_COND & BYTE_CHECK == 0x2)? RES_SB2 : 0x0) |/*SB*/
	   ((SB_COND & BYTE_CHECK == 0x3)? RES_SB3 : 0x0) |/*SB*/
	   ((SH_COND & BIT_CHECK == 0x0)? RES_SH0 : 0x0) |/*SH*/
	   ((SH_COND & BIT_CHECK == 0x1)? RES_SH1 : 0x0) | /*SH*/
	   ((RESULT_EXP)? 0x0 : 0x0);/*EXPECTION*/



  bool WB_TEMP_REG = (J_COND | BEQZ_COND | JR_COND | SYS_COND | BNE_COND | BEQZ_COND | BLEZ_COND | BLTZ_COND | BGEZ_COND | BGTZ_COND | ADDU_COND | NOP_COND | ADDIU_COND);
  bool WB_RD = (SLL_COND | SLLV_COND | SRA_COND | SRAV_COND | SRL_COND | SRLV_COND | AND_COND | OR_COND | XOR_COND | NOR_COND | SLT_COND | SLTU_COND | SUBU_COND | MFHI_COND | MFLO_COND);  
  bool WB_RT = (SLTI_COND | SLTIU_COND | LW_COND | LBU_COND | LB_COND | LH_COND | LHU_COND | ORI_COND | ANDI_COND | XORI_COND /*| ADDIU_COND */| LUI_COND); 
  bool WB_MEM_ADD = (SW_COND | SB_COND | SH_COND);

  WB_LOC = ((WB_TEMP_REG)? TEMP_REG : 0x0) | 
	   ((WB_RD)? *rd : 0x0) |
	   ((WB_RT)? *rt : 0x0) |
	   ((WB_MEM_ADD)? MEM_ADD : 0x0) | 
	   ((MULT_COND)? HI : 0x0) | /*MULT*/
	   ((JAL_COND)? 31 : 0x0);/*JAL*/
#if 0
  WB_LOC = ((SLL_COND)? *rd : 0x0) | /*SLL*/
	   ((SLLV_COND)? *rd : 0x0) | /*SLLV*/
	   ((SRA_COND)? *rd : 0x0) | /*SRA*/
	   ((SRAV_COND)? *rd : 0x0) | /*SRAV*/
	   ((SRL_COND)? *rd : 0x0) | /*SRL*/
	   ((SRLV_COND)? *rd : 0x0) | /*SRLV*/
	   ((AND_COND)? *rd : 0x0) | /*AND*/
	   ((OR_COND)? *rd : 0x0) | /*OR*/
	   ((XOR_COND)? *rd : 0x0) | /*XOR*/
	   ((NOR_COND)? *rd : 0x0) | /*NOR*/
	   ((SLT_COND)? *rd : 0x0) | /*SLT*/
	   ((SLTU_COND)? *rd : 0x0) | /*SLTU*/
	   ((J_COND)? TEMP_REG : 0x0) | /*J*/
	   ((BEQZ_COND)? TEMP_REG : 0x0) | /*BEQZ*/
	   ((SLTI_COND)? *rt : 0x0) | /*SLTI*/
	   ((SLTIU_COND)? *rt : 0x0) | /*SLTIU*/
	   ((LW_COND)? *rt : 0x0) | /*LW*/
	   ((LBU_COND)? *rt : 0x0) | /*LBU*/
	   ((LB_COND)? *rt : 0x0) | /*LB*/
	   ((LH_COND)? *rt : 0x0) | /*LH*/
	   ((LHU_COND)? *rt : 0x0) | /*LHU*/
	   ((ORI_COND)? *rt : 0x0) | /*ORI*/
	   ((ANDI_COND)? *rt : 0x0) | /*ANDI*/
	   ((XORI_COND)? *rt : 0x0) | /*XORI*/
	   ((JR_COND)? TEMP_REG : 0x0) | /*JR*/
	   ((SYS_COND)? TEMP_REG : 0x0) | /*SYSCALL*/
	   ((ADDU_COND)? *rd : 0x0) | /*ADDU*/
	   ((SUBU_COND)? *rd : 0x0) | /*SUBU*/
	   ((MULT_COND)? HI : 0x0) | /*MULT*/
	   ((MFHI_COND)? *rd : 0x0) | /*MFHI*/
	   ((MFLO_COND)? *rd : 0x0) | /*MFLO*/
	   ((JAL_COND)? 31 : 0x0) | /*JAL*/
	   ((BNE_COND)? TEMP_REG : 0x0) | /*BNE*/
	   ((BEQZ_COND)? TEMP_REG : 0x0) | /*BEQZ*/
	   ((BLEZ_COND)? TEMP_REG : 0x0) | /*BLEZ*/
	   ((BLTZ_COND)? TEMP_REG : 0x0) | /*BLTZ*/
	   ((BGEZ_COND)? TEMP_REG : 0x0) | /*BGEZ*/
	   ((BGTZ_COND)? TEMP_REG : 0x0) | /*BGTZ*/
	   ((ADDIU_COND)? *rt : 0x0) | /*ADDIU*/
	   ((LUI_COND)? *rt : 0x0) | /*LUI*/
	   ((SW_COND)? MEM_ADD : 0x0) | /*SW*/
	   ((SB_COND)? MEM_ADD : 0x0) | /*SB*/
	   ((SH_COND)? MEM_ADD : 0x0) | /*SH*/
	   ((NOP_COND)? TEMP_REG : 0x0); /*NOP*/
#endif



#if 0
  WB_LOC = ((SLL_COND)? *rd : 0x0) | /*SLL*/
	   ((*opcode == 0x00 & *funct == 0x04)? *rd : 0x0) | /*SLLV*/
	   ((*opcode == 0x00 & *funct == 0x03)? *rd : 0x0) | /*SRA*/
	   ((*opcode == 0x00 & *funct == 0x07)? *rd : 0x0) | /*SRAV*/
	   ((*opcode == 0x00 & *funct == 0x02)? *rd : 0x0) | /*SRL*/
	   ((*opcode == 0x00 & *funct == 0x06)? *rd : 0x0) | /*SRLV*/
	   ((*opcode == 0x00 & *funct == 0x24)? *rd : 0x0) | /*AND*/
	   ((*opcode == 0x00 & *funct == 0x25)? *rd : 0x0) | /*OR*/
	   ((*opcode == 0x00 & *funct == 0x26)? *rd : 0x0) | /*XOR*/
	   ((*opcode == 0x00 & *funct == 0x27)? *rd : 0x0) | /*NOR*/
	   ((*opcode == 0x00 & *funct == 0x2A)? *rd : 0x0) | /*SLT*/
	   ((*opcode == 0x00 & *funct == 0x2B)? *rd : 0x0) | /*SLTU*/
	   ((*opcode == 0x02)? TEMP_REG : 0x0) | /*J*/
	   ((*opcode == 0x04)? TEMP_REG : 0x0) | /*BEQZ*/
	   ((*opcode == 0x0A)? *rt : 0x0) | /*SLTI*/
	   ((*opcode == 0x0B)? *rt : 0x0) | /*SLTIU*/
	   ((*opcode == 0x23)? *rt : 0x0) | /*LW*/
	   ((*opcode == 0x24)? *rt : 0x0) | /*LBU*/
	   ((*opcode == 0x20)? *rt : 0x0) | /*LB*/
	   ((*opcode == 0x21)? *rt : 0x0) | /*LH*/
	   ((*opcode == 0x25)? *rt : 0x0) | /*LHU*/
	   ((*opcode == 0x0D)? *rt : 0x0) | /*ORI*/
	   ((*opcode == 0x0C)? *rt : 0x0) | /*ANDI*/
	   ((*opcode == 0x0E)? *rt : 0x0) | /*XORI*/
	   ((*opcode == 0x00 & *funct == 0x08)? TEMP_REG : 0x0) | /*JR*/
	   ((*opcode == 0x00 & *funct == 0x0C)? TEMP_REG : 0x0) | /*SYSCALL*/
	   ((*opcode == 0x00 & *funct == 0x21)? *rd : 0x0) | /*ADDU*/
	   ((*opcode == 0x00 & *funct == 0x23)? *rd : 0x0) | /*SUBU*/
	   ((*opcode == 0x00 & *funct == 0x18)? HI : 0x0) | /*MULT*/
	   ((*opcode == 0x00 & *funct == 0x10)? *rd : 0x0) | /*MFHI*/
	   ((*opcode == 0x00 & *funct == 0x12)? *rd : 0x0) | /*MFLO*/
	   ((*opcode == 0x03)? 31 : 0x0) | /*JAL*/
	   ((*opcode == 0x05)? TEMP_REG : 0x0) | /*BNE*/
	   ((*opcode == 0x04)? TEMP_REG : 0x0) | /*BEQZ*/
	   ((*opcode == 0x06)? TEMP_REG : 0x0) | /*BLEZ*/
	   ((*opcode == 0x01 & *rt == 0x0)? TEMP_REG : 0x0) | /*BLTZ*/
	   ((*opcode == 0x01 & *rt == 0x1)? TEMP_REG : 0x0) | /*BGEZ*/
	   ((*opcode == 0x07)? TEMP_REG : 0x0) | /*BGTZ*/
	   ((*opcode == 0x09)? *rt : 0x0) | /*ADDIU*/
	   ((*opcode == 0x0F)? *rt : 0x0) | /*LUI*/
	   ((*opcode == 0x2B)? MEM_ADD : 0x0) | /*SW*/
	   ((*opcode == 0x28)? MEM_ADD : 0x0) | /*SB*/
	   ((*opcode == 0x29)? MEM_ADD : 0x0) | /*SH*/
	   ((NOP_COND)? TEMP_REG : 0x0); /*NOP*/
#endif

  /*syscall*/
  int SYSCALL_EXIT = get_value(2);
  *EMULATOR_STATUS = (*opcode == 0x00 & *funct == 0x0C & SYSCALL_EXIT == 10) ? FINISH : NORMAL;

  write_value (RES_MULT_LO, LO);
  write_value (RESULT, WB_LOC);
  *prog_count = RET_ADD;
  printf(" RET_ADD: %x\n", *prog_count << 2);
#if 0
  if (*opcode == 0x00 & *funct == 0x00 & instruction != 0x00){
    printf("Instruction: %8.8x\n", instruction);
  }
#endif
}


unsigned int get_value(unsigned int location){
  return MEM[location];
}

void write_value(int value, unsigned int location){
  MEM[location] = value;
}


int NEW_ADD_prev (int a, int b){
  return a + b;
}

int NEW_ADD (int a, int b){
  return a + b;
}

int NEW_ADD2 (int a, int b){
  return (a+b) & 0x1FFF;
}

int SUB (int a, int b){
  return (a -b);
}

int SRA (int a, int b){
  unsigned char check = a >> 31;
  unsigned int TEMP = (check == 1)? (a >> b) | (0xFFFFFFFF << (32 -a)): (a >> b);
  return TEMP;
}

int SRL (int a, int b){
  return (a >> b);
}

int SLL (int a, int b){
  return (a << b);
}

unsigned int OR (unsigned int a, unsigned int b){
  return (a | b);
}

unsigned int XOR (unsigned int a, unsigned int b){
  return (a ^ b);
}

unsigned int AND (unsigned int a, unsigned int b){
  return (a & b);
}
long MULT (int a, int b){
  return (a * b);
}

// I assume each routine takes two arguments in Y and Z, and the return value will be stored in LO.
unsigned int subleq_machine_int(unsigned int prog_count, unsigned int arg1, unsigned long arg2) {
  write_value(arg1, Y);
  write_value(arg2, Z);
  while (true) {
    unsigned int argA = get_value (prog_count);
    unsigned int argB = get_value (prog_count + 1);
    unsigned int argC = get_value (prog_count + 2);

    int valA = get_value (argA);
    int valB = get_value (argB);
    int result = valB - valA;

    write_value(result, argB);
    if (result <= 0)
      return get_value(LO);
  }
}

// I assume each routine takes two arguments in Y and Z, and the return values will be stored in HI and LO.
void subleq_machine(unsigned int prog_count) {
  while(prog_count != (-1)){
    unsigned int a = get_value (prog_count);
    unsigned int b = get_value (prog_count + 0x1);
    unsigned int c = get_value (prog_count + 0x2);

    unsigned int locA = ((a == 0) ? TEMP0 : 0 ) |
                        ((a == 1) ? TEMP1 : 0 ) |
                        ((a == 2) ? TEMP2 : 0 ) |
                        ((a == 3) ? TEMP3 : 0 ) |
                        ((a == 4) ? TEMP4 : 0 ) |
                        ((a == 5) ? TEMP5 : 0 ) |
                        ((a == 6) ? TEMP6 : 0 ) |
                        ((a == 7) ? TEMP7 : 0 );

    unsigned int locB = ((b == 0) ? TEMP0 : 0 ) |
                        ((b == 1) ? TEMP1 : 0 ) |
                        ((b == 2) ? TEMP2 : 0 ) |
                        ((b == 3) ? TEMP3 : 0 ) |
                        ((b == 4) ? TEMP4 : 0 ) |
                        ((b == 5) ? TEMP5 : 0 ) |
                        ((b == 6) ? TEMP6 : 0 ) |
                        ((b == 7) ? TEMP7 : 0 );

    signed int src1 = get_value (locA);
    signed int src2 = get_value (locB);

    //printf("subleq: %d, %d, %d: %d, %d: %d, %d -> %d\n", a, b, c, locA, locB, src1, src2, src2 - src1);
    src2 = src2 - src1;

    write_value (src2, locB);
    prog_count = (src2 > 0) ? prog_count + 0x3 : c;
  }
}
// vim: set noet fenc=utf-8 ff=unix sts=0 sw=2 ts=8 :
