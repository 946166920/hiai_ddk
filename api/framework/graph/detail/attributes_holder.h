/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: attributes_holder
 */

#ifndef GE_ATTRIBUTES_HOLDER_H
#define GE_ATTRIBUTES_HOLDER_H

#include "graph/debug/ge_error_codes.h"
#include "graph/graph_api_export.h"
#include <map>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace hiai {
class IAttrMapDef;
class IAttrDef;
}

namespace ge {
using std::string;

class AttrValue;
using AttrMap = std::map<std::string, AttrValue>;

class GRAPH_API_EXPORT AttrHolder {
public:
    GraphErrCodeStatus SetAttr(const string& name, const AttrValue& value);

    GraphErrCodeStatus GetAttr(const string& name, AttrValue& value) const;

    bool HasAttr(const string& name) const;

    GraphErrCodeStatus DelAttr(const string& name);

    hiai::IAttrDef* MutableAttr(const string& name);

    const hiai::IAttrDef* GetAttr(const string& name) const;

protected:
    AttrHolder() = default;

    virtual ~AttrHolder() = default;

    const std::map<string, AttrValue> GetAllAttrs() const;

    virtual const hiai::IAttrMapDef* GetAttrMapDef() const = 0;

    virtual hiai::IAttrMapDef* MutableAttrMapDef() = 0;
};
} // namespace ge

#endif // GE_ATTRIBUTES_HOLDER_H
