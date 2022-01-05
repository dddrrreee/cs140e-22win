# Async Programming in C

## Background

If you've done backend web development recently, you've probably run into code 
which looks like this:

```python
import asyncio

async def main():
    print('hello')
    await asyncio.sleep(1)
    print('world')

asyncio.run(main())
```
[example from docs.python.org](https://docs.python.org/3/library/asyncio-task.html).

This "async" (asynchronous) model of programming lets programs multitask 
without running multiple threads, and therefore without all the pain of 
multithreaded programming (locks, race conditions, etc.).  Many async functions 
can be launched together, where they're all doing different things ostensibly 
"at the same time", but somehow they magically seem to provide the benefits of 
multiple threads without the overhead of setting up multiple OS threads.

What's actually happening behind the scenes (usually) is that the language 
runtime is doing _cooperative_ multithreading automatically.  The problems we 
often associate with OS threads (like race conditions) are actually problems 
that arise from preemption---this is why we saw the same problems pop up when 
we were doing interrupts, even though we didn't have multiple threads yet.  
Preemptive multithreading is what most major OSes provide for us; cooperative 
multithreading is much simpler, but usually implemented at the language level.

When you have a lot different tasks that each involve a lot of waiting (e.g., 
I/O, sleeping), cooperative threads can be a good fit.  Each thread knows 
exactly when it's going to be wasting time busy-looping, and can instead yield 
the processor to a different thread.  The reason we don't use this at the OS 
level is that one program which doesn't yield properly can force the entire OS 
to freeze.  Expensive computations will stop everything else on the processor, 
and even a single accidental `while (1);` loop in any program could force a 
reboot.  Cooperative threading is therefore usually implemented by language 
runtimes, so if a program misbehaves it's hurting no one except itself.

## In C Code

For our purposes, you could imagine something like this pseudo-C:

```C
async void blink() {
    int level = 0;
    while (1) {
        level = 1 - level;
        gpio_write(21, level);
        await sleep_ms(500);
    }
}

async void hello() {
    while (1) {
        await uart_puts('hello, world');
        await sleep_ms(1000);
    }
}

void notmain() {
    async_start(&blink);
    async_start(&hello);
    async_join_all();
    clean_reboot();
}
```

Once we implement cooperative threads in Lab 9, the pseudo-C above could be 
translated into the following:

```C
void blink() {
    int level = 0;
    while (1) {
        level = 1 - level;
        gpio_write(21, level);
        await_sleep_ms(500);
    }
}

void hello() {
    while (1) {
        await_puts('hello, world');
        await_sleep_ms(1000);
    }
}

void notmain() {
    rpi_fork(&blink, NULL);
    rpi_fork(&hello, NULL);
    thread_join_all();
    clean_reboot();
}
```

Note how similar this is to the original pseudo-C, even though C doesn't 
provide any async support (or any meaningful runtime).  Languages like 
JavaScript and Python need async to be built into the language, but we can get 
roughly the same result with much less work since we have full control of the 
processor.

The `await_*` functions called above can be implemented like this:
```C
void await_delay_ms(unsigned ms) {
    // There's a smarter way to do this which we'll see in lab
    unsigned us = ms * 1000000;
    unsigned rb = timer_get_usec();
    while (1) {
        unsigned ra = timer_get_usec();
        if ((ra - rb) >= us) {
            break;
        }
        rpi_yield();
    }
}

void await_uart_putc(char c) {
    while (!uart_can_putc()) {
        rpi_yield();
    }
    uart_putc(c);
}

void await_uart_puts(char *s) {
    for (;*s;s++) {
        await_uart_putc(*s);
    }
}
```

The big change here is that, every time we have a busy loop, we call the 
`yield()` system call.  Our kernel will save our thread state and switch over 
to another thread which is hopefully able to make progress.  We never waste 
time spinning in one thread; in fact, if we did, we'd block every other thread.  

Since we spend most of our time in both threads waiting, we'll still be okay on 
timing, but this will reduce our accuracy compared to only having one thread.  
You can try implementing something similar with an async version of your 
`test-gen` running in one thread and see how much accuracy we actually lose 
with cooperative threading over single-threading.

This is, of course, a very rudimentary form of async.  A true async executor 
would only resume a thread when it knew for sure that thread could make 
progress; each thread would therefore have to specify to the runtime a 
condition on which it ought to be resumed.  A simple example is for 
`delay_ms`---since we know the exact time at which the thread will start making 
progress again, the runtime could simply check if that time has passed already 
before swapping the thread's state onto the processor.

In general, cooperative threads tend to be useful within a single application 
where one programmer controls everything, and where you're not doing any heavy 
computation but are doing a lot of I/O.  They're not as good when you have hard 
timing constraints, computation-heavy workloads, or untrusted programs; for 
that, we'll build preemptive threads instead.
