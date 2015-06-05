/* ----------------------------------------

* File Name : emulator.c

* Created on : 05-01-2015

* Last Modified on : Fri 05 Jun 2015 11:30:37 AM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include "../config/config.h"
#include "../config/listTrace.h"
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


#ifdef FAULT_ANALYZER
uint fault_value;
uint fault_singal;
uint signal_bit;
struct listTrace *headTrace;
struct listTrace *currTrace;
#endif


#ifdef FAULT_ANALYZER
//unsigned int MEM_COPY[MEM_SIZE];
//void grep_initial_memory_map(unsigned int*);
//void grep_fault_free_memory_map();
//void generate_fault_information();
//void generate_fault();
//void inject_fault();
//void out_of_mem_check();
//void fault_control();
#endif

#if 1
uint emulator (uint, bool);
void exec (uint, uchar*, uchar*, uchar*, uchar*, uchar*, ushort*, uchar*, uint*, uint*, bool, unsigned int);
uint get_value(uint);
void write_value(int, uint);
//void add ();
int add(int, int);
int add2(int, int);
int add3(int, int);
uint sub (uint, uint);
int sll (int, int);
int srl (int, int);
int sra (int, int);
uint or (uint, uint);
uint and (uint, uint);
uint xor (uint, uint);
int64_t mult (int, int);
void subleq_machine(ushort prog_count);
void printReg(void);
#endif

int main(int argc, char **argv){
  uint prog_count = 0x2000;
#ifdef FAULT_ANALYZER
  bool injectFault = false;
  createNewMem ();
#endif
  unsigned int status = emulator (prog_count >> 2, injectFault);//fault free run
  printf("Fault Free Execution");
  printReg();
#ifdef FAULT_ANALYZER
  //printTrace();
  //printf("%8.8x\n",headTrace);
  struct listTrace *tempHeadTrace = headTrace;
  updateMem ();
  headTrace = tempHeadTrace;
  //printTrace();
  injectFault = true;
  unsigned int statusDup = emulator (prog_count >> 2, injectFault);//fault injected
  printf("\nExecution After Fault Injection");
  printReg();
#endif
  return (status - statusDup);
}


uint emulator (uint prog_count, bool injectFault) {
  uint emulator_status = NORMAL;
  uint prog_count_1 = prog_count;
  uint instruction;

  uchar opcode;
  uchar funct;
  uchar rs;
  uchar rt;
  uchar rd;
  ushort imm;
  uchar sa;

  unsigned int cycleCount = 0;
  //if (injectFault)
    //printTrace();


  while (emulator_status == NORMAL){
    /*fetch instructions*/
    instruction = get_value (prog_count);//signal-1, 32-bit
/*
    if (injectFault){
      unsigned int error = 0x1;
      instruction = instruction | error; //for sa1
    }*/
    prog_count_1 = prog_count_1 + 1;//signal-2, 32-bit
    prog_count = prog_count_1;//signal-3,32-bit
    exec (instruction, &opcode, &funct, &rs, &rt, &rd, &imm, &sa, &prog_count_1, &emulator_status, injectFault, cycleCount);
    cycleCount++;
  }
  prog_count_1 = prog_count_1 & 0x1; 
  return prog_count_1;//emulator_status;
}

void exec (uint instruction, uchar *opcode, uchar *funct, uchar *rs, uchar *rt, uchar *rd, ushort *imm, uchar *sa, uint *prog_count, uint *emulator_status, bool injectFault, unsigned int cycleCount){
  /*decode instructions*/
  *opcode = instruction >> 26;//signal-4, 6-bit
  *rs = (instruction >> 21) & 0X0000001F;//signal-5, 6-bit
  *rt = (instruction >> 16) & 0X0000001F;//signal-6, 6-bit
  *rd = (instruction >> 11) & 0X0000001F;//signal-7, 6-bit
  *sa = (instruction >> 6) & 0X0000001F;//signal-8, 6-bit
  *funct = instruction & 0X0000003F;//signal-9, 6-bit
  *imm = instruction & 0xFFFF;//signal-10, 6-bit


  int src1 = get_value (*rs);//signal-11, 32-bit
  int src2 = get_value (*rt);//signal-12, 32-bit
  int result = 0;//add (src1, src2); /*addu*/
  uint ret_addr = *prog_count;// = *prog_count;//signal-13, 16-bit
  uint wb_loc = TEMP_REG;

  bool r_type = (*opcode == 0x00);//signal-14,bit
  bool nop_cond = (instruction == 0x0);//signal-15,bit
  bool sll_cond = (r_type & !nop_cond & (*funct == 0x00));//signal-16,bit
  bool srl_cond = (r_type & (*funct == 0x02));//signal-17,bit
  bool sra_cond = (r_type & (*funct == 0x03));//signal-18,bit
  bool sllv_cond = (r_type & (*funct == 0x04));//signal-19, bit
  bool srlv_cond = (r_type & (*funct == 0x06));//signal-20, bit
  bool srav_cond = (r_type & (*funct == 0x07));//signal-21, bit
  bool jr_cond = (r_type & (*funct == 0x08));//signal-22, bit
  bool sys_cond = (r_type & (*funct == 0x0C));//signal-23,bit
  bool mfhi_cond = (r_type & (*funct == 0x10));//signal-24,bit
  bool mflo_cond = (r_type & (*funct == 0x12));//signal-25,bit
  bool mult_cond = (r_type & (*funct == 0x18));//signal-26,bit
  //bool div_cond =
  bool addu_cond = (r_type & (*funct == 0x21));//signal-27,bit
  bool subu_cond = (r_type & (*funct == 0x23));//signal-28,bit
  bool and_cond = (r_type & (*funct == 0x24));//signal-29, bit
  bool or_cond = (r_type & (*funct == 0x25));//signal-30, bit
  bool xor_cond = (r_type & (*funct == 0x26));//signal-31,bit
  bool nor_cond = (r_type & (*funct == 0x27));//signal-32,bit
  bool slt_cond = (r_type & (*funct == 0x2A));//signal-33,bit
  bool sltu_cond = (r_type & (*funct == 0x2B));//signal-34-bit
  //
  bool j_cond = (*opcode == 0x02);//signal-35,bit
  bool jal_cond = (*opcode == 0x03);//signal-36,bit
  //
  bool bltz_cond = ((*opcode == 0x01) & (*rt == 0x0));//signal-37,bit
  bool bgez_cond = ((*opcode == 0x01) & (*rt == 0x1));//signal-38,bit
  bool beq_cond = ((*opcode == 0x04) & (*rt != 0x0));//signal-39,bit
  bool beqz_cond = ((*opcode == 0x04) & (*rt == 0x0));//signal-40,bit
  bool bne_cond = (*opcode == 0x05);//signal-41,bit
  bool blez_cond = (*opcode == 0x06);//signal-42,bit
  bool bgtz_cond = (*opcode == 0x07);//signal-43,bit
  bool addiu_cond = (*opcode == 0x09);//signal-44,bit
  bool slti_cond = (*opcode == 0x0A);//signal-45,bit
  bool sltiu_cond = (*opcode == 0x0B);//signal-46,bit
  bool andi_cond = (*opcode == 0x0C);//signal-47,bit
  bool ori_cond = (*opcode == 0x0D);//signal-48,bit
  bool xori_cond = (*opcode == 0x0E);//signal-49,bit
  bool lui_cond = (*opcode == 0x0F);//signal-50,bit
  bool lb_cond = (*opcode == 0x20);//signal-51,bit
  bool lh_cond = (*opcode == 0x21);//signal-52,bit
  bool lw_cond = (*opcode == 0x23);//signal-53,bit
  bool lbu_cond = (*opcode == 0x24);//signal-54,bit
  bool lhu_cond = (*opcode == 0x25);//signal-55,bit
  bool sb_cond = (*opcode == 0x28);//signal-56,bit
  bool sh_cond = (*opcode == 0x29);//signal-57,bit
  bool sw_cond = (*opcode == 0x2B);//signal-58,bit



  /*AND ANDI OR ORI XOR XORI*/
  bool logici_cond = andi_cond | ori_cond | xori_cond;//signal-59,bit 

  uint logici_inp = (logici_cond) ? (ushort) *imm : src2;//signal-60,32-bit
  uint res_and = and (src1, logici_inp);//signal-61,32-bit
  uint res_or = or (src1, logici_inp);//signal-62,32-bit
  uint res_xor = xor (src1, logici_inp);//signal-63,32-bit
  /*NOR*/
  uint res_nor = ~res_or;//signal-64,32-bit

  uint res_logic = ((and_cond | andi_cond) ? res_and : 0x0) |
		   ((or_cond | ori_cond) ? res_or : 0x0) |
		   ((xor_cond | xori_cond) ? res_xor : 0x0) |
		   ((nor_cond) ? res_nor : 0x0);//signal-65,32-bit

  if (injectFault){
    unsigned int error = 0x1;
    res_logic = res_logic | error; //for sa1
  }

  bool subleq_cond = false 
#ifndef USE_ADDER
		     | addu_cond | addiu_cond
#endif
#ifndef USE_SUB
		     | subu_cond 
#endif
#ifndef USE_MULT
		     | mult_cond 
#endif
#ifndef USE_MFHI
		     | mfhi_cond 
#endif
#ifndef USE_MFLO
		     | mflo_cond
#endif
#ifndef USE_SHIFTER
		     | sll_cond | sllv_cond | srl_cond | srlv_cond | sra_cond | srav_cond
#endif
#ifndef USE_SYS
		     | sys_cond
#endif
#ifndef USE_SET_LESS_THAN
		     | slt_cond | slti_cond | sltu_cond | sltiu_cond
#endif
		     ; 
  
  /*MFHI*//**opcode == 0x00 & *funct == 0x10*/
#ifdef USE_MFHI
  uint res_mfhi = get_value (HI);//signal-66,32-bit
#endif
#ifdef USE_MFLO
  uint res_mflo = get_value (LO);//signal-67,32-bit
#endif


#ifdef USE_MULT
  /*MULT*/ /**opcode == 0x00 & *funct == 0x18*/
  int64_t res_mult_temp = mult (src1, src2);
  int res_mult_lo = (mult_cond)? res_mult_temp & 0xFFFFFFFF : res_mflo;//signal-68,32-bit
  int res_mult_hi = (res_mult_temp >> 32);//signal-69,32-bit
#endif


#ifdef USE_SUB  
  /*SUBU*/ /**opcode == 0x00 & *funct == 0x23*/
  uint res_subu = sub ((uint) src1, (uint) src2);//signal-70,32-bit
#endif

  /*SRA*//*SRAV*//*SLL*//*SLLV*//*SRL*//*SRLV*/
  int shift_inp = (srav_cond | sllv_cond | srlv_cond) ? src1 : *sa;//signal-71,32-bit
#ifdef USE_SHIFTER
  int res_sra_v = sra (src2, shift_inp);//signal-72,32-bit
  int res_sllv = sll (src2, shift_inp);//signal-73,32-bit
  int res_srlv = srl (src2, shift_inp);//signal-74,32-bit
#endif

  int add_inp = (addiu_cond)? (sshort)*imm : src2;//signal-75,32-bit
#ifdef USE_ADDER
  /*addu addiu*/
  uint res_addu = add (src1, add_inp);//signal-76,32-bit
#endif  

  int slt_inp = (slti_cond | sltiu_cond) ? (sshort) *imm : src2;//signal-77,32-bit
#ifdef	USE_SET_LESS_THAN
  /*SLT*//*SLTI*/
  int res_slt_i = (src1 < slt_inp) ? 1 : 0;//signal-78,32-bit
  /*SLTU*//*SLTIU*/
  int res_sltu_i = ((uint) src1 < (uint) slt_inp) ? 1 : 0;//signal-79,32-bit
#endif



#if !defined(USE_ADDER) || !defined(USE_MULT) || !defined(USE_SUB) || !defined(USE_MFLO) || !defined(USE_MFHI) || !defined(USE_SHIFTER) || !defined(USE_SYS) || !defined(USE_SET_LESS_THAN)
  uint res_subleq = 0;
  uint routine_addr = 0x0
#ifndef	USE_ADDER
			     | ((addu_cond | addiu_cond) ? ADD_ROUTINE : 0x0)
#endif
#ifndef	USE_MULT
			     | ((mult_cond) ? MUL_ROUTINE : 0x0)
#endif
#ifndef USE_SUB
			     | ((subu_cond) ? SUB_ROUTINE : 0x0)
#endif
#ifndef	USE_MFLO
			     | ((mflo_cond) ? MFLO_ROUTINE : 0x0)
#endif
#ifndef	USE_MFHI
			     | ((mfhi_cond) ? MFHI_ROUTINE : 0x0)
#endif
#ifndef USE_SHIFTER
			     | ((sllv_cond | sll_cond) ? SLL_ROUTINE : 0x0)
			     | ((srlv_cond | srl_cond) ? SRL_ROUTINE : 0x0)
			     | ((srav_cond | sra_cond) ? SRA_ROUTINE : 0x0)
#endif
#ifndef	USE_SYS
			     | ((sys_cond) ? SYS_ROUTINE : 0x0)
#endif
#ifndef USE_SET_LESS_THAN
			     | ((slt_cond | slti_cond) ? SLT_ROUTINE : 0x0)
			     | ((sltu_cond | sltiu_cond) ? SLTU_ROUTINE : 0x0)
#endif
			     ;

  int subleq_inp1 = 0x0
#ifndef USE_ADDER
		   | ((addiu_cond | addu_cond) ? src1 : 0x0)
#endif
#ifndef USE_MULT
		   | ((mult_cond) ? src1 : 0x0)
#endif
#ifndef USE_SUB
		   | ((subu_cond) ? src1 : 0x0) 
#endif
#ifndef	USE_SHIFTER
		   | ((sll_cond | sllv_cond | srl_cond | srlv_cond | sra_cond | srav_cond) ? src2 : 0x0)
#endif
#ifndef USE_SYS
		   | ((sys_cond) ? src1 : 0x0)
#endif
#ifndef	USE_SET_LESS_THAN
		   | ((slt_cond | slti_cond | sltu_cond | sltiu_cond) ? src1 : 0x0)
#endif
		   ; 


  int subleq_inp2 = 0x0
#ifndef	USE_ADDER
		   | ((addiu_cond | addu_cond) ? add_inp : 0x0)
#endif
#ifndef	USE_MULT
		   | ((mult_cond) ? src2 : 0x0)
#endif
#ifndef USE_SUB
		   | ((subu_cond) ? src2 : 0x0)
#endif
#ifndef	USE_SHIFTER
		   | ((sll_cond | sllv_cond | srl_cond | srlv_cond | sra_cond | srav_cond) ? shift_inp : 0x0)
#endif
#ifndef USE_SYS
		   | ((sys_cond) ? src2 : 0x0)
#endif
#ifndef	USE_SET_LESS_THAN
		   | ((slt_cond | slti_cond | sltu_cond | sltiu_cond) ? slt_inp : 0x0)
#endif
		   ; 

  if (subleq_cond) {
    write_value (subleq_inp1, SRC1_LOC);
    write_value (subleq_inp2, SRC2_LOC);
    subleq_machine(routine_addr);
    res_subleq = get_value(DEST_LOC);
  }
#endif


  /*JR*/
  uint ret_addr_jr = add2 (src1, -1);//signal-80,16-bit

  /*jal, j*/
  uint res_jal = add2 (ret_addr, 1);//signal-81,16-bit
  uint ret_addr_j = add2 (((((*rs << 5) | *rt) << 16) | *imm), -1);//signal-82,16-bit


  /*BNE BEQZ BLEZ*/
  uint cond_br_addr = add2 (add2 (ret_addr, (sshort)*imm), -1);//signal-83,16-bit
  bool check_eq = (src1 == src2)? true : false;//signal-84,bit
  bool src_eq_0 = (src1 == 0) ? true : false;//signal-85,bit
  bool src_gt_0 = (src1 > 0) ? true : false;//signal-86,bit

  bool bne_cond_temp = (bne_cond & !check_eq);//signal-87,bit
  bool beq_cond_temp = (beq_cond & check_eq);//signal-88,bit
  bool beqz_cond_temp = (beqz_cond & (src_eq_0));//signal-89,bit
  bool bgez_cond_temp = (bgez_cond & (src_eq_0 | src_gt_0));//signal-90,bit
  bool blez_cond_temp = (blez_cond & (src_eq_0 | !src_gt_0));//signal-91,bit
  bool bltz_cond_temp = (bltz_cond & (!src_gt_0));//signal-92,bit
  bool bgtz_cond_temp = (bgtz_cond & (src_gt_0));//signal-93,bit

  bool br_cond_all = (bne_cond_temp | beq_cond_temp | beqz_cond_temp | bgez_cond_temp | blez_cond_temp | bltz_cond_temp | bgtz_cond_temp);//signal-94,bit

  uint ret_addr_cond_branch = (br_cond_all) ? cond_br_addr : ret_addr;//signal-94,16-bit

  /*lui*/
  int res_lui = (*imm << 16) | 0x0000;//signal-95,32-bit
  
  int mem_addr_temp = add3 (src1, (sshort) *imm);//signal-96,16-bit
  /*sw*/
  uint mem_addr = (mem_addr_temp >> 2);//signal-97,16-bit
  int res_sw = src2;//signal-98,32-bit
  /*LW*/
  int res_lw = get_value(mem_addr);//src2;//signal-99,32-bit
  /*SB*/
  uchar byte_check = mem_addr_temp & 0x2;//signal-100,2-bit

  uint res_sb0 = (res_lw & 0xFFFFFF00) | (0xFF & (src2 << 0));//signal-101,32-bit
  uint res_sb1 = (res_lw & 0xFFFF00FF) | (0xFF00 & (src2 << 8));//signal-102,32-bit
  uint res_sb2 = (res_lw & 0xFF00FFFF) | (0xFF0000 & (src2 << 16));//signal-103,32-bit
  uint res_sb3 = (res_lw & 0x00FFFFFF) | (0xFF000000 & (src2 << 24));//signal-104,32-bit

  uint res_sb = (byte_check >> 1) ? ((byte_check & 0x1)? res_sb3 : res_sb2) : ((byte_check & 0x1)? res_sb1 : res_sb0);//signal-105,32-bit

  /*LBU, LB*/
  uint res_lbu0 = (res_lw & 0xFF) >> 0;//signal-106,32-bit
  uint res_lbu1 = (res_lw & 0xFF00) >> 8;//signal-107,32-bit
  uint res_lbu2 = (res_lw & 0xFF0000) >> 16;//signal-108,32-bit
  uint res_lbu3 = (res_lw & 0xFF000000) >> 24;//signal-109,32-bit

  uint res_lbu = (byte_check >> 1) ? ((byte_check & 0x1)? res_lbu3 : res_lbu2) : ((byte_check & 0x1)? res_lbu1 : res_lbu0);//signal-110,32-bit

  /*SH*/
  uchar bit_check = mem_addr_temp & 0x1;//signal-111,1-bit
  uint res_sh0 = (res_lw & 0xFFFF0000) | (0xFFFF & (src2 << 0));//signal-112,32-bit
  uint res_sh1 = (res_lw & 0x0000FFFF) | (0xFFFF0000 & (src2 << 16));//signal-113,32-bit
  uint res_sh = (bit_check)? res_sh1 : res_sh0;//signal-114,32-bit

  /*LHU, LH*/
  uint res_lhu0 = (res_lw & 0xFFFF);//signal-115,32-bit
  uint res_lhu1 = (res_lw & 0xFFFF0000) >> 16;//signal-116,32-bit
  uint res_lhu = (bit_check)? res_lhu1 : res_lhu0;//signal-117,32-bit

  bool ret_addr_exp = (j_cond | jr_cond | jal_cond | bne_cond | beqz_cond | beq_cond | blez_cond | bltz_cond | bgez_cond | bgtz_cond);//signal-118,bit
  ret_addr = ((j_cond)? ret_addr_j : 0x0) |/*J*/
	    ((jr_cond)? ret_addr_jr : 0x0) |/*JR*/
	    ((jal_cond)? ret_addr_j : 0x0) |/*JAL*/
	    ((bne_cond | beqz_cond | beq_cond | blez_cond | bltz_cond | bgez_cond | bgtz_cond) ? ret_addr_cond_branch : 0x0) |
	    ((ret_addr_exp)? 0x0 : ret_addr);//signal-119,16-bit

  bool result_exp = (j_cond | jr_cond
#ifndef USE_SYS
		    | sys_cond 
#endif
		    | bne_cond | beqz_cond | blez_cond | bltz_cond | bgez_cond | bgtz_cond | nop_cond
#ifndef USE_MULT
		    | mult_cond
#endif
		    );//signal-120,bit

  result = (and_cond | andi_cond | or_cond | ori_cond | xor_cond | xori_cond | nor_cond) ? res_logic : 0x0 |
#ifdef USE_SET_LESS_THAN
	   ((slt_cond | slti_cond)? res_slt_i : 0x0) |/*SLT*//*SLTI*/
	   ((sltu_cond | sltiu_cond)? res_sltu_i : 0x0) |/*SLTU*/
#endif
	   ((lw_cond)? res_lw : 0x0) |/*LW*/
	   ((lbu_cond)? res_lbu : 0x0) | /*LBU*/
	   ((lb_cond)? (sint) res_lbu : 0x0) | /*LB*/
	   ((lh_cond)? (sint) res_lhu : 0x0) | /*LH*/
	   ((lhu_cond)? res_lhu : 0x0) | /*LHU*/
#if !defined(USE_ADDER) || !defined(USE_SUB) || !defined(USE_MULT) || !defined(USE_MFLO) || !defined(USE_MFHI) || !defined(USE_SHIFTER) || !defined(USE_SET_LESS_THAN) || !defined (USE_SYS)
	   ((subleq_cond)? res_subleq : 0x0) |/*ADDU*/
#endif
#ifdef USE_ADDER
	   ((addu_cond | addiu_cond)? res_addu : 0x0)|
	   //((addiu_cond)? RES_ADDIU : 0x0)|
#endif
#ifdef USE_SUB
	   ((subu_cond)? res_subu : 0x0) | /*SUBU*/
#endif
#ifdef USE_MULT
	   ((mult_cond)? res_mult_hi : 0x0) | /*MULT*/
#endif
#ifdef USE_MFHI
	   ((mfhi_cond)? res_mfhi : 0x0) | /*MFHI*/
#endif
#ifdef USE_MFLO
	   ((mflo_cond)? res_mflo : 0x0) | /*MFLO*/
#endif
#ifdef USE_SHIFTER
           ((sll_cond | sllv_cond)? res_sllv : 0x0) |/*SLL*/
	   ((sra_cond | srav_cond)? res_sra_v : 0x0) | /*SRA*//*SRAV*/
	   ((srl_cond | srlv_cond)? res_srlv : 0x0) | /*SRL*/
#endif
	   ((jal_cond)? res_jal : 0x0) |/*JAL*/
	   ((lui_cond)? res_lui : 0x0) |/*LUI*/
	   ((sw_cond)? res_sw : 0x0) |/*SW*/
	   ((sb_cond)? res_sb : 0x0) | /*SB*/
	   ((sh_cond)? res_sh : 0x0) | /*SH*/
	   ((result_exp)? 0x0 : 0x0);/*EXPECTION*///signal-121,32-bit



  bool wb_temp_reg = (j_cond | beq_cond | jr_cond | sys_cond | bne_cond | beqz_cond | blez_cond | bltz_cond | bgez_cond | bgtz_cond | nop_cond
#ifndef USE_MULT
		     | mult_cond
#endif
		     );//signal-122,bit
  bool wb_rd = (sll_cond | sllv_cond | sra_cond | srav_cond | srl_cond | srlv_cond | and_cond | or_cond | xor_cond | nor_cond | slt_cond | sltu_cond | addu_cond | subu_cond | mfhi_cond | mflo_cond);//signal-123,bit  
  bool wb_rt = (slti_cond | sltiu_cond | lw_cond | lbu_cond | lb_cond | lh_cond | lhu_cond | ori_cond | andi_cond | xori_cond | addiu_cond | lui_cond);//signal-123,bit 
  bool wb_mem_addr = (sw_cond | sb_cond | sh_cond);//signal-124,bit

  wb_loc = ((wb_temp_reg)? TEMP_REG : 0x0) | 
	   ((wb_rd)? *rd : 0x0) |
	   ((wb_rt)? *rt : 0x0) |
	   ((wb_mem_addr)? mem_addr : 0x0) |
#ifdef USE_MULT 
	   ((mult_cond)? HI : 0x0) | /*MULT*/
#endif
	   ((jal_cond)? 31 : 0x0);/*JAL*///sginal-125,16-bit

  /*syscall*/
#ifdef USE_SYS
  int syscall_exit = get_value(2);//signal-126,32-bit
  *emulator_status = (sys_cond & (syscall_exit == 10)) ? FINISH : NORMAL;//signal-127,32-bit
#else
  *emulator_status = (sys_cond & (res_subleq == 10)) ? FINISH : NORMAL;
#endif 

#ifdef USE_MULT
  write_value (res_mult_lo, LO);
#endif
  write_value (result, wb_loc);
  *prog_count = ret_addr;//signal-128,16-bit

  if (!injectFault) {
    unsigned int structLocation = 0;
    faultFreeTrace (cycleCount, *prog_count, instruction, result, wb_loc, &structLocation);
  }
  if (injectFault) {
    bool error = checkFault (cycleCount, *prog_count, instruction, result, wb_loc);
    if (error){
      *emulator_status = FINISH;
      printf ("Fault detected @ cycle %8.8x\n", cycleCount);
    }
    //compare the result
  }
}


uint get_value(uint location){
  return MEM[location];
}

void write_value(int value, uint location){
  MEM[location] = value;
}


int add_prev (int a, int b){
  return a + b;
}

int add (int a, int b){
  return a + b;
}

int add2 (int a, int b){
  return (a+b) & 0xFFFF;
}

int add3 (int a, int b){
  return (a+b) & 0xFFFF;
}

uint sub (uint a, uint b){
  return (a - b);
}

int sra (int a, int b){
  uchar check = a >> 31;
  uint TEMP = (check == 1)? (a >> b) | (0xFFFFFFFF << (32 -a)): (a >> b);
  return TEMP;
}

int srl (int a, int b){
  return (a >> b);
}

int sll (int a, int b){
  return (a << b);
}

uint or (uint a, uint b){
  return (a | b);
}

uint xor (uint a, uint b){
  return (a ^ b);
}

uint and (uint a, uint b){
  return (a & b);
}
int64_t mult (int a, int b){
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
void printReg(void){
  int i;
  for (i = 0; i < 32; i++){
    if ((i % 4) == 0)
      printf("\n");
    printf("%8.8x\t",get_value(i));
  }
  printf("\n");
}
