#ifndef __LIBUNIX_H__
#define __LIBUNIX_H__
// engler: put all the prototypes you define in this header file.
#include <assert.h>

// staff-provided prototypes and routines.
#include "staff-libunix.h"

//
// You need to implement:
//      - find_ttyusb* (in find-ttyusb.c)
//      - read_file  (in read-file.c)
//


// looks in /dev for a ttyusb device. 
// returns:
//  - device name.
// panic's if 0 or more than 1.
char *find_ttyusb(void);
char *find_ttyusb_first(void);
char *find_ttyusb_last(void);

// read in file <name>
// returns:
//  - pointer to the code.  pad code with 0s up to the
//    next multiple of 4.  
//  - bytes of code in <size>
//
// fatal error open/read of <name> fails.
void *read_file(unsigned *size, const char *name);


// create file <name>: truncates if already exists.
int create_file(const char *name);
FILE *fcreate_file(const char *name);

// if you want bit-manipulation routines.
#include "bit-support.h"

// uncomment if you want time macros
// #include "time-macros.h"


// add any other prototypes you want!





#endif

