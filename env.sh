#! /bin/sh

deps() { rm -rf deps && mkdir deps && cd deps && wget http://www.digip.org/jansson/releases/jansson-2.12.tar.gz && tar xvf jansson-2.12.tar.gz && cd jansson-2.12 && autoreconf -fi && ./configure &&  make && make check && sudo make install && cd ../../; }
b() { make clean && sudo make && /usr/bin/fifoserver; }
v() { make clean && sudo make && valgrind /usr/bin/fifoserver; }
r() { /usr/bin/fifoserver; }
d() { gdb /usr/bin/fifoserver; }
db() { make clean && sudo make && gdb /usr/bin/fifoserver; }
c() { make clean; }

