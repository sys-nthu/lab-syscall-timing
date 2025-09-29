// pipe.c
// Build (default: both on CPU0):
//   gcc -O2 -Wall -Wextra -std=c11 pipe.c -o ping-00
// Build (parent->CPU0, child->CPU1):
//   gcc -O2 -Wall -Wextra -std=c11 pipe.c -o ping-01 \
//       -DPARENT_CPU=0 -DCHILD_CPU=1
// Disable pinning:
//   gcc -O2 -Wall -Wextra -std=c11 pipe.c -o ping -DUSE_AFFINITY=0

#define _GNU_SOURCE
#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#ifndef LOOPS
#define LOOPS 100000
#endif

#ifndef USE_AFFINITY
#define USE_AFFINITY 1
#endif

#ifndef PARENT_CPU
#define PARENT_CPU 0
#endif

#ifndef CHILD_CPU
#define CHILD_CPU 0
#endif

static void die(const char *msg) { perror(msg); exit(EXIT_FAILURE); }

static uint64_t ns_now(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) die("clock_gettime");
    return (uint64_t)ts.tv_sec * 1000000000ull + ts.tv_nsec;
}

static void pin_to_cpu(int cpu) {
#if USE_AFFINITY
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);
    if (sched_setaffinity(0, sizeof(set), &set) != 0) die("sched_setaffinity");
#else
    (void)cpu;
#endif
}

int main(void) {
    int p2c[2], c2p[2];
    if (pipe(p2c) < 0) die("pipe p2c");
    if (pipe(c2p) < 0) die("pipe c2p");

    pin_to_cpu(PARENT_CPU);
    pid_t pid = fork();
    if (pid < 0) die("fork");

    if (pid == 0) {
        // child
        pin_to_cpu(CHILD_CPU);
        close(p2c[1]); close(c2p[0]);

        int token;
        for (int i = 0; i < LOOPS; i++) {
            if (read(p2c[0], &token, sizeof(token)) != sizeof(token)) die("child read");
            token++;
            if (write(c2p[1], &token, sizeof(token)) != sizeof(token)) die("child write");
        }
        _exit(0);

    } else {
        // parent
        close(p2c[0]); close(c2p[1]);

        int token = 0;
        uint64_t t0 = ns_now();

        for (int i = 0; i < LOOPS; i++) {
            if (write(p2c[1], &token, sizeof(token)) != sizeof(token)) die("parent write");
            if (read(c2p[0], &token, sizeof(token)) != sizeof(token)) die("parent read");
        }

        uint64_t t1 = ns_now();
        wait(NULL);

        double total_ns = (double)(t1 - t0);
        printf("# %d pipe round-trips (parent->child->parent)\n", LOOPS);
        printf("# Affinity: parent->CPU%d, child->CPU%d, USE_AFFINITY=%d\n",
               PARENT_CPU, CHILD_CPU, USE_AFFINITY);
        printf("  Total time : %.3f sec\n", total_ns / 1e9);
        printf("  ns per rtrip: %.2f\n", total_ns / LOOPS);
        printf("  rtrips/sec : %.0f\n", 1e9 / (total_ns / LOOPS));
    }
}
