// simple test for your put/get implementation.
#include "fake-pi.h"


int main(void) {
    // make sure that everyone has the same random.
    fake_random_init();

#   define N 128

    uint32_t addrs[N], vals[N];
    for(int i = 0; i < N; i++) {
        addrs[i] = fake_random();

        if(fake_random()%2 == 0)
            vals[i] = GET32(addrs[i]);
        else
            PUT32(addrs[i], vals[i] = fake_random());
    }
    for(int i = 0; i < N; i++)
        assert(GET32(addrs[i]) == vals[i]);

    return 0;
}
