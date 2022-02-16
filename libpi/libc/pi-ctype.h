#ifndef __PI_CTYPE_H__
#define __PI_CTYPE_H__

static inline int isprint(int ch) {
    // i think all characters b/n this are printable.
    return ch >= 33 && ch <= 126;
}
static inline int islower(int ch) {
    return (ch >= 'a' && ch <= 'z');
}
static inline int isupper(int ch) {
    return (ch >= 'A' && ch <= 'Z');
}
static inline int isalpha(int ch) {
    return islower(ch) || isupper(ch);
}
static inline int isdigit(int ch) {
    return ch >= '0' && ch <= '9';
}

#endif
