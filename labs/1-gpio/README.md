## Lab: write your own code to control the r/pi; throw ours out.

**Important: as always, read and complete the [PRELAB](PRELAB.md)
before lab!**

The first lab was just setup.  Now we get to the fun part: you'll use
the Broadcom document (`../../docs/BCM2835-ARM-Peripherals.annot.PDF`)
to figure out how to write the code to turn the GPIO pins on/off yourself
as well as reading them to get values produced by a digital device.
You'll use this code to blink an LED and to detect when a capacitive
touch sensor is touched.

Sign off: to get credit for the lab show the following:

   1. That `code/2-blink.c` correctly blinks two LEDs on pin 20 and 21 in
      opposite orders (i.e., if 20 is on, 21 should be off and vice
      versa).  (This will point out a subtle mistake people make reading
      the docs).

   2. That `code/3-input.c` turns on an LED connected to pin 20 when
      pin 21 is connected to 3v (either directly, via an LED, or via a
      touch sensor or whatever other device you might want to try).

   3. That you can forward the signal from one pi to another.
      This requires the ability to run two pi's at once (as described
      in 0-blink).

      Not everyone will be able to do this if you need to buy additional
      adaptors for your laptop --- make sure you can do this part very
      soon, since we need two pi's at once for networking.

--------------------------------------------------------------------------
### Part 0.  Background on how to think about the Broadcom document.

The r/pi, like most processors has a bunch of different devices it can
control (e.g., the GPIO pins we've been using, the SD card reader, etc.)
Obviously, to use these devices, the pi must have a way to communicate
with them.

An old-school, obsolete approach for device communication is to have
special assembly instructions that the pi CPU could issue.  This sucks,
since each new device needs its own set of instructions.

Instead, modern systems use the following hack: they give each device
its own chunk of the physical address space and code can read or write
these locations with magic, device-specific values to communicate with
the device.   

For example, to turn on GPIO pin 20, we look in the Broadcom document:
   1. On page 95 states that a write to the ith bit of `GPSET0` will set
      the ith GPIO pin on.

   2. Using the table on page 90 we see `GPSET0` is located at address
      `0x7E20001C` (note: a constant prefixed with `0x` means it is
      written in hex notation.)

   3. Finally, just to confuse things, we know after staring at the diagram on 
      page 5 that the broadcom "CPU bus addresses" at 
      `0x7Exx xxxx` are mapped to physical addresses at `0x20xx xxxx` on the pi.  
      Thus the actual address we write is `0x2020001C`.

      Note: such ad hoc, "you just have to know" factoids are wildly
      common when dealing with hardware, which is why this is a lab class.
      Otherwise you can get stuck for weeks on some uninteresting fact
      you simply do not know.  Hopefully, after this class you operate
      robustly in the face of such nonsense.)

The result of all this investigation is the following sleazy C code:

        *(volatile unsigned *)0x2020001C = (1 << 20);

I.e., cast the locations `0x2020001C` to a `volatile unsigned` pointer
and write  (store) the constant produced by shifting a 1 to the 20th
(`1 << 20`) position there:

   - `volatile` tells the compiler this pointer is magic and don't optimize its use away.
   - `unsigned` on the pi is 32-bits.   

Morally the above is fine, despite what some people might tell you.
However, empirically, it is very easy to forget a `volatile` type
qualifier, which will cause the compiler to (sometimes!) silently remove
and/or reorder reads or writes.  In this class we will *never* directly
read or write device memory, instead we will call the procedures `get32`
and `put32` to read and write addresses.  For example:

        put32(0x2020202, (1 << 20));

`put32` will call out to assembly code (`gcc` currently cannot optimize
this) writes the given value of the second argument to the address
specified by the first.   

In addition to correctness, this method of using `put32` and `get32`
makes it trivial for us to write code that monitors, records, or intercepts
all read and writes to device memory.   This trivial bit of instructure
makes it easy for you to do a bunch of surprisingly powerful tricks:


  1. Instead of performing them on the local device memory we can
     send them over the network and control one or many remote r/pi's.

  2. We can record the reads and writes that are done to device memory
     and then use this for testing, or automatically construct a bare-bones
     program replaces our original and simply replays them.

     Lab 3 uses this trick to sort-of-prove that your code is correct.
     It works by running your code and checking that it does the same
     reads and writes in the same order with the same values as everyone
     else.  This makes it easy to show your code is equivalant to everyone
     else's code --- despite the fact that it will look very different.
     If one person is correct, all must be correct.

  3. Many many others!

Generally, whenever you need to control a device, you'll do something like
the following: 

  0. Get the datasheet for the device (i.e., the oft poorly-written PDF that describes it).
  1. Figure out which memory locations control which device actions.
  2. Figure out what magic values have to be written to cause the device to do something.
  3. Figure out when you read from them how to interpret the results.

Devices typically require some kind of initialization, a sequence of
writes to kick-start the device down a set of actions, and some number
of reads when we know data is (or could be) available.

In our case, we want to get the GPIO pin initialized to be an output
pin (to control and LED) or input (so that we can read the value being
produced by the device).  So:

   1. Figure out what location to write to set a pin to input or output.
   2. Implement `gpio_write` to write the value of an output pin (LED).
   3. Implement `gpio_read` to read the value of an input pin (touch sensor).

While a lot of this may sound complicated, and the Broadcom document
is not particularly friendly, you will see that what is actually going
on is pretty simple, there is just a bunch of jargon, a few loads or
stores to weird addresses, and with a few lines of code you can start
to control some pretty neat stuff.

Important correctness note:

  - While the device operations are initiated using memory loads and
    stores, what actually occurs "behind the scenes" is often far
    more complex and expensive than a memory operation.  (While they
    look like memory operations they are closer to arbitrarily complex
    procedure calls.)  Thus, they will take much longer than a normal
    memory operation.  Their cost leads to the following hard-to-debug
    problem (I had one that literally took two days a few years back.)

    On many machines, including the pi, when you perform a store
    to a device, the store can return before the device operation
    completes.  This hack can give a big speed improvement by, among
    other things, allowing you to pipeline operations to the same device.
    However, it also can lead to subtle ordering mistakes.   

    The pi does guarantee that all reads and writes to the same device
    occur in order (these operations are "sequentially consistent"
    w.r.t. each other).  However it *does not* guarantee that a write to
    one device A followed by a second write to device B will complete in
    that order.  If they are entirely independent, this may not matter.
    However, if the device operations were supposed to happen in that
    order (A then B), the code is broken.

    The way we handle this is to put in a "memory barrier" that
    (over-simplifying) guarantees that all previous loads and stores
    to memory or devices have completed before execution goes beyond
    the barrier.  You can use these to impose ordering.  We will discuss
    them at length later.  There are some very subtle issues, especially
    when dealing with virtual memory hardware.   We don't worry about this
    for the current lab.

--------------------------------------------------------------------------
### Part 1.  Make blink work. (30 minutes)

You'll implement the following routines in `code/gpio.c`:
   1. `gpio_set_output(pin)` which will set `pin` to an output pin.  This should 
       take only a few lines of code.
   2. `gpio_set_on(pin)` which will turn `pin` on.  This should take one line of code.
   3. `gpio_set_off(pin)` which will turn `pin` off.  This should take one line of code.
   4.  After doing so, wire up an LED pin to pin 20, power-cycle
       the pi, and use the bootloader to load the code:

             # unplug the pi.
             # connect an LED to pin 20
             % cd code
             # do your edits
             % make
             % pi-install 1-blink.bin
             # the LED on pin 20 should be blinking.

   5. Make sure that `code/2-blink.c` also works:

             # unplug the pi
             # connect an LED to pin 21
             # plug back in to reset.
             % pi-install 2-blink.bin
             # the LEDs on pin 20 and pin 21 should be in opposite orders.

      Success looks like:

<p float="left">
  <img src="images/part1-succ-green.jpg" width="450" />
  <img src="images/part1-succ-blue.jpg" width="450" />
</p>



Hints:
   1. You write `GPFSELn` register (pages 91 and 92) to set up a pin as an
      output or input. You'll have to set GPIO 20 in GPFSEL2 to output.

   2. You'll turn it off and on by writing to the `GPSET0` and `GPCLR0`
      registers on page 95.  We write to `GPSET0` to set a pin (turn it on)
      and write to `GPCLR0` to clear a pin (turn it off).

   3. The different `GPFSELn` registers handle group of 10, so you 
	  can divide the pin number to compute the right `GPFSEL` register.

   4. Be very careful to read the descriptions in the broadcom document to
      see when you are supposed to preserve old values or ignore them.
      If you don't ignore them when you should, you can write back
      indeterminate values, causing weird behavior.  If you overwrite old
      values when you should not, the code in this assignment may work,
      but later when you use other pins, your code will reset them.
 
               // assume: we want to set the bits 7,8 in <x> to v and
               // leave everything else undisturbed.
               x &=  ~(0b11 << 7);   // clear the bits 7, 8  in x
               x |=   (v << 7);   // or in the new bits
                          
   5. You will be using this code later!   Make sure you test the code by 
	  rewiring your pi to use pins in each group.

--------------------------------------------------------------------------
### Part 2.  Make input work.

Part 1 used GPIO for output, you'll extend your code to handle input and
use this to read input.  At this point you have the tools to control
a surprising number of digital devices you can buy on eBay, adafruit,
sparkfun, alibaba, etc.

What you will do below:

   1. Implement `gpio_set_input` --- it should just be a few lines of
      code, which will look very similar to `gpio_set_output`. 

      Make sure you do not overwrite a previous configuration in `fsel`
      for other pins!   You code will likely still work today, but later
      if you have multiple devices it will not.

   2. Implement `gpio_read` --- make sure you do not return bits that
      are spurious!  This can lead to garbage results.

   3. Connect the positive leg of an LED to one of the pi's 3v outputs: 
      DO NOT CONNECT TO 5V!  Test it by touching the other LED leg to ground: 
      it should turn on.
 
   4. Run the code:


            % make
            % pi-install 3-input.bin
            # touch the LED leg to pin 21: the second LED (connected to pin 20) goes on.
            # remove the LEG touching pin 21: LED (connected to pin 20) goes off.

   5. Success looks like the following (note: we used a bare jumper, but that is
      likely asking for trouble):

<p float="left">
  <img src="images/part2-succ-on.jpg" width="450" />
  <img src="images/part2-succ-off.jpg" width="450" />
</p>

--------------------------------------------------------------------------
### Part 3. Forward one pi signal to another.

We now do a cool trick: transparently forward signals from one pi to
another. While mechanically trivial, this is a "hello world" version of
some deep topics.

What to do:

   1. Hook up pin 20 from one pi (call this pi-1) to pin 21 of the other
      (pi-2).  


      Note: strictly speaking, when connecting two devices we must
      connect (share) ground as well.  However, since these are both
      powered from your laptop they already do.

   2. Plug pi-2 in and run `3-input.bin` input program.

        % pi-install /dev/ttyUSB0 code/3-input.bin

   2. Plug pi-1 in and run `1-blink.bin` program.

        % pi-install /dev/ttyUSB1 code/1-blink.bin


   3. Success looks like:
<p float="left">
  <img src="images/part3-succ-on.jpg" width="230" />
  <img src="images/part3-succ-off.jpg" width="230" />
  <img src="images/part3-succ-close.jpg" width="230" />
</p>

This doesn't seem like much, but is shows the glimmer of several deeper 
tricks:

   1. It shows how you can take code written with very strong assumptions
      and --- without any software changes --- dramatically change
      its behavior, correctly.  In this case we take a system that
      assumed it running locally, controlling its own device and made
      it transparently control a remote device (or even many).

      Among other examples this trick (on a much grander scale) is similar
      in spirit to how virtual machines such as VMWare work.

   2. You can view this single wire as a very primitive network or bus.
      On = sending a 1 bit.  Off = sending a 0.  And, if we can send 0
      and 1 we can send anything.

      With some simple changes (we will do these in later labs) you
      can use this method to send general-pupose messages between pi's.
      And if you add a second wire, you easily can do so with incredibly
      low-latency: almost certainly lower than between two full-fledged
      Unix servers.  (One of many examples where we will be able to write
      custom, clean, simple code that is far faster or more powerful
      than a full-fledged "real" system.)

--------------------------------------------------------------------------
### Extension: Break and tweak stuff.

If you finish, the are a bunch of ways you can "kick the tires" on your
system to understand better what is going on.


   1. Change the delay in the blink code to increasingly smaller amounts.
      What is going on?


   2. Add the reboot code below (we'll go into what different things mean)
      so that you don't have to unplug, plug your rpi each time:

           // define: dummy to immediately return and PUT32 as above.
           void reboot(void) {
                const int PM_RSTC = 0x2010001c;
                const int PM_WDOG = 0x20100024;
                const int PM_PASSWORD = 0x5a000000;
                const int PM_RSTC_WRCFG_FULL_RESET = 0x00000020;
                int i;
                for(i = 0; i < 100000; i++)
                     nop();
                PUT32(PM_WDOG, PM_PASSWORD | 1);
                PUT32(PM_RSTC, PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET);
                while(1);
           }

      Change your code to just loop for a small fixed number of times and make
      sure reboot() works.  

   3. Force the blink loop to be at different code alignments mod 64.
      If you look at `1-blink.list` you can see the executable code and
      the addresses it is at.   Do you notice any difference in timing?
      (You may have to make your delay longer.)  What is going on?

--------------------------------------------------------------------------
#### Additional information

More links:

  1. Useful baremetal information: (http://www.raspberrypi.org/forums/viewtopic.php?t=16851)

  2. More baremetalpi: (https://github.com/brianwiddas/pi-baremetal)

  3. And even more bare metal pi: (http://www.valvers.com/embedded-linux/raspberry-pi/step01-bare-metal-programming-in-cpt1)

  4. Finally: it's worth running through all of dwelch's examples:
  (https://github.com/dwelch67/raspberrypi).
