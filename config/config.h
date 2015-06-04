/* ----------------------------------------

* File Name : config.h

* Created on : 06-01-2015

* Last Modified on : Thu 04 Jun 2015 09:54:26 AM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include <stdio.h>
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
//#include "../benchmarks/vec_add.h"


//#define	USE_ADDER
//#define USE_SUB
//#define USE_MULT
//#define USE_MFLO
//#define USE_MFHI
//#define USE_MTLO
//#define USE_MTHI
//#define USE_SHIFTER
//#define USE_SYS
//#define USE_SET_LESS_THAN


#define FAULT_ANALYZER
#define BIT
#define ADDRESS
#define DATA
#define BUS


void createNewMem (void);
void faultFreeTrace (unsigned int, unsigned int, unsigned int, int, unsigned int);
void updateMem (void);
void faultInjection(unsigned int*, unsigned int, unsigned char, bool);




//#define MEM_SIZE	1024//0xe000000
#define MEM_SIZE	0x8000//3800000//1024//0xe000000
//unsigned int MEM[MEM_SIZE];
//unsigned char MEM0[MEM_SIZE];
//unsigned char MEM1[MEM_SIZE];
//unsigned char MEM2[MEM_SIZE];
//unsigned char MEM3[MEM_SIZE];


#ifdef FAULT_ANALYZER
unsigned int MEM_COPY[MEM_SIZE];
#endif

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

#define ADD_ROUTINE	1063
#define MUL_ROUTINE	610
#define	SUB_ROUTINE	268
#define MFLO_ROUTINE	1039
#define MFHI_ROUTINE	1051
#define SLL_ROUTINE	577
#define SRL_ROUTINE	289
#define SRA_ROUTINE	364
#define SYS_ROUTINE	256
#define SLT_ROUTINE	526
#define	SLTU_ROUTINE	457

#define SLLV_ROUTINE	0x0
#define SRLV_ROUTINE	0x0
#define SRAV_ROUTINE	0x0
#define XOR_ROUTINE	0x0
#define NOR_ROUTINE	0x0
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
