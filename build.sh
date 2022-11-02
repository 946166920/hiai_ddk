#!/bin/bash

cur_dir=$(dirname $0)

if [ -d ${cur_dir}/out ]; then
    rm -rf ${cur_dir}/out
fi
mkdir ${cur_dir}/out

cd ${cur_dir}/out
# cmake ..
cmake -DCMAKE_TOOLCHAIN_FILE=${cur_dir}/build/core/himake/hi_toolchain/android-ndk.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=android-19 \
      ..

make
# make -j
