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

#include "framework/graph/core/node/node_functor.h"

// inc/framework
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"

// framework/inc
#include "infra/base/assertion.h"

namespace ge {
namespace {
class TypedImpl {
public:
    TypedImpl(std::vector<std::string> types, NodeVisitor visitor)
        : types_(std::move(types)), visitor_(std::move(visitor))
    {}

    ~TypedImpl() = default;

    hiai::Status operator()(Node& node) const
    {
        for (const auto& type : types_) {
            if (node.ROLE(NodeSpec).Type() == type) {
                HIAI_EXPECT_EXEC(visitor_(node));
            }
        }
        return hiai::SUCCESS;
    }

private:
    std::vector<std::string> types_;
    NodeVisitor visitor_;
};
} // namespace

NodeVisitor NodeFunctor::Typed(std::vector<std::string> types, NodeVisitor visitor)
{
    return TypedImpl(std::move(types), std::move(visitor));
}
} // namespace ge
