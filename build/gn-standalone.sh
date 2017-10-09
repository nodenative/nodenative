#!/bin/bash

#set -e
#set -v

# a possibility to extract binaries can be found: https://aur.archlinux.org/cgit/aur.git/tree/PKGBUILD?h=gn-bin
# sha1=$(curl "https://chromium.googlesource.com/chromium/buildtools/+/master/linux64/gn.sha1?format=TEXT" | base64 --decode)
# pkgname='gn-bin'
# pkgdesc="Prebuilt binary for GN (Generate Ninja), Chromium's meta-build system"
# pkgver=r456551
# pkgrel=1
# arch=('x86_64')
# conflicts=('gn-git')
# provides=('gn')
# url='https://chromium.googlesource.com/chromium/src/tools/gn/'
# source=(
#   "https://storage.googleapis.com/chromium-gn/$sha1"
# )
# noextract=("$sha1")
# sha1sums=("$sha1")

# pkgver () {
#   chmod +x $srcdir/$sha1
#   printf "r$($srcdir/$sha1 --version)"
# }

# package () {
#   install -Dm755 "$srcdir/$sha1" "$pkgdir/usr/bin/gn"
# }


# An interesting script can be found https://github.com/colinblundell/mojo-standalone/blob/master/build/install-build-deps.sh
git_checkout_commit()
{
  curr_dir=`pwd`
  git_dir=$1
  git_hash=$2
  git_patch=$3

  if [ ! -z "$git_hash" ]; then
    echo "checkout $git_dir to $git_hash..."
    cd $git_dir
    git checkout $git_hash
    cd $curr_dir
  fi;

  if [ ! -z "$git_patch" ]; then
    echo "applying patch $git_patch to $git_dir..."
    cd $git_dir
    git apply < $git_patch
# commit
#    git am --signoff < $git_patch
    cd $curr_dir
  fi;
}

git_clone()
{
  curr_path=`pwd`
  req_path=$1
  git_url=$2
  git_dir=$3
  git_hash=$4
  git_patch=$5

  echo "cloning $git_url to $curr_path/$req_path/$git_dir..."
  mkdir -p $req_path
  cd $req_path
  git clone $git_url $git_dir
  git_checkout_commit "$git_dir" "$git_hash" "$git_patch"
  cd $curr_path
}

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
CURR_DIR=`pwd`

RELEASE_TYPE="$1"
GIT_TESTED_COMMIT_GN="9ffda3e8f3f7756bab67f49055296a89d43b4543"
GIT_TESTED_COMMIT_BASE="bb89059924fe115f70c9eee3034234db14c12c68"
GIT_TESTED_COMMIT_BUILD="adaf9e56105b814105e2d49bc4fa63e2cd4795f5"
GIT_TESTED_COMMIT_BUILD_CONFIG="adaf9e56105b814105e2d49bc4fa63e2cd4795f5"
GIT_TESTED_COMMIT_TESTING_GTEST="d8a16832a3dd620e59db8771a6004ffb043941d3"

# Format a patch from the last commit with the following command: git format-patch --stdout -1 > file.patch
# or from current branch against master branch: git format-patch --stdout master > file.patch
# for more details check https://www.devroom.io/2009/10/26/how-to-create-and-apply-a-patch-with-git/
GIT_TESTED_PATCH_GN="$DIR/gn.patch"

if [ "$RELEASE_TYPE" = "dev" ]; then
  GIT_TESTED_COMMIT_GN=""
  GIT_TESTED_COMMIT_BASE=""
  GIT_TESTED_COMMIT_BUILD=""
  GIT_TESTED_COMMIT_BUILD_CONFIG=""
  GIT_TESTED_COMMIT_TESTING_GTEST=""
  GIT_TESTED_PATCH_GN=""
elif [ "$RELEASE_TYPE" = "devPatch" ]; then
  GIT_TESTED_COMMIT_GN=""
  GIT_TESTED_COMMIT_BASE=""
  GIT_TESTED_COMMIT_BUILD=""
  GIT_TESTED_COMMIT_BUILD_CONFIG=""
  GIT_TESTED_COMMIT_TESTING_GTEST=""
elif [ -z "$RELEASE_TYPE" ]; then
  RELEASE_TYPE="stable"
elif [ "$RELEASE_TYPE" = "help" ]; then
  echo "use ./gn-standalone.sh [stable|dev|devPatch|help]"
  exit 0
else
  echo "invalid argument $RELEASE_TYPE, use [stable|dev|devPatch|help]"
  exit 128
fi;

echo "reelase: $RELEASE_TYPE\n"

# Get the sources
#rm -fr gn-standalone
if [ ! -d "gn-standalone" ]; then
  git_clone gn-standalone/tools https://chromium.googlesource.com/chromium/src/tools/gn gn "$GIT_TESTED_COMMIT_GN" "$GIT_TESTED_PATCH_GN"
  git_clone gn-standalone https://chromium.googlesource.com/chromium/src/base base "$GIT_TESTED_COMMIT_BASE"

  #mkdir -p third_party/libevent
  #cd third_party/libevent
  #wget --no-check-certificate https://chromium.googlesource.com/chromium/chromium/+archive/master/third_party/libevent.tar.gz
  #tar -xvzf libevent.tar.gz
  #cd ../..

  mkdir -p gn-standalone/third_party/apple_apsl
  cd gn-standalone/third_party/apple_apsl
  # copied from: https://github.com/google/shaka-packager/tree/master/packager/third_party/apple_apsl
  wget https://raw.githubusercontent.com/google/shaka-packager/master/packager/third_party/apple_apsl/malloc.h
  cd ../../../

  git_clone gn-standalone https://chromium.googlesource.com/chromium/src/build build $GIT_TESTED_COMMIT_BUILD
  git_clone gn-standalone https://chromium.googlesource.com/chromium/src/build/config config $GIT_TESTED_COMMIT_BUILD_CONFIG
  git_clone gn-standalone/testing https://chromium.googlesource.com/chromium/testing/gtest gtest $GIT_TESTED_COMMIT_TESTING_GTEST
fi

# Build
echo "building..."
cd gn-standalone/tools/gn
if hash python2 2>/dev/null; then
  python2 ./bootstrap/bootstrap.py -s
else
  python ./bootstrap/bootstrap.py -s
fi

echo "At this point, the resulting binary is at:\n"
echo "gn-standalone/out/Release/gn"
