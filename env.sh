#! /bin/sh

deps() { rm -rf deps && mkdir deps && cd deps && wget http://www.digip.org/jansson/releases/jansson-2.12.tar.gz && tar xvf jansson-2.12.tar.gz && cd jansson-2.12 && autoreconf -fi && ./configure &&  make && make check && sudo make install && cd ../../; }
b() { make clean && make && bin/fifoserver; }
v() { bin/fifoserver; }
d() { gdb bin/fifoserver; }
db() { make clean && make && gdb bin/fifoserver; }
c() { make clean; }

