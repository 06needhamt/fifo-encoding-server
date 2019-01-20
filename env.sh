#! /bin/sh

deps() { sudo apt-get install autoconf libavutil-dev libswresample-dev libavcodec-dev libavformat-dev libwebsockets-dev libmicrohttpd-dev libssl-dev && rm -rf deps && mkdir deps && cd deps && wget http://www.digip.org/jansson/releases/jansson-2.12.tar.gz && tar xvf jansson-2.12.tar.gz && cd jansson-2.12 && autoreconf -fi && ./configure &&  make && make check && sudo make install && cd ../../; }
b() { make clean && sudo make && bin/fifoserver; }
v() { make clean && sudo make && bin/fifoserver; }
r() { bin/fifoserver; }
d() { gdb bin/fifoserver; }
db() { make clean && sudo make && gdb bin/fifoserver; }
c() { make clean; }

