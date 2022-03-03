### ESP8266 prelab

***Crucial: make sure you bring any adapters you need so you can plug
two USB devices into your laptop at the same time.***

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

  - [AT command examples](docs/4b-esp8266_at_command_examples_en.pdf).
  - [AT command definitions](docs/4a-esp8266_at_instruction_set_en.pdf).

Additionally, for coding:
  - In `libunix`: make sure you have implemented `find_ttyusb_last`
    (find the newest tty-usb device that has been attached --- the
    "last" one) and `find_ttyusb_first` (find the oldest tty-usb that
    has been attached --- the "first" one) since they will make the lab
    alot easier.

  - Make sure your GPIO interrupt code still works
    (`7-device-interrupts/1-gpio-int`).  If you have time, make both
    a software UART receive that works at 115200 and extend it to use
    interrupts.  Doing so will let you use your own code to drive the
    ESP8266 on the pi.   This will be one of the extensions.
