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

DDK_INFO_CONFIG_FILE = os.path.join('config', 'info.xml')
DDK_PACKAGE_NAME = 'hwhiai-ddk-100.520.020.010'
DDK_RELEASE_DIR = os.path.join('out', DDK_PACKAGE_NAME)
DDK_PACKAGE_FILE = os.path.join('config', 'ddk.xml')
DDK_LIST = ["libhiai.so", "libhiai_ir.so", "libhiai_ir_build.so", "libhiai_ir_build_aipp.so"]
DDK_ZIP_NAME = 'hwhiai-ddk-100.520.020.010.zip'

def cp_item(class_config):
    for item in class_config:
        src_file = item.attrib['source']
        dest_file = os.path.join(DDK_RELEASE_DIR, 'ddk','ai_ddk_lib', item.attrib['destination'])

        dest_dir = os.sep.join(dest_file.split(os.sep)[:-1])

        if not os.path.exists(dest_dir):
            os.makedirs(dest_dir)
        cp_cmd = "cp {} {}".format(src_file, dest_file)
        os.system(cp_cmd)

def package_ddk_files():
    if os.path.exists(DDK_RELEASE_DIR):
        os.system("rm -rf {}".format(DDK_RELEASE_DIR))

    f = open(DDK_PACKAGE_FILE, 'r', encoding='utf-8')
    root = ET.parse(f).getroot()
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


def gen_ddk_info():
    '''
    Generate ddk_info.txt
    '''
    if not os.path.isfile(DDK_INFO_CONFIG_FILE):
        print('Error: ddk repack, Can not get ddk_info file:', DDK_INFO_CONFIG_FILE)
        return False

    root = ET.parse(DDK_INFO_CONFIG_FILE).getroot()

    # get file_name
    if 'file_name' in root.attrib:
        ddk_info_file = os.path.join(DDK_RELEASE_DIR, root.attrib['file_name'])
    else:
        print('Error: can not get ddk_info file name !')
        return False

    # get content
    content = '{\n'
    for child in root:
        content = '%s%s%s%s%s%s' % (content, '"', child.tag, '":"', child.text, '",\n')
    content += '}\n'

    if os.path.exists(ddk_info_file):
        cmd = 'rm -rf ' + ddk_info_file
        subprocess.run(cmd, check=True, shell=True, stdout=subprocess.PIPE)
    try:
        with open(ddk_info_file, 'a') as ddk_info_write:
            ddk_info_write.write(content)
    except:
        print('Error: gen ddk_info file fail!')
        return False

    return True


def gen_ddk_package():
    ddk_zip = zipfile.ZipFile(DDK_ZIP_NAME, 'w', zipfile.ZIP_DEFLATED)
    for path, dirnames, filenames in os.walk(DDK_RELEASE_DIR):
        zip_path = path.replace(DDK_RELEASE_DIR, DDK_PACKAGE_NAME)
        for filename in filenames:
            ddk_zip.write(os.path.join(path, filename), os.path.join(zip_path, filename))
    ddk_zip.close()
    print('[INFO] Packaged successfully!')


if __name__ == "__main__":
    package_ddk_files()
    gen_ddk_info()
    gen_ddk_package()
