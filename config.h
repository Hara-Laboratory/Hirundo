/* ----------------------------------------

* File Name : config.h

* Created on : 06-01-2015

* Last Modified on : Tue Mar 10 19:29:44 2015

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include <stdio.h>

#define	VERIFICATION

//#define	USE_SUBLEQ

//#define	USE_ADDER
//#define USE_SUB
//#define USE_MULT
//#define USE_MFLO
//#define USE_MFHI
#define USE_MTLO
#define USE_MTHI
//#define USE_SHIFTER

//#define MEM_SIZE	1024//0xe000000
#define MEM_SIZE	0x4000//3800000//1024//0xe000000
//unsigned int MEM[MEM_SIZE];
//unsigned char MEM0[MEM_SIZE];
//unsigned char MEM1[MEM_SIZE];
//unsigned char MEM2[MEM_SIZE];
//unsigned char MEM3[MEM_SIZE];

//unsigned int TEMP[8];
//
unsigned int TEMP0;
unsigned int TEMP1;
unsigned int TEMP2;
unsigned int TEMP3;
unsigned int TEMP4;
unsigned int TEMP5;
unsigned int TEMP6;
unsigned int TEMP7;

#define NORMAL 0x01
#define FINISH 0x02
#define FORCE  0x03

#define RFILE_START	0
#define LO		32
#define	HI		33
#define TEMP_REG	34//holds imm values
#define Y		35//use in subleq machine 
#define	Z		36//use in subleq machine
#define DEST_LOC	37//holds the return pc
#define SRC1_LOC	38//use in subleq machine 
#define	SRC2_LOC	39//use in subleq machine

#define ADD_ROUTINE	898
#define MUL_ROUTINE	469
#define	SUB_ROUTINE	256
#define MFLO_ROUTINE	874
#define MFHI_ROUTINE	886
#define SLL_ROUTINE	436
#define SRL_ROUTINE	274
#define SRA_ROUTINE	346
#define SLLV_ROUTINE	317
#define SRLV_ROUTINE	317
#define SRAV_ROUTINE	317
#define XOR_ROUTINE	317
#define NOR_ROUTINE	317
#define LUI_ROUTINE	317
#define SLT_ROUTINE	317

#define START_PC	700

typedef	unsigned int	uint;
typedef	unsigned char	uchar;
typedef	unsigned short	ushort;
typedef	unsigned long	ulong;



unsigned int ADD[15] = {
    SRC1_LOC, Z, ADD_ROUTINE+3,
    SRC2_LOC, Z, ADD_ROUTINE+6,
    DEST_LOC, DEST_LOC, ADD_ROUTINE+9,
    Z, DEST_LOC, ADD_ROUTINE+12,
    Z, Z, 999//(-1)
};

