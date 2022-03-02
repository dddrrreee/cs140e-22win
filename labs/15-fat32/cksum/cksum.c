#include "libunix.h"

int main(int argc, char *argv[]) { 
    for(int i = 1; i < argc; i++) {
        unsigned nbytes;
        void *p = read_file(&nbytes, argv[i]);

        output("file <%s>: crc=%x, nbytes=%d\n", 
            argv[i], our_crc32(p, nbytes), nbytes);
        free(p);
    }

    return 0; 
}
