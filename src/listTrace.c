/* ----------------------------------------

* File Name : listTrace.c

* Created on : 01-05-2015

* Last Modified on : Thu 07 May 2015 11:37:18 AM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../config/listTrace.h"

struct listTrace *headTrace = NULL;
struct listTrace *currTrace = NULL;

void createTrace (unsigned int cycleNo, unsigned int progCount, unsigned int inst, int result, unsigned int wbLoc) {
  struct listTrace *ptr = (struct listTrace*)malloc(sizeof(struct listTrace));
  ptr->cycleNo = cycleNo;
  ptr->progCount = progCount;
  ptr->inst = inst;
  ptr->result = result;
  ptr->wbLoc = wbLoc;
  ptr->next = NULL;

  headTrace = currTrace = ptr;
}


void addToTrace(unsigned int cycleNo, unsigned int progCount, unsigned int inst, int result, unsigned int wbLoc) {
  if(headTrace == NULL)
    createTrace(cycleNo, progCount, inst, result, wbLoc);
  struct listTrace *ptr = (struct listTrace*)malloc(sizeof(struct listTrace));
  ptr->cycleNo = cycleNo;
  ptr->progCount = progCount;
  ptr->inst = inst;
  ptr->result = result;
  ptr->wbLoc = wbLoc;
  ptr->next = NULL;
  
  currTrace->next = ptr;
  currTrace = ptr;
}


//need to modify for fault injector
struct listTrace* searchTrace (int val, struct listTrace **prev) {
  struct listTrace *ptr = headTrace;
  struct listTrace *tmp = NULL;
  bool found = false;
  
  while(ptr != NULL) {
    if(ptr->val == val) {
      found = true;
      break;
    }
    else {
      tmp = ptr;
      ptr = ptr->next;
    }
  }

  if(found == true) {
    if(prev)
      *prev = tmp;
    return ptr;
  }
  else {
    return NULL;
  }
}


int deleteTrace (int val) {
  struct listTrace *prev = NULL;
  struct listTrace *del = NULL;

  del = searchTrace (val,&prev);
  if(del == NULL)
    return -1;
  else {
    if(prev != NULL)
      prev->next = del->next;
      if(del == currTrace) {
        currTrace = prev;
      }
      else if(del == headTrace) {
	headTrace = del->next;
      }
  }

  free(del);
  del = NULL;
  return 0;
}


void printTrace (void) {
  struct listTrace *ptr = headTrace;
  printf("\n -------Printing list Start------- \n");
  while(ptr != NULL) {
    printf("\n [%d] \n",ptr->val);
    ptr = ptr->next;
  }
  printf("\n -------Printing list End------- \n");
  return;
}
