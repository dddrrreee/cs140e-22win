#ifndef __NRF_TEST_H__
#define __NRF_TEST_H__

#define MY_NRF_CHANNEL nrf_default_channel

// all the definitions we use the in the test code --- easier to have in a header so that
// it can be shared b/n client and server code by updating a single place.
#define NTEST 10
#define DELAY_MS 1
#define DELAY_USEC 1

// should effectively make reliable.
#define RETRAN_ATTEMPTS 15
#define RETRAN_DELAY 2500

#define NRF_GPIO_INT 21

// make these different from the defaults.
// 0xc2c2c2c2c2
// 0xe7e7e7e7e7;
enum {
    server_addr = 0xd5d5d5,
    client_addr = 0xe5e5e5,
};

#define test_output(iter, msg, args...)  \
    do { if((iter) % 100 == 0) output(msg, ##args); } while(0)



enum { retran = 2, retran_delay = 250 };

// not sure this is the best way, but.
static nrf_t *client_config(unsigned nbytes, int ack_p) {
    output("doing client addr=%x: nbytes=%d, ack=%d\n", 
            client_addr, nbytes, ack_p);
    nrf_config_t c = ack_p ?
            nrf_conf_reliable_mk(retran, retran_delay) :
            nrf_conf_unreliable_mk();
    c = nrf_conf_spi(c,1);
    c = nrf_conf_ce(c, 21);
    c = nrf_set_Mhz_delta(c,  MY_NRF_CHANNEL);

    return ack_p ?
        nrf_init_acked(c, client_addr, nbytes) :
        nrf_init_noacked(c, client_addr, nbytes);
}

static nrf_t *server_config(unsigned nbytes, int ack_p) {
    output("doing server addr=%x: nbytes=%d, ack=%d\n", 
            server_addr, nbytes, ack_p);
    nrf_config_t c = ack_p ?
            nrf_conf_reliable_mk(retran, retran_delay) :
            nrf_conf_unreliable_mk();
    c = nrf_conf_spi(c,0);
    c = nrf_conf_ce(c, 20);
    c = nrf_set_Mhz_delta(c,  MY_NRF_CHANNEL);
    return ack_p ?
        nrf_init_acked(c, server_addr, nbytes) :
        nrf_init_noacked(c, server_addr, nbytes);
}



#endif
