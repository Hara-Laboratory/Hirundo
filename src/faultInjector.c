/* ----------------------------------------

* File Name : faultInjector.c

* Created on : 23-04-2015

* Last Modified on : Fri 05 Jun 2015 10:50:57 AM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/


#include "../config/config.h"
#include "../config/listMem.h"
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
//#include "../benchmarks/vec_add.h"


extern const size_t MEM_index_end;


unsigned int prog_counter_copy;

unsigned int get_value (unsigned int);
unsigned int write_value (unsigned int, unsigned int);

void createNewMem () {
  int i;
  for (i = 0; i < MEM_index_end; i++) {
    addToMem (get_value(i));
  }
} 

void faultFreeTrace (unsigned int cycleNo, unsigned int progCount, unsigned int inst, int result, unsigned int wbLoc, unsigned int *structLocation) {
  unsigned int inFunctionStructLocation;
  addToTrace(cycleNo, progCount, inst, result, wbLoc, &inFunctionStructLocation);
  *structLocation = inFunctionStructLocation;
}


bool checkFault (unsigned int cycleNo, unsigned int progCount, unsigned int inst, int result, unsigned int wbLoc){
  bool error = checkTrace (cycleNo, progCount, inst, result, wbLoc);
  return error;
}



void updateMem (){
  int i;
  bool start = true;
  for (i = 0; i < MEM_index_end; i++) {
    unsigned int inst = copyMem(&start);
    write_value (inst, i);
  }
}


void grep_initial_memory_map(unsigned int *prog_count){
  prog_counter_copy = *prog_count;
}


void faultInjection (unsigned int *signal, unsigned int mask, unsigned char faultType, bool injectFault) {
  if ((faultType == 0x00) & (injectFault)) {//st-1
    *signal = *signal | mask;
  }
  else if ((faultType == 0x01) & (injectFault)) {//st-0
    *signal = *signal & (~mask);
  }
  else if ((faultType == 0x02) & (injectFault)) {//transient fault,bit flip
    *signal = *signal ^ mask;
  }
}

//void grep_information();
//void get_singal();
//void get_bit();
//void generate_fault();
//void inject_fault();
