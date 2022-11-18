#!/usr/bin/env python
# encoding: utf-8
# Copyright 2022 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================


import os
import sys
import zipfile
import tarfile

CONFIG_FILE = os.path.join("config", "build.conf")
BUILDTOOLS_DIR = "buildtools"
THIRD_PARTY_DIR = "third_party"
DDK_LIST = ["libhiai.so", "libhiai_ir.so", "libhiai_ir_build.so", "libhiai_ir_build_aipp.so"]

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


def process_mockcpp(compressed_package):
    unzip_cmd = "unzip -q -o -d {} {} ".format(THIRD_PARTY_DIR, os.path.join(THIRD_PARTY_DIR, compressed_package))
    os.system(unzip_cmd)


def process_googletest(compressed_package):
    with tarfile.open(os.path.join(THIRD_PARTY_DIR, compressed_package)) as t:
        t.extractall(THIRD_PARTY_DIR)


# build tools
def process_ndk(compressed_package):
    unzip_cmd = "unzip -q -o -d {} {} ".format(BUILDTOOLS_DIR, os.path.join(BUILDTOOLS_DIR, compressed_package))
    os.system(unzip_cmd)
    # ndk 加权限
    chmod_ndk = "chmod -R u+x {}/android-ndk-r23b".format(BUILDTOOLS_DIR)
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
    "cutils": [
        "https://android.googlesource.com/platform/system/core/+archive/refs/heads/master/libcutils/include/ \
            cutils.tar.gz",
        process_cutils],
    "bounds_checking_function": [
        "https://github.com/openeuler-mirror/libboundscheck/archive/refs/tags/v1.1.11.zip",
        process_bounds_checking_function],
    "protobuf": [
        "https://github.com/protocolbuffers/protobuf/archive/v3.13.0.zip",
        process_protobuf],
    "mockcpp-2.7": [
        "https://github.com/sinojelly/mockcpp/archive/refs/tags/v2.7.zip",
        process_mockcpp],
    "googletest-release-1.8.1": [
        "https://codeload.github.com/google/googletest/tar.gz/release-1.8.1",
        process_googletest],
}

BUILDTOOLS_LINK_LIST = {
    "cmake-3.20.5": [
        "https://cmake.org/files/v3.20/cmake-3.20.5-linux-x86_64.tar.gz",
        process_cmake],
    "android-ndk-r23b": [
        "https://dl.google.com/android/repository/android-ndk-r23b-linux.zip",
        process_ndk],
}

def run_process(package, link_list):
    compressed_package = link_list[package][0].split('/')[-1]
    process_func = link_list[package][1]
    process_func(compressed_package)


def config_dependence(dir, link_list):
    for dependence in link_list:
        package_name = link_list[dependence][0].split('/')[-1]
        # 没有压缩包，需要下载后解压；如果压缩包已存在，则直接解压
        if not os.path.exists(os.path.join(dir, package_name)):
            download_cmd = "wget -c -t 3 -P {} {} --no-check-certificate".format(dir, link_list[dependence][0])
            os.system(download_cmd)
        compressed_package = link_list[dependence][0].split('/')[-1]
        print("[INFO] Decompressing package {} ...".format(compressed_package))
        if os.path.exists(os.path.join(dir, dependence)):
            os.system("rm -rf {}".format(os.path.join(dir, dependence)))
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
            not os.path.exists(os.path.join(ndk_path, "build", "cmake", "android.toolchain.cmake")):
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
        need_download_list.pop("android-ndk-r23b")
    if "CMAKE_MAKE_PROGRAM" in config_dict:
        need_download_list.pop("cmake-3.20.5")

    if len(need_download_list) > 0:
        if not os.path.exists(BUILDTOOLS_DIR):
            os.system("mkdir {}".format(BUILDTOOLS_DIR))
        config_dependence(BUILDTOOLS_DIR, BUILDTOOLS_LINK_LIST)

def get_buildtools_config(config_dict):
    py_cwd = os.getcwd()
    buildtools_config = {
        "ANDROID_NDK_PATH" : os.path.join(py_cwd, BUILDTOOLS_DIR, "android-ndk-r23b"),
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
    os.environ["PATH"] += os.pathsep + cmake_bin_path


def build_protoc():
    protoc_target_path = "third_party/protoc/bin/protoc"
    if os.path.exists(protoc_target_path):
        return True

    cwd = os.getcwd()
    os.chdir("third_party/protobuf")

    os.system("chmod u+x autogen.sh")
    os.system("./autogen.sh")
    
    build_protoc_path = "protoc-artifacts/build-protoc.sh"
    os.system("chmod u+x {}".format(build_protoc_path))
    build_protoc_cmd = "./{} linux x86_64 protoc".format(build_protoc_path)
    os.system(build_protoc_cmd)

    os.chdir(cwd)

    protoc_target_dir = "third_party/protoc/bin"
    if not os.path.exists(protoc_target_dir):
        os.makedirs(protoc_target_dir)
    
    protoc_gen_path = "third_party/protobuf/protoc-artifacts/target/linux/x86_64/protoc.exe"
    if not os.path.exists(protoc_gen_path):
        return False

    cp_protoc_cmd = "cp {} {}".format(protoc_gen_path, protoc_target_dir)
    os.system(cp_protoc_cmd)
    
    rename_cmd = "mv {}/protoc.exe {}".format(protoc_target_dir, protoc_target_path)
    os.system(rename_cmd)

    # protoc 加权限
    chmod_protoc = "chmod u+x {}".format(protoc_target_path)
    os.system(chmod_protoc)

    return True

def chmod_script():
    chmod_sh_script = "find . -name *.sh | xargs chmod u+x "
    os.system(chmod_sh_script)
    chmod_py_script = "find . -name *.py | xargs chmod u+x "
    os.system(chmod_py_script)

def build(buildtools_config):
    if not build_protoc():
        print("[ERROR:] build protoc failed!")        
        return False
    # 添加环境变量
    set_environ(buildtools_config)

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

        os.system(build_cmd)
        make_cmd = "make -j"
        os.system(make_cmd)

    os.chdir(os.path.join(os.getcwd(), ".."))

    for abi in buildtools_config["ABI"]:
        for so_name in DDK_LIST:
            if os.path.exists(os.path.join(os.getcwd(), "out", "hiai", abi, "lib", so_name)):
                print("[INFO] : PASS! {} has successfully generated!".format(so_name))
            else:
                return False

    return True


def check_argv(argv):
    is_run_test = True
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
            is_run_test = False
        else:
            errmsg_invalid_para = \
            "ERROR]: Unknown options and parameters.\n" \
            "[INFO] : You can run the following command to obtain more information:\n" \
            "         'python build.py --help'"
            print(errmsg_invalid_para)
            sys.exit(-1)
        
    return is_run_test

def run_test(buildtools_config):
    #Building the testBuild
    prj_root_path = os.getcwd()
    testBuild = os.path.join(prj_root_path, "tests", "build")
    if os.path.exists(testBuild):
        os.system("rm -r {}".format(testBuild))
    os.makedirs(testBuild)

    os.chdir(testBuild)
    cmake_bin = os.path.join(buildtools_config["CMAKE_MAKE_PROGRAM"], "bin", "cmake")
    os.system("{} ..".format(cmake_bin, os.path.join(prj_root_path, "tests")))
    os.system("make -j")

    UT_LIST = [ os.path.join(testBuild, "ut", "graph", "ut_graph"),
                os.path.join(testBuild, "ut", "model_manager", "ddk", "ddk_model_manager_ut"),
                os.path.join(testBuild, "ut", "model_manager", "direct_model_runtime", "direct_model_runtime_ut"),
                os.path.join(testBuild, "ut", "model_manager", "model_manager_v2", "model_manager_v2_ut"),
    ]
    for ut in UT_LIST:
        if not os.path.exists(ut):
            print("[ERROR] : Building testcase failed! Test object {} has not generated!".format(ut))
            sys.exit(-1)
        os.system("export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./gtest/lib/ && {}".format(ut))

    os.chdir(os.path.join(os.getcwd(), "../.."))

if __name__ == '__main__':
    is_run_test = check_argv(sys.argv)

    # 读取编译配置
    config_dict = read_config()
    if not is_config_valid(config_dict):
        print("[ERROR] : FAIL! Invalid configuration.")
        sys.exit(-1)
    
    # 下载编译工具
    download_buildtools(config_dict)
    # 下载三方源码
    download_third_party()

    # 获取编译配置
    buildtools_config = get_buildtools_config(config_dict)

    chmod_script()

    # build
    if not build(buildtools_config):
        print("[ERROR] : FAIL! build error.")
        sys.exit(-1)

    # build test
    if is_run_test:
        run_test(buildtools_config)

    # 打包
    os.system("python3 script/repack_ddk.py")