#ifndef __RPI_H__
#define __RPI_H__
// begin defining our r/pi library.

/************************************************************
 * The following routines are written by us.  They are in 
 * start.s: look at the implementations --- they are not 
 * complicated!  
 *
 * They are used to prevent the compiler from reordering or 
 * removing operations.  gcc does not does not understand assembly 
 * and thus can't optimize it.
 */

#include <stdint.h> // uint32_t

// used to write device memory: do not use a raw store.
// writes the 32-bit value <v> to address <addr>:   
//     *(uint32_t *)addr = v;
void put32(volatile void *addr, uint32_t v);
// same, but takes <addr> as a uint32_t
void PUT32(uint32_t addr, uint32_t v);

// used to read device memory: do not use a raw dereference!
//
// returns the 32-bit value at <addr>:  
//   return *(uint32_t *)addr
uint32_t get32(const volatile void *addr);
// same but takes <addr> as a uint32_t
uint32_t GET32(uint32_t addr);

// asm routine that does nothing: used so compiler cannot 
// optimize away loops.
void nop(void);

// countdown 'ticks' cycles
static inline void delay(unsigned ticks) {
    while(ticks-- > 0)
        nop();
}

/************************************************************
 * GPIO routines: you will implement these.
 */

// set <pin> to be an output pin.
void gpio_set_output(unsigned pin);

// set <pin> to input.
void gpio_set_input(unsigned pin);

// set GPIO <pin> on.
void gpio_set_on(unsigned pin);

// set GPIO <pin> off
void gpio_set_off(unsigned pin);

// set <pin> to <v> (v \in {0,1})
void gpio_write(unsigned pin, unsigned v);

// return the value of <pin>.
int gpio_read(unsigned pin);

#endif
