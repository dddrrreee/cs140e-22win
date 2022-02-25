#include "pix-internal.h"

static pix_env_t init_process;
pix_env_t *pix_cur_process = &init_process;


// create a new pix structure that fills in the 
// register save area with the right initial values so 
// that when you use <switch_to> on it for the first time
// it will behave identically to calling <user_mode_run_fn>
pix_env_t pix_env_mk(uint32_t pc, uint32_t sp) {
    pix_env_t p = {0};

    // put unimplemented here.
    p.reg_save[0] = pc;
    p.reg_save[1] = sp;
    p.reg_save[SPSR_OFF] = 0x190;
    p.reg_save[PC_OFF] = pc;
    p.reg_save[SP_OFF] = sp;

    printk("pc=%x, sp=%x\n", pc,sp);
    return p;
}
