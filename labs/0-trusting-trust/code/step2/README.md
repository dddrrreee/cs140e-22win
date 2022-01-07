In this second step we're going to inject trivial attacks into the login 
and compiler in the obvious way and check that these work.

You can do all this on your own from the Thompson paper and ignore our
code (as long as it checks out!) or do the suggestions in Step 1 and
Step 2 below.

#### Step 0: Check that `make` completes successfully.

It should look like:

        % make
        gcc -Og -g -Wall login.c -o login
        gcc -Og -g -Wall compiler.c -o compiler
        ------------------------------------------
        going to check that compiler compiles login
        ./compiler login.c -o login
        echo "guest\npassword\n" | ./login
        user: passwd: successful login: <guest>
        diff login.out out
        success: compiler compiled login correctly!

#### Step 1: attack `login`

Here you'll inject an trivial attack in the login program that will make
`login` return true for user `ken` without asking for a password.

The basic approach: 
   0. To save you time we created a copy of `compiler.c` as `trojan-compiler.c`.

   1. Modify `trojan-compiler` so that if it matches the string `int login(char
      *user) {` in the input code, then inject the attack:

            if(strcmp(user, \"ken\") == 0) return 1;

      at the beginning of the `int login(...)` routine.  Emit all other
      code identically.

   2. When you type `ken` as a user, `login` should give you success:

            % ./trojan-compiler login.c -o login-attacked
            % ./login-attacked 
            user: ken
            successful login: <ken>

      And, of course, the one other user should still work:

            % ./login-attacked
            user: guest
            passwd: password
            successful login: <guest>

#### step 2: do a dumb attack on `compiler`

Before doing the fancy Thompson trick we'll just make sure everything
works by injecting a dumb "attack" into the compiler that will cause it
to inject the `printf` into the attacked compiler:

        printf("%s:%d: could have run your attack here!!\n\", 
                                        __FUNCTION__, __LINE__);

As the very first `fprintf` in `compile`.

It should work similar to the `login` attack:

   1. Match the beginning of compile:

            static void compile(char *program, char *outname) {
                FILE *fp = fopen("./temp-out.c", "w\");
                assert(fp);"

   2. Inject a `fprintf` that will emit the `printf` above in the
      emitted code.

   3. When you compile `compiler` and then compile `login` it 
      should emit:

            % ./trojan-compiler compiler.c -o cc-attacked
            % ./cc-attacked login.c -o login
            compile:19: could have run your attack here!!

Obviously, this isn't very interesting; we just do it to make debugging simple
before the next step.

#### Summary 

Cool!  These two are the easier steps just to make sure you can attack
the programs you want.  

The next one is the tricky step where you will
 - inject the code to
 - inject these attacks 
 - into the compiler so that it can 
 - inject them into the compiler :)

You will use the trick from `code/step1` to inject a self-replicating
copy of the attack into `compiler.c` while compiling it.

Why do we have to do anything more?   The big problem of course is that 
after you replace the trojan copy of `compiler` with the code it 
generates it will no longer contain the attack.  

So, for Ken, if anyone ever re-compiled the system C compiler and then 
overwrote Ken's evil binary with the new copy, the attack is gone.
For example using our toys to see the tragedy:

        # compile trojan
        % ./compiler trojan-compiler.c -o trojan-compiler  

        # replace "system" compiler
        % mv ./trojan-compiler ./compiler

        # make sure we can attack with the trojan system compiler
        % ./compiler -o login.c -o login
        % ./login
        user: ken
        successful login: <ken>   # great, still works!

        # now recompile the system compiler using the 
        # attacked compiler
        % ./compiler compiler.c -o compiler.2 
        compile:19: could have run your attack here!!  
        % mv ./compiler.2 ./compiler

        # and see if it still has the login attack.
        % ./compiler login.c -o login
        compile:19: could have run your attack here!!
        % ./login
        user: ken
        user <ken> does not exist
        login failed for: <ken>

        # uh oh, login attack does not work anymore.
        # why?

The fancy step (next) is to fix this.
