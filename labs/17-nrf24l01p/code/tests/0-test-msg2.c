// test the msg2.h implementation.
//
// crazy that the test code is larger than the implementation.  and we didn't
// verify!
#include "rpi.h"
#include "msg2.h"

static uint32_t data_val(const void *data, unsigned size) {
    uint32_t x = 0;
    memcpy(&x, data, size);
    return x;
}

static const void *data_ptr(const void *data, unsigned off) {
    return (const char *)data + off;
}

static inline void msg_send_cmp(const char *msg, const msg_send_t *m1, const msg_send_t *m2) {
#if 0
    unsigned g1 = m1->get_off;
    unsigned g2 = m2->get_off;
    if(g1 != g2)
        output("m1->get_off %d != %d m2->get_off\n", g1,g2);
#endif

    unsigned p1 = m1->put_off;
    unsigned p2 = m2->put_off;
    if(p1 != p2)
        panic("m1->put_off %d != %d m2->put_off\n", p1,p2);

    assert(m1->maxbytes == m2->maxbytes);

    for(unsigned i = 0; i < m1->maxbytes; i++) {
        unsigned d1 = m1->msg[i];
        unsigned d2 = m2->msg[i];
        if(d1 != d2)
            panic("data1[%d] %d != %d data2[%d]\n", i, d1,d2,i);
    }
}
static void test_put_rand(const msg_send_t *m_ref, const void *data, unsigned msg_nbytes) {
    msg_send_t m = msg_send_mk(msg_nbytes);

    const char *p = data;
    int n = 0; 

    unsigned left = msg_nbytes;
    for(unsigned off = 0; off < msg_nbytes; ) { 
        assert(off + left == msg_nbytes);

        unsigned b = rpi_rand16() % 8 + 1;
        if(b > left)
            b = left;
        switch(b) {
        case 1: msg_put8(&m, data_val(p, 1)); break;
        case 2: msg_put16(&m, data_val(p, 2)); break;
        case 4: msg_put32(&m, data_val(p, 4)); break;
        default: msg_put_n(&m, p, b); break;
        }
        p += b;
        off += b;
        left -= b;
        assert(off + left == msg_nbytes);
    } 
    assert(n == 0);
    
    if(memcmp(m_ref, &m, sizeof m) != 0) {
        msg_send_dump("m_ref", m_ref);
        msg_send_dump("m_rand", &m);
        msg_send_cmp("m_ref == m_rand", m_ref, &m);
        panic("m_ref different from m_rand\n");
    }
}

// array ref
static uint32_t data_get(const void *data, unsigned off, unsigned size) {
    off *= size;
    data = (const char *)data+off;
    return data_val(data, size);
}

void notmain(void) { 
    unsigned msg_nbytes = 32;

    // check that putting in different ways works out.
    char data[NRF_MAXPKT];
    for(unsigned i = 0; i < msg_nbytes; i++)
        data[i] = rpi_rand16();

    for(unsigned i = 0; i < msg_nbytes; i++)
        assert(data[i] == data_get(data,i,1));

    msg_send_t m8 = msg_send_mk(msg_nbytes);
    for(unsigned i = 0; i < msg_nbytes; i++)
        msg_put8(&m8, data_get(data,i,1));

    msg_recv_t m = msg_send_to_recv(&m8);
    for(unsigned i = 0; i < msg_nbytes; i++)
        assert(msg_get8(&m) == data[i]);
    output("SUCCESS: get8 passed\n");

    msg_send_t m16 = msg_send_mk(msg_nbytes);
    for(unsigned i = 0; i < msg_nbytes/2; i++)
        msg_put16(&m16, data_get(data,i,2));

    if(memcmp(&m16,&m8, sizeof m16) != 0)
        panic("m16 and m8 differ!\n");
    output("SUCCESS: m8 == m16\n");

    m = msg_send_to_recv(&m16);
    for(unsigned i = 0; i < msg_nbytes/2; i++)
        assert(msg_get16(&m) == data_get(data,i,2));
    output("SUCCESS: get16 passed\n");


    msg_send_t m32 = msg_send_mk(msg_nbytes);
    for(unsigned i = 0; i < msg_nbytes/4; i++)
        msg_put32(&m32, data_get(data,i,4));
    if(memcmp(&m8,&m32, sizeof m8) != 0)
        panic("m32 and m8 differ!\n");
    output("SUCCESS: m8 == m32\n");

    m = msg_send_to_recv(&m32);
    for(unsigned i = 0; i < msg_nbytes/4; i++)
        assert(msg_get32(&m) == data_get(data,i,4));
    output("SUCCESS: get32 passed\n");

    unsigned n = 512;
    for(unsigned i = 0; i < n; i++)  {
        test_put_rand(&m8, data, msg_nbytes);
        // output("passed random check: %d\n", i);
    }
    output("passed %d random checks\n", n);

#if 0
    for(unsigned i = 0; i < msg_nbytes; i++) {
        unsigned x = msg_get8(&m, i);
        if(x != i)
            panic("got %d, expected %d\n", x,i);
    }
#endif
}
