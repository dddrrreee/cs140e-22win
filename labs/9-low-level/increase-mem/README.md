## Increasing your pi memory size.

The box our r/pi came in claims 512MB of physical memory (as does the
r/pi foundation website).  *However*, depending on how parameters are
set, a good chunk of this memory can be claimed by the GPU and thus not
available for our code running on the CPU.

Both methods above gave me 128mb.  Two different calculations, same
astonishingly tiny result --- starting to think it was computed right.
This is bad, since we some of our interrupt stack addresses are
above 128MB!   It's unclear what the consequence of using memory the
GPU thinks is for it, but a likely result is that the GPU can randomly
corrupt our interrupt stack.  (Since we weren't using graphics, perhaps
we were getting away with this, but it's wildly bad form.)


##### Increasing memory by replacing the firmware


Ok, our memory size sucks: So how to change it?   I spent a surprisingly
long time fighting with this problem going in circles on forum/blog posts:

  - Generally people claim if you modify `gpu_mem` in `config.txt` (the file on
    your SD card) this will change how much memory the GPU uses.  Unfortunately,
    doing *only* this change had no effect, at least for our firmware.

  - I then found a forum post that stated you can use a different
    `start.elf` files on the SD card to do the partitioning.  (This was
    a WTF of supreme magnitude: such a bad design to require replacing a
    random black-box file in addition to modifying `config.txt` especially
    when the official documents seem to not mention the need to do so!)
    However, it was unclear where to get this magic file and there were
    tons of later posts stating that this was an old method so don't
    do it.  Unfortunately, I tried the "new" approach with no success.

  - Finally, I went back to trying to use different `start.elf` files ---
    it turns out that this replacement *is* a valid method, and at
    least for our bare-metal approach, possibly the *only* valid
    method.  Credit to this post on making [the pi boot as fast as
    possible](https://www.raspberrypi.org/forums/viewtopic.php?t=84734).
    Without it, I might have given up.

So, great: let's change your memory to a reasonable amount from "first principles":

  1. The [elinux](https://elinux.org/RPi_Software) website describes the different
     `start.elf` file options.   They state there is a stripped down one that works
      with `gpu_mem=16GB`.

  2. Go to the [linux firmware site](https://github.com/raspberrypi/firmware/tree/master/boot)
     and download the right `start` and `fixup` file.

  3. Copy these files to your SD card, and create a new `config.txt`
     that is simply:


            # config.txt: save the old one!
            gpu_mem=16
            start_file=start_cd.elf
            fixup_file=fixup_cd.dat

  4. Plug the SD card back in your pi (after doing a `sync`) and re-run
     your mailbox code to see that the memory size: it should be 496MB.

Great!   You may well have saved a couple of annoying hours and now have
almost 4x more physical memory.  With that said: there are potential
downsides to this increase:

  1. If you ever do graphics and need the GPU, you will need to increase memory
     back up to as usuable number --- 16MB is far too small.
  2. More memory bookkeeping data structures, including the work mapping
     physical memory into each process's address space (at least until
     we use a single system page table).  If you were making a shippable
     system, counter-intuitively you might want to artificially limit
     the memory available so that it could be more stripped down / faster.

