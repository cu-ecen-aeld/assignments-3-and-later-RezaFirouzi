#!/bin/bash

# Author: Reza Firouzi

set -e

# Check if exactly two arguments are provided
if [ $# -ne 2 ]; then
    echo "Error: Two required arguments must be passed to the script!"
    echo "Usage: $0 <directory> <search_string>"
    exit 1
fi

FILESDIR="$1"
SEARCHSTR="$2"

# Check if the first argument is a valid directory
if [ ! -d "$FILESDIR" ]; then
    echo "Error: The first argument is not a valid directory"
    exit 1
fi

# Count the number of files in the directory
NUMFILES=$(find $FILESDIR -type f | wc -l)

# Count the number of matching lines across all files
NUMMATCH=$(grep -r $SEARCHSTR $FILESDIR 2>/dev/null | wc -l)

# Output the results
echo "The number of files are $NUMFILES and the number of matching lines are $NUMMATCH"

exit 0
