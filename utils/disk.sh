#!/bin/sh
set -e

readonly DISK_FILE=../disk.img

if [ ! -f $DISK_FILE ]; then
    dd if=/dev/zero of=$DISK_FILE bs=1M count=100

    # Format the disk
    gcc -o programs/newfs programs/newfs.c
    programs/newfs $DISK_FILE
    rm -f programs/newfs
fi

