#!/bin/bash
# Function: Traverse a directory to get the total byte size
# and the number of files large than 1MB in size
# Skill: wc file | awk '{print $3}' # Output the number of bytes in the third column

# Usage: temp=($(traverse $DIR))
function traverse {

    size=0 # Total size of the directory
    large_then1M=0 # Number of files over 1MB
    if [ $# -eq 1 ]; then

        directory=$1
        for file in $directory/*; do

            if [ -d "$file" ]; then # Is a directory, recursive
                temp=($(traverse $file))
                size=$[$size + ${temp[0]}]
                large_then1M=$[$large_then1M + ${temp[1]}]

            elif [ -f "$file" ]; then # Is a file, the record size
                file_size=$(wc -c $file | awk '{print $1}') # wc -c,output total bytes
                size=$[$size + $file_size]
                if [ $file_size -gt $[1024 * 1024] ]; then # More than 1MB in size
                    large_then1M=$[$large_then1M + 1]
                fi
            fi
        done
    fi
    echo $size $large_then1M
    # temp=($size $large_then1M); echo ${temp[*]} # Return using array mode
}

# Number of parameters != 1, or the first parameter is not a directory or does not exist
if [ $# -ne 1 ] || [ ! -d $1 ]; then
    echo "Error: Need parameter"
    echo "Usage: $0 [path]"
    exit 1
else
    file=$1
    temp=($(traverse $file)) # Get two array variables
    size=${temp[0]}
    large_then1M=${temp[1]}
    echo "The total size of the $1 directory: $size[byte]"
    echo "The number of files larger than 1M: $large_then1M"
fi
