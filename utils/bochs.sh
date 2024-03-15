#!/bin/sh
set -e
. ./iso.sh

# Create a disk image if it does not exist
if [ ! -f $DISK ]; then
    dd if=/dev/zero of=$DISK bs=1M count=1024
fi

bochs -f utils/bochs/bochsrc.txt -q
