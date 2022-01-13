#ifndef __MATH_HELPERS_H__
#define __MATH_HELPERS_H__

static uint32_t  abs_diff_u(uint32_t  x, uint32_t  y) {
    return (x>y) ?  x - y : y - x;
}

#endif
