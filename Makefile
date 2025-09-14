CC = gcc
CFLAGS = -O2 -Wall
TARGET = busy
SRC = busy.c
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
	@echo "Running with vDSO"
	./$(TARGET)

run_novdso:
	@echo "Running without vDSO"
	LD_PRELOAD=./$(PRELOAD) ./$(TARGET)

$(PRELOAD): disable-vdso.c
	$(CC) -Wall -fPIC -shared -o $(PRELOAD) disable-vdso.c -ldl

clean:
	rm -f $(TARGET) $(PRELOAD) perf.data
