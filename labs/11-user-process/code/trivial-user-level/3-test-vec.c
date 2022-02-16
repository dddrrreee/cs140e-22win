// vector multiply.
#include "libos.h"

#define N 16
static volatile unsigned v[N];

void notmain(void) {
    for(int i = 0; i < N; i++)
        v[i] = i;

    unsigned prod = 1;
    for(int i = 0; i < N; i++)
        prod *= v[i];

    sys_exit(prod);
}

