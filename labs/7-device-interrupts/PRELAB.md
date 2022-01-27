## Lab 8: device interrupts

We're going to setup the pi so you can get GPIO interrupts and make your
software UART implementations more robust.

----------------------------------------------------------------------
### ARM exception handler relocation.

Having exceptions starting at address 0 causes all sorts of problems.
The general ARMv6 documents talk about how you can relocate the handlers
to a fixed address in high memory, but this requires using virtual memory
(since we don't have enough physical to reach that high).  

However, it turns out that the `arm1176` provides a coprocessor
instruction to move these anywhere in memory.  I wish I had known about
this years ago!  Among other things it would make it easy to mark the
0 page as no-access, trapping null pointer reads and writes.  

We're going to implement this approach today.   

Read:
  - 3-121 in `../../docs/arm1176.pdf`: this let's us control where the
    exception jump table is!
  - In fact, whenever you get some time and inclination, I'd read this
    whole chapter (ideally print it out and use a pen) --- there are
    all sorts of interesting tricks you can play using the special
    instructions in it.

If you can, write (in assembly):
   - `void arm_vector_set(void *base)`: set the exception vector
     base to `base`.
   - `void *arm_vector_get(void)`: get the exception vector base.

To get you started doing inline assembly:
  - [simple introduction](http://199.104.150.52/computers/gcc_inline.html)
  - [gcc arm inline assembly cookbook](../../docs/ARM-GCC-Inline-Assembler-Cookbook.pdf)

I'll have some test cases checked in tuesday.

----------------------------------------------------------------------
### Background reading for assembly / interrupts

For reading:
  1. `../../docs/hohl-book-interrupts.annot.pdf`: if you were confused
     about the interrupts, this is a good book chapter to read.
  2. `../../docs/subroutines.hohl-arm-asm.pdf`: this is a good review
     of ARM assembly as used by procedure calls: stack allocation, 
     caller and callee saved registers, parameter passing, etc.
  3. `../../docs/IHI0042F_aapcs.pdf`: this gives you a detailed view
     of the procedure call standard for the ARM.  

----------------------------------------------------------------------
### Using C to figure out useful things.

In addition, you can often simply use C code to figure some of this
stuff out.

For example: 

  1.  What register is the result returned in?   To figure this out,
      just write a simple routine that returns something and see which
      register it uses:

            % cat foo.c
            int foo(void) { return 1; }
            % arm-none-eabi-gcc -O2 -march=armv6 -c foo.c
            % arm-none-eabi-objdump -d foo.o
            00000000 <foo>:
                0:	e3a0000d 	mov	r0, #13
                4:	e12fff1e 	bx	lr

      In this case: `r0`.

  2. What register are the first four arguments passed in?

            % cat args.c
            typedef unsigned u32;
            
            u32 args1(u32 arg1) {
                return arg1;
            }
            u32 args2(u32 arg1, u32 arg2) {
                return arg2;
            }
            u32 args3(u32 arg1, u32 arg2, u32 arg3) {
                return arg3;
            }
            u32 args4(u32 arg1, u32 arg2, u32 arg3, u32 arg4) {
                return arg4;
            }


            % arm-none-eabi-gcc -O2 -march=armv6 -c args.c
            % arm-none-eabi-objdump -d args.o
            00000000 <args1>:
                0:	e12fff1e 	bx	lr
            
            00000004 <args2>:
                4:	e1a00001 	mov	r0, r1
                8:	e12fff1e 	bx	lr
            
            0000000c <args3>:
                c:	e1a00002 	mov	r0, r2
                10:	e12fff1e 	bx	lr
            
            00000014 <args4>:
                14:	e1a00003 	mov	r0, r3
                18:	e12fff1e 	bx	lr


     As you can see: the first argument is in `r0`, the second in `r1`,
     the third in `r2` and the fourth in `r3`.

  3. The fanciest, very cute hack is using inline assembly to figure
     out what is a caller or callee saved register.  See
     `prelab/caller-callee.c` and `prelab/caller-callee.list`.

----------------------------------------------------------------------
### GPIO Interface background

If you keep hacking on embedded stuff, the single most common activities
your code will do is (1) setup a hardware device and (2) then configure
device interrupts over GPIO pins (e.g., in response to setting them high,
low, transition, etc).  So today's lab will involve doing the latter:
set the GPIO pins you are doing SW-uart reception over to give you an 
interrupt when they go from low-to-high and also from high-to-low.  You
can use these transitions to detect when bits are being transmitted.

Before lab, implement four functions (prototypes are in `gpio.h` in `libpi`):

  - `gpio_int_rising_edge(pin)`: enabling rising edge detection for `pin`.  Page 97
    discusses it and the address to write to is on page 90.

  - `gpio_int_falling_edge(pin)`: enabling rising edge detection for
    `pin`.  Page 99 discusses it and the address to write to is on page 90.

  - `gpio_event_detected(pin)`: returns 1 if an event was detected (p96).

  - `gpio_event_clear(pin)`: clears the event set on `pin` (p96: write the same
    address as `gpio_event_detected`).

Don't panic.  These are pretty much like all the other GPIO functions
you've built: get an address, write a bit offset.   No big deal.  

However, just as with timer interrupts, enabling is a two-step process: we first
enable the specific GPIO interrupt(s) we care about using the first two routines
above, and then tell the pi that we care about GPIO interrupts generally.  If you 
look on page 113:
  - We want to enable `gpio_int[0]`, which is `49` in terms of general
    interrupts.  (we use `gpio[0]` since we are using a GPIO pin that
    is less than 32).

  - To set this, we bitwise-or a 1 to the `(49-32)` position in
    `Enable_IRQs_2` (which covers interrupts `[32-64)`).


This appears magic, especially since the documents are not clear.  But,
mechanically, it's just a few lines.   You should look at the timer
interrupt code to see how to set the general interrupts.

At this point you'll have interrupts enabled.

The interrupt handler works by checking if an event happened, and then using the 
`pin` value to determine if it was a rising or falling edge:
    
    if(a GPIO event was detected for pin) 
       if <pin> = 0, then
           it was a falling edge (a transition from high to low, i.e., 1 -> 0).  
       else 
           it was a rising edge (a transition from low to high).
      clear the event

As with timer interrupts, you need to clear the event that generated
the interrupt (using `gpio_event_clear`).

To make your code cleaner, I'd suggest using something like the following to
make it easier to do your `read-modify-write` of enabled pin events:

        static void or32(volatile void *addr, uint32_t val) {
            device_barrier();
            put32(addr, get32(addr) | val);
            device_barrier();
        }
        static void OR32(uint32_t addr, uint32_t val) {
            or32((volatile void*)addr, val);
        }

More complete discussion of the routines is in `gpio.h` but to make sure you
don't miss it the comments are cut-and-paste below:

    // gpio_int_rising_edge and gpio_int_falling_edge (and any other) should
    // call this routine (you must implement) to setup the right GPIO event.
    // as with setting up functions, you should bitwise-or in the value for the 
    // pin you are setting with the existing pin values.  (otherwise you will
    // lose their configuration).  you also need to enable the right IRQ.   make
    // sure to use device barriers!!
    int is_gpio_int(unsigned gpio_int);
    

    // p97 set to detect rising edge (0->1) on <pin>.
    // as the broadcom doc states, it  detects by sampling based on the clock.
    // it looks for "011" (low, hi, hi) to suppress noise.  i.e., its triggered only
    // *after* a 1 reading has been sampled twice, so there will be delay.
    // if you want lower latency, you should us async rising edge (p99)
    void gpio_int_rising_edge(unsigned pin);
    
    // p98: detect falling edge (1->0).  sampled using the system clock.  
    // similarly to rising edge detection, it suppresses noise by looking for
    // "100" --- i.e., is triggered after two readings of "0" and so the 
    // interrupt is delayed two clock cycles.   if you want  lower latency,
    // you should use async falling edge. (p99)
    void gpio_int_falling_edge(unsigned pin);
    
    // p96: a 1<<pin is set in EVENT_DETECT if <pin> triggered an interrupt.
    // if you configure multiple events to lead to interrupts, you will have to 
    // read the pin to determine which caused it.
    int gpio_event_detected(unsigned pin);
    
    // p96: have to write a 1 to the pin to clear the event.
    void gpio_event_clear(unsigned pin);


------------------
----------------------------------------------------------------------
