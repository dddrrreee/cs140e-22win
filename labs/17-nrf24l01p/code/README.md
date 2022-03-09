Unfortunately a lot of helper code.  Sorry!  You're welcome to go through
and dramatically simplify for a final project.

Crucial:
  - `nrf-driver.c`: you only have to implement these routines.  everything else
     "should just work".  Just search for `unimplemented()` calls.
  - `nrf.h`: main interface.

Helper stuff:
  - `nrf-config.h`: helpers to set and print configuration.
  - `nrf-util.c`: routines to use SPI to talk to nrf chip, and to print
    contents.
  - `nrf-internal.h`: bunch of accessor methods that you can use to set
     and get different NRF fields.
  - `nrf-regs.h`: defines the different registers and their initial values.
  - `nrf-public.c` this calls the driver and wraps up the interface to
     be a bit easier.

Stuff you don't need:
  - `msg2.h`: a message interface to hopefully make it easy to push and pop
    entries from messages.
