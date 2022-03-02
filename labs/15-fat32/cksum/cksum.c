#include "libunix.h"

int main(int argc, char *argv[]) { 
    for(int i = 1; i < argc; i++) {
        unsigned nbytes;
        void *p = read_file(&nbytes, argv[i]);

        output("file <%s>: crc=0x%x, nbytes=%d\n", 
            argv[i], fast_hash(p, nbytes), nbytes);
        free(p);
    }

    return 0; 
}
