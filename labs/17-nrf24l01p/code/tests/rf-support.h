#ifndef __RF_SUPPORT_H__
#define __RF_SUPPORT_H__

static inline unsigned
bytes_per_sec(unsigned npackets, unsigned nbytes, unsigned tot_usec) {
    // number of microseconds that passed.
    unsigned tot_bytes = npackets * nbytes;
    unsigned tot_ms = tot_usec / 1000;
    return (1000*tot_bytes)  / tot_ms;
}       
        
typedef struct {
    unsigned whole;
    unsigned fraction;
} frac_t;

static inline frac_t 
div_1000(unsigned x) {
    return (frac_t) { .whole = x / 1000, .fraction = x % 1000 };
}

// trial: should we add more information?  timeout size?
typedef struct {
    // change this to uint64_t?
    uint32_t tot_usec;   // how long it took;
    uint32_t nsent;
    uint32_t nrecv;     // received: nsent - nlost
    uint32_t nlost;
    uint32_t nbytes;
    uint32_t bytes_per_sec;
} trial_t;

static inline trial_t
trial_mk(uint32_t tot_usec, uint32_t nsent, uint32_t nlost, uint32_t nbytes) {
    //assert(nsent);
    assert(nsent >= nlost);

    unsigned nrecv = nsent - nlost;
    return (trial_t) {
        .tot_usec   = tot_usec,
        .nsent      = nsent,
        .nrecv      = nrecv,
        .nlost      = nlost,
        .nbytes      = nbytes,
        .bytes_per_sec = bytes_per_sec(nrecv, nbytes, tot_usec)
    };
}

// combine two trials: pretty simple, actually, if the nbytes are the same..
static inline trial_t
trial_add(trial_t t0, trial_t t1) {
    assert(t0.nbytes == t1.nbytes);
    return trial_mk(
            t0.tot_usec + t1.tot_usec, 
            t0.nsent + t1.nsent, 
            t0.nlost + t1.nlost,
            t0.nbytes);
}

// do we count lost?
static inline void trial_print(const char *msg, trial_t t) {
    output("trial: <%s>: total usec=%d sent %d packets lost %d\n",
        msg, t.tot_usec, t.nsent, t.nlost);
    
    // should have a way to say how many decimal places.
    frac_t f = div_1000(t.bytes_per_sec);
    printk("\ttot_millisec = %d, tot_bytes = %d  bytes_s=%d, bandwidth=%d.%d kb/sec\n",
        t.tot_usec / 1000, 
        t.nsent * t.nbytes,
        t.bytes_per_sec,
        f.whole, f.fraction);
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
trial_conf_mk(const char *msg, uint32_t ntrial, uint32_t nbytes, uint32_t timeout_usec) {
    return (trial_conf_t) {
        .msg        = msg,
        .ntrial     = ntrial,
        .nbytes     = nbytes,
        .timeout_usec = timeout_usec,
    };
}

typedef struct {
    unsigned hi, lo, cur;
} estimate_t;

static inline estimate_t
est_mk(unsigned hi, unsigned lo) {
    return (estimate_t) { .hi = hi, .lo = lo, .cur = hi };
}

// do we assume 0 can never happen?
static inline int
est_decrease(estimate_t *e) {
    e->hi = e->cur;
    unsigned x = (e->hi + e->lo) / 2;
    // done: smaller than e->lo, which didn't work.
    if(x <= e->lo)
        return 0;
    assert(x > e->lo); 
    assert(x < e->hi);  // should we return 0?

    e->cur  = x;
    return 1;
}
static inline int
est_increase(estimate_t *e) {
    e->lo = e->cur;
    unsigned x = (e->hi + e->lo) / 2;
    // done: larger than e->hi which works.
    if(x >= e->hi)
        return 0;
    assert(x > e->lo);  // should we return 0?

    e->cur = x;
    return 1;
}
// get current estimate
static inline unsigned 
est_get(estimate_t *e) {
    return e->cur;
}

static inline unsigned
est_bsearch(estimate_t *e, unsigned increase_p) {
    return increase_p ? 
        est_increase(e) : est_decrease(e);
}

#endif
