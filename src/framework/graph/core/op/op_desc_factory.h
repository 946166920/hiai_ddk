/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: op desc factory header file
 */

#ifndef FRAMEWORK_GRAPH_CORE_OP_OP_DESC_FACTORY_H
#define FRAMEWORK_GRAPH_CORE_OP_OP_DESC_FACTORY_H

#include <mutex>

#include "framework/graph/core/node/node.h"
#include "framework/graph/core/op/op_desc.h"

// npu/src/framework
#include "graph/persistance/interface/op_def.h"

namespace ge {
using CREATOR_OP_DESC_FUN = std::function<std::shared_ptr<OpDesc>(hiai::IOpDef*, bool isOwner)>;

class OpDescFactory {
public:
    static OpDescFactory& GetInstance();

    std::shared_ptr<OpDesc> Create(hiai::IOpDef* opDef);

    void Register(CREATOR_OP_DESC_FUN creatorFunc);

private:
    OpDescFactory() = default;
    ~OpDescFactory() = default;

private:
    std::vector<CREATOR_OP_DESC_FUN> creators_;
    std::mutex mutex_;
};

class OpDescRegisterar {
public:
    OpDescRegisterar(CREATOR_OP_DESC_FUN creatorFun) noexcept
    {
        OpDescFactory::GetInstance().Register(creatorFun);
    }

    ~OpDescRegisterar() = default;
};

#define REGISTER_OP_DESC_CREATOR(creatorFunc) \
    static const OpDescRegisterar g_##opDescType##_OpDesc(creatorFunc)
} // namespace ge
#endif // FRAMEWORK_GRAPH_CORE_OP_OP_DESC_FACTORY_H
