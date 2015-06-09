/* ----------------------------------------

* File Name : listMem.h

* Created on : 01-05-2015

* Last Modified on : Sat 06 Jun 2015 03:39:43 PM JST

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
void delListMem (void);

//struct test_struct *head = NULL;
//struct test_struct *curr = NULL;
