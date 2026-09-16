# Entity simulator benchmark

This folder is an isolated benchmark executable. It links the production simulator sources directly, but does not modify or run `src/main.cpp`.

## Build and run

From `entity_simulator/benchmark`:

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build --config Release
.\build\entity_simulator_benchmark.exe --entities 1000 --ticks 2000 --events 2000
```

The output is deliberately key/value based so the values can be copied into a resume. `stable=true` means the complete workload finished without an exception or failed invariant. Increase `--entities` until the workload no longer meets the chosen stability budget, then report the largest passing value. The benchmark reports a median across the requested samples.

## Valgrind

For a Linux or WSL build matching the `Custom_ML/build_linux` workflow:

```bash
cmake -S . -B build_linux \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build_linux --parallel
```

Run the Linux executable under Valgrind Memcheck:

```bash
valgrind --tool=memcheck \
  --leak-check=full \
  --show-leak-kinds=all \
  --error-exitcode=99 \
  --log-file=build_linux/valgrind.log \
  ./build_linux/entity_simulator_benchmark \
  --entities 1000 --ticks 2000 --events 2000
```

For the measured 1,000-Autobot workload, Memcheck reported:

```text
total heap usage: 26,651,029 allocs, 26,651,029 frees, 690,358,612 bytes allocated
in use at exit: 0 bytes in 0 blocks
All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors from 0 contexts
```

Use Valgrind's `total heap usage` line as the allocation count. The Windows MinGW executable is not a substitute for this native Linux/WSL run.