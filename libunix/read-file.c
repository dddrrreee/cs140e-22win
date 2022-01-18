#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libunix.h"

// read in file <name>
// returns:
//  - pointer to the code.  pad code with 0s up to the
//    next multiple of 4.  
//  - bytes of code in <size>
//
// fatal error open/read of <name> fails.
// 
// How: 
//    - use stat to get the size of the file.
//    - round up to a multiple of 4.
//    - allocate a buffer  --- 
//    - zero pads to a // multiple of 4.
//    - read entire file into buffer.  
//    - make sure any padding bytes have zeros.
//    - return it.   
//
// make sure to close the file descriptor (this will
// matter for later labs).
void *read_file(unsigned *size, const char *name) {
    unimplemented();
}
