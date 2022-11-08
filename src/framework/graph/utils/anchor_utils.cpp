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
#include "framework/graph/utils/anchor_utils.h"
#include <algorithm>
#include "framework/graph/debug/ge_log.h"
#include "graph/debug/ge_error_codes.h"

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
