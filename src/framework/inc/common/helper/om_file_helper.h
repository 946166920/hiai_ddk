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

#ifndef DOMI_OMFILE_HELPER_H_
#define DOMI_OMFILE_HELPER_H_

#include <vector>
#include "framework/common/types.h"
#include "framework/common/fmk_types.h"

namespace hiai {
using Status = uint32_t;
struct ModelPartition {
    ModelPartitionType type;
    uint8_t* data = nullptr;
    uint32_t size = 0;
};

struct OmFileContext {
    std::vector<ModelPartition> partitionDatas;
    std::vector<char> partitionTable;
    uint32_t modelDataLen = 0;
};

class OmFileLoadHelper {
public:
    Status Init(const uint8_t* model, uint32_t modelSize);

    const ModelFileHeader* GetModelFileHeader()
    {
        return modelHeader_;
    }

    Status GetModelPartition(ModelPartitionType type, ModelPartition& partition) const;

public:
    const ModelFileHeader* modelHeader_;
    OmFileContext context_;

private:
    // check MODEL_DEF partition must exist, other partitions can not repeat
    Status CheckModelPartitionTable(const uint8_t* modelData, uint32_t size);

    Status LoadModelPartitionTable(const uint8_t* modelData, uint32_t size);

private:
    bool isInited_ {false};
};

class OmFileSaveHelper {
public:
    ModelFileHeader& GetModelFileHeader()
    {
        return modelHeader_;
    }

    uint32_t GetModelDataSize()
    {
        return context_.modelDataLen;
    }

    ModelPartitionTable* GetPartitionTable();

    void AddPartition(ModelPartition& partition);

    std::vector<ModelPartition>& GetModelPartitions();

    Status UpdataModelWeights(uint8_t* weightData, uint32_t weightSize);

public:
    ModelFileHeader modelHeader_;
    OmFileContext context_;
};
} // namespace hiai
#endif // DOMI_OMFILE_HELPER_H_
