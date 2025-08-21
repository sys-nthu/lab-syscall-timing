// for xv6
// Copy this `timing_nothing.c` into `user`, and add to xv6's `Makefile`
#include "kernel/types.h"
#include "user/user.h"

#define N 10000

#ifndef S
#define S 1  // Default: sleep for 1 tick
#endif

volatile int times[N];
volatile int sum = 0;
volatile int max = 0;

int
main(int argc, char *argv[])
{
    int i;
    int start, end;

    for (i = 0; i < N; i++) {
        start = uptime();
#ifdef MEASURE_SLEEP
        sleep(S);  // Intentional sleep to induce context switch
#endif
        end = uptime();
        times[i] = end - start;
    }

    // Calculate adjusted time (subtract sleep duration)
    for (i = 0; i < N; i++) {
#ifdef MEASURE_SLEEP
        if (times[i] > S) times[i] -= S; 
        else times[i] = 0; // sleep might be delayed, but in practice never happen.
#endif
      sum += times[i];
    }

    int avg = sum / N;
    int avg_125 = 5 * avg;
    avg_125 = avg_125 / 4;
    // Count samples more than 25% greater than average
    int count_above_125 = 0;

    for (i = 0; i < N; i++) {
        if (times[i] > max)
            max = times[i];
        if (times[i] > avg_125)
            count_above_125++;
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

    int prcn = 10000 * count_above_125;
    prcn = prcn / N;

    printf("25%% greater than average: %d (%d.%d%%)\n",
           count_above_125,
           prcn / 100, prcn % 100);
        
    exit(0);
}