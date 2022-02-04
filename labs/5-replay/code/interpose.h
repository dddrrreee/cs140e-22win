#ifndef __INTERPOSE_H__
#define __INTERPOSE_H__

#define trace_output(args...) output("INTERPOSE:" args)
#define trace_err(args...) die("INTERPOSE:ERROR:" args)
#define trace_ok(args...) output("INTERPOSE:SUCCESS:" args)

int clean_run(char *install, char *dev_name, char *pi_prog);
int timeout_run(char *install, char *dev_name, char *pi_prog, unsigned timeout_byte);

#endif 
