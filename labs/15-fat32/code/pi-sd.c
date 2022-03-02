#include "rpi.h"
#include "pi-sd.h"
#include "bzt-sd.h"
#include "libc/crc.h"

static int trace_p = 1;
static int init_p = 0;

int pi_sd_init(void) {
  if(sd_init() != SD_OK)
    panic("sd_init failed\n");
  init_p = 1;
  return 1;
}

int pi_sd_read(void *data, uint32_t lba, uint32_t nsec) {
  demand(init_p, "SD card not initialized!\n");
  int res;
  if((res = sd_readblock(lba, data, nsec)) != 512 * nsec)
    panic("could not read from sd card: result = %d\n", res);

  if(trace_p)
    trace("sd_read: lba=<%x>, cksum=%x\n", lba, our_crc32(data,nsec*512));
  return 1;
}

// allocate <nsec> worth of space, read in from SD card, return pointer.
// your kmalloc better work!
void *pi_sec_read(uint32_t lba, uint32_t nsec) {
  demand(init_p, "SD card not initialized!\n");
  uint8_t *data = kmalloc(nsec * 512);
  if(!pi_sd_read(data, lba, nsec))
    panic("could not read from sd card\n");
  return data;
}

int pi_sd_write(void *data, uint32_t lba, uint32_t nsec) {
  demand(init_p, "SD card not initialized!\n");
  int res;
  if((res = sd_writeblock(data, lba, nsec)) != 512 * nsec)
    panic("could not write to sd card: result = %d\n", res);

  if(trace_p)
    trace("sd_write: lba=<%x>, cksum=%x\n", lba, our_crc32(data,nsec*512));
  return 1;
}
