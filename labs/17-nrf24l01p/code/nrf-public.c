// public interface for the nrf driver.
// we do the gross thing of having a global variable.

#include "nrf-internal.h"
#include "timeout.h"

#if 0
// we hard-code that there is one nrf for the whole system.  
// it's better to support multiple --- for such usage you'd
// need interrupts, so you'd have a way to associate each with
// the GPIO interrupt it was supposed to get.
static nrf_t *nic;
#endif

nrf_t *nrf_get_nic(nrf_pipe_t *p) {
    return p->nic;
}

nrf_t *nrf_init_noacked(nrf_config_t c, uint32_t rx_addr, unsigned msg_nbytes) {
    assert(c.addr_nbytes);
    assert(msg_nbytes > 0);
    return staff_nrf_init(c,rx_addr,msg_nbytes,0);
}
nrf_t *nrf_init_acked(nrf_config_t c, uint32_t rx_addr, unsigned msg_nbytes) {
    assert(c.addr_nbytes);
    assert(msg_nbytes > 0);
    return staff_nrf_init(c,rx_addr,msg_nbytes,1);
}

// returns the number of bytes available.
// pulls data off if it's on the interface (since there can't be that much
// and we'd start dropping stuff).
int nrf_pipe_nbytes_avail(nrf_pipe_t *p) {
    if(staff_nrf_get_pkts(nrf_get_nic(p))) 
        assert(cq_nelem(&p->recvq));
    return cq_nelem(&p->recvq);
}

// non-blocking: if there is less than <nbytes> of data, return 0 immediately.
//    otherwise read <nbytes> of data into <msg> and return <nbytes>.
int nrf_get_data_exact_noblk(nrf_pipe_t *p, void *msg, unsigned nbytes) {
    assert(nbytes > 0);

    // explicitly, obviously return 0 if n == 0.
    unsigned n;
    if(!(n = nrf_pipe_nbytes_avail(p)))
        return 0;
    if(n < nbytes)
        return 0;

    // fix this stupid circular queue with all the extra volatiles.
    cq_pop_n(&p->recvq, msg, nbytes);
    return nbytes;
}

int nrf_get_data_exact_timeout(nrf_pipe_t *p, void *msg, unsigned nbytes, 
    unsigned usec_timeout) {
    timeout_t t = timeout_start();
    int n;
    while(!(n = nrf_get_data_exact_noblk(p, msg, nbytes))) {
        // put a yield in here while busywaiting: if there are threads
        // would work.

        // if we are running a thread, would switch

        if(timeout_usec(&t, usec_timeout))
            return -1;
        rpi_wait();
    }
    return n;
}

// wait: this is not packet level --- should it be?
// should we preserve packet boundaries?  maybe?  otherwise
// stuff can get intermixed.  idk.  alternatively, 
// you'll have extra padding and stuff in the queue.
// hmmm.  XXX revisit.  maybe a circular queue of 
// message structures is the way to go?  alot of copies.
// no idea if it really matters.

// it'd be really nice if C could return tuples.
// should the message hold the maxbytes before?
int nrf_get_msg_exact_timeout(nrf_pipe_t *p, 
    msg_recv_t *out, 
    unsigned nbytes, 
    unsigned usec_timeout) {

    assert(nrf_legal_msg_nbytes(nbytes));

    *out = msg_recv_mk_empty(nbytes);
    return nrf_get_data_exact_timeout(p, 
            msg_recv_ptr(out), nbytes, usec_timeout);
}

enum { NRF_TIMEOUT = 10};

// blocking: read exactly <nbytes> of data.
// we have it as an int in case we need to return < 0 errors.
int nrf_get_data_exact(nrf_pipe_t *p, void *msg, unsigned nbytes) {
    while(1) {
        unsigned usec = NRF_TIMEOUT * 1000 * 1000;
        int n = nrf_get_data_exact_timeout(p, msg, nbytes, usec);
        if(n == nbytes) 
            return n;

        if(n < 0) {
            debug("addr=%x: connection error: no traffic after %d seconds\n",
                    p->rxaddr,  NRF_TIMEOUT);
            // nrf_dump("timeout config\n");
            panic("fix nrf_dump\n");
        }
        assert(n< nbytes);
    }
}

// if the message is not big enough, what should we do?  <msg> is full of 
// zeros, so i guess you can just bump the size up?
int nrf_send_ack(nrf_t *nic, uint32_t txaddr, const void *msg, unsigned nbytes) {
    return staff_nrf_tx_send_ack(nic, txaddr, msg, nbytes);
}
int nrf_send_noack(nrf_t *nic, uint32_t txaddr, const void *msg, unsigned nbytes) {
    return staff_nrf_tx_send_noack(nic, txaddr, msg, nbytes);
}
