#include "fake-pi.h"

void run_test(const char *name, void (*fp)(unsigned), int ntrials) {
    printf("testing: <%s>\n", name);
    // test pins 0..32, then a bunch of random.
    for(int i = 0; i < 32; i++) 
        fp(i);
    for(int i = 0; i < ntrials; i++) 
        fp(fake_random());
}

// can refactor this code, but we keep it simple.
void test_gpio_set_output(int ntrials) {
    run_test(__FUNCTION__, gpio_set_output, ntrials);
}
void test_gpio_set_input(int ntrials) {
    run_test(__FUNCTION__, gpio_set_input, ntrials);
}
void test_gpio_set_on(int ntrials) {
    run_test(__FUNCTION__, gpio_set_on, ntrials);
}
void test_gpio_set_off(int ntrials) {
    run_test(__FUNCTION__, gpio_set_off, ntrials);
}

void test_gpio_set_function(int ntrials) {
    printf("testing: <%s>\n", __FUNCTION__);
    // test pins 0..32, then a bunch of fake_random.
    for(int pin = 0; pin < 32; pin++)  {
        for(int func = 0;  func < 16; func++) {
            gpio_set_function(pin, func);
            gpio_set_function(fake_random(), func);
        }
    }
    for(int i = 0; i < ntrials; i++) 
        gpio_set_function(fake_random(), fake_random()%6);
}

int main(int argc, char *argv[]) {
    fake_random_init();
#   define N 128
    test_gpio_set_input(N);
    test_gpio_set_output(N); 
    test_gpio_set_on(N); 
    test_gpio_set_off(N);
    test_gpio_set_function(N);
    return 0;
}
