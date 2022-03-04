## Simple networking with the ESP8266

The most important rule for today:

   ***MAKE SURE YOU ONLY CONNECT the ESP TO 3V (NOT 5V) power***

This lab we will use the ESP8266 chip (version: esp-01) to connect your
pi to your laptop and/or to another pi.

The ESP8266 is a cheap, reasonable way to add wireless to various
projects assuming they don't need extremely low latency or high bandwidth.
It can can assign wireless IP addresses (be a dhcp server), a webserver
and communicate using TCP or UDP.

We will control the ESP8266 using `AT` commands, which means we will
send the ESP8266 human-readable character string commands (the first
token will be an `AT`, hence the name) using the same 8n1 UART protocol
we use to communicate with your pi.  You should look in the `docs`
directory and read:

Workflow:

 0. Get two esp8266's, one programmer (see below) and another tty-USB device.
 1. Make sure both esp's work.
 2. See how to communicate on Unix.
 3. Convert the code to communicte on the pi.
 4. Do something cool.

#### Checkoff

Deliverables: today is kind of a light lab:

  1. You can ping-pong a value with your patner using TCP on Unix.
  2. You can ping-pong a value between your laptop and your pi.
     (From this stepping stone you can (if so inclined) build a network
     version of `GET32` and `PUT32` and control a pi remotely.)
  3. Do something interesting with the esp (below).

------------------------------------------------------------------------------
### Part 0: Make sure your esp8266's work

Before doing anything, make sure that the `esp8266` device you have
actually works.

##### Quick test hardware  with the programmer

As the quickest test, plug it into the USB programmer we gave out and
see that `esp-shell` gives an `OK` when you type `AT`.

  1. The esp is the small device on top, and the programmer is the larger
     device plugged into my USB port.  Make sure that the little switch
     (red arrow) is pushed to the left:

<p float="left">
  <img src="images/esp8266-prog.jpg" width="450" />
</p>

     The programmer is cool since it makes it easy to update the ESP's
     firmware.  (Ours is pretty old: 2016.)

  2. Once you plug it into your laptop, run the shell:

            % cd esp-shell/
            % make
            % ./esp-shell --ack
            about to communicate with </dev/ttyUSB0>
            AT   #    <---- type this
            AT   #    <---- esp first echoes command
            OK   #    <---- esp then sends "OK"

   3. Try the other ESP with the programmer as well (since it's hard to miswire).


##### Test with a tty-USB device

Reconnect one of them to the tty-USB device you picked up and make sure
it also works using the same steps above.  Which pin is which is printed
on the underside of the ESP.

The most important rule:

   ***MAKE SURE YOU ONLY CONNECT TO THE 3V (NOT THE 5V)***

   ***MAKE SURE YOU ONLY CONNECT TO THE 3V (NOT THE 5V)***

   ***MAKE SURE YOU ONLY CONNECT TO THE 3V (NOT THE 5V)***

   ***MAKE SURE YOU ONLY CONNECT TO THE 3V (NOT THE 5V)***

   ***MAKE SURE YOU ONLY CONNECT TO THE 3V (NOT THE 5V)***

It will fry, and I only have so many of them.

If you don't get any communication, the tty-USB could be broken or,
much more likely, you could have miswired.

##### Test that you can ping two devices at once

Before you start writing code, make sure you can run the `client` and
`server` programs in `1-ping-pong.  This will check:

 - Our code works in your environment.
 - Your laptop does indeed support two USB devices at once.

If it fails:
   - The bug likely is in your `find_ttyusb_first()` or `find_ttyusb_last()`.


What to do:
   0. Change the `NETWORK` constant in `libesp/esp-constants.h` to be some 
      kind of variation on your stanford SUID (so you don't conflict with 
      anyone else).  

   1. Compile:

        % cd 1-ping-pong
        % make

   2. Plug both devices into your laptop.

   3. In one window first start the server:

            % ./server
            esp-run.c:cmd_puts:51:14: issuing exp-cmd <AT+GMR>
            esp-parse.c:match_linev:244:matched line <AT+GMR>!
            esp-run.c:cmd_ack:232:esp ack'd cmd <AT+GMR>:15
	            skipping extra:: <AT version:1.1.0.0(May 11 2016 18:09:56)>
	            skipping extra:: <SDK version:1.5.4(baaeaebb)>
	            skipping extra:: <compile time:May 20 2016 15:08:19>
            esp-parse.c:match_linev:244:matched line <OK>!
            Going to wait for a connection
            0: waiting on input from esp
            100: waiting on input from esp
            200: waiting on input from esp
            ...

   4.  In another window after the server prints its waiting for input start
       the client:

            % ./client


       After a bit (the client is slow to connect) you should see values being sent back and
       forth:


            ...bunch of output sending a counter back and forth...

            going to wait on ch=0
            received ack from client=0: <hello 126>
            passed check!
            sending <hello 127>
            0: waiting on input from esp
            going to wait on ch=0
            received ack from client=0: <hello 127>
            passed check!
    
            SUCCESS!
    
       This should send data back and forth successfully.  

Useful:

  - You can easily see the commands run by this program by looking at the
    log files in the directory.  You can in theory type these commands
    into the shell yourself and get the same result.

------------------------------------------------------------------------------
### Part 1.

The `1-ping-pong` lab has a client and a server program.  You should be able to
plug in two esp's and run the server using one tty and the client on the other.
   1. start there server first.
   2. after it is waiting for data, then start the client.
   3. they should send hello messages back and forth.
   4. There are logs in the directory after each run.  You can also switch
      between input options in `libesp/esp-lex.h` which gives and example
      of using `ifdef` to switch between implementations.
        
### Part 1:  make a copy that does PUT32 and GET32

The code you'll modify is in the `libesp/esp-commands.c`.  First make
sure the ping pong works.  Then start generalizing it.

Copy the ping pong code to a new directory:
  1. Change it so you can do a `PUT32` and a `GET32` and the values are
     correct back and forth.

  2. Measure the overhead.

### 2. realism.

Start adding functionality to your `libesp`.   Please keep all your code in
`esp-commands.c` so that I can push any bug-fixes without conflicts.

  1. Try to handle mutiple client connections.  this will require you
     can accept `CONNECT` and `DISCONNECT` messages during normal message
     processing --- you will need to modify the out-of-order messages handler
     to do this.   Connect from multiple esps.

  2. Handle errors.  for the client, write the code to do a close and modify the 
     server to handle it.  Also, have the client reboot (versus getting stuck) if the
     server goes away.

### Homework: the ESP8266 working with the pi with interrupts.

I checked  in a hardware `cs140e-objs/uart-int.o` --- add this to your
`put-your-src-here.mk` and run the example to make sure things work.
You can start writing the `pi-support.c` code in `libesp` and getting the 
code to run.

Will write this up this week.  Due two weeks after.

### Homework: build `net_GET32` and `net_PUT32` and control the GPIO on another pi.

When you have the above working, make network versions of the `GPIO` routines
and control another pi.

### Homework: build `net_GET32` and `net_PUT32` and control your pi from your laptop.

Will write this up this week.  Due two weeks after.
