#!/bin/sh
set -e

./iso.sh
./disk.sh
cd ..

bochs -f utils/bochs/bochsrc.txt -q
