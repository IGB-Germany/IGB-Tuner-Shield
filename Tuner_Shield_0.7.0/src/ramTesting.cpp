/*Calculate available free RAM*/
#include "ramTesting.h"

//https://www.codeproject.com/articles/1013667/optimize-arduino-memory-usage

int freeRam ()
{
  //.data variables is the first RAM section and it is used to store program static data,
  //such as strings, initialized structures and global variables
  
  //__bss_end is the end of memory allocated for uninitialized global and static variables
  extern unsigned int __bss_start;
  extern unsigned int __bss_end;
  
  extern unsigned int __heap_start;
  extern unsigned int *__brkval;
  //test variable created on stack
  unsigned int newVariable = 0;
//Heap
//Stack

  // heap is empty, use bss_end as start memory address
  //if (__brkval == 0) return ((unsigned int)(&newVariable) - (&__bss_end));
  //use heap end as the start of the memory address
  //else return ((unsigned int)(&newVariable) - (__brkval));
  
  return (unsigned int) &newVariable - (__brkval == 0 ? (unsigned int) &__heap_start : (unsigned int) __brkval);
}
