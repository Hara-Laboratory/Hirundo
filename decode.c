/* ----------------------------------------

* File Name : decode.c

* Created on : 06-01-2015

* Last Modified on : Mon 02 Feb 2015 05:50:41 PM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include "config.h"
//#include "function.c"

void decode(uint instruction, uchar *opcode, uchar *funct, uchar *rs, uchar *rt, uchar *rd, ushort *imm, uchar *sa, uint *prog_count, unsigned int *EMULATOR_STATUS){
  /*decode instructions*/
  //printf("%x\n",instruction);
  *opcode = instruction >> 26;
  *funct = instruction & 0X0000003F;
  *rs = (instruction >> 21) & 0X0000001F;
  *rt = (instruction >> 16) & 0X0000001F;
  *rd = (instruction >> 11) & 0X0000001F;
  *imm = instruction & 0xFFFF;
  *sa = (instruction >> 6) & 0X0000001F;

  /*common for every thing*/
  TEMP[4] = Y;
  TEMP[5] = Z;
  
  if (*opcode == 0x00) {
    switch (*funct) {
#if 0
      case 0x00:/*sll*/
	write_value (*sa,TEMP_REG);
	TEMP[0] = *rt;
	TEMP[1] = TEMP_REG;
	TEMP[2] = *rd;
	//*prog_count = SLL_ROUTINE;
	break;
      case 0x02:/*srl*/
	write_value (*sa,TEMP_REG);
	TEMP[0] = *rt;
	TEMP[1] = TEMP_REG;
	TEMP[2] = *rd;
	//*prog_count = SRL_ROUTINE;
	srl ();
	break;
      case 0x03:/*sra*/
	write_value (*sa,TEMP_REG);
	TEMP[0] = *rt;
	TEMP[1] = TEMP_REG;
	TEMP[2] = *rd;
	sra ();
	//*prog_count = SRA_ROUTINE;
	break;
      case 0x04:/*sllv*/
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = *rd;
	//*prog_count = SLLV_ROUTINE;
	printf("%x Unimplemented Instructions sllv\n", *prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	break;
      case 0x06:/*srlv*/
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = *rd;
	//*prog_count = SRLV_ROUTINE;
	printf("%x Unimplemented Instructions srlv\n",*prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	break;
      case 0x07:/*srav*/
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = *rd;
	//*prog_count = SRAV_ROUTINE;
	printf("%x Unimplemented Instructions srav\n", *prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	break;
#endif
      case 0x08:/*jr*/
	//TEMP[0] = *rs;
	write_value (get_value(*rs) - 1, RETURN_PC);
	break;
#if 0
      case 0x09:/*jalr*/
	printf("%x Unimplemented Instructions jalr\n", *prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	break;
#endif
      case 0x0C:/*syscall*/
	TEMP[0] = 2;
	if ((get_value (2) == 10)){
	  *EMULATOR_STATUS = FINISH;
        }
	break;
#if 0
      case 0x10:/*mfhi*/
	write_value (get_value (HI),*rd);
	//printf("%x Unimplemented Instructions mfhi\n", *prog_count << 2);
	//*EMULATOR_STATUS = FINISH;
	break;
      case 0x11:/*mthi*/
	//write_value (get_value (*rs),HI);
	printf("%x Unimplemented Instructions mthi\n", *prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	break;
      case 0x12:/*mflo*/
	write_value (get_value (LO),*rd);
	break;
      case 0x13:/*mtlo*/
	//write_value (get_value (*rs),LO);
	printf("%x Unimplemented Instructions mtlo\n", *prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	break;
      case 0x18:/*mult*/
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = LO;
	TEMP[3] = HI;
	mult ();
	//printf("%x Unimplemented Instructions mult\n", *prog_count << 2);
	//*EMULATOR_STATUS = FINISH;
	break;
      case 0x19:/*multu*/
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = LO;
	TEMP[3] = HI;
	mult ();
	//printf("%x Unimplemented Instructions multu\n", *prog_count << 2);
	//*EMULATOR_STATUS = FINISH;
	break;
      case 0x1A:/*div*///FIXME
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = LO;
	TEMP[3] = HI;
	printf("%x Unimplemented Instructions div\n", *prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	break;
      case 0x1B:/*divu*/
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = LO;
	TEMP[3] = HI;
	printf("%x Unimplemented Instructions divu\n", *prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	break;
      case 0x20:/*add*/
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = *rd;
	//*prog_count = ADD_ROUTINE;
	//printf("%x Unimplemented Instructions add\n", *prog_count << 2);
	//*EMULATOR_STATUS = FINISH;
	add ();
	break;
#endif
      case 0x21:/*addu*/
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = *rd;
	//*prog_count = ADD_ROUTINE;
	add ();
	break;
#if 0
      case 0x22:/*sub*///FIXME
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = *rd;
	//*prog_count = SUB_ROUTINE;
	printf("%x Unimplemented Instructions sub\n", *prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	break;
      case 0x23:/*subu*/
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = *rd;
	//*prog_count = SUB_ROUTINE;
	sub ();
	break;
      case 0x24:/*and*/
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = *rd;
	//*prog_count = AND_ROUTINE;
	and ();
	break;
      case 0x25:/*or*/
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = *rd;
	//*prog_count = OR_ROUTINE;
	or ();
	//printf("%x %x %x\n",get_value(*rs),get_value(*rt),get_value(*rd));
	break;
      case 0x26:/*xor*/
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = *rd;
	//*prog_count = XOR_ROUTINE;
	xor ();
	//printf("%x Unimplemented Instructions xor\n", *prog_count << 2);
	//*EMULATOR_STATUS = FINISH;
	break;
      case 0x27:/*nor*/
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = *rd;
	//*prog_count = NOR_ROUTINE;
	printf("%x Unimplemented Instructions nor\n", *prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	break;
      case 0x2A:/*slt*///FIXME
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = *rd;
	if ((int) get_value(*rs) < (int) get_value(*rt)){
	  write_value(1,*rd);
	}
	else {
	  write_value(0,*rd);
	}
	break;
      case 0x2B:/*sltu*/
	TEMP[0] = *rs;
	TEMP[1] = *rt;
	TEMP[2] = *rd;
	if (get_value(*rs) < get_value(*rt)){
	  write_value(1,*rd);
	}
	else {
	  write_value(0,*rd);
	}
	//*prog_count = SLT_ROUTINE;
	//printf("%x Unimplemented Instructions sltu\n", *prog_count << 2);
	//*EMULATOR_STATUS = FINISH;
	break;
#endif
      default:
	printf("%x Unimplemented Instructions R\n", *prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	*EMULATOR_STATUS = FINISH;
	//*prog_count = *prog_count;
      }
  }
  else if (*opcode == 0x03 || *opcode == 0x02) {
    switch (*opcode) {
#if 0
      case 0x02:/*j*/
	write_value ((((((*rs<<5)|*rt)<<16)|*imm)-1), RETURN_PC);
	break;
#endif
      case 0x03:/*jal*/
	write_value ((((((*rs<<5)|*rt)<<16)|*imm)-1), RETURN_PC);
	write_value (*prog_count + 1, 31);
	//printf("JAL\n");
	//printf("%x\n",get_value(RETURN_PC)<<2);
	break;
      default:
	printf("%x Unimplemented Instructions J\n",*prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	//*prog_count = *prog_count;
    }
  }
#if 0
  else if (*opcode == 0x11) {
  }
#endif
  else {
    unsigned char temp1;
    unsigned char temp2;
    unsigned int temp;
    switch (*opcode) {
#if 0
      case 0x01:/*bltz,bgez*/
	if (*rt == 0x00) {
	  //write_value(0,TEMP_REG);
	  //TEMP[0] = *rs;
	  //TEMP[1] = TEMP_REG;
	  if (get_value(*rs) < 0){
	    write_value (((signed short) *imm + *prog_count - 1), RETURN_PC);
	  }
	  
	}/*bltz*/
	else if (*rt == 0x01){
	  if (get_value(*rs) >= 0){
	    write_value (((signed short) *imm + *prog_count - 1), RETURN_PC);
	  }
	}/*bgez*/
	break;
      case 0x04:
	if (*rt == 0x00){
	  TEMP[0] = *rs;//not used now
	  TEMP[1] = *rt;//not used now
	  if (get_value(*rs) == get_value(*rt)){
	    write_value (((signed short) *imm + *prog_count - 1), RETURN_PC);
	  }
	}/*beq*/
	else if (*rt == 0x01){
	printf("%x Unimplemented Instructions beqz\n", *prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	}/*beqz*///FIXME
	break;
#endif
      case 0x05:/*bne*/
	TEMP[0] = *rs;//not used now
	TEMP[1] = *rt;//now used now
	if (get_value(*rs) != get_value(*rt)){
	  write_value (((signed short) *imm + *prog_count - 1), RETURN_PC);
	}
	break;
#if 0
      case 0x06:/*blez*/
	if (get_value(*rs) <= 0){
	  write_value (((signed short) *imm + *prog_count - 1), RETURN_PC);
	}
	break;
      case 0x07:/*bgtz*///FIXME
	if (get_value(*rs) > 0){
	  write_value (((signed short) *imm + *prog_count - 1), RETURN_PC);
	}
	break;
      case 0x08:/*addi*///FIXME for signed
	write_value((0|*imm), TEMP_REG);
	TEMP[0] = *rs;
	TEMP[1] = TEMP_REG;
	TEMP[2] = *rt;
	//*prog_count = ADD_ROUTINE;
	printf("%x Unimplemented Instructions addi\n", *prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	add ();
	break;
#endif
      case 0x09:/*addiu*/
	//write_value((signed short)*imm, TEMP_REG);
	write_value((signed short)*imm, TEMP_REG);
	TEMP[0] = *rs;
	TEMP[1] = TEMP_REG;
	TEMP[2] = *rt;
	add ();
	//printf("ADDIU\n");
	//printf("RS: %d RT: %d RD: %d\n",get_value(TEMP[0]), get_value(TEMP[1]), get_value(TEMP[2]));
	break;
#if 0
      case 0x0A:/*slti*/
	write_value((0|*imm), TEMP_REG);
	TEMP[0] = *rs;
	TEMP[1] = TEMP_REG;
	TEMP[2] = *rt;
	//*prog_count = SLT_ROUTINE;
	if (get_value(*rs) < get_value(TEMP_REG)){
	  write_value(1,*rt);
	}
	else {
	  write_value(0,*rt);
	}
	break;
      case 0x0B:/*sltiu*/
	write_value((0|*imm), TEMP_REG);
	TEMP[0] = *rs;
	TEMP[1] = TEMP_REG;
	TEMP[2] = *rt;
	//*prog_count = SLT_ROUTINE;
	if (get_value(*rs) < get_value(TEMP_REG)){
	  write_value(1,*rt);
	}
	else {
	  write_value(0,*rt);
	}
	//printf("%x Unimplemented Instructions sltiu\n",*prog_count << 2);
	break;
      case 0x0C:/*andi*/
	write_value((0|*imm), TEMP_REG);
	TEMP[0] = *rs;
	TEMP[1] = TEMP_REG;
	TEMP[2] = *rt;
	and ();
	break;
      case 0x0D:/*ori*/
	write_value((0|*imm), TEMP_REG);
	TEMP[0] = *rs;
	TEMP[1] = TEMP_REG;
	TEMP[2] = *rt;
	//*prog_count = OR_ROUTINE;
	or ();
	break;
      case 0x0E:/*xori*/
	write_value((0|*imm), TEMP_REG);
	TEMP[0] = *rs;
	TEMP[1] = TEMP_REG;
	TEMP[2] = *rt;
	//*prog_count = XOR_ROUTINE;
	printf("%x Unimplemented Instructions xori\n",*prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	break;
#endif
      case 0x0F:/*lui*/
	write_value((*imm << 16 | 0x0000), *rt);
	break;
#if 0
      case 0x20:/*lb*///FIXME
	TEMP[0] = *rs;
	TEMP[1] = (0|*imm);
	TEMP[2] = TEMP_REG;
	//*prog_count = ADD_ROUTINE;
	write_value (get_value(TEMP_REG), *rt);
	printf("%x Unimplemented Instructions lb\n",*prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	break;
      case 0x21:/*lh*///FIXME
	write_value((0|*imm), TEMP_REG);
	TEMP[0] = *rs;
	TEMP[1] = TEMP_REG;
	TEMP[2] = TEMP_REG;
	//*prog_count = ADD_ROUTINE;
	add ();
	switch (get_value(TEMP[2])&0x00000003){
	  case 0x0:
	    temp1 = MEM0[get_value(TEMP[2])>>2];
	    temp2 = MEM1[get_value(TEMP[2])>>2];
	    temp = temp1 << 8 | temp2;
	    write_value(temp, *rt);
	    break;
	  default:
	    temp1 = MEM2[get_value(TEMP[2])>>2];
	    temp2 = MEM3[get_value(TEMP[2])>>2];
	    temp = temp1 << 8 | temp2;
	    write_value(temp, *rt);
	}
	break;
      case 0x23:/*lw*/
	write_value((signed short) *imm, TEMP_REG);
	TEMP[0] = *rs;
	TEMP[1] = TEMP_REG;
	TEMP[2] = TEMP_REG;
	//*prog_count = ADD_ROUTINE;
	add ();
	//printf("%d %d %d\n", get_value(*rs), (signed short)*imm, get_value(TEMP_REG));
	write_value (get_value(get_value(TEMP[2])>>2), *rt);
	break;
      case 0x24:/*lbu*/
	TEMP[0] = *rs;
	TEMP[1] = (0|*imm);
	TEMP[2] = TEMP_REG;
	//*prog_count = ADD_ROUTINE;
	write_value (get_value(TEMP_REG), *rt);
	printf("%x Unimplemented Instructions lbu\n",*prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	break;
      case 0x25:/*lhu*/
	write_value((0|*imm), TEMP_REG);
	TEMP[0] = *rs;
	TEMP[1] = TEMP_REG;
	TEMP[2] = TEMP_REG;
	//*prog_count = ADD_ROUTINE;
	add ();
	switch (get_value(TEMP[2])&0x00000003){
	  case 0x0:
	    temp1 = MEM0[get_value(TEMP[2])>>2];
	    temp2 = MEM1[get_value(TEMP[2])>>2];
	    temp = temp1 << 8 | temp2;
	    write_value(temp, *rt);
	    break;
	  default:
	    temp1 = MEM2[get_value(TEMP[2])>>2];
	    temp2 = MEM3[get_value(TEMP[2])>>2];
	    temp = temp1 << 8 | temp2;
	    write_value(temp, *rt);
	}
	break;
      case 0x28:/*sb*///FIXME
	write_value((0|*imm), TEMP_REG);
	TEMP[0] = *rs;
	TEMP[1] = TEMP_REG;
	TEMP[2] = TEMP_REG;
	add ();//calculate address
	switch (get_value(TEMP[2])&0x00000003){
	  case 0x0:
	    MEM0[get_value(TEMP[2])>>2] = get_value(*rt);
	    break;
	  case 0x1:
	    MEM1[get_value(TEMP[2])>>2] = get_value(*rt);
	    break;
	  case 0x2:
	    MEM2[get_value(TEMP[2])>>2] = get_value(*rt);
	    break;
	  default:
	    MEM3[get_value(TEMP[2])>>2] = get_value(*rt);
	}
	break;
      case 0x29:/*sh*///FIXME
	write_value((0|*imm), TEMP_REG);
	TEMP[0] = *rs;
	TEMP[1] = TEMP_REG;
	TEMP[2] = TEMP_REG;
	add ();//calculate address
	switch (get_value(TEMP[2])&0x00000003){
	  case 0x0:
	    MEM0[get_value(TEMP[2])>>2] = get_value(*rt) >> 8;
	    MEM1[get_value(TEMP[2])>>2] = get_value(*rt);
	    break;
	  default:
	    MEM2[get_value(TEMP[2])>>2] = get_value(*rt) >> 8;
	    MEM3[get_value(TEMP[2])>>2] = get_value(*rt);
	}
	break;
#endif
      case 0x2B:/*sw*///FIXME
	write_value((signed short)(*imm), TEMP_REG);
	TEMP[0] = *rs;
	TEMP[1] = TEMP_REG;
	TEMP[2] = TEMP_REG;
	add ();//calculate address
	//printf("%d %d %d\n",get_value(TEMP[0]),(signed short)*imm, get_value(TEMP[2]));
	write_value (get_value(*rt), get_value(TEMP[2]) >> 2);
	break;
      default:
	printf("%x Unimplemented Instructions I\n",*prog_count << 2);
	*EMULATOR_STATUS = FINISH;
	//*prog_count = *prog_count;
    }
  }
}
