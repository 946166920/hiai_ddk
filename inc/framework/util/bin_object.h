/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: bin object
 *
 */
#ifndef _BIN_OBJECT_H_
#define _BIN_OBJECT_H_

#include <cstddef>

namespace ge {
class BinObject {
public:
    virtual ~BinObject() = default;
    virtual size_t GetSize() const = 0;
};
} // namespace ge
#endif
