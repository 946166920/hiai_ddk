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
import sys
import zipfile
import tarfile
from xml.etree.ElementTree import parse

CONFIG_FILE = "build.conf"
BUILDTOOLS_DIR = "buildtools"
THIRD_PARTY_DIR = "third_party"
DDK_PACKAGE_FILE = "ddk.xml"

# Third party source code
def process_cutils(compressed_package):
    with tarfile.open(os.path.join(THIRD_PARTY_DIR, compressed_package)) as t:
        t.extractall(os.path.join(THIRD_PARTY_DIR, "cutils"))


def process_bounds_checking_function(compressed_package):
    with zipfile.ZipFile(os.path.join(THIRD_PARTY_DIR, compressed_package), 'r') as z:
        z.extractall(THIRD_PARTY_DIR)

    mv_c_sec_dir_name = "mv {}/libboundscheck-1.1.11 {}/bounds_checking_function".format(
        THIRD_PARTY_DIR, THIRD_PARTY_DIR)
    os.system(mv_c_sec_dir_name)


def process_protobuf(compressed_package):
    with zipfile.ZipFile(os.path.join(THIRD_PARTY_DIR, compressed_package), 'r') as z:
        z.extractall(THIRD_PARTY_DIR)

    mv_protobuf_dir_name = "mv {}/protobuf-3.13.0 {}/protobuf".format(THIRD_PARTY_DIR, THIRD_PARTY_DIR)
    os.system(mv_protobuf_dir_name)


def process_protoc(compressed_package):
    with zipfile.ZipFile(os.path.join(THIRD_PARTY_DIR, compressed_package), 'r') as z:
        z.extractall(os.path.join(THIRD_PARTY_DIR, "protoc-3.13.0"))
    # protoc 加权限
    chmod_protoc = "chmod -R u+x {}/protoc-3.13.0".format(THIRD_PARTY_DIR)
    os.system(chmod_protoc)

def process_mockcpp(compressed_package):
    with zipfile.ZipFile(os.path.join(THIRD_PARTY_DIR, compressed_package), 'r') as z:
        z.extractall(THIRD_PARTY_DIR)

def process_googletest(compressed_package):
    with tarfile.open(os.path.join(THIRD_PARTY_DIR, compressed_package)) as t:
        t.extractall(THIRD_PARTY_DIR)


# build tools
def process_ndk(compressed_package):
    with zipfile.ZipFile(os.path.join(BUILDTOOLS_DIR, compressed_package), 'r') as z:
        z.extractall(BUILDTOOLS_DIR)
    # ndk 加权限
    chmod_ndk = "chmod -R u+x {}/android-ndk-r20b".format(BUILDTOOLS_DIR)
    os.system(chmod_ndk)


def process_cmake(compressed_package):
    with tarfile.open(os.path.join(BUILDTOOLS_DIR, compressed_package)) as t:
        t.extractall(BUILDTOOLS_DIR)
    
    mv_cmake_dir_name = "mv {}/cmake-3.20.5-linux-x86_64 {}/cmake-3.20.5".format(BUILDTOOLS_DIR, BUILDTOOLS_DIR)
    os.system(mv_cmake_dir_name)
    # cmake 加export
    export_cmake_path = "export PATH={}/{}/cmake-3.20.5/bin:$PATH".format("$(pwd)", BUILDTOOLS_DIR)
    os.system(export_cmake_path)


THIRD_PARTY_LINK_LIST = {
    "cutils": ["http://10.136.104.34:5051/core-refs_heads_master-libcutils-include-cutils.tar.gz", process_cutils],
    "bounds_checking_function": ["http://10.136.104.34:5051/libboundscheck-1.1.11.zip", process_bounds_checking_function],
    "protobuf": ["http://10.136.104.34:5051/protobuf-3.13.0.zip", process_protobuf],
    "protoc-3.13.0": ["http://10.136.104.34:5051/protoc-3.13.0-linux-x86_64.zip", process_protoc],
    "mockcpp-2.7": ["http://10.136.104.34:5051/mockcpp-2.7.zip", process_mockcpp],
    "googletest-release-1.8.1": ["http://10.136.104.34:5051/googletest-release-1.8.1.tar.gz", process_googletest],
}

BUILDTOOLS_LINK_LIST = {
    "android-ndk-r20b": ["http://10.136.104.231:4543/android-ndk-r20b-linux-x86_64.zip", process_ndk],
    "cmake-3.20.5": ["http://10.136.104.231:4543/cmake-3.20.5-linux-x86_64.tar.gz", process_cmake],
    # "android-ndk-r23b": ["http://10.136.104.231:4543/android-ndk-r23b-linux.zip", process_ndk],
}


def run_process(package, link_list):
    compressed_package = link_list[package][0].split('/')[-1]
    process_func = link_list[package][1]
    process_func(compressed_package)


def config_dependence(dir, link_list):
    for dependence in link_list:
        if os.path.exists(os.path.join(dir, dependence)):
            continue
        download_cmd = "wget -c -t 3 -P {} {} --no-check-certificate".format(dir, link_list[dependence][0])
        os.system(download_cmd)
        compressed_package = link_list[dependence][0].split('/')[-1]
        process_func = link_list[dependence][1]
        process_func(compressed_package)


def download_third_party():
    if not os.path.exists(THIRD_PARTY_DIR):
        os.system("mkdir {}".format(THIRD_PARTY_DIR))
    config_dependence(THIRD_PARTY_DIR, THIRD_PARTY_LINK_LIST)

def read_config():
    config_dict = {}
    if not os.path.exists(CONFIG_FILE):
        return config_dict

    with open(CONFIG_FILE) as f:
        for line in f.readlines():
            if line.strip() == "" or line.strip().startswith("#"):
                continue

            if len(line.split("=")) != 2:
                print("[ERROR] : Config not valid :")
                print("    {}".format(line))
                return None
            config_tool = str(line.strip().split("=")[0].strip())
            config_context = str(line.strip().split("=")[1].strip())
            config_dict[config_tool] = config_context
    return config_dict


def is_config_valid(config_dict):
    if len(config_dict) == 0:
        return True
    
    if "ANDROID_NDK_PATH" in config_dict:
        ndk_path = config_dict["ANDROID_NDK_PATH"]
        if not os.path.exists(ndk_path) or \
            not os.path.exists(ndk_path, "build", "cmake", "android.toolchain.cmake"):
            return False

    if "CMAKE_MAKE_PROGRAM" in config_dict:
        cmake_path = config_dict["CMAKE_MAKE_PROGRAM"]
        if not os.path.exists(cmake_path) or \
           not os.path.exists(os.path.join(cmake_path, "bin", "cmake")):
            return False

    if "ABI" in config_dict:
        abi = config_dict["ABI"]
        if abi not in ["armeabi-v7a", "arm64-v8a", "both"]:
            return False
    return True

def download_buildtools(config_dict):
    need_download_list = BUILDTOOLS_LINK_LIST
    if "ANDROID_NDK_PATH" in config_dict:
        need_download_list.pop("android-ndk-r20b")
    if "CMAKE_MAKE_PROGRAM" in config_dict:
        need_download_list.pop("cmake-3.20.5")

    if len(need_download_list) > 0:
        if not os.path.exists(BUILDTOOLS_DIR):
            os.system("mkdir {}".format(BUILDTOOLS_DIR))
        config_dependence(BUILDTOOLS_DIR, BUILDTOOLS_LINK_LIST)

def get_buildtools_config(config_dict):
    py_cwd = os.getcwd()
    buildtools_config = {
        "ANDROID_NDK_PATH" : os.path.join(py_cwd, BUILDTOOLS_DIR, "android-ndk-r20b"),
        "CMAKE_MAKE_PROGRAM" : os.path.join(py_cwd, BUILDTOOLS_DIR, "cmake-3.20.5"),
        "ABI" : "arm64-v8a",
    }
    for config in config_dict:
        buildtools_config[config] = config_dict[config]
    
    if buildtools_config["ABI"] == "both":
        buildtools_config["ABI"] = ["arm64-v8a","armeabi-v7a"]
    else:
        buildtools_config["ABI"] = [buildtools_config["ABI"]]
    return buildtools_config

def set_environ(buildtools_config):
    # Add PATH Environment Variables
    cmake_bin_path = os.path.join(buildtools_config["CMAKE_MAKE_PROGRAM"], "bin", "cmake")
    print("cmake_bin_path :")
    print(cmake_bin_path)
    print(os.environ["PATH"])
    print("-----------------------------------")
    os.environ["PATH"] += os.pathsep + cmake_bin_path
    print(os.environ["PATH"])

def build(buildtools_config):
    cmake_bin = os.path.join(buildtools_config["CMAKE_MAKE_PROGRAM"], "bin", "cmake")
    ndk_path = buildtools_config["ANDROID_NDK_PATH"]
    toolchain = os.path.join(os.getcwd(),
        "build", "core", "himake", "hi_toolchain", "android-ndk.toolchain.cmake")

    if os.path.exists("out"):
        os.system("rm -r out")
    os.makedirs("out")
    os.chdir("out")

    for abi in buildtools_config["ABI"]:
        build_cmd = "{} .. " \
                    "-DNDK_PATH={} " \
                    "-DCMAKE_TOOLCHAIN_FILE={} " \
                    "-DANDROID_ABI={} " \
                    "-DANDROID_PLATFORM=android-19 ".format(
                    cmake_bin,
                    ndk_path,
                    toolchain,
                    abi)

        print("build_cmd:", build_cmd)
        os.system(build_cmd)
        make_cmd = "make -j"
        os.system(make_cmd)

    os.chdir(os.path.join(os.getcwd(), ".."))

def cp_item(class_config):
    for item in class_config:
        src_file = item.attrib['source']
        dest_file = item.attrib['destination']
        dest_dir = '/'.join(dest_file.split('/')[:-1])
        if not os.path.exists(dest_dir):
            os.makedirs(dest_dir)
        cp_cmd = "cp {} {}".format(src_file, dest_file)
        os.system(cp_cmd)

def package_ddk():
    if os.path.exists("ddk"):
        os.system("rm -rf ddk")

    f = open(DDK_PACKAGE_FILE, 'r', encoding='utf-8')
    root = parse(f).getroot()
    class_configs = list(root)
    # 拷贝头文件
    cp_item(class_configs[0])

    # 拷贝32位so
    if os.path.exists(os.path.join(os.getcwd(), "out", "hiai", "armeabi-v7a")):
        cp_item(class_configs[1])

    # 拷贝64位so
    if os.path.exists(os.path.join(os.getcwd(), "out", "hiai", "arm64-v8a")):
        cp_item(class_configs[2])
    f.close()

def CheckArgv(argv):
    run_test = True
    if len(argv) > 2:
        errmsg_invalid_para_count = \
        "[ERROR]: The number of command parameters more than 2.\n" \
        "[INFO] : You can run the following command to obtain more information:\n" \
        "         'python build.py --help'"
        print(errmsg_invalid_para_count)
        sys.exit(-1)
    if len(argv) == 2:
        if argv[1] == "--help":
            helpmessage = \
            "usage: [ python | python3 ] build.py [ --help | --only_ddk ]\n"\
            "--help      : print this help message and exit\n"\
            "--only_ddk   : run compile so command only.\n"\
            "If no option is available, both compile so and run the test code by default."
            print(helpmessage)
            sys.exit(-1)
        elif argv[1] == "--only_ddk":
            run_test = False
        else:
            errmsg_invalid_para = \
            "ERROR]: Unknown options and parameters.\n" \
            "[INFO] : You can run the following command to obtain more information:\n" \
            "         'python build.py --help'"
            print(errmsg_invalid_para)
            sys.exit(-1)
        
    return run_test

def RunTest():
    #Building the testBuild
    prj_root_path = os.getcwd()
    testBuild = os.path.join(prj_root_path, "tests", "build")
    if not os.path.exists(testBuild):
        os.makedirs(testBuild)

    os.chdir(testBuild)
    os.system("cmake {}".format(os.path.join(prj_root_path, "tests")))

    os.system("make -j4")
    if not os.path.exists(os.path.join(testBuild, "ut", "graph", "ut_graph")):
        print("[ERROR] : Building testcase failed! Test object has not generated!")
        sys.exit(-1)
    os.system("export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./gtest/lib/ && ./ut/graph/ut_graph")

    if not os.path.exists(os.path.join(testBuild, "ut", "model_manager", "ddk", "ddk_model_manager_ut")):
        print("[ERROR] : Building testcase failed! Test object has not generated!")
        sys.exit(-1)
    os.system("export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./gtest/lib/ && ./ut/model_manager/ddk/ddk_model_manager_ut")

    if not os.path.exists(os.path.join(testBuild, "ut", "model_manager", "direct_model_runtime", "direct_model_runtime_ut")):
        print("[ERROR] : Building testcase failed! Test object has not generated!")
        sys.exit(-1)
    os.system("export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./gtest/lib/ && ./ut/model_manager/direct_model_runtime/direct_model_runtime_ut")

    if not os.path.exists(os.path.join(testBuild, "ut", "model_manager", "model_manager_v2", "model_manager_v2_ut")):
        print("[ERROR] : Building testcase failed! Test object has not generated!")
        sys.exit(-1)
    os.system("export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./gtest/lib/ && ./ut/model_manager/model_manager_v2/model_manager_v2_ut")

    os.chdir(os.path.join(os.getcwd(), "../.."))

if __name__ == '__main__':
    run_test = CheckArgv(sys.argv)

    # 读取编译配置
    config_dict = read_config()
    if not is_config_valid(config_dict):
        print("[ERROR] : FAIL! Invalid configuration.")
        sys.exit(-1)
    
    # 下载编译工具
    download_buildtools(config_dict)
    # 下载三方源码
    download_third_party()

    # 编译
    buildtools_config = get_buildtools_config(config_dict)

    # 添加环境变量
    set_environ(buildtools_config)

    build(buildtools_config)

    # build test
    if run_test:
        RunTest()

    # 打包
    package_ddk()