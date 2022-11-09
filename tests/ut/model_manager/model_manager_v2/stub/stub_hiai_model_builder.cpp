/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: hiai model builder impl
 */
#include "framework/c/hiai_model_builder.h"
#include "framework/infra/log/log.h"
#include "control_c.h"

HIAI_Status HIAI_ModelBuilder_Build(const HIAI_ModelBuildOptions* options, const char* modelName,
    const void* inputModelData, size_t inputModelSize, HIAI_BuiltModel** builtModel)
{
    if (!hiai::ControlC::GetInstance().CheckBuildOptions(options)) {
        FMK_LOGE("build options is error.");
        return HIAI_FAILURE;
    }

    *builtModel = (HIAI_BuiltModel*)malloc(1);
    return HIAI_SUCCESS;
}
