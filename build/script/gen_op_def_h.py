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

from schema_parser import *
from schema_header_dump import *


def main(root_path, target_component):
    schema_files = {
        'api': ['api/framework/graph/op', 'inc_api_graph_op_', 'graph/op/',
            ['array_defs', 'const_defs', 'control_flow_defs', 'detection_defs', 'image_defs',
            'math_defs', 'nn_defs', 'random_defs']],
        'inc': ['inc/framework/graph/op', 'inc_framework_graph_op_', 'framework/graph/op/',
            ['data_flow_defs', 'internal_defs', 'internal_nn_defs']]
    }

    schema_file = schema_files[target_component]
    path = os.path.join(root_path, schema_file[0])

    # 头文件生成路径
    out_h_gen_dir = os.path.join(root_path, "out", "opDefs", schema_file[2])
    if not os.path.exists(out_h_gen_dir):
        os.makedirs(out_h_gen_dir)
    
    macro = schema_file[1]
    for file in schema_file[3]:
        dest_head_file = os.path.join(path, file + '.h')

        yaml_path = os.path.join(path, file + '.yaml')

        yaml_file = open(yaml_path, 'r')
        yaml_parser = YamlSchemaParser()
        yaml_result = yaml_parser.parse(yaml_file)
        yaml_file.close()

        generate_header_path = os.path.join(out_h_gen_dir,  file + '.h')

        head_file = open(generate_header_path, 'w')
        header_macro = macro + file + "_h"
        dumper = SchemaHeaderDump(yaml_result)
        dumper.dump(head_file, file, header_macro)
        head_file.close()

        os.system("cp " + generate_header_path + " " + path)

if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])