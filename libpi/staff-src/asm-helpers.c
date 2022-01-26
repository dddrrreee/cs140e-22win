#include "rpi.h"

void asm_not_reached_helper(uint32_t pc) {
    panic("impossible: reached pc=%x\n", pc);
}
void asm_not_implemented_helper(uint32_t pc) {
    panic("unimplemented: pc=%x\n", pc);
}
