#!/bin/sh
set -e

./iso.sh
./disk.sh

cd ..

qemu-system-$(utils/arch.sh) \
-m 1024M \
-boot order=d \
-cdrom myos.iso \
-drive id=disk,file=disk.img,format=raw,if=none \
-device ide-hd,drive=disk,bus=ide.0
