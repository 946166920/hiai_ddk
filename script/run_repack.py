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
import subprocess
import zipfile
import xml.etree.ElementTree as ET

DDK_PACKAGE_NAME = 'hwhiai-ddk-master'
DDK_ZIP_NAME = DDK_PACKAGE_NAME + '.zip'
DDK_RELEASE_DIR = os.path.join('out', DDK_PACKAGE_NAME)
DDK_PACKAGE_FILE = os.path.join('config', 'ddk.xml')
DDK_LIST = ["libhiai.so", "libhiai_ir.so", "libhiai_ir_build.so", "libhiai_ir_build_aipp.so"]


class repack_ddk(object):
    def __init__(self, abi):
        self.abi = abi


    def repack(self):
        self.check_so()
        self.pack_ddk_files()
        self.gen_ddk_package()


    def check_so(self):
        for abi in self.abi:
            for so_name in DDK_LIST:
                if os.path.exists(os.path.join(os.getcwd(), "out", "hiai", abi, "lib", so_name)):
                    print("[INFO] : PASS! {} has successfully generated!".format(so_name))
                else:
                    print("[ERROR] : PASS! {} is not generated!".format(so_name))
                    sys.exit(-1)


    def pack_ddk_files(self):
        if os.path.exists(DDK_RELEASE_DIR):
            os.system("rm -rf {}".format(DDK_RELEASE_DIR))

        f = open(DDK_PACKAGE_FILE, 'r', encoding='utf-8')
        root = ET.parse(f).getroot()
        class_configs = list(root)
        # copy head files
        self.cp_item(class_configs[0])

        # copy the so file of the armeabi-v7a architecture
        if os.path.exists(os.path.join(os.getcwd(), "out", "hiai", "armeabi-v7a")):
            self.cp_item(class_configs[1])

        # copy the so file of the arm64-v8a architecture
        if os.path.exists(os.path.join(os.getcwd(), "out", "hiai", "arm64-v8a")):
            self.cp_item(class_configs[2])
        
        f.close()


    def gen_ddk_package(self):
        ddk_zip = zipfile.ZipFile(DDK_ZIP_NAME, 'w', zipfile.ZIP_DEFLATED)
        for path, dirnames, filenames in os.walk(DDK_RELEASE_DIR):
            zip_path = path.replace(DDK_RELEASE_DIR, DDK_PACKAGE_NAME)
            for filename in filenames:
                ddk_zip.write(os.path.join(path, filename), os.path.join(zip_path, filename))
        ddk_zip.close()
        print('[INFO] Packaged successfully!')


    def cp_item(self, class_config):
        for item in class_config:
            src_file = item.attrib['source']
            dest_file = os.path.join(DDK_RELEASE_DIR, item.attrib['destination'])

            dest_dir = os.sep.join(dest_file.split(os.sep)[:-1])

            if not os.path.exists(dest_dir):
                os.makedirs(dest_dir)
            
            if 'is_open' in item.attrib and item.attrib['is_open'] == 'false':
                continue
            
            if 'has_macros' in item.attrib and item.attrib['has_macros'] == 'true':
                is_defif = False
                new_context = []
                with open(src_file, 'r') as f:
                    for line in f.readlines():
                        if line.startswith('#ifdef'):
                            is_defif = True
                        if is_defif and line.startswith('#endif'):
                            is_defif = False
                            continue
                        if is_defif:
                            continue
                        new_context.append(line)
            
                with open(dest_file, 'w') as f:
                    f.writelines(new_context)
                continue

            cp_cmd = "cp {} {}".format(src_file, dest_file)
            os.system(cp_cmd)