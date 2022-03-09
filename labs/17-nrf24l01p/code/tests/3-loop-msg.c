// fragile one-way test to see that something is coming out.
#include "rpi.h"
#include "nrf.h"
#include "nrf-test.h"
#include "rf-support.h"

#if 0
// not sure this is the best way, but.
static nrf_t *client_config(unsigned nbytes, int ack_p) {
    output("doing client: nbytes=%d, ack=%d\n", nbytes, ack_p);
    nrf_config_t c = ack_p ? 
            nrf_conf_reliable_mk(retran, retran_delay) : 
            nrf_conf_unreliable_mk();
    c = nrf_conf_spi(c,1);
    c = nrf_conf_ce(c, 21);
    return ack_p ?
        nrf_init_acked(c, client_addr, nbytes) :
        nrf_init_noacked(c, client_addr, nbytes);
}

static nrf_t *server_config(unsigned nbytes, int ack_p) {
    output("doing server: nbytes=%d, ack=%d\n", nbytes, ack_p);
    nrf_config_t c = ack_p ? 
            nrf_conf_reliable_mk(retran, retran_delay) :
            nrf_conf_unreliable_mk();
    c = nrf_conf_spi(c,0);
    c = nrf_conf_ce(c, 20);
    return ack_p ? 
        nrf_init_acked(c, server_addr, nbytes) :
        nrf_init_noacked(c, server_addr, nbytes);
}
#endif

#if 0
// trial: should we add more information?  timeout size?
typedef struct {
    uint32_t tot_usec;   // how long it took;
    uint32_t nsent;
    uint32_t nlost;
} trial_t;

static inline trial_t
trial_mk(uint32_t tot_usec, uint32_t nsent, uint32_t nlost) {
    return (trial_t) { 
        .tot_usec   = tot_usec, 
        .nsent      = nsent,
        .nlost      = nlost,
    };
}

// should we include the sender and receiver?
typedef struct {
    const char *msg;        // some message about the trial.
    uint32_t ntrial;        // number of trials to run.
    uint32_t nbytes;        // message size
    uint32_t timeout_usec;  // timeout
    nrf_config_t config;    // the actual nrf configuration.
} trial_conf_t;

static inline trial_conf_t
trial_conf_mk(uint32_t ntrial, uint32_t nbytes, uint32_t timeout_usec, nrf_config_t c) {
        .ntrial     = ntrial,
        .nbytes     = nbytes,
        .timeout_usec = timeout_usec,
        .config = c,
};
#endif

// should add a checksum to see if things are getting corrupted.
static inline msg_send_t mk_test(uint32_t i, unsigned nbytes) {
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
static trial_t 
one_way_test(nrf_t *server_nic, nrf_t *client_nic, trial_conf_t c, int verbose_p, int ack_p) {
    unsigned ntrial = c.ntrial; 
    unsigned timeout = c.timeout_usec;
    unsigned nbytes = c.nbytes;

    nrf_pipe_t *client = nrf_get_pipe(client_nic, 0);
    unsigned client_addr = client->rxaddr;

    unsigned start = timer_get_usec();

    unsigned ntimeout = 0, npackets = 0;
    for(unsigned i = 0; i < ntrial; i++) {

        // send from server nic to client nic
        // add these routines as a VFS to the nic struct?
        if(ack_p)
            nrf_send_msg_ack(server_nic, client_addr, mk_test(i, nbytes));
        else {
            nrf_send_msg_noack(server_nic, client_addr, mk_test(i, nbytes));
#if 0
            // jumps frm 76 to 94 if we don't do the receive.
            npackets++;
            continue;
#endif
        }

        // receive from client nic
        msg_recv_t m;
        if(nrf_get_msg_exact_timeout(client, &m, nbytes, timeout) == nbytes) {
            assert(msg_nbytes_get(&m) == nbytes);
            unsigned x= msg_get32(&m);
            nrf_debug("client: received %d (expected=%d)\n", x,i);
            assert(x == i);
            npackets++;
        } else {
            if(verbose_p) 
                debug("receive failed for packet=%d\n", i);
            ntimeout++;
        }
    }
    unsigned tot_usec = timer_get_usec() - start;

    trial_t t = trial_mk(tot_usec, ntrial, ntimeout,nbytes);
    if(verbose_p)
        trial_print(c.msg, t);

    assert((ntimeout + npackets) == ntrial);
    return t;
}

void notmain(void) {
    enable_cache();
    unsigned nbytes = 32;
    unsigned ack_p = 0;
    //nrf_t *server_nic = server_config(nbytes, ack_p);

    output("about to configure client\n");
    nrf_t *client_nic = client_config(nbytes, ack_p);
    output("about to configure server\n");
    nrf_t *server_nic = server_config(nbytes, ack_p);

    unsigned ntrial = 1000;

    // for loopback, there's no real timeout needed; huh.  i thought at least 1
    // usec or something.
    unsigned timeout = 10;
    trial_t tsum = trial_mk(0, 0, 0, nbytes);

    for(unsigned n = 0; n < 10; n++) {
        trial_conf_t c = trial_conf_mk("sending server->client", ntrial, nbytes, timeout);
        trial_t t0 = one_way_test(server_nic, client_nic, c, 1, ack_p);

        tsum = trial_add(tsum, t0);

        c = trial_conf_mk("sending client->server", ntrial, nbytes, timeout);
        trial_t t1 = one_way_test(client_nic, server_nic, c, 1, ack_p);

        tsum = trial_add(tsum, t1);
    }
    output("\nDone with nbytes = %d\n", nbytes);
    trial_print("summary:", tsum);
}

#if 0 
        estimate_t e = est_mk(timeout, 0);
        timeout = est_get(&e);
        while(1) { 
            // increase_p = did we lose anything?
            unsigned tot_lost = t0.nlost + t1.nlost;
            if(!est_bsearch(&e, tot_lost)) {
                debug("done: timeout=%d is <= to hi=%d and larger than lo=%d\n", 
                        e.cur, e.hi, e.lo);
                break;
            }
            debug("timeout going from %d to %d\n", timeout, e.cur);
            timeout = e.cur;
#endif

#if 0
    while(1) { 
        trial_conf_t c = trial_conf_mk("sending server->client", ntrial, 4, timeout);
        trial_t t0 = one_way_test(server_nic, client_nic, c, 1);

        c = trial_conf_mk("sending client->server", ntrial, 4, timeout);
        trial_t t1 = one_way_test(client_nic, server_nic, c, 1);

        // we didn't lose much --- halve it and see what happens.
        unsigned tot_lost = t0.nlost + t1.nlost;
        if(t0.nlost + t1.nlost < 2) {
            output("good: timeout=%d: total lost = %d\n", timeout, tot_lost);

            hi = timeout;   // a high value that worked.
            unsigned t = (hi + lo) / 2;
            if(t <= lo)
                panic("done: new timeout=%d would go below a broken timeout = %d\n", t, lo);
            debug("\tdecreasing from %d to %d\n", timeout, t);
            assert(t < timeout); 

            timeout = t;    // our new timeout.
        // we lost too much bump it back.
        } else {
            output("bad: timeout %d: total lost = %d\n", timeout, tot_lost);

            // this should increase us back.
            lo = timeout;
            unsigned t = (hi + lo) / 2;
            if(t >= hi)
                panic("done: increasing timeout to =%d gives us the previous hi=%d\n", t, hi);
            debug("increasing from %d to %d\n", timeout, t);
            assert(t > timeout);
        }
    }
}
#endif
