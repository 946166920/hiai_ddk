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
import tarfile
import zipfile


ANDROID_NDK_PATH = "ANDROID_NDK_PATH"
CMAKE_MAKE_PROGRAM = "CMAKE_MAKE_PROGRAM"
ABI = "ABI"
BUILDTOOLS_DIR = "buildtools"
THIRD_PARTY_DIR = "third_party"


class HandleConfig(object):
    def __init__(self, config_file):
        self.config_file = config_file
        self.ndk_path = os.path.join(os.getcwd(), "buildtools", "android-ndk-r23b")
        self.is_configed_ndk = False
        self.cmake_path = os.path.join(os.getcwd(), "buildtools", "cmake-3.20.5")
        self.is_configed_cmake = False
        self.abi = "both"

        self.BUILDTOOLS_LINK_LIST = {
            "cmake": [
                "https://cmake.org/files/v3.20/cmake-3.20.5-linux-x86_64.tar.gz",
                "cmake-3.20.5-linux-x86_64.tar.gz"],
            "ndk": [
                "https://dl.google.com/android/repository/android-ndk-r23b-linux.zip",
                "android-ndk-r23b-linux.zip"],
            }
        self.THIRD_PARTY_LINK_LIST = {
            "cutils": [
                "https://mirrors.aliyun.com/android.googlesource.com/system/core/libcutils/include/cutils/native_handle.h",
                "cutils",
                self.decompress_cutils],
            "bounds_checking_function": [
                "https://github.com/openeuler-mirror/libboundscheck/archive/refs/tags/v1.1.11.zip",
                "libboundscheck-1.1.11.zip",
                self.decompress_bounds_checking_function],
            "protobuf": [
                "https://github.com/protocolbuffers/protobuf/archive/v3.13.0.zip",
                "protobuf-3.13.0.zip",
                self.decompress_protobuf],
            "mockcpp-2.7": [
                "https://github.com/sinojelly/mockcpp/archive/refs/tags/v2.7.zip",
                "mockcpp-2.7.zip",
                self.decompress_mockcpp],
            "googletest-release-1.8.1": [
                "https://codeload.github.com/google/googletest/tar.gz/release-1.8.1",
                "googletest-release-1.8.1.tar.gz",
                self.decompress_googletest],
            }


    def get_config(self):
        if not os.path.exists(self.config_file):
            print("[ERROR] : FAIL! config file {} is not exist.".format(self.config_file))
            return False

        self.read_config_file()
        
        if self.check_config_valid():
            self.download_third_party()
            self.chmod_script()
            return True
        else:
            return False


    def read_config_file(self):
        with open(self.config_file) as f:
            for line in f.readlines():
                if line.strip() == "" or line.strip().startswith("#"):
                    continue

                if len(line.split("=")) != 2:
                    print("[ERROR] : Config not valid :")
                    print("    {}".format(line))
                    return None

                config_tool = str(line.strip().split("=")[0].strip())
                config_context = str(line.strip().split("=")[1].strip())
                if config_tool == "ANDROID_NDK_PATH":
                    self.ndk_path = config_context
                    self.is_configed_ndk = True

                if config_tool == "CMAKE_MAKE_PROGRAM":
                    self.cmake_path = config_context
                    self.is_configed_cmake = True

                if config_tool == "ABI":
                    self.abi = config_context


    def check_config_valid(self):
        ndk_res = self.check_ndk_config()
        cmake_res = self.check_cmake_config()
        abi_res = self.check_abi_config()
        return ndk_res and cmake_res and abi_res


    def download_third_party(self):
        native_handle_h_path = os.path.join(THIRD_PARTY_DIR, "cutils", "native_handle.h")
        if not os.path.exists(native_handle_h_path):
            print("[ERROR] : FAIL! file {} is not exist.".format(native_handle_h_path))
            return False

        for item in self.THIRD_PARTY_LINK_LIST:
            if not os.path.exists(os.path.join(THIRD_PARTY_DIR, item)):
                if not os.path.exists(os.path.join(THIRD_PARTY_DIR, self.THIRD_PARTY_LINK_LIST[item][1])):
                    # download
                    self.download_package(THIRD_PARTY_DIR, self.THIRD_PARTY_LINK_LIST[item][0])
                # decompress
                self.THIRD_PARTY_LINK_LIST[item][2](self.THIRD_PARTY_LINK_LIST[item][1])

    # build tools
    # ndk
    def check_ndk_config(self):
        if self.is_configed_ndk:
            if not os.path.exists(self.ndk_path) or \
                not os.path.exists(os.path.join(self.ndk_path, "build", "cmake", "android.toolchain.cmake")):
                print("[ERROR] : FAIL! The path of the NDK is invalid.")
                return False
        else:
            if not os.path.exists(os.path.join(BUILDTOOLS_DIR, "android-ndk-r23b")):
                if not os.path.exists(os.path.join(BUILDTOOLS_DIR, self.BUILDTOOLS_LINK_LIST["ndk"][1])):
                    self.download_package(BUILDTOOLS_DIR, self.BUILDTOOLS_LINK_LIST["ndk"][0])

                print("[INFO] Decompressing package {} ...".format(self.BUILDTOOLS_LINK_LIST["ndk"][1]))
                unzip_cmd = "unzip -q -o -d {} {} ".format(
                    BUILDTOOLS_DIR, os.path.join(BUILDTOOLS_DIR, self.BUILDTOOLS_LINK_LIST["ndk"][1]))
                os.system(unzip_cmd)
                # add permissions to ndk executable files.
                chmod_ndk = "chmod -R u+x {}/android-ndk-r23b".format(BUILDTOOLS_DIR)
                os.system(chmod_ndk)
        return True


    # cmake
    def check_cmake_config(self):
        if self.is_configed_cmake:
            if not os.path.exists(self.cmake_path) or not os.path.exists(os.path.join(self.cmake_path, "bin", "cmake")):
                print("[ERROR] : FAIL! The path of the CMake is invalid.")
                return False
        else:
            if self.check_exists_in_system("cmake"):
                return True

            if not os.path.exists(os.path.join(BUILDTOOLS_DIR, "cmake-3.20.5")):
                if not os.path.exists(os.path.join(BUILDTOOLS_DIR, self.BUILDTOOLS_LINK_LIST["cmake"][1])):
                    self.download_package(BUILDTOOLS_DIR, self.BUILDTOOLS_LINK_LIST["cmake"][0])
                print("[INFO] Decompressing package {} ...".format(self.BUILDTOOLS_LINK_LIST["cmake"][1]))
                with tarfile.open(os.path.join(BUILDTOOLS_DIR, self.BUILDTOOLS_LINK_LIST["cmake"][1])) as t:
                    t.extractall(BUILDTOOLS_DIR)

                mv_cmake_dir_name = "mv {}/cmake-3.20.5-linux-x86_64 {}/cmake-3.20.5".format(BUILDTOOLS_DIR, BUILDTOOLS_DIR)
                os.system(mv_cmake_dir_name)

        self.cmake_path = os.path.join(self.cmake_path, "bin", "cmake")
        return True


    def check_abi_config(self):
        if self.abi not in ["armeabi-v7a", "arm64-v8a", "both"]:
            print("[ERROR] : FAIL! Invalid ABI configuration.")
            return False

        if self.abi == "both":
            self.abi = ["arm64-v8a","armeabi-v7a"]
        else:
            self.abi = [self.abi]   # str to list
        return True


    def check_exists_in_system(self, tool):
        # Check whether the cmake already exists
        which_cmd = os.popen('which {}'.format(tool))
        res = which_cmd.readlines()
        if len(res) != 0:
            path = res[0].strip()
            self.cmake_path = path
            return True
        return False

    # third_party
    def decompress_cutils(self, package):
        native_handle_h_path = os.path.join(THIRD_PARTY_DIR, package, "native_handle.h")
        if not os.path.exists(native_handle_h_path):
            download_path = os.path.join(THIRD_PARTY_DIR, "native_handle.h")
            if not os.path.exists(download_path):
                print("[ERROR] : FAIL! file {} is not exist.".format(download_path))
                sys.exit(-1)
            os.makedirs(os.path.join(THIRD_PARTY_DIR, package))
            mv_cmd = "mv {} {}".format(download_path, native_handle_h_path)
            os.system(mv_cmd)

    def decompress_protobuf(self, package):
        print("decompress_protobuf")
        with zipfile.ZipFile(os.path.join(THIRD_PARTY_DIR, package), 'r') as z:
            z.extractall(THIRD_PARTY_DIR)

        mv_protobuf_dir_name = "mv {}/protobuf-3.13.0 {}/protobuf".format(THIRD_PARTY_DIR, THIRD_PARTY_DIR)
        os.system(mv_protobuf_dir_name)


    def decompress_bounds_checking_function(self, package):
        with zipfile.ZipFile(os.path.join(THIRD_PARTY_DIR, package), 'r') as z:
            z.extractall(THIRD_PARTY_DIR)

        mv_c_sec_dir_name = "mv {}/libboundscheck-1.1.11 {}/bounds_checking_function".format(
            THIRD_PARTY_DIR, THIRD_PARTY_DIR)
        os.system(mv_c_sec_dir_name)
    

    def decompress_mockcpp(self, package):
        unzip_cmd = "unzip -q -o -d {} {} ".format(THIRD_PARTY_DIR,os.path.join(THIRD_PARTY_DIR, package))
        os.system(unzip_cmd)

    
    def decompress_googletest(self, package):
        with tarfile.open(os.path.join(THIRD_PARTY_DIR, package)) as t:
                t.extractall(THIRD_PARTY_DIR)


    def download_package(self, download_dir, link):
        download_cmd = "wget -c -t 3 -P {} {} --no-check-certificate".format(download_dir, link)
        print("[INFO] : Downloading...")
        os.system(download_cmd)
    

    def chmod_script(self):
        chmod_sh_py_script = 'find ./ -name "*.sh" -o -name "*.py" | xargs chmod u+x '
        os.system(chmod_sh_py_script)
