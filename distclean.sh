#!/bin/bash
set -e

cd code/build.linux
make distclean

cd ../test
make distclean
