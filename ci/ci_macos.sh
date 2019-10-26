#!/bin/bash
#
# Build script for GitHub Actions.
#

cd "$HOME" || exit
git clone --recursive https://github.com/steinbergmedia/vst3sdk.git
cd vst3sdk || exit

mkdir build
cd build || exit
cmake -DCMAKE_BUILD_TYPE=Release \
  -DSMTG_MYPLUGINS_SRC_PATH="$GITHUB_WORKSPACE" \
  -DSMTG_ADD_VST3_HOSTING_SAMPLES=FALSE \
  -DSMTG_ADD_VST3_PLUGINS_SAMPLES=FALSE \
  ..
cmake --build .

# No idea how to set $HOME as path of actions/upload-artifact@v1.
mv "$HOME"/vst3sdk/build/VST3/Release "$GITHUB_WORKSPACE"/vst_macOS
