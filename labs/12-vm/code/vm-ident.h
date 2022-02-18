#ifndef __VM_IDENT_H__
#define __VM_IDENT_H__

#include "rpi.h"
#include "rpi-constants.h"
#include "rpi-interrupts.h"

#include "mmu.h"

fld_t * vm_ident_mmu_init(int start_p);
void vm_ident_mmu_on(fld_t *pt);
void vm_ident_mmu_off(void);

// kinda lame, but useful to have around.
enum { OneMB = 1024 * 1024 };

// we use a non-zero domain id to test things out.
enum { dom_id = 1};

#endif
