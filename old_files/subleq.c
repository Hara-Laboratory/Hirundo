/* ----------------------------------------

* File Name : subleq.c

* Created on : 06-01-2015

* Last Modified on : Fri 09 Jan 2015 12:31:35 PM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include "config.h"


void subleq(unsigned int prog_count){
  prog_count &= MEM_SIZE-1;
  while(prog_count != 999){
    prog_count &= (MEM_SIZE - 1);
    unsigned int a = get_value (prog_count);
    unsigned int b = get_value (prog_count + 0x1);
    unsigned int c = get_value (prog_count + 0x2);

    signed int src1 = get_value (TEMP[a]);
    signed int src2 = get_value (TEMP[b]);

    src2 = src2- src1;
    printf("%d\n", src2);

    write_value (src2, TEMP[b]);
    prog_count = (src2 > 0) ? prog_count + 0x3 : c;
  }
}
