#!/bin/bash

set -e
set -v

# Get the sources
#rm -fr gn-standalone
if [ ! -d "gn-standalone" ]; then
  mkdir gn-standalone
  cd gn-standalone
  mkdir tools
  cd tools
  git clone https://chromium.googlesource.com/chromium/src/tools/gn
  cd ..
  #mkdir -p third_party/libevent
  #cd third_party/libevent
  #wget --no-check-certificate https://chromium.googlesource.com/chromium/chromium/+archive/master/third_party/libevent.tar.gz
  #tar -xvzf libevent.tar.gz
  #cd ../..
  git clone https://chromium.googlesource.com/chromium/src/base
  git clone https://chromium.googlesource.com/chromium/src/build
  git clone https://chromium.googlesource.com/chromium/src/build/config
  mkdir testing
  cd testing
  git clone https://chromium.googlesource.com/chromium/testing/gtest
  cd ../..
fi

# Build
cd gn-standalone/tools/gn
if hash python2 2>/dev/null; then
  python2 ./bootstrap/bootstrap.py -s
else
  python ./bootstrap/bootstrap.py -s
fi

# At this point, the resulting binary is at:
# gn-standalone/out/Release/gn
