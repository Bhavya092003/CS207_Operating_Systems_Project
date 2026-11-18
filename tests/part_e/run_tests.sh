
#!/bin/bash

set -euo pipefail

PROGRAM="./bin/memory_disk"
TEST_DIR="tests/part_e"
OUTPUT_DIR="outputs/part_e"

mkdir -p "$OUTPUT_DIR"

run_test() {
    local test_name="$1"
    local expected_fifo="$2"
    local expected_lru="$3"
    local expected_fcfs="$4"
    local expected_sstf="$5"

    local output_file="$OUTPUT_DIR/${test_name}_output.txt"

    echo "Running $test_name..."

    "$PROGRAM" < "$TEST_DIR/${test_name}.txt" > "$output_file"

    grep -q "Total page faults (FIFO): $expected_fifo" "$output_file"
    grep -q "Total page faults (LRU): $expected_lru" "$output_file"
    grep -q "Total head movement (FCFS): $expected_fcfs" "$output_file"
    grep -q "Total head movement (SSTF): $expected_sstf" "$output_file"

    echo "$test_name: PASSED"
    echo "  FIFO page faults: $expected_fifo"
    echo "  LRU page faults: $expected_lru"
    echo "  FCFS head movement: $expected_fcfs"
    echo "  SSTF head movement: $expected_sstf"
    echo
}

run_test "test1" 10 9 640 236
run_test "test2" 10 8 240 224

echo "All Part E tests passed successfully."
