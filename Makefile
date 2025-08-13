CC = gcc
CFLAGS = -O2 -Wall
TARGET = timing_nothing_linux
SRC = timing-nothing-linux.c
PRELOAD = disable-vdso.so

.PHONY: clean run_vdso run_novdso perf_run perf_run_novdso


all: $(TARGET)

no_sleep: $(SRC) 
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

sleep1: $(SRC)
	$(CC) $(CFLAGS) -DMEASURE_SLEEP -DS=1 -o $(TARGET) $(SRC)

sleep10: $(SRC)
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


# Build the disable-vdso shim
$(PRELOAD): disable-vdso.c
	$(CC) -Wall -fPIC -shared -o $(PRELOAD) disable-vdso.c -ldl

clean:
	rm -f $(TARGET) $(PRELOAD) perf.data
