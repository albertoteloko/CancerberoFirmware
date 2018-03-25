#!/bin/bash

./compile-and-run.sh Talamanca electron compile

for file in *.bin; do
    echo $file
    particle flash --usb "$file"
done

rm *.bin

