#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define N 10000

#ifndef S
#define S 1  // Default: sleep for 1 millisecond
#endif

uint64_t get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

int main(int argc, char *argv[]) {
    uint64_t times[N];
    int i;
    uint64_t start, end;

    for (i = 0; i < N; i++) {
        start = get_time_ns();
#ifdef MEASURE_SLEEP
        struct timespec req = {
            .tv_sec = 0,
            .tv_nsec = S * 1000000L
        };
        nanosleep(&req, NULL);
#endif
        end = get_time_ns();
        times[i] = end - start;
    }

    // Subtract sleep duration (in ns) if needed
    uint64_t sleep_ns = (uint64_t)S * 1000000L;
    uint64_t sum = 0;
    for (i = 0; i < N; i++) {
#ifdef MEASURE_SLEEP
        if (times[i] > sleep_ns) times[i] -= sleep_ns;
        else times[i] = 0;
#endif
        sum += times[i];
    }

    uint64_t avg = sum / N;
    uint64_t avg_p25 = avg + avg / 4; // 1.25 * avg
    int count_above_p25 = 0;
    uint64_t max = 0;
    for (i = 0; i < N; i++) {
        if (times[i] > max)
            max = times[i];
        if (times[i] > avg_p25)
            count_above_p25++;
    }

#ifdef MEASURE_SLEEP
    printf("MEASURE_SLEEP enabled (S = %d ms)\n", S);
#else
    printf("MEASURE_SLEEP disabled (measuring plain system call overhead)\n");
#endif
    printf("total samples: %d\n", N);
    printf("average overhead (ns): %lu\n", avg);
    printf("max overhead (ns): %lu\n", max);
    printf("25%% greater than average: %d (%.2f%%)\n",
           count_above_p25,
           100.0f * count_above_p25 / N);

    return 0;
}
