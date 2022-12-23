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

class TensorSchema(object):
    def __init__(self, name, types, comments):
        self.name_ = name
        self.types_ = types
        self.optional_ = False
        self.dynamic_ = False
        self.comments_ = comments


class AttrSchema(object):
    def __init__(self, name, type, comments):
        self.name_ = name
        self.type_ = type
        self.required_ = False
        self.default_ = None
        self.comments_ = comments


class GraphSchema(object):
    def __init__(self, name, comments):
        self.name_ = name
        self.comments_ = comments


class OpSchema(object):
    def __init__(self, typeName):
        self.typeName_ = typeName
        self.comments_ = None
        self.addVersion_ = None
        self.inputs_ = []
        self.outputs_ = []
        self.graphs_ = []
        self.attrs_ = []
        self.examples_ = None


class OpSchemaRepo(object):
    def __init__(self, category):
        self.category_ = category
        self.ops_ = []


class YamlSchemaParser(object):
    def __init__(self):
        pass

    def parse_comments(self, comments, op):
        op.comments_ = comments.strip()

    def parse_addVersion(self, addVersion, op):
        op.addVersion_ = addVersion

    def parse_tensors(self, tensors, tensorRepo):
        for k, v in tensors.items():
            ts = TensorSchema(k, v["tensor_types"], v["comment"].strip())
            if "optional" in v:
                ts.optional_ = v["optional"]
            if "dynamic" in v:
                ts.dynamic_ = v["dynamic"]
            tensorRepo.append(ts)

    def parse_graphs(self, graphs, graphRepo):
        for k, v in graphs.items():
            gs = GraphSchema(k, v["comment"].strip())
            graphRepo.append(gs)

    def parse_attrs(self, attrs, attrRepo):
        for k, v in attrs.items():
            a_s = AttrSchema(k, v["type"], v["comment"].strip())
            if "required" in v:
                a_s.required_ = v["required"]
            if "default" in v:
                a_s.default_ = v["default"]
            attrRepo.append(a_s)

    def parse_ops(self, ops, repo):
        for k, v in ops.items():
            # print("parse op: [" + k + "]")
            op = OpSchema(k)

            if "comment" in v:
                self.parse_comments(v["comment"], op)
            if "add_version" in v:
                self.parse_addVersion(v["add_version"], op)
            if "inputs" in v:
                self.parse_tensors(v["inputs"], op.inputs_)
            if "outputs" in v:
                self.parse_tensors(v["outputs"], op.outputs_)
            if "graphs" in v:
                self.parse_graphs(v["graphs"], op.graphs_)
            if "attrs" in v:
                self.parse_attrs(v["attrs"], op.attrs_)
            if "examples" in v:
                op.examples_ = v["examples"]

            repo.ops_.append(op)

    def parse(self, f):
        content = yaml.load(f)
        for k, v in content.items():
            repo = OpSchemaRepo(k)
            # print("parse op category: [" + k + "]")
            self.parse_ops(v, repo)
            return repo
