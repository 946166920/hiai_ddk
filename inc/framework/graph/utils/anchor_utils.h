/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: anchor_utils
 */

#ifndef GE_ANCHOR_UTILS_H
#define GE_ANCHOR_UTILS_H

#include "framework/graph/core/edge/anchor.h"
#include "framework/graph/core/node/node.h"
#include "graph/graph_api_export.h"

namespace ge {
class AnchorUtils {
public:
    // get anchor format
    GRAPH_API_EXPORT static Format GetFormat(DataAnchorPtr dataAnchor);

    // set anchor format
    GRAPH_API_EXPORT static GraphErrCodeStatus SetFormat(DataAnchorPtr dataAnchor, Format dataFormat);

    // get anchor status
    GRAPH_API_EXPORT static AnchorStatus GetStatus(DataAnchorPtr dataAnchor);

    // set anchor status
    GRAPH_API_EXPORT static GraphErrCodeStatus SetStatus(DataAnchorPtr dataAnchor, AnchorStatus anchorStatus);
};
} // namespace ge
#endif // GE_ANCHOR_UTILS_H
