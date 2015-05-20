/* ----------------------------------------

* File Name : faultInjector.c

* Created on : 23-04-2015

* Last Modified on : Fri May  8 12:30:07 2015

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





unsigned int prog_counter_copy;

unsigned int get_value (unsigned int);
unsigned int write_value (unsigned int, unsigned int);

void createNewMem () {
  int i;
  for (i = 0; i < MEM_SIZE; i++) {
    addToMem (get_value(i));
  }
} 

void faultFreeTrace (unsigned int cycleNo, unsigned int progCount, unsigned int inst, int result, unsigned int wbLoc) {
  addToTrace(cycleNo, progCount, inst, result, wbLoc);
}



void updateMem (){
  int i;
  bool start = true;
  for (i = 0; i < MEM_SIZE; i++) {
    write_value (copyMem(&start), i);
  }
}


void grep_initial_memory_map(unsigned int *prog_count){
  prog_counter_copy = *prog_count;
}

void grep_information();
void get_singal();
void get_bit();
void generate_fault();
void inject_fault();
