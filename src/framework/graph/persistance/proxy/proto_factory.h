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
#ifndef GE_PROTO_FACTORY_H
#define GE_PROTO_FACTORY_H

#include <string>

#include "graph/graph_api_export.h"

namespace hiai {
class IModelDef;
using CREATE_MODEL_DEF_FUNC = IModelDef* (*)();
using DESTROY_MODEL_DEF_FUNC = void (*)(IModelDef*);

class IGraphDef;
using CREATE_GRAPH_DEF_FUNC = IGraphDef* (*)();
using DESTROY_GRAPH_DEF_FUNC = void (*)(IGraphDef*);

class IOpDef;
using CREATE_OP_DEF_FUNC = IOpDef* (*)();
using DESTROY_OP_DEF_FUNC = void (*)(IOpDef*);

class ITensorDescDef;
using CREATE_TENSOR_DESC_DEF_FUNC = ITensorDescDef* (*)();
using DESTROY_TENSOR_DESC_DEF_FUNC = void (*)(ITensorDescDef*);

class ITensorDef;
using CREATE_TENSOR_DEF_FUNC = ITensorDef* (*)();
using DESTROY_TENSOR_DEF_FUNC = void (*)(ITensorDef*);

class IShapeDef;
using CREATE_SHAPE_DEF_FUNC = IShapeDef* (*)();
using DESTROY_SHAPE_DEF_FUNC = void (*)(IShapeDef*);

class IAttrDef;
using CREATE_ATTR_DEF_FUNC = IAttrDef* (*)();
using DESTROY_ATTR_DEF_FUNC = void (*)(IAttrDef*);

class IAttrMapDef;
using CREATE_ATTR_MAP_DEF_FUNC = IAttrMapDef* (*)();
using DESTROY_ATTR_MAP_DEF_FUNC = void (*)(IAttrMapDef*);

class INamedAttrDef;
using CREATE_NAMED_ATTR_DEF_FUNC = INamedAttrDef* (*)();
using DESTROY_NAMED_ATTR_DEF_FUNC = void (*)(INamedAttrDef*);

class GRAPH_API_EXPORT ProtoFactory {
public:
    static ProtoFactory* Instance();
    ~ProtoFactory();

    IModelDef* CreateModelDef();
    void DestroyModelDef(IModelDef* modelDef);

    IGraphDef* CreateGraphDef();
    void DestroyGraphDef(IGraphDef* graphDef);

    IOpDef* CreateOpDef();
    void DestroyOpDef(IOpDef* opDef);

    ITensorDescDef* CreateTensorDescDef();
    void DestroyTensorDescDef(ITensorDescDef* tensorDescDef);

    ITensorDef* CreateTensorDef();
    void DestroyTensorDef(ITensorDef* tensorDef);

    IShapeDef* CreateShapeDef();
    void DestroyShapeDef(IShapeDef* shapeDef);

    IAttrDef* CreateAttrDef();
    void DestroyAttrDef(IAttrDef* attrDef);

    IAttrMapDef* CreateAttrMapDef();
    void DestroyAttrMapDef(IAttrMapDef* attrMapDef);

    INamedAttrDef* CreateNamedAttrDef();
    void DestroyNamedAttrDef(INamedAttrDef* namedDef);

private:
    bool LoadPersistanceSo();
    void* GetSymbols(const std::string& funcName);

private:
    void* handle_{nullptr};

    CREATE_MODEL_DEF_FUNC createModelDef_{nullptr};
    DESTROY_MODEL_DEF_FUNC destroyModelDef_{nullptr};

    CREATE_GRAPH_DEF_FUNC createGraphDef_{nullptr};
    DESTROY_GRAPH_DEF_FUNC destroyGraphDef_{nullptr};

    CREATE_OP_DEF_FUNC createOpDef_{nullptr};
    DESTROY_OP_DEF_FUNC destroyOpDef_{nullptr};

    CREATE_TENSOR_DESC_DEF_FUNC createTensorDescDef_{nullptr};
    DESTROY_TENSOR_DESC_DEF_FUNC destroyTensorDescDef_{nullptr};

    CREATE_TENSOR_DEF_FUNC createTensorDef_{nullptr};
    DESTROY_TENSOR_DEF_FUNC destroyTensorDef_{nullptr};

    CREATE_SHAPE_DEF_FUNC createShapeDef_{nullptr};
    DESTROY_SHAPE_DEF_FUNC destroyShapeDef_{nullptr};

    CREATE_ATTR_DEF_FUNC createAttrDef_{nullptr};
    DESTROY_ATTR_DEF_FUNC destroyAttrDef_{nullptr};

    CREATE_ATTR_MAP_DEF_FUNC createAttrMapDef_{nullptr};
    DESTROY_ATTR_MAP_DEF_FUNC destroyAttrMapDef_{nullptr};

    CREATE_NAMED_ATTR_DEF_FUNC createNamedAttrDef_{nullptr};
    DESTROY_NAMED_ATTR_DEF_FUNC destroyNamedAttrDef_{nullptr};
};

} // namespace hiai

#endif // GE_PROTO_FACTORY_H