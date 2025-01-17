#! /bin/bash

CORES="$(cat /proc/cpuinfo | awk '/^processor/{print $3}' | wc -l)"

deps() { ./install_packages.sh && sudo rm -rf deps && mkdir deps && cd deps && ../build_ffmpeg.sh && ../build_core_deps.sh && cd .. && mkdir bin/ && mkdir obj/; }
b() { make clean && sudo make -j"$CORES" && LD_LIBRARY_PATH=/usr/local/lib bin/fifoserver; }
v() { make clean && sudo make -j"$CORES" && LD_LIBRARY_PATH=/usr/local/lib valgrind bin/fifoserver; }
r() { LD_LIBRARY_PATH=usr/local/lib bin/fifoserver; }
d() { LD_LIBRARY_PATH=/usr/local/lib gdb bin/fifoserver; }
db() { make clean && sudo make -j"$CORES" && LD_LIBRARY_PATH=/usr/local/lib gdb bin/fifoserver; }
c() { make clean; }
start() { fifoserver; }
