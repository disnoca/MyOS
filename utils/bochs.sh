#!/bin/sh
set -e
. ./iso.sh

bochs -f utils/bochs/bochsrc.txt -q
