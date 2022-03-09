// one way test of ack packets.
#include "rpi.h"
#include "nrf.h"
#include "nrf-test.h"
#include "rf-support.h"

// change these however you want.
#define NTRIAL 1000
#define TIMEOUT_USEC 1000
#define NBYTES 32

// should add a checksum to see if things are getting corrupted.
static inline msg_send_t mk_test_msg(uint32_t i, unsigned nbytes) {
    struct { uint32_t v; uint8_t pad[32-4]; } test_msg = { .v = i };
#if 0
    return msg_mk_pkt(nbytes, &test_msg, nbytes);
#else
    msg_send_t m = msg_send_mk(nbytes);
    msg_put_n(&m, &test_msg, nbytes);
    return m;

#endif
}

// returns number of lost packets
static void
one_way_noack(nrf_t *server_nic, nrf_t *client_nic, int verbose_p) {

    unsigned ntrial = NTRIAL;
    unsigned timeout = TIMEOUT_USEC;
    unsigned nbytes = NBYTES;

    nrf_pipe_t *client = nrf_get_pipe(client_nic, 0);
    unsigned client_addr = client->rxaddr;

    unsigned start = timer_get_usec();

    unsigned ntimeout = 0, npackets = 0;
    for(unsigned i = 0; i < ntrial; i++) {
        if(verbose_p && i  && i % 100 == 0)
            output("sent %d ack'd packets\n", i);
        nrf_send_msg_ack(server_nic, client_addr, mk_test_msg(i, nbytes));

        // receive from client nic
        msg_recv_t m;
        if(nrf_get_msg_exact_timeout(client, &m, nbytes, timeout) == nbytes) {
            assert(msg_nbytes_get(&m) == nbytes);
            unsigned x= msg_get32(&m);

            if(x != i)
                debug("client: received %d (expected=%d)\n", x,i);
            assert(x == i);
            npackets++;
        } else {
            if(verbose_p) 
                debug("receive failed for packet=%d\n", i);
            ntimeout++;
        }
    }
    output("trial: total successfully sent %d ack'd packets lost %d\n",
        npackets, ntimeout);

    assert((ntimeout + npackets) == ntrial);
}

void notmain(void) {
    unsigned nbytes = 32;
    int ack_p = 1;

    output("about to configure ack'd client\n");
    nrf_t *client_nic = client_config(nbytes, ack_p);
    output("about to configure ack'd server\n");
    nrf_t *server_nic = server_config(nbytes, ack_p);
    one_way_noack(server_nic, client_nic, 1);
}
