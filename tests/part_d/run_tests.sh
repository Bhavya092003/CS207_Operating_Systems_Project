
#!/bin/bash

set -e

echo "========================================"
echo " PART D - BANKER'S ALGORITHM TESTS"
echo "========================================"

mkdir -p outputs/part_d

if [ ! -x bin/banker ]; then
    echo "Error: banker executable not found."
    echo "Run 'make part-d' first."
    exit 1
fi

run_test() {
    local name="$1"
    local input="$2"
    local expected="$3"
    local output="outputs/part_d/${name}_output.txt"

    echo ""
    echo "Running ${name}..."

    ./bin/banker < "$input" | tee "$output"

    if grep -q "$expected" "$output"; then
        echo "PASS: ${name}"
    else
        echo "FAIL: ${name}"
        exit 1
    fi
}

run_test "safe" \
    "tests/part_d/safe.txt" \
    "SYSTEM IS IN A SAFE STATE"

run_test "unsafe" \
    "tests/part_d/unsafe.txt" \
    "SYSTEM IS IN AN UNSAFE STATE"

run_test "multiple_safe" \
    "tests/part_d/multiple_safe.txt" \
    "SYSTEM IS IN A SAFE STATE"

echo ""
echo "========================================"
echo " ALL PART D TESTS PASSED"
echo "========================================"
