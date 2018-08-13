#!/bin/bash

# 表达式：
# var1=$[$var1+1]

# 文件名的十位和个位
ten=0
unit=0

# fibonacci 数列
fib_last_1=0
fib_last_2=0

# 被填充的字符

for (( i = 1; i <= 45; i++)); do

    file_name=00$[$i/10]$[$i%10].bin # 生成对应文件名
    touch $file_name # 创建文件

    fib_cur=$[$fib_last_1 + $fib_last_2]; # fibonacci数列求当前值

    # 第一次循环，
    if [ $fib_cur -eq 0 ]
    then
        fib_cur=1
    fi

    cat /dev/urandom | head -c $fib_cur > file_name # 写入固定数量的随机字符

    # echo $fib_last_2,$fib_last_1,$fib_cur # debug

    fib_last_2=$fib_last_1;
    fib_last_1=$fib_cur;

done

exit 0
