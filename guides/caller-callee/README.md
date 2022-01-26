### hack: see which registers `gcc` treats as caller and callee.

If you look at `caller-callee.c` and `caller-callee.list` you can see
which registers `gcc` thinks it must save when clobbered using inline
assembly.
