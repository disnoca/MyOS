#!/bin/sh
set -e
. ./iso.sh

qemu-system-$(utils/arch.sh) -cdrom myos.iso
