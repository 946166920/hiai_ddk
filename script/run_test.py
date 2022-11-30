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

class RunTest(object):
    def __init__(self, cmake_bin_path):
        self.cmake_bin_path = cmake_bin_path
        

    def run_test(self):
        prj_root_path = os.getcwd()
        test_build_dir = os.path.join(prj_root_path, "tests", "build")
        if os.path.exists(test_build_dir):
            os.system("rm -r {}".format(test_build_dir))
        os.makedirs(test_build_dir)
        os.chdir(test_build_dir)

        os.system("{} ..".format(self.cmake_bin_path))
        os.system("make -j")

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
            os.system("export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./gtest/lib/ && {}".format(ut))

        os.chdir(os.path.join(os.getcwd(), "../../"))

        if os.path.exists(test_build_dir):
            os.system("rm -rf {}".format(test_build_dir))

        test_out_dir = os.path.join(prj_root_path, "tests", "out")
        if os.path.exists(test_out_dir):
            os.system("rm -rf {}".format(test_out_dir))
