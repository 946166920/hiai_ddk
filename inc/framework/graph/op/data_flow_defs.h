/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: data_flow_defs
 */
#ifndef INC_FRAMEWORK_GRAPH_OP_DATA_FLOW_DEFS_H
#define INC_FRAMEWORK_GRAPH_OP_DATA_FLOW_DEFS_H
#include "graph/operator_hiai_reg.h"

// clang-format off
namespace hiai {
/*
 * Class wrapping dynamic-sized, per-time-step, write-once Tensor arrays.
 * <Input>
 *    size : The size of the TensorArray.
 * <Output>
 *    handle : A Tensor handle to an existing TensorArray.
 *    flow : Output tensor.
 * <Attr>
 *    dtype : (required) Data type of the TensorArray.
 *    element_shape : A TensorShape object specifying the shape constraints of
 *                    each of the elements of the TensorArray.
 *    dynamic_size : A boolean that determines whether writes to the TensorArray
 *                   are allowed to grow the size. Default to False.
 *    clear_after_read : If True, clear TensorArray values after reading them.Default: True.
 *    identical_element_shapes : If true (default is false), then all elements in the TensorArray will
 *                               be expected to have have identical shapes.Defaults to False.
 *    tensor_array_name : the name of the TensorArray.Defaults to "".
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(TensorArray)
.INPUT(size, TensorType({ DT_UINT32 }))
.OUTPUT(handle, TensorType({ DT_RESOURCE }))
.OUTPUT(flow, TensorType({ DT_FLOAT, DT_FLOAT16, DT_INT8, DT_INT16, DT_UINT16, DT_UINT8, DT_INT32, DT_INT64,
    DT_BOOL, DT_DOUBLE }))
.REQUIRED_ATTR(dtype, AttrValue::DATA_TYPE)
.ATTR(element_shape, AttrValue::LIST_INT ({ 0 }))
.ATTR(dynamic_size, AttrValue::BOOL { false })
.ATTR(clear_after_read, AttrValue::BOOL { true })
.ATTR(identical_element_shapes, AttrValue::BOOL { false })
.ATTR(tensor_array_name, AttrValue::STR { "" })
.OP_END()

/*
 * Read an element from the TensorArray into output value.
 * <Input>
 *    handle : The handle to a TensorArray.
 *    index : The position to read to inside the TensorArray.
 *    flow_in : A float scalar that enforces proper chaining of operations.
 * <Output>
 *    y : The tensor that is read from the TensorArray.
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(TensorArrayRead)
.INPUT(handle, TensorType({ DT_RESOURCE }))
.INPUT(index, TensorType({ DT_INT32 }))
.INPUT(flow_in, TensorType({ DT_FLOAT }))
.OUTPUT(y, TensorType({ DT_FLOAT, DT_FLOAT16, DT_INT8, DT_INT16, DT_UINT16, DT_UINT8, DT_INT32, DT_INT64,
    DT_UINT32, DT_UINT64, DT_BOOL, DT_DOUBLE }))
.OP_END()

/*
 * Push an element onto the tensor_array.
 * <Input>
 *    handle : The handle to a TensorArray.
 *    index : The position to write to inside the TensorArray.
 *    value : The tensor to write to the TensorArray.
 *    flow_in : A float scalar that enforces proper chaining of operations.
 * <Output>
 *    flow_out : A float scalar that enforces proper chaining of operations.
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(TensorArrayWrite)
.INPUT(handle, TensorType({ DT_RESOURCE }))
.INPUT(index, TensorType({ DT_INT32 }))
.INPUT(value, TensorType({ DT_FLOAT, DT_FLOAT16, DT_INT8, DT_INT16, DT_UINT16, DT_UINT8, DT_INT32, DT_INT64,
    DT_BOOL, DT_DOUBLE }))
.INPUT(flow_in, TensorType({ DT_FLOAT }))
.OUTPUT(flow_out, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * Scatter the data from the input value into specific TensorArray elements.
 * <Input>
 *    handle : The handle to a TensorArray.
 *    index : The locations at which to write the tensor elements.
 *    value : The concatenated tensor to write to the TensorArray.
 *    flow_in : A float scalar that enforces proper chaining of operations.
 * <Output>
 *    flow_out : A float scalar that enforces proper chaining of operations.
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(TensorArrayScatter)
.INPUT(handle, TensorType({ DT_RESOURCE }))
.INPUT(index, TensorType({ DT_INT32 }))
.INPUT(value, TensorType({ DT_FLOAT, DT_FLOAT16, DT_INT8, DT_INT16, DT_UINT16, DT_UINT8, DT_INT32, DT_INT64,
    DT_BOOL, DT_DOUBLE }))
.INPUT(flow_in, TensorType({ DT_FLOAT }))
.OUTPUT(flow_out, TensorType({ DT_FLOAT }))
.OP_END()

/*
 * Get the current size of the TensorArray.
 * <Input>
 *    handle : The handle to a TensorArray.
 *    flow_in : A float scalar that enforces proper chaining of operations.
 * <Output>
 *    size : The current size of the TensorArray.
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(TensorArraySize)
.INPUT(handle, TensorType({ DT_RESOURCE }))
.INPUT(flow_in, TensorType({ DT_FLOAT }))
.OUTPUT(size, TensorType({ DT_INT32 }))
.OP_END()

/*
 * Gather specific elements from the TensorArray into output value.
 * <Input>
 *    handle : The handle to a TensorArray.
 *    indices : The locations in the TensorArray from which to read tensor elements.
 *    flow_in : A float scalar that enforces proper chaining of operations.
 * <Output>
 *    value : A Tensor handle to an existing TensorArray.
 * <Attr>
 *    dtype : (required) The type of the elem that is returned.
 *    element_shape : The expected shape of an element, if known. Used to validate the shapes of TensorArray
 *                    elements.
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(TensorArrayGather)
.INPUT(handle, TensorType({ DT_RESOURCE }))
.INPUT(indices, TensorType({ DT_INT32 }))
.INPUT(flow_in, TensorType({ DT_FLOAT }))
.OUTPUT(value, TensorType({ DT_FLOAT, DT_FLOAT16, DT_INT8, DT_INT16, DT_UINT16, DT_UINT8, DT_INT32, DT_INT64,
    DT_BOOL, DT_DOUBLE }))
.REQUIRED_ATTR(dtype, AttrValue::DATA_TYPE)
.ATTR(element_shape, AttrValue::LIST_INT ({ 0 }))
.OP_END()

/*
 * operator close tensorarray
 * <Input>
 *    handle : The handle to a TensorArray.
 * <Added in HiAI version>
 *    100.500.010.010
 */
REG_OP(TensorArrayClose)
.INPUT(handle, TensorType({ DT_RESOURCE }))
.OP_END()
} // namespace hiai
// clang-format on

#endif