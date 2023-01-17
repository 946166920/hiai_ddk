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


class RunBuild(object):
    def __init__(self, build_config):
        self.build_config = build_config


    def build_ddk(self):
        if not self.build_protoc():
            print("[ERROR:] build protoc failed!")        
            return False
        self.build()


    def build_protoc(self):
        protoc_target_path = "third_party/protoc/bin/protoc"
        if os.path.exists(protoc_target_path):
            return True

        cwd = os.getcwd()
        os.chdir("third_party/protobuf")

        os.system("./autogen.sh")
        
        build_protoc_path = "protoc-artifacts/build-protoc.sh"
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

        chmod_protoc = "chmod u+x {}".format(protoc_target_path)
        os.system(chmod_protoc)

        return True


    def build(self):
        toolchain = os.path.join(os.getcwd(),
            "build", "core", "himake", "hi_toolchain", "android-ndk.toolchain.cmake")

        if os.path.exists("out"):
            os.system("rm -r out")
        os.makedirs("out")
        os.chdir("out")

        for abi in self.build_config.abi:
            build_cmd = "{} .. " \
                        "-DNDK_PATH={} " \
                        "-DCMAKE_TOOLCHAIN_FILE={} " \
                        "-DANDROID_ABI={} " \
                        "-DANDROID_PLATFORM=android-19 ".format(
                        self.build_config.cmake_path,
                        self.build_config.ndk_path,
                        toolchain,
                        abi)

            print("build_cmd:", build_cmd)
            os.system(build_cmd)
            make_cmd = "make -j"
            os.system(make_cmd)

        os.chdir(os.path.join(os.getcwd(), ".."))