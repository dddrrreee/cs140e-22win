### Overview
We're going to be working with the FAT32 filesystem for this lab. As you 
learned in CS110, there are a lot of different filesystems, each with its own 
pros and cons.  However, the Pi insists that our SD card uses FAT32, which kind 
of forces us to do the same for our kernel. Note that _technically_, we could 
just have a tiny FAT32 partition at the beginning of the SD card and do our own 
thing afterwards, like Linux does, but that adds yet another moving part to 
deal with.  For simplicity, we're just going to use one partition for the whole 
disk.

--------------------------------------------------------------------------
### Reading
  - [paul's summary](https://www.pjrc.com/tech/8051/ide/fat32.html).  This has
    some useful pictures.   An [annotated version](./docs/pauls-fat32.annoted.pdf)
    is in the `docs/` directory: this is a good place to start.
  -  [wikipedia](https://en.wikipedia.org/wiki/Design_of_the_FAT_file_system).
     full description.  Perhaps more detail than you need, but useful
     for all the weird flags that can pop up.   
     An [annotated version](./docs/wikipedia-fat32.annotated.pdf) is in the `docs/` directory.  It is a good companion to Paul's
    writeup.  It has the most thorough LFN discussion.
  - [aeb's summary](https://www.win.tue.nl/~aeb/linux/fs/fat/fat-1.html).
  - [data structure layout](http://www.c-jump.com/CIS24/Slides/FileSysDataStructs/FileSysDataStructs.html) this has useful tables.
  - [cpl.li's writeup](https://cpl.li/2019/mbrfat/) -- more pictures.  Some
    extra sentences.
  - [OSDev's partition writeup](https://wiki.osdev.org/Partition_Table).

  - [linux source](https://elixir.bootlin.com/linux/latest/source/fs/fat/dir.c).
  - The file system implementation chapter from the "three easy pieces" book 
    (in the `docs/` directory).
  - Microsofts specification (in the `docs/` directory).
  - [forensic wiki](https://www.forensicswiki.org/wiki/FAT) --- seems to be gone.

If you're confused by "little-endian" versus not:
  - [little endian](https://en.wikipedia.org/wiki/Endianness#Little-endian).

Once you've done these readings, you should:
- know the differences between a sector, a cluster, a volume, and a partition
- be able to verify that a disk has a FAT32 filesystem on it
- be able to find the root directory of a FAT32 filesystem
- be able to find the first cluster of a file within a directory
- be able to find the full contents of a file given the index of the first 
  cluster

--------------------------------------------------------------------------
### Check your kmalloc works.

The directory `0-kmalloc` has a simple test of tests for your kmalloc.  You
need to have:
  - `kmalloc_aligned`
  - `kmalloc_init_set_start` (to set where the heap starts).

Make sure:
  - you always return an 8-byte aligned pointer if no alignment is specified.
  - Zero fill memory.
  - Don't over allocate.

You should be able to test with:

    make
    make test

If that fails, try running it manually and see what the output/error is.

--------------------------------------------------------------------------
### Get an SD card driver.

In general, we prefer to write our own code from primary sources
(datasheets, ARM manuals):
  1. Otherwise you really don't know what you're talking about and (related);
  2. Whenever we do so, we realize how broken most of the other code out there 
     is.

However, for larger drivers, we'll lower our standards and steal other
people's code.   This is a useful thing to do even if you are going to
write from scratch so that you can cross-check against a purportedly
working implementation.  (A good project for the class would be building
a driver cross checking framework --- I can pretty much guarantee there
are different tricky behaviors in the ones out there.)

One useful ability is being comfortable ripping code out of its original
source-tree and making shims to have it work in yours.  Today's lab will
be good practice for doing so.

   1. Go to [bzt's repo](https://github.com/bztsrc/raspi3-tutorial/tree/master/15_writesector)
      and get `sd.c`, `sd.h` and `gpio.h`.

   2. Put these in the `0-sd-driver/external-code` directory, rename them with 
      a `bzt-` prefix (`bzt-sd.c`, `bzt-sd.h`, `bzt-gpio.h` --- update any 
      includes).

   3. Make them compile on our code.

   4. You'll have to `#define` various functions: `wait_msec` `uart_puts` and 
      `wait_cycles` (I put these all in `sd.c`) but there shouldn't be much 
      difference.    In general, we prefer external changes versus changing the 
      code directly so that we minimize the work if we want to pull updates 
      from the original code base.

   5. This code is for the 64-bit rpi3 so you'll get a few warnings about
      printing variables; you can comment these two cases out.  You will 
      definitely have to change where it thinks the base of the GPIO memory 
      region is (we use `0x20000000`).  If the code hangs, it's because this 
      base was not set.
      (This can be a tricky error to figure out, so remember this fault mode
      for when you port code from other r/pi models to yours.)

   6. You'll also get undefined reference to a division function.
      If you look at the code, you'll notice the division is by a constant
      passed as a function argument, which is good news because it lets us 
      eliminate the problem by simply declaring the routine as `inline` (`gcc` 
      will replace the division by shifts, adds and possibly multiplications 
      --- you can examine the `.list` file to check).

      If you are having a hard time finding the 
      division, run :

              arm-none-eabi-objdump -d external-code/bzt-sd.o >x

      and look for the calls to division.

The helper routine `pi_sec_read` `pi-sd.c` uses the code you imported.
It allocates a buffer needed to hold `nsec` worth of data, read the data
in, and return a pointer to the buffer.

After 10-20 minutes you should have a working driver.  You should be able to 
run `make test` and have `tests/0-sd.bin pass`.

--------------------------------------------------------------------------
### Take a look at the Master Boot Record

To be sure our SD card driver works, take a look at the Master Boot Record 
(MBR) on the SD card (if you don't remember what that is, review the reading at 
the top).  The MBR is the first sector on the disk, so it's really easy to 
find; its Logical Block Address (LBA) is 0.

Open `mbr.c` and implement `mbr_read()`.  This function should only be a few 
lines long.  Once you do this, you should be able to pass `tests/1-mbr.bin`.
