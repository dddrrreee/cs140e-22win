#include "rpi.h"
#include "libesp.h"

// used only so the same code can run on the pi and on
// linux.   we currently assume the output is to 
// a sw uart and the esp is on hw.
void notmain(void) {
    esp_handle_t x = 0;

    lex_t l;
    lex_init(&l, x);
    esp_main(&l);
}
