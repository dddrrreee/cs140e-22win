This is mostly just the code from the pre-lab, split into different files.

   - `pi-random.[ch]` amd `fake-random.[ch]`: you can ignore these four
      files.  They are our pseudo-random number generateor (PNG).
      We have our own private PNG so it's easy to guarantee everyone
      gets the same numbers.

   - `fake-driver.c`: this just has the `main` from the prelab.
   -  `fake-pi.h` some simple definitions.      
   - `fake-put-get.c`: your fake PUT32 and GET32 implementation.  It looks basically
     like the prelab, except you should handle all addresses.

how to handle spinning on a pin (or reg) until it changes?
how to handle time.
how to handle input.
    can just record transitions.

    - i think if there are multiple reads in a row to the same location, just emit
      when it changes

    - cleanest approach is to have a log and emit at the end.

have them write this:
for memory:

    // track tuples

    struct { uint32_t addr, uint32_t val };

    void insert(mem_t *m, uint32_t addr, uint32_t v)
    struct mem *lookup(mem_t *m, uint32_t addr, uint32_t v)
    struct mem *last(mem_t *mvoid);

    if(!lookup())
        insert()
