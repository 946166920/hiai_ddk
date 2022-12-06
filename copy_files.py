import os
import sys

open_source_file_dir = "OpenSource"

def special_handle():
    # 原先开源下的inc/framework/infra/mem 移动到api/infra/base
    os.makedirs("OpenSource/api/infra/base")
    h_list = ["identity.h", "inline_variable.h", "in_place.h", "static_storage.h", "maybe.h"]
    for h_file in h_list:
        cp_cmd = "cp api/infra/base/{} OpenSource/api/infra/base".format(h_file)
        os.system(cp_cmd)
    # node_modifier.h node_modifier.cpp文件已删除了


# 在vendor/hisi/npu目录下执行
def copy_files():
    open_source_file_list = "open_source_file_list.txt"
    file_list_txt = open(open_source_file_list, 'r')

    if not os.path.exists(open_source_file_dir):
        os.mkdir(open_source_file_dir)
    for file_path in file_list_txt.readlines():
        if file_path.startswith("#"):
            continue
        file_path = file_path.strip()
        # print(file_path)
        dest_file = file_path
        if dest_file.startswith("test/framework/"):
            dest_file = "tests/" + file_path.split('test/framework/')[1]
        dest_file = os.path.join(open_source_file_dir, dest_file)
        # print("dest_file:", dest_file)
        dest_dir = os.sep.join(dest_file.split(os.sep)[:-1])
        if not os.path.exists(dest_dir):
            os.makedirs(dest_dir)
        cp_cmd = "cp {} {}".format(file_path, dest_file)
        print(cp_cmd)
        os.system(cp_cmd)


copy_files()
# special_handle()