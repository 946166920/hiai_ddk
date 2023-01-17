/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: op desc factory source file
 */
#include "graph/core/op/op_desc_factory.h"

#include <mutex>

#include "infra/base/securestl.h"

#include "framework/graph/core/node/node.h"

// npu/src/framework
#include "graph/persistance/interface/attr_def.h"

namespace ge {
OpDescFactory& OpDescFactory::GetInstance()
{
    static OpDescFactory instance;
    return instance;
}

std::shared_ptr<OpDesc> OpDescFactory::Create(hiai::IOpDef* opDef)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = creators_.cbegin(); it != creators_.cend(); it++) {
        auto op = (*it)(opDef, false);
        if (op != nullptr) {
            return op;
        }
    }
    return hiai::make_shared_nothrow<OpDesc>(opDef, false);
}

void OpDescFactory::Register(CREATOR_OP_DESC_FUN creatorFunc)
{
    creators_.push_back(creatorFunc);
}

} // namespace ge
