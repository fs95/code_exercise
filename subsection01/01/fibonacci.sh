#!/bin/bash

# If there are parameters
if [ $# -ne 0 ]; then

    if [ $1 == "--clean" ]; then
        rm 00[0-4][0-9].bin 2> /dev/null
        exit 0
    else
        echo "invalid option"
        echo "usage: $0 [--clean]"
        exit 1
    fi
fi

# Initalize
file_name_1='0001.bin'
file_name_2='0002.bin'
cat /dev/urandom | head -c 1 > $file_name_1 # Write a random character
cat /dev/urandom | head -c 1 > $file_name_2

for (( i = 3; i <= 45; i++)); do

    file_name=00$[$i/10]$[$i%10].bin # Generate matching files
    cat $file_name_1 > $file_name # Copy
    cat $file_name_2 >> $file_name # Append

    # Cyclic assignment
    file_name_2=$file_name_1
    file_name_1=$file_name

    # DEBUG
    echo -n "$i: "
    date
done

exit 0
