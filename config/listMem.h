/* ----------------------------------------

* File Name : listMem.h

* Created on : 01-05-2015

* Last Modified on : Fri 05 Jun 2015 10:43:34 AM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include <stdbool.h>


struct listMem {
  int val;
  struct listMem *next;
};

void printMem (void);
void addToMem (int);
unsigned int copyMem (bool*);

struct listMem* searchMem (int, struct listMem**);
int deleteMem (int);
void createMem (int);

//struct test_struct *head = NULL;
//struct test_struct *curr = NULL;
