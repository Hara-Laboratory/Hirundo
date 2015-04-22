/* ----------------------------------------

* File Name : emulator.c

* Created on : 05-01-2015

* Last Modified on : Wed 22 Apr 2015 12:18:13 PM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include "../config/config.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "../benchmarks/adpcm.h"
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
//#include "../benchmarks/vec_add.h"


#ifdef FAULT_ANALYZER
uint fault_value;
uint fault_singal;
uint signal_bit;
#endif

uint emulator (uint);
void exec (uint, uchar*, uchar*, uchar*, uchar*, uchar*, ushort*, uchar*, uint*, uint*);
uint get_value(uint);
void write_value(int, uint);
//void add ();
int ADD(int, int);
int ADD2(int, int);
int ADD3(int, int);
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


  while (EMULATOR_STATUS == NORMAL){
    /*fetch instructions*/
    instruction = get_value (prog_count);//signal-1, 32-bit
    prog_count_1 = prog_count_1 + 1;//signal-2, 32-bit
    prog_count = prog_count_1;//signal-3,32-bit
    exec (instruction, &opcode, &funct, &rs, &rt, &rd, &imm, &sa, &prog_count_1, &EMULATOR_STATUS);
  }
  prog_count_1 = prog_count_1 & 0x1; 
  return prog_count_1;//EMULATOR_STATUS;
}

void exec (uint instruction, uchar *opcode, uchar *funct, uchar *rs, uchar *rt, uchar *rd, ushort *imm, uchar *sa, uint *prog_count, uint *EMULATOR_STATUS){
  /*decode instructions*/
  *opcode = instruction >> 26;//signal-4, 6-bit
  *rs = (instruction >> 21) & 0X0000001F;//signal-5, 6-bit
  *rt = (instruction >> 16) & 0X0000001F;//signal-6, 6-bit
  *rd = (instruction >> 11) & 0X0000001F;//signal-7, 6-bit
  *sa = (instruction >> 6) & 0X0000001F;//signal-8, 6-bit
  *funct = instruction & 0X0000003F;//signal-9, 6-bit
  *imm = instruction & 0xFFFF;//signal-10, 6-bit


  int SRC1 = get_value (*rs);//signal-11, 32-bit
  int SRC2 = get_value (*rt);//signal-12, 32-bit
  int RESULT = 0;//ADD (SRC1, SRC2); /*addu*/
  uint RET_ADD = *prog_count;// = *prog_count;//signal-13, 16-bit
  uint WB_LOC = TEMP_REG;

  bool R_TYPE = (*opcode == 0x00);//signal-14,bit
  bool NOP_COND = (instruction == 0x0);//signal-15,bit
  bool SLL_COND = (R_TYPE & !NOP_COND & (*funct == 0x00));//signal-16,bit
  bool SRL_COND = (R_TYPE & (*funct == 0x02));//signal-17,bit
  bool SRA_COND = (R_TYPE & (*funct == 0x03));//signal-18,bit
  bool SLLV_COND = (R_TYPE & (*funct == 0x04));//signal-19, bit
  bool SRLV_COND = (R_TYPE & (*funct == 0x06));//signal-20, bit
  bool SRAV_COND = (R_TYPE & (*funct == 0x07));//signal-21, bit
  bool JR_COND = (R_TYPE & (*funct == 0x08));//signal-22, bit
  bool SYS_COND = (R_TYPE & (*funct == 0x0C));//signal-23,bit
  bool MFHI_COND = (R_TYPE & (*funct == 0x10));//signal-24,bit
  bool MFLO_COND = (R_TYPE & (*funct == 0x12));//signal-25,bit
  bool MULT_COND = (R_TYPE & (*funct == 0x18));//signal-26,bit
  //bool DIV_COND =
  bool ADDU_COND = (R_TYPE & (*funct == 0x21));//signal-27,bit
  bool SUBU_COND = (R_TYPE & (*funct == 0x23));//signal-28,bit
  bool AND_COND = (R_TYPE & (*funct == 0x24));//signal-29, bit
  bool OR_COND = (R_TYPE & (*funct == 0x25));//signal-30, bit
  bool XOR_COND = (R_TYPE & (*funct == 0x26));//signal-31,bit
  bool NOR_COND = (R_TYPE & (*funct == 0x27));//signal-32,bit
  bool SLT_COND = (R_TYPE & (*funct == 0x2A));//signal-33,bit
  bool SLTU_COND = (R_TYPE & (*funct == 0x2B));//signal-34-bit
  //
  bool J_COND = (*opcode == 0x02);//signal-35,bit
  bool JAL_COND = (*opcode == 0x03);//signal-36,bit
  //
  bool BLTZ_COND = ((*opcode == 0x01) & (*rt == 0x0));//signal-37,bit
  bool BGEZ_COND = ((*opcode == 0x01) & (*rt == 0x1));//signal-38,bit
  bool BEQ_COND = ((*opcode == 0x04) & (*rt != 0x0));//signal-39,bit
  bool BEQZ_COND = ((*opcode == 0x04) & (*rt == 0x0));//signal-40,bit
  bool BNE_COND = (*opcode == 0x05);//signal-41,bit
  bool BLEZ_COND = (*opcode == 0x06);//signal-42,bit
  bool BGTZ_COND = (*opcode == 0x07);//signal-43,bit
  bool ADDIU_COND = (*opcode == 0x09);//signal-44,bit
  bool SLTI_COND = (*opcode == 0x0A);//signal-45,bit
  bool SLTIU_COND = (*opcode == 0x0B);//signal-46,bit
  bool ANDI_COND = (*opcode == 0x0C);//signal-47,bit
  bool ORI_COND = (*opcode == 0x0D);//signal-48,bit
  bool XORI_COND = (*opcode == 0x0E);//signal-49,bit
  bool LUI_COND = (*opcode == 0x0F);//signal-50,bit
  bool LB_COND = (*opcode == 0x20);//signal-51,bit
  bool LH_COND = (*opcode == 0x21);//signal-52,bit
  bool LW_COND = (*opcode == 0x23);//signal-53,bit
  bool LBU_COND = (*opcode == 0x24);//signal-54,bit
  bool LHU_COND = (*opcode == 0x25);//signal-55,bit
  bool SB_COND = (*opcode == 0x28);//signal-56,bit
  bool SH_COND = (*opcode == 0x29);//signal-57,bit
  bool SW_COND = (*opcode == 0x2B);//signal-58,bit



  /*AND ANDI OR ORI XOR XORI*/
  bool LOGIC_IMM_COND = ANDI_COND | ORI_COND | XORI_COND;//signal-59,bit 

  uint LOGIC_IMM_INP = (LOGIC_IMM_COND) ? (ushort) *imm : SRC2;//signal-60,32-bit
  uint RES_AND = AND (SRC1, LOGIC_IMM_INP);//signal-61,32-bit
  uint RES_OR = OR (SRC1, LOGIC_IMM_INP);//signal-62,32-bit
  uint RES_XOR = XOR (SRC1, LOGIC_IMM_INP);//signal-63,32-bit
  /*NOR*/
  uint RES_NOR = ~RES_OR;//signal-64,32-bit

  uint RES_LOGIC = ((AND_COND | ANDI_COND) ? RES_AND : 0x0) |
		   ((OR_COND | ORI_COND) ? RES_OR : 0x0) |
		   ((XOR_COND | XORI_COND) ? RES_XOR : 0x0) |
		   ((NOR_COND) ? RES_NOR : 0x0);//signal-65,32-bit

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
  uint RES_MFHI = get_value (HI);//signal-66,32-bit
#endif
#ifdef USE_MFLO
  uint RES_MFLO = get_value (LO);//signal-67,32-bit
#endif


#ifdef USE_MULT
  /*MULT*/ /**opcode == 0x00 & *funct == 0x18*/
  int64_t RES_MULT_TEMP = MULT (SRC1, SRC2);
  int RES_MULT_LO = (MULT_COND)? RES_MULT_TEMP & 0xFFFFFFFF : RES_MFLO;//signal-68,32-bit
  int RES_MULT_HI = (RES_MULT_TEMP >> 32);//signal-69,32-bit
#endif


#ifdef USE_SUB  
  /*SUBU*/ /**opcode == 0x00 & *funct == 0x23*/
  uint RES_SUBU = SUB ((uint) SRC1, (uint) SRC2);//signal-70,32-bit
#endif

  /*SRA*//*SRAV*//*SLL*//*SLLV*//*SRL*//*SRLV*/
  int SHIFT_INP = (SRAV_COND | SLLV_COND | SRLV_COND) ? SRC1 : *sa;//signal-71,32-bit
#ifdef USE_SHIFTER
  int RES_SRA_V = SRA (SRC2, SHIFT_INP);//signal-72,32-bit
  int RES_SLLV = SLL (SRC2, SHIFT_INP);//signal-73,32-bit
  int RES_SRLV = SRL (SRC2, SHIFT_INP);//signal-74,32-bit
#endif

  int ADD_INP = (ADDIU_COND)? (sshort)*imm : SRC2;//signal-75,32-bit
#ifdef USE_ADDER
  /*addu addiu*/
  uint RES_ADDU = ADD (SRC1, ADD_INP);//signal-76,32-bit
#endif  

  int SLT_INP = (SLTI_COND | SLTIU_COND) ? (sshort) *imm : SRC2;//signal-77,32-bit
#ifdef	USE_SET_LESS_THAN
  /*SLT*//*SLTI*/
  int RES_SLT_I = (SRC1 < SLT_INP) ? 1 : 0;//signal-78,32-bit
  /*SLTU*//*SLTIU*/
  int RES_SLTU_I = ((uint) SRC1 < (uint) SLT_INP) ? 1 : 0;//signal-79,32-bit
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
#endif
		   ; 

  if (SUBLEQ_COND) {
    write_value (SUBLEQ_INP1, SRC1_LOC);
    write_value (SUBLEQ_INP2, SRC2_LOC);
    subleq_machine(ROUTINE_ADD);
    RES_SUBLEQ = get_value(DEST_LOC);
  }
#endif


  /*JR*/
  uint RET_ADD_JR = ADD2 (SRC1, -1);//signal-80,16-bit

  /*jal, j*/
  uint RES_JAL = ADD2 (RET_ADD, 1);//signal-81,16-bit
  uint RET_ADD_J = ADD2 (((((*rs << 5) | *rt) << 16) | *imm), -1);//signal-82,16-bit


  /*BNE BEQZ BLEZ*/
  uint COND_BR_ADD = ADD2 (ADD2 (RET_ADD, (sshort)*imm), -1);//signal-83,16-bit
  bool CHECK_EQ = (SRC1 == SRC2)? true : false;//signal-84,bit
  bool SRC_EQ_0 = (SRC1 == 0) ? true : false;//signal-85,bit
  bool SRC_GT_0 = (SRC1 > 0) ? true : false;//signal-86,bit

  bool BNE_COND_TEMP = (BNE_COND & !CHECK_EQ);//signal-87,bit
  bool BEQ_COND_TEMP = (BEQ_COND & CHECK_EQ);//signal-88,bit
  bool BEQZ_COND_TEMP = (BEQZ_COND & (SRC_EQ_0));//signal-89,bit
  bool BGEZ_COND_TEMP = (BGEZ_COND & (SRC_EQ_0 | SRC_GT_0));//signal-90,bit
  bool BLEZ_COND_TEMP = (BLEZ_COND & (SRC_EQ_0 | !SRC_GT_0));//signal-91,bit
  bool BLTZ_COND_TEMP = (BLTZ_COND & (!SRC_GT_0));//signal-92,bit
  bool BGTZ_COND_TEMP = (BGTZ_COND & (SRC_GT_0));//signal-93,bit

  bool BR_COND_ALL = (BNE_COND_TEMP | BEQ_COND_TEMP | BEQZ_COND_TEMP | BGEZ_COND_TEMP | BLEZ_COND_TEMP | BLTZ_COND_TEMP | BGTZ_COND_TEMP);//signal-94,bit

  uint RET_ADD_COND_BRANCH = (BR_COND_ALL) ? COND_BR_ADD : RET_ADD;//signal-94,16-bit

  /*lui*/
  int RES_LUI = (*imm << 16) | 0x0000;//signal-95,32-bit
  
  int MEM_ADD_TEMP = ADD3 (SRC1, (sshort) *imm);//signal-96,16-bit
  /*sw*/
  uint MEM_ADD = (MEM_ADD_TEMP >> 2);//signal-97,16-bit
  int RES_SW = SRC2;//signal-98,32-bit
  /*LW*/
  int RES_LW = get_value(MEM_ADD);//SRC2;//signal-99,32-bit
  /*SB*/
  uchar BYTE_CHECK = MEM_ADD_TEMP & 0x2;//signal-100,2-bit

  uint RES_SB0 = (RES_LW & 0xFFFFFF00) | (0xFF & (SRC2 << 0));//signal-101,32-bit
  uint RES_SB1 = (RES_LW & 0xFFFF00FF) | (0xFF00 & (SRC2 << 8));//signal-102,32-bit
  uint RES_SB2 = (RES_LW & 0xFF00FFFF) | (0xFF0000 & (SRC2 << 16));//signal-103,32-bit
  uint RES_SB3 = (RES_LW & 0x00FFFFFF) | (0xFF000000 & (SRC2 << 24));//signal-104,32-bit

  uint RES_SB = (BYTE_CHECK >> 1) ? ((BYTE_CHECK & 0x1)? RES_SB3 : RES_SB2) : ((BYTE_CHECK & 0x1)? RES_SB1 : RES_SB0);//signal-105,32-bit

  /*LBU, LB*/
  uint RES_LBU0 = (RES_LW & 0xFF) >> 0;//signal-106,32-bit
  uint RES_LBU1 = (RES_LW & 0xFF00) >> 8;//signal-107,32-bit
  uint RES_LBU2 = (RES_LW & 0xFF0000) >> 16;//signal-108,32-bit
  uint RES_LBU3 = (RES_LW & 0xFF000000) >> 24;//signal-109,32-bit

  uint RES_LBU = (BYTE_CHECK >> 1) ? ((BYTE_CHECK & 0x1)? RES_LBU3 : RES_LBU2) : ((BYTE_CHECK & 0x1)? RES_LBU1 : RES_LBU0);//signal-110,32-bit

  /*SH*/
  uchar BIT_CHECK = MEM_ADD_TEMP & 0x1;//signal-111,1-bit
  uint RES_SH0 = (RES_LW & 0xFFFF0000) | (0xFFFF & (SRC2 << 0));//signal-112,32-bit
  uint RES_SH1 = (RES_LW & 0x0000FFFF) | (0xFFFF0000 & (SRC2 << 16));//signal-113,32-bit
  uint RES_SH = (BIT_CHECK)? RES_SH1 : RES_SH0;//signal-114,32-bit

  /*LHU, LH*/
  uint RES_LHU0 = (RES_LW & 0xFFFF);//signal-115,32-bit
  uint RES_LHU1 = (RES_LW & 0xFFFF0000) >> 16;//signal-116,32-bit
  uint RES_LHU = (BIT_CHECK)? RES_LHU1 : RES_LHU0;//signal-117,32-bit

  bool RET_ADD_EXP = (J_COND | JR_COND | JAL_COND | BNE_COND | BEQZ_COND | BEQ_COND | BLEZ_COND | BLTZ_COND | BGEZ_COND | BGTZ_COND);//signal-118,bit
  RET_ADD = ((J_COND)? RET_ADD_J : 0x0) |/*J*/
	    ((JR_COND)? RET_ADD_JR : 0x0) |/*JR*/
	    ((JAL_COND)? RET_ADD_J : 0x0) |/*JAL*/
	    ((BNE_COND | BEQZ_COND | BEQ_COND | BLEZ_COND | BLTZ_COND | BGEZ_COND | BGTZ_COND) ? RET_ADD_COND_BRANCH : 0x0) |
	    ((RET_ADD_EXP)? 0x0 : RET_ADD);//signal-119,16-bit

  bool RESULT_EXP = (J_COND | JR_COND
#ifndef USE_SYS
		    | SYS_COND 
#endif
		    | BNE_COND | BEQZ_COND | BLEZ_COND | BLTZ_COND | BGEZ_COND | BGTZ_COND | NOP_COND
#ifndef USE_MULT
		    | MULT_COND
#endif
		    );//signal-120,bit

  RESULT = (AND_COND | ANDI_COND | OR_COND | ORI_COND | XOR_COND | XORI_COND | NOR_COND) ? RES_LOGIC : 0x0 |
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
	   ((RESULT_EXP)? 0x0 : 0x0);/*EXPECTION*///signal-121,32-bit



  bool WB_TEMP_REG = (J_COND | BEQZ_COND | JR_COND | SYS_COND | BNE_COND | BEQZ_COND | BLEZ_COND | BLTZ_COND | BGEZ_COND | BGTZ_COND | NOP_COND
#ifndef USE_MULT
		     | MULT_COND
#endif
		     );//signal-122,bit
  bool WB_RD = (SLL_COND | SLLV_COND | SRA_COND | SRAV_COND | SRL_COND | SRLV_COND | AND_COND | OR_COND | XOR_COND | NOR_COND | SLT_COND | SLTU_COND | ADDU_COND | SUBU_COND | MFHI_COND | MFLO_COND);//signal-123,bit  
  bool WB_RT = (SLTI_COND | SLTIU_COND | LW_COND | LBU_COND | LB_COND | LH_COND | LHU_COND | ORI_COND | ANDI_COND | XORI_COND | ADDIU_COND | LUI_COND);//signal-123,bit 
  bool WB_MEM_ADD = (SW_COND | SB_COND | SH_COND);//signal-124,bit

  WB_LOC = ((WB_TEMP_REG)? TEMP_REG : 0x0) | 
	   ((WB_RD)? *rd : 0x0) |
	   ((WB_RT)? *rt : 0x0) |
	   ((WB_MEM_ADD)? MEM_ADD : 0x0) |
#ifdef USE_MULT 
	   ((MULT_COND)? HI : 0x0) | /*MULT*/
#endif
	   ((JAL_COND)? 31 : 0x0);/*JAL*///sginal-125,16-bit

  /*syscall*/
#ifdef USE_SYS
  int SYSCALL_EXIT = get_value(2);//signal-126,32-bit
  *EMULATOR_STATUS = (SYS_COND & (SYSCALL_EXIT == 10)) ? FINISH : NORMAL;//signal-127,32-bit
#else
  *EMULATOR_STATUS = (SYS_COND & (RES_SUBLEQ == 10)) ? FINISH : NORMAL;
#endif 

#ifdef USE_MULT
  write_value (RES_MULT_LO, LO);
#endif
  write_value (RESULT, WB_LOC);
  *prog_count = RET_ADD;//signal-128,16-bit
}


uint get_value(uint location){
  return MEM[location];
}

void write_value(int value, uint location){
  MEM[location] = value;
}


int ADD_prev (int a, int b){
  return a + b;
}

int ADD (int a, int b){
  return a + b;
}

int ADD2 (int a, int b){
  return (a+b) & 0xFFFF;
}

int ADD3 (int a, int b){
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
