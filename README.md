# Lab: System Call Overhead

## Setup


```bash
sudo apt-get update
sudo apt-get install build-essential strace -y
```


## How to Run the Experiments


###  Measuring Fast System Call Overhead

This test compares a vDSO call against a direct system call without any sleep involved.

1.  **Compile the program** in the `no_sleep` configuration:

```bash
make no_sleep
```

2.  **Run the tests**:

```bash
# Run the default version using the vDSO optimization
make run_vdso

# Run the version that forces a real system call
make run_novdso
```


###  Measuring Context Switch Overhead

This test compiles the program with a `nanosleep` call to measure the impact of a potential context switch.

1.  **Compile with a 1ms sleep**:

```bash
make sleep1
```

2.  **Run the 1ms sleep tests**:

```bash
make run_vdso
make run_novdso
```

3.  **Compile with a 10ms sleep** (optional):

```bash
make sleep10
```

4.  **Run the 10ms sleep tests**:

```bash
make run_vdso
make run_novdso
```


## Cleanup

To remove the compiled program (`busy`) and the shared library (`disable-vdso.so`):

```bash
make clean
```
