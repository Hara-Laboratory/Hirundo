/* ----------------------------------------

* File Name : listTrace.h

* Created on : 01-05-2015

* Last Modified on : Fri 05 Jun 2015 10:43:32 AM JST

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



//struct listTrace *headTrace = NULL;
//struct listTrace *currTrace = NULL;


void printTrace (void);
void addToTrace (unsigned int, unsigned int, unsigned int, int, unsigned int, unsigned int*);
bool checkTrace (unsigned int, unsigned int, unsigned int, int, unsigned int);
//struct listTrace* searchTrace (int, struct listTrace**);
int deleteTrace (int);
void createTrace (unsigned int, unsigned int, unsigned int, int, unsigned int);

//struct test_struct *head = NULL;
//struct test_struct *curr = NULL;
