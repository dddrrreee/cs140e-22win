#include "rpi.h"




void a(void);
void notmain(void) {
    a();
}

static unsigned depth;
static void indent(unsigned n) {
    n *= 3;
    while(n-->0)
        output(" ");
}
static void push(const char *msg) { 
    indent(depth++);
    output("---> call %s\n", msg);
}
static void pop(const char *msg) { 
    indent(--depth);
    output("<--- ret %s\n", msg);
}


#define MK_STR(w) #w

#define gen_fn(x,y)                                             \
    void y (void);                                              \
    void x (void) { push(MK_STR(y)); y(); pop(MK_STR(y)); }

void f(void);

gen_fn(a,b)
gen_fn(b,c)
gen_fn(c,d)
gen_fn(d,f)

void backtrace(void) {}
void f(void) { 
    indent(depth); 
    debug("done\n"); 
    backtrace();
}
