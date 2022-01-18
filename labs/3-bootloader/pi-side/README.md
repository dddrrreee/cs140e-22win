Finish implementing the code in:
   - `get-code.h`, which has the routine `get_code` that does the actual loading.
     This is the only file you'll have to modify:

You'll have to look at:
   - `simple-boot.h`: the values used for the protocol (refer to the lab's `README` for
      how to use them). 
   - `bootloader.c`: the driver to call `get_code`.
   - `staff-crc32.h`: code to compute the checksum.  You'll have to call this.

The other files are from us:
   - `staff-start.S`:  look at how it differs from the one used by `1-gpio`.
   - `libpi.small` a stripped down `libpi`.   Your `gpio.o` could be used instead
     and the `uart.o` you write next lab will replace the rest.
