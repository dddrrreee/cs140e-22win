#ifndef __INTERPOSE_INTERNAL_H__
#define __INTERPOSE_INTERNAL_H__
// internal starter code for replay lab.

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

#include "libunix.h"
#include "simple-boot-defs.h"

#ifdef __APPLE__
#include <util.h>
#else
#include <pty.h>
#endif

#define trace_output(args...) output("INTERPOSE:" args)
#define trace_err(args...) die("INTERPOSE:ERROR:" args)
#define trace_ok(args...) output("INTERPOSE:SUCCESS:" args)

// use union?   ugh.   want to run replay with both ends pulling from a trace.
typedef struct endpoint {
    const char *name;
    const char *dev_name;
    int type;
    int fd;
    int fd2;        // gross: need to close this on exit.
    int pid;        // 0 if is a device.
    unsigned sent_nbytes;

    unsigned corrupt_p;
    unsigned corrupt_byte;

    
    unsigned state;
    uint32_t last_u32;      // last u32 that was read


    int log_fd;
    // returns:
    //  : 1 if exited as expected.
    //  : 0 if exited not as expected
    //  : -1 if didn't exit yet.
    int (*exit_check_fn)(struct endpoint *e);
} endpt_t;

#if 0
e.
enum {
    ARMBASE=0x8000, // where program gets linked.  we could send this.

    // the weird numbers are to try to help with debugging
    // when you drop a byte, flip them, corrupt one, etc.
    BOOT_START      = 0xFFFF0000,

    GET_PROG_INFO   = 0x11112222,       // pi sends
    PUT_PROG_INFO   = 0x33334444,       // unix sends

    GET_CODE        = 0x55556666,       // pi sends
    PUT_CODE        = 0x77778888,       // unix sends

    BOOT_SUCCESS    = 0x9999AAAA,       // pi sends on success
    BOOT_ERROR      = 0xBBBBCCCC,       // pi sends on failure.

    PRINT_STRING    = 0xDDDDEEEE,       // pi sends to print a string.

    // error codes from the pi to unix
    BAD_CODE_ADDR   = 0xdeadbeef,
    BAD_CODE_CKSUM  = 0xfeedface,
};

#endif
#include "simple-boot-defs.h"

static inline endpt_t 
endpt_mk_dev(const char *name, const char *dev_name, int fd) {
    assert(fd);
    return (endpt_t) {
        .state = GET_PROG_INFO,
        .type = 'p',
        .name = name,
        .dev_name = dev_name,
        .fd = fd
    };
}


static inline endpt_t 
endpt_mk_unix(const char *name, const char *dev_name, int fd, int fd2, int pid) {
    assert(fd);
    assert(fd2);
    return (endpt_t) {
        .type = 'u',
        .name = name,
        .dev_name = dev_name,
        .fd = fd,
        .fd2 = fd2,
        .pid = pid,
    };
}

// all we do for cleanup is close fds.   perhaps check if the process is
// still around?
#include <signal.h>
static inline void endpt_destroy(endpt_t *e) {
    assert(e->fd);
    close_nofail(e->fd);
    if(e->fd2)
        close_nofail(e->fd2);
    
    if(e->pid)
        kill(e->pid, SIGKILL);
}

static int exec_server(char **argv);

static inline endpt_t start_unix_side(char *install, char *pi_prog) {

    int master_fd, slave_fd; char ptty_name[1024];
    int ret = openpty(&master_fd, &slave_fd, ptty_name, 0, 0);
    if(ret < 0)
        sys_die(openpty, "openpty failed!\n");
    trace_output("using <%s> for install <%s>\n", ptty_name, install);
    assert(master_fd);
    assert(slave_fd);

    // ah: if we don't do anything, the installer
    // is trying to open the ptty (which it does) and
    // then goes until it times out.
    char *child_argv[4] = { install, ptty_name, pi_prog, 0 };

    // i think if not verbose: remap stdout and stderr to /dev/null?
    int pid = exec_server(child_argv);
    trace_output("<%s> pid = %d\n", install, pid);

    trace_output("about to forward\n");
    return endpt_mk_unix("my-install", ptty_name, master_fd, slave_fd, pid);
}

// not much: we just open it.
endpt_t start_pi_side(const char *dev_name) {
    assert(dev_name);
    int fd = open_tty(dev_name);
    if(fd < 0)
        panic("could not open tty: <%s>\n", dev_name);
    fd = set_tty_to_8n1(fd, B115200, 1);
    return endpt_mk_dev("pi", dev_name, fd);
}


#include "interpose.h"

#endif 
