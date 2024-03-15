#!/bin/sh
readonly DISK="disk.img"

set -e
. ./iso.sh

# Create a disk image if it does not exist
if [ ! -f $DISK ]; then
    dd if=/dev/zero of=$DISK bs=1M count=1024
fi

qemu-system-$(utils/arch.sh) \
-m 1024M \
-boot c \
-cdrom myos.iso \
-drive id=disk,file=$DISK,format=raw,if=none \
-device ide-hd,drive=disk,bus=ide.0
