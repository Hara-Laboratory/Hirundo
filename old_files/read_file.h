/* ----------------------------------------

* File Name : read_file.h

* Created on : 08-05-2014

* Last Modified on : Wed 14 Jan 2015 04:47:27 PM JST

* Primary Author : Tanvir Ahmed 
* Email : tanvira@ieee.org
------------------------------------------*/

//#include<iostream>
//#include<string>

//using namespace std;

#define ALOCLIMIT       0x0d000000      /* default size is 218MB   */
#define MINADDR         0x00001000      /* minimum address is 4KB  */
#define MEMSIZE         0x0e000000      /* default size is 234MB   */
#define HDRADDR		0x00001000

//#define MAX_MEM_REGIONS 16
//
#define TEMP_MEMSIZE	0xe000000

unsigned char i_mem[TEMP_MEMSIZE];

