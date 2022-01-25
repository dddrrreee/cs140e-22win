"interpose" sits between your pi and your unix server and forwards all
messages between them.   

This is a simple test to make sure the method it uses (pseudo-tty's)
works on your machine.


 1. Plug in your pi.

 2. If on linux and your pi is mounted on /dev/ttyUSB0:

            % ./interpose.linux pi-install /dev/ttyUSB0 hello.bin

    If on macos, run "interpose.macos" and give the path to the device.
   

 3. You should get something similar to the following:


        % ./interpose.linux pi-install /dev/ttyUSB0 hello.bin
        TRACE:going to forward: pi-install /dev/ttyUSB0 hello.bin>
        opened tty port </dev/ttyUSB0>.
        TRACE:using </dev/pts/20> for install <pi-install>
        TRACE:<pi-install> pid = 1498685
        TRACE:about to forward
        opened tty port </dev/pts/20>.
        pi-install: tty-usb=</dev/pts/20> program=<hello.bin> about to boot
        HASH: simple_boot: sending 3372 bytes, crc32=cf4943ae
        waiting for a start
        putting code
        bootloader: Done.
        listening on ttyusb=</dev/pts/20>
        hello world
        DONE!!!
        
        Saw done
        
        bootloader: pi exited.  cleaning up
        TRACE:SUCCESS: server cleanly
        TRACE:pi sent 36 bytes to unix, unix send 3392 bytes to pi
        TRACE:done forwarding:exited cleanly
