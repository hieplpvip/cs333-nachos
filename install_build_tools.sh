#!/bin/bash
set -e

if [ $(arch) = "aarch64" ]; then
  sudo apt install -y build-essential gcc-i686-linux-gnu gcc-multilib-i686-linux-gnu g++-i686-linux-gnu g++-multilib-i686-linux-gnu qemu-user-static binfmt-support
  sudo ln -sf /usr/i686-linux-gnu/lib/ld-linux.so.2 /lib/ld-linux.so.2
else
  sudo apt install -y build-essential gcc g++ gcc-multilib g++-multilib
fi

cd ..
if [ ! -f mips-decstation.linux-xgcc.gz ]; then
  wget https://www.fit.hcmus.edu.vn/~ntquan/os/assignment/mips-decstation.linux-xgcc.gz
  tar zxvf mips-decstation.linux-xgcc.gz
fi
