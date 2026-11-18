#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$ROOT"

make part-c >/dev/null
BIN="$ROOT/bin/producer_consumer"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

pass() {
    printf 'PASS: %s\n' "$1"
}

fail() {
    printf 'FAIL: %s\n' "$1" >&2
    exit 1
}

run_and_check() {
    local count="$1"
    local output="$TMP/run_${count}.out"
    local produced
    local consumed

    timeout 20s "$BIN" "$count" > "$output" || fail "program completes with $count items"
    grep -Fq "Verification: PASS - produced $count items and consumed each exactly once." "$output" || fail "exactly-once verification for $count items"
    grep -Fq 'Buffer bounds: PASS - capacity 5, no overflow or underflow.' "$output" || fail "buffer bounds verification for $count items"

    produced="$(grep -Ec '^Producer [0-9]+ produced item [0-9]+' "$output" || true)"
    consumed="$(grep -Ec '^Consumer [0-9]+ consumed item [0-9]+' "$output" || true)"

    [[ "$produced" -eq "$count" ]] || fail "exactly $count producer log entries"
    [[ "$consumed" -eq "$count" ]] || fail "exactly $count consumer log entries"

    pass "$count items produced and consumed exactly once"
}

run_and_check 100
run_and_check 1000

if timeout 20s "$BIN" > "$TMP/default.out"; then
    grep -Fq 'Verification: PASS - produced 100 items and consumed each exactly once.' "$TMP/default.out" || fail 'default item count is 100'
    pass 'default execution uses 100 items'
else
    fail 'default execution completes'
fi

if "$BIN" 99 > /dev/null 2>&1; then
    fail 'item counts below 100 are rejected'
fi
pass 'item counts below 100 are rejected'

if "$BIN" 1000001 > /dev/null 2>&1; then
    fail 'item counts above the supported maximum are rejected'
fi
pass 'item counts above the supported maximum are rejected'

if "$BIN" abc > /dev/null 2>&1; then
    fail 'non-numeric item counts are rejected'
fi
pass 'non-numeric item counts are rejected'

if "$BIN" 100 200 > /dev/null 2>&1; then
    fail 'extra command-line arguments are rejected'
fi
pass 'extra command-line arguments are rejected'

printf '\nAll Part C tests passed.\n'
