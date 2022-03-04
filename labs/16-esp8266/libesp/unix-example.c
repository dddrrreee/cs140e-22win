
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>

#include "libesp.h"
#include "libunix.h"

int main(int argc, char *argv[]) {
    if(argc != 2)
        panic("incorrect number of arguments: %d expected 2\n", argc);
    char *name = argv[1];

    int fd = open(name, O_RDONLY);
    if(fd < 0)
        sys_die(open, "can't open: <%s>\n", name);
    printk("about to check: <%s>\n", name);

    lex_t l;
    lex_init(&l, fd);

    while(esp_has_data_timeout(&l, 1000)) {
        if(!match(&l, "esptest"))
            panic("did not match!\n");
        printk("matched esptest!\n");
    }
    return 0;
}

