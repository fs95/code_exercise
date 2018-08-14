#!/bin/bash
# 功能：遍历一个目录得到总字节大小和其中大于1MB字节大小的文件数量
# wc file | awk '{print $3}' # 输出第三列的字节数

# 遍历一个目录 usage: temp=($(traverse $file))
function traverse {

    size=0 # 目录里的总大小
    large_then1M=0 # 超过1M的文件数量
    if [ $# -eq 1 ]; then

        directory=$1
        for file in $directory/*; do

            if [ -d "$file" ]; then # 目录，递归
                temp=($(traverse $file))
                size=$[$size + ${temp[0]}]
                large_then1M=$[$large_then1M + ${temp[1]}]

            elif [ -f "$file" ]; then # 文件，记录大小
                file_size=$(wc -c $file | awk '{print $1}') # wc -c输出总字节数（总大小）
                size=$[$size + $file_size]
                if [ $file_size -gt $[1024 * 1024] ]; then # 大小超过1MB
                    large_then1M=$[$large_then1M + 1]
                fi
            fi
        done
    fi
    # temp=($size $large_then1M); echo ${temp[*]} # 返回数组的方式
    echo $size $large_then1M
}

if [ $# -ne 1 ] || [ ! -d $1 ]; then # 参数数量！=1，或者第一个参数不是目录或不存在
    echo "counter: Need parameter"
    echo "usage: counter DIR"
    exit 1
else
    file=$1
    temp=($(traverse $file)) # 得到两个数组变量
    size=${temp[0]}
    large_then1M=${temp[1]}
    echo "The total size of the $1 directory: $size[byte]"
    echo "The number of files larger than 1M: $large_then1M"
fi
