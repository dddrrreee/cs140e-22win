#ifndef __FAKE_PI_H__
#define __FAKE_PI_H__
// for the moment: don't modify this (we may check in variations)

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "fake-mem.h"
#include "fake-random.h"

// there is no assembly or other weird things in rpi.h so we can
// include and use it on your laptop even tho it is intended for
// the pi.
#include "rpi.h"

#define random  "do not call directly"

// what is the deal with just including demand.h?

#if 0
#include "demand.h"
#endif

// macro tricks to make error reporting easier.
#undef output
#undef trace
#undef panic
#undef debug
#define output(msg, args...) printf(msg, ##args )
#define trace(msg, args...) fprintf(stderr, "TRACE:" msg, ##args )

#define panic(msg, args...) do {                                         \
    printf("PANIC:%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__);   \
    printf(msg, ##args);                                                \
    exit(1);                                                        \
} while(0)

#define debug(msg, args...) do {                                         \
    printf("debug:%s:%s:%d:", __FILE__, __FUNCTION__, __LINE__);   \
    printf(msg, ##args);                                                \
} while(0)

// all the routines should call this before starting: right now doesn't do
// much.
static inline void fake_init(void) {
    fake_random_init();
}

void notmain(void);

#endif
