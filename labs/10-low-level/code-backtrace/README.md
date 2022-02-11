### Backtrace

Again, the cs107e website has a good writeup.  This is a more quick and
dirty run through.

If you look at the `test-backtrace.list` file, you can see at the start
of each routine there's a few instructions similar to the following:

        00008274 <backtrace_print>:
            8274:   e1a0c00d    mov ip, sp
            8278:   e92dd800    push    {fp, ip, lr, pc}
            827c:   e24cb004    sub fp, ip, #4

Where these three instructions do the following:

  1. Moves the original `sp` before we modify it to a temporary register `ip`:

            8274:   e1a0c00d    mov ip, sp

  2. It then stores four registers --- the original `fp`, the original
     stack pointer (held in `ip`), the return address `lr`,
     and the current `pc` onto the stack.

            8278:   e92dd800    push    {fp, ip, lr, pc}

     Note 1: the stack grows down so after this push will have a smaller
     value.

     Note 2:  Registers are stored in order so that the lowest numbered
     register is at the lowest address (top of the stack).  
    
     Note 3: the value stored for `pc` will be some number of instructions beyond
     the load: you should print it out and see!  You can look in the arm manual to 
     see the definition as well.

  3. It modifies `fp` by subtracting 4 so that `fp` now points at the
     first word of the four saved registers.  In this case, `fp` points
     to where `pc` is stored since `pc` (`r15`) is the highest register
     number and thus will be stored at the highest address.

Based on the logic above, we can define a structure: 

        struct bt {
            struct bt *next;
            uint32_t sp;
            uint32_t lr;
            uint32_t pc;
        };

Where the fields are sorted by by register number.  (Recall: C memory
grows upward!)  If we subtract 12 bytes from the `fp`, that should be
the start of this structure. You can walk up the stack using next (again:
minus 12) until `next` is null.

Note:
  - `gcc` has a useful intrinsic `__builtin_frame_address (0)` to get the 
    framepointer.

