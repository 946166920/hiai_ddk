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

THIRD_PARTY_LINK_LIST = [
    "http://10.136.104.231:4543/core-refs_heads_master-libcutils-include-cutils.tar.gz",
    "http://10.136.104.231:4543/libboundscheck-1.1.11.zip",
    "http://10.136.104.231:4543/protobuf-3.13.0.zip",
    "http://10.136.104.231:4543/protoc-3.13.0-linux-x86_64.zip",
    "http://10.136.104.231:4543/cmake-3.21.4-linux-x86_64.tar.gz",
    "http://10.136.104.231:4543/android-ndk-r20b-linux-x86_64.zip",

    # "http://10.136.104.231:4543/android-ndk-r23b-linux.zip",
    # "http://10.136.104.231:4543/cmake-3.20.5-linux-x86_64.tar.gz"
]

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

TOOLS_DIR = "tools_download"
def download_third_party():
    os.system("mkdir {}".format(TOOLS_DIR))

    for link in THIRD_PARTY_LINK_LIST:
        download_cmd = "wget -c -t 3 -P {} {} --no-check-certificate".format(TOOLS_DIR, link)
        os.system(download_cmd)


THIRD_PARTY_DIR = "third_party"
def decopress_tools():
    package_list = os.listdir(TOOLS_DIR)
    print(package_list)
    for package in package_list:
        if package.endswith(".zip"):
            with zipfile.ZipFile(os.path.join(TOOLS_DIR, package), 'r') as z:
                if package == "protoc-3.13.0-linux-x86_64.zip":
                    z.extractall(os.path.join(THIRD_PARTY_DIR, "protoc-3.13.0"))
                else:
                    z.extractall(THIRD_PARTY_DIR)
        if package.endswith(".tar.gz"):
            with tarfile.open(os.path.join(TOOLS_DIR, package)) as t:
                if package == "core-refs_heads_master-libcutils-include-cutils.tar.gz":
                    t.extractall(os.path.join(THIRD_PARTY_DIR, "cutils"))
                else:
                    t.extractall(THIRD_PARTY_DIR)

    # ndk 加权限
    chmod_ndk = "chmod -R u+x {}".format("third_party/android-ndk-r20b")
    os.system(chmod_ndk)
    # chmod_ndk = "chmod -R u+x {}".format("third_party/android-ndk-r23b")
    # os.system(chmod_ndk)

    # cmake 加export
    # export_cmake_path = "export PATH=third_party/cmake-3.21.4-linux-x86_64/bin:$PATH"
    # os.system(export_cmake_path)
    export_cmake_path = "export PATH=third_party/cmake-3.20.5-linux-x86_64/bin:$PATH"
    os.system(export_cmake_path)
    # c_src 换目录名
    mv_c_sec_dir_name = "mv third_party/libboundscheck-1.1.11 third_party/bounds_checking_function"
    os.system(mv_c_sec_dir_name)
    # protobuf 换目录名
    mv_protobuf_dir_name = "mv third_party/protobuf-3.13.0 third_party/protobuf"
    os.system(mv_protobuf_dir_name)
    # protoc 加权限
    chmod_protoc = "chmod -R u+x {}".format("third_party/protoc-3.13.0")
    os.system(chmod_protoc)


if __name__ == '__main__':
    if os.path.exists(THIRD_PARTY_DIR):
        os.system("rm -rf {}".format(THIRD_PARTY_DIR))
    if os.path.exists(TOOLS_DIR):
        os.system("rm -rf {}".format(TOOLS_DIR))

    # 下载
    download_third_party()
    # 解压
    decopress_tools()
    # 编译
    os.system("./build.sh")
    # 打包