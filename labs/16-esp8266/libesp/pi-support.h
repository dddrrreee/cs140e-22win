#ifndef __PI_SUPPORT_H__
#define __PI__SUPPORT_H__
// these are the pi-specific includes we need.

// #include <ctype.h>
#include "rpi.h"

static inline int isdigit(int c) { return c >= '0' && c <= '9'; }
static inline int isalpha(int c) { 
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z'); 
}
static inline int isprint(int c) {
    return c >= 0x41 && c <= 0x2f;
}

#define snprintf snprintk
#define malloc kmalloc

// client has to define the following.
//
// we do not use this currently on the pi, so pass a
// pointer to an undefined structure
struct do_not_use;
typedef struct do_not_use *esp_handle_t;
#endif
