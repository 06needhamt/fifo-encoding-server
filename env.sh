#! /bin/sh

deps() { sudo apt-get install cmake autoconf libavutil-dev libswresample-dev libavcodec-dev libavformat-dev libmicrohttpd-dev libssl-dev && rm -rf deps && mkdir deps && cd deps && ../build_ffmpeg.sh && wget http://www.digip.org/jansson/releases/jansson-2.12.tar.gz && tar xvf jansson-2.12.tar.gz && cd jansson-2.12 && autoreconf -fi && ./configure &&  make && make check && sudo make install && cd ../ && git clone https://github.com/warmcat/libwebsockets && cd libwebsockets && mkdir build && cd build && cmake .. && make && sudo make install && cd ../../../; }
b() { make clean && sudo make && LD_LIBRARY_PATH=/usr/local/lib bin/fifoserver; }
v() { make clean && sudo make && LD_LIBRARY_PATH=/usr/local/lib bin/fifoserver; }
r() { LD_LIBRARY_PATH=usr/local/lib bin/fifoserver; }
d() { LD_LIBRARY_PATH=/usr/local/lib gdb bin/fifoserver; }
db() { make clean && sudo make && LD_LIBRARY_PATH=/usr/local/lib gdb bin/fifoserver; }
c() { make clean; }
start() { fifoserver; }
