#!/usr/bin/env bash
# group-86 | MT26150 Abhishek Kumar Singh | MT26203 Abhay Sahani

set -e

make -C src

PASS=0
TOTAL=0

for infile in tests/T*.in; do
    TOTAL=$((TOTAL + 1))
    base="${infile%.in}"
    outfile="${base}.out"
    actual=$(./src/acaderp < "$infile")
    expected=$(cat "$outfile")

    if [ "$actual" = "$expected" ]; then
        PASS=$((PASS + 1))
        echo "[PASS] $(basename "$infile")"
    else
        echo "[FAIL] $(basename "$infile")"
        diff -u <(echo "$expected") <(echo "$actual") || true
    fi
done

echo "Passed $PASS / $TOTAL tests."
if [ "$PASS" -ne "$TOTAL" ]; then
    exit 1
fi