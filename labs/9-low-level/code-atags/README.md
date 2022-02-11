### ATAGs

The internal bootloader (that sets up the hardware and then loads our
`kernel.img`) passes parameters to `kernel.img` using  `ATAGS` which
is a primitive take on a "key-value" store used by the pi firmware
bootloader (`bootcode.bin`).

This table has two big constraints:
  1. It has to work across different programs, likely
     written in different languages.
  2. There must be a way to write generic code that can iterate
     over these keys without understanding them.  Otherwise, you could
     never add new ones, among other issues.

Their hack: the first two 32-bit words of each `ATAG` record are as follows:
  - word 0: gives the size of the entire `ATAG` record (a count of 32-bit words)
  - word 1: the 32-bit tag.
  - The end of the ATAG list has its tag equal to `ATAG_NONE`.

Thus, to iterate over a list:
  - check the tag word and stop if its `ATAG_NONE`
  - otherwise, get the next record by incrementing the current record pointer
    by the wordcount number of bytes.

The header `atag.h` gives a structure to make this a bit cleaner:

        // linux bootloader atag discussion:
        //   http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html
        // struct definitions.
        //   http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html#ATAG_CORE
        typedef struct atag {
                uint32_t size;      // length of tag in words including this header 
                uint32_t tag;       // tag value 
                // pointer to the op-specific data in the atag record
                uint32_t data[];    // struct hack
        } *atag_t;


Along with some iterators.

The key tag we want is `ATAG_MEM` to get the memory size.  You can see
the structure definition for this tag at:

   - [struct definitions](http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html#ATAG_CORE)

Confusingly, when I did this, the `ATAG` entry claimed the pi had
128MB of physical memory.  This was way too small, so I thought I had
misinterpreted (or the bootloader had screwed up).   Turns out that
it's the expected default, so we (in a bit) setup your pi to use the
full physical memory.


For the life of me, other than that wiki page, I cannot
find any information in the main Broadcom PDF.  [This
blog](http://magicsmoke.co.za/?p=284) does have a clear writeup, but it
would be nice to have a primary source:

Some useful pages
   - [linux bootloader atag discussion](http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html)

    - [ATAGS](http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html#ATAG_MEM)


There's a starter simple `ATAG` implementation in `atag.h`--- this isn't a
super useful interface on the pi since it (appears to) only provide three
different key-value pairs.  However, the approach is a neat (albeit gross)
hack for passing arbitrary key-values in an extensible way.  Funny enough,
I had "re-invented" something similar as a way to pass information from
our bootloader to the process that it started.  You might want to keep
this hack in mind for your later systems.  Gross, but pretty robust.

Checkoff:
  - fill in the rest of the `atag.h`.
  - make sure prints out a sensible number for the amount of MB we have on the
    pi --- I have 128MB.

