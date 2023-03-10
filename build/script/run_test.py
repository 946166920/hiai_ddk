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
import shutil

class RunTest(object):
    def __init__(self, cmake_bin_path):
        self.cmake_bin_path = cmake_bin_path

    def run_test(self):
        prj_root_path = os.getcwd()

        test_build_dir = os.path.join(prj_root_path, "test", "framework", "build")
        if os.path.exists(test_build_dir):
            os.system("rm -r {}".format(test_build_dir))
        os.makedirs(test_build_dir)

        bin_src_dir = os.path.join(prj_root_path, "test", "framework", "bin")
        bin_dst_dir = os.path.join(test_build_dir, "bin", "llt", "framework", "domi")
        os.makedirs(bin_dst_dir)

        bin_dst_dir = os.path.join(bin_dst_dir, "modelmanager")
        os.symlink(bin_src_dir, bin_dst_dir)

        test_out_dir = os.path.join(test_build_dir, "out")
        if os.path.exists(test_out_dir):
            os.system("rm -r {}".format(test_out_dir))
        os.makedirs(test_out_dir)

        os.chdir(test_build_dir)

        self.special_handle(prj_root_path)
        os.system("{} ..".format(self.cmake_bin_path))
        os.system("make -j")
        self.recover_special_handle(prj_root_path)

        UT_LIST = [
            os.path.join(test_build_dir, "ut", "model_manager", "ddk", "ddk_model_manager_ut"),
            os.path.join(test_build_dir, "ut", "model_manager", "direct_model_runtime", "direct_model_runtime_ut"),
            os.path.join(test_build_dir, "ut", "model_manager", "model_manager_v2", "model_manager_v2_ut"),
            os.path.join(test_build_dir, "ut", "graph", "ut_graph"),
        ]
        for ut in UT_LIST:
            if not os.path.exists(ut):
                print("[ERROR] : Building testcase failed! Test object {} has not generated!".format(ut))
                sys.exit(-1)
            os.system("export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./gtest/lib/:./stubs/depends/hiai_ddk/ && {}".format(ut))

        os.chdir(os.path.join(os.getcwd(), "../../"))

        if os.path.exists(test_build_dir):
            os.system("rm -rf {}".format(test_build_dir))


    def special_handle(self, prj_root_path):
        # handle hiai_base_type.h
        hiai_base_types_path = os.path.join(prj_root_path, "inc", "framework", "c", "hiai_base_types.h")
        file_data = ""
        with open(hiai_base_types_path, "r", encoding="utf-8") as f:
            for line in f:
                if "HIAI_FORMAT_RESERVED" in line:
                    file_data += "HIAI_FORMAT_NCDHW = 40,\n"
                    file_data += "HIAI_FORMAT_NDHWC,\n"
                file_data += line
        with open(hiai_base_types_path, "w", encoding="utf-8") as f:
            f.write(file_data)

        # handle ge_model_unittest.cpp
        ge_model_unittest_path = os.path.join(prj_root_path, "test", "framework", "ut", "graph",
            "testcase", "ge_ir", "ge_model_unittest.cpp")
        file_data = ""
        with open(ge_model_unittest_path, "r", encoding="utf-8") as f:
            for line in f:
                if line.startswith("#define") or line.startswith("#undef"):
                    line = "//" + line
                file_data += line
        with open(ge_model_unittest_path, "w", encoding="utf-8") as f:
            f.write(file_data)

    def recover_special_handle(self, prj_root_path):
        # recover hiai_base_type.h
        hiai_base_types_path = os.path.join(prj_root_path, "inc", "framework", "c", "hiai_base_types.h")
        file_data = ""
        with open(hiai_base_types_path, "r", encoding="utf-8") as f:
            for line in f:
                if line.startswith("HIAI_FORMAT_NCDHW") or line.startswith("HIAI_FORMAT_NDHWC"):
                    continue
                file_data += line
        with open(hiai_base_types_path, "w", encoding="utf-8") as f:
            f.write(file_data)

        # recover ge_model_unittest.cpp
        ge_model_unittest_path = os.path.join(prj_root_path, "test", "framework", "ut", "graph",
            "testcase", "ge_ir", "ge_model_unittest.cpp")
        file_data = ""
        with open(ge_model_unittest_path, "r", encoding="utf-8") as f:
            for line in f:
                if line.startswith("//#define") or line.startswith("//#undef"):
                    line = line.split("//")[1]
                file_data += line
        with open(ge_model_unittest_path, "w", encoding="utf-8") as f:
            f.write(file_data)
