#!/bin/sh
set -e

./clean.sh
cd .. && make all
