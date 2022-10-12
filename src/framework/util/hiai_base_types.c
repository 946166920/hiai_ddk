/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: base type implementation
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