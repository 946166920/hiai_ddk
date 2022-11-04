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
#ifndef DOMI_DATATYPE_CONVERT_H
#define DOMI_DATATYPE_CONVERT_H
#include <map>
#include "util/base_types.h"
#include "graph/types.h"

namespace hiai {

const std::map<enum ge::DataType, enum hiai::DataType> DATA_TYPE_MAP = {{ge::DataType::DT_UINT8, hiai::DataType::UINT8},
    {ge::DataType::DT_FLOAT, hiai::DataType::FLOAT32}, {ge::DataType::DT_FLOAT16, hiai::DataType::FLOAT16},
    {ge::DataType::DT_INT8, hiai::DataType::INT8}, {ge::DataType::DT_INT32, hiai::DataType::INT32},
    {ge::DataType::DT_INT16, hiai::DataType::INT16}, {ge::DataType::DT_BOOL, hiai::DataType::BOOL},
    {ge::DataType::DT_INT64, hiai::DataType::INT64}, {ge::DataType::DT_UINT32, hiai::DataType::UINT32},
    {ge::DataType::DT_DOUBLE, hiai::DataType::DOUBLE}, {ge::DataType::DT_UNDEFINED, hiai::DataType::RESERVED}};

}
#endif
