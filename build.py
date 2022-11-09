#!/usr/bin/env python
# encoding: utf-8
# Copyright © Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
# Description: build.py
# This program is free software; you can redistribute it and/or modify
# it under the terms of the Apache License Version 2.0.You may not use this file except in compliance with the License.
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# Apache License for more details at
# http://www.apache.org/licenses/LICENSE-2.0


import os
import zipfile
import tarfile

THIRD_PARTY_LINK_LIST_URL = [
    "https://android.googlesource.com/platform/system/core/+archive/refs/heads/master/libcutils/include/cutils.tar.gz",
    "https://github.com/openeuler-mirror/libboundscheck/archive/refs/tags/v1.1.11.zip",
    "https://github.com/protocolbuffers/protobuf/archive/refs/tags/v3.13.0.zip",
    "https://github.com/protocolbuffers/protobuf/releases/download/v3.13.0/protoc-3.13.0-linux-x86_64.zip",
    "https://github.com/Kitware/CMake/releases/download/v3.21.4/cmake-3.21.4-linux-x86_64.tar.gz",
    "https://dl.google.com/android/repository/android-ndk-r20b-linux-x86_64.zip",

    # "http://10.136.104.231:4543/android-ndk-r23b-linux.zip",
    # "http://10.136.104.231:4543/cmake-3.20.5-linux-x86_64.tar.gz"
]

THIRD_PARTY_DOWNLOAD_DIR = "third_party_download"
THIRD_PARTY_DIR = "third_party"

def process_cutils(compressed_package):
    with tarfile.open(os.path.join(THIRD_PARTY_DOWNLOAD_DIR, compressed_package)) as t:
        t.extractall(os.path.join(THIRD_PARTY_DIR, "cutils"))


def process_bounds_checking_function(compressed_package):
    with zipfile.ZipFile(os.path.join(THIRD_PARTY_DOWNLOAD_DIR, compressed_package), 'r') as z:
        z.extractall(THIRD_PARTY_DIR)

    mv_c_sec_dir_name = "mv third_party/libboundscheck-1.1.11 third_party/bounds_checking_function"
    os.system(mv_c_sec_dir_name)


def process_protobuf(compressed_package):
    with zipfile.ZipFile(os.path.join(THIRD_PARTY_DOWNLOAD_DIR, compressed_package), 'r') as z:
        z.extractall(THIRD_PARTY_DIR)

    mv_protobuf_dir_name = "mv third_party/protobuf-3.13.0 third_party/protobuf"
    os.system(mv_protobuf_dir_name)


def process_protoc(compressed_package):
    with zipfile.ZipFile(os.path.join(THIRD_PARTY_DOWNLOAD_DIR, compressed_package), 'r') as z:
        z.extractall(os.path.join(THIRD_PARTY_DIR, "protoc-3.13.0"))
    # protoc 加权限
    chmod_protoc = "chmod -R u+x {}".format("third_party/protoc-3.13.0")
    os.system(chmod_protoc)


def process_ndk(compressed_package):
    with zipfile.ZipFile(os.path.join(THIRD_PARTY_DOWNLOAD_DIR, compressed_package), 'r') as z:
        z.extractall(THIRD_PARTY_DIR)
    # ndk 加权限
    chmod_ndk = "chmod -R u+x {}".format("third_party/android-ndk-r20b")
    os.system(chmod_ndk)


def process_cmake(compressed_package):
    with tarfile.open(os.path.join(THIRD_PARTY_DOWNLOAD_DIR, compressed_package)) as t:
        t.extractall(THIRD_PARTY_DIR)
    
    mv_cmake_dir_name = "mv third_party/cmake-3.20.5-linux-x86_64 third_party/cmake-3.20.5"
    os.system(mv_cmake_dir_name)
    # cmake 加export
    export_cmake_path = "export PATH={}/third_party/cmake-3.20.5/bin:$PATH".format("$(pwd)")
    print("export_cmake_path", export_cmake_path)
    os.system(export_cmake_path)


THIRD_PARTY_LINK_LIST = {
    "cutils": ["http://10.136.104.231:4543/core-refs_heads_master-libcutils-include-cutils.tar.gz", process_cutils],
    "bounds_checking_function": ["http://10.136.104.231:4543/libboundscheck-1.1.11.zip", process_bounds_checking_function],
    "protobuf": ["http://10.136.104.231:4543/protobuf-3.13.0.zip", process_protobuf],
    "protoc-3.13.0": ["http://10.136.104.231:4543/protoc-3.13.0-linux-x86_64.zip", process_protoc],
    "android-ndk-r20b": ["http://10.136.104.231:4543/android-ndk-r20b-linux-x86_64.zip", process_ndk],
    "cmake-3.20.5": ["http://10.136.104.231:4543/cmake-3.20.5-linux-x86_64.tar.gz", process_cmake],

    # "http://10.136.104.231:4543/android-ndk-r23b-linux.zip",
}

def run_process(package):
    compressed_package = THIRD_PARTY_LINK_LIST[package][0].split('/')[-1]
    print(compressed_package)
    process_func = THIRD_PARTY_LINK_LIST[package][1]
    process_func(compressed_package)

def download_third_party():
    if not os.path.exists(THIRD_PARTY_DOWNLOAD_DIR):
        os.system("mkdir {}".format(THIRD_PARTY_DOWNLOAD_DIR))
    if not os.path.exists(THIRD_PARTY_DIR):
         os.system("mkdir {}".format(THIRD_PARTY_DIR))

    package_exist_list = os.listdir(THIRD_PARTY_DIR)
    package_download_list = THIRD_PARTY_LINK_LIST.keys() - package_exist_list
    print("THIRD_PARTY_LINK_LIST.keys", THIRD_PARTY_LINK_LIST.keys())
    print("package_exist_list", package_exist_list)
    print("package_download_list", package_download_list)
    for package in package_download_list:
        download_cmd = "wget -c -t 3 -P {} {} --no-check-certificate".format(THIRD_PARTY_DOWNLOAD_DIR, THIRD_PARTY_LINK_LIST[package][0])
        os.system(download_cmd)
        run_process(package)


if __name__ == '__main__':
    # 下载
    download_third_party()

    # 编译
    os.system("./build.sh")
    # 打包