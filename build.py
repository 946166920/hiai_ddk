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

sys.path.append("script")
from handle_config import *
from handle_ddk import *


CONFIG_FILE = os.path.join("config", "build.conf")


def check_argv(argv):
    is_run_build = True
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
            "usage: python3 build.py [ --help | --ddk | --test]\n"\
            "--help      : print this help message and exit\n"\
            "--ddk       : run compile so command only.\n"\
            "--test      : run test code only.\n"\
            "If no option is available, both compile so and run the test code by default."
            print(helpmessage)
            sys.exit(-1)
        elif argv[1] == "--ddk":    # run ddk only
            is_run_build = True
            is_run_test = False
        elif argv[1] == "--test":   # run test only
            is_run_build = False
            is_run_test = True
        else:
            errmsg_invalid_para = \
            "ERROR]: Unknown options and parameters.\n" \
            "[INFO] : You can run the following command to obtain more information:\n" \
            "         'python build.py --help'"
            print(errmsg_invalid_para)
            sys.exit(-1)
        
    return is_run_build, is_run_test

if __name__ == '__main__':
    is_run_build, is_run_test = check_argv(sys.argv)
    config = HandleConfig(CONFIG_FILE)

    # read build config
    if not config.get_config():
        print("get_config error.")

    # init
    ddk = HandleDDK(config)

    # run build and package
    if is_run_build:
        # ddk.run_build()
        ddk.run_repack()

    # run test
    if is_run_test:
        ddk.run_test()
