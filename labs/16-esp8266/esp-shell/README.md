### ESP shell program


Trivial shell that you can use to test commands: type commands and it
will send to the ESP and echo replies.  It can do some simple checking
to see that what you got was what you expected.

From `docs/4a-esp8266_at_instruction_set_en.pdf`: Some simple commands
that will work:

    AT          # just test
    AT+GMR      # get the version information
    ATE0        # turn off echo
    ATE1        # turn on echo
    AT+RESTORE  # restore to factory settings
    AT+RST      # do a hard reset


E.g.,:

        > AT
        AT
        OK
        > ATE0
        ATE0
        OK
        > AT
        OK
        > ATE1
        OK
        > AT
        AT
        OK
        > AT+GMR
        AT+GMR
        AT version:1.1.0.0(May 11 2016 18:09:56)
        SDK version:1.5.4(baaeaebb)
        compile time:May 20 2016 15:08:19
        OK
        > AT+RESTORE
        AT+RESTORE
        OK
        ets Jan  8 2013,rst cause:2, boot mode:(3,6)
        ...


Right now it's useful just for checking syntax and seeing if the esp is
alive / communicating.  You could extend so it can run scripts.
