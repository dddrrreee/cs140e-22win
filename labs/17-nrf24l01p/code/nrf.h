#ifndef __NRF_H__
#define __NRF_H__
// public NRF interface.
//  - see nrf-internal.h for a lot of helper routines.

// defines a bunch of helpers.
#include "nrf-config.h"
#include "msg2.h"

#define NRF_MAX_PKT_BYTES 32

#define spi_transfer spi_transfer_undef
#define spi_init spi_init_undef

// per pipe config: for lab, we just have one pipe.
typedef struct nrf_pipe {
    struct nrf      *nic;
    uint64_t        rxaddr;       // prx address for the given pipe: we fix at 4bytes.
    uint8_t         msg_nbytes;     // fixed size message: 1..32 bytes

    unsigned        is_retran_p:1;  // do we retran this pipe?
    cq_t            recvq;          // received message queue.

    // total bytes we've received on this pipe.
    unsigned tot_recv_bytes;
    unsigned tot_msgs;
} nrf_pipe_t;

static inline int nrf_pipe_active(const nrf_pipe_t *p) {
    if(!p->rxaddr) return 0;
    if(!p->msg_nbytes) return 0;
    return 1;
}
static inline unsigned nrf_pipe_pkt_size(const nrf_pipe_t *p) {
    assert(nrf_pipe_active(p));
    return p->msg_nbytes;
}
static inline unsigned nrf_pipe_nbytes(const nrf_pipe_t *p) {
    return nrf_pipe_pkt_size(p);
}
static inline unsigned nrf_pipe_acked(const nrf_pipe_t *p) {
    assert(nrf_pipe_active(p));
    return p->is_retran_p;
}
static inline unsigned nrf_pipe_rxaddr(const nrf_pipe_t *p) {
    assert(nrf_pipe_active(p));
    return p->rxaddr;
}

static inline nrf_pipe_t 
nrf_pipe_mk(uint32_t rxaddr, unsigned msg_nbytes, unsigned acked_p) {
    nrf_pipe_t p = (nrf_pipe_t) {
        .rxaddr = rxaddr,
        .msg_nbytes = msg_nbytes,
        .is_retran_p = acked_p
    };
    cq_init(&p.recvq, 1);
    return p;
}

typedef struct nrf {
    uint8_t enabled_p;  // used to sanity check you've set up the nic.

    // these are assigned right into the hardware register.
    uint8_t rx_config;  // pre-computed value we can write into NRF_CONFIG
    uint8_t tx_config;  // pre-computed value we can write into NRF_CONFIG

    uint32_t last_txaddr;   // speed hack: if same as last time, don't set.

    nrf_config_t    c;      // global NRF configuration.
    nrf_pipe_t      pipe;   // for lab, we just have a single pipe.

    // stats to keep track of stuff.
    unsigned tot_sent_msgs;
    unsigned tot_sent_bytes;
    unsigned tot_retrans;
    unsigned tot_lost;
} nrf_t;


// step 0: create a simple one channel connection with <rx_addr>.
nrf_t *nrf_init_acked(nrf_config_t c, uint32_t rx_addr, unsigned msg_nbytes);
nrf_t *nrf_init_noacked(nrf_config_t c, uint32_t rx_addr, unsigned msg_nbytes);
nrf_t *nrf_init_acked(nrf_config_t c, uint32_t rx_addr, unsigned msg_nbytes);
nrf_t *nrf_init_noacked(nrf_config_t c, uint32_t rx_addr, unsigned msg_nbytes);

static inline nrf_pipe_t *nrf_get_pipe(nrf_t *nic, unsigned pipeno) {
    assert(pipeno == 0);
    return &nic->pipe;
}

// print out the NRF configuration by reading it from the hardware.
void nrf_dump(const char *msg, nrf_t *n);

// maximum sized packet
typedef struct {
    uint32_t data[8];
} nrf_pkt_t;
static inline nrf_pkt_t nrf_pkt_mk(void) { return (nrf_pkt_t){}; }
_Static_assert(sizeof(nrf_pkt_t) == 32, "invalid packet size");

// send to an acknowledging pipe
int nrf_send_ack(nrf_t *nic, uint32_t txaddr, const void *msg, unsigned nbytes);

static inline int 
nrf_send_msg_ack(nrf_t *nic, uint32_t txaddr, msg_send_t m) {
    return nrf_send_ack(nic, txaddr, msg_send_ptr(&m), msg_send_nbytes(&m));
}

// send to a non-acknowledging pipe
int nrf_send_noack(nrf_t *nic, uint32_t txaddr, const void *msg, unsigned nbytes);
static inline int 
nrf_send_msg_noack(nrf_t *nic, uint32_t txaddr, msg_send_t m) {
    return nrf_send_noack(nic, txaddr, msg_send_ptr(&m), msg_send_nbytes(&m));
}

// returns -1 on timeout.
int nrf_get_data_exact_timeout(nrf_pipe_t *p, void *msg, unsigned nbytes,
    unsigned usec_timeout);
int nrf_get_msg_exact_timeout(nrf_pipe_t *p, msg_recv_t *out, unsigned nbytes,
    unsigned usec_timeout);

// get exactly <nbytes> of data.
int nrf_get_data_exact(nrf_pipe_t *p, void *msg, unsigned nbytes);
int nrf_get_data_exact_noblk(nrf_pipe_t *p, void *msg, unsigned nbytes);

nrf_t *nrf_get_nic(nrf_pipe_t *p);

void nrf_dump_raw(const char *msg, nrf_t *n);
void nrf_set_verbose(int v_p);


extern unsigned nrf_verbose_p;
#define nrf_debug(args...) do { if(nrf_verbose_p) output(args); } while(0)
#define nrf_output(args...) do { output("NRF:"); output(args); } while(0)

//#define NDEBUG
#ifdef NDEBUG
#   define nrf_assert(_nic, bool) do { } while(0)
#else
#   define nrf_assert(_nic, bool) do {                    \
        if((bool) == 0) {                       \
            nrf_dump("dying with a panic\n", _nic);   \
            panic("%s", #bool);                 \
        }                                       \
    } while(0)
#endif

// change db from what is in the config.
void nrf_redo_db(nrf_t *nic, nrf_db_t db);
// change the datarate.
void nrf_redo_dr(nrf_t *nic, nrf_datarate_t dr);

// current effort at a transmit connection.
typedef struct {
    nrf_t *nic;
    nrf_pipe_t *p;

    // opaque list: if the connection needs a nonce table.
    // struct Q_t *nonce_Q;

    uint32_t txaddr;
    uint32_t rxaddr;
    unsigned pkt_size;

    unsigned retran_attempts;
    unsigned timeout_usec;

    // doesn't have to have a reply back.
} nrf_conn_t;

// current effort at a transmit connection.
typedef struct {
    nrf_t *nic;         // nic to do the send on.
    uint32_t txaddr;    // addresss to send to
    unsigned nbytes;    // expected size of packets.
} nrf_tx_t;
static inline nrf_tx_t nrf_tx_mk(nrf_t *nic, uint32_t txaddr, unsigned nbytes) {
    return (nrf_tx_t){ .nic = nic, .txaddr = txaddr, .nbytes = nbytes };
}

enum { NRF_CONN_RETRAN = 4, NRF_TIMEOUT_USEC = 1000 };

// currently: there always has to be a return pipe  will need to modify for
// broadcast.
// do we have the replay pipe?
static inline nrf_conn_t nrf_conn_mk(nrf_pipe_t *p, uint32_t txaddr) {
    return (nrf_conn_t){
        .nic = nrf_get_nic(p),
        .p = p,
        .txaddr = txaddr,
        .rxaddr = nrf_pipe_rxaddr(p),
        // right now we force pkt_size to be the same as the pipe's pktsize.
        .pkt_size = nrf_pipe_pkt_size(p),

        // not sure if it makes sense to have here -- should it be with the 
        // protocol?
        .retran_attempts = NRF_CONN_RETRAN,
        .timeout_usec = NRF_TIMEOUT_USEC,
    };
}

static inline nrf_conn_t 
nrf_conn_retran_set(nrf_conn_t c, unsigned retran, unsigned timeout_usec) {
    c.retran_attempts = retran;
    c.timeout_usec = timeout_usec;
    return c;
}


#endif
