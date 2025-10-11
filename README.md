# Lab: Timing Nothing – Analyzing System Call Overhead

In this lab, you will explore how long it takes to do **nothing** (a little better than bitcoin mining).  Specifically, you will measure how much time is spent between two calls to a timing function when no meaningful work is done in between.
You will run this experiment on: **xv6-riscv** and **Linux x86-64** (with and without `vdso`).

You will learn:

* The overhead of entering and exiting the kernel.
* Context switch behavior.
* The variability introduced by interrupts and scheduling.

## Part 1: Timing on xv6-riscv

### Experiment 1-1: Do nothing

A starter program is provided in the lab repository. It performs the following:

1. Calls `uptime()` twice in a loop of 10000 iterations.
2. Measures the difference in tick counts returned by the system calls.
3. Calculates: the average overhead, the maximum observed overhead, and the percentage of samples that are more than 25% above the average.

Run the provided program as-is, and answer the following questions.

#### Questions

* What is the minimum overhead? What does it represent?
* Why is the overhead not constant?
* What might cause some iterations to take longer than others?

### Experiment 1-2: Do nothing + sleep

Recompile the same program (don't forget to `make clean` first) using the `MEASURE_SLEEP` flag to introduce an intentional sleep between the two system calls (`-DS=1` will set the `S` macro to 1, this is the number of tick to sleep):

```bash=
make CPPFLAGS='-DMEASURE_SLEEP -DS=1' qemu
```

Then set a higher tick:

```bash=
make CPPFLAGS='-DMEASURE_SLEEP -DS=10' qemu
```

1. How does the adjusted average overhead change as `S` increases?
2. Why might some measurements still be much larger than expected?
3. Can you observe a clear overhead due to sleep-induced context switches?

## Part 2: Timing on Linux x86 (with and without `vdso`)

Now we will continue our investigation of system call overhead, this time on a Linux system. Unlike xv6, Linux implements **fast paths** for many common system calls using a mechanism called **VDSO (Virtual Dynamic Shared Object)**.

You will explore the overhead of calling `clock_gettime(CLOCK_MONOTONIC)` under two different settings:

1. **With VDSO (default)**: the syscall is converted into a user-space memory read.
2. **Without VDSO**: we force the system to execute a full syscall using `LD_PRELOAD` and a custom override.

You will also use the Linux tool `perf` to **observe scheduler behavior**, especially when delays (`nanosleep`) are introduced.

### Experiment 2-1: Timing `clock_gettime()` back-to-back

Build and run the base timing program using both the VDSO-optimized and raw-syscall versions:

```sh
make no_sleep
make run_vdso
make run_novdso
```

This will run the same program:

* Once with the **default VDSO-enabled** `clock_gettime()` (fast path),
* Once with **VDSO disabled** (slow path using syscall fallback).

#### Questions

* What is the **average time per call** with VDSO? Without VDSO?
* What is the **factor of difference** in overhead? (e.g., 3x, 10x?)
* How consistent is the latency for VDSO calls vs syscall fallback?  Which version exhibits **higher variance**, and why?
* Use `strace` to check if syscall is truly gone?

```shell=
strace -e clock_gettime -f LD_PRELOAD=./disable-vdso.so ./timing_nothing_linux
strace -e clock_gettime -f ./timing_nothing_linux
```

### Experiment 2-2: do nothing + sleep

Now compile and run with sleep inserted between the two timing calls:

```sh
make sleep1
make run_vdso

make sleep10
make run_vdso
```

#### Questions

* After subtracting the known sleep duration, what is the remaining overhead? Is it consistent? How does it scale with longer sleep durations?
* Do outliers become more frequent with longer sleeps?
* Is the wakeup timing consistent?

### Experiment 2-3: Observing Context Switches with `perf`

We will use the Linux `perf` tool to observe **scheduler activity** during program execution:

```sh
make perf_run
make perf_run_novdso
```

Then, run `perf report`. [Here's an example of what you could see](https://en.ittrip.xyz/c-language/c-perf-context-switches#index_id11).

### Questions

* How many context switches occur during your run? Can you use `time` to measure the overall running time and find out the *number of context switch per second*?
* Can you infer whether `nanosleep()` puts the process to sleep or spins?

Excellent idea — introducing external interference using `stress` will help students understand how **system load** affects syscall latency, context switching, and scheduler behavior. It adds real-world unpredictability and sharpens their ability to analyze timing variance.

Here’s how you can incorporate that into the lab:

---

### Experiment 2-4: Interference with `stress`

In this experiment, you'll deliberately **introduce CPU or memory contention** to observe how system load affects timing overhead.

#### Step 1: Install `stress`

```sh
sudo apt-get update && sudo apt-get install -y stress
```

#### Step 2: Run `stress` in the Background

Open a **separate terminal**, and run:

```sh
stress --cpu 2 --timeout 20
```

This creates 2 busy loops that consume CPU for 20 seconds.

#### Step 3: Run Your Timing Program During Interference

In your original terminal, run:

```sh
make run_vdso
make run_novdso
make sleep1
make raw_sleep1
```

Compare the results you observe **with and without `stress` running**.

### Questions

* How does the timing variability change under CPU stress?
* Does it affect syscall (`run_novdso`) more than VDSO (`run_vdso`)?
* For the sleep experiments (`sleep1`, `sleep10`), does `stress` introduce **more outliers**?
