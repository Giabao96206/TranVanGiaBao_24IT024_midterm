#!/usr/bin/env bash
set -e

printf '\n===== 1. Build project =====\n'
make clean
make

printf '\n===== 2. Basic ls =====\n'
./myls

printf '\n===== 3. Show hidden files (-a) =====\n'
./myls -a

printf '\n===== 4. Long format (-l) =====\n'
./myls -l

printf '\n===== 5. Human-readable size (-lh) =====\n'
./myls -lh

printf '\n===== 6. Recursive (-R) =====\n'
./myls -R demo

printf '\n===== 7. Combined options (-alhR) =====\n'
./myls -alhR demo

printf '\n===== 8. Symlink =====\n'
./myls -l demo/dir1/link.txt

printf '\n===== 9. Error handling =====\n'
if ./myls does_not_exist; then
    echo "Unexpected success"
else
    echo "Error handled correctly (non-zero exit status)."
fi

printf '\n===== Demo finished =====\n'
