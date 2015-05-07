/* ----------------------------------------

* File Name : listTrace.h

* Created on : 01-05-2015

* Last Modified on : Thu 07 May 2015 11:37:06 AM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/


struct listTrace {
  unsigned int cycleNo;
  unsigned int progCount;
  unsigned int inst;
  int result;
  unsigned int wbLoc;
  int val;
  struct listTrace *next;
};

void printTrace (void);
void addToTrace (unsigned int, unsigned int, unsigned int, int, unsigned int);
struct listTrace* searchTrace (int, struct listTrace**);
int deleteTrace (int);
void createTrace (unsigned int, unsigned int, unsigned int, int, unsigned int);

//struct test_struct *head = NULL;
//struct test_struct *curr = NULL;
