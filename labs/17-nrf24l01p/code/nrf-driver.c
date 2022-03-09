#include "nrf-internal.h"

/*************************************************************************
 * most helpers are in nrf-internal.h  or nrf-util.c
 *
 * these are inlined here to make things a bit easier.
 */

// enable crc, enable 2 byte
#   define set_bit(x) (1<<(x))
#   define enable_crc      set_bit(3)
#   define crc_two_byte    set_bit(2)
#   define mask_int         set_bit(6)|set_bit(5)|set_bit(4)
enum { tx_config = enable_crc | crc_two_byte | set_bit(PWR_UP) | mask_int,
        rx_config = tx_config | set_bit(PRIM_RX) } ;

static nrf_pipe_t *nrf_pipe_get(nrf_t *n, unsigned pipeno) {
    assert(pipeno == 1);
    return &n->pipe;
}

static int nrf_rx_has_packet(nrf_t *nic) {
    return !nrf_rx_fifo_empty(nic);
}

// TX_FLUSH,p51: afaik we *only* do this when a reliable 
// tx failed b/c of max attempts (or, probably good:
// on boot up to put the FIFO in a good state).
static uint8_t nrf_tx_flush(const nrf_t *n) {
    uint8_t cmd = FLUSH_TX, res = 0;
    spi_n_transfer(n->c.spi, &res, &cmd, 1);
    return res;     // status.
}

static uint8_t nrf_rx_flush(const nrf_t *n) {
    uint8_t cmd = FLUSH_RX, res = 0;
    spi_n_transfer(n->c.spi, &res, &cmd, 1);
    return res;     // status.
}

/**************************************************************************
 * PUT YOUR MODIFICATIONS BELOW
 * PUT YOUR MODIFICATIONS BELOW
 * PUT YOUR MODIFICATIONS BELOW
 * PUT YOUR MODIFICATIONS BELOW
 * PUT YOUR MODIFICATIONS BELOW
 * the rest of the code you write
 */



// put device in RX mode: we always keep it here other than during startup
// and during the brief moment needed to TX a packet.   we delay until 
// device settles into a valid state so that any subsequent operation is
// guaranteed to be done in a legal state (RX).
static void nrf_rx_mode(nrf_t *n) {
    // p 22: figure 4: document says to avoid the forbidden zone b/n states.
    //
    // 1. write CE to go to <Standby-I>: so we an transition in a legal state
    // 2. then switch the NRF_CONFIG registetr.
    // 3. write CE to go to RX
    // 4. delay til device settles.
    unimplemented();

    assert(nrf_is_rx(n));
}

// set nrf into tx mode.
static void nrf_tx_mode(nrf_t *n) {
    nrf_debug("setting to tx\n");

    //  p22: from figure 4, there is no edge directly from <RX> to <TX>.
    //  thus, we first go to <Standby-I> (by setting CE=0) *then* to <TX>.
    //  I don't think there is a timing requirement?   
    //
    //  note: it might seem weird that we subsequently send by raising the
    //  ce pin hi then back lo after 10usec.  from p22: the key state change
    //  is that the FIFO is not empty.
    //
    //  1. write CE to go to a legal state.
    //  2. write the TX config.
    //  I don't think we have to wait?

    //
    // alternatively we could eliminate the 10usec requirement by going from:
    //       RX -> StandbyI (by setting CE=0) 
    //          -> StandbyII  (by setting RX=0 and CE=1)
    //          -> TX (by writing data into the FIFO)
    // see the state machine on p22.

    unimplemented();
}

// send the packet: called by nrf_tx_send_noack and nrf_tx_send_ack
//
// XXX: adapt it so you can actually send multiple packets at once.
static void do_tx(nrf_t *n, const void *msg, unsigned nbytes) {
    slow_assert(n, nrf_get8(n, NRF_CONFIG) == n->tx_config);

    // clock in a payload that has the same length as the receiver.
    // hint: use putn!
    unimplemented();

    // 9. pulse CE to transmit the packet (p23: min 10 sec)


    // set CE low
    ce_lo(n->c.ce_pin);
}


// initialize the NRF: should pass in a channel instead.
nrf_t *
nrf_init(const nrf_config_t c, uint32_t rx_addr, unsigned msg_nbytes, unsigned acked_p) {
    // change this to kmalloc.
    nrf_t nic = {0};

    // gcc bug??  or do we have a memory issue?
    memcpy(&nic.c, &c, sizeof c);
    nic.enabled_p = 1;

    // not sure if we should just send the config.
    nic.c.spi = nrf_gpio_init(c.ce_pin, c.spi_pin);

    nic.pipe = nrf_pipe_mk(rx_addr, msg_nbytes, acked_p);

    nic.tx_config = tx_config;
    nic.rx_config = rx_config;

    // atm we just pull in the interrupt code all the time.
    // could factor it out.
    if(c.int_pin) {
        nic.tx_config = bit_clr(nic.tx_config, 6);
        nic.rx_config = bit_clr(nic.rx_config, 6);
    }

    // the rule for setting parameters:
    //      p ... must do in power down, standby-I standby-II
    // we initially did in power down since that seemed like a clean, easy
    // to handle state:
    //
    //      on page ... it says we can modify when in PWR_DOWN or the standby's.
    //      to keep things simple, we put the device in PWR_DOWN
    nrf_put8_chk(&nic, NRF_CONFIG, 0);

    assert(!nrf_is_pwrup(&nic));

    // disable all eipes.
    nrf_put8_chk(&nic, NRF_EN_RXADDR, 0);

    if(!acked_p) {
        // reg1: disable retran.
        nrf_put8_chk(&nic, NRF_EN_AA, 0);
        // reg2: enable pipes.
        nrf_put8_chk(&nic, NRF_EN_RXADDR, 1 << 1);
    } else {
        // reg=1: p57
        // XXX: seems like both retran pipe and pipe0 have to be ENAA_P0 = 1 (p75)
        // BUG from before: didn't enable for PO.
        nrf_put8_chk(&nic, NRF_EN_AA, 1 << 1 | 1);

        // reg=2: p 57, enable pipes --- always enable pipe 0 for retran.
        nrf_put8_chk(&nic, NRF_EN_RXADDR, 1 << 1 | 1);

        unsigned rt_cnt = c.retran_attempts;
        unsigned rt_d = c.retran_delay;
        assert(bits_get(rt_d,4,7) == 0);

        // reg = 4: setup retran
        nrf_put8_chk(&nic, NRF_SETUP_RETR, rt_cnt | (rt_d << 4));

        // double check
        assert(nrf_pipe_is_enabled(&nic, 0));
        assert(nrf_pipe_is_enabled(&nic, 1));
        assert(nrf_pipe_is_acked(&nic, 1));
        assert(nrf_pipe_is_acked(&nic, 0));
    }

    // reg=3 setup address size
    nrf_assert(&nic, c.addr_nbytes == nrf_default_addr_nbytes);
    unsigned v = c.addr_nbytes - 2;
    nrf_put8_chk(&nic, NRF_SETUP_AW, v);

    nrf_pipe_t *p = &nic.pipe;

    nrf_put8_chk(&nic, NRF_RX_PW_P1, p->msg_nbytes);
    nrf_set_addr(&nic, NRF_RX_ADDR_P1, p->rxaddr, c.addr_nbytes);
    
    // set message size = 0 for unused pipes.  [i think is redundant]
    nrf_put8_chk(&nic, NRF_RX_PW_P2, 0);
    nrf_put8_chk(&nic, NRF_RX_PW_P3, 0);
    nrf_put8_chk(&nic, NRF_RX_PW_P4, 0);
    nrf_put8_chk(&nic, NRF_RX_PW_P5, 0);


    // reg=5: RF_CH: setup channel --- this is for all addresses.
    nrf_put8_chk(&nic, NRF_RF_CH, c.Mhz_delta);

    // reg=6: RF_SETUP: setup data rate and power
    // datarate already has the right encoding.
    assert(!bits_intersect(c.dr, c.db));
    nrf_put8_chk(&nic, NRF_RF_SETUP, c.dr | c.db);

    // reg=7: status.  p59
    // sanity check that it is empty and nothing else is set.

    // NOTE: if we are in the midst of an active system,
    // it's possible this node receives a message which will
    // change these values.   we might want to set the
    // rx addresses to something that won't get a message.
    // 
    // ideally we would do something where we use different 
    // addresses across reboots?   we get a bit of this benefit
    // by waiting the 100ms.

    // these are not guaranteed if a stray packet arrives.
    nrf_tx_flush(&nic);
    nrf_rx_flush(&nic);

    nrf_put8(&nic, NRF_STATUS, ~0);
    assert(nrf_get8(&nic, NRF_STATUS) == (0b111 << 1));

    assert(!nrf_tx_fifo_full(&nic));
    assert(nrf_tx_fifo_empty(&nic));
    assert(!nrf_rx_fifo_full(&nic));
    assert(nrf_rx_fifo_empty(&nic));

    assert(!nrf_has_rx_intr(&nic));
    assert(!nrf_has_tx_intr(&nic));
    assert(pipeid_empty(nrf_rx_get_pipeid(&nic)));
    assert(!nrf_rx_has_packet(&nic));

    // we skip reg=0x8 (observation)
    // we skip reg=0x9 (RPD)
    // we skip reg=0xA (P0)


    // we skip reg=0x10 (TX_ADDR): used only when sending.

    // reg=0x17: FIFO_STATUS p61

    // reg=0x1c dynamic payload (next register --- don't set the others!)
    assert(nrf_get8(&nic, NRF_DYNPD) == 0);

    // reg 0x1d: feature register.  we don't use it yet.
    nrf_put8_chk(&nic, NRF_FEATURE, 0);


    // XXX: flushing = the receiver will think we received,
    // but we then kill it.  I think your network protocol has to be
    // aware of this.  if we move these earlier are the other invariants
    // guaranteed?
    nrf_tx_flush(&nic);
    nrf_rx_flush(&nic);

    // i think w/ the nic is off, this better be true.
    assert(nrf_tx_fifo_empty(&nic));
    assert(nrf_rx_fifo_empty(&nic));

    // when you go to power up for first time, have to delay.
    // p20: go from <PowerDown> to <Standby-I>
    nrf_or8(&nic, NRF_CONFIG, PWR_UP << 1);
    delay_ms(2);

    // now go to RX mode: invariant = we are always in RX except for the 
    // small amount of time we need to switch to TX to send a message.
    nrf_rx_mode(&nic);

    nrf_t *n = kmalloc(sizeof *n);
    memcpy(n,&nic,sizeof *n);
    n->pipe.nic = n;        // setup internal pointer.

    if(nrf_verbose_p)
        nrf_dump("after config\n", n);
    return n;
}

// while the RX fifo is not empty: read packets into the 
// circular queue.
int nrf_get_pkts(nrf_t *n) {
    // ERROR: very painful: as you probably recall from awhile back,
    // the datasheet reads like you will not receive packets if you
    // are not in RX mode.  *this appears to be correct* (but should
    // test more).    thus, if you check for packets before you put in
    // rx mode, you won't receive any.  this was causing a nasty problem
    // where nothing was getting received.   aiya.  so do not optimize
    // the check here --- always do it.  in tx if you want to empty
    // the rx queue first, you can check there before calling.  also,
    // when done with tx, you should put back in rx mode.
    //
    // if you want to optimize sends, you can do a batch version where
    // you leave in tx.  right now won't be doing this.   there are a lot of
    // settling times, so it makes sense to have this kind of option.  (you
    // can also do a deep send where you set multiple packets in the FIFO)

    // always should be in rx config, aways should have ce_pin high.
    slow_assert(n, nrf_get8(n, NRF_CONFIG) == n->rx_config);

    // *ERROR* if you are not in receive mode, you cannot check the rx_has_packet
    //    before: we don't have to be in RX mode for this, right?
    if(!nrf_rx_has_packet(n)) 
        return 0;

    unsigned cnt = 0;

    // data sheet gives the sequence to follow to get packets.
    // p63: 
    //    1. read packet through spi.
    //    2. clear IRQ.
    //    3. read fifo status to see if more packets: 
    //       if so, repeat from (1) --- we need to do this now in case
    //       a packet arrives b/n (1) and (2)
    do { 
        unsigned pipen = nrf_rx_get_pipeid(n);
        if(pipen == NRF_PIPEID_EMPTY)
            panic("impossible: empty pipeid: %b\n", pipen);

        nrf_pipe_t *p = nrf_pipe_get(n,pipen);
        assert(p);
        unsigned nbytes = p->msg_nbytes;

        // lots of extra copies: temp buffer.
#       define NRF_MAX_PKT 32
        uint8_t msg[NRF_MAX_PKT];

        // getn returns status: extract the pipeid from it and 
        // make sure they match.
        uint8_t status = nrf_getn(n, NRF_R_RX_PAYLOAD, msg, nbytes);

        // this isn't really a slow check.
        slow_assert(n, pipen == pipeid_get(status));

        // XXX: maybe start deleting things?  or?
        if(!cq_push_n(&p->recvq, msg, nbytes))
            panic("not enough space left for message on pipe=%d\n", pipen);

        p->tot_msgs++;
        p->tot_recv_bytes += nbytes;

        // XX: do we have to clear the int?   
        // clear RX interrupt: note since it is a binary value, there could be more 
        // packets on the queue so we have to check again.
        nrf_rx_intr_clr(n);
        nrf_debug("received message on pipe: %d\n", pipen);
        cnt++;
    } while(!nrf_rx_fifo_empty(n));

    return cnt;
}

// send a packet without ack.
int nrf_tx_send_noack(nrf_t *n, uint32_t txaddr, const void *msg, unsigned nbytes) {
    slow_assert(n, nrf_get8(n, NRF_CONFIG) == n->rx_config);

    // do we have to do this?   only if no interrupt?
    while(nrf_get_pkts(n))
        ;
    nrf_tx_mode(n);

    slow_assert(n, !nrf_pipe_is_enabled(n, 0));
    slow_assert(n, nrf_pipe_is_enabled(n, 1));
    slow_assert(n, !nrf_pipe_is_acked(n, 1));
    slow_assert(n, nrf_tx_fifo_empty(n));

    unsigned addr_nbytes = n->c.addr_nbytes;
    nrf_debug("setting tx addresses: %x, nbytes=%d\n", txaddr, addr_nbytes);

    unimplemented();

    nrf_debug("about to do a tx\n");
    do_tx(n,msg,nbytes);


    // wait for send to succeed:
    //  1. wait until tx is empty.
    //  2. clear the tx interrupt.
    //  3. switch back to rx mode.
    unimplemented();


    nrf_debug("tx success\n");
    nrf_rx_mode(n);
    return nbytes;
}

// send a packet with an ack
int nrf_tx_send_ack(nrf_t *n, uint32_t txaddr, const void *msg, unsigned nbytes) {
    slow_assert(n, nrf_get8(n, NRF_CONFIG) == n->rx_config);

    // drain the rx if it's not empty so that we can receive acks.
    // do this before we switch out of rx mode (once we're out of rx i think
    // we never receive any packets).   
    while(nrf_get_pkts(n))
        ;
    nrf_tx_mode(n);

    slow_assert(n, !nrf_has_max_rt_intr(n));
    slow_assert(n, !nrf_has_tx_intr(n));

    // not necessarily true, but is for our current setup.
    slow_assert(n, nrf_pipe_is_enabled(n, 0));
    slow_assert(n, nrf_pipe_is_enabled(n, 1));
    slow_assert(n, nrf_pipe_is_acked(n, 0));
    slow_assert(n, nrf_pipe_is_acked(n, 1));

    // this is only true b/c we send synchronously.   have to remove if doing
    // interrupts.
    //  note: if our rx buffer is full, we can't get acks.
    slow_assert(n, nrf_tx_fifo_empty(n));

    // this can be false: race condition b/n get_pkts and going to tx mode.
    // maybe pull more packets if it's not?
    slow_assert(n, nrf_rx_fifo_empty(n));

    // XXX: I've seen other implementations that do rx/tx flushes.  I 
    // do not see why you would have to.
    // nrf_tx_flush();
    // nrf_tx_flush();

    // we do everything synchronously right now, so should not happen.
    // need to seperate out.  i'm a bit confused about what happens if 
    // the ack fails in this case --- do you have to resend the other
    // messages to xmit?  tx_flush will kill them, right?
    //
    // note: if you want to send multiple at once, will have to handle  
    // out of order.
    if(nrf_tx_fifo_full(n)) 
        panic("tx fifo is full? (afaik this should not happen.\n");

    // for ack.
    unsigned addr_nbytes = n->c.addr_nbytes;
    nrf_debug("setting tx addresses: %x, nbytes=%d\n", txaddr, addr_nbytes);

    // setup for ack:
    //  1. write address for retran and for tx.
    unimplemented();

    do_tx(n,msg,nbytes);


    
    // we need to spin until we have a tx success or we get an 
    // error in terms of the number of retran attempts.
    // on error: p48: need to to a tx_flush() to get the failed
    // packet out of the fifo.
    // 
    // p43: when ack successful, will get a tx interrupt (TX_DS,p59).  if 
    // ack not successful, will get a MAX_RT interrupt (MAX_RT,p59)

    // XXX: big problem: the whole time we are sending we won't be able to
    // receive any messages.  (tho the datasheet does say it switches to 
    // rx mode to get ack packets).  the longer you are here the more
    // stuff you lose.   
    //
    // XXX: if you want to remove big delays need to do this with an interrupt.
    // in that case need to return a handle of some kind.
    while(1) {

        // success!
        if(nrf_has_tx_intr(n)) {
            slow_assert(n, nrf_tx_fifo_empty(n));
            /// clear the TX interrupt
            unimplemented();
            goto done;
        }

        // test this by setting retran to 1.
        if(nrf_has_max_rt_intr(n)) {
            // have to flush and clear the rt interrupt.
            nrf_dump("max inter config", n);
            panic("max intr\n");
        }

        // XXX: if we transition to RX, do we lose all the TX setup?
        // if a bunch of nodes send to us all at once, the rx could fill
        // up.  i think we need to spin and pull stuff out.   1/ are we able
        // to do so if not in rx mode?  2/ would we be pulling out the ack
        // packet and messing w/ the hw?
        if(nrf_rx_fifo_full(n))
            panic("rx fifo is full!\n");
    }
    not_reached();

done:
    // this used to be in the main loop: it failed there.
    // *in a general system with many senders this is not going to be true*
    if(slow_check_p && !nrf_rx_fifo_empty(n))
        debug("got a non-empty RX FIFO during TX: interesting.\n");

    nrf_rx_mode(n);
    return nbytes;
}
