## User-process: Using a process structure and virtual memory

I had hoped we would do a more full-featured OS process lab, but given:

  1. People are still working on the past VM labs;
  2. We should start thinking seriously about projects;
  3. Ukraine.

Today's lab is a bit smaller and targeted: 
  1. We'll build and check the
     pieces needed to context switch using a real process structure (what
     you need for pre-emptive threading/user-processes) and add virtual
     memory.  The entire thing will be checked using single stepping.

  2. You should reach a tentative agreement for what your final project is,
     and spec out the hardware you need.  Show us this as part of checkoff.

We will still need to add some steps to get to a simple UNIX style
system, but these are (1) needed pieces also (2) don't really need you
to understand much additional starter code, so it's a nice breather
(hopefully).

For the first couple of parts, we'll start by adding to a copy of your
lab 11 (the user process lab) to keep new stuff to a minimum.  Then we'll
flip that code into a more full fledged code base.


***Checkoff***:
  1. You have a process structure and virtual memory and pass the checks.
  2. You have a tentative final project proposal with some parts.

--------------------------------------------------------------------
### Part 0: update your code and make sure it still works.


#### Make sure your `11-user-process/code` still works.

Before we start making changes, make sure your code still works:

   0. Make sure the `Makefile` in
      `11-user-process/code/trivial-user-level-prebuilt` runs all the
      test (not many, I know: add more as an extension!):

            TESTS := $(wildcard ./[0-3]*-test*.bin)

   1. Rebuild from scratch and check that the tests pass

            % cd 11-user-process/code
            % make clean
            % make
            % cd 11-user-process/code/trivial-user-level-prebuilt
            % make check


Great. We now have a defined stable state to work from.

#### Update your code and sure compilation works

The lab is going to be a set of small pieces.  As a first
step we just update what gets included:

   1. Add the line:

            #include "pix-internal.h" 

     the the end of `trivial-os.h`.

     The header `pix-internal.h` has the following trivial process
     structure:

            // add this to your trivial-os.h
            typedef struct {
                // register save area: keep it at offset 0 for easy
                // assembly calculations.
                uint32_t reg_save[16 + 1];  // 16 general purpose registers 
                                            // + spsr

                ... more stuff ...
        
                // used by the equiv code.
                pix_equiv_t ctx;
            } pix_env_t;

   2. If you look in the new file `pix-env.c` you'll see the trivial 
      definition of `pix_cur_process`:
    
            static pix_env_t init_process;
            pix_env_t *pix_cur_process = &init_process;

   3.  Compile and run the checks to make sure you get the same checksums.

--------------------------------------------------------------------
### Part 1: save state to a process structure

For our OS we'll want to save the current registers into a process
structure rather than onto a random exception stack.  This makes it easier
to switch to another process.  To do so, make the following changes:

  0. Make copy of the code and routines you used for Part 4 of
     lab 11: your exception vector, a copy of the equivalence routine that
     it calls, and add another `case` arm in `trivial-os.c` and update
     the value in `part` to call this.  Make sure `make check` passes.

   1. Modify the copy of the assembly trampoline from (0)
      and have it load the location to save registers to using the
      `pix_cur_process` variable (above).  You might want to look at the
      old interrupt code (in `7-interrupts/timer-int/interrupts-asm.S`)
      to see how to do this easily in assembly.

      Note: people make a ton of mistakes doing this, so write some simple
      C code that does what you want, and inspect the machine code to see
      how to load a global variable.

      As a sanity check: before rewriting the assembly code, first pass in
      the value you loaded above into the single step exception handler
      as an argument and `assert` that it is equal to the value held in
      your `pix_cur_process`.

      Note: you will need to do two `ldr` instructions: one to get the
      value of the global variable `pix_cur_process` and then another to
      load its contents.  You should be able to do an `=pix_cur_process`
      in the assembly for the first one.

      Note: An easy mistake to make is to forget that `r0`, `r1` and
      the other caller saved registers will get trashed by the exception
      handler --- so don't assume their original values are still there!

   2. Make a copy of your equivalence code that stores the current context
      in this process structure and uses it to track the current hash.
      Have the assembly routine from (1) pass a pointer to the current
      context structure in.

   3. Rerun your code and make sure it still gives the same checksums.
      (You'll notice a pattern: this is how I always make changes --
      tiny change, then verify checksum equivalence --- so that I don't
      have to think that hard.)

At this point the registers should be saved and restored out of
the current structure.

--------------------------------------------------------------------
### Part 2: build and use `switchto_asm`

In general we want to be able to switch to an arbitrary process.  For this
part you'll write a `switchto_asm` routine that takes a pointer to the
`reg_save` array of the next process you want to run and loads everything
and jumps to the code:

        // switches into process <p>: does not return.
        switchto_asm(&p->reg_save[0]);

NOTE: It should use `rfe` instead of changing `spsr` first.

For this part:

   1. Implement `switchto_asm`: it should load registers from
      the same offsets they were saved to in the previous step.

   2. Change `equiv_run_fn_proc` to call `switchto_asm` at the
      end of the equivalence routine instead of returning.

   3. Rerun your code to make sure it still gives the same checksums.

--------------------------------------------------------------------
### Part 3: Stop using `user_mode_run_fn`.

Given `switchto_asm` we don't need the `user_mode_run_fn`: all we need is
to setup the process structure correctly and then `switchto_asm`
will work both for the first switch as well as all the subsequent ones.

   0. Create a new `case` arm based on the previous one where you do
      the following:

   1. Modify `pix-env.c:pix_env_mk` to initialize the structure save
      area so the registers in it have the same values as would be set
      by your `user_mode_run_fn`.

      I.e., the offset the `pc` is stored at should be the address of the
      code we want to jump to, the stack pointer `sp` offset should be
      set to the initial stack value and the `SPSR` offset to the `spsr`
      value we want (you can see what this is from your past runs).
      Everything else should be zero.

      Then you should be able to call `switchto_asm`:

            switchto_asm(&p->reg_save[0]);

      And have it work equivalently to:

            user_mode_run_fn((void*)p->reg_save[15], p->reg_save[13]);

   2. Change the code to call `switchto_asm` instead of 
      `user_mode_run_fn`.

   3. Rerun your code to make sure it still gives the same checksums!

Great: now you have code that will correctly save and restore registers
from a process structure (which we need as a first step for an OS).

--------------------------------------------------------------------
### Part 4: add virtual memory using an identity map

For this part, you should modify the code to add and enable virtual memory
(based on the past lab):

   1. Add a new `case` arm, make copies of anything you need rather than
      break working code: you should always be able to run the past test
      for (parts 1-3).

   2. Modify the Makefile to get the code from your previous lab.  You'll
      add `CFLAGS = -I../../labs/12-vm/code/`  so it finds the right
      headers.

   3. After your modifications make sure `make check` works when virtual
      memory is disabled.

   4. Now run with virtual memory enabled.  Your checksums should pass.

--------------------------------------------------------------------
### Part 5: add virtual memory not using an identity map

For this part, run the code not using the identity map.
Do so by implementing the routine:

        // clone the page table <pt_old>: copy any global mappings over,
        // duplicate (allocate + copy) any non-global pages.
        void vm_pt_clone(pix_pt_t *pt_new,  const pix_pt_t *pt_old);


To clone a page table, and then just use this to clone the page table
you already made.   This probably seems a bit odd, but it's a useful
building block for `fork`, next lab.

   1. Make sure you mark only the memory needed for the process as non-global.
   2. Everything else should be global.

To test: make another copy of everything, and change the virtual
memory code to allocate sections as it needs.  Checks should pass!

--------------------------------------------------------------------
### Part 6: tune your code to use registers

Today we violate the cardinal tenet of optimization: never optimize if
you can't measure a speedup.  (Given how slow our code is, it's hard to
see any difference.)  We do the optimization as a "remix" strategy so
you get a firmer grasp by doing a useful task multiple ways.

The cool thing about single-stepping is that it will check these
modifications --- it somewhat checks them today, and when we add multiple
processes it will really check them (to the extent I personally will be
surprised if the code is wrong).

#### Use `rfe` and (maybe) `srs`

Change your code to use `rfe` if it does not already. You can look at
`../11-user-process/prelab-code/` for example code that uses `srs` and
`rfe` to do exception handling code.

#### Global registers

The arm1176 provides (at least) three coprocessor registers for "process
and thread id's."  However, since the values are not interpreted by the
hardware, they can be used to store arbitrary values.

OSes often stick various pointers in global variables.  Instead they can
put such pointers in these coprocessor registers.  If you are counting
cycles, loading a value from a global adds overhead (e.g., easily one
a cache miss, maybe more).

As one example, in pix you used a global variable to track the
current environment.  Instead you can store this in the co-processor.
Or for eraser or your memory tracer you can leave a pointer to your
global table.  Similarly, you can put large constants in it rather
than having to do a load from memory.  One nice benefit is that a
memory corruption bug will not corrupt these registers.

If you look at  `3-129` in the `arm1176.pdf` manual you can see their 
definition. You should make `get` and `set` methods like usual, and 
replace our code that uses the global variable of the current process.

--------------------------------------------------------------------
### Extensions: Big, useful steps:

We should have done this today, but time is tight:
   1. You can change the code to rerun the program over and over:
      the checksum it gets should not change.

   2. Change the code so it creates two processes and then always switches 
      between them on each equivalent call.  

      You should implement a `schedule` call that will do this.  It will
      be called both in the equivalence code and during `exit`.

--------------------------------------------------------------------
#### Extensions

Some simple changes:
  0. change the linker to load multiple programs at once.
  1. Change the page size.
  2. Use two page tables (one for system, one for user-level)
  3. Compile `pix` with higher level optimizations.
  4. Turn on caching; switch from write-through to write-back.
  5. Change the expensive virtual memory coherence methods from lab 12
     to be more targeted (and much more efficient).  In particular, don't
     kill the entire TLB when modifying a PTE entry (or defer the coherence
     until the end).
  6. If you turn on interrupts, and change the frequency, there should be
     no user-visible difference.
  7. Load the binaries from the SD card using your FAT32 file system.
  8. Send the binaries over the NRF network.

There's tons of additional ones!   The cool thing is your checksums should
never change.
