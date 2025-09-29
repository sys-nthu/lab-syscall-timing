CC = gcc
CFLAGS = -O2 -Wall
TARGET = timing_nothing_linux ping-00 ping-01 ping
SRC = timing-nothing-linux.c
PRELOAD = disable-vdso.so

.PHONY: clean run_vdso run_novdso perf_run perf_run_novdso 


all: $(TARGET) 

no_sleep: $(SRC) $(PRELOAD)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

sleep1: $(SRC) $(PRELOAD)
	$(CC) $(CFLAGS) -DMEASURE_SLEEP -DS=1 -o $(TARGET) $(SRC)

sleep10: $(SRC) $(PRELOAD)
	$(CC) $(CFLAGS) -DMEASURE_SLEEP -DS=10 -o $(TARGET) $(SRC)

run_vdso:
	@echo "Running with VDSO (default)"
	./$(TARGET)

run_novdso:
	@echo "Running without VDSO (forced syscall fallback)"
	LD_PRELOAD=./$(PRELOAD) ./$(TARGET)

perf_run:
	sudo perf record -e sched:sched_switch -a -- ./$(TARGET)

perf_run_novdso:
	sudo LD_PRELOAD=./$(PRELOAD) perf record -e sched:sched_switch -a -- ./$(TARGET)


$(PRELOAD): disable-vdso.c
	$(CC) -Wall -fPIC -shared -o $(PRELOAD) disable-vdso.c -ldl

ping-00: pipe.c
	$(CC) $(CFLAGS) pipe.c -o $@

ping-01: pipe.c
	$(CC) $(CFLAGS) pipe.c -o $@ -DPARENT_CPU=0 -DCHILD_CPU=1

ping: pipe.c
	$(CC) $(CFLAGS) pipe.c -o $@ -DUSE_AFFINITY=0

clean:
	rm -f $(TARGET) $(PRELOAD) perf.data
