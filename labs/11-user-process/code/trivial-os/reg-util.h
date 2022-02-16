// add whatever helpers you need here.
#include "rpi-constants.h"

static inline unsigned reg_get_sp(void) {
    unsigned sp;
    asm volatile ("mov %0, sp" : "=r" (sp));
    return sp;
}

static inline unsigned reg_get_cpsr(void) {
    unsigned cpsr;
    asm volatile ("mrs %0, cpsr" : "=r" (cpsr));
    return cpsr;
}

static inline unsigned reg_get_mode(void) {
    return reg_get_cpsr() & 0b11111;
}
