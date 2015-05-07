/* ----------------------------------------

* File Name : listMem.h

* Created on : 01-05-2015

* Last Modified on : Thu 07 May 2015 12:45:54 PM JST

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
