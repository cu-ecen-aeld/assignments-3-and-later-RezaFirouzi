#!/bin/bash

# This script checks for the ARM pre-built cross compile toolchain and generates a report
# It assumes that the PATH environment variable has been updated with the toolchain location

# Exit immediately if a command exits with a non-zero status
set -e

FILEPATH="assignments/assignment2"
FILENAME="cross-compile.txt"
COMPILER="aarch64-none-linux-gnu-gcc"

# Create the target directory (removes if exists, ensures no errors)
rm -rf "$FILEPATH"
mkdir -p "$FILEPATH"

# Move into the directory
cd "$FILEPATH"

# Check if the cross-compiler is available
if ! command -v $COMPILER &> /dev/null
then
  echo  "Error: $COMPILER not found in PATH. Please check your toolchain installation."
  exit 1
fi

{
  echo "Cross-compile toolchain report:"
  echo "==============================="
  echo
  echo "Sysroot path:"
  $COMPILER -print-sysroot
  echo
  echo "Compiler version and configurations:"
} >> "$FILENAME"

# Append the verbose configuration output, redirecting both stdout and stderr
$COMPILER -v >> "$FILENAME" 2>&1


echo "Report generated successfully at: $FILEPATH/$FILENAME"
