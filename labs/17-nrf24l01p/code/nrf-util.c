/***************************************************************************
 *          *** Do not modify: will get conflicts. ***
 *          *** Do not modify: will get conflicts. ***
 *          *** Do not modify: will get conflicts. ***
 *          *** Do not modify: will get conflicts. ***
 *          *** Do not modify: will get conflicts. ***
 *
 * low level utility routines for the NRF.
 */
#include "nrf-internal.h"

unsigned nrf_verbose_p = 0;
void nrf_set_verbose(int v_p) { 
    nrf_verbose_p = v_p; 
}

// change db from what is in the config.
// change the datarate.
void nrf_redo_datarate(nrf_t *nic, nrf_datarate_t dr) {
    assert(nrf_is_standbyI(nic));
    nrf_config_t c = nic->c;

    nrf_output("reseting dr from %dMbs to %dMbs\n", c.dr, dr);
    nic->c = c = nrf_set_datarate(c, dr);
    assert(!bits_intersect(c.dr, c.db));

    // reg=6: RF_SETUP: setup data rate and power
    // datarate already has the right encoding.
    nrf_put8_chk(nic, NRF_RF_SETUP, c.dr | c.db);
}

void nrf_redo_db(nrf_t *nic, nrf_db_t db) {
    assert(nrf_is_standbyI(nic));
    nrf_config_t c = nic->c;

    nrf_output("reseting db from %ddBm to %ddBm\n",
            nrf_bits_to_db(c.db), nrf_bits_to_db(db));
    nic->c = c = nrf_set_db(c, db);
    assert(!bits_intersect(c.dr, c.db));

    // reg=6: RF_SETUP: setup data rate and power
    // datarate already has the right encoding.
    nrf_put8_chk(nic, NRF_RF_SETUP, c.dr | c.db);
}


// read the value of nrf <reg>
// as an option, change this to use getn so there is just one copy.
// we did it like this so it's a simpler bit of code to inspect at the 
// very beginning.
uint8_t nrf_get8(const nrf_t *n, uint8_t reg) {
    uint8_t rx[3], tx[3];

    // is this supposed to be three?
    tx[0] = reg;
    tx[1] = NRF_NOP;

    // nrf_debug("sent: tx[0]=%b\n", tx[0]);
    spi_n_transfer(n->c.spi, rx,tx,2);
    // nrf_debug("recv: rx[0]=%b, rx[1]=%b\n", rx[0], rx[1]);
    return rx[1];
}

// set nrf <reg> = <v>
// as an option, getn so there is just one copy.
// we did it like this so it's a simpler bit of code to inspect at the 
// very beginning. this to use putn
uint8_t nrf_put8(nrf_t *n, uint8_t reg, uint8_t v) {
    uint8_t rx[3], tx[3];

    tx[0] = NRF_WR_REG | reg;
    tx[1] = v;

    // nrf_debug("wr8: sent: tx[0]=%b, tx[1]=%b\n", tx[0], tx[1]);
    spi_n_transfer(n->c.spi, rx,tx,2);
    // nrf_debug("wr8: recv: rx[0]=%b, rx[1]=%b\n", rx[0], rx[1]);

    return rx[0];   // status.
}

uint8_t nrf_put8_chk_helper(src_loc_t loc, nrf_t *nic, uint8_t reg, uint8_t v) {
    uint8_t status = nrf_put8(nic, reg, v);

    // doing this *always* ran into problems during message sending.
    // so made this seperate routine.
    uint8_t x = nrf_get8(nic, reg);
    if(x != v)
        loc_panic(loc, "reg=<%x>: got <%x>, expected <%x>\n", reg, x, v);
    return status;
}

// send <cmd> and get <nbytes> of data from the nrf.
uint8_t nrf_getn(nrf_t *nic, uint8_t cmd, void *bytes, uint32_t nbytes) {
    uint8_t rx[64], tx[64];
    assert(nbytes < (sizeof tx + 1));
    assert(nbytes > 0);

    // need to fix the spi interface.
    tx[0] = cmd;
    memset(&tx[1], NRF_NOP, nbytes);

    spi_n_transfer(nic->c.spi, rx,tx,nbytes+1);
    memcpy(bytes, &rx[1], nbytes);

    return rx[0];   // status
}

// write <cmd> with <nbytes> of data to the nrf.
uint8_t nrf_putn(nrf_t *nic, uint8_t cmd, const void *bytes, uint32_t nbytes) {
    uint8_t rx[64], tx[64];
    assert(nbytes < (sizeof tx + 1));
    assert(nbytes > 0);

    // need to fix the spi interface.
    tx[0] = cmd;
    memcpy(&tx[1], bytes, nbytes);
    spi_n_transfer(nic->c.spi, rx,tx,nbytes+1);

    return rx[0];   // status.
}

uint8_t nrf_or8(nrf_t *n, uint8_t reg, uint8_t v) {
    return nrf_put8(n, reg, nrf_get8(n, reg) | v);
}

// rmw: set reg:bit=0
void nrf_bit_clr(nrf_t *n, uint8_t reg, unsigned bit) {
    assert(bit < 8);
    nrf_put8_chk(n, reg, bit_clr(nrf_get8(n, reg), bit));
}
// rmw: set reg:bit=1
void nrf_bit_set(nrf_t *n, uint8_t reg, unsigned bit) {
    assert(bit < 8);
    nrf_put8_chk(n, reg, bit_set(nrf_get8(n, reg), bit));
}
// is reg:bit == 1?
int nrf_bit_isset(nrf_t *n, uint8_t reg, uint8_t bit_n) {
    assert(bit_n<8);
    return bit_get(nrf_get8(n, reg), bit_n) == 1;
}

// pi side GPIO setup
spi_t nrf_gpio_init(unsigned ce, unsigned spi_chip) {
    // nrf_ce_pin = ce;
    // ok: we used to use a power pin, however, the GPIO pins produce
    // very little current (16ma at most?).  even the dedicated 3.3v gives at most
    // 50ma.   i think we need a different approach, but for the moment,
    // we should go w/ 3.3v.

    delay_ms(100);          // p 20 implies takes 100ms to get device online?

    // not sure about this --- probably want to allow a different CE pin and a different
    // SPI.
    assert(spi_chip == 0 || spi_chip == 1);
    // spi_t s = spi_n_init(chip, 1024); /* 250KHz: what the pi is clocked at */


    // this is the smallest divisor that seems to work (19 fails): 
    // does not have to be a power of 2.
    spi_t s = spi_n_init(spi_chip, 22);

#if 0
    if(spi_chip == 0 ) {
        assert(ce == 20);
        s = sw_spi_mk(spi_chip, 22);
    } else {
        assert(ce == 21);
        s = spi_n_init(spi_chip, 22);
    }
#endif

    assert(s.chip == spi_chip);

    delay_ms(100);

    dev_barrier();
    gpio_set_output(ce);
    gpio_set_pulldown(ce);
    gpio_set_off(ce);
    dev_barrier();

    //check_reset_values();
    return s;
}

static inline unsigned nrf_get_addr_nbytes(nrf_t *n) {
    return nrf_get8(n, NRF_SETUP_AW) + 2;
}

void nrf_dump(const char *msg, nrf_t *nic) {
    nrf_output("---------------------------------------------\n");
    nrf_output("%s:\n", msg);
    uint8_t x = nrf_get8(nic, NRF_CONFIG);
    nrf_output("\tCONFIG (%b):\tPWR_UP=%d, PRIM_RX=%d, CRC=%d EN_CRC=%d", 
        x,bit_get(x,1), 
        bit_get(x,0),
        bit_get(x,2),
        bit_get(x,3));
    output(" MASK_RX_DR==%d, MASK_TX_DS=%d, MASK_MAX_RT=%d\n",
            bit_get(x,6), bit_get(x,5), bit_get(x,4));

    x = nrf_get8(nic, NRF_EN_AA);
    nrf_output("\tEN_AA (%b):\t", x);
    if(x == 0b111111)
        output("all pipes have auto-ack enabled\n");
    else {
        output("pipes enabled:");
        for(int i = 0; i < 6; i++)
            if(bit_get(x,i))
                output(" pipe%d=auto-ack ", i);
        output("\n");
    }

    x = nrf_get8(nic, NRF_EN_RXADDR);
    nrf_output("\tEN_RXADDR (%b):", x);
    if(x == 0b111111)
        output("\tall pipes enabled\n");
    else {
        output("\tpipes enabled:");
        for(int i = 0; i < 6; i++)
            if(bit_get(x,i))
                output(" pipe%d=1 ", i);
        output("\n");
    }

    x = nrf_get8(nic, NRF_SETUP_AW);
    assert(x >= 1 && x <= 3);
    unsigned addr_nbytes = nrf_get_addr_nbytes(nic);
    assert(nrf_get_addr_nbytes(nic) == x + 2);

    nrf_output("\tSETUP_AW:\taddress=%d bytes\n", addr_nbytes);

    x = nrf_get8(nic, NRF_SETUP_RETR);
    unsigned delay = ((x >> 4)+1) * 250;
    unsigned retry = x & 0b1111;
    nrf_output("\tSETUP_RETR (=%b):\tretran delay=%dusec, attempts=%d\n", x, delay, retry);

    x = nrf_get8(nic, NRF_RF_CH);
    nrf_output("\tRF_CH(=%b):\t2.%dMhz\n", x, 400 + x); // i think this is correct
    
    x = nrf_get8(nic, NRF_RF_SETUP);
    assert(!bit_get(x,5));
    unsigned bwidth = bit_get(x,3) ? 2 : 1;
    int pwr;
    switch(bits_get(x,1,2)) {
    case 0b00: pwr = -18; break;
    case 0b01: pwr = -12; break;
    case 0b10: pwr = -6; break;
    case 0b11: pwr = 0; break;
    default: panic("bad encoding\n");
    }
    nrf_output("\tRF_SETUP(=%b):\tbwidth = %dMbs, power= %ddBm\n", x, bwidth, pwr);

    x = nrf_get8(nic, NRF_STATUS);
    nrf_output("\tSTATUS(=%b): RX-int=%d, TX-int=%d, RT-int=%d RXpipe=%b (%s), TX-full=%d\n",
            x, 
            bit_get(x, 6),
            bit_get(x, 5),
            bit_get(x, 4),
            bits_get(x, 1, 3),
            (bits_get(x, 1, 3) == 0b111) ? "empty" : "has data!",
            
            bit_get(x, 0));
    
    
    uint64_t txaddr = 0;
    nrf_getn(nic, NRF_TX_ADDR, &txaddr, addr_nbytes);
    nrf_output("\tTX_ADDR = %llx\n", txaddr);

    unsigned pipes_en = nrf_get8(nic, NRF_EN_RXADDR);
    for(unsigned i = 0; i < 6; i++) {
        if(!bit_get(pipes_en, i))
            continue;
        uint64_t addr = 0;

        nrf_getn(nic, NRF_RX_ADDR_P0+i, &addr, addr_nbytes);
        nrf_output("\tRX_ADDR_P%d = %llx\n", i, addr);

        x = nrf_get8(nic, NRF_RX_PW_P0+i);
        nrf_output("\tRX_PW_P%d (%b): %d byte fixed size packets\n", i, x, x);
    }
    
    x = nrf_get8(nic, NRF_FIFO_STATUS);
    nrf_output("\tFIFO_STATUS(=%b): TX_FULL=%d, TX_EMPTY=%d, RX_FULL=%d, RX_EMPTY=%d\n",
        x,
        bit_get(x,5),
        bit_get(x,4),
        bit_get(x,1),
        bit_get(x,0));

    assert(nrf_get8(nic, NRF_FEATURE) == 0);
    assert(nrf_get8(nic, NRF_DYNPD) == 0);
    nrf_output("---------------------------------------------\n");
}



struct reg_spec {
    unsigned reg;
    const char *reg_str;
    unsigned nbytes;
    uint64_t init_val;
} reg_spec[] = {
#   undef NRF_REG
#   define NRF_REG(enum_name, reg_num, init_v, reg_nbytes)        \
      (struct reg_spec) {                                           \
            .reg = NRF_ ## enum_name,                               \
            .reg_str = _XSTRING(enum_name),                         \
            .nbytes = reg_nbytes,                                   \
            .init_val = init_v                                      \
      },    

#   include "nrf-regs.h"
    {0}
};


void nrf_dump_raw(const char *msg, nrf_t *nic) {
    unsigned addr_nbytes = nrf_get_addr_nbytes(nic);
    output("%s:\n", msg);
    for(int i = 0; reg_spec[i].reg_str; i++) {
        struct reg_spec *r = &reg_spec[i];

        output("\t%s (%x): ", r->reg_str, r->reg);
        if(r->nbytes == 1)
            output("%b\n", nrf_get8(nic, r->reg));
        // i think it's only the addresses that get read like this.
        else if(r->nbytes == 5) {
            uint64_t x = 0;
            nrf_getn(nic, r->reg, &x, addr_nbytes);
            output("%llx\n", x);
        } else
            panic("invalid number of bytes: %d\n", r->nbytes);
    }
}
