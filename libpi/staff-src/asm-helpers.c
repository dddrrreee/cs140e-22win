#include "rpi.h"

void asm_not_reached(uint32_t pc) {
    panic("impossible: reached pc=%x\n", pc);
}
void asm_unimplemented(uint32_t pc) {
    panic("unimplemented: pc=%x\n", pc);
}
