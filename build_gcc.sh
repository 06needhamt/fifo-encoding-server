#! /bin/bash

CORES="$(cat /proc/cpuinfo | awk '/^processor/{print $3}' | wc -l)"
WORKING_DIR="$(pwd)"

sudo apt install build-essential

cd $WORKING_DIR
wget https://ftpmirror.gnu.org/gcc/gcc-8.2.0/gcc-8.2.0.tar.gz
tar xf gcc-8.2.0.tar.gz
cd gcc-8.2.0
contrib/download_prerequisites

cd $WORKING_DIR
mkdir build && cd build
../gcc-8.2.0/configure -v --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=x86_64-linux-gnu --prefix=/usr/local/gcc-8.2 --enable-checking=release --enable-languages=c,c++,objc --disable-multilib --program-suffix=-8.2

make -j$CORES

sudo make install

cd $WORKING_DIR
