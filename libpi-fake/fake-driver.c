// we put main() in a seperate file so that the linker will ignore it
// if we link the libpi-fake.a library against something that has a main()

#include "fake-pi.h"

int main(int argc, char *argv[]) {
    fake_init();

    // extension: run in a subprocess to isolate
    // errors.
    output("calling pi code\n");
    notmain();
    output("pi exited cleanly\n");
    trace("%d calls to random\n", fake_random_calls());
    return 0;
}
