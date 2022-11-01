/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: om_file_helper.h
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
    Status Init(const ModelData* model);

    Status Init(uint8_t* modelData, uint32_t modelDataSize);

    Status GetModelPartition(ModelPartitionType type, ModelPartition& partition) const;

public:
    OmFileContext context_;

private:
    Status CheckModelValid(const ModelData* model);

    // check MODEL_DEF partition must exist, other partitions can not repeat
    Status CheckModelPartitionTable(uint8_t* modelData, uint32_t size);

    Status LoadModelPartitionTable(uint8_t* modelData, uint32_t size);

private:
    bool isInited_ {false};
};

class OmFileSaveHelper {
public:
    ModelFileHeader& GetModelFileHeader()
    {
        return model_header_;
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
    ModelFileHeader model_header_;
    OmFileContext context_;
};
} // namespace hiai
#endif // DOMI_OMFILE_HELPER_H_
