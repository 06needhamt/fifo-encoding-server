#! /bin/sh
b() { make clean && make && bin/fifoserver; }
v() { bin/fifoserver; }
d() { gdb bin/fifoserver; }
db() { make clean && make && gdb bin/fifoserver; }
c() { make clean; }
