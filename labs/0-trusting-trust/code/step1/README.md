### Overview 

Finish implementing a self-reproducing program based on Figure 1 in
Thompson's paper.

You are given:

  - `seed.c`: the main part of the C code in the paper (so 
     you don't have to type it in).

   - the recipe in the `Makefile` for making and checking
     the program (see below).


### What to do

You should implement:
  - `string-to-char-array.c` which when fed `seed.c` will spit out a
  character array describing it (as shown in the beginning of Figure 1).


When you run 

    % ./string-to-char-array < seed.c 

You should get something that looks like:

    char prog[] = {
	    47, 	47, 	32, 	99, 	97, 	110, 	32, 	112,
	    117, 	116, 	32, 	97, 	110, 	121, 	32, 	112,


### How to check your work

When the `Makefile` runs the following, it should pass:

    # 1. generate byte array based on seed.c
    ./string-to-char-array < seed.c > replicate.c
    # 2. concatenate seed.c to the emitted byte array
    cat seed.c >> replicate.c
    # 3. compile the result
    gcc replicate.c -o replicate
    # 4. run it and put the output in kid.c
    ./replicate > kid.c
    # 5. make sure kid.c and replicate.c are identical.
    diff replicate.c kid.c

Congratulations!  This is the first step in replicating Thompson's hack.
