/* ----------------------------------------

* File Name : list.c

* Created on : 01-05-2015

* Last Modified on : Tue 02 Jun 2015 10:32:55 AM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../config/list.h"

struct list *head = NULL;
struct list *curr = NULL;

void create_list(int val) {
  struct list *ptr = (struct list*)malloc(sizeof(struct list));
  ptr->val = val;
  ptr->next = NULL;

  head = curr = ptr;
}


void add_to_list(int val) {
  if(head == NULL){
    create_list(val);
  }
  else {
    struct list *ptr = (struct list*)malloc(sizeof(struct list));
    ptr->val = val;
    ptr->next = NULL;
  
    curr->next = ptr;
    curr = ptr;
  }
}



struct list* search_in_list(int val, struct list **prev) {
  struct list *ptr = head;
  struct list *tmp = NULL;
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


int delete_from_list(int val) {
  struct list *prev = NULL;
  struct list *del = NULL;

  del = search_in_list(val,&prev);
  if(del == NULL)
    return -1;
  else {
    if(prev != NULL)
      prev->next = del->next;
      if(del == curr) {
        curr = prev;
      }
      else if(del == head) {
	head = del->next;
      }
  }

  free(del);
  del = NULL;
  return 0;
}


void print_list(void) {
  struct list *ptr = head;
  printf("\n -------Printing list Start------- \n");
  while(ptr != NULL) {
    printf("\n [%d] \n",ptr->val);
    ptr = ptr->next;
  }
  printf("\n -------Printing list End------- \n");
  return;
}
