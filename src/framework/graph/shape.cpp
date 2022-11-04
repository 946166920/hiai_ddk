/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License Version 2.0.You may not use this file except in compliance with the License.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Apache License for more details at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Description: The description of the shape operation class
 * @file shape.cpp
 *
 * @brief
 *
 * @version 1.0
 *
 */
#include "graph/shape.h"

#include <cstring>
#include <map>

#include "framework/graph/debug/ge_log.h"
#include "infra/base/assertion.h"

#include "graph/persistance/interface/shape_def.h"
#include "graph/persistance/proxy/proto_factory.h"

namespace ge {
using namespace std;
Shape::Shape() : Shape(hiai::ProtoFactory::Instance()->CreateShapeDef(), true)
{
}

Shape::Shape(hiai::IShapeDef* shapeDef, bool isOwner) : shapeDef_(shapeDef), isOwner_(isOwner)
{
}

Shape::~Shape()
{
    if (isOwner_ && shapeDef_ != nullptr) {
        hiai::ProtoFactory::Instance()->DestroyShapeDef(shapeDef_);
    }
    shapeDef_ = nullptr;
}

void Shape::RefTo(const Shape& shape)
{
    shapeDef_ = shape.shapeDef_;
    isOwner_ = shape.isOwner_;
}

Shape::Shape(std::vector<int64_t> s) : Shape() // default
{
    if (shapeDef_ != nullptr) {
        for (size_t i = 0; i < s.size(); ++i) {
            shapeDef_->add_dim(s[i]);
        }
    }
}

size_t Shape::GetDimNum() const
{
    return shapeDef_ != nullptr ? shapeDef_->dim_size() : 0;
}

int64_t Shape::GetDim(size_t idx) const
{
    return shapeDef_ != nullptr ? shapeDef_->dim(idx) : 0;
}

GraphErrCodeStatus Shape::SetDim(size_t idx, int64_t value)
{
    HIAI_EXPECT_NOT_NULL(shapeDef_);

    if (idx >= shapeDef_->dim_size()) {
        return GRAPH_FAILED;
    }
    shapeDef_->set_dim(idx, value);
    return GRAPH_SUCCESS;
}

const std::vector<int64_t> Shape::GetDims() const
{
    std::vector<int64_t> ret;
    HIAI_EXPECT_NOT_NULL_R(shapeDef_, ret);

    for (size_t i = 0; i < shapeDef_->dim_size(); i++) {
        ret.push_back(shapeDef_->dim(i));
    }
    return ret;
}

int64_t Shape::GetTotalDimNum() const
{
    if (shapeDef_ == nullptr || shapeDef_->dim_size() == 0) {
        return 0;
    }

    int64_t res = 1;
    for (size_t i = 0; i < shapeDef_->dim_size(); i++) {
        int64_t dim = shapeDef_->dim(i);
        if (dim <= 0 || res > INT64_MAX / dim) {
            FMK_LOGE("shape dim multiplication can result in overflow!");
            return 0;
        }
        res *= dim;
    }
    return res;
}

Shape::Shape(const Shape& other) : Shape()
{
    if (shapeDef_ != nullptr) {
        shapeDef_->CopyFrom(other.shapeDef_);
    }
}

Shape& Shape::operator=(const Shape& other)
{
    if (&other == this) {
        return *this;
    }
    if (shapeDef_ != nullptr) {
        shapeDef_->CopyFrom(other.shapeDef_);
    }
    return *this;
}

bool Shape::operator==(const Shape& other) const
{
    if (other.shapeDef_ == nullptr || shapeDef_ == nullptr) {
        return false;
    }
    if (shapeDef_->dim_size() != other.shapeDef_->dim_size()) {
        return false;
    }
    for (size_t i = 0; i < shapeDef_->dim_size(); i++) {
        if (shapeDef_->dim(i) != other.shapeDef_->dim(i)) {
            return false;
        }
    }
    return true;
}
} // namespace ge
