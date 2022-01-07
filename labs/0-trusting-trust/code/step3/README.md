## Injecting a self replicating attack.

I'll give some hints below, but you're more than welcome to do this
on your own!  Just make sure you that you make a copy of your trojan
(`trojan-compile2`) that injects a self-replicating attack and make sure the
binary it generates when compiling a clean, virgin copy of the system
compiler is the same:

    # checking that the attack will generate itself
   
    # 1. compile compiler to generate an attacked binary.
    % ./trojan-compile2 ../step2/compiler.c -o cc-attacked

    # 2. compile clean compiler.c with the attacked copy
    % ./cc-attacked ../step2/compiler.c -o cc-attacked2

    # 3. make sure they are the same!
    # 
    % diff cc-attacked cc-attacked2

    # yea!  at this point we will automatically regenerate our attack
    # whenever someone compiles the system compiler.

    # 4. NOTE: step 3 is way too strong since it assumes same input 
    # to gcc gives the same output (e.g., no embedded time stamps etc).  
    # If it succeeds we know we have the same, but if it fails it doesn't 
    # mean we have a problem --- the real test is the login.
    % ./cc-attacked2 ../step2/login.c -o login-attacked
    % ./login-attacked
    user: ken
    successful login: <ken>
    
    # success!

### Hints

The basic idea is to take your attack and create a self-replicating version
using the code in `step1`.  Basic idea:
  1. You'll have to generate an array of ASCII values of your attack code
     as in `step1`.
  2. You'll have to modify your attack on the compiler to inject both this
     array and a printed version of it (i.e., the code) into the compiler
     you are attacking.  This is why we looked at self-replicated programs.

Overall this doesn't take much code.  In order to make it easy to regenerate
the attack as I changed it, I used `#includes`:

  1. Seperate out your attack into its own file (e.g., `attack.c`).
  2. Generate an array of this attack file using `step1` into a second
     file (`attack-array.h`).
  3. Include both files into `trojan-compile2.c`.
  4. Profit.
