/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: MembufferUtil
 */

#ifndef FRAMEWORK_MODEL_MANAGER_COMPATIBLE_MEMBUFFER_UTIL_H
#define FRAMEWORK_MODEL_MANAGER_COMPATIBLE_MEMBUFFER_UTIL_H
#include <string>
#include <vector>

#include "compatible/MemBuffer.h"
#include "compatible/hiai_base_types_cpt.h"

namespace hiai {
class MembufferUtil {
public:
    static MemBuffer* InputMemBufferCreate(void* data, uint32_t size);
    static MemBuffer* InputMemBufferCreate(const std::string& path);
    static MemBuffer* OutputMemBufferCreate(int32_t framework, const std::vector<MemBuffer*>& pinputMemBuffer);
    static void MemBufferDestroy(MemBuffer* membuf);
    static AIStatus MemBufferExportFile(MemBuffer* membuf, uint32_t pbuildSize, const std::string& pbuildPath);
};
} // namespace hiai
#endif