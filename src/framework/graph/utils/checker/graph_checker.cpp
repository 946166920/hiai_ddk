/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: graph checker
 */

#include "framework/graph/utils/checker/graph_checker.h"

// api/framework
#include "graph/op/nn_defs.h"
#include "graph/op/detection_defs.h"
#include "graph/op/math_defs.h"

// inc/framework
#include "framework/common/hcs_types.h"
#include "framework/graph/debug/ge_op_types.h"
#include "framework/graph/core/cgraph/compute_graph.h"
#include "framework/graph/core/cgraph/graph_list_walker.h"
#include "framework/graph/core/node/node.h"
#include "framework/graph/core/node/node_spec.h"
#include "framework/graph/core/node/node_sub_graph.h"

// src/framework/inc
#include "infra/base/assertion.h"

namespace ge {
bool GraphChecker::IsInputsFullyLinked(const ComputeGraph& graph)
{
    auto v = [](Node& node) {
        const auto& type = node.ROLE(NodeSpec).Type();
        if (type == GRAPH_OP_TYPE) {
            return node.ROLE(NodeSubGraph).WalkSubGraphs([](ge::ComputeGraphPtr& subGraph) {
                return GraphChecker::IsInputsFullyLinked(*subGraph);
            });
        }

        /* Note: close_hione_baltimore_fmk_onnx_9732901 tc_fmk_onnx_net_search_distill_aipp_0001
         close_hione_baltimore_fmk_onnx_9732901 tc_fmk_onnx_net_template_distill_aipp_0001
         close_hione_m536_fmk_onnx_9732895 tc_fmk_onnx_net_template_distill_aipp_0001
         close_phoenix_4g_phoenix_fmk_onnx_9732828 tc_fmk_onnx_net_template_distill_aipp_0001 */
        if (type == hiai::op::ConvTranspose::TYPE) {
            return hiai::SUCCESS;
        }
        // Note: close_hione_baltimore_fmk_cbg_internal_9732940 hiai_cbg_he_net_caffe_personDetector_001_internalDDK
        if (type == hiai::op::SSDDetectionOutput::TYPE) {
            return hiai::SUCCESS;
        }
        /* Note: close_hione_baltimore_hpc_performance_evaluation_9734522
         tc_perf_Evaluation_AiMark2_5_SSD_Mobilenet_Tensorflow_INT8_2B_AIPP_ASYNC */
        if (type == ge::SSDPOSTPROCESSOR) {
            return hiai::SUCCESS;
        }
        // Note: tf_Antutu_AI_mobilenetv1_ssd_coco_0001
        if (type == "SSDAnchorGenerator") {
            return hiai::SUCCESS;
        }
        if (type == "_Retval") {
            return hiai::SUCCESS;
        }

        return node.ROLE(NodeSpec).IsInputFullyLinked() ? hiai::SUCCESS : hiai::FAILURE;
    };

    return graph.ROLE(GraphListWalker).WalkAllNodes(std::move(v)) == hiai::SUCCESS;
}
} // namespace ge