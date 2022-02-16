// this will mismatch on the first instruction at user level.
void brkpt_mismatch_start(void);

// stop mismatching.
void brkpt_mismatch_stop(void);

// set a mismatch on <addr> --- call the prefetch abort handler on mismatch.
//  - you cannot get mismatches in "privileged" modes (all modes other than
//    USER_MODE)
//  - once you are in USER_MODE you cannot switch modes on your own since the 
//    the required "msr" instruction will be ignored.  if you do want to 
//    return from user space you'd have to do a system call ("swi") that switches.
void brkpt_mismatch_set(uint32_t addr);
