#include <string.h>
#include "libunix.h"
#include "interpose.h"

// they should wrap up in case it crashes: also for cleanup.
// actually: we do not need logging files if we do this.  maybe keep w/ it?
// what's the right way to compare the output?  we could have them fork and
// grab from the file descriptor.
// maybe keep it simple stupid: this is a crash application. 
// then have the makefile run the success after you do n crashes.
// actually: i think this is the simplest way, right?  crash the pi or unix side.
//  1. make sure the pi and unix side give errors.
//  2. rerun nix and check.
//
// it's nicer for the user if you pull into 

static void usage(char *name, const char *msg) {
    output("usage: %s: %s [-lo <int>] [-hi <int> [trace file]", msg,name);
    exit(1);
}


// get my-install output for a clean run
int do_clean_run(const char *outfile,
            const char *install, const char *dev, const char *pi_prog) {

    const char *filter = "2>&1 > /dev/null | grep -v /dev";
    char cmd[1024];
    sprintf(cmd, "%s %s %s %s > %s", install, dev, pi_prog, filter, outfile);

    if(!run_system_err_ok(1, "%s", cmd))
        panic("my-install failed during a clean run\n");

    return 1;
}

// have a verbose mode where you pull out the output.
// how can they speed stuff up?   mess w the timeout?
int main(int argc, char *argv[]) {

    unsigned need_args = 3;

    unsigned lo=0,hi=60;
    int i;
    for(i = 1; i < argc; i++) {
        if(argv[i][0] != '-')
            break;
        if(strcmp(argv[i], "-lo") == 0) {
            if(++i == argc-need_args)
                usage(argv[0], "-lo needs an integer");
            lo = atoi(argv[i]);
        } else if(strcmp(argv[i], "-hi") == 0) {
            if(++i == argc-need_args)
                usage(argv[0], "-hi needs an integer");
            hi = atoi(argv[i]);
        }
    }
    if(argc != i + need_args)
        usage(argv[0], "missing non-optional arguments");
    if(lo > hi)
        usage(argv[0], "lo is > hi");

    char *install = argv[i+0];
    char *dev_name = argv[i+1];
    char *pi_prog = argv[i+2];

    output("-------------------------------------------------------------------\n");
    trace_output("going to forward: %s %s %s>\n", install, dev_name, pi_prog);

    // why do we do two runs?
    unsigned nbytes = clean_run(install, dev_name, pi_prog);
    do_clean_run("/tmp/ref.out", install, dev_name, pi_prog);

    if(hi > nbytes)
        hi = nbytes;

    output("unix side sent %d bytes: timeout for %d to %d\n", nbytes, lo,hi);
    // if corrupt code, doesn't protect.
    // if corrupt n, doesn't really work well.

    // now go through and corrupt different bytes sent from unix to the pi.
    //for(unsigned i = 8; i < 3000; i++) {
    // for(unsigned i = 60; i < 600; i++) {
    for(unsigned i = lo; i < hi; i++) {
        output("-------------------------------------------------------------------\n");
        output("INTERPOSE: about to timeout byte: %d\n", i);

        // the first byte should be fine?
        timeout_run(install, dev_name, pi_prog, i);

      //  sleep(1);

        output("-------------------------------------------------------------------\n");
        output("about to do a clean run: should succeed\n");
        do_clean_run("/tmp/test.out", install, dev_name, pi_prog);
        if(!run_system_err_ok(1, "diff /tmp/ref.out /tmp/test.out"))
            panic("%s failed after a timeout on byte %d\n", install, i);
    }

    return 0;
}
