#!/bin/bash
set -e

cd code/build.linux
make depend
make
make cleandepend

# Test
./nachos
