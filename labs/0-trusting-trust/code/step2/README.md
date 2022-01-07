In this second step we're going to inject trivial attacks into the login 
and compiler in the obvious way and check that these work.

You can do all this on your own from the Thompson paper and ignore our
code (as long as it checks out!) or do the suggestions in Step 1 and
Step 2 below.

We give you:

  - `login.c`: a dumb login program.
  - `compile.c`: a trivial "compiler" that just reading in a file and
    runs `gcc` on it.
  - `trojan-cc.c`: a copy of `compile.c` that you will modify to 
    insert "attacks".

What you will do:
You will modify a copy of `compile.c`, `trojan-cc.c` so that:
  1. Modify `compile` in `trojan-cc.c` so that it scans its input for
     the start of the `login` routine in `login.c` and if it finds it,
     inject a backdoor.  This is fairly easy/mechanical.

  2. Similarly: modify `compile` in `trojan-cc.c` so that it scans its
     input for the `compile` routine and, if it finds it, injects a
     simple print statement that will print each time `compile` runs ---
     this is a placeholder for the final, subtle step.

#### Step 0: Check that `make` completes successfully.

When you run `make` you should see something like:

        % make
        gcc -Og -g -Wall login.c -o login
        gcc -Og -g -Wall identity-cc.c -o identity-cc
        ------------------------------------------
        going to check that identity-cc compiles login
        ./identity-cc login.c -o login
        echo "guest\npassword\n" | ./login
        user: passwd: successful login: <guest>
        diff login.out out
        success: identity-cc compiled login correctly!

#### Step 1: attack `login`

Here you'll inject an trivial attack in the login program that will make
`login` return true for user `ken` without asking for a password.

The basic approach: 
   0. To save you time we created a copy of `identity-cc.c` as `trojan-cc1.c`.

   1. Modify `trojan-cc1` so that if it matches the string `int login(char
      *user) {` in the input code, then inject the attack:

        if(strcmp(user, \"ken\") == 0) return 1;

      at the beginning of `login`.  Emit all other code identically.

   2. When you type `ken` as a user, `login` should give you success:

        % ./trojan-cc1 login.c -o login-attacked
        % ./login-attacked 
        user: ken
        successful login: <ken>

      And, of course, the one other user should still work:

        % ./login-attacked
        user: guest
        passwd: password
        successful login: <guest>

#### step 2: do a dumb attack on `identity-cc`

Before doing the fancy Thompson trick we'll just make sure everyting
works by injecting a dumb "attack" into the compiler that will cause it
to add the `printf`:

        printf("%s:%d: could have run your attack here!!\n\", 
                                        __FUNCTION__, __LINE__);

As the very first `fprintf` in `compile`.

It should work similar to the `login` attack:

   1. Match the beginning of compile:

        static void compile(char *program, char *outname) {
            FILE *fp = fopen("./temp-out.c", "w\");
            assert(fp);"

   2. Inject the `printf` above in the emitted code.

   3. When you compile `identity-cc` and then compile `login` it 
      should emit:

        % ./trojan-cc1 identity-cc.c -o cc-attacked
        % ./cc-attacked login.c -o login
        compile:19: could have run your attack here!!

Obviously, this isn't very interesting; we just do it to make debugging simple
before the next step.

#### Summary 

Cool!  These two are the easier steps just to make sure you can attack
the programs you want.  

The next one is the tricky step where you will:
    - inject the code to
    - inject these attacks 
    - into the compiler so that it can 
    - inject them into the compiler :)

You will use the trick from `code/step1` to inject a self-replicating
copy of the attack into `identity-cc.c` while compiling it.

Why do we have to do anything more?   The big problem of course is that 
after you replace the trojan copy of `identity-cc` with the code it 
generates it will no longer contain the attack.  

So, for Ken, if anyone ever re-compiled the system C compiler and
replaced his binary of it that contains his attack, the attack is gone.
For example using our toys to see the tragedy:

        # compile trojan
        % ./identity-cc trojan-cc1.c -o trojan-cc1  

        # replace "system" compiler
        % mv ./trojan-cc1 ./identity-cc

        # make sure we can attack with the trojan system compiler
        % ./identity-cc -o login.c -o login
        % ./login
        user: ken
        successful login: <ken>   # great, still works!

        # now recompile the system compiler using the 
        # attacked compiler
        % ./identity-cc identity-cc.c -o identity-cc.2 
        compile:19: could have run your attack here!!  
        % mv ./identity-cc.2 ./identity-cc

        # and see if it still has the login attack.
        % ./identity-cc login.c -o login
        compile:19: could have run your attack here!!
        % ./login
        user: ken
        user <ken> does not exist
        login failed for: <ken>

        # uh oh, login attack does not work anymore.

The fancy step (next) is to fix this.
