// usage 
//  pitag-linker  A.bin B.bin out.ptag
//      A.bin = an "OS" pi program
//      B.bin = a "user-level" pi program
//      out.ptag = name of the output program.
//
// A should copy B where it wants to go.
//
// A holds:
//  BIN_TAG
//  total nbytes: strlen(string) + 1 + nbytes(B.bin)
//  0 terminated string
//  the binary

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "libunix.h"

#define BIN_TAG 0x12345678

int main(int argc, char *argv[]) {
    if(argc != 4)
        die("invalid number of arguments: expected 2, have %d\n", argc);

    char *A = argv[1];
    char *B = argv[2];
    char *pi = argv[3];

    unsigned a_nbytes;
    uint32_t *a_bin = (void*)read_file(&a_nbytes, A);

    unsigned b_nbytes;
    uint32_t *b_bin = (void*)read_file(&b_nbytes, B);

#define link_debug(args...) do { } while(0)

    link_debug("pitag-linker %s (nbytes=%d) %s (nbytes=%d)\n", A, a_nbytes, B, b_nbytes);

    //char *pi = calloc(strlen(A)+4, 1);
    //strcpy(pi, A);
    //pi[strlen(pi)-3] = 0;
    //strcat(pi, "ptag");


    unsigned pi_nbytes = strlen(B) + 1 + b_nbytes + 4;
    link_debug("appending linked file %s of size %d+%d bytes\n", pi, pi_nbytes, 8);
    link_debug("should start at offset = %d\n", a_nbytes);
    
    int fd = create_file(pi);

    write_exact(fd, a_bin, a_nbytes);

    // word 0
    unsigned tag = BIN_TAG;
    write_exact(fd, &tag, sizeof tag);

    // word 1
    // this is the offset
    link_debug("total bytes should be: %d\n", pi_nbytes);
    write_exact(fd, &pi_nbytes, sizeof pi_nbytes);

    // word 2
    link_debug("B bytes should be: %d\n", b_nbytes);
    write_exact(fd, &b_nbytes, sizeof b_nbytes);

    // wod 3
    // this is the offset in the file.
    link_debug("offset should be: %d\n", a_nbytes);
    write_exact(fd, &a_nbytes, sizeof a_nbytes);

    write_exact(fd, b_bin, b_nbytes);

    link_debug("string should be: <%s>\n", B);
    write_exact(fd, B, strlen(B)+1);

    close(fd);
    return 0;
}
