#!/bin/bash

ACTION=$1
NODE=$2
ARCH=$3

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
CURRENT_DIR=${pwd}

if [ "$NODE" == "" ]; then
    NODE="Shop_Alarm"
fi

if [ "$ARCH" == "" ]; then
    ARCH="photon"
fi

echo "Node: $NODE"
echo "Arch: $ARCH"

cd "$DIR"

if [ "$ACTION" == "compile" ]; then
    echo "Compiling..." && particle compile $ARCH
else
    echo "Flashing..." && particle flash $NODE
fi
rm *.bin
cd "$CURRENT_DIR"
echo "Done!"

