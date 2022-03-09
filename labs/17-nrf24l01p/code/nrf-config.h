#ifndef __NRF_CONFIG_H__
#define __NRF_CONFIG_H__
/**************************************************************************
 * different enums and sanity checking for each nrf register.
 *
 * channel options:
 *  - signal power  (nrf_db_t)
 *  - data rate     (nrf_datarate_t)
 *  - the frequency (in units of Mhz added to a baseline of 2.4GHz)
 *  - whether it is reliable (retrans, and delay)
 *
 * four parts:
 *   1. the different enums and checking routines
 *   2. the config structure.
 *   3. the setter methods.
 *   4. default settings.
 */
#include "libc/circular.h"
#include "spi.h"

/***********************************************************************
 * part 1: enums and checking routines.
 */

// reg 0x3: RF_SETUP (p 58): bandwidth.
// assuming i'm reading the datasheet correctly:
//      - RF_DR_HI is bit 3.
//      - RF_DR_LO is bit 5 (yes, i know, confusing).
// description on p 25, 6.2
// you can just or this into the RF_SETUP.
//
// XXX: not sure we should do the encoding here since we 
// don't for the others, but wvs.
#define RF_DR_HI(x) ((x) << 3)
#define RF_DR_LO(x) ((x) << 5)
// clear both bits.
#define RF_DR_CLR(x) ((x) & ~(RF_DR_HI(1) | RF_DR_LO(1)))
typedef enum {
    nrf_1Mbps   = 0,            // both RF_DR_HI=0 and RF_DR_LO=0.
    nrf_2Mbps   = RF_DR_HI(1),  // RF_DR_LO=0, RF_DR_HI=1.
    nrf_250kbps = RF_DR_LO(1),  // RF_DR_LO=1, RF_DR_HI=0.
} nrf_datarate_t;
static inline int nrf_legal_datarate(nrf_datarate_t dr) {
    return dr == nrf_1Mbps || dr == nrf_2Mbps || dr == nrf_250kbps;
}

// reg: 0x3, SETUP_AW (p58) addresss sizes (3,4,5 bytes).
// common for all pipes. 
//
// for the moment, just force all pipes to be 4 bytes?  easiest.
typedef uint64_t nrf_addr_t;

static inline int nrf_is_legal_addr_sz(unsigned nbytes) {
    return nbytes >= 3 && nbytes <= 5;
}
static int nrf_is_legal_addr(uint64_t addr, unsigned nbytes) {
    if(!nrf_is_legal_addr_sz(nbytes))
        panic("illegal address size: %d nbytes\n", nbytes);
    // check that addr does not have more values than fit in nbytes.
    uint32_t x = addr >> nbytes*8;
    if(x)
        panic("too many bits are set: %x\n", x);
    return 1;
}

// reg 0x4: SETUP_RETR (p58)
//   for retrans: need the attempts and delays.
#define NRF_RETRAN_COUNT_LB 0
#define NRF_RETRAN_COUNT_UB 3
#define NRF_RETRAN_DELAY_LB 4
#define NRF_RETRAN_DELAY_UB 7
enum {
    nrf_rt_max = 0b1111,
    nrf_rt_delay_max = 0b1111
};
static inline int nrf_legal_retran_count(unsigned retran_attempts) {
    return retran_attempts <= nrf_rt_max;
}
// (delay+1) * 250usec
static inline int nrf_legal_retran_delay(unsigned delay) {
    return delay <= nrf_rt_delay_max;
}

// reg 0x5: RF_CH
// Mhz added to base frequency of 2.4GHz in units of 1MHz.  
//  - RF_CH (p58,0x5). seven bits possible.
//  - description on page 25, 6.3
#define NRF_RF_CH_LB 0
#define NRF_RF_CH_UB 6
static inline int nrf_legal_Mhz_delta(unsigned d) {
    return d < (1<<8);
}

// reg=0x6: RF_PWR: tx power: high power = more current.
// description on p25, 6.5
#define NRF_DB_LB 1
#define NRF_DB_UB 2
typedef enum {
    dBm_minus18 = 0b00 << 1, // 7mA
    dBm_minus12 = 0b01 << 1, // 7.5mA
    dBm_minus6  = 0b10 << 1, // 9mA
    dBm_0       = 0b11 << 1, // 11mA
} nrf_db_t;

static inline unsigned
nrf_bits_to_db(unsigned x) {
    switch(x) {
    case 0b00: return -18; break;
    case 0b01: return -12; break;
    case 0b10: return -6; break;
    case 0b11: return 0; break;
    default: panic("bad value for db: %b\n");
    }
}

static inline int nrf_legal_db(nrf_db_t db) {
    return db >= 0 && db <= dBm_0;
}

// fixed size message: can be changed for each pipe.
// RX_PW_P0..RX_PW_P5 (page 60, reg=0x11, ... 0x16)
#define NRF_NBYTES_LB 0
#define NRF_NBYTES_UB 5
static inline int nrf_legal_msg_nbytes(unsigned nbytes) {
    return (nbytes > 0 && nbytes <= 32);
}

/******************************************************************
 * part 2: config structure: global for the entire nic.
 */

// these are the global parameters for the nic.
typedef struct nrf_config {
    // should pull out the universal things from this.
    uint8_t         Mhz_delta;          // channel frequency.
    nrf_datarate_t  dr;                 // datarate
    nrf_db_t        db;                 // tx power: -18, -12, -8, 0dBm

    uint8_t         addr_nbytes;        // address nbytes.

    // counter-intuitive, but both attempts and delay are tied to all pipes,
    // not just one.
    uint8_t         retran_attempts;    // number of retran attempts: 0..15
    uint8_t         retran_delay;       // delay betwen retrans in units of 250usec.

    uint8_t ce_pin;                   // CE GPIO pin used to
    uint8_t int_pin;                  // GPIO pin used for interrupts.

    uint8_t spi_pin;                 // either 0 or 1 (or make it the actual chip?)
    spi_t   spi;                     // spi device
} nrf_config_t;

/***************************************************************
 * part 3: simple setting methods.  look roughly the same:
 *  1. take in <config> structure and a parameter <p>
 *  2. do sanity checking on <p>.
 *  3. set it in <config> and return.
 */
static inline nrf_config_t nrf_set_Mhz_delta(nrf_config_t c, unsigned d) {
    if(!nrf_legal_Mhz_delta(d))
        panic("illegal delta: %d\n", d);
    c.Mhz_delta = d;
    return c;
}
static inline nrf_config_t
nrf_set_datarate(nrf_config_t c, nrf_datarate_t dr) {
    if(!nrf_legal_datarate(dr))
        panic("illegal datarate %d\n", dr);
    c.dr = dr;
    return c;
}
static inline nrf_config_t nrf_set_db(nrf_config_t c, nrf_db_t db) {
    if(!nrf_legal_db(db))
        panic("invalid db=%d\n", db);
    c.db = db;
    return c;
}
static inline nrf_config_t
nrf_set_addr_size(nrf_config_t c, unsigned nbytes) {
    if(!nrf_is_legal_addr_sz(nbytes))
        panic("illegal sized address: %d\n", nbytes);
    c.addr_nbytes = nbytes;
    return c;
}

// config channel's retrans strategy.  
//  - <rt_delay_usec> is in multiples of 250usec.
//  - <rt_attempts> is [1..15] inclusive.

static inline nrf_config_t
nrf_set_retran_attempts(nrf_config_t c, unsigned rt_attempts) {
    if(!nrf_legal_retran_count(rt_attempts))
        panic("illegal retran count: %d, max retran=%d\n", rt_attempts, nrf_rt_max);
    c.retran_attempts = rt_attempts;
    return c;
}

static inline nrf_config_t
nrf_set_retran_delay(nrf_config_t c, unsigned rt_delay_usec) {
    if(rt_delay_usec % 250 != 0)
        panic("illegal delay=<%d>: must be in units of 250usec\n", rt_delay_usec);

    unsigned rt_delay = rt_delay_usec / 250 - 1;
    if(!nrf_legal_retran_delay(rt_delay))
        panic("illegal retran delay: %d, max=%d\n", rt_delay, nrf_rt_delay_max);

    c.retran_delay = rt_delay;
    return c;
}
static inline nrf_config_t
nrf_retran_enable(nrf_config_t c, unsigned rt_attempts, unsigned rt_delay_usec) {
    c = nrf_set_retran_attempts(c, rt_attempts);
    return nrf_set_retran_delay(c, rt_delay_usec);
}
static inline nrf_config_t nrf_retran_disable(nrf_config_t c) {
    c.retran_attempts = 0;
    c.retran_delay = 0;
    return c;
}

static inline nrf_config_t 
nrf_conf_int(nrf_config_t c, unsigned pin) {
    assert(pin > 0 && pin < 30);
    c.int_pin = pin;
    return c;
}

static inline nrf_config_t 
nrf_conf_ce(nrf_config_t c, unsigned pin) {
    assert(pin > 0 && pin < 30);
    c.ce_pin = pin;
    return c;
}

// either SPI_CE1 or SPI_CE0 --- change convention to nrf_set_*
static inline 
nrf_config_t nrf_conf_spi(nrf_config_t c, unsigned spi_pin) {
    if(spi_pin != 0 && spi_pin != 1)
        panic("illegal spi pin: %x\n", spi_pin);
    c.spi_pin = spi_pin;
    return c;
}

/*************************************************************************
 * part 4: reasonable defaults that you can use without messing around.
 */
enum {
    nrf_default_nbytes              = 4,            // 4 byte packets.
    nrf_default_channel             = 113,          // allegedly semi-safe from interference
    // original: interference at barronne, interesting.
    // nrf_default_channel             = 61,          // allegedly semi-safe from interference
    nrf_default_data_rate           = nrf_2Mbps,    // lower data rate ==> longer distance.

    // other values cause loss in loop back: interesting.
    nrf_default_db                  = dBm_minus12,
    nrf_default_retran_attempts     = 2,            // 2 retran attempts
    nrf_default_retran_delay        = 1000,         // 500 usec retran delay
    // if we increase?  doesn't seem to matter.
    nrf_default_addr_nbytes         = 3,

    nrf_default_ce_pin              = 20,
    nrf_default_int_pin             = 21,
    nrf_default_spi_pin             = 1             // device 1.
};

// we do it like this as a way to sort-of test the various setter routines.
static inline nrf_config_t nrf_conf_unreliable_mk(void) {
    nrf_config_t c = {
            .ce_pin = nrf_default_ce_pin,  
            .spi_pin = nrf_default_spi_pin
    };
    c = nrf_set_Mhz_delta(c,  nrf_default_channel);
    c = nrf_set_datarate(c,   nrf_default_data_rate);
    c = nrf_set_addr_size(c, nrf_default_addr_nbytes);
    c = nrf_set_addr_size(c, nrf_default_addr_nbytes);
    return nrf_set_db(c,      nrf_default_db);
}
static inline nrf_config_t
nrf_conf_reliable_mk(unsigned retran_n, unsigned retran_delay) {
    nrf_config_t c = nrf_conf_unreliable_mk();
    return nrf_retran_enable(c, retran_n, retran_delay);
}
#endif
