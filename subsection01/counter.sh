#!/bin/bash

# wc file | awk '{print $3}' # 输出第三列的字节数

# file_count=0;
function traverse {

    size=0 # 目录里的总大小
    if [ $# -eq 1 ]; then

        dirctory=$1
        for file in $dirctory/*; do

            if [ -d "$file" ]; then # 目录，递归
                size=$[$size + $(traverse $file)]

            elif [ -f "$file" ]; then # 文件，记录大小
                file_size=$(wc -c $file | awk '{print $1}') # wc -c输出总字节数（总大小）
                size=$[$size + $file_size]
                # file_count=$[$file_count + 1]
                # echo "file_counter: $file_count $file: FILE"
            fi
        done
    fi
    echo $size
}

if [ $# -ne 1 ] || [ ! -d $1 ]; then # 参数数量！=1，或者第一个参数不是目录或不存在
    echo "counter: Need parameter"
    echo "usage: counter DIR"
    exit 1
else
    file=$1
    size=$(traverse $file)
    echo "The total size of the $1 directory: $size[byte]"
fi
