// wrapper for the our local random() implementation (in pi-random.c).
#include "fake-random.h"
#include "pi-random.h"

#include <string.h>
#include <assert.h>
#include <stdlib.h>


#define STATESIZE 128
static int seed = 0;
static char statebuf[STATESIZE];
static struct pi_random_data r;
static int init_p = 0;

void fake_random_seed(unsigned x) {
    memset(&r, 0, sizeof r);
    if(pi_initstate_r(seed, statebuf, STATESIZE, &r))
        assert(0);
    if(pi_srandom_r(seed, &r))
        assert(0);
}

// make sure that everyone has the same random.
void fake_random_init(void) {
    init_p = 1;
    fake_random_seed(0);
    unsigned u = fake_random();
    assert(0x6b8b4567 == u);
}

static unsigned nrandom_calls;
unsigned fake_random_calls(void) {
    return nrandom_calls;
}

unsigned fake_random(void) {
    assert(init_p);

    nrandom_calls++;
    int x;
    if(pi_random_r(&r, &x))
        assert(0);
    return x;
}

