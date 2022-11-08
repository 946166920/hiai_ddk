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
#include "framework/c/hiai_base_types.h"

#include <stdint.h>

static size_t g_dataTypeElementSize[HIAI_DATATYPE_SIZE] = {sizeof(uint8_t), sizeof(float), sizeof(float) / 2,
    sizeof(int32_t), sizeof(int8_t), sizeof(int16_t), sizeof(char), sizeof(int64_t), sizeof(uint32_t), sizeof(double)};

size_t HIAI_DataType_GetElementSize(HIAI_DataType type)
{
    if (type >= HIAI_DATATYPE_SIZE) {
        return 0;
    }
    return g_dataTypeElementSize[type];
}