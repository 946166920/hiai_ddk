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

#include "common/helper/om_file_helper.h"
#include <set>
#include "infra/base/assertion.h"
#include "common/util.h"
#include "common/math/math_util.h"
#include "framework/common/fmk_error_codes.h"

using std::string;
namespace hiai {
// 模型中partion数目最大值；考虑静态多Shape模型，最多16档(16+16+1)
constexpr int MAX_PARTITION_NUM = 35;
const char* ToString(ModelPartitionType type)
{
    if (type == ModelPartitionType::MODEL_DEF) {
        return "MODEL_DEF";
    } else if (type == ModelPartitionType::WEIGHTS_DATA) {
        return "WEIGHTS_DATA";
    } else if (type == ModelPartitionType::TASK_INFO) {
        return "TASK_INFO";
    } else if (type == ModelPartitionType::SIGNATURE_INFO) {
        return "SIGNATURE_INFO";
    } else if (type == ModelPartitionType::MODEL_CONFIG) {
        return "MODEL_CONFIG";
    } else if (type == ModelPartitionType::AIPP_CUSTOM_INFO) {
        return "AIPP_CUSTOM_INFO";
    } else {
        return "UN_KNOWN";
    }
}

// for Load

FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY hiai::Status OmFileLoadHelper::Init(
    uint8_t* modelData, uint32_t modelDataSize)
{
    HIAI_EXPECT_EXEC_R(LoadModelPartitionTable(modelData, modelDataSize), hiai::FAILED);
    isInited_ = true;
    return hiai::SUCCESS;
}

// both use
FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY hiai::Status OmFileLoadHelper::GetModelPartition(
    ModelPartitionType type, ModelPartition& partition) const
{
    if (!isInited_) {
        FMK_LOGE("OmFileLoadHelper not Inited!");
        return hiai::PARAM_INVALID;
    }

    for (ModelPartition part : context_.partitionDatas) {
        if (part.type == type) {
            partition = part;
            return hiai::SUCCESS;
        }
    }

    FMK_LOGD("GetModelPartition:type:%s is not in partition_datas_", ToString(type));
    return hiai::FAILED;
}

Status OmFileLoadHelper::CheckModelPartitionTable(uint8_t* modelData, uint32_t size)
{
    if (size <= sizeof(ModelPartitionTable)) {
        FMK_LOGE("model size less than sizeof(ModelPartitionTable)");
        return hiai::PARAM_INVALID;
    }
    auto* partitionTable = reinterpret_cast<ModelPartitionTable*>(modelData);
    if (partitionTable == nullptr) {
        FMK_LOGE("partitionTable is nullptr");
        return hiai::PARAM_INVALID;
    }
    if (partitionTable->num > MAX_PARTITION_NUM) {
        FMK_LOGE("ERROR: The partition num : %u not support!", partitionTable->num);
        return hiai::FAILED;
    }
    uint32_t partitionSize = SIZE_OF_MODEL_PARTITION_TABLE(*partitionTable);
    if (size < partitionSize) {
        FMK_LOGE("model size less than sizeof(ModelPartitionTable)+sizeof(ModelPartitionMemInfo)*num ");
        return hiai::PARAM_INVALID;
    }

    bool isModelDefExist = false;
    for (uint32_t i = 0; i < partitionTable->num; i++) {
        const ModelPartitionMemInfo& partition = partitionTable->partition[i];
        if (partition.type == MODEL_DEF) {
            isModelDefExist = true;
            break;
        }
    }
    if (!isModelDefExist) {
        FMK_LOGE("ModelPartition of type MODEL_DEF is not exist");
        return hiai::PARAM_INVALID;
    }
    return hiai::SUCCESS;
}

Status OmFileLoadHelper::LoadModelPartitionTable(uint8_t* modelData, uint32_t size)
{
    HIAI_EXPECT_NOT_NULL_R(modelData, hiai::PARAM_INVALID);

    if (CheckModelPartitionTable(modelData, size) != hiai::SUCCESS) {
        FMK_LOGE("OmFileLoadHelper::LoadModelPartitionTable, CheckModelPartitionTable failed");
        return hiai::FAILED;
    }

    ModelPartitionTable* partitionTable = reinterpret_cast<ModelPartitionTable*>(modelData);
    if (partitionTable == nullptr) {
        FMK_LOGE("partitionTable is nullptr");
        return hiai::PARAM_INVALID;
    }
    uint32_t memOffset = SIZE_OF_MODEL_PARTITION_TABLE(*partitionTable);
    FMK_LOGD("sizeof(ModelFileHeader)=%u, sizeof(ModelPartitionTable)=%u",
        static_cast<uint32_t>(sizeof(ModelFileHeader)), memOffset);

    uint32_t dataLength = 0;
    for (uint32_t i = 0; i < partitionTable->num; i++) {
        FMK_UINT32_ADDCHECK(dataLength, partitionTable->partition[i].memSize);
        dataLength += partitionTable->partition[i].memSize;
    }
    if (size != (SIZE_OF_MODEL_PARTITION_TABLE(*partitionTable) + dataLength)) {
        FMK_LOGE("invalid partition size");
        return hiai::PARAM_INVALID;
    }

    dataLength = 0;
    for (uint32_t i = 0; i < partitionTable->num; i++) {
        ModelPartition partition;
        partition.size = partitionTable->partition[i].memSize;
        partition.data = modelData + memOffset;
        partition.type = partitionTable->partition[i].type;
#if defined(AI_SUPPORT_32_BIT_OS)
        if (partition.type == MODEL_DEF) {
            if (partition.size % 4 != 0) {
                dataLength = ((partition.size + 3) / 4) * 4 - partition.size;
                memOffset += dataLength;
            }
        }

        if (partition.type == WEIGHTS_DATA) {
            partition.size = partitionTable->partition[i].memSize - dataLength;
        }
#endif
        memOffset += partition.size;
        if (partition.type > AIPP_CUSTOM_INFO) {
            continue;
        }
        context_.partitionDatas.push_back(partition);
    }
    return hiai::SUCCESS;
}

// for Save
FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY void OmFileSaveHelper::AddPartition(ModelPartition& partition)
{
    FMK_LOGD("AddPartition:type is:%s", ToString(partition.type));
    context_.partitionDatas.push_back(partition);
    context_.modelDataLen += partition.size;
}

FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY std::vector<ModelPartition>& OmFileSaveHelper::GetModelPartitions()
{
    return context_.partitionDatas;
}

FMK_FUNC_HOST_VISIBILITY FMK_FUNC_DEV_VISIBILITY ModelPartitionTable* OmFileSaveHelper::GetPartitionTable()
{
    uint32_t partitionSize = context_.partitionDatas.size();
    // build ModelPartitionTable, flex array
    context_.partitionTable.clear();
    context_.partitionTable.resize(sizeof(ModelPartitionTable) + sizeof(ModelPartitionMemInfo) * partitionSize, 0);

    ModelPartitionTable* partitionTable = reinterpret_cast<ModelPartitionTable*>(context_.partitionTable.data());
    HIAI_EXPECT_NOT_NULL_R(partitionTable, nullptr);
    partitionTable->num = partitionSize;

    uint32_t memOffset = 0;
    for (uint32_t i = 0; i < partitionSize; i++) {
        ModelPartition partition = context_.partitionDatas[i];
        partitionTable->partition[i] = {partition.type, memOffset, partition.size};
        FMK_LOGD("get partition, type: %s, offset: %u, size: %u", ToString(partition.type), memOffset, partition.size);
        memOffset += partition.size;
    }
    return partitionTable;
}
} // namespace hiai
