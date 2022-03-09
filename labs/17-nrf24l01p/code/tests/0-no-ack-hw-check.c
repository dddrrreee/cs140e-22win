// simple configure hardware for a single pipe, no ack
#include "rpi.h"
#include "nrf.h"
#include "nrf-test.h"

void notmain(void) {
    unsigned nbytes = 32;
    unsigned ack_p = 0;

    output("configuring unreliable server with %d nbyte messages\n", nbytes);
    // definition in nrf-test.h
    nrf_t *server_nic = server_config(nbytes, ack_p);
    nrf_dump("unreliable server config:\n", server_nic);

    output("configuring unreliable client with %d nbyte messages\n", nbytes);
    // definition in nrf-test.h
    nrf_t *client_nic = client_config(nbytes, ack_p);
    nrf_dump("unreliable client config:\n", client_nic);
}
