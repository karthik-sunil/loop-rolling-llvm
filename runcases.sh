#!/bin/bash

# Clear the screen
clear

# Define the path to the pass library and the pass name
PATH2LIB="build/rollingpass/rollingpass.so"
PASS="loop-roll"
# CASES="stores"

# Define the output CSV filename
CSV_FILENAME="testcases/filesize_comparison.csv"

# Write the CSV headers
echo "\"Filename\",\"Base Size\",\"Rolled Size\"" > "$CSV_FILENAME"

# Iterate over each C file in the testcases directory
for BENCH in testcases/"${CASES}/"*.c; do
    # Extract the basename (filename without extension)
    BASENAME=$(basename "$BENCH" .c)

    # Compile the C file to LLVM IR (.ll)
    clang -emit-llvm -c "$BENCH" -Xclang -disable-O0-optnone -o "testcases/${BASENAME}.bc"
    # clang -emit-llvm -S "$BENCH" -Xclang -disable-O0-optnone -o "testcases/${BASENAME}.ll"

    # Apply the LLVM pass to the IR file and output to an optimized IR file (.bc)
    opt -load-pass-plugin="${PATH2LIB}" -passes="${PASS},dot-cfg" "testcases/${BASENAME}.bc" -o "testcases/${BASENAME}_opt.bc"
    # opt -load-pass-plugin="${PATH2LIB}" -S -passes="${PASS},dot-cfg" "testcases/${BASENAME}.ll" -o "testcases/${BASENAME}_opt.ll"
    
    # Calculate file sizes for the original and optimized .bc files
    BASE_SIZE=$(stat -c%s "testcases/${BASENAME}.bc")
    ROLLED_SIZE=$(stat -c%s "testcases/${BASENAME}_opt.bc")

    # Append the data to the CSV file
    echo "\"${BASENAME}\",${BASE_SIZE},${ROLLED_SIZE}" >> "$CSV_FILENAME"

    # Cleanup: remove intermediate bytecode files
    rm -f "testcases/${BASENAME}.bc" "testcases/${BASENAME}_opt.bc"
    rm -f "testcases/${BASENAME}.ll" "testcases/${BASENAME}_opt.ll"
done

# All done
echo "File size comparison has been written to $CSV_FILENAME"
