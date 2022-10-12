/*
  Copyright (C) Huawei Technologies Co., Ltd.  2017-2021.  All rights reserved.
 * Description: data flow attr defs
 */

#ifndef INC_FRAMEWORK_GRAPH_OP_DATA_FLOW_ATTR_DEFS_H
#define INC_FRAMEWORK_GRAPH_OP_DATA_FLOW_ATTR_DEFS_H

namespace hiai {
// 数据流的定义
// resource operator
const char * const TENSORARRAY = "TensorArray";
const char * const TENSORARRAYREAD = "TensorArrayRead";
const char * const TENSORARRAYWRITE = "TensorArrayWrite";
const char * const TENSORARRAYGATHER = "TensorArrayGather";
const char * const TENSORARRAYSIZE = "TensorArraySize";
const char * const TENSORARRAYSCATTER = "TensorArrayScatter";
const char * const TENSORARRAYCLOSE = "TensorArrayClose";

// resource operator attr
const char * const TENSORARRAY_ADDR_OFFSET = "tensor_array_addr_offset";
const char * const ATTR_NAME_ELEMENT_SHAPE = "element_shape";
const char * const ATTR_NAME_NUM = "num";
const char * const ATTR_NAME_DTYPE = "dtype";
const char * const ATTR_NAME_DYNAMIC_SIZE = "dynamic_size";
const char * const ATTR_NAME_CLEAR_AFTER_READ = "clear_after_read";
const char * const ATTR_NAME_IDENTICAL_ELEMENT_SHAPES = "identical_element_shapes";
} // namespace hiai
#endif // INC_FRAMEWORK_GRAPH_OP_DATA_FLOW_ATTR_DEFS_H
