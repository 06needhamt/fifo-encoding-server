#! /bin/bash

CORES="$(cat /proc/cpuinfo | awk '/^processor/{print $3}' | wc -l)"
WORKING_DIR="$(pwd)"

wget http://www.digip.org/jansson/releases/jansson-2.12.tar.gz
tar xvf jansson-2.12.tar.gz
cd jansson-2.12
autoreconf -fi
./configure
make -j$CORES
make check
sudo make install

cd $WORKING_DIR
git clone https://github.com/warmcat/libwebsockets
cd libwebsockets
mkdir build
cd build
cmake ..
make -j$CORES
sudo make install

cd $WORKING_DIR
wget https://www.openssl.org/source/openssl-1.0.2r.tar.gz
tar xvf openssl-1.0.2r.tar.gz
cd openssl-1.0.2r
./config --prefix=/usr/local/ssl --openssldir=/usr/local/ssl shared zlib
make -j$CORES
make test
sudo make install

cd $WORKING_DIR
