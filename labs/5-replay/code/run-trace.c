// simple forwarding driver that (should) transparently
// sit between the unix and pi sides and forward bytes
// between them.
//      run-trace <my-install> [device] <pi-program>
//
// there should be no behavioral difference  other than some 
// extra print statements (all prefixed with "INTERPOSE:")
#include <string.h>
#include "libunix.h"
#include "interpose.h"

int main(int argc, char *argv[]) {
    char *install = argv[1];

    char *dev_name = 0, *pi_prog = 0;
    if(argc == 3) {
        dev_name = find_ttyusb();
        pi_prog = argv[2];
    } else if(argc == 4) {
        dev_name = argv[2];
        pi_prog = argv[3];
    } else 
        die("need 2 or 3 arguments: have %d", argc);

    trace_output("going to forward: %s %s %s>\n", install, dev_name, pi_prog);
    int nbytes = clean_run(install, dev_name, pi_prog);
    if(nbytes <= 0)
        trace_err("tracing run failed\n");
    trace_ok("unix side sent %d bytes\n", nbytes);
    return 0;
}
