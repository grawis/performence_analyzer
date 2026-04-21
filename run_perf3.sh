#!/bin/bash

OUT=perf_results3.txt

echo "CPU / compiler info" > "$OUT"
uname -a >> "$OUT"
gcc --version | head -n 1 >> "$OUT"
echo "N=20000000, REPEAT=30" >> "$OUT"
echo "" >> "$OUT"

echo "[1] branch + mostly_pos" >> "$OUT"
perf stat -r 5 -e cycles,instructions,branches,branch-misses ./relu_bench3 branch mostly_pos >> "$OUT" 2>&1
echo "" >> "$OUT"

echo "[2] branchless + mostly_pos" >> "$OUT"
perf stat -r 5 -e cycles,instructions,branches,branch-misses ./relu_bench3 branchless mostly_pos >> "$OUT" 2>&1
echo "" >> "$OUT"

echo "[3] branch + mostly_neg" >> "$OUT"
perf stat -r 5 -e cycles,instructions,branches,branch-misses ./relu_bench3 branch mostly_neg >> "$OUT" 2>&1
echo "" >> "$OUT"

echo "[4] branchless + mostly_neg" >> "$OUT"
perf stat -r 5 -e cycles,instructions,branches,branch-misses ./relu_bench3 branchless mostly_neg >> "$OUT" 2>&1
echo "" >> "$OUT"

echo "[5] branch + random50" >> "$OUT"
perf stat -r 5 -e cycles,instructions,branches,branch-misses ./relu_bench3 branch random50 >> "$OUT" 2>&1
echo "" >> "$OUT"

echo "[6] branchless + random50" >> "$OUT"
perf stat -r 5 -e cycles,instructions,branches,branch-misses ./relu_bench3 branchless random50 >> "$OUT" 2>&1
echo "" >> "$OUT"

echo "Done. Results saved to $OUT"
