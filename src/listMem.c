/* ----------------------------------------

* File Name : listMem.c

* Created on : 01-05-2015

* Last Modified on : Thu 04 Jun 2015 12:09:26 PM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../config/listMem.h"
#include "../config/config.h"

struct listMem *headMem = NULL;
struct listMem *currMem = NULL;

void createMem (int val) {
  struct listMem *ptr = (struct listMem*)malloc(sizeof(struct listMem));
  ptr->val = val;
  ptr->next = NULL;
  headMem = currMem = ptr;
}


void addToMem (int val) {
  if(headMem == NULL){
    createMem (val);
  }
  else {
    struct listMem *ptr = (struct listMem*)malloc(sizeof(struct listMem));
    ptr->val = val;
    ptr->next = NULL;
    currMem->next = ptr;
    currMem = ptr;
  }
}



unsigned int copyMem (bool *start) {
  unsigned int inst = 0;
  if (*start){
    currMem = headMem;
    *start = false;
  }

  if (currMem != NULL) {
    inst = currMem->val;
    //printf("%8.8x\n",currMem->val);
    currMem = currMem->next;
  }
  //printf("TEST OK\n");
  return inst;
}

//need to modify for fault injector
struct listMem* searchMem (int val, struct listMem **prev) {
  struct listMem *ptr = headMem;
  struct listMem *tmp = NULL;
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


int deleteMemory (int val) {
  struct listMem *prev = NULL;
  struct listMem *del = NULL;

  del = searchMem (val,&prev);
  if(del == NULL)
    return -1;
  else {
    if(prev != NULL)
      prev->next = del->next;
      if(del == currMem) {
        currMem = prev;
      }
      else if(del == headMem) {
	headMem = del->next;
      }
  }

  free(del);
  del = NULL;
  return 0;
}


void printMem (void) {
  struct listMem *ptr = headMem;
  printf("\n -------Printing list Start------- \n");
  while(ptr != NULL) {
    printf("\n [%d] \n",ptr->val);
    ptr = ptr->next;
  }
  printf("\n -------Printing list End------- \n");
  return;
}
