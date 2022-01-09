### Pre-lab for 0-blink

Read through the following before our first lab pi lab (Tues).  Since this
is the first lab and people are missing context, we over-explain ---
the other labs will be more succinct.

For this lab, we'll use different variations of a blinky light to check
that parts of your toolchain / understanding are working.

You'll likely forget this link, but if at any point you get confused
about some basic concept, cs107e has a good set of guides on electricity,
shell commands, pi stuff in general:
   - [CS107E repo] (http://cs107e.github.io/guides/)

--------------------------------------------------------------------
### Deliverables before `0-blink`:

To re-iterate the last PRELAB, make sure you:

 1. Figure out how to add a local `~/bin` directory in your home directory
    to your shell's `PATH` variable.
 2. Have a way to mount a micro-SD card on your laptop.  You may need
    an adaptor. 
 3. Make sure you know how to use common unix commands such as: `ls`,
    `mkdir`, `cd`, `pwd`.
 4. Read through the [README](README.md) and look through the code.
 5. Ideally: Install the rest of the software needed by Part 3 and Part 
    4 of the lab (described in the `README`).

The Tuesday lab (`0-blink` and `1-gpio`) is broken down into five parts:
  1. You'll turn on an LED manually;
  2. Then copy a pre-compiled program to an micro-SD and boot up your pi;
  3. Then use a bootloader to ship the program to the pi directly;
  4. Then install the r/pi tool chain, compile a given assembly
       version and use it;
  5. Then write your own `blink` program, compile and run it;

You should be able to answer the following questions:

  1. What is a bug that is easier to find b/c we broke up steps 1 and 2?
  2. What is a bug that is easier to find b/c we broke up steps 2 and 3?
  3. What is a bug that is easier to find b/c we broke up steps 3 and 4?
  4. What is a bug that is easier to find b/c we broke up steps 4 and 5?

Differential debugging: Your pi setup in step 2 is not working. 
Your partner's does.  You plug your setup into their laptop.

  1. Yours works in their laptop: what do you know?
  2. Yours does not work in their laptop: what do you know?
  
--------------------------------------------------------------------
#### Part 1: micro-SD card stuff

In order to run code on the pi, you will need to be able to write to a
micro-SD card on your laptop:

   1.  Get/bring a micro-SD card reader or adaptor if its not built-in
       to your laptop.  The [CS107E guide on SD
       cards](http://cs107e.github.io/guides) is a great description
       (with pictures!).

   2. Figure out where your SD card is mounted (usually on MacOS it is in
      `/Volumes` and on linux in `/media/yourusername/`, some newer linuxes 
      might put it in `/run/media/yourusername/`).   Figure out
      how to copy files to the SD card from the command line using
      `cp`, which is much much faster and better than using a gui (e.g.,
      you can put it in a
     `Makefile`, or use your shell to redo a command).  For me,

           % cp kernel.img /media/engler/0330-444/
           % sync
 
      will copy the file in `kernel.img` in the current directory to the
      top level directory of the mounted SD card, the `sync` command forces
      the OS to flush out all dirty blocks to all stable media (SD card,
      flash drive, etc).  At this point you can pull the card out.

      Pro tip: ***do not omit either a `sync` or some equivalant file
      manager eject action*** if you do, parts of the copied file(s)
      may in fact not be on the SD card.  (Why: writes to stable storage
      are slow, so the OS tends to defer them.)

--------------------------------------------------------------------
#### Install the rest of the code 

Gates wireless can really suck, so it's best if you download the rest
of the code that you need: look in Part 3 (serial drivers) and Part 4
(gcc toolchain) of the lab, and install the software it says.~~

We don't have to worry about Gates wireless this year, but you'll still want to 
download everything else before lab.  Look in Part 3 (serial drivers) and Part 
4 (gcc toolchain) of [the lab](README.md), and install the software it says.
