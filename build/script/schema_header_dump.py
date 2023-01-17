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

class SchemaHeaderDump(object):
    def __init__(self, opSchemaRepo):
        self.opSchemaRepo_ = opSchemaRepo

    def split_comments(self, comments):
        if comments.startswith("\""):
            comments = comments[1:len(comments) - 1]
            comments = comments.replace('\\\"', '\"')
        return comments.strip().split("\n")

    def dump_op_comments(self, f, op):
        if len(op.comments_) > 0:
            for comment in self.split_comments(op.comments_):
                f.write(" * " + comment + "\n")

    def dump_indent(self, f, indent):
        for i in range(indent):
            f.write(" ")

    def dump_comments(self, f, indent, comments):
        first_comment = True
        for comment in comments:
            if first_comment == True:
                f.write(comment)
                f.write("\n")
                first_comment = False
            else:
                f.write(" *")
                self.dump_indent(f, indent)
                f.write(comment)
                f.write("\n")

    def dump_comment(self, f, indent, name, comments):
        f.write(" *")
        self.dump_indent(f, indent)
        f.write(name)
        f.write(" : ")
        self.dump_comments(f, indent + len(" : ") + len(name), self.split_comments(comments))

    def dump_tensors_comment(self, f, tensorName, tensors):
        if len(tensors) > 0:
            f.write(" * <" + tensorName + ">\n")
            for tensor in tensors:
                self.dump_comment(f, 4, tensor.name_, tensor.comments_)

    def dump_graphs_comment(self, f, graphs):
        if len(graphs) > 0:
            f.write(" * <Graph>\n")
            for graph in graphs:
                self.dump_comment(f, 4, graph.name_, graph.comments_)

    def dump_attrs_comment(self, f, attrs):
        if len(attrs) > 0:
            f.write(" * <Attr>\n")
            for attr in attrs:
                self.dump_comment(f, 4, attr.name_, attr.comments_)

    def dump_addVersion_comment(self, f, addVersion):
        if addVersion != None:
            f.write(" * <Added in HiAI version>\n")
            f.write(" *    " + addVersion + "\n")

    def dump_example_comment(self, f, examples):
        if examples != None:
            f.write(" * <Examples>\n")
            f.write(" *    ")
            self.dump_comments(f, 0, self.split_comments(examples))

    def dump_op_comment(self, f, op):
        f.write("/*\n")

        self.dump_op_comments(f, op)
        self.dump_tensors_comment(f, "Input", op.inputs_)
        self.dump_tensors_comment(f, "Output", op.outputs_)
        self.dump_graphs_comment(f, op.graphs_)
        self.dump_attrs_comment(f, op.attrs_)
        self.dump_addVersion_comment(f, op.addVersion_)
        self.dump_example_comment(f, op.examples_)

        f.write(" */\n")

    def dump_input(self, f, input):
        f.write(".")
        if input.optional_ == True:
            f.write("OPTIONAL_INPUT")
        elif input.dynamic_ == True:
            f.write("DYNAMIC_INPUT")
        else:
            f.write("INPUT")
        f.write("(")
        f.write(input.name_)
        f.write(", ")
        f.write("TensorType({ ")
        f.write(input.types_)
        f.write(" })")
        f.write(")\n")

    def dump_inputs(self, f, inputs):
        for input in inputs:
            self.dump_input(f, input)

    def dump_output(self, f, output):
        f.write(".")
        if output.optional_ == True:
            f.write("OPTIONAL_OUTPUT")
        elif output.dynamic_ == True:
            f.write("DYNAMIC_OUTPUT")
        else:
            f.write("OUTPUT")
        f.write("(")
        f.write(output.name_)
        f.write(", ")
        f.write("TensorType({ ")
        f.write(output.types_)
        f.write(" })")
        f.write(")\n")

    def dump_outputs(self, f, outputs):
        for output in outputs:
            self.dump_output(f, output)

    def dump_graphs(self, f, graphs):
        for graph in graphs:
            if graph.dynamic_ == True:
                f.write(".DYNAMIC_GRAPH(")
            else:
                f.write(".REQUIRED_GRAPH(")
            f.write(graph.name_)
            f.write(")\n")

    def dump_attr(self, f, attr):
        f.write(".")
        if attr.required_ == True:
            f.write("REQUIRED_ATTR")
        else:
            f.write("ATTR")
        f.write("(")
        f.write(attr.name_)
        f.write(", AttrValue::")
        f.write(attr.type_)
        if attr.default_ != None:
            if attr.type_ == "STR":
                f.write(" {")
                if len(attr.default_) != 0:
                    f.write(" \"" + attr.default_ + "\" ")
                else:
                    f.write(" \"\" ")
                f.write("}")
            elif attr.type_ == "LIST":
                f.write(" ({")
                if len(attr.default_) != 0:
                    f.write(" " + attr.default_ + " ")
                f.write("})")
            elif attr.type_ == "LIST_INT":
                f.write(" ({")
                if len(attr.default_) != 0:
                    f.write(" " + attr.default_ + " ")
                f.write("})")
            elif attr.type_ == "LIST_FLOAT":
                f.write(" ({")
                if len(attr.default_) != 0:
                    f.write(" " + attr.default_ + " ")
                f.write("})")
            elif attr.type_ == "NamedAttrs":
                f.write("(")
                if len(attr.default_) != 0:
                    f.write(" " + attr.default_ + " ")
                f.write(")")
            elif attr.type_ == "TENSOR":
                f.write("(new (std::nothrow) Tensor(TensorDesc()))")
            else:
                f.write(" {")
                if len(attr.default_) != 0:
                    f.write(" " + attr.default_ + " ")
                f.write("}")
        f.write(")\n")

    def dump_attrs(self, f, attrs):
        for attr in attrs:
            self.dump_attr(f, attr)

    def dump_opReg(self, f, op):
        f.write("REG_OP(" + op.typeName_ + ")\n")

        self.dump_inputs(f, op.inputs_)
        self.dump_outputs(f, op.outputs_)
        self.dump_graphs(f, op.graphs_)
        self.dump_attrs(f, op.attrs_)

        f.write(".OP_END()\n")

    def dump_op(self, f, op):
        self.dump_op_comment(f, op)
        self.dump_opReg(f, op)

    def dump_header(self, f, file, macro):
        f.write(
            '/*\n' \
            ' * Copyright (c) Huawei Technologies Co., Ltd. 2018-2022. All rights reserved.\n' \
            ' * Description: {0}\n' \
            ' */\n' \
            '#ifndef {1}\n' \
            '#define {1}\n' \
            '#include "graph/operator_hiai_reg.h"\n' \
            '\n' \
            '// clang-format off\n' \
            'namespace hiai {{\n' \
            '\n'.format(file, macro.upper())
        )

    def dump_tailer(self, f):
        f.write(
            '} // namespace hiai\n'
            '// clang-format on\n'
            '\n'
            '#endif'
        )

    def dump(self, f, file, macro):
        self.dump_header(f, file, macro)
        for op in self.opSchemaRepo_.ops_:
            self.dump_op(f, op)
            f.write("\n")
        self.dump_tailer(f)