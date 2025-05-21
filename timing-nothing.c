// for xv6
// Copy this `timing_nothing.c` into `user`, and add to xv6's `Makefile`
#include "kernel/types.h"
#include "user/user.h"

#define N 10000

#ifndef S
#define S 1  // Default: sleep for 1 tick
#endif

int
main(int argc, char *argv[])
{
    int times[N];
    int i;
    uint start, end;

    for (i = 0; i < N; i++) {
        start = uptime();
#ifdef MEASURE_SLEEP
        sleep(S);  // Intentional sleep to induce context switch
#endif
        end = uptime();
        times[i] = end - start;
    }

    // Calculate adjusted time (subtract sleep duration)
    long sum = 0;
    for (i = 0; i < N; i++) {
#ifdef MEASURE_SLEEP
        if (times[i] > S) times[i] -= S; 
        else times[i] = 0; // sleep might be delayed, but in practice never happen.
#endif
      sum += times[i];
    }

    uint avg = (unsigned int)((float)sum / N);
    uint avg_p25 = 1.25 * avg;
    // Count samples more than 25% greater than average
    uint count_above_p25 = 0;
    uint max = 0;
    for (i = 0; i < N; i++) {
        if (times[i] > max)
            max = times[i];
        if (times[i] > avg_p25)
            count_above_p25++;
    }

    // Output results
#ifdef MEASURE_SLEEP
    printf("MEASURE_SLEEP enabled (S = %d ticks)\n", S);
#else
    printf("MEASURE_SLEEP disabled (measuring plain system call overhead)\n");
#endif
    printf("total samples: %d\n", N);
    printf("average overhead ticks: %u\n", avg);
    printf("max overhead ticks: %u\n", max);
    printf("25% greater than average: %d (%.2f%%)\n",
           count_above_125,
           100.0f * count_above_p25 / N);

    exit(0);
}
