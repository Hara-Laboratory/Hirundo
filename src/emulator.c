/* ----------------------------------------

* File Name : emulator.c

* Created on : 05-01-2015

* Last Modified on : Fri 04 Sep 2015 09:41:39 AM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include "../config/config.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdint.h>

//#include "../adpcm.h"
//#include "../bf.h"
//#include "../bs.h"
//#include "../bubble.h"
//#include "../crc.h"
//#include "../fibcall.h"
//#include "../gsm.h"
//#include "../intmm.h"
//#include "../isort.h"
//#include "../jfdctint.h"
//#include "../mpeg.h"
//#include "../vec_add.h"


//#include "../adpcm_e.h"
//#include "../bf_e.h"
//#include "../bs_e.h"
//#include "../bubble_e.h"
//#include "../crc_e.h"
//#include "../fibcall_e.h"
//#include "../gsm_e.h"
//#include "../intmm_e.h"
//#include "../isort_e.h"
//#include "../jfdctint_e.h"
#include "../mpeg_e.h"
//#include "../vec_add_e.h"





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
//#include "../benchmarks/vec_add.h"

#ifdef PROFILE
int addition = 0;
int subtraction = 0;
int shift = 0;
int logic = 0;
int multiplication = 0;
int slt_u = 0;
int jump = 0;
int load = 0;
int store = 0;
int branch = 0;
#endif


int64_t nosi = 0;
//unsigned int nosi = 0;

unsigned int bitReversal (unsigned int);// {
uint emulator (uint);
void exec (uint, uchar*, uchar*, uchar*, uchar*, uchar*, ushort*, uchar*, uint*, uint*);
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
void extended_subleq_machine (unsigned int prog_count);


int main(int argc, char **argv){
  uint prog_count = 0x2000;
  uint status = emulator(prog_count >> 2);
#ifdef PRINT
  int i;
  printf("========\nREG FILE\n========\n");
  for (i = 0; i < 32; i++){
    printf("%d\t",get_value(i));
    if (((i+1) % 4) == 0)
      printf("\n");
  }
#endif
  printf("Total Number of Subleq Instruction: %lld\n", (long long)nosi);

#ifdef PROFILE
  printf("\n=====================\nInstruction Profiling\n=====================\n");
  printf("Addition (add, addu, addi, addiu): %d\n", addition);
  printf("Subtraction (sub, subu): %d\n", subtraction);
  printf("Shift (sll, sllv, srl, srlv, sra, srav): %d\n", shift);
  printf("Logic (and, andi, or, ori, xor, xori, nor): %d\n", logic);
  printf("Multiplication (mult, multu, mflo, mfhi, mtlo, mthi): %d\n", multiplication);
  printf("Set-less-than (slt, slti. sltu, sltui ...): %d\n", slt_u);
  printf("Jump (j, jal, ...): %d\n", jump);
  printf("Load (lw, lh, lhu, lb, lbu): %d\n", load);
  printf("Store (sw, sh, sb): %d\n", store);
  printf("Conditional branch (beq, beqz, ble, ....): %d\n", branch);
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
  return status;
}


uint emulator (uint prog_count){
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

#ifdef PRINT
  uint i = 0;
#endif

  while (emulator_status == NORMAL){
    /*fetch instructions*/
#ifdef PRINT
    printf("%d: %3.3x, INST: %8.8x", i, prog_count << 2, get_value(prog_count));
#endif
    instruction = get_value (prog_count);
    prog_count_1 = prog_count_1 + 1;
    prog_count = prog_count_1;
    exec (instruction, &opcode, &funct, &rs, &rt, &rd, &imm, &sa, &prog_count_1, &emulator_status);
#ifdef PRINT
    i++;
#endif
  }
  prog_count_1 = prog_count_1 & 0x1; 
  return prog_count_1;//emulator_status;
}

void exec (uint instruction, uchar *opcode, uchar *funct, uchar *rs, uchar *rt, uchar *rd, ushort *imm, uchar *sa, uint *prog_count, uint *emulator_status){
  /*decode instructions*/
  *opcode = instruction >> 26;
  *rs = (instruction >> 21) & 0X0000001F;
  *rt = (instruction >> 16) & 0X0000001F;
  *rd = (instruction >> 11) & 0X0000001F;
  *sa = (instruction >> 6) & 0X0000001F;
  *funct = instruction & 0X0000003F;
  *imm = instruction & 0xFFFF;


  int src1 = get_value (*rs);
  int src2 = get_value (*rt);
  int result = 0;//add (src1, src2); /*addu*/
  uint ret_addr = *prog_count;// = *prog_count;
  uint wb_loc = TEMP_REG;

  bool r_type = (*opcode == 0x00);
  bool nop_cond = (instruction == 0x0);
  bool sll_cond = (r_type & !nop_cond & (*funct == 0x00));
  bool srl_cond = (r_type & (*funct == 0x02));
  bool sra_cond = (r_type & (*funct == 0x03));
  bool sllv_cond = (r_type & (*funct == 0x04));
  bool srlV_COND = (r_type & (*funct == 0x06));
  bool srav_cond = (r_type & (*funct == 0x07));
  bool jr_cond = (r_type & (*funct == 0x08));
  bool sys_cond = (r_type & (*funct == 0x0C));
  bool mfhi_cond = (r_type & (*funct == 0x10));
  bool mflo_cond = (r_type & (*funct == 0x12));
  bool mult_cond = (r_type & (*funct == 0x18));
  //bool div_cond =
  bool addu_cond = (r_type & (*funct == 0x21));
  bool subu_cond = (r_type & (*funct == 0x23));
  bool and_cond = (r_type & (*funct == 0x24));
  bool or_cond = (r_type & (*funct == 0x25));
  bool xor_cond = (r_type & (*funct == 0x26));
  bool nor_cond = (r_type & (*funct == 0x27));
  bool slt_cond = (r_type & (*funct == 0x2A));
  bool sltu_cond = (r_type & (*funct == 0x2B));
  //
  bool j_cond = (*opcode == 0x02);
  bool jal_cond = (*opcode == 0x03);
  //
  bool bltz_cond = ((*opcode == 0x01) & (*rt == 0x0));
  bool bgez_cond = ((*opcode == 0x01) & (*rt == 0x1));
  bool beq_cond = ((*opcode == 0x04) & (*rt != 0x0));
  bool beqz_cond = ((*opcode == 0x04) & (*rt == 0x0));
  bool bne_cond = (*opcode == 0x05);
  bool blez_cond = (*opcode == 0x06);
  bool bgtz_cond = (*opcode == 0x07);
  bool addiu_cond = (*opcode == 0x09);
  bool slti_cond = (*opcode == 0x0A);
  bool sltiu_cond = (*opcode == 0x0B);
  bool andi_cond = (*opcode == 0x0C);
  bool ori_cond = (*opcode == 0x0D);
  bool xori_cond = (*opcode == 0x0E);
  bool lui_cond = (*opcode == 0x0F);
  bool lb_cond = (*opcode == 0x20);
  bool lh_cond = (*opcode == 0x21);
  bool lw_cond = (*opcode == 0x23);
  bool lbu_cond = (*opcode == 0x24);
  bool lhu_cond = (*opcode == 0x25);
  bool sb_cond = (*opcode == 0x28);
  bool sh_cond = (*opcode == 0x29);
  bool sw_cond = (*opcode == 0x2B);


#ifdef SUBLEQ_COUNT
  if (jr_cond) nosi += 5;
  if (mfhi_cond) nosi += 4;
  if (mflo_cond) nosi += 4;
  if (addu_cond) nosi += 5;
  if (subu_cond) nosi += 7;
  if (nor_cond); 
  if (j_cond) nosi += 1;
  if (jal_cond) nosi += 4;
  if (bltz_cond) nosi += 2;
  if (bgez_cond) nosi += 2;
  if (beq_cond) {if ((src2 - src1) > 0) nosi += 4; else nosi += 7;}
  if (beqz_cond) {if (src1 > 0) nosi += 2; else if (src1 < 0) nosi += 3; else if (src1 == 0) nosi += 4;}
  if (bne_cond) {if ((src2 - src1) > 0) nosi += 4; else nosi += 7;}
  if (blez_cond) nosi += 1;
  if (bgtz_cond) nosi += 2;
  if (addiu_cond) nosi += 5;
  if (lui_cond) {write_value (*imm, SRC1_LOC); write_value (16, SRC2_LOC); extended_subleq_machine(SLL_ROUTINE);}
  if (lb_cond) {
    unsigned int mem_addr_temp_temp = add3 (src1, (sshort) *imm);
    nosi += 5;

    write_value (mem_addr_temp_temp, SRC1_LOC);
    write_value (2, SRC1_LOC);
    extended_subleq_machine (SRL_ROUTINE);
    //unsigned int final_addr_temp = get_value (DEST_LOC);
    //unsigned int final_value_temp = get_value (final_addr_temp);
    nosi += 4;
  }
  if (lh_cond) {
    int mem_addr_temp_temp = add3 (src1, (sshort) *imm);
    nosi += 5;
    write_value (mem_addr_temp_temp, SRC1_LOC);
    write_value (2, SRC1_LOC);
    extended_subleq_machine (SRL_ROUTINE);
    //unsigned int final_addr_temp = get_value (DEST_LOC);
    //unsigned int final_value_temp = get_value (final_addr_temp);
    nosi += 4;
  }
  if (lw_cond) {/*DONE*/
    int mem_addr_temp_temp = add3 (src1, (sshort) *imm);
    nosi += 5;
    write_value (mem_addr_temp_temp, SRC1_LOC);
    write_value (2, SRC1_LOC);
    extended_subleq_machine (SRL_ROUTINE);
    nosi += 4;
  }
  if (lbu_cond) {
    int mem_addr_temp_temp = add3 (src1, (sshort) *imm);
    nosi += 5;
    write_value (mem_addr_temp_temp, SRC1_LOC);
    write_value (2, SRC1_LOC);
    extended_subleq_machine (SRL_ROUTINE);
    //unsigned int final_addr_temp = get_value (DEST_LOC);
    //unsigned int final_value_temp = get_value (final_addr_temp);
    nosi += 4;
  }
  if (lhu_cond) {
    int mem_addr_temp_temp = add3 (src1, (sshort) *imm);
    nosi += 5;
    write_value (mem_addr_temp_temp, SRC1_LOC);
    write_value (2, SRC1_LOC);
    extended_subleq_machine (SRL_ROUTINE);
    //unsigned int final_addr_temp = get_value (DEST_LOC);
    //unsigned int final_value_temp = get_value (final_addr_temp);
    nosi += 4;
  }
  if (sb_cond) {
    int mem_addr_temp_temp = add3 (src1, (sshort) *imm);
    nosi += 5;
    write_value (mem_addr_temp_temp, SRC1_LOC);
    write_value (2, SRC1_LOC);
    extended_subleq_machine (SRL_ROUTINE);
    //unsigned int final_addr_temp = get_value (DEST_LOC);
    //unsigned int final_value_temp = get_value (final_addr_temp);
    nosi += 4;
  }
  if (sh_cond) {
    int mem_addr_temp_temp = add3 (src1, (sshort) *imm);
    nosi += 5;
    write_value (mem_addr_temp_temp, SRC1_LOC);
    write_value (2, SRC1_LOC);
    extended_subleq_machine (SRL_ROUTINE);
    //unsigned int final_addr_temp = get_value (DEST_LOC);
    //unsigned int final_value_temp = get_value (final_addr_temp);
    nosi += 4;
  }
  if (sw_cond) {/*DONE*/
    int mem_addr_temp_temp = add3 (src1, (sshort) *imm);
    nosi += 5;
    write_value (mem_addr_temp_temp, SRC1_LOC);
    write_value (2, SRC1_LOC);
    extended_subleq_machine (SRL_ROUTINE);
    nosi += 4;
  }
#endif


#ifdef PROFILE
  if (addu_cond | addiu_cond) addition++;
  if (subu_cond) subtraction++;
  if (and_cond | andi_cond | or_cond | ori_cond | xor_cond | xori_cond | nor_cond) logic++;
  if (sll_cond | sllv_cond | srl_cond | srlV_COND | sra_cond | srav_cond) shift++;
  if (mult_cond | mfhi_cond | mflo_cond) multiplication++;
  if (slt_cond | sltu_cond | slti_cond | sltiu_cond) slt_u++;
  if (jr_cond | j_cond | sys_cond | jal_cond) jump++;
  if (lb_cond | lh_cond | lw_cond | lbu_cond | lhu_cond) load++;
  if (sb_cond | sh_cond | sw_cond) store++;
  if (bltz_cond | bgez_cond | beq_cond | beqz_cond | bne_cond | blez_cond | bgtz_cond) branch++;
#endif


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
		     | sll_cond | sllv_cond | srl_cond | srlV_COND | sra_cond | srav_cond
#endif
#ifndef USE_SYS
		     | sys_cond
#endif
#ifndef USE_SET_LESS_THAN
		     | slt_cond | slti_cond | sltu_cond | sltiu_cond
#endif
#ifndef	USE_LOGIC_UNIT
		     | and_cond | andi_cond | or_cond | ori_cond | xor_cond | xori_cond | nor_cond
#endif
		     ; 

  /*and andI or orI xor xorI*/
  bool logici_cond = andi_cond | ori_cond | xori_cond; 

  uint logici_inp = (logici_cond) ? (ushort) *imm : src2;
#ifdef	USE_LOGIC_UNIT  
  uint res_and = and (src1, logici_inp);
  uint res_or = or (src1, logici_inp);
  uint res_xor = xor (src1, logici_inp);
  /*Nor*/
  uint res_nor = ~res_or;

  uint res_logic = ((and_cond | andi_cond) ? res_and : 0x0) |
		   ((or_cond | ori_cond) ? res_or : 0x0) |
		   ((xor_cond | xori_cond) ? res_xor : 0x0) |
		   ((nor_cond) ? res_nor : 0x0);
#endif


  /*MFHI*//**opcode == 0x00 & *funct == 0x10*/
#ifdef USE_MFHI
  uint res_mfhi = get_value (HI);
#endif
#ifdef USE_MFLO
  uint res_mflo = get_value (LO);
#endif


#ifdef USE_MULT
  /*MULT*/ /**opcode == 0x00 & *funct == 0x18*/
  int64_t res_mult_temp = mult (src1, src2);
  int res_mult_lo = (mult_cond)? res_mult_temp & 0xFFFFFFFF : res_mflo;
  int res_mult_hi = (res_mult_temp >> 32);
#endif


#ifdef USE_SUB  
  /*SUBU*/ /**opcode == 0x00 & *funct == 0x23*/
  uint res_subu = sub ((uint) src1, (uint) src2);
#endif

  /*sra*//*sraV*//*sll*//*sllV*//*srl*//*srlV*/
  int shift_inp = (srav_cond | sllv_cond | srlV_COND) ? src1 : *sa;
#ifdef USE_SHIFTER
  int res_srav = sra (src2, shift_inp);
  int res_sllv = sll (src2, shift_inp);
  int res_srlv = srl (src2, shift_inp);
#endif

  int add_inp = (addiu_cond)? (sshort)*imm : src2;
#ifdef USE_ADDER
  /*addu addiu*/
  uint res_addu = add (src1, add_inp);
#endif  

  int slt_inp = (slti_cond | sltiu_cond) ? (sshort) *imm : src2;
#ifdef	USE_SET_LESS_THAN
  /*SLT*//*SLTI*/
  int res_slt_i = (src1 < slt_inp) ? 1 : 0;
  /*SLTU*//*SLTIU*/
  int res_sltu_i = ((uint) src1 < (uint) slt_inp) ? 1 : 0;
#endif



#if !defined(USE_ADDER) || !defined(USE_MULT) || !defined(USE_SUB) || !defined(USE_MFLO) || !defined(USE_MFHI) || !defined(USE_SHIFTER) || !defined(USE_SYS) || !defined(USE_SET_LESS_THAN) || !defined(USE_LOGIC_UNIT)
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
			     | ((srlV_COND | srl_cond) ? SRL_ROUTINE : 0x0)
			     | ((srav_cond | sra_cond) ? SRA_ROUTINE : 0x0)
#endif
#ifndef	USE_SYS
			     | ((sys_cond) ? SYS_ROUTINE : 0x0)
#endif
#ifndef USE_SET_LESS_THAN
			     | ((slt_cond | slti_cond) ? SLT_ROUTINE : 0x0)
			     | ((sltu_cond | sltiu_cond) ? SLTU_ROUTINE : 0x0)
#endif
#ifndef USE_LOGIC_UNIT
			     | ((and_cond | andi_cond) ? AND_ROUTINE : 0x0)
			     | ((or_cond | ori_cond) ? OR_ROUTINE : 0x0)
			     | ((xor_cond | xori_cond) ? XOR_ROUTINE : 0x0)
			     | ((nor_cond) ? NOR_ROUTINE : 0x0)
#endif
			     ;

  int subleq_src1 = 0x0
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
		   | ((sll_cond | sllv_cond | srl_cond | srlV_COND | sra_cond | srav_cond) ? src2 : 0x0)
#endif
#ifndef USE_SYS
		   | ((sys_cond) ? src1 : 0x0)
#endif
#ifndef	USE_SET_LESS_THAN
		   | ((slt_cond | slti_cond | sltu_cond | sltiu_cond) ? src1 : 0x0)
#endif
#ifndef	USE_LOGIC_UNIT
		   | ((and_cond | andi_cond | or_cond | ori_cond | xor_cond | xori_cond | nor_cond) ? src1 : 0x0)
#endif
		   ; 


  int subleq_src2 = 0x0
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
		   | ((sll_cond | sllv_cond | srl_cond | srlV_COND | sra_cond | srav_cond) ? shift_inp : 0x0)
#endif
#ifndef USE_SYS
		   | ((sys_cond) ? src2 : 0x0)
#endif
#ifndef	USE_SET_LESS_THAN
		   | ((slt_cond | slti_cond | sltu_cond | sltiu_cond) ? slt_inp : 0x0)
		   /*| ((sltu_cond | sltiu_cond) ? slt_inp : 0x0)*/
#endif
#ifndef	USE_LOGIC_UNIT
		   | ((and_cond | andi_cond | or_cond | ori_cond | xor_cond | xori_cond | nor_cond) ? logici_inp : 0x0)
#endif
		   ; 

  if (subleq_cond) {
    write_value (subleq_src1, SRC1_LOC);
    write_value (subleq_src2, SRC2_LOC);
    //subleq_machine(routine_addr);
    extended_subleq_machine(routine_addr);
    res_subleq = get_value(DEST_LOC);
  }
#endif


#ifdef DEBUG_SUB
  if (subu_cond){
    printf("SUB: N:(%d,%d,%d), S:(%d,%d,%d)\n",src1,src2,(src1-src2),subleq_src1,subleq_src2,res_subleq);
  }
#endif
#ifdef DEBUG_ADD
  if (addu_cond){
    printf("ADDU: N:(%d,%d,%d), S:(%d,%d,%d)\n",src1,add_inp,(src1+add_inp),subleq_src1,subleq_src2,res_subleq);
  }
  else if (addiu_cond){
    printf("ADDIU: N:(%d,%d,%d), S:(%d,%d,%d)\n",src1,add_inp,(src1+add_inp),subleq_src1,subleq_src2,res_subleq);
  }
#endif
#ifdef DEBUG_MULT
  if (mult_cond){
    if ((((int64_t)src1) * src2) != (int64_t)(((uint64_t)get_value(HI) << 32) | (uint64_t)get_value(LO))) {
      printf("MULT: N:(%d,%d,%d,%d), S:(%d,%d,%d,%d)\n",src1,src2,(int)((((int64_t)src1)*src2)>>32), (int)((((int64_t)src1)*src2)&0xFFFFFFFF),subleq_src1,subleq_src2,get_value(HI),get_value(LO));
    }
  }
#endif
#ifdef DEBUG_MFLO
#endif
#ifdef DEBUG_MFHI
#endif
#ifdef DEBUG_SHIFTER
  if (sll_cond){
    if (sll(src2,shift_inp) != res_subleq){
      printf("sll: N:(%x,%x,%x), S:(%x,%x,%x)\n",src2,shift_inp,sll(src2,shift_inp),subleq_src1,subleq_src2,res_subleq);
    }
  }
  else if (sllv_cond){
    if (sll(src2, shift_inp) != res_subleq) {
      printf("sllV: N:(%x,%x,%x), S:(%x,%x,%x)\n",src2,shift_inp,sll(src2,shift_inp),subleq_src1,subleq_src2,res_subleq);
    }
  }
  else if (srl_cond){
    if (srl(src2, shift_inp) != res_subleq)
      printf("srl: N:(%x,%x,%x), S:(%x,%x,%x)\n",src2,shift_inp,srl(src2,shift_inp),subleq_src1,subleq_src2,res_subleq);
  }
  else if (srlV_COND){
    if (srl(src2, shift_inp) != res_subleq)
      printf("srlV: N:(%x,%x,%x), S:(%x,%x,%x)\n",src2,shift_inp,srl(src2,shift_inp),subleq_src1,subleq_src2,res_subleq);
  }
  else if (sra_cond){
    if (sra(src2, shift_inp) != res_subleq)
      printf("SRA: N:(%x,%x,%x), S:(%x,%x,%x)\n",src2,shift_inp,sra(src2,shift_inp),subleq_src1,subleq_src2,res_subleq);
  }
  else if (srav_cond){
    if (sra(src2, shift_inp) != res_subleq)
      printf("SRAV: N:(%x,%x,%x), S:(%x,%x,%x)\n",src2,shift_inp,sra(src2,shift_inp),subleq_src1,subleq_src2,res_subleq);
  }
#endif
#ifdef DEBUG_SET_LESS_THAN
  if (slt_cond){
    unsigned int slt_check = (src1 < slt_inp) ? 1 : 0;
    if (slt_check != res_subleq)
      printf("SLT: N:(%d,%d,%d), S:(%d,%d,%d)\n",src1,slt_inp,((src1 < slt_inp)?1:0),subleq_src1,subleq_src2,res_subleq);
  }
  else if (sltu_cond){
    unsigned int sltu_check = ((unsigned int)src1 < (unsigned int)slt_inp) ? 1 : 0;
    if (sltu_check != res_subleq)
      printf("SLTU: N:(%d,%d,%d), S:(%d,%d,%d)\n",src1,slt_inp,(((uint) src1 < (uint) slt_inp)?1:0),subleq_src1,subleq_src2,res_subleq);
  }
  else if (slti_cond){
    unsigned int slti_check = (src1 < slt_inp) ? 1 : 0;
    if (slti_check != res_subleq)
      printf("SLTI: N:(%d,%d,%d), S:(%d,%d,%d)\n",src1,slt_inp,((src1 < slt_inp)?1:0),subleq_src1,subleq_src2,res_subleq);
  }
  else if (sltiu_cond){
    unsigned int sltiu_check = ((unsigned int) src1 < (unsigned int) slt_inp) ? 1 : 0;
    if (sltiu_check != res_subleq)
      printf("SLTIU: N:(%d,%d,%d), S:(%d,%d,%d)\n",src1,slt_inp,(((uint) src1 < (uint) slt_inp)?1:0),subleq_src1,subleq_src2,res_subleq);
  } 
#endif
#ifdef	DEBUG_LOGIC_UNIT
  if (and_cond | andi_cond) {
    if (and (src1, logici_inp) != res_subleq) {
      printf ("AND: N:(%x,%x,%x), S:(%x,%x,%x)\n",src1, logici_inp, and(src1,logici_inp), subleq_src1, subleq_src2, res_subleq);
    }
  }
  else if (or_cond | ori_cond) {
    if (or (src1, logici_inp) != res_subleq) {
      printf ("OR: N:(%x,%x,%x), S:(%x,%x,%x)\n",src1, logici_inp, or(src1,logici_inp), subleq_src1, subleq_src2, res_subleq);
    }
  }
  else if (xor_cond | xori_cond) {
    if (xor (src1, logici_inp) != res_subleq) {
      printf ("XOR: N:(%x,%x,%x), S:(%x,%x,%x)\n",src1, logici_inp, xor(src1,logici_inp), subleq_src1, subleq_src2, res_subleq);
    }
  }
  else if (nor_cond) {
    printf ("NOR operation\n");
  }
#endif



  /*JR*/
  uint ret_addr_jr = add2 (src1, -1);

  /*jal, j*/
  uint link_addr = add2 (ret_addr, 1);
  uint ret_addr_J = add2 (((((*rs << 5) | *rt) << 16) | *imm), -1);


  /*BNE BEQZ BLEZ*/
  uint cond_br_addr = add2 (add2 (ret_addr, (sshort)*imm), -1);
  bool check_eq = (src1 == src2)? true : false;
  bool src_eq_0 = (src1 == 0) ? true : false;
  bool src_gt_0 = (src1 > 0) ? true : false;

  bool bne_cond_temp = (bne_cond & !check_eq);
  bool beq_cond_temp = (beq_cond & check_eq);
  bool beqz_cond_temp = (beqz_cond & (src_eq_0));
  bool bgez_cond_temp = (bgez_cond & (src_eq_0 | src_gt_0));
  bool blez_cond_temp = (blez_cond & (src_eq_0 | !src_gt_0));
  bool bltz_cond_temp = (bltz_cond & (!src_gt_0));
  bool bgtz_cond_temp = (bgtz_cond & (src_gt_0));

  bool cond_br_all = (bne_cond_temp | beq_cond_temp | beqz_cond_temp | bgez_cond_temp | blez_cond_temp | bltz_cond_temp | bgtz_cond_temp);

  uint ret_addr_cond_br = (cond_br_all) ? cond_br_addr : ret_addr;

  /*lui*/
  int res_lui = (*imm << 16) | 0x0000;
  
  int mem_addr_temp = add3 (src1, (sshort) *imm);
  /*sw*/
  uint mem_addr = (mem_addr_temp >> 2);
  int res_sw = src2;
  /*LW*/
  int res_lw = get_value(mem_addr);//src2;
  /*SB*/
  uchar byte_check = mem_addr_temp & 0x2;

  uint res_sb0 = (res_lw & 0xFFFFFF00) | (0xFF & (src2 << 0));
  uint res_sb1 = (res_lw & 0xFFFF00FF) | (0xFF00 & (src2 << 8));
  uint res_sb2 = (res_lw & 0xFF00FFFF) | (0xFF0000 & (src2 << 16));
  uint res_sb3 = (res_lw & 0x00FFFFFF) | (0xFF000000 & (src2 << 24));

  uint res_sb = (byte_check >> 1) ? ((byte_check & 0x1)? res_sb3 : res_sb2) : ((byte_check & 0x1)? res_sb1 : res_sb0);

  /*LBU, LB*/
  uint res_lbu0 = (res_lw & 0xFF) >> 0;
  uint res_lbu1 = (res_lw & 0xFF00) >> 8;
  uint res_lbu2 = (res_lw & 0xFF0000) >> 16;
  uint res_lbu3 = (res_lw & 0xFF000000) >> 24;

  uint res_lbu = (byte_check >> 1) ? ((byte_check & 0x1)? res_lbu3 : res_lbu2) : ((byte_check & 0x1)? res_lbu1 : res_lbu0);

  /*SH*/
  uchar bit_check = mem_addr_temp & 0x1;
  uint res_sh0 = (res_lw & 0xFFFF0000) | (0xFFFF & (src2 << 0));
  uint res_sh1 = (res_lw & 0x0000FFFF) | (0xFFFF0000 & (src2 << 16));
  uint res_sh = (bit_check)? res_sh1 : res_sh0;

  /*LHU, LH*/
  uint res_lhu0 = (res_lw & 0xFFFF);
  uint res_lhu1 = (res_lw & 0xFFFF0000) >> 16;
  uint res_lhu = (bit_check)? res_lhu1 : res_lhu0;

  bool ret_addr_exp = (j_cond | jr_cond | jal_cond | bne_cond | beqz_cond | beq_cond | blez_cond | bltz_cond | bgez_cond | bgtz_cond);
  ret_addr = ((j_cond)? ret_addr_J : 0x0) |/*J*/
	    ((jr_cond)? ret_addr_jr : 0x0) |/*JR*/
	    ((jal_cond)? ret_addr_J : 0x0) |/*JAL*/
	    ((bne_cond | beqz_cond | beq_cond | blez_cond | bltz_cond | bgez_cond | bgtz_cond) ? ret_addr_cond_br : 0x0) |
#if 0
	    ((bne_cond)? ret_addr_BNE : 0x0) |/*BNE*/
	    ((beqz_cond)? ret_addr_BEQZ : 0x0) |/*BEQZ*/
	    ((beq_cond)? ret_addr_BEQ : 0x0) |/*BEQ*/
	    ((blez_cond)? ret_addr_BLEZ : 0x0) |/*BLEZ*/
	    ((bltz_cond)? ret_addr_BLTZ : 0x0) |/*BLTZ*/
	    ((bgez_cond)? ret_addr_BGEZ : 0x0) |/*BGEZ*/
	    ((bgtz_cond)? ret_addr_BGTZ : 0x0) |/*BGTZ*/
#endif
	    ((ret_addr_exp)? 0x0 : ret_addr);

  bool result_EXP = (j_cond | jr_cond
#ifndef USE_SYS
		    | sys_cond 
#endif
		    | bne_cond | beqz_cond | blez_cond | bltz_cond | bgez_cond | bgtz_cond | nop_cond
#ifndef USE_MULT
		    | mult_cond
#endif
		    );

  result = 0x0 |
#ifdef	USE_LOGIC_UNIT
	   (and_cond | andi_cond | or_cond | ori_cond | xor_cond | xori_cond | nor_cond) ? res_logic : 0x0 |
#endif
#if 0
  result = ((and_cond | andi_cond)? res_and : 0x0) | /*and*/
	   ((or_cond | ori_cond)? res_or : 0x0) | /*or*/
	   ((xor_cond | xori_cond)? res_xor : 0x0) | /*xor*/
	   ((nor_cond)? res_nor : 0x0) | /*Nor*/
#endif
#ifdef USE_SET_LESS_THAN
	   ((slt_cond | slti_cond)? res_slt_i : 0x0) |/*SLT*//*SLTI*/
	   ((sltu_cond | sltiu_cond)? res_sltu_i : 0x0) |/*SLTU*/
#endif
	   ((lw_cond)? res_lw : 0x0) |/*LW*/
	   ((lbu_cond)? res_lbu : 0x0) | /*LBU*/
	   ((lb_cond)? (sint) res_lbu : 0x0) | /*LB*/
	   ((lh_cond)? (sint) res_lhu : 0x0) | /*LH*/
	   ((lhu_cond)? res_lhu : 0x0) | /*LHU*/
#if !defined(USE_ADDER) || !defined(USE_SUB) || !defined(USE_MULT) || !defined(USE_MFLO) || !defined(USE_MFHI) || !defined(USE_SHIFTER) || !defined(USE_SET_LESS_THAN) || !defined (USE_SYS) || !defined(USE_LOGIC_UNIT)
	   ((subleq_cond)? res_subleq : 0x0) |/*ADDU*/
#endif
#ifdef USE_ADDER
	   ((addu_cond | addiu_cond)? res_addu : 0x0)|
	   //((addiu_cond)? res_ADDIU : 0x0)|
#endif
#ifdef USE_SUB
	   ((subu_cond)? res_subu : 0x0) | /*SUBU*/
#endif
#ifdef USE_MULT
	   ((mult_cond)? res_mult_hi : 0x0) | /*mult*/
#endif
#ifdef USE_MFHI
	   ((mfhi_cond)? res_mfhi : 0x0) | /*MFHI*/
#endif
#ifdef USE_MFLO
	   ((mflo_cond)? res_mflo : 0x0) | /*MFLO*/
#endif
#ifdef USE_SHIFTER
           ((sll_cond | sllv_cond)? res_sllv : 0x0) |/*sll*/
	   ((sra_cond | srav_cond)? res_srav : 0x0) | /*sra*//*sraV*/
	   ((srl_cond | srlV_COND)? res_srlv : 0x0) | /*srl*/
#endif
	   ((jal_cond)? link_addr : 0x0) |/*JAL*/
	   ((lui_cond)? res_lui : 0x0) |/*LUI*/
	   ((sw_cond)? res_sw : 0x0) |/*SW*/
	   ((sb_cond)? res_sb : 0x0) | /*SB*/
	   ((sh_cond)? res_sh : 0x0) | /*SH*/
	   ((result_EXP)? 0x0 : 0x0);/*EXPECTION*/



  bool wb_temp_reg = (j_cond | beq_cond | jr_cond | sys_cond | bne_cond | beqz_cond | blez_cond | bltz_cond | bgez_cond | bgtz_cond | nop_cond
#ifndef USE_MULT
		     | mult_cond
#endif
		     );
  bool wb_rd = (sll_cond | sllv_cond | sra_cond | srav_cond | srl_cond | srlV_COND | and_cond | or_cond | xor_cond | nor_cond | slt_cond | sltu_cond | addu_cond | subu_cond | mfhi_cond | mflo_cond);  
  bool wb_rt = (slti_cond | sltiu_cond | lw_cond | lbu_cond | lb_cond | lh_cond | lhu_cond | ori_cond | andi_cond | xori_cond | addiu_cond | lui_cond); 
  bool wb_mem_addr = (sw_cond | sb_cond | sh_cond);

  wb_loc = ((wb_temp_reg)? TEMP_REG : 0x0) | 
	   ((wb_rd)? *rd : 0x0) |
	   ((wb_rt)? *rt : 0x0) |
	   ((wb_mem_addr)? mem_addr : 0x0) |
#ifdef USE_MULT 
	   ((mult_cond)? HI : 0x0) | /*mult*/
#endif
	   ((jal_cond)? 31 : 0x0);/*JAL*/

  /*syscall*/
#ifdef USE_SYS
  int syscall_exit = get_value(2);
  *emulator_status = (sys_cond & (syscall_exit == 10)) ? FINISH : NORMAL;
#else
  *emulator_status = (sys_cond & (res_subleq == 10)) ? FINISH : NORMAL;
#endif 

#ifdef USE_MULT
  write_value (res_mult_lo, LO);
#endif
  write_value (result, wb_loc);
  *prog_count = ret_addr;
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





void extended_subleq_machine (unsigned int prog_count) {
  //while (prog_count != 0){
  //}
  while (true) {
    nosi ++;
    //printf("WORKING\n");
    unsigned int a = get_value (prog_count);
    unsigned int b = get_value (prog_count + 0x1);
    signed int c = get_value (prog_count + 0x2);

    signed int src1 = get_value (a);
    signed int src2 = get_value (b);

    bool subleq = (c >= 0);
    bool extended_subleq = (c < 0);
    //bool xor_logic = ((prog_count & 0x3) == 2);

    if (subleq) {
      src2 = src2 - src1;
      write_value (src2, b);
      prog_count = ((src2 > 0) ? prog_count + 0x3 : c);
    }
    else if (extended_subleq) {
      src2 = bitReversal (src2);
      src1 = bitReversal (src1);
      src2 = src2 - src1;
      prog_count = ((src2 > 0) ? prog_count + 0x3 : (-c));
      src2 = bitReversal (src2);
      write_value (src2, b);
    }

	if (c == 0) {
      break;
    }
/*
    else {//xor logic
    }*/
  }
}



unsigned int bitReversal (unsigned int src) {
  unsigned int reversedBit = 0x0;

  reversedBit = reversedBit | (((src & 0x1) >> 0) << 31);
  reversedBit = reversedBit | (((src & 0x2) >> 1) << 30);
  reversedBit = reversedBit | (((src & 0x4) >> 2) << 29);
  reversedBit = reversedBit | (((src & 0x8) >> 3) << 28);
  reversedBit = reversedBit | (((src & 0x10) >> 4) << 27);
  reversedBit = reversedBit | (((src & 0x20) >> 5) << 26);
  reversedBit = reversedBit | (((src & 0x40) >> 6) << 25);
  reversedBit = reversedBit | (((src & 0x80) >> 7) << 24);
  reversedBit = reversedBit | (((src & 0x100) >> 8) << 23);
  reversedBit = reversedBit | (((src & 0x200) >> 9) << 22);
  reversedBit = reversedBit | (((src & 0x400) >> 10) << 21);
  reversedBit = reversedBit | (((src & 0x800) >> 11) << 20);
  reversedBit = reversedBit | (((src & 0x1000) >> 12) << 19);
  reversedBit = reversedBit | (((src & 0x2000) >> 13) << 18);
  reversedBit = reversedBit | (((src & 0x4000) >> 14) << 17);
  reversedBit = reversedBit | (((src & 0x8000) >> 15) << 16);
  reversedBit = reversedBit | (((src & 0x10000) >> 16) << 15);
  reversedBit = reversedBit | (((src & 0x20000) >> 17) << 14);
  reversedBit = reversedBit | (((src & 0x40000) >> 18) << 13);
  reversedBit = reversedBit | (((src & 0x80000) >> 19) << 12);
  reversedBit = reversedBit | (((src & 0x100000) >> 20) << 11);
  reversedBit = reversedBit | (((src & 0x200000) >> 21) << 10);
  reversedBit = reversedBit | (((src & 0x400000) >> 22) << 9);
  reversedBit = reversedBit | (((src & 0x800000) >> 23) << 8);
  reversedBit = reversedBit | (((src & 0x1000000) >> 24) << 7);
  reversedBit = reversedBit | (((src & 0x2000000) >> 25) << 6);
  reversedBit = reversedBit | (((src & 0x4000000) >> 26) << 5);
  reversedBit = reversedBit | (((src & 0x8000000) >> 27) << 4);
  reversedBit = reversedBit | (((src & 0x10000000) >> 28) << 3);
  reversedBit = reversedBit | (((src & 0x20000000) >> 29) << 2);
  reversedBit = reversedBit | (((src & 0x40000000) >> 30) << 1);
  reversedBit = reversedBit | (((src & 0x80000000) >> 31) << 0);

  return reversedBit;
}






