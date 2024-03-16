#!/bin/sh
set -e

cd ..

if [ ! -f disk.img ]; then
    dd if=/dev/zero of=disk.img bs=1M count=1024
fi
