/**
 * Copyright 2019-2022 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INC_FRAMEWORK_GRAPH_OP_CONTROL_FLOW_ATTR_DEFS_H
#define INC_FRAMEWORK_GRAPH_OP_CONTROL_FLOW_ATTR_DEFS_H

namespace hiai {
// 控制流定义
// control flow operator
const char * const SWITCH = "Switch";
const char * const MERGE = "Merge";
const char * const ENTER = "Enter";
const char * const EXIT = "Exit";
const char * const NEXTITERATION = "NextIteration";
const char * const LOOPCOND = "LoopCond";
const char * const FAKE_PARAM = "FakeParam";
const char * const WHILE = "While";
const char * const IF = "If";

// if operator attr
const char * const ATTR_NAME_THEN_BRANCH = "then_branch";
const char * const ATTR_NAME_ELSE_BRANCH = "else_branch";
const char * const ATTR_NAME_THEN_BRANCH_GRAPH = "then_branch_graph";
const char * const ATTR_NAME_ELSE_BRANCH_GRAPH = "else_branch_graph";

// while operator attr
const char * const ATTR_NAME_IS_SUB_GRAPH = "is_sub_graph";
const char * const ATTR_NAME_COND = "cond";
const char * const ATTR_NAME_BODY = "body";
const char * const ATTR_NAME_COND_GRAPH = "cond_graph";
const char * const ATTR_NAME_BODY_GRAPH = "body_graph";
const char * const ATTR_NAME_OUTPUTS_SHAPES = "output_shapes";

// function operator
const char * const ARG = "_Arg";
const char * const RETVAL = "_Retval";
} // namespace hiai

#endif // INC_FRAMEWORK_GRAPH_OP_CONTROL_FLOW_ATTR_DEFS_H
