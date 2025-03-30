#!/bin/bash
#!/bin/bash

if [ $# -eq 1 ]; then
    file=$1
    # Check if the file exists as provided
    if [ -f "$file" ]; then
        echo "Processing $file..."
        ./parser < "$file"
    # If the file does not exist, check in the test_traces directory
    elif [ -f "test_traces/$file" ]; then
        echo "Processing test_traces/$file..."
        ./parser < "test_traces/$file"
    else
        echo "Error: File $file does not exist."
        exit 1
    fi
else
    # If no argument is provided, process all files in the test_traces directory
    for file in test_traces/*.txt; do
        echo "Processing $file..."
        ./parser < "$file"
    done
fi

