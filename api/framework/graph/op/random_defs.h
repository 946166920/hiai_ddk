/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: random_defs
 */
#ifndef INC_API_GRAPH_OP_RANDOM_DEFS_H
#define INC_API_GRAPH_OP_RANDOM_DEFS_H
#include "graph/operator_hiai_reg.h"

// clang-format off
namespace hiai {
/*
 * Extract samples from multiple distributions.
 * <Input>
 *    x : Input tensor, only support 2-D.
 *    num_samples : 0-D, number of independent samples to draw for each row slice. If the datatype of size is
 *                  DT_FOLAT, the elements must be >= 1.
 * <Output>
 *    y : Output tensor
 * <Attr>
 *    dtype : The datatype of output, now only support DT_INT32.
 *    seed : If either 'seed' or 'seed2' is set to non-zero, the internal random number generator is seeded by
 *           the given seed. Otherwise, a random seed is used.
 *    seed2 : Second seed to avoid seed collision
 * <Added in HiAI version>
 *    100.310.010.013
 */
REG_OP(Multinomial)
.INPUT(x, TensorType({ DT_FLOAT }))
.INPUT(num_samples, TensorType({ DT_INT32, DT_FLOAT, DT_UINT8 }))
.OUTPUT(y, TensorType({ DT_INT32 }))
.ATTR(dtype, AttrValue::INT { 3 })
.ATTR(seed, AttrValue::INT { 0 })
.ATTR(seed2, AttrValue::INT { 0 })
.OP_END()
} // namespace hiai
// clang-format on

#endif