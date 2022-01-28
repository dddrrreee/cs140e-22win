## Writing a non-preemptive threads package

***MAKE SURE YOU DO THE [PRELAB](PRELAB.md) FIRST!***

Big picture:  by the end of this lab you will have a very simple
round-robin threading package for "cooperative" (i.e., non-preemptive)
threads.

The main operations (see `rpi-thread.[ch]`):
  1. `rpi_fork(code, arg)`: to create a new thread, put it on the `runq`.
  2. `rpi_yield()`: yield control to another thread.
  3. `rpi_exit(int)`: kills the current thread.
  4. `rpi_thread_start()`: starts the threading system for the first time.
      Returns when there are threads left to run.

Recall we split threads into two types:
  - Non-preemptive threads run until they explicitly yield the processor.

  - Pre-emptive threads can be interrupted at any time (e.g., if their
    time slice expires, or a higher-priority thread becomes
    runnable, etc).

The trade-offs:
  - Cooperative threads make preserving large invariants easy: by
    default, all code is a critical section, only broken up when you
    yield [Atul et al].  Their downside is that if they don't yield
    "often enough" they add large, difficult-to-debug latencies.

  - Pre-emptive threads let you to eliminate the need to trust
    that the threading code yields control in a well-behaved way, which
    is why multi-user OSes (Linux and MacOS) preempt user processes.
    However, they typically make it much more difficult to write correct
    code.  (E.g., would your gpio implementation work if it was called
    by multiple pre-emptive threads?  UART?)

Generally, at a minimum, each thread has its own stack --- which has
to be "large enough" so that it doesn't get overrun --- and the thread
package has to provide an assembly routine to "context switch" from one
thread to another.  Context switching works by:

   1. Saving all registers from the first thread onto its stack and storing
      this new stack pointer in the thread control block.
   2. Loading all registers for the second from where they were saved.
   3. Changing the program counter value to resume the new thread.

The main magic you'll do today is writing this context-switching code.
Context-switching doesn't require much work --- about five instructions
using special ARM instructions, but mistakes can be extremely hard
to find.  So we'll break it down into several smaller pieces.

### Sign-off:

   - You pass the tests in the 2-threads` directory.

----------------------------------------------------------------------
### Hints

While all this is exciting, a major sad is that a single bug can lead
to your code jumping off into hyperspace.  This is hard to debug.
So before you write a bunch of code:
  1. Try to make it into small, simple, testable pieces.
  2. Print all sorts of stuff so you can sanity check!  (e.g., the value
  of the stack pointer, the value of the register you just loaded).
  Don't be afraid to call C code from assembly to do so.

The single biggest obstacle I've seen people make when writing assembly is
that when they get stuck, they wind up staring passively at instructions
for a long time, trying to discern what it does or why it is behaving
"weird."

  - Instead of treating the code passively, be aggressive: your computer
    cannot fight back, and if you ask questions in the form of code
    it must answer.  When you get unsure of something, write small
    assembly routines to clarify.  For example, can't figure out why
    `LDMIA` isn't working?  Write a routine that uses it to store a
    single register.  Can't figure out if the stack grows up or down?
    take addresses of local variables in the callstack print them (or
    look in the `.list` files).

   - Being active will speed up things, and give you a way to
    ferret out the answer to corner case questions you have.

----------------------------------------------------------------------
### Background: A crash course in ARM registers

You should have read the ARM assembly chapter in the PRELAB
(`docs/subroutines.hohl-arm-asm.pdf`); this is just a cliff-notes version.

Context-switching may sound mysterious, but mechanically it is simple: we
just need to need to save all the registers in use.  The ARM has sixteen
general-purpose registers `r0`---`r15` which can be saved and restored
using standard load and store instructions.  If you look through the
`.list` files in any of the pi program directories, you'll see lots of
examples of saving and loading registers.

Even easier, because we are doing non-preemptive threads we don't have to
save all the registers, just the "callee saved" ones.  The ARM, like most
modern architectures, splits registers in into two categories (Q: why?):

  - "Callee-saved": must be saved by a procedure before any use and
    then restored before returning to the calling routine.

  - "Caller-saved": can be used as scratch registers by any procedure
    without saving/restoring them first.  The downside is that if a
    routine needs the contents of a caller-saved register preserved
    across a function call it must save the register before the call
    and restore it afterwards.

As we mentioned, a non-preemptive thread only context switches if it
explicitly calls a thread yield procedure of some kind (in our case,
`rpi_yield()`).  As a result we only need to save the "callee-saved"
registers, since any live caller-saved registers must be already saved
by the compiler.


Recall our hack from `8-device-interrupts/prelab/caller-callee.c`.
You can always modify/use that kind of code to double-check your
understanding.

Recall:
   - `r4 --- r11`: callee-saved.

   - `r13` : stack pointer (`sp`).  Today it doesn't matter, but in the
      general case where you save registers onto the sack, you will
      be using the stack pointer `sp`.  Be careful when you save this
      register.  (How to tell if the stack grows up or down?)

   - `r14` : link register (`lr`), holds the address of the instruction
     after the call instruction that called the current routine (in our
     case `rpi_yield`).  Since any subsequent call will overwrite this
     register we need to save it during context switching.

   - `r15`: program counter (`pc`).  Writing a value `val` to `pc` causes
     in control to immediately jump to `val`.  (Irrespective of whether
     `addr` points to valid code or not.)  From above: moving `lr` to `pc`
     will return from a call.  Oddly, we do not have to save this value!

So, to summarize, context-switching must save:

  - `r4 --- r12, r13, r14`.   You can use `push` and `pop` to do most of 
    this.

For reference, the [ARM procedure call ABI](http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042f/IHI0042F_aapcs.pdf) document:
<table><tr><td>
  <img src="images/arm-registers2.png"/>
</td></tr></table>


The link
[here](https://azeria-labs.com/arm-data-types-and-registers-part-2/)
has a more informal rundown of the above.

You can find a bunch of of information in the ARM Manual:
<table><tr><td>
  <img src="images/arm-registers.png"/>
</td></tr></table>

----------------------------------------------------------------------
### Part 1: getting run-to-completion threads working (20 minutes)

I've tried to break down the lab into isolated pieces so you can test
each one.  The first is to get a general feel for the `rpi_thread`
interfaces by getting "run-to-completion" (RTC) threads working: these
execute to completion without blocking.

RTC threads are an important special case that cover a surprising number
of cases.  Since they don't block, you can run them as deferred function
calls.   They should be significantly faster than normal threads since:
  1. you don't need to context switch into them.
  2. you don't need to context switch out of them.
  3. you won't have to fight bugs from assembly coding.
Most interrupt handlers will fall into this category.

What to do:
  1. `rpi_fork` create a thread block, set the `fn` and `arg` field
     to the values passed into `rpi_fork` and put the thread on the 
     run queue.
  2. `rpi_thread_start` remove threads in FIFO order, and call
      each function, free the thread block.

What you do not have to do:
  1. Write any assembly.
  2. Do anything with the thread stack.

----------------------------------------------------------------------
### Part 2: assembly and saving state (30 minutes)

Now we'll do some assembly language to make some building blocks:

  1. Does `push` write to the stack before or after changing the stack
     pointer?    One reason we need this is to know where exactly
     we have to set the initial stack pointer to point to.  Another
     is whether the stack grows up or down.

     Look in `2-where-push.c` to see what we need to check this, and 
     then implement `check_push_asm` in `2-where-push-asm.S`.

  2. How to write out the registers we need to save?  For context
     switching we need to save (1) the callee-saved registers, (2)
     the return register, and finally, (3) write the stack pointer to
     a storage location (`saved_sp` in our thread block).

     Look in `2-write-regs.c` to see what we need to check this, and
     then implement `write_regs_to_stack` in `2-write-regs-to-stack.S`.
     When you run the test, each printed register other than the stack
     pointer should match its expected value (the location for `r4`
     should hold `4` etc).

     This pattern of doing something in assembly, then calling C code to 
     print the results and then exit (because our execution state is messed
     up) is a useful one to use throughout the lab.

Note that the bulk register save and restore instructions store the smallest
registers so that the smallest is at the lowest address; also, don't 
include the stack pointer in the register list!   Useful [ARM
   doc](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0473m/dom1359731152499.html)
   or [this](https://www.heyrick.co.uk/armwiki/STM).

Don't be afraid to go through the ARM manual (`docs/armv6.pdf`) or the
lectures we posted in `6-threads/docs`.

Our next step is to start writing assembly code and build up the 
context-switching code in pieces.  

----------------------------------------------------------------------
### Part 3: context-switching.

There is no part 3.

----------------------------------------------------------------------
### Part 4: building `rpi_fork` and `rpi_start_thread`

Given that you have context-switching, the main tricky thing is figuring
out how to setup a newly created thread so that when you run context
switching on it, the right thing will happen (i.e., it will invoke to
`code(arg)`).

   - The standard way to do this is by manually storing values
     onto the thread's stack (sometimes called "brain-surgery") so that
     when its state is loaded via `rpi_cswitch` control will jump to
     a trampoline routine (written in assembly) with `code` with `arg`
     in known registers The trampoline will then branch-and-link to the
     `code` with `arg` in `r0`.

   - The use of a trampoline lets us handle the problem of missing
     `rpi_exit` calls.  The problem: If the thread code does not call
     `rpi_exit` explicitly but instead returns, the value in the `lr`
     register that it jumps to will be nonsense.  Our hack: have our
     trampoline that calls the thread code simply call `rpi_exit` if the
     intial call to `code` returns.

  - `rpi_start_thread` will context switch into the first thread it
     removes from the run queue.  Doing so will require creating a
     dummy thread so that when there are no more runnable threads,
     `rpi_cswitch` will transfer control back and we can return back
     to the main program.  (If you run into problems, try first just
     rebooting when there are no runnable threads.)

  - `rpi_cswitch`: this will be based on your code from `2-write-regs-asm.S`
    except that you will add the code to restore the registers as well.

What to do `rpi_fork` :
  0. Move your `cswitch` code into `rpi_cswitch` in `thread-asm.S`

     Make sure you verify that your `rpi_cswitch` code works when you
     pass the current thread as both arguments --- behaviorally this
     is a no-op since your code should save and restore the state,
     and resume right after the call.

  1. `rpi_fork` should write the address of trampoline
     `rpi_init_trampoline` to the `lr` offset in the thread
      stack (make sure you understand why!)
      and the `code` and `arg` to some other register offsets
      (e.g., `r4` and `r5`).

  2. Implement `rpi_init_trampoline` in `thread-asm.S` so that
     it moves the register holding `arg` to `r0` and does a branch and
     link to the register that holds `code`.

  3. To handle missing `rpi_exit`: do a call to `rpi_exit` at the end
     of the trampoline.

  4. To help debug problems: you can initially have the
     trampoline code you write (`rpi_init_trampoline`) initially just
     call out to C code to print out its values so you can sanity check
     that they make sense.

What to do for `rpi_start_thread`:
  1. If the runqueue is empty, return.
  2. Otherwise: allocate a thread block, set the `scheduler_thread`
     pointer to it and contxt switch into the first runable thread.

Note, you *do not* have to do with the scheduler thread:
 - Setup a stack.
 - The scheduler thread is never on the runqueue.
 - You contxt-switch out of the scheduler thread once and into it once
   when runque is empty.

There is a trivial program to test a single fork in
  - `4-test-one-fork.c`: it forks a single thread, runs it, and exits.
    This makes it easier to debug if something is going on in your context
    switching.

----------------------------------------------------------------------
### Part 5: adding `rpi_exit` and `rpi_yield`

You'll now finish out the main routines:

  - `rpi_exit` if it can dequeue a runnable thread, context switch
     into it.  Otherwise resume the initial start thread created in
     step 1.

  - Change `rpi_yield` so that it works as expected.

There are a two tests:
  - `5-test-exit.c`: forks `N` threads that all explicitly call `rpi_exit`.
  - `5-test-yield.c`: forks `N` threads that all explicitly call `rpi_yield`
     and then call `rpi_exit`.

----------------------------------------------------------------------
### Part 6: Handle missing `rpi_exit` calls.

Now we just make sure threads work when threads do not have an explicit
`rpi_exit`.  If your trampoline in part 4 works as expected, this should
"just work".

   - `6-test-implicit-exit.c`: should run and print `SUCCESS`.

----------------------------------------------------------------------
### Part 7: Running the larger programs.

Checking:
   - `7-test-thread.c` should work and print `SUCCESS`.
   - `7-test-ping-pong.c` should work and print `SUCCESS`.

If you want to get fancy, you should be able to run two LEDs in 
   - `7-test-yield.c`

Congratulations!  Now you have a simple, but working thread implementation
and understand the most tricky part of the code (context-switching)
at a level most CS people do not.

-------------------------------------------------------------------------
### Lab extensions

There's a lot more you can do.  We will be doing a bunch of this later
in the class:

  - Save state onto the stack itself.

  - Make versions of the `libpi` `delay` routines that use your function rather than
  busy wait.  Make sure to check if threads are enabled in `rpi_yield()`!

  - Have a `sleep_until_us(us)` to put the current thread to sleep with a 
  somewhat hard-deadline that it is woken up in `usec` micro-seconds.  Keep
  track of your cumulative error to see how well you're scheduling is doing.

  - Tune your threads package to be fast.  In particular, you can avoid
  context switching when a thread is run for the first time (there is
  no context to load) and you don't thave to save state when it exits
  (it's done).  You can also try tuning the interrupt handling code,
  since it is at least 2x slower than optimal.  (I'm embarrassed, but
  time = short.  Next year!)

   - See how many PWM threads you can run in `4-yield-test`.
