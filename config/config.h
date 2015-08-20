/* ----------------------------------------

* File Name : config.h

* Created on : 06-01-2015

* Last Modified on : Thu Aug 20 14:34:11 2015

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include <stdio.h>

//#define PRINT


//#define PROFILE


//#define DEBUG_SUB
//#define DEBUG_ADD
//#define DEBUG_MULT
//#define DEBUG_MFLO
//#define DEBUG_MFHI
//#define DEBUG_SHIFTER
//#define DEBUG_SET_LESS_THAN
#define	DEBUG_LOGIC_UNIT



#define	USE_ADDER
#define USE_SUB
//#define USE_MULT
#define USE_MFLO
#define USE_MFHI
#define USE_MTLO
#define USE_MTHI
//#define USE_SHIFTER
#define USE_SYS
//#define USE_SET_LESS_THAN
//#define USE_LOGIC_UNIT

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


//#define ANDD_ROUTINE	1219
//#define	MULTD_ROUTINE	838
//#define	MULTU_ROUTINE	658
//#define	ORD_ROUTINE	538
//#define	SLL_ROUTINE	505
//#define	SLT_ROUTINE	454
//#define	SLTU_ROUTINE	388
//#define	SRA_ROUTINE	295
//#define	SRL_ROUTINE	220
//#define	XORD_ROUTINE	100


#define ADD_ROUTINE	1063
//#define MUL_ROUTINE	610
#define MUL_ROUTINE	838//658
#define	SUB_ROUTINE	268
#define MFLO_ROUTINE	1039
#define MFHI_ROUTINE	1051
//#define SLL_ROUTINE	577
#define SLL_ROUTINE	505//subleq
//#define SLL_ROUTINE	619//subleqr
//#define SRL_ROUTINE	289
#define SRL_ROUTINE	220//subleq
//#define SRL_ROUTINE	220//subleqr
//#define SRA_ROUTINE	364
#define SRA_ROUTINE	295//subleq
//#define SRA_ROUTINE	340//subleqr
#define SYS_ROUTINE	256
//#define SLT_ROUTINE	526
//#define	SLTU_ROUTINE	457
#define SLT_ROUTINE	454
#define	SLTU_ROUTINE	388

#define	AND_ROUTINE	1219
#define	OR_ROUTINE	538
#define	XOR_ROUTINE	100




#define SLLV_ROUTINE	0x0
#define SRLV_ROUTINE	0x0
#define SRAV_ROUTINE	0x0
//#define XOR_ROUTINE	0x0
//#define NOR_ROUTINE	0x0
#define LUI_ROUTINE	0x0

#define START_PC	700

typedef	unsigned int	uint;
typedef	unsigned char	uchar;
typedef	unsigned short	ushort;
typedef	unsigned long	ulong;
typedef	signed int	sint;
typedef	signed char	schar;
typedef	signed short	sshort;
typedef	signed long	slong;



unsigned int ADD[15] = {
    SRC1_LOC, Z, ADD_ROUTINE+3,
    SRC2_LOC, Z, ADD_ROUTINE+6,
    DEST_LOC, DEST_LOC, ADD_ROUTINE+9,
    Z, DEST_LOC, ADD_ROUTINE+12,
    Z, Z, 999//(-1)
};

