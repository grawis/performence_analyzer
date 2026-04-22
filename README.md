# performance_analyzer

This repository collects small benchmarking programs for different homework questions.
Each question is documented independently so that more experiments can be added in the future without changing the overall structure.

## Environment

- OS: Ubuntu 24.04
- Compiler: GCC 13.3.0
- Platform: x86_64
- Tools: `perf`, `taskset`

---

## Homework 1 - Q2

### Topic

Temporal locality and memory access pattern analysis with linked-list traversal.

### Related file

- `two_pass_vs_fast_slow_pointer.c`

### Description

This program compares two linked-list traversal strategies:

- `fast/slow pointer`
- `two-pass traversal`

The goal is to observe how different traversal patterns affect cache behavior and memory access efficiency.

### Build

    gcc -O2 -o bench two_pass_vs_fast_slow_pointer.c

### Run

    ./bench <N> <mode> <repeat>

- `N`: number of nodes
- `mode = 0`: fast/slow pointer
- `mode = 1`: two-pass traversal
- `repeat`: number of repeated runs

Example:

    ./bench 1000000 0 50
    ./bench 1000000 1 50

### perf analysis

Use `perf stat` to compare cache and instruction behavior:

    taskset -c 0 perf stat -e cache-references,cache-misses,cycles,instructions ./bench 1000000 0 50
    taskset -c 0 perf stat -e cache-references,cache-misses,cycles,instructions ./bench 1000000 1 50

Use `perf record` and `perf report` for hotspot analysis:

    taskset -c 0 perf record -o fs_rec.data -e cache-misses -g -- ./bench 1000000 0 50
    taskset -c 0 perf record -o tp_rec.data -e cache-misses -g -- ./bench 1000000 1 50
    perf report -i fs_rec.data --stdio > fs_report.txt
    perf report -i tp_rec.data --stdio > tp_report.txt

### Purpose

This experiment is used to compare the cache behavior of different linked-list traversal methods and identify where cache misses are concentrated.

---

## Homework 4 - Q2

### Topic

Performance comparison between branch-based and branchless ReLU implementations.

### Related files

- `relu_bench3.c`
- `run_perf3.sh`

### Description

This benchmark compares two ways to implement ReLU:

- `branch version`
- `branchless version`

The purpose is to study whether branchless code performs better under different input distributions, especially when branch prediction becomes less effective.

### Build

    gcc -O2 -o relu_bench3 relu_bench3.c
    chmod +x run_perf3.sh

### Run

    ./relu_bench3 [branch|branchless] [mostly_pos|mostly_neg|random50]

Examples:

    ./relu_bench3 branch mostly_pos
    ./relu_bench3 branchless mostly_pos
    ./relu_bench3 branch random50
    ./relu_bench3 branchless random50

### perf analysis

Measure execution behavior with:

    perf stat -r 5 -e cycles,instructions,branches,branch-misses ./relu_bench3 branch random50
    perf stat -r 5 -e cycles,instructions,branches,branch-misses ./relu_bench3 branchless random50

Or run the helper script:

    ./run_perf3.sh

Results will be saved in:

    perf_results3.txt

### Purpose

This experiment evaluates the impact of branch prediction on ReLU performance and compares branch-based and branchless implementations using hardware performance counters.

---

## Notes

- Each homework question is written as an independent section.
- New benchmark programs can be added later using the same format.
- For fair comparison, it is recommended to bind execution to a single CPU core with `taskset` when measuring performance.
