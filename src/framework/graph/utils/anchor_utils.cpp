/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2019. All rights reserved.
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the Apache License Version 2.0.You may not use this file except in compliance with the License.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Apache License for more details at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Description: The description of the anchor commom operation class
 * @file anchor_utils.cpp
 *
 * @brief
 *
 * @version 1.0
 *
 */
#include "framework/graph/utils/anchor_utils.h"
#include <algorithm>
#include "framework/graph/debug/ge_log.h"
#include "graph/debug/ge_error_codes.h"
#include "framework/graph/debug/ge_util.h"

namespace ge {
Format AnchorUtils::GetFormat(DataAnchorPtr dataAnchor)
{
    if (dataAnchor == nullptr) {
        FMK_LOGE("The input data anchor is invalid.");
        return FORMAT_RESERVED;
    }
    return dataAnchor->format_;
}

GraphErrCodeStatus AnchorUtils::SetFormat(DataAnchorPtr dataAnchor, Format dataFormat)
{
    if (dataAnchor == nullptr || dataFormat == FORMAT_RESERVED) {
        FMK_LOGE("The input data anchor or input data format is invalid .");
        return GRAPH_FAILED;
    }
    dataAnchor->format_ = dataFormat;
    return GRAPH_SUCCESS;
}

// get anchor status
AnchorStatus AnchorUtils::GetStatus(DataAnchorPtr dataAnchor)
{
    if (dataAnchor == nullptr) {
        FMK_LOGE("The input data anchor is invalid.");
        return ANCHOR_RESERVED;
    }
    return dataAnchor->status_;
}

// set anchor status
GraphErrCodeStatus AnchorUtils::SetStatus(DataAnchorPtr dataAnchor, AnchorStatus anchorStatus)
{
    if (dataAnchor == nullptr || anchorStatus == ANCHOR_RESERVED) {
        FMK_LOGE("The input data anchor or input data format is invalid .");
        return GRAPH_FAILED;
    }
    dataAnchor->status_ = anchorStatus;
    return GRAPH_SUCCESS;
}
} // namespace ge
