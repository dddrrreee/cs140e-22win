## Debug hardware cheat sheet.

Like most modern machines, the `arm1176jzf` CPU in our r/pi's supports:
  - *watchpoints*: which cause an exception to occur when a load or
    store is performed to a small number of addresses.
  - *breakpoints*: which cause an exception to occur when the CPU attempts to
    execute an instruction at a small number of addresses.
  - *single-stepping*: where an exception occurs for each insturciton executed
    in order.

Most OS designers skip through the debug chapter of any hardware
document ("boring!")  or read it just enough to re-target a `gdb` stub.
I certainly used to!  But this is a mistake.  There's a lot of useful
tricks we can play with this hardware.

Unfortunately, also like many hardware documentation, the ARM1176
document is written in definitional voice, rather than with examples
and "cookbook" algorithms to do specific tasks.  So you have to read
chapter 13 with a goal in mind, gather up the different rules scattered
throughout, and hope you don't miss "must" or "do A before B" edict
(there are several of these!).   We'll give some of the page numbers
below to hopefully make this a bit easier.  Of course: if you want the
full experience just ignore these hints and go into the chapter raw.
This is certainly the way to learn!

### Useful page index

Tables:
  - p 13-5: Table 13-1: terms for registers.  (R, W, etc)
  - p 13-5: Table 13-2: the `Opcode_2` and `CRm` values needed to specify
    a debug register (e.g., `WFAR`, `DIDR`, etc). 

Pages:
  - p 13-3 (13.2.2): monitor debug-mode: this is what we use.
  - p 13-4 (13.2.4): states it uses modified virtual addresses, but AFAIK we can
    use physical addresses.  
  - 13-6: debug id (`DIDR`) register layout. Useful: gives number of
    watchpoints (2) and breakpoints (6) supported by our CPU.
  - 13-7: debug status and control (`DSCR`): in particular, enable
    the debug mode 

    Subpoints:
      - 13-9: "for the core to take an exception, monitor debug mode
        has to be both selected and enables, bit 14 clear and bit 15 set").
      - 13-11: how to tell which exception occurred (bits `5:2`):
        watchpoint or breakpoint.

        RULE: "must first check IFSR or DFSR to determine a debug
        exception has occured before checking `DSCR`).








This is a quick cheat sheet 

## Breakpoints

Do the readings in the [PRELAB.md](./PRELAB.md) first!

The ARM chip we use, like many machines, has a way to set both
*breakpoints*, which cause a fault when the progam counter is set to
a specific address, and *watchpoints* which cause a fault when a load
or store is performed for a specific address.  These are usually used
by debuggers to run a debugged program at full-speed until a specific
location is read, written or executed.

This lab will use watchpoints as a way to detect memory corruption
efficiently.  As you know too-well by now, challenge of bare-metal
programming is that we have not had protection against memory corruption.
By this point in the quarter, I believe everyone in the class has had
to waste a bunch of time figuring out what was causing memory corruption
of some location in their program.  After today's lab you should be able
to detect such corruption quickly:

   1. Simply set a watchpoint on the address of the memory getting corrupted,
   2. When a load or store to this address occurs, you'll immediately
      get an exception, at which point you can print out the program
      counter value causing the problem (or if you're fancy, a backtrace)
      along with any other information you would find useful.

Yes, we can do memory protection with virtual memory, but that also
requires a lot of machinery and can easily make real-time guarantees
tricky.  ARM virtual memory also only provides page-level protection,
whereas the watchpoints can track a single word (or even byte).

The lab today should be around 100-200 lines of code, doesn't need
virtual memory, and if you continue to do OS or embedded stuff, will be
very useful in the future.

### Background

The arm1176jzf chip we are using has multiple "debug" modes.  We are using
"monitor debug-mode" which just means that you can configure the hardware
to throw exceptions when the contents of a small set of addresses are
read, written or executed.  The manual confusingly refers to the addresses as 
*modified virtual addresses* (MVAs), but as far as I can tell, you can also 
use physical addresses.   (I can't find any sentence that gives us any guarantee
of this, so if you see one let me know!).  


As we've seen repeatedly: 
  - The ARM will control functionality through different co-processors.  For
    debuging functionality, that is co-processor 14.
  - There will typically be a configuration register that controls whether
    the functionality is enabled or disabled.  For us, this is the
    "debug status and control register" (`DSCR`) on 13-7.

The list of the hardware debug registers is on page 13-5 (page 480 in my pdf).

A cheat-sheet of assembly instructions to access these registers (13-26):
<table><tr><td>
<img src="images/cp14-asm.png"/>
</td></tr></table>

When a debug fault happens, the 
hardware will put additional values in different fault registers.
A short cheat sheet of the assembly code to get various of the fault registers:
<table><tr><td>
<img src="images/cheat-sheet-fault-regs.png"/>
</td></tr></table>

-----------------------------------------------------------------------------
### Part 0:  Get the debug ID register (DIDR)

As a warmup, implement a routine to get the debug id register (page 13-6)
and use this to determine how many watchpoint and breakpoints our specific
ARM processor provides.

As stated on 13-5, the CP14 debug registers have:
  - `Opcode_1` is `0`.
  - `CRn` is `0`.

From the table abov:
  - `Opcode_2` is `0`.
  - `CRm` is `c0`.

So we need to use the instruction:

    mrc p14, 0, <Rd>, c0, c0, 0

You should implement the routine:
    - `cp14-debug.h:didr_get()`
    - `bit-support.h` provides some functions for bit manipulation.
    - There are some macros that may or may not help you at the top of `cp14-debug.h`.
    - When you run your code, `tests/test1.c` should pass: make sure you can figure
      out how many watchpoints and how many breakpoints we have.

-----------------------------------------------------------------------------
### Part 1:  Catch loads and stores of `NULL`.

So far we've been vulnerable to load and stores of NULL (address 0).
If you run this code it will execute "successfully"!

    #include "rpi.h"
    void notmain(void) {
        *(volatile unsigned *)0 = 0x12345678;
        printk("NULL = %x\n", *(volatile unsigned *)0);
    }

As mentioned above and in the readings, the ARM chip we're using provides
*watchpoints* to trap when an address is used in a load or store and
*breakpoints* for when you try to execute the contents of an address.
The exception you receive for each is different.

For both, there will be one register that you put the address to watch in,
and a second, paired register to control what happens when the address
is used.

To set a watchpoint you can follow the recipe on 13-47.
  1. Enable monitor debugging using the `DSCR` (13-7): bits 14 and 15.
  2. Set the "watchpoint value register" (WVR) on 13-20 to 0.
  3. Set the "watchpoint control register" (WCR) on 13-21.
  4. After finishing your modifications of cp14, make sure you do a
     `prefetchflush` (see below) to make sure the processor refetches
     and re-decodes the instructions in the instuction prefetch buffer.
  5. Implement the code in the `data_abort_int` handler to check if the
     exception is from a debug exception and, if so crash with an error.

For the WCR: We don't want linking (which refers to the use of context id's).
We do want:
   - Watchpoints both in secure and non-secure;
   - For both loads and stores.
   - Both priviledged and user.
   - Enabled.
   - Byte address select for all accesses (0x0, 0x1, 0x2, 0x3).

When you are done, `tests/test1.c` should pass.

After any modification to a co-processor 14 register, you have to do a 
`PrefetchFLush`:
<table><tr><td>
<img src="images/prefetch-flush.png"/>
</td></tr></table>

How to get the data fault status register (DFSR, page 3-64): 
<table><tr><td>
<img src="images/dfsr-get.png"/>
</td></tr></table>

You can use the DFSR to get the cause of the fault from bits `0:3` if `bit[10]=0`:
<table><tr><td>
<img src="images/data-fault-field.png"/>
</td></tr></table>


How to get the fault address register (FAR): 
<table><tr><td>
<img src="images/far-get.png"/>
</td></tr></table>

-----------------------------------------------------------------------------
### Part 2:  Catch jumps to `NULL`.

Now catch if we jump to `NULL` --- this will require setting a breakpoint
instead of a watchpoint and handling the exception in the `prefetch_int`
handler.

As above, differentiate that the exception was caused by a debug exception.

How to get the instruction fault status register (IFSR): 
<table><tr><td>
<img src="images/ifsr-get.png"/>
</td></tr></table>

-----------------------------------------------------------------------------
### Part 3: Single-stepping.

In many tools, we'd want to run a single instruction and then get control
back for a tool.

In the simplistic memory checker we built, we had the problem that it was
tricky to figure out where an instuction would wind up if if we executed a
single instruction.  Even an `add` instruction can jump somewhere else,
if it modifies the program counter `r15`.  We could handle this (the CPU
certainly does), but there are enough ARM instructions that it becomes
more and more tricky.

For this part, just set up for mismatch, set up for trampoline.

-----------------------------------------------------------------------------
### Extension: handle multiple watch and breakpoints.

In general, as a safety measure, we should probably always enable
watchpoint and breakpoints on `NULL`.   However, we'd also like to be
able to catch breakpoints on other addresses.

Extend your code to add support for a second simultaneous watchpoint and
breakpoint to a different address.  In the handler differentiate it if
it is a null pointer or a from the second value.

For this:
  1. Set a breakpoint on `foo` and see that you catch it.  
  2. Set a watchpoint on a value and see that you catch it.

-----------------------------------------------------------------------------
### Extension: a more general breakpoint setup.

We hard-coded the breakpoint numbers and watchpoints to keep things simple.
You'd probably want a less architecture-specific method.  One approach
is to allocate breakpoints/watchpoints until there are no more available.

    // returns 1 if there were free breakpoints and it could set.
    // 0 otherwise. 
    int bkpt_set(uint32_t addr);
    // delete <addr>: error if wasn't already set.
    void bkpt_delete(uint32_t addr);

    // same: for watchpoints.
    int watchpt_set(uint32_t addr);
    int watchpt_delete(uint32_t addr);


Note: you may want to design your own interface. The above is likely
not the best one possible.

-----------------------------------------------------------------------------
### Extension:  Failure-oblivious computing.

Possibly the most-unsound paper in all of systems research is Rinard
et al's "Failure-oblivious computing", which made buggy server programs
"work" by handling memory corruption as follows:
   1. Discard out of bound writes.
   2. Return a random value for out of bound reads (starting at 0, 1, 2, ...).

We can only handle a single address, but we can do a similar thing.   Change
your exception code to take the complete set of registers, and restore from 
this set (so that you can change it).   
