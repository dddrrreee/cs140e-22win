#ifndef __VECTOR_BASE_SET_H__
#define __VECTOR_BASE_SET_H__
#include "libc/bit-support.h"
#include "asm-helpers.h"

/*
 * vector base address register:
 *   3-121 --- let's us control where the exception jump table is!
 *
 * defines: 
 *  - vector_base_set  
 *  - vector_base_get
 */

cp_asm_set(vector_base_asm, p15, 0, c12, c0, 0)
cp_asm_get(vector_base_asm, p15, 0, c12, c0, 0)

static inline void *vector_base_get(void) {
    return (void*)vector_base_asm_get();
}

// check alignment.
static inline void vector_base_set(void *vector_base) {
    assert(vector_base);

    uint32_t v = (uint32_t)vector_base;
    if(bits_get(v, 0, 4) != 0)
        panic("lower 5 bits of the vector base should be 0, have: %x\n", v);
    vector_base_asm_set(v);
    assert(vector_base_get() == vector_base);
}

#endif
