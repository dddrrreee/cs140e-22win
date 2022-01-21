/*                  DO NOT MODIFY THIS CODE.
 *                  DO NOT MODIFY THIS CODE.
 *                  DO NOT MODIFY THIS CODE.
 *                  DO NOT MODIFY THIS CODE.
 *
 * engler, cs140e: simple driver for your get_code implementation,
 * which does the actual getting and loading of the code.
 *
 * much more robust than xmodem, which seems to have bugs in terms of 
 * recovery with inopportune timeouts.
 */
#include "rpi.h"
#include "staff-crc32.h"        // has the crc32 implementation.
#include "simple-boot-defs.h"   // protocol values.

/*
 * input output definitions for get_code: this might look a bit
 * weird, but it allows us to repurpose the get_code implementation
 * for other devices later on and not pay any overhead (the pi is
 * slow and the input device might not have much buffering)
 */
// blocking calls to send / receive a single byte from the uart.
#define boot_put8       uart_putc
#define boot_get8       uart_getc
// non-blocking: returns 1 if there is data, 0 otherwise.
#define boot_has_data   uart_has_data

// put this after you define <boot_put8>, <boot_get8> 
// and <boot_has_data>.  a hack, but it lets us keep all
// the names local (private) to this file.
#include "get-code.h"

void notmain(void) {
    uart_init();

    long addr;
    if((addr = get_code()) < 0)
        rpi_reboot();

	// We used to have these delays to stop the unix side from getting 
    // confused.  I believe it's b/c the code we call re-initializes the 
    // uart.  Instead we now flush the hardware tx buffer.   If this
    // isn't working, put the delay back.  However, it makes it much faster
    // to test multiple programs without it.
    // delay_ms(500);
    uart_flush_tx();

    // run what we got.
    BRANCHTO(addr);

    // should not get back here, but just in case.
    rpi_reboot();
}
