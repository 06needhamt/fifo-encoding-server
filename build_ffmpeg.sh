#! /bin/bash

CORES="$(cat /proc/cpuinfo | awk '/^processor/{print $3}' | wc -l)"
WORKING_DIR="$(pwd)"

sudo apt-get update -qq && sudo apt-get -y install \
  autoconf \
  automake \
  build-essential \
  cmake \
  git-core \
  libass-dev \
  libfreetype6-dev \
  libsdl2-dev \
  libtool \
  libva-dev \
  libvdpau-dev \
  libvorbis-dev \
  libxcb1-dev \
  libxcb-shm0-dev \
  libxcb-xfixes0-dev \
  pkg-config \
  texinfo \
  wget \
  zlib1g-dev;

  rm -rf ffmpeg_build/
  rm -rf ffmpeg_sources/
  rm -rf build/

  mkdir -p ffmpeg_sources bin

cd $WORKING_DIR/ffmpeg_sources && \
wget https://www.nasm.us/pub/nasm/releasebuilds/2.13.03/nasm-2.13.03.tar.bz2 && \
tar xjvf nasm-2.13.03.tar.bz2 && \
cd nasm-2.13.03 && \
./autogen.sh && \
PATH="$WORKING_DIR/bin:$PATH" ./configure --prefix="$WORKING_DIR/ffmpeg_build" --bindir="$WORKING_DIR/bin" && \
make -j"$CORES" && \
make install;

cd $WORKING_DIR/ffmpeg_sources && \
wget -O yasm-1.3.0.tar.gz https://www.tortall.net/projects/yasm/releases/yasm-1.3.0.tar.gz && \
tar xzvf yasm-1.3.0.tar.gz && \
cd yasm-1.3.0 && \
./configure --prefix="$WORKING_DIR/ffmpeg_build" --bindir="$WORKING_DIR/bin" && \
make -j"$CORES" && \
make install;

cd $WORKING_DIR/ffmpeg_sources && \
git -C x264 pull 2> /dev/null || git clone --depth 1 https://git.videolan.org/git/x264 && \
cd x264 && \
PATH="$WORKING_DIR/bin:$PATH" PKG_CONFIG_PATH="$WORKING_DIR/ffmpeg_build/lib/pkgconfig" ./configure --prefix="$WORKING_DIR/ffmpeg_build" --bindir="$WORKING_DIR/bin" --enable-static --enable-pic && \
PATH="$WORKING_DIR/bin:$PATH" make -j"$CORES" && \
make install;

sudo apt-get install mercurial libnuma-dev && \
cd $WORKING_DIR/ffmpeg_sources && \
if cd x265 2> /dev/null; then hg pull && hg update; else hg clone https://bitbucket.org/multicoreware/x265; fi && \
cd x265/build/linux && \
PATH="$WORKING_DIR/bin:$PATH" cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="$WORKING_DIR/ffmpeg_build" -DENABLE_SHARED=off ../../source && \
PATH="$WORKING_DIR/bin:$PATH" make -j"$CORES" && \
make install;

cd $WORKING_DIR/ffmpeg_sources && \
git -C libvpx pull 2> /dev/null || git clone --depth 1 https://chromium.googlesource.com/webm/libvpx.git && \
cd libvpx && \
PATH="$WORKING_DIR/bin:$PATH" ./configure --prefix="$WORKING_DIR/ffmpeg_build" --disable-examples --disable-unit-tests --enable-vp9-highbitdepth --as=yasm && \
PATH="$WORKING_DIR/bin:$PATH" make -j"$CORES" && \
make install;

cd $WORKING_DIR/ffmpeg_sources && \
git -C fdk-aac pull 2> /dev/null || git clone --depth 1 https://github.com/mstorsjo/fdk-aac && \
cd fdk-aac && \
autoreconf -fiv && \
./configure --prefix="$WORKING_DIR/ffmpeg_build" --disable-shared && \
make -j"$CORES" && \
make install;

cd $WORKING_DIR/ffmpeg_sources && \
wget -O lame-3.100.tar.gz https://downloads.sourceforge.net/project/lame/lame/3.100/lame-3.100.tar.gz && \
tar xzvf lame-3.100.tar.gz && \
cd lame-3.100 && \
PATH="$WORKING_DIR/bin:$PATH" ./configure --prefix="$WORKING_DIR/ffmpeg_build" --bindir="$WORKING_DIR/bin" --disable-shared --enable-nasm && \
PATH="$WORKING_DIR/bin:$PATH" make -j"$CORES" && \
make install;

cd $WORKING_DIR/ffmpeg_sources && \
git -C opus pull 2> /dev/null || git clone --depth 1 https://github.com/xiph/opus.git && \
cd opus && \
./autogen.sh && \
./configure --prefix="$WORKING_DIR/ffmpeg_build" --disable-shared && \
make -j"$CORES" && \
make install;

cd $WORKING_DIR/ffmpeg_sources && \
git -C aom pull 2> /dev/null || git clone --depth 1 https://aomedia.googlesource.com/aom && \
mkdir -p aom_build && \
cd aom_build && \
PATH="$WORKING_DIR/bin:$PATH" cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="$WORKING_DIR/ffmpeg_build" -DENABLE_SHARED=off -DENABLE_NASM=on ../aom && \
PATH="$WORKING_DIR/bin:$PATH" make -j"$CORES" && \
make install;

cd $WORKING_DIR/ffmpeg_sources && \
wget -O ffmpeg-4.1.4.tar.bz2 https://ffmpeg.org/releases/ffmpeg-4.1.4.tar.bz2 && \
tar xjvf ffmpeg-4.1.4.tar.bz2 && \
cd ffmpeg && \
PATH="$WORKING_DIR/bin:$PATH" PKG_CONFIG_PATH="$WORKING_DIR/ffmpeg_build/lib/pkgconfig" ./configure \
  --prefix="$WORKING_DIR/ffmpeg_build" \
  --pkg-config-flags="--static" \
  --extra-cflags="-I$WORKING_DIR/ffmpeg_build/include" \
  --extra-ldflags="-L$WORKING_DIR/ffmpeg_build/lib" \
  --extra-libs="-lpthread -lm" \
  --bindir="$WORKING_DIR/bin" \
  --enable-gpl \
  --enable-libaom \
  --enable-libass \
  --enable-libfdk-aac \
  --enable-libfreetype \
  --enable-libmp3lame \
  --enable-libopus \
  --enable-libvorbis \
  --enable-libvpx \
  --enable-libx264 \
  --enable-libx265 \
  --enable-nonfree && \
PATH="$WORKING_DIR/bin:$PATH" make -j"$CORES" && \
make install && \
hash -r;

source ~/.profile;

cd $WORKING_DIR;
