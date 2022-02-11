## Mailboxes

The pi is a bit weird in that the GPU controls a lot of the action.
As described at:

   - [mailboxes](https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface)

The pi gives a way to send messages to the GPU and receive a
response.  You can use this to query and configure the hardware.
The mailbox interface is not super-intuitive, and the main writeup uses
passive-declarative voice style that makes it hard to figure out what
to do.  (One key fact: the memory used to send the request is re-used
for replies.)  When I got the mailbox working, it also claimed 128MB.

If you look through the (unfortunately incomplete) [mailbox
writeup](https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface)
you'll see all sorts of useful facts you can query for --- model number,
serial number, ethernet address, etc.  So it's worth figuring out how
to do it.

So that's what we will do.  Some hints:

   1. As always: if you want to write the code completely from scratch,
      I think doing so is a valuable exercise.  However, in the interests
      of time I put some starter code in `part0-mailbox/`.  Extend it
      to query for physical memory size along with a couple of other
      things that seem useful.

Rules:
  1. Buffer must be 16-byte aligned (because the low bits are ignored).
  2. The response will overwrite the request, so the size of the buffer
     needs to be the maximum of these (but not the summation).
  3. The document states that additional tags could be returned --- it may
     be worth experimenting with sending a larger buffer and checking
     the result.

Write code to get:
  - model number
  - revision number
  - memory size
