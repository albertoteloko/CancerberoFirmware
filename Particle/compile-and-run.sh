#!/bin/bash

NODE=$1
ARCH=$2
ACTION=$3

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
CURRENT_DIR=${pwd}


echo "Node: $NODE"
echo "Arch: $ARCH"

cd "$DIR"

if [ "$ACTION" == "compile" ]; then
    echo "Compiling..." && particle compile $ARCH
else
    echo "Flashing..." && particle flash $NODE
    rm *.bin
fi

cd "$CURRENT_DIR"
echo "Done!"

