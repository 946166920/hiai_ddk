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
#include "proto_factory.h"

#include "graph/persistance/proto_impl/proto_attr_def.h"
#include "graph/persistance/proto_impl/proto_attr_list_def.h"
#include "graph/persistance/proto_impl/proto_attr_map_def.h"
#include "graph/persistance/proto_impl/proto_graph_def.h"
#include "graph/persistance/proto_impl/proto_model_def.h"
#include "graph/persistance/proto_impl/proto_named_attr_def.h"
#include "graph/persistance/proto_impl/proto_op_def.h"
#include "graph/persistance/proto_impl/proto_shape_def.h"
#include "graph/persistance/proto_impl/proto_tensor_def.h"
#include "graph/persistance/proto_impl/proto_tensor_desc_def.h"

namespace hiai {
ProtoFactory* ProtoFactory::Instance()
{
    static ProtoFactory instance;
    return &instance;
}

ProtoFactory::~ProtoFactory()
{
}

IModelDef* ProtoFactory::CreateModelDef()
{
    return new (std::nothrow) ProtoModelDef();
}

void ProtoFactory::DestroyModelDef(IModelDef* modelDef)
{
    delete modelDef;
}

IGraphDef* ProtoFactory::CreateGraphDef()
{
    return new (std::nothrow) ProtoGraphDef();
}

void ProtoFactory::DestroyGraphDef(IGraphDef* graphDef)
{
    delete graphDef;
}

IOpDef* ProtoFactory::CreateOpDef()
{
    return new (std::nothrow) ProtoOpDef();
}

void ProtoFactory::DestroyOpDef(IOpDef* opDef)
{
    delete opDef;
}

ITensorDescDef* ProtoFactory::CreateTensorDescDef()
{
    return new (std::nothrow) ProtoTensorDescDef();
}

void ProtoFactory::DestroyTensorDescDef(ITensorDescDef* tensorDescDef)
{
    delete tensorDescDef;
}

ITensorDef* ProtoFactory::CreateTensorDef()
{
    return new (std::nothrow) ProtoTensorDef();
}

void ProtoFactory::DestroyTensorDef(ITensorDef* tensorDef)
{
    delete tensorDef;
}

IShapeDef* ProtoFactory::CreateShapeDef()
{
    return new (std::nothrow) ProtoShapeDef();
}

void ProtoFactory::DestroyShapeDef(IShapeDef* shapeDef)
{
    delete shapeDef;
}

IAttrDef* ProtoFactory::CreateAttrDef()
{
    return new (std::nothrow) ProtoAttrDef();
}

void ProtoFactory::DestroyAttrDef(IAttrDef* attrDef)
{
    delete attrDef;
}

IAttrMapDef* ProtoFactory::CreateAttrMapDef()
{
    return new (std::nothrow) ProtoAttrMapDef();
}

void ProtoFactory::DestroyAttrMapDef(IAttrMapDef* attrMapDef)
{
    delete attrMapDef;
}

INamedAttrDef* ProtoFactory::CreateNamedAttrDef()
{
    return new (std::nothrow) ProtoNamedAttrDef();
}

void ProtoFactory::DestroyNamedAttrDef(INamedAttrDef* namedDef)
{
    delete namedDef;
}

} // namespace hiai