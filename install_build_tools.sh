#!/bin/bash
set -e

sudo apt install -y build-essential gcc g++ gcc-multilib g++-multilib

cd ..
wget https://www.fit.hcmus.edu.vn/~ntquan/os/assignment/mips-decstation.linux-xgcc.gz
tar zxvf mips-decstation.linux-xgcc.gz
