/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: shape
 */

#ifndef GE_SHAPE_H
#define GE_SHAPE_H

#include <cstddef>
#include <vector>

#include "graph/debug/ge_error_codes.h"
#include "graph/attributes_holder.h"
#include "graph/graph_api_export.h"
#include "graph/types.h"

namespace hiai {
class IShapeDef;
}

namespace ge {
class GRAPH_API_EXPORT Shape {
public:
    Shape();
    Shape(hiai::IShapeDef* shapeDef, bool isOwner);
    explicit Shape(std::vector<int64_t> s);
    Shape(const Shape& other);
    Shape& operator=(const Shape& other);
    ~Shape();

    size_t GetDimNum() const;

    // if the idx is invalid, return 0
    int64_t GetDim(size_t idx) const;
    GraphErrCodeStatus SetDim(size_t idx, int64_t value);
    const std::vector<int64_t> GetDims() const;
    int64_t GetTotalDimNum() const;

    bool operator==(const Shape& other) const;

private:
    hiai::IShapeDef* shapeDef_;
    bool isOwner_;

    friend class TensorDesc;
    void RefTo(const Shape& shape);
};
} // namespace ge

#endif // GE_SHAPE_H
