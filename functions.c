/* ----------------------------------------

* File Name : functions.c

* Created on : 14-01-2015

* Last Modified on : Mon 02 Feb 2015 08:46:44 AM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/


#include "config.h"


void add (){
  unsigned int a = get_value(TEMP[0]);
  unsigned int b = get_value(TEMP[1]);

  unsigned int c = a + b;

	//printf("%d %d %d\n", a , b, c);

  write_value (c,TEMP[2]);
}


void sub (){
  unsigned int a = get_value(TEMP[0]);
  unsigned int b = get_value(TEMP[1]);

  unsigned int c = a - b;

  write_value (c,TEMP[2]);
}


void mult (){
  unsigned int a = get_value(TEMP[0]);
  unsigned int b = get_value(TEMP[1]);

  unsigned long long c = a * b;

  write_value (c & 0x00000000FFFFFFFF, TEMP[2]);
  write_value (c >> 32,TEMP[3]);
}



void or (){
  unsigned int a = get_value(TEMP[0]);
  unsigned int b = get_value(TEMP[1]);

  unsigned int c = a | b;

  write_value (c,TEMP[2]);
}



void and (){
  unsigned int a = get_value(TEMP[0]);
  unsigned int b = get_value(TEMP[1]);

  unsigned int c = a & b;

  write_value (c,TEMP[2]);
}


void xor (){
  unsigned int a = get_value(TEMP[0]);
  unsigned int b = get_value(TEMP[1]);

  unsigned int c = a ^ b;

  write_value (c,TEMP[2]);
}


void srl (){
  unsigned int a = get_value(TEMP[0]);
  unsigned int b = get_value(TEMP[1]);
  
  unsigned int c = a >> b;

  write_value (c,TEMP[2]);
}

void sll(){
  unsigned int a = get_value(TEMP[0]);
  unsigned int b = get_value(TEMP[1]);
  
  unsigned int c = a << b;

  write_value (c,TEMP[2]);
}

void sra (){
  unsigned int a = get_value(TEMP[0]);
  unsigned int b = get_value(TEMP[1]);

  unsigned int c;

  if (a >> 1 == 1){
    c = (a >> b) | (0xFFFFFFFF << (32-b));
  }
  else {
    c = a >> b;
  }
  write_value (c,TEMP[2]);
}
