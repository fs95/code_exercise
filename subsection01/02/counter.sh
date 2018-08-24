#!/bin/bash
# Function: Traverse a directory to get the total byte size
# and the number of files large than 1MB in size
if [ $# -ne 1 ] || [ ! -d $1 ]; then
    path=.
else
    path=$1
fi
file=$1
limitFileSize=$[1000 * 1000]
# size=$(du -bs $1 | awk '{print $1}')
size=$(ls -Ral $1 | gawk '$1 ~ /^-/ {sum+=$5};END{print sum;}')
large_then1M=$(ls -Ral $path | gawk '$1 ~ /^-/ $5>=('"$limitFileSize"')' | wc -l)
echo "The total size of the [$path] directory: $size[byte]"
echo "The number of files larger than $limitFileSize[byte]: $large_then1M"
