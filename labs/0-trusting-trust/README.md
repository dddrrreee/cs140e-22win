## Trusting trust

Read the short Ken Thompson paper in this directory --- this was his
Turing award lecture (our nobel prize), where he discussed an evil,
very slippery hack he implemented on the early Unix systems.

We will write the code for this hack.  While the paper is short, and the
hack seems not that hard, when you actually have to write out the code,
you'll likely realize you don't actually know the next thing to type.
Is a neat example of how circular definitions work when you build
compilers.

Everyone I've met above a certain age in systems has read this paper
and they all seem to think they understand it.  However, I've not met
a single one that had ever written the code to do it.  There is a big
difference between passively nodding to nouns and verbs and being able
to actively construct an implementation of an idea.

### Reading questions about the paper.

Thompson: wrote Unix; got early Turing award.   

Simple example to illustrate point of paper: `while` loops do not exist
in compiler `C0`.  How do you add them?
  1. Write code to parse/implement them in copy of `C0`, call it `C1`.
  2. Compile `C1` with `C0`.
  3. Rewrite code in `C1` to use `while` loops, giving `C2`.
  4. Compile `C2` with `C1`.  Pretty easy so far, just requires some work.
  5. Can then compile `C2` with itself and *throw away C0 and C1*.
     Circular definitions can work in compilers.  This is weirder
     than the words used to describe it
  6.  Of course, you better not lose both `C1` and `C2`!

Thompson wants to add a hack to the the compiler that will:
  1. Recognize when it compiles the password program and insert a
     backdoor into it.  (Not that hard to understand.)
  2. Recognize the compiler itself and insert (1) into it.
  3. Compile (2) and throw away the source.  (As in our `while` exmaple).
  4. Thus, will automatically modify `passwd` and also will automatically
     modify a clean copy of the compiler.

Breaking down the paper:
 - program that when run will produce itself.
 - "not precisely a self-reproducing program"?   (it has things such as `\t`
   rather than `10`).
 - How does it produce a self-reproducing program?  [When you run it it 
   spits out ASCII code.]
 - "Contains an arbitrary set of baggage": where?
 - What can you delete?
 - From the code: the comment states that `s` is a representation "from `0`
   to the end". 
     - Which `0` do you mean?
     - How do you know?
     - What this point in the code?
 - Can you do this without a loop?
 - Why not `printf` the string twice?
 - Can you reverse the order?
 - "Many chicken and egg problems" --- others?  (Any contruct if you 
   compile the compiler with itself.)

Questions about his code:
  - What is interesting about 2.1 (2.2 in the text)?
    [`\n` is defined in terms of itself]
    
  - What happens if you compile 2.3 in the original compiler?
    [Parse error: `\v` does not exist.]

  - So what is the hack?  Whatever computer system you're on has some
    character encoding scheme --- figure out the decimal and return that.
  
  - Why not leave it at that?  It's a magic number.  Doesn't work with other
    encodings.

  - Then compile with `\v`.
  - What's odd?  [Keep the binary: it knows `\v`.   Compile the source
    with it.  ***The compiler now defines `\v` in terms of itself***
    Very cool: circular definitions work with computer stuff.  Kind of 
    an odd induction.]

  - How does it matter that this is ascii?  [2.3: hardcode the value of 
    `\v` as 11, can then compile 2.2]

  - Where is the trojan horse?  [In the compiler]
  - How actually match?  In the parse tree or whitespace removed source?

### Code

For this lab you will write code to implement Thompson's hack.  We break
it down in four stages.

#### Part 1: self-reproducing program.

Follow the [README.md](code/step1/README.md) `code/step1` and implement
`string-to-char-array.c` such that you can generate a self-reproducing
program based on `Figure 1` in Thompson's paper.  This will hopefully
shake out any ambiguity in the paper.

#### Part 2: inject an attack into login and compiler

We now start writing a simple version of the compiler code injection
attack Thompson described.

Follow the [README.md](code/step2/README.md) in `code/step2` and
modify `trojan-compiler.c` so that it will perform simple attacks on
the trivial login program (`step2/login.c`) and the identity compiler
(`step2/compiler.c`).

#### Part 3: inject an attack that will inject an attack into the compiler.

Finally, we use the trick from Part 1 to inject a self-replicating attack
into the compiler.

Follow the [README.md](code/step3/README.md) in `code/step3` and
combine the code from Part 1 and Part 2 so that your trojan compiler
(`trojan-compiler.c`) will take a clean, virgin copy of the "system"
compiler `compiler.c` and inject a self-replicating copy of its trojan
attack into it.

#### Postscript

You have now replicated Thompon's hack.  Startlingly, there seem to be
only a few people that have ever done so, and most that believe they
understand the paper woulnd't actually be able to write out the code.
You can probably really stand out at parties by explaining what you did.
