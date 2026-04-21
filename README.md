# performance_analyzer
## ReLU Branch vs Branchless Benchmark

This repository contains a small Linux microbenchmark for comparing:

- branch-based ReLU
- branchless bitwise ReLU

It also includes perf-based measurements of:

- cycles
- instructions
- branches
- branch-misses

Environment:
- Ubuntu 24.04
- GCC 13.3.0
- x86_64
