#!/bin/bash

# This script assumes that the ARM pre-built cross compile toolchain has already been installed on your Linux machine
# The PATH environment variable has also already been updated with the path where the toolchain is located

set -e

FILEPATH="assignments/assignment2/"
FILENAME="cross-compile.txt"

rm -rf "$FILEPATH"
mkdir -p "$FILEPATH"
cd "$FILEPATH"

echo -e "Compiler version:" >> "$FILENAME"
aarch64-none-linux-gnu-gcc --version >> "$FILENAME" 2>&1
echo -e "Sysroot path:" >> "$FILENAME"
aarch64-none-linux-gnu-gcc -print-sysroot >> "$FILENAME" 2>&1
echo -e "\nConfigurations:" >> "$FILENAME"
aarch64-none-linux-gnu-gcc -v >> "$FILENAME" 2>&1
