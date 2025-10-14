#!/bin/bash

# Generic script to automatically find all C source files that are needed 
# by following the dependency chain through #include statements from a starting file

find_c_dependencies() {
    local start_file="$1"
    local src_dir="${2:-src}"  # Default source directory
    local processed=()
    local to_process=("$start_file")
    local result=()
    
    while [ ${#to_process[@]} -gt 0 ]; do
        local current="${to_process[0]}"
        to_process=("${to_process[@]:1}")  # Remove first element
        
        # Skip if already processed
        if [[ " ${processed[@]} " =~ " ${current} " ]]; then
            continue
        fi
        
        processed+=("$current")
        
        # If this is a C file, add it to results
        if [[ "$current" == *.c ]] && [ -f "$current" ]; then
            result+=("$current")
        fi
        
        # Find header file corresponding to this C file
        if [[ "$current" == *.c ]]; then
            local header="${current%.c}.h"
            if [ -f "$header" ]; then
                to_process+=("$header")
            fi
        fi
        
        # Process includes from current file
        if [ -f "$current" ]; then
            # Find local includes (quoted includes only, not system includes)
            local includes=$(grep '^[[:space:]]*#include[[:space:]]*"' "$current" 2>/dev/null | \
                           sed 's/^[[:space:]]*#include[[:space:]]*"//' | \
                           sed 's/".*//')
            
            for include in $includes; do
                # Try to find the include file in various locations
                local include_path=""
                local current_dir=$(dirname "$current")
                
                # Search order: relative to current file, then relative to src root
                if [ -f "$current_dir/$include" ]; then
                    include_path="$current_dir/$include"
                elif [ -f "$src_dir/$include" ]; then
                    include_path="$src_dir/$include"
                fi
                
                if [ -n "$include_path" ]; then
                    # Normalize the path (remove ../ patterns)
                    include_path=$(cd "$(dirname "$include_path")" && pwd)/$(basename "$include_path")
                    # Convert back to relative path from current directory
                    include_path=$(realpath --relative-to="." "$include_path" 2>/dev/null || echo "$include_path")
                    
                    to_process+=("$include_path")
                    
                    # If it's a header file, also check for corresponding C file
                    if [[ "$include_path" == *.h ]]; then
                        local c_file="${include_path%.h}.c"
                        if [ -f "$c_file" ]; then
                            to_process+=("$c_file")
                        fi
                    fi
                fi
            done
        fi
    done
    
    # Output unique C files, sorted, with normalized paths
    printf '%s\n' "${result[@]}" | sort -u | while read -r file; do
        # Normalize path and make it relative to current directory
        if [ -f "$file" ]; then
            realpath --relative-to="." "$file" 2>/dev/null || echo "$file"
        fi
    done | sort -u
}

# Auto-detect main file and source directory
main_file=""
src_dir="src"

# Look for main.c in common locations
if [ -f "src/main.c" ]; then
    main_file="src/main.c"
elif [ -f "main.c" ]; then
    main_file="main.c"
    src_dir="."
else
    echo "Error: Could not find main.c" >&2
    exit 1
fi

# Find dependencies starting from the discovered main file
find_c_dependencies "$main_file" "$src_dir"