/* ----------------------------------------

* File Name : emulator.c

* Created on : 05-01-2015

* Last Modified on : Mon Jun 15 11:53:12 2015

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include "../config/config.h"
#include "../config/listTrace.h"
#include "../config/listMem.h"
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
uint emulator (uint, bool,int);
void exec (uint, uchar*, uchar*, uchar*, uchar*, uchar*, ushort*, uchar*, uint*, uint*, bool, unsigned int,int);
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

#define SIGNAL 126
int signal;//fault signal number
int place[] = {32,32,16,6,6,6,6,6,6,6,32,32,16,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,32,
32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,16,
16,16,16,1,1,1,1,1,1,1,1,1,1,16,16,16,16,32,32,2,
32,32,32,32,32,8,8,8,8,8,1,32,32,32,16,16,16,1,16,1,
32,1,1,1,1,16,16};//fault-place bitnum

bool stuck1 = true;//******=1?stuck at 1:0*******

int main(int argc, char **argv){
  FILE *fout= fopen(argc > 1 ? argv[1] : "output.dat","w");
  
  int N;
  uint prog_count = 0x2000;
#ifdef FAULT_ANALYZER
  int bitnum =0,cyc_num=0 ,i=0;

  bool injectFault = false;
  createNewMem ();
#endif
  unsigned int cycleCount = emulator (prog_count >> 2, injectFault, bitnum);//fault free run
  //printf("Fault Free Execution");
  //printReg();
#ifdef FAULT_ANALYZER
  //printTrace();
  //printf("%8.8x\n",headTrace);
  printf("Total number of cycles: %d\n", cycleCount);
  int bits[32];
  //struct listMem *tempHeadMem = headMem;
  for(signal=0;signal <= SIGNAL-1;signal++){
    N=place[signal];
    printf("\n**********************[FaultSignal-%d-Totalbits:%d]**************************\n",signal+1,N);
    printf("FaultFreeBit:");
    cyc_num=0;
    for(bitnum = 0 ;bitnum < N; bitnum++){

      struct listTrace *tempHeadTrace = headTrace;
      updateMem ();
      //printMem();

      headTrace = tempHeadTrace;
      //printTrace();
      injectFault = true;
      unsigned int cycleCountDup = emulator (prog_count >> 2, injectFault, bitnum);//fault injected
      //printf("\nExecution After Fault Injection");
      //printReg();
      bits[bitnum] = cycleCountDup;
      if (cycleCount == cycleCountDup){
        //printf("Fault has no effect on the program#bit:%d\n",bitnum + 1);
        //printf("Total number of cycles: %d\n", cycleCount);
        printf(" %d,", bitnum + 1);
      }
      else {
        cyc_num < cycleCount - cycleCountDup ? cyc_num = cycleCount - cycleCountDup : cyc_num;


        //printf("Fault stop the program execution#bit:%d\n",bitnum + 1);
        //printf("Total number of cycles: %d\n", cycleCount);
        //printf("Number of cycles (fault free): %d\n", cycleCountDup);
        //printf("Stopped before: %d\n", (cycleCount - cycleCountDup));
      }	

#endif
    }
    //printf("\nsignalNUM: %d\n",signal);
    //printf("\nTotal number of cycles: %d\n", cycleCount);
    printf("\ncritical fault-free cycles: %d\ncritical bit:",cycleCount -cyc_num);
    fprintf(fout,"%d\n",cycleCount -cyc_num);
    for(i = 0; i < N; i++){
      if (bits[i] == cycleCount-cyc_num){
        printf("%d,",i+1);
      }
    }

  }
  delListMem();
  fclose(fout);
  printf("\nTotal number of cycles: %d\n", cycleCount);
  return 0;
  //return (cycleCount - cycleCountDup);

  
}

    
uint emulator (uint prog_count, bool injectFault, int bitnum) {
  unsigned int error = 0x00000001<<bitnum;
  unsigned int error0 = ~(0x00000001<<bitnum);
  
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
    if(injectFault & (signal==0)){
        instruction =stuck1 ?instruction | error: instruction & error0;
    }
    prog_count_1 = prog_count_1 + 1;//signal-2, 32-bit
    if(injectFault & (signal==1)){
       prog_count_1 = stuck1 ? prog_count_1 | error: prog_count_1 & error0;
    }
    prog_count = prog_count_1;//signal-3,16-bit
    if(injectFault & (signal==2)){
       prog_count =  stuck1 ? prog_count | error: prog_count & error0;
    }
     
    exec (instruction, &opcode, &funct, &rs, &rt, &rd, &imm, &sa, &prog_count_1, &emulator_status, injectFault, cycleCount, bitnum);
    cycleCount++;
  }
  prog_count_1 = prog_count_1 & 0x1; 
  return cycleCount;//emulator_status;
}


void exec (uint instruction, uchar *opcode, uchar *funct, uchar *rs, uchar *rt, uchar *rd, ushort *imm, uchar *sa, uint *prog_count, uint *emulator_status, bool injectFault, unsigned int cycleCount, int bitnum){
  
  unsigned int error = 0x00000001<<bitnum;
  unsigned int error0 = ~(0x00000001<<bitnum);
  /*decode instructions*/
  *opcode = instruction >> 26;//signal-4, 6-bit
  if(injectFault & (signal==3)){
      *opcode = stuck1 ? *opcode | error: *opcode & error0;
  }
  *rs = (instruction >> 21) & 0X0000001F;//signal-5, 6-bit
  if(injectFault & (signal==4)){
      *rs = stuck1 ? *rs | error:*rs & error0;
  }
  *rt = (instruction >> 16) & 0X0000001F;//signal-6, 6-bit
  if(injectFault & (signal==5)){
      *rt =stuck1 ? *rt | error:*rt & error0;
  }
  *rd = (instruction >> 11) & 0X0000001F;//signal-7, 6-bit
  if(injectFault & (signal==6)){
      *rd =stuck1 ? *rd | error: *rd & error0;
  }
  *sa = (instruction >> 6) & 0X0000001F;//signal-8, 6-bit
  if(injectFault & (signal==7)){
      *sa =stuck1 ? *sa | error:*sa & error0;
  }
  *funct = instruction & 0X0000003F;//signal-9, 6-bit
  if(injectFault & (signal==8)){
      *funct = stuck1 ?*funct | error: *funct & error0;
  }
  *imm = instruction & 0xFFFF;//signal-10, 6-bit
  if(injectFault & (signal==9)){
     *imm =stuck1 ?  *imm | error : *imm & error0;
  }

  int src1 = get_value (*rs);//signal-11, 32-bit
  if(injectFault & (signal==10)){
     src1 = stuck1 ? src1 | error: src1 & error0;
  }
  int src2 = get_value (*rt);//signal-12, 32-bit
  if(injectFault & (signal==11)){
    src2 = stuck1 ? src2 | error:src2 & error0;
  }
  int result = 0;//add (src1, src2); /*addu*/
  uint ret_addr = *prog_count;// = *prog_count;//signal-13, 16-bit
  if(injectFault & (signal==12)){
     ret_addr =stuck1 ? ret_addr | error: ret_addr & error0;
  }
  uint wb_loc = TEMP_REG;

  bool r_type = (*opcode == 0x00);//signal-14,bit
  if(injectFault & (signal==13)){
     r_type =stuck1 ? r_type | error : r_type & error0;
  }
  bool nop_cond = (instruction == 0x0);//signal-15,bit
  if(injectFault & (signal==14)){
     nop_cond = stuck1 ? nop_cond | error : nop_cond & error0;
  }
  bool sll_cond = (r_type & !nop_cond & (*funct == 0x00));//signal-16,bit
  if(injectFault & (signal==15)){
     sll_cond = stuck1 ? sll_cond | error : sll_cond & error0;
  }
  bool srl_cond = (r_type & (*funct == 0x02));//signal-17,bit
  if(injectFault & (signal==16)){
     srl_cond = stuck1 ? srl_cond | error : srl_cond & error0;
  }
  bool sra_cond = (r_type & (*funct == 0x03));//signal-18,bit
  if(injectFault & (signal==17)){
    sra_cond = stuck1 ? sra_cond | error : sra_cond & error0;
  }
  bool sllv_cond = (r_type & (*funct == 0x04));//signal-19, bit
  if(injectFault & (signal==18)){
     sllv_cond = stuck1 ? sllv_cond | error : sllv_cond & error0;
  }
  bool srlv_cond = (r_type & (*funct == 0x06));//signal-20, bit
  if(injectFault & (signal==19)){
     srlv_cond = stuck1 ? srlv_cond | error : srlv_cond & error0;
  }
  bool srav_cond = (r_type & (*funct == 0x07));//signal-21, bit
  if(injectFault & (signal==20)){
     srav_cond = stuck1 ? srav_cond | error : srav_cond & error0;
  }
  bool jr_cond = (r_type & (*funct == 0x08));//signal-22, bit
  if(injectFault & (signal==21)){
     jr_cond = stuck1 ? jr_cond | error : jr_cond & error0;
  }
  bool sys_cond = (r_type & (*funct == 0x0C));//signal-23,bit
  if(injectFault & (signal==22)){
     sys_cond = stuck1 ? sys_cond | error : sys_cond & error0;
  }
  bool mfhi_cond = (r_type & (*funct == 0x10));//signal-24,bit
  if(injectFault & (signal==23)){
     mfhi_cond = stuck1 ? mfhi_cond | error : mfhi_cond & error0;
  }
  bool mflo_cond = (r_type & (*funct == 0x12));//signal-25,bit
  if(injectFault & (signal==24)){
     mflo_cond = stuck1 ? mflo_cond | error : mflo_cond & error0;
  }
  bool mult_cond = (r_type & (*funct == 0x18));//signal-26,bit
  if(injectFault & (signal==25)){
     mult_cond = stuck1 ? mult_cond | error : mult_cond & error0;
  }
  //bool div_cond =
  bool addu_cond = (r_type & (*funct == 0x21));//signal-27,bit
  if(injectFault & (signal==26)){
     addu_cond = stuck1 ? addu_cond | error :  addu_cond & error0;
  }
  bool subu_cond = (r_type & (*funct == 0x23));//signal-28,bit
  if(injectFault & (signal==27)){
     subu_cond = stuck1 ? subu_cond | error : subu_cond & error0;
  }
  bool and_cond = (r_type & (*funct == 0x24));//signal-29, bit
  if(injectFault & (signal==28)){
    and_cond =  stuck1 ? and_cond | error : and_cond & error0;
  }
  bool or_cond = (r_type & (*funct == 0x25));//signal-30, bit
  if(injectFault & (signal==29)){
    or_cond =  stuck1 ?or_cond | error : or_cond & error0;
  }
  bool xor_cond = (r_type & (*funct == 0x26));//signal-31,bit
  if(injectFault & (signal==30)){
     xor_cond = stuck1 ? xor_cond | error : xor_cond & error0;
  }
  bool nor_cond = (r_type & (*funct == 0x27));//signal-32,bit
  if(injectFault & (signal==31)){
     nor_cond = stuck1 ? nor_cond | error : nor_cond & error0;
  }
  bool slt_cond = (r_type & (*funct == 0x2A));//signal-33,bit
  if(injectFault & (signal==32)){
     slt_cond = stuck1 ? slt_cond | error : slt_cond & error0;
  }
  bool sltu_cond = (r_type & (*funct == 0x2B));//signal-34-bit
  if(injectFault & (signal==33)){
     sltu_cond = stuck1 ? sltu_cond | error : sltu_cond & error0;
  }
  bool j_cond = (*opcode == 0x02);//signal-35,bit
  if(injectFault & (signal==34)){
     j_cond = stuck1 ? j_cond | error : j_cond & error0;
  }
  bool jal_cond = (*opcode == 0x03);//signal-36,bit
  if(injectFault & (signal==35)){
     jal_cond = stuck1 ? jal_cond | error : jal_cond & error0;
  }
  bool bltz_cond = ((*opcode == 0x01) & (*rt == 0x0));//signal-37,bit
  if(injectFault & (signal==36)){
     bltz_cond = stuck1 ? bltz_cond | error : bltz_cond & error0;
  }
  bool bgez_cond = ((*opcode == 0x01) & (*rt == 0x1));//signal-38,bit
  if(injectFault & (signal==37)){
     bgez_cond = stuck1 ? bgez_cond | error : bgez_cond & error0;
  }
  bool beq_cond = ((*opcode == 0x04) & (*rt != 0x0));//signal-39,bit
  if(injectFault & (signal==38)){
     beq_cond = stuck1 ? beq_cond | error : beq_cond & error0;
  }
  bool beqz_cond = ((*opcode == 0x04) & (*rt == 0x0));//signal-40,bit
  if(injectFault & (signal==39)){
     beqz_cond = stuck1 ? beqz_cond | error : beqz_cond & error0;
  }
  bool bne_cond = (*opcode == 0x05);//signal-41,bit
  if(injectFault & (signal==40)){
     bne_cond = stuck1 ? bne_cond | error : bne_cond & error0;
  }
  bool blez_cond = (*opcode == 0x06);//signal-42,bit
  if(injectFault & (signal==41)){
     blez_cond = stuck1 ? blez_cond | error : blez_cond & error0;
  }
  bool bgtz_cond = (*opcode == 0x07);//signal-43,bit
  if(injectFault & (signal==42)){
     bgtz_cond = stuck1 ? bgtz_cond | error : bgtz_cond & error0;
  }
  bool addiu_cond = (*opcode == 0x09);//signal-44,bit
  if(injectFault & (signal==43)){
     addiu_cond = stuck1 ? addiu_cond | error : addiu_cond & error0;
  }
  bool slti_cond = (*opcode == 0x0A);//signal-45,bit
  if(injectFault & (signal==44)){
     slti_cond = stuck1 ? slti_cond | error : slti_cond & error0;
  }
  bool sltiu_cond = (*opcode == 0x0B);//signal-46,bit
  if(injectFault & (signal==45)){
     sltiu_cond = stuck1 ? sltiu_cond | error : sltiu_cond & error0;
  }
  bool andi_cond = (*opcode == 0x0C);//signal-47,bit
  if(injectFault & (signal==46)){
     andi_cond = stuck1 ? andi_cond | error : andi_cond & error0;
  }
  bool ori_cond = (*opcode == 0x0D);//signal-48,bit
  if(injectFault & (signal==47)){
     ori_cond = stuck1 ? ori_cond | error : ori_cond & error0;
  }
  bool xori_cond = (*opcode == 0x0E);//signal-49,bit
  if(injectFault & (signal==48)){
     xori_cond = stuck1 ? xori_cond | error : xori_cond & error0;
  }
  bool lui_cond = (*opcode == 0x0F);//signal-50,bit
  if(injectFault & (signal==49)){
     lui_cond = stuck1 ? lui_cond | error : lui_cond & error0;
  }
  bool lb_cond = (*opcode == 0x20);//signal-51,bit
  if(injectFault & (signal==50)){
     lb_cond = stuck1 ? lb_cond | error : lb_cond & error0;
  }
  bool lh_cond = (*opcode == 0x21);//signal-52,bit
  if(injectFault & (signal==51)){
     lh_cond = stuck1 ? lh_cond | error : lh_cond & error0;
  }
  bool lw_cond = (*opcode == 0x23);//signal-53,bit
  if(injectFault & (signal==52)){
     lw_cond = stuck1 ? lw_cond | error : lw_cond & error0;
  }
  bool lbu_cond = (*opcode == 0x24);//signal-54,bit
  if(injectFault & (signal==53)){
     lbu_cond = stuck1 ? lbu_cond | error : lbu_cond & error0;
  }
  bool lhu_cond = (*opcode == 0x25);//signal-55,bit
  if(injectFault & (signal==54)){
     lhu_cond = stuck1 ? lhu_cond | error : lhu_cond & error0;
  }
  bool sb_cond = (*opcode == 0x28);//signal-56,bit
  if(injectFault & (signal==55)){
     sb_cond = stuck1 ? sb_cond | error : sb_cond & error0;
  }
  bool sh_cond = (*opcode == 0x29);//signal-57,bit
  if(injectFault & (signal==56)){
     sh_cond = stuck1 ? sh_cond | error : sh_cond & error0;
  }
  bool sw_cond = (*opcode == 0x2B);//signal-58,bit
  if(injectFault & (signal==57)){
     sw_cond = stuck1 ? sw_cond | error : sw_cond & error0;
  }


  /*AND ANDI OR ORI XOR XORI*/
  bool logici_cond = andi_cond | ori_cond | xori_cond;//signal-59,bit 
  if(injectFault & (signal==58)){
     logici_cond = stuck1 ? logici_cond | error : logici_cond & error0;
  }
  uint logici_inp = (logici_cond) ? (ushort) *imm : src2;//signal-60,32-bit
  if(injectFault & (signal==59)){
     logici_inp = stuck1 ? logici_inp | error : logici_inp & error0;
  }
  uint res_and = and (src1, logici_inp);//signal-61,32-bit
  if(injectFault & (signal==60)){
     res_and = stuck1 ? res_and | error : res_and & error0;
  }
  uint res_or = or (src1, logici_inp);//signal-62,32-bit
  if(injectFault & (signal==61)){
     res_or = stuck1 ? res_or | error : res_or & error0;
  }
  uint res_xor = xor (src1, logici_inp);//signal-63,32-bit
  if(injectFault & (signal==62)){
     res_xor = stuck1 ? res_xor | error : res_xor & error0;
  }

  /*NOR*/
  uint res_nor = ~res_or;//signal-64,32-bit
  if(injectFault & (signal==63)){
     res_nor = stuck1 ? res_nor | error : res_nor & error0;
  }
  uint res_logic = ((and_cond | andi_cond) ? res_and : 0x0) |
		   ((or_cond | ori_cond) ? res_or : 0x0) |
		   ((xor_cond | xori_cond) ? res_xor : 0x0) |
		   ((nor_cond) ? res_nor : 0x0);//signal-65,32-bit
  if(injectFault & (signal==64)){
     res_logic = stuck1 ? res_logic | error : res_logic & error0;
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
  if(injectFault & (signal==65)){
     res_mfhi = stuck1 ? res_mfhi | error : res_mfhi & error0;
  }
#endif
#ifdef USE_MFLO
  uint res_mflo = get_value (LO);//signal-67,32-bit
  if(injectFault & (signal==66)){
     res_mflo = stuck1 ? res_mflo | error : res_mflo & error0;
  }
#endif


#ifdef USE_MULT
  /*MULT*/ /**opcode == 0x00 & *funct == 0x18*/
  int64_t res_mult_temp = mult (src1, src2);
  int res_mult_lo = (mult_cond)? res_mult_temp & 0xFFFFFFFF : res_mflo;//signal-68,32-bit
  if(injectFault & (signal==67)){
     res_mult_lo = stuck1 ? res_mult_lo | error : res_mult_lo & error0;
  }
  int res_mult_hi = (res_mult_temp >> 32);//signal-69,32-bit
  if(injectFault & (signal==68)){
     res_mult_hi = stuck1 ? res_mult_hi | error : res_mult_hi & error0;
  }
#endif


#ifdef USE_SUB  
  /*SUBU*/ /**opcode == 0x00 & *funct == 0x23*/
  uint res_subu = sub ((uint) src1, (uint) src2);//signal-70,32-bit
  if(injectFault & (signal==69)){
     res_subu = stuck1 ? res_subu | error : res_subu & error0;
  }
#endif

  /*SRA*//*SRAV*//*SLL*//*SLLV*//*SRL*//*SRLV*/
  int shift_inp = (srav_cond | sllv_cond | srlv_cond) ? src1 : *sa;//signal-71,32-bit
  if(injectFault & (signal==70)){
     shift_inp = stuck1 ? shift_inp | error : shift_inp & error0;
  }
#ifdef USE_SHIFTER
  int res_sra_v = sra (src2, shift_inp);//signal-72,32-bit
  if(injectFault & (signal==71)){
     res_sra_v = stuck1 ? res_sra_v | error : res_sra_v & error0;
  }
  int res_sllv = sll (src2, shift_inp);//signal-73,32-bit
  if(injectFault & (signal==72)){
     res_sllv = stuck1 ? res_sllv | error : res_sllv & error0;
  }
  int res_srlv = srl (src2, shift_inp);//signal-74,32-bit
  if(injectFault & (signal==73)){
     res_srlv = stuck1 ? res_srlv | error : res_srlv & error0;
  }
  
#endif

  int add_inp = (addiu_cond)? (sshort)*imm : src2;//signal-75,32-bit
  if(injectFault & (signal==74)){
     add_inp = stuck1 ? add_inp | error : add_inp & error0;
  }
#ifdef USE_ADDER
  /*addu addiu*/
  uint res_addu = add (src1, add_inp);//signal-76,32-bit
  if(injectFault & (signal==75)){
     res_addu = stuck1 ? res_addu | error : res_addu & error0;
  }
#endif  

  int slt_inp = (slti_cond | sltiu_cond) ? (sshort) *imm : src2;//signal-77,32-bit
  if(injectFault & (signal==76)){
     slt_inp = stuck1 ? slt_inp | error : slt_inp & error0;
  }
#ifdef	USE_SET_LESS_THAN
  /*SLT*//*SLTI*/
  int res_slt_i = (src1 < slt_inp) ? 1 : 0;//signal-78,32-bit
  if(injectFault & (signal==77)){
     res_slt_i = stuck1 ? res_slt_i | error : res_slt_i & error0;
  }
  /*SLTU*//*SLTIU*/
  int res_sltu_i = ((uint) src1 < (uint) slt_inp) ? 1 : 0;//signal-79,32-bit
  if(injectFault & (signal==78)){
     res_sltu_i = stuck1 ? res_sltu_i | error : res_sltu_i & error0;
  }
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
  if(injectFault & (signal==79)){
    ret_addr_jr =  stuck1 ? ret_addr_jr | error : ret_addr_jr & error0;
  }

  /*jal, j*/
  uint res_jal = add2 (ret_addr, 1);//signal-81,16-bit
  if(injectFault & (signal==80)){
     res_jal =  stuck1 ? res_jal | error : res_jal & error0;
  }
  uint ret_addr_j = add2 (((((*rs << 5) | *rt) << 16) | *imm), -1);//signal-82,16-bit
  if(injectFault & (signal==81)){
     ret_addr_j =  stuck1 ? ret_addr_j | error : ret_addr_j & error0;
  }


  /*BNE BEQZ BLEZ*/
  uint cond_br_addr = add2 (add2 (ret_addr, (sshort)*imm), -1);//signal-83,16-bit
  if(injectFault & (signal==82)){
     cond_br_addr =  stuck1 ? cond_br_addr | error : cond_br_addr & error0;
  }
  bool check_eq = (src1 == src2)? true : false;//signal-84,bit
  if(injectFault & (signal==83)){
     check_eq =  stuck1 ? check_eq | error : check_eq & error0;
  }
  bool src_eq_0 = (src1 == 0) ? true : false;//signal-85,bit
  if(injectFault & (signal==84)){
     src_eq_0 =  stuck1 ? src_eq_0 | error : src_eq_0 & error0;
  }
  bool src_gt_0 = (src1 > 0) ? true : false;//signal-86,bit
  if(injectFault & (signal==85)){
     src_gt_0 =  stuck1 ? src_gt_0 | error : src_gt_0 & error0;
  }

  bool bne_cond_temp = (bne_cond & !check_eq);//signal-87,bit
  if(injectFault & (signal==86)){
     bne_cond_temp = stuck1 ?  bne_cond_temp | error : bne_cond_temp & error0;
  }
  bool beq_cond_temp = (beq_cond & check_eq);//signal-88,bit
  if(injectFault & (signal==87)){
     beq_cond_temp = stuck1 ?  beq_cond_temp | error : beq_cond_temp & error0;
  }
  bool beqz_cond_temp = (beqz_cond & (src_eq_0));//signal-89,bit
  if(injectFault & (signal==88)){
     beqz_cond_temp =stuck1 ?  beqz_cond_temp | error : beqz_cond_temp & error0;
  }
  bool bgez_cond_temp = (bgez_cond & (src_eq_0 | src_gt_0));//signal-90,bit
  if(injectFault & (signal==89)){
     bgez_cond_temp = stuck1 ?  bgez_cond_temp | error : bgez_cond_temp & error0;
  }
  bool blez_cond_temp = (blez_cond & (src_eq_0 | !src_gt_0));//signal-91,bit
  if(injectFault & (signal==90)){
     blez_cond_temp =stuck1 ?  blez_cond_temp | error : blez_cond_temp & error0;
  }
  bool bltz_cond_temp = (bltz_cond & (!src_gt_0));//signal-92,bit
  if(injectFault & (signal==91)){
     bltz_cond_temp =stuck1 ?  bltz_cond_temp | error : bltz_cond_temp & error0;
  }
  bool bgtz_cond_temp = (bgtz_cond & (src_gt_0));//signal-93,bit
  if(injectFault & (signal==92)){
     bgtz_cond_temp =stuck1 ?  bgtz_cond_temp | error : bgtz_cond_temp & error0;
  }

  bool br_cond_all = (bne_cond_temp | beq_cond_temp | beqz_cond_temp | bgez_cond_temp | blez_cond_temp | bltz_cond_temp | bgtz_cond_temp);//signal-94,bit
  if(injectFault & (signal==93)){
     br_cond_all =stuck1 ?  br_cond_all | error : br_cond_all & error0;
  }

  uint ret_addr_cond_branch = (br_cond_all) ? cond_br_addr : ret_addr;//signal-94,16-bit
  if(injectFault & (signal==93)){
     ret_addr_cond_branch =stuck1 ?  ret_addr_cond_branch | error : ret_addr_cond_branch & error0;
  }

  /*lui*/
  int res_lui = (*imm << 16) | 0x0000;//signal-95,32-bit
  if(injectFault & (signal==94)){
     res_lui =stuck1 ?  res_lui | error : res_lui & error0;
  }
  if (injectFault){
    unsigned int error = ~(0x00000001<<bitnum);
    res_lui = res_lui & error; //for sa1
  }
  
  int mem_addr_temp = add3 (src1, (sshort) *imm);//signal-96,16-bit
  if(injectFault & (signal==95)){
     mem_addr_temp =stuck1 ?  mem_addr_temp | error : mem_addr_temp & error0;
  }
  /*sw*/
  uint mem_addr = (mem_addr_temp >> 2);//signal-97,16-bit
  if(injectFault & (signal==96)){
     mem_addr =stuck1 ?  mem_addr | error : mem_addr & error0;
  }
  int res_sw = src2;//signal-98,32-bit
  if(injectFault & (signal==97)){
     res_sw =stuck1 ?  res_sw | error : res_sw & error0;
  }
  /*LW*/
  int res_lw = get_value(mem_addr);//src2;//signal-99,32-bit
  if(injectFault & (signal==98)){
     res_lw =stuck1 ?  res_lw | error : res_lw & error0;
  }
  /*SB*/
  uchar byte_check = mem_addr_temp & 0x2;//signal-100,2-bit
  if(injectFault & (signal==99)){
     byte_check =stuck1 ?  byte_check | error : byte_check & error0;
  }

  uint res_sb0 = (res_lw & 0xFFFFFF00) | (0xFF & (src2 << 0));//signal-101,32-bit
  if(injectFault & (signal==100)){
     res_sb0 =stuck1 ?  res_sb0 | error : res_sb0 & error0;
  }
  uint res_sb1 = (res_lw & 0xFFFF00FF) | (0xFF00 & (src2 << 8));//signal-102,32-bit
  if(injectFault & (signal==101)){
     res_sb1 =stuck1 ?  res_sb1 | error : res_sb1 & error0;
  }
  uint res_sb2 = (res_lw & 0xFF00FFFF) | (0xFF0000 & (src2 << 16));//signal-103,32-bit
  if(injectFault & (signal==102)){
     res_sb2 =stuck1 ?  res_sb2 | error : res_sb2 & error0;
  }
  uint res_sb3 = (res_lw & 0x00FFFFFF) | (0xFF000000 & (src2 << 24));//signal-104,32-bit
  if(injectFault & (signal==103)){
     res_sb3 =stuck1 ?  res_sb3 | error : res_sb3 & error0;
  }

  uint res_sb = (byte_check >> 1) ? ((byte_check & 0x1)? res_sb3 : res_sb2) : ((byte_check & 0x1)? res_sb1 : res_sb0);//signal-105,32-bit
  if(injectFault & (signal==104)){
     res_sb =stuck1 ?  res_sb | error : res_sb & error0;
  }

  /*LBU, LB*/
  uint res_lbu0 = (res_lw & 0xFF) >> 0;//signal-106,32-bit
  if(injectFault & (signal==105)){
     res_lbu0 =stuck1 ?  res_lbu0 | error : res_lbu0 & error0;
  }
  uint res_lbu1 = (res_lw & 0xFF00) >> 8;//signal-107,32-bit
  if(injectFault & (signal==106)){
     res_lbu1 =stuck1 ?  res_lbu1 | error : res_lbu1 & error0;
  }
  uint res_lbu2 = (res_lw & 0xFF0000) >> 16;//signal-108,32-bit
  if(injectFault & (signal==107)){
     res_lbu2 =stuck1 ?  res_lbu2 | error : res_lbu2 & error0;
  }
  uint res_lbu3 = (res_lw & 0xFF000000) >> 24;//signal-109,32-bit
  if(injectFault & (signal==108)){
     res_lbu3 =stuck1 ?  res_lbu3 | error : res_lbu3 & error0;
  }

  uint res_lbu = (byte_check >> 1) ? ((byte_check & 0x1)? res_lbu3 : res_lbu2) : ((byte_check & 0x1)? res_lbu1 : res_lbu0);//signal-110,32-bit
  if(injectFault & (signal==109)){
     res_lbu =stuck1 ?  res_lbu | error : res_lbu & error0;
  }

  /*SH*/
  uchar bit_check = mem_addr_temp & 0x1;//signal-111,1-bit
  if(injectFault & (signal==110)){
     bit_check =stuck1 ?  bit_check | error : bit_check & error0;
  }
  uint res_sh0 = (res_lw & 0xFFFF0000) | (0xFFFF & (src2 << 0));//signal-112,32-bit
  if(injectFault & (signal==111)){
     res_sh0 =stuck1 ?  res_sh0 | error : res_sh0 & error0;
  }
  uint res_sh1 = (res_lw & 0x0000FFFF) | (0xFFFF0000 & (src2 << 16));//signal-113,32-bit
  if(injectFault & (signal==112)){
     res_sh1 =stuck1 ?  res_sh1 | error : res_sh1 & error0;
  }
  uint res_sh = (bit_check)? res_sh1 : res_sh0;//signal-114,32-bit
  if(injectFault & (signal==113)){
     res_sh =stuck1 ?  res_sh | error : res_sh & error0;
  }

  /*LHU, LH*/
  uint res_lhu0 = (res_lw & 0xFFFF);//signal-115,32-bit
  if(injectFault & (signal==114)){
     res_lhu0 =stuck1 ?  res_lhu0 | error : res_lhu0 & error0;
  }
  uint res_lhu1 = (res_lw & 0xFFFF0000) >> 16;//signal-116,32-bit
  if(injectFault & (signal==115)){
     res_lhu1 =stuck1 ?  res_lhu1 | error : res_lhu1 & error0;
  }
  uint res_lhu = (bit_check)? res_lhu1 : res_lhu0;//signal-117,32-bit
  if(injectFault & (signal==116)){
     res_lhu =stuck1 ?  res_lhu | error : res_lhu & error0;
  }

  bool ret_addr_exp = (j_cond | jr_cond | jal_cond | bne_cond | beqz_cond | beq_cond | blez_cond | bltz_cond | bgez_cond | bgtz_cond);//signal-118,bit
  if(injectFault & (signal==117)){
     ret_addr_exp =stuck1 ?  ret_addr_exp | error : ret_addr_exp & error0;
  }
  ret_addr = ((j_cond)? ret_addr_j : 0x0) |/*J*/
	    ((jr_cond)? ret_addr_jr : 0x0) |/*JR*/
	    ((jal_cond)? ret_addr_j : 0x0) |/*JAL*/
	    ((bne_cond | beqz_cond | beq_cond | blez_cond | bltz_cond | bgez_cond | bgtz_cond) ? ret_addr_cond_branch : 0x0) |
	    ((ret_addr_exp)? 0x0 : ret_addr);//signal-119,16-bit
	    
  if(injectFault & (signal==118)){
     ret_addr =stuck1 ?  ret_addr | error : ret_addr & error0;
  }

  bool result_exp = (j_cond | jr_cond
#ifndef USE_SYS
		    | sys_cond 
#endif
		    | bne_cond | beqz_cond | blez_cond | bltz_cond | bgez_cond | bgtz_cond | nop_cond
#ifndef USE_MULT
		    | mult_cond
#endif
		    );//signal-120,bit
  if(injectFault & (signal==119)){
     result_exp =stuck1 ?  result_exp | error : result_exp & error0;
  }

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
  if(injectFault & (signal==120)){
     result =stuck1 ?  result | error : result & error0;
  }


  bool wb_temp_reg = (j_cond | beq_cond | jr_cond | sys_cond | bne_cond | beqz_cond | blez_cond | bltz_cond | bgez_cond | bgtz_cond | nop_cond
#ifndef USE_MULT
		     | mult_cond
#endif
		     );//signal-122,bit
  if(injectFault & (signal==121)){
     wb_temp_reg =stuck1 ?  wb_temp_reg | error : wb_temp_reg & error0;
  }
  bool wb_rd = (sll_cond | sllv_cond | sra_cond | srav_cond | srl_cond | srlv_cond | and_cond | or_cond | xor_cond | nor_cond | slt_cond | sltu_cond | addu_cond | subu_cond | mfhi_cond | mflo_cond);//signal-123,bit  
  if(injectFault & (signal==122)){
     wb_rd =stuck1 ?  wb_rd | error : wb_rd & error0;
  }
  bool wb_rt = (slti_cond | sltiu_cond | lw_cond | lbu_cond | lb_cond | lh_cond | lhu_cond | ori_cond | andi_cond | xori_cond | addiu_cond | lui_cond);//signal-124,bit 
  if(injectFault & (signal==123)){
     wb_rt =stuck1 ?  wb_rt | error : wb_rt & error0;
  }
  bool wb_mem_addr = (sw_cond | sb_cond | sh_cond);//signal-125,bit
  if(injectFault & (signal==124)){
     wb_mem_addr =stuck1 ?  wb_mem_addr | error : wb_mem_addr & error0;
  }

  wb_loc = ((wb_temp_reg)? TEMP_REG : 0x0) | 
	   ((wb_rd)? *rd : 0x0) |
	   ((wb_rt)? *rt : 0x0) |
	   ((wb_mem_addr)? mem_addr : 0x0) |
#ifdef USE_MULT 
	   ((mult_cond)? HI : 0x0) | /*MULT*/
#endif
	   ((jal_cond)? 31 : 0x0);/*JAL*///sginal-126,16-bit
  if(injectFault & (signal==125)){
     wb_loc =stuck1 ?  wb_loc | error : wb_loc & error0;
  }

  /*syscall*/
#ifdef USE_SYS
  int syscall_exit = get_value(2);//signal-127,32-bit,not consider
  if(injectFault & (signal==126)){
     syscall_exit =stuck1 ?  syscall_exit | error : syscall_exit & error0;
  }
  *emulator_status = (sys_cond & (syscall_exit == 10)) ? FINISH : NORMAL;//signal-128,32-bit,not consider
  if(injectFault & (signal==129)){
     *emulator_status =stuck1 ?  *emulator_status | error : *emulator_status & error0;
  }
#else
  *emulator_status = (sys_cond & (res_subleq == 10)) ? FINISH : NORMAL;
#endif 

#ifdef USE_MULT
  write_value (res_mult_lo, LO);
#endif
  write_value (result, wb_loc);
  *prog_count = ret_addr;//signal-129,16-bit
  if(injectFault & (signal==127)){
     *prog_count =stuck1 ?  *prog_count | error : *prog_count & error0;
  }

  if (!injectFault) {
    unsigned int structLocation = 0;
    faultFreeTrace (cycleCount, *prog_count, instruction, result, wb_loc, &structLocation);
  }
  if (injectFault) {
    bool error = checkFault (cycleCount, *prog_count, instruction, result, wb_loc);
    if (error){
      *emulator_status = FINISH;
      //printf ("Fault detected @ cycle %8.8x\n", cycleCount);
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
      printf("\n  ");
    printf("%8.8x\t",get_value(i));
  }
  printf("\n");
}
// vim: set sw=2 et :
