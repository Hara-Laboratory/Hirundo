/* ----------------------------------------

* File Name : list.h

* Created on : 01-05-2015

* Last Modified on : Fri May  1 14:00:48 2015

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/


struct list {
  unsigned int cycle_no;
  unsigned int prog_count;
  unsigned int inst;
  unsigned int src1;
  unsigned int src2;
  unsigned int src3;
  unsigned int res;
  unsigned int ret_add;
  int val;
  struct list *next;
};

void print_list(void);
void add_to_list(int);
struct list* search_in_list(int, struct list**);
int delete_from_list(int);
void create_list(int);

//struct test_struct *head = NULL;
//struct test_struct *curr = NULL;
