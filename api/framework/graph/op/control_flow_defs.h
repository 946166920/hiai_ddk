/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: control_flow_defs
 */
#ifndef INC_API_GRAPH_OP_CONTROL_FLOW_DEFS_H
#define INC_API_GRAPH_OP_CONTROL_FLOW_DEFS_H
#include "graph/operator_hiai_reg.h"

// clang-format off
namespace hiai {
/*
 * If Operator
 * <Input>
 *    cond : Input tensor.
 *           If the tensor is a scalar of non-boolean type, the scalar is converted to a boolean according to the
 *           following rule:
 *           if the scalar is a numerical value, non-zero means True and zero means False;
 *           if the scalar is a string, non-empty means True and empty means False.
 *           If the tensor is not a scalar, being empty means False and being non-empty means True.
 *    x : A list of input tensors
 * <Output>
 *    y : = cond ? then_branch(input) : else_branch(input)
 * <Graph>
 *    then_branch : then subgraph name
 *    else_branch : else subgraph name
 * <Attr>
 *    output_shapes : a list of output tensor desc
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(If)
.INPUT(cond, TensorType({ DT_BOOL }))
.DYNAMIC_INPUT(x, TensorType({ DT_FLOAT, DT_INT8, DT_INT32, DT_BOOL }))
.DYNAMIC_OUTPUT(y, TensorType({ DT_FLOAT, DT_INT8, DT_INT32, DT_BOOL }))
.REQUIRED_GRAPH(then_branch)
.REQUIRED_GRAPH(else_branch)
.ATTR(output_shapes, AttrValue::LIST_TENSOR_DESC {})
.OP_END()

/*
 * While operator
 * <Input>
 *    x : A list of input tensors
 * <Output>
 *    y : A list of output tensors
 * <Graph>
 *    cond : cond subgraph name
 *    body : else subgraph name
 * <Attr>
 *    output_shapes : a list of output tensor desc
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(While)
.DYNAMIC_INPUT(x, TensorType({ DT_FLOAT, DT_INT8, DT_INT32, DT_BOOL }))
.DYNAMIC_OUTPUT(y, TensorType({ DT_FLOAT, DT_INT8, DT_INT32, DT_BOOL }))
.REQUIRED_GRAPH(cond)
.REQUIRED_GRAPH(body)
.ATTR(output_shapes, AttrValue::LIST_TENSOR_DESC {})
.OP_END()
} // namespace hiai
// clang-format on

#endif