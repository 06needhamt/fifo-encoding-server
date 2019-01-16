#! /bin/sh

deps() { rm -rf deps && mkdir deps && cd deps && wget http://www.digip.org/jansson/releases/jansson-2.12.tar.gz && tar xvf jansson-2.12.tar.gz && cd jansson-2.12 && autoreconf -fi && ./configure &&  make && make check && sudo make install && cd ../../; }
b() { make clean && sudo make && sudo /usr/bin/fifoserver; }
v() { make clean && sudo make && sudo valgrind /usr/bin/fifoserver; }
r() { sudo /usr/bin/fifoserver; }
d() { sudo gdb /usr/bin/fifoserver; }
db() { make clean && sudo make && sudo gdb /usr/bin/fifoserver; }
c() { make clean; }

