#ifndef __MSG2_H__
#define __MSG2_H__
// attempt to make a sort-of clean interface for messages
// a bit more stuff than we probably need, but wvs.

/******************************************************************
 * send msg methods.
 */

// maximum packat size on nrf is 32 bytes
#define NRF_MAXPKT 32
typedef struct {
    uint8_t msg[NRF_MAXPKT];
    uint32_t put_off,   // msg[put_off] = next byte to write.
             maxbytes;  // size of the packet (can be smaller than MAXPKT)
} msg_send_t;

// create a new empty msg.
static inline msg_send_t msg_send_mk(unsigned maxbytes) {
    // <msg> should be zeroed after this.
    return (msg_send_t) { .put_off = 0, .maxbytes = maxbytes };
}

// should probably have more checking that they just put or just get.
static inline int msg_wrote_all(msg_send_t *m) {
    return m->maxbytes == m->put_off;
}

// idk if this is the best way to go.  extra stuff.
// number of bytes available to send.
static inline unsigned msg_send_nbytes(const msg_send_t *m) 
    { return m->put_off; }

// base pointer to send.  
static inline const void *msg_send_ptr(const msg_send_t *m) 
    { return m->msg; }

// get a pointer to the current offset, check that there is space to 
// put_off the value 
//
// panic's if not --- maybe should prop the error.
static inline void *m_put_ptr(msg_send_t *m, unsigned nbytes) {
    unsigned off = m->put_off;
    unsigned maxbytes = m->maxbytes;

    assert(off < maxbytes);

    unsigned n = maxbytes - off;
    if(n < nbytes)
        panic("not enough bytes left: have %d, need %d\n", n, nbytes);
    return &m->msg[off];
}

// get a pointer to the current offset, check that the 
// data is >= <nbytes>, increment by <nbytes>
static inline void *m_put_ptr_inc(msg_send_t *m, unsigned nbytes) {
    void *out = m_put_ptr(m,nbytes);
    m->put_off += nbytes;
    return out;
}

static inline void msg_put_n(msg_send_t *m, const void *src, unsigned nbytes) {
    void *dst = m_put_ptr_inc(m, nbytes);
    memcpy(dst, src, nbytes);
}
static inline void msg_put8(msg_send_t *m, uint8_t x) {
    msg_put_n(m,&x,sizeof x);
}
static inline void msg_put16(msg_send_t *m, uint16_t x) {
    msg_put_n(m,&x,sizeof x);
}
static inline void msg_put32(msg_send_t *m, uint32_t x) {
    msg_put_n(m,&x,sizeof x);
}
static inline void msg_put64(msg_send_t *m, uint64_t x) {
    msg_put_n(m,&x,sizeof x);
}
static inline void msg_put24(msg_send_t *m, uint32_t x) {
    assert((x >> 24) == 0);
    msg_put8(m,(x >> 0) &0xff);
    msg_put8(m,(x >> 8) &0xff);
    msg_put8(m,(x >> 16) &0xff);
}

static void msg_dump_data(const uint8_t *msg, unsigned nbytes) {
    output("\tdata = {");
    for(unsigned i = 0; i < nbytes; i++) {
        output("%x,", msg[i]);
        if(i && i % 16)
            output("\n\t\t");
    }
    output("\n");
}

static inline void msg_send_dump(const char *msg, const msg_send_t *m) {
    unsigned maxbytes = m->maxbytes;
    output("<%s>: \tput_off = %d maxbytes = %d\n", 
            msg, m->put_off, maxbytes);
    msg_dump_data(m->msg, maxbytes);
}

/******************************************************************
 * recv msg methods.
 */

// basically the same as msg_send_t but we have a different type for
// easier checking.
typedef struct {
    uint8_t msg[NRF_MAXPKT];
    uint32_t get_off,   // msg[put_off] = next byte to write.
             nbytes;  // size of the packet (can be smaller than MAXPKT)
} msg_recv_t;

static inline msg_recv_t msg_recv_mk_empty(unsigned nbytes) {
    return (msg_recv_t) { .nbytes = nbytes };
}

static inline msg_recv_t msg_recv_mk(const void *data, unsigned nbytes) {
    // zero everything but <nbytes>
    msg_recv_t m = (msg_recv_t) { .nbytes = nbytes };
    memcpy(m.msg, data, nbytes);
    return m;
}

// recast message
static msg_recv_t msg_send_to_recv(const msg_send_t *m) {
    return msg_recv_mk(m->msg, msg_send_nbytes(m));
}

// number of bytes left for a get
static inline int msg_nbytes_get(msg_recv_t *m) {
    assert(m->nbytes);
    assert(m->get_off <= m->nbytes);
    return m->nbytes - m->get_off;
}

static inline void *msg_recv_ptr(msg_recv_t *m) 
    { return m->msg; }
static inline uint32_t msg_recv_nbytes(msg_recv_t *m) 
    { return m->nbytes; }

// return base pointer and check that the number of expected bytes are 
// there.
static inline const void *msg_get_ptr(const msg_recv_t *m, unsigned expected_nbytes) {
    unsigned n = m->nbytes;
    if(n != expected_nbytes)
        panic("expected %d bytes, have %d\n", expected_nbytes, n);
    return m->msg;
}

static inline void *m_get_ptr(msg_recv_t *m, unsigned nbytes) {
    unsigned get_off = m->get_off;
    unsigned left = msg_nbytes_get(m);

    if(left < nbytes)
        panic("not enough bytes left: have %d, need %d\n", left, nbytes);
    return &m->msg[get_off];
}

static inline const void *m_get_ptr_inc(msg_recv_t *m, unsigned nbytes) {
    void *out = m_get_ptr(m,nbytes);
    m->get_off += nbytes;
    return out;
}

// copy out <nbytes> of data from <msg>: error if not enough there.
static inline void msg_get_n(void *out, msg_recv_t *m, unsigned nbytes) {
    memcpy(out, m_get_ptr_inc(m,nbytes), nbytes);
}
// copy out 32-bits of data, error if not enough in <m>
static inline uint32_t msg_get32(msg_recv_t *m) {
    uint32_t out;
    msg_get_n(&out, m, sizeof out);
    return out;
}
// copy out 16-bits of data: error if not enough in <m>
static inline uint16_t msg_get16(msg_recv_t *m) {
    uint16_t out;
    msg_get_n(&out, m, sizeof out);
    return out;
}
// copy out 8-bits of data, error if not enough in <m>
static inline uint8_t msg_get8(msg_recv_t *m) {
    uint8_t out;
    msg_get_n(&out, m, sizeof out);
    return out;
}

static inline void msg_peek_n(void *out, msg_recv_t *m, unsigned nbytes) {
    memcpy(out, m_get_ptr(m,nbytes), nbytes);
}
static inline uint8_t msg_peek8(msg_recv_t *m) {
    uint8_t out;
    msg_peek_n(&out, m, sizeof out);
    return out;
}


static inline void msg_recv_dump(const char *msg, const msg_recv_t *m) {
    unsigned nbytes = m->nbytes;
    output("<%s>: \tget_off = %d, nbytes = %d\n", 
            msg, m->get_off, nbytes);
    msg_dump_data(m->msg, nbytes);
}

// synthetic ops.
static inline uint32_t msg_get24(msg_recv_t *m) {
    uint32_t x = 0;
    x |= msg_get8(m) << 16;
    x |= msg_get8(m) << 8;
    x |= msg_get8(m) << 0;
    return x;
}

#endif
