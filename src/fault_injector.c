/* ----------------------------------------

* File Name : fault_injector.c

* Created on : 23-04-2015

* Last Modified on : Thu 23 Apr 2015 04:03:17 PM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/


#include "../config/config.h"
//#include <string.h>
//#include <stdlib.h>
//#include <stdint.h>
//#include <stdbool.h>

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

void grep_initial_memory_map(unsigned int *prog_count){
  prog_counter_copy = *prog_count;
  //*prog_count = 200;
}

void grep_information();
void get_singal();
void get_bit();
void generate_fault();
void inject_fault();
