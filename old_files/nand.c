/* ----------------------------------------

* File Name : nand.c

* Created on : 07-01-2015

* Last Modified on : Wed 07 Jan 2015 09:58:22 AM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/


#include "config.h"


void nand(unsigned int prog_count){
  while (prog_count != 999) {
    int a = get_value (prog_count);
    int b = get_value (prog_count + 0x1);
    int c = get_value (prog_count + 0x2);

    int src1 = get_value (TEMP[a]);
    int src2 = get_value (TEMP[b]);

    src2 = ~(src2 & src1);
    printf("%d\n", src2);

    write_value (src2, TEMP[b]);
    prog_count = c;
  }
}
