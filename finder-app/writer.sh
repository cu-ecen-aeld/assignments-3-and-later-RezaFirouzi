#!/bin/sh

# Author: Reza Firouzi

set -e

# Check if at least two arguments are provided
if [ $# -lt 2 ]; then
  echo "Error: Two required arguments must be passed to the script!"
  echo "Usage: $0 <file_path> <string_to_write>"
  exit 1
fi

WRITEFILE="$1"
WRITESTR="$2"

# Extract directory path from the provided file path
DIRPATH=$(dirname "$WRITEFILE")

# Check if the directory exist, if not, create it
if [ ! -d "$DIRPATH" ]; then
  mkdir -p "$DIRPATH"
fi

# Write the string to the file and check if successful
if ! echo "$WRITESTR" > "$WRITEFILE"; then
  echo "Error: Failed to write to the file '$WRITEFILE'."
  exit 1
fi

echo "Successfully written '$WRITESTR' to the file '$WRITEFILE'."
exit 0
