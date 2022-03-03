// simple minded "shell" for the ESP so you can type commands and see what happens.
// useful extension: 
//      read in a file of commands and run them.
#include <sys/stat.h>
#include <unistd.h>
#include <termios.h>
#include <ctype.h>

#include <string.h>
#include "libunix.h"

static void usage(const char *msg) {
    output("%s\n", msg);
    output("usage: zero or more of the following options:\n");
    output("    --first : attach to the oldest ttyUSB device\n");
    output("    --last: attach to the newest ttyUSB device\n");
    output("    --ack: wait for an ack on each ESP interaction\n");
    output("    --cmds <filename>: run the given commands first\n");
    output("    --verbose print out extra information\n");
    exit(1);
}

// read until we hit a newline.
static int readline(int fd, char *buf, unsigned n, unsigned usec_timeout, const char *esp_port) {
    if(!can_read_timeout(fd, usec_timeout)) 
        return 0;

    for(unsigned i = 0; i < n-1;) {
        if(read(fd, &buf[i], 1) <= 0) {
            // isn't an esp read
            if(!esp_port)
                sys_die(read, "read failed\n");
            // is an esp read
            else {
                // this isn't the program's fault.  so we exit(0).
                if(tty_gone(esp_port))
                    clean_exit("ESP ttyusb <%s> gone: connection closed.  cleaning up\n", esp_port);
                sys_die(read, "esp connection closed, port still there\n");
            }
        }

        // skip cr
        if(buf[i] == '\r') 
            continue;

        // when we hit a newline we are done.
        if(buf[i] == '\n') {
            buf[i] = 0;
            return i;
        }
        i++;
    }
    
    panic("line too long: max bytes=%d\n", n);
}

static int match_busy(const char *s) {
    return strstr(s, "busy s...") 
        || strstr(s, "busy p...") 
        || strstr(s, "BUSY")
        ;
}
static int match_ok(const char *s) {
    return strstr(s, "OK") || strstr(s, "ready");
}

// i think BUSY means you have to retry
static int match_error(const char *s) {
    return strstr(s, "ERROR") != 0;
}

typedef struct {
    int unix_fd;
    int esp_fd;
    const char *dev_name;

    unsigned wait_for_ack_p:1,
            check_cmd_echo_p:1,
            verbose_p:1;

    const char *cmds;
} esp_opt_t;

static inline esp_opt_t esp_opt_mk(void) {
    return (esp_opt_t) { };
}

// read and echo the characters from the usbtty until it closes 
// (pi rebooted) or we see a string indicating a clean shutdown.
static void esp_echo(esp_opt_t *o) {
    int unix_fd = o->unix_fd;
    int esp_fd = o->esp_fd;
    assert(esp_fd);

    while(1) {
        char cmd[4096];
        int saw_cmd_p = 0;

        // write user command
        int n = readline(unix_fd, cmd, sizeof cmd - 4, 1000, 0);
        if(n) {
            assert(n>0);
            write_exact(esp_fd, cmd, strlen(cmd));
            write_exact(esp_fd, "\r\n", 2);
            saw_cmd_p = 1;
        }
        
        char reply[4096];
        if(saw_cmd_p && o->check_cmd_echo_p) {
            output("about to get echo: n=%d\n", n);
            if(!(n = readline(esp_fd, reply, sizeof reply - 4, ~0, o->dev_name))) 
                panic("ESP timed out waiting for echo for <%s>\n", cmd);

            if(strcmp(cmd, reply) != 0)
                panic("should have seen an echo of <%s>: got <%s>\n", cmd, reply);
            else
                output("%s: got echo\n", cmd);
        }

        // we need to get some kind of reply.
        unsigned timeout = 1000;

        while(1) { 
            if(!(n = readline(esp_fd, reply, sizeof reply - 4, timeout, o->dev_name))) {
                if(!saw_cmd_p || !o->wait_for_ack_p)
                    break;
            } else {
                reply[n] = 0;
                remove_nonprint((unsigned char*)reply,n);
                output("%s", reply);

                char *done_msg = 0;
                if(match_busy(reply))
                    done_msg = "signaled busy: i think you have to retry";
                else if(match_ok(reply))
                    done_msg = "gave an OK for cmd=<%s>!";
                else if(match_error(reply)) 
                    done_msg = "gave an ERROR for cmd <%s>!";
                else
                    output("\n");
        
                if(done_msg) {
                    if(o->verbose_p) {
                        output("\t# ESP ");
                        output(done_msg, cmd);
                    }
                    output("\n");
                    break;
                }
            }
        }
    }
    notreached();
}

int main(int argc, char *argv[]) {
    esp_opt_t opts = esp_opt_mk();

    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "--first") == 0)
            opts.dev_name = find_ttyusb_first();
        else if(strcmp(argv[i], "--last") == 0)
            opts.dev_name = find_ttyusb_last();
        else if(strcmp(argv[i], "--ack") == 0)
            opts.wait_for_ack_p = 1;
        else if(strcmp(argv[i], "--verbose") == 0)
            opts.verbose_p = 1;
        else if(strcmp(argv[i], "--cmds") == 0) {
            char *cmd_file = argv[++i];
            if(!cmd_file)
                usage("--cmds with no file\n");

            unsigned n;
            opts.cmds = read_file(&n, cmd_file);
            if(!n)
                panic("file has %d nbytes\n", n);
        } else 
            panic("unknown option <%s>\n", argv[i]);
    }

    if(!opts.dev_name)
        opts.dev_name = find_ttyusb();

    output("opening: <%s>\n", opts.dev_name);
    opts.esp_fd = set_tty_to_8n1(open_tty(opts.dev_name), B115200, 1);

    if(opts.cmds)
        panic("not handling command file\n");
    output("about to communicate with <%s>\n", opts.dev_name);
    esp_echo(&opts);
    return 0;
}
