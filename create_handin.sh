#!/bin/bash

# List of files/directories to exclude (relative paths)
EXCLUDE_LIST=(
    ".git"
    "docs"
    ".vscode"
    "node-1/build"
    "node-1/openocd"
    "node-1/src/fonts"
    "node-2/build"
    "node-2/sam"
    "node-2/main.c"
    "node-2/Makefile"
    "node-2/can_controller.c"
    "node-2/can_controller.h"
    "node-2/can_interrupt.c"
    "node-2/can_interrupt.h"
    "node-2/can.c"
    "node-2/can.h"
    "node-2/time.c"
    "node-2/time.h"
    "node-2/uart.c"
    "node-2/uart.h"
)

# Delete existing handin directory if it exists
if [ -d "handin" ]; then
    echo "Removing existing handin directory..."
    rm -rf handin
fi

# Create handin directory
echo "Creating handin directory..."
mkdir -p handin

# Function to check if a file should be excluded
should_exclude() {
    local file="$1"
    for exclude_pattern in "${EXCLUDE_LIST[@]}"; do
        # Check exact match or if file is under an excluded directory
        if [[ "$file" == "$exclude_pattern" ]] || [[ "$file" == "$exclude_pattern"/* ]]; then
            return 0  # Should exclude
        fi
    done
    return 1  # Should not exclude
}

# Copy files while excluding those in the exclude list
echo "Copying files to handin/..."
find . -type f | while read -r file; do
    # Remove leading ./
    relative_file="${file#./}"
    
    # Check if file should be excluded
    if should_exclude "$relative_file"; then
        # echo "Excluding: $relative_file"
        continue
    fi
    
    # Create directory structure in handin/
    target_dir="handin/$(dirname "$relative_file")"
    mkdir -p "$target_dir"
    
    # Copy the file
    cp "$file" "handin/$relative_file"
    echo "Copied: $relative_file"
done

echo "Handout created in handin/ directory"