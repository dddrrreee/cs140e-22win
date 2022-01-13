#ifndef __RPI_INTERNAL_H__
#define __RPI_INTERNAL_H__

#include "rpi.h"

void (panic)(const char *file, int lineno, const char *msg);


// symbols defined in memmap
extern char 
            __code_start__, // code.
            __code_end__,
            __bss_start__,  // 0 initialized data
            __bss_end__,
            __data_start__, // global r/w data
            __data_end__,   
            __prog_end__,   // entire end of program: code+data.
            __heap_start__; // where the heap can start 
                            // (right now = __prog_end__)

#endif
