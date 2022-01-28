## The labs

***The labs below are tentative --- we still need to check in updated code
and there will likely be some fussing about.  Take them as a suggested
roadmap***

Below describes where we're going and why.   The labs themselves have a
lot more prose on each topic.  There is a pointer to where we currently
are: the labs after this point can still see significant revisions.

I would strongly recommend reading the labs carefully before starting.
And then, once you finish, going back and reading them for things you
missed the first time, and also looking closely at your code --- for 
every line, you should understand why we needed it or what it does.
And on second reading, you'll likely find some lines that we could have
cut out!

---------------------------------------------------------------------
### Part 0: non-pi hacking

Given the current Stanford-forced virtual teaching, we are first doing
a Unix-based lab while we get equipment to everyone:

  - [0-trust](0-trusting-trust): Ken Thompson is arguably our patron
    saint of operating systems --- brilliant, with a gift for simple
    code that did powerful things.   We will reimplement a simplified
    version of a crazy hack he described in his Turing award lecture
    that let him log into any Unix system in a way hidden even from
    careful code inspection.

---------------------------------------------------------------------
### Part 1: Going down.

The first chunk of the class will be writing all the low-level code needed
to run the r/pi and using modern techniques to validate it.  Doing so will
remove all magic from what is going on since all of the interesting code
on both the pi and Unix side will be written by you:

  - [0-blink](0-blink/): get everyone up to speed and all
    necessary software installed.  This should be a fast lab.

  - [1-gpio](1-gpio/): start getting used to understanding hardware
    datasheets by writing your own code to control the r/pi `GPIO` pins
    using the Broadcom document GPIO description.  You will use this to
    implement your own blink and a simple network between your r/pi's.

  - [2-cross-check](2-cross-check/): you will use read-write logging
    of all loads and stores to device memory to verify that your GPIO
    code is equivalent to everyone else's.  If one person got the code
    right, everyone will have it right.

    A key part of this class is having you write all the low-level,
    fundamental code your OS will need.  The good thing about this
    approach is that there is no magic.  A bad thing is that a single
    mistake can make a miserable quarter.  Thus, we show you modern
    (or new) tricks for checking code correctness.

  - [3-bootloader](3-bootloader/): two of the biggest pieces of code
    we've given you have been the Unix-side and pi-side bootloader code
    (`pi-install` and `bootloader.bin` respectively).  So this lab has
    you implement your own and get rid of ours.

    The most common r/pi bootloader out there uses the `xmodem` protocol.
    This approach is overly complicated.  You will do a much simpler,
    more-likely-to-be-correct protocol.  It has the advantage that it
    will later make it easier to boot over the network.

  - [4-uart](4-uart/): the last key bit of code we've given you is for
    controlling the UART (`uart.o`); so this lab has you write your own
    and remove ours.

    To do so you'll write your first real device driver --- for the UART
    --- using only the Broadcom document.  At this point, all key code
    on the pi is written by you.  You will use the cross checking code
    from lab 2 to verify your implementation matches everyone else's.


We've reversed the next two labs  to give people time to finish up
the bootloader.  We'll do a simple interrupt lab so you have a working
example (and thus, the readings will make more sense) and then do an
interesting trick for transparently testing the bootloader code under
a near-exhaustive set of failures --- if it passes you will be very
surprised if breaks in the future.


  - [5-replay](5-replay): In a twist on lab-3,
    you will use Unix system calls to interpose between your Unix and
    pi bootloader code, record all reads and writes, and test your
    bootloader implementation by replaying these back, both as seen and
    with systematic corruption.

    This approach comes from the model-checking community, and I believe
    after you implement this lab and test (and fix) your bootloader you
    will be surprised if it breaks later.  (In general, the approach
    we follow here applies well to other network protocols which have
    multi-step protocols and many potential failure modes, difficult to
    test in practice.)

    Using read-write equivalence and simple systematic exploration will
    let you check the code thoroughly enough that you will be surprised
    if the code ever breaks.

At this point you have written low-level device driver code, a bootloader,
and pretty much replaced all of our code.  You've learned how to crush it
under a barrage of automatic checks (especially if you do the optional
lab) and done a simple implementation of a fake pi environment that
allows you to check correctness of pi code using your laptop.

-------------------------------------------------------------------------
### Part 2: Execution: interrupts, threads, user-level processes.

  - [6-interrupts](6-interrupts/): (note: we have flipped this
    with lab 6) you will walk through a simple, self-contained
    implementation of pi interrupts (for timer-interrupts), kicking
    each line until you understand what, how, why.  You will use these
    to then implement a version of `gprof` (Unix statistical profiler)
    in about 30 lines.

    Perhaps the thing I love most about this course is that because we
    write all the code ourselves, we aren't constantly fighting some
    large, lumbering OS that can't get out of its own way.  As a result,
    simple ideas only require simple code.  This lab is a great example:
    a simple idea, about twenty minutes of code, an interesting result.
    If we did on Unix could spend weeks or more fighting various corner
    cases and have a result that is much much much slower and, worse,
    in terms of insight.

  - [7-device-interrupts](8-device-interrupts): if you keep doing this 
    kind of work the single most common
    fancy "OS" type thing you'll likely do in the future is to setup
    GPIO pin interrupts so that you can get notified when a hardware
    device has data.  So, in this lab we'll setup GPIO interrupts
    and tune how we do general interrupts.

    Without interrupts, it's difficult to get networking working, since
    our GPIO pins (and our UART options) have limited space and, thus,
    unless our code checks them at exactly the right time, incoming
    messages will vaporize.

    We've spent two labs on interrupts, but this can be a tricky topic --
    both because of correctness issues and because you have to get used
    to understanding architecture manuals.   This understanding will
    help you in the next two labs.  The fact that you have become much
    more comfortable saving and restoring registers will save you many
    debugging headaches in the next week.


We are here ===>


  - [8-threads](9-threads/): we build a simple, but functional
    threads package.  You will write the code for non-preemptive context
    switching:  Most people don't understand such things so, once again,
    you'll leave lab knowing something many do not.

Tentative:
   - For lab 10: Akshay and I are debating, but it seems likely we'll
     do user-level processes and/or dynamically linked tasks so that
     it's easy to add different types of features (such as interrupts
     or memory protection) without modifying the application.

   - The following labs are going to see some major surgery.

-------------------------------------------------------------------------
### Part 3: file systems.

  - [11-fuse.A](11-fuse/): In this lab you will use the FUSE file
    system to wrap up your pi as a special file system and mount it on your
    laptop, where you can use standard utilities (and your normal shell)
    to interact with it.  You can then control the pi by reading / writing
    to special files: e.g., echoing a `1` to `pi/reboot` to cause a reboot,
    echoing a program to `/pi/run` to run it.

    This lab is a great example of the power of Unix's simple, powerful
    OO-interface that lets you package a variety of disparate things as
    files, directories, links and interact with them using a uniform set
    of verbs (e.g., `open()-read()-write()-close()`).

  - [12-fuse.B](12-fuse/): We continue fuse since it was a big lab :). 

  - [13-fat32](13-fat32/): The SD and virtual memory are the biggest
    unknowns in our universe, so we'll bang out quick versions of each,
    and then circle back around and make your system more real.

    You will write a simple read-only FAT32 file system that can read from
    your SD card. You will use this to do a very OS-style "hello world":
    use your FAT32 to read `hello-fixed.bin` from last lab from your SD
    card, jump to it and run it.

### Part 4: Virtual Memory

  - [14-vm](14-vm/): You take a working VM system and, using the
    ARM documents, replace its page table manipulation with your own.
    You also add exception handling to catch protection and missing
    translation faults.   The former gives you the tools to start doing
    user-level protection, the latter for demand paging and extending
    a stack as code accesses it.

  - [15-vm-ops](15-vm-ops/): The previous lab defined the main noun in the
    virtual memory universe (the page table); this lab does the main verbs
    used to set up the VM hardware, including how to synchronize hardware,
    translation, and page table state (more subtle than it sounds).
    At the end you should be able to delete all our starter code.

***[HAVE NOT UPDATED THE LABS BELOW]***

#### Part 5: processes

  - [16-processes](16-user-level): today you will make user level
    processes.  This will combine the virtual memory, system calls, and
    threads into a capstone implementation.  You will use this ability to
    your fuse-FS support multiprocessing, so that you can have multiple
    programs running at the same time.

  - Additional labs that we can fit in to make the above more real and more
    integrated.

### Optional labs:


  - virtualization: this lab will show how
    to virtualize hardware.  We will use simple tricks to transparently flip
    how your pi code is compiled so you can run it on Unix, only shipping
    the GPIO reads and writes to a small stub on the pi.  As a result,
    you have full Unix debugging for pi code (address space protection,
    valgrind, etc) while getting complete fidelity in how the pi will behave
    (since we ship the reads and writes to it directly).

  - sonar-int: we take a bit of a fun break,
    and bang out a quick device driver for a simple sonar device. You
    will then get a feel for how interrupts can be used to simplify code
    structure (counter-intuitive!)  by adapting the interrupt code from
    the previous lab to make this code better.

