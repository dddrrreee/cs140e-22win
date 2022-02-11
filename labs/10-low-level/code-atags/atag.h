#ifndef __ATAG_H__
#define __ATAG_H__
enum {
    // incomplete
    ATAG_NONE = 0x00000000,
    ATAG_MEM = 0x54410002,
};

struct atag_mem {
        uint32_t     size;   /* size of the area */
        uint32_t     start;  /* physical start address */
};

// linux bootloader atag discussion:
//   http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html
// struct definitions.
//   http://www.simtec.co.uk/products/SWLINUX/files/booting_article.html#ATAG_CORE
typedef struct atag {
        uint32_t size;      // length of tag in words including this header 
        uint32_t tag;       // tag value 
        // pointer to the op-specific data in the atag record
        uint32_t data[];    // struct hack
} *atag_t;
_Static_assert(sizeof(struct atag) == 8, "Invalid sizeof atag");

static inline int atag_tag(atag_t a)
    { return a->tag; }
static inline void *atag_data(atag_t a)  
    { return &a->data[0]; }
static inline int atag_nwords(atag_t a) 
    { return a->size; }

// trivial iterator interface
static inline atag_t atag_first(void)
    { return (void*)0x100; }
static inline int atag_done(atag_t a)
    { return atag_tag(a) == ATAG_NONE; }
static inline atag_t atag_next(atag_t a) 
    { return (atag_t)((uint32_t*)a+ atag_nwords(a)); }

// look up <tag> and return a pointer to its data.
static const void *atag_lookup(unsigned tag) {
    unimplemented();
}

static unsigned atag_memsize(void) {
    const struct atag_mem *m = atag_lookup(ATAG_MEM);
    if(!m)
        panic("impossible: no ATAG_MEM\n");
    return m->size;
}

// implement this routine to print out the information for each field.  
// NOTE: ATAG_CORE has all 0s for me.
static void atag_print(const char *msg) {
    output("%s\n", msg);
    for(atag_t a = atag_first(); !atag_done(a); a = atag_next(a)) {
        uint32_t tag = atag_tag(a);
        uint32_t *u = atag_data(a);
        uint32_t nbytes = atag_nwords(a)*4;
        output("tag=%x, nbytes=%d\n", tag,nbytes);
    }
}
#endif
