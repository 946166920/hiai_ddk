/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: model builder
 */
#ifndef FRAMEWORK_C_HIAI_MODEL_BUILDER_H
#define FRAMEWORK_C_HIAI_MODEL_BUILDER_H
#include "hiai_c_api_export.h"
#include "hiai_built_model.h"
#include "hiai_model_builder_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// build interface
AICP_C_API_EXPORT HIAI_Status HIAI_ModelBuilder_Build(const HIAI_ModelBuildOptions* options, const char* modelName,
    const void* inputModelData, size_t inputModelSize, HIAI_BuiltModel** builtModel);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_C_HIAI_MODEL_BUILDER_H
