

import os
import sys

def FindAllFile(path):
    for root, ds, fs in os.walk(path):
        for f in fs:
            # fullname = f    # 文件名
            fullname = os.path.join(root, f)  # 文件完整路径
            yield fullname


def get_all_files():
    dir_list = ["api", "inc", "src", "tests"]

    open_source_file_list = "open_source_file_list.txt"
    file_list_txt = open(open_source_file_list, 'w')
    for dir in dir_list:
        for file_path in FindAllFile(dir):
            if file_path.endswith(".c") or \
                file_path.endswith(".cpp") or \
                file_path.endswith(".h") or \
                file_path.endswith(".cc") or \
                file_path.endswith(".proto") or \
                file_path.endswith(".om"):
                print(file_path)
                if file_path.startswith("tests"):
                    file_path = "test/framework/" + file_path.split('tests/')[1]
                file_list_txt.write(file_path + '\n')

    file_list_txt.close()
get_all_files()