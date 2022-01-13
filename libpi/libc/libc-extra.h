#ifndef __LIBC_EXTRA_H__
#define __LIBC_EXTRA_H__

// put your extra libc prototypes here.
uint32_t fast_hash(const void *data, uint32_t len);
uint32_t fast_hash_inc(const void * _data, uint32_t len, uint32_t hash);

#endif
