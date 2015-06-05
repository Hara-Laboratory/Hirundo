/* ----------------------------------------

* File Name : listTrace.c

* Created on : 01-05-2015

* Last Modified on : Fri 05 Jun 2015 10:50:56 AM JST

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
  //printf("%8.8x\n",currTrace);
}


void addToTrace (unsigned int cycleNo, unsigned int progCount, unsigned int inst, int result, unsigned int wbLoc, unsigned int *structLocation) {
  if (headTrace == NULL){
    createTrace(cycleNo, progCount, inst, result, wbLoc);
    //printf("%8.8x\n",headTrace);
  }
  else {
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
  //printf("%8.8x\n",headTrace);
}


bool checkTrace (unsigned int cycleNo, unsigned int progCount, unsigned int inst, int result, unsigned int wbLoc) {
  bool error = false;
  //bool found = false;
  //printf("%8.8x\n",headTrace);
  struct listTrace *ptrTrace = headTrace;

  //printf("%x\n",headTrace);
  //if (headTrace)
    //printf("HEAD CHECKK\n");

  //currTrace = headTrace;

  while (ptrTrace != NULL) {
    //printf("CHECK POINT\n");
    if (ptrTrace->cycleNo == cycleNo){
      bool progCountError = (ptrTrace->progCount != progCount) ? true : false;
      bool instError = (ptrTrace->inst != inst) ? true : false;
      bool resultError = (ptrTrace->result != result) ? true : false;
      bool wbLocError = (ptrTrace->wbLoc != wbLoc) ? true : false;
      error = progCountError | instError | resultError | wbLocError;
      //printf("fault detected\n");
      //found = true;
      break;
    }
    else {
      ptrTrace = ptrTrace->next;
    }
  }


  return error;
  
}

/*
struct listTrace* searchTrace (int cycleNo, struct listTrace **prev) {
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
}*/

#if 0
//need to modify for fault injector
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
#endif

void printTrace (void) {
  struct listTrace *ptr = headTrace;
  printf("\n -------Printing list Start------- \n");
  while(ptr != NULL) {
    printf("\n [%d] \n",ptr->cycleNo);
    ptr = ptr->next;
  }
  printf("\n -------Printing list End------- \n");
  return;
}
