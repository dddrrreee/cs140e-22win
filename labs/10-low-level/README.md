### Low-level hacks: Mailboxes, ATAGS, and backtraces.

The last few labs have involved writing some pretty tricky code.  We take
a bit of a pause and do some fairly simple but useful low level hacks
that do not depend on the previous labs so that people can get everything
sorted out.

  - `ATAGS`: OSes use these as a way to pass various useful parameters.
    We will use these to get the memory size.

  - mailboxes: the pi gives a way to get and set a variety of attributes by
    sending messages to  different mailbox addresses.  

    As a simple hack: You'll use this to get the memory size (along with
    some other things) and compare it to the `ATAGS` value.

  - Increasing your pi memory size: the pi ships with a bunch of memory.  By
    default we can't access it.   You'll change that.

  - Stack backtraces: so far, when you get an assertion error or print a message
    we only have information about the file, function, and line the assertion
    is at --- it can be hard to figure out what is going on without the 
    caller(s).  You'll write a simple backtrace implementation that walks
    back up the stack and gets the current callers.

#### 1. ATAGS

We have a simple explanation in [atags](code-atags/README.md).  You sohuld
fill in the missing code in `code-atags/atags.h`) so that you can see how
they work and use them to get the memory size of the pi.

Copy this header to `libpi/src`.

#### 2. mailboxes

We have a simple explanation in [mailbox](code-mbox/README.md).
You should fill in the missing code in `code-mbox/mbox.h`) so that you
can see how they work and use them to get the memory size of the pi.


Note: the serial number should be different across all pi's.  For the pi zero, I was
getting 0 for the model number.

Copy this header to `libpi/src`.

#### 3. Increase your pi memory to 496MB

Look in [increase memory](increase-mem/README.md) to see how to 
increase your pi memoyr and get it up to 496MB.

The program in the directory should pass.

#### 4. Implement stack backtraces

The CS107E website has a nice writeup on 
[building backtraces](http://cs107e.github.io/assignments/assign4/)

Implement this and show it works!  (We'll push some code in a bit).

There is a trivial program in `code-backtrace` to test (weakly) test it.
It manually prints the call stack --- when you print the backtrace it
should match this (different formatting is ok).

### Extension: exception backtraces

If we get a weird exception, we'd like to print out where it the original
code it came from (besides just the exception pc) --- the issue here is
that you need to get the original frame pointer so we can walk backwards.
This shouldn't require too much work, but is useful.
