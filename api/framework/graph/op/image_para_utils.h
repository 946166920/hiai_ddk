/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: image_defs
 */

#ifndef _HIAI_GRAPH_OP_GE_OP_IMAGE_PARA_UTILS_H
#define _HIAI_GRAPH_OP_GE_OP_IMAGE_PARA_UTILS_H
#include "image_defs.h"
#include "const_defs.h"
#include "graph/types.h"
#include "graph/tensor.h"

namespace hiai {
template<typename T>
static hiai::op::Const CreateConfigConst(T& para, const string& name)
{
    TensorDesc tensorDesc(Shape({1, sizeof(T), 1, 1}), FORMAT_NCHW, DT_UINT8);
    TensorPtr outputTensorPtr = std::make_shared<Tensor>(tensorDesc, reinterpret_cast<uint8_t*>(&para), sizeof(T));
    hiai::op::Const constData = hiai::op::Const(name).set_attr_value(outputTensorPtr);

    return constData;
}

template<typename T>
static hiai::op::ConfigData CreateConfigData(T& para, const string& name, const string& type)
{
    TensorDesc tensorDesc(Shape({1, sizeof(T), 1, 1}), FORMAT_NCHW, DT_UINT8);
    TensorPtr outputTensorPtr = std::make_shared<Tensor>(tensorDesc, reinterpret_cast<uint8_t*>(&para), sizeof(T));
    hiai::op::ConfigData configdata = hiai::op::ConfigData(name).set_attr_value(outputTensorPtr).set_attr_type(type);

    return configdata;
}
} // namespace hiai
#endif