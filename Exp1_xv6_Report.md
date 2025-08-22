# 結果

以下皆為將 `w_stimecmp(r_time() + 1000000)`改為 `w_stimecmp(r_time() + 100)`之後的結果，各個操作變因測三次：

<details><summary>MEASURE_SLEEP disabled (measuring plain system call overhead)</summary>

MEASURE_SLEEP disabled (measuring plain system call overhead)
total samples: 10000
average overhead ticks: 6
max overhead ticks: 44
25% greater than average: 2955 (29.55%)

MEASURE_SLEEP disabled (measuring plain system call overhead)
total samples: 10000
average overhead ticks: 6
max overhead ticks: 42
25% greater than average: 2858 (28.58%)

MEASURE_SLEEP disabled (measuring plain system call overhead)
total samples: 10000
average overhead ticks: 6
max overhead ticks: 44
25% greater than average: 2836 (28.36%)</details>

<details><summary>MEASURE_SLEEP enabled (S = 1 ticks)</summary>

MEASURE_SLEEP enabled (S = 1 ticks)
total samples: 10000
average overhead ticks: 13
max overhead ticks: 66
25% greater than average: 2698 (26.98%)

MEASURE_SLEEP enabled (S = 1 ticks)
total samples: 10000
average overhead ticks: 13
max overhead ticks: 57
25% greater than average: 2477 (24.77%)

MEASURE_SLEEP enabled (S = 1 ticks)
total samples: 10000
average overhead ticks: 12
max overhead ticks: 62
25% greater than average: 2807 (28.7%)</details>

<details><summary>MEASURE_SLEEP enabled (S = 10 ticks)</summary>

MEASURE_SLEEP enabled (S = 10 ticks)
total samples: 10000
average overhead ticks: 13
max overhead ticks: 71
25% greater than average: 2517 (25.17%)

MEASURE_SLEEP enabled (S = 10 ticks)
total samples: 10000
average overhead ticks: 13
max overhead ticks: 60
25% greater than average: 2647 (26.47%)

MEASURE_SLEEP enabled (S = 10 ticks)
total samples: 10000
average overhead ticks: 13
max overhead ticks: 58
25% greater than average: 2642 (26.42%)</details>

<details><summary>MEASURE_SLEEP enabled (S = 100 ticks)</summary>

MEASURE_SLEEP enabled (S = 100 ticks)
total samples: 10000
average overhead ticks: 13
max overhead ticks: 62
25% greater than average: 2698 (26.98%)

MEASURE_SLEEP enabled (S = 100 ticks)
total samples: 10000
average overhead ticks: 14
max overhead ticks: 93
25% greater than average: 2952 (29.52%)

MEASURE_SLEEP enabled (S = 100 ticks)
total samples: 10000
average overhead ticks: 13
max overhead ticks: 73
25% greater than average: 2912 (29.12%)</details>

Exp 1-1:

- What is the minimum overhead? What does it represent?
  
  A: minimum overhead 是 0，這只代表 `timing_nothing.c` 在 tick 未遞增下成功完成兩次`uptime()`，timer interrupt 的更新頻率不管是 0.1 秒 還是 10^-5 秒他都會是 0

- Why is the overhead not constant?
  
  What might cause some iterations to take longer than others?
  
  A: 這個實驗必須將 timer interrupt 頻率拉到非常高 (原先設定的一萬倍) 才能觀察到有意義的數據，否則測出來的 overhead 會全部都是 0，但將 timer interrupt 的頻率拉到非常高的話會造成非常大量的 interrupt，而由於 xv6 在發生 timer interrupt 的時候會主動將 process `yield()` 讓出來，但同時 xv6 的 scheduler 又並不如 Round-robin 般是 non-preemptive scheduling，也就是 xv6 的 schedule 方法並不保證 process 一定會在固定時間內被執行，同時大量 timer interrupt 也造成大量 context switch，這些原因互相導致了以上這兩個問題的現象產生。

Exp 1-2:

- How does the adjusted average overhead change as `S` increases?
  
  A: 並無明顯改變，只有 MEASURE_SLEEP DISABLED 跟 MEASURE_SLEEP ENABLED 才有明顯差異

- Why might some measurements still be much larger than expected?
  
  A: `sleep()` 也是 system call，本身就會有 system call 的 overhead，而且 sleep() 本身也會執行`sched()`導致 context switch 產生

- Can you observe a clear overhead due to sleep-induced context switches?
  
  A: 可以，從 average overhead 變高很明顯可以看得出來
