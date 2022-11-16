cmake_minimum_required(VERSION 3.8.0)

set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../)
set(FRAMEWORK_BASE_DIR_FOR_INC_DIRS ${TOP_DIR}/src/framework)
set(THIRD_PARTY_PATH ${TOP_DIR}/third_party/)
set(THIRD_PARTY_PROTOBUF_PATH ${THIRD_PARTY_PATH}/protobuf/)
set(THIRD_PARTY_CSEC_PATH ${THIRD_PARTY_PATH}/bounds_checking_function/)
set(GRAPH_IR_PATH ${TOP_DIR}/src/framework/graph)

set(GRAPH_PROTOBUF_LITE_SRC_FILES
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/arena.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/extension_set.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/generated_message_table_driven_lite.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/generated_message_util.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/implicit_weak_message.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/io/coded_stream.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/io/zero_copy_stream.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/io/zero_copy_stream_impl_lite.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/map_field.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/message_lite.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/parse_context.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/repeated_field.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/stubs/bytestream.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/stubs/common.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/stubs/int128.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/stubs/status.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/stubs/statusor.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/stubs/stringpiece.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/stubs/stringprintf.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/stubs/structurally_valid.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/stubs/strutil.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/stubs/time.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/wire_format_lite.cc
)

set(PROTO_LIST
    ${GRAPH_IR_PATH}/persistance/proto_impl/proto/ge_ir.proto
)

protobuf_generate(hiai PROTO_SRCS PROTO_HDRS ${PROTO_LIST})
set_source_files_properties(${PROTO_HDRS} PROPERTIES GENERATED TRUE)
message(STATUS  "PROTO_SRCS: ${PROTO_SRCS}")

message(STATUS  "PROTO_HDRS: ${PROTO_HDRS}")
set(HIAI_C_SEC_SRC
    ${THIRD_PARTY_CSEC_PATH}/src/memcpy_s.c
    ${THIRD_PARTY_CSEC_PATH}/src/memset_s.c
    ${THIRD_PARTY_CSEC_PATH}/src/vsnprintf_s.c
    ${THIRD_PARTY_CSEC_PATH}/src/snprintf_s.c
    ${THIRD_PARTY_CSEC_PATH}/src/secureprintoutput_a.c
)

set(GRAPH_CORE_SRC_FILES
    ${GRAPH_IR_PATH}/core/cgraph/compute_graph.cpp 
    ${GRAPH_IR_PATH}/core/cgraph/graph_builder.cpp 
    ${GRAPH_IR_PATH}/core/cgraph/graph_bypasser.cpp
    ${GRAPH_IR_PATH}/core/cgraph/graph_finder.cpp
    ${GRAPH_IR_PATH}/core/cgraph/graph_list_walker.cpp
    ${GRAPH_IR_PATH}/core/cgraph/graph_modifier.cpp
    ${GRAPH_IR_PATH}/core/cgraph/graph_pruner.cpp
    ${GRAPH_IR_PATH}/core/cgraph/graph_sorter.cpp
    ${GRAPH_IR_PATH}/core/cgraph/graph_spec.cpp  
    ${GRAPH_IR_PATH}/core/cgraph/graph_store.cpp 
    ${GRAPH_IR_PATH}/core/cgraph/graph_topo_walker.cpp
    ${GRAPH_IR_PATH}/core/cgraph/graph_listener.cpp
    ${GRAPH_IR_PATH}/core/cgraph/graph_notifier.cpp
    ${GRAPH_IR_PATH}/core/cgraph/legacy_graph.cpp
    ${GRAPH_IR_PATH}/core/cgraph/graph_serializer.cpp
    ${GRAPH_IR_PATH}/core/edge/anchor.cpp        
    ${GRAPH_IR_PATH}/core/edge/edge.cpp          
    ${GRAPH_IR_PATH}/core/edge/endpoint.cpp      
    ${GRAPH_IR_PATH}/core/node/legacy_node.cpp   
    ${GRAPH_IR_PATH}/core/node/node_const_input.cpp
    ${GRAPH_IR_PATH}/core/node/node.cpp          
    ${GRAPH_IR_PATH}/core/node/node_spec.cpp     
    ${GRAPH_IR_PATH}/core/node/node_store.cpp    
    ${GRAPH_IR_PATH}/core/node/node_functor.cpp  
    ${GRAPH_IR_PATH}/core/node/node_walker.cpp   
    ${GRAPH_IR_PATH}/core/node/node_sub_graph.cpp
    ${GRAPH_IR_PATH}/core/node/node_compatibler.cpp
    ${GRAPH_IR_PATH}/core/node/quick_query_nodes.cpp
    ${GRAPH_IR_PATH}/core/node/node_serializer.cpp
    ${GRAPH_IR_PATH}/core/op/op_desc.cpp
    ${GRAPH_IR_PATH}/attr_value.cpp
    ${GRAPH_IR_PATH}/buffer.cpp
    ${GRAPH_IR_PATH}/compute_graph_builder.cpp
    ${GRAPH_IR_PATH}/graph.cpp
    ${GRAPH_IR_PATH}/graph_impl.cpp
    ${GRAPH_IR_PATH}/model.cpp
    ${GRAPH_IR_PATH}/operator.cpp
    ${GRAPH_IR_PATH}/operator_impl.cpp
    ${GRAPH_IR_PATH}/tensor.cpp
    ${GRAPH_IR_PATH}/shape.cpp
    ${GRAPH_IR_PATH}/attributes_holder.cpp
    ${GRAPH_IR_PATH}/utils/anchor_utils.cpp
    ${GRAPH_IR_PATH}/utils/graph_utils.cpp
    ${GRAPH_IR_PATH}/utils/node_utils.cpp
    ${GRAPH_IR_PATH}/utils/op_desc_utils.cpp
    ${GRAPH_IR_PATH}/utils/tensor_utils.cpp
    ${GRAPH_IR_PATH}/utils/attr_utils.cpp
    ${GRAPH_IR_PATH}/utils/replacer/graph_replacer.cpp
    ${GRAPH_IR_PATH}/utils/checker/node_checker.cpp
    ${GRAPH_IR_PATH}/utils/checker/graph_checker.cpp
    ${GRAPH_IR_PATH}/model.cpp
    ${GRAPH_IR_PATH}/persistance/proxy/static_proto_factory.cpp
)

set(GRAPH_PERSISTANCE_PROTO_SRC_FILES
    ${GRAPH_IR_PATH}/persistance/proto_impl/proto_attr_def.cpp
    ${GRAPH_IR_PATH}/persistance/proto_impl/proto_attr_list_def.cpp
    ${GRAPH_IR_PATH}/persistance/proto_impl/proto_attr_map_def.cpp
    ${GRAPH_IR_PATH}/persistance/proto_impl/proto_model_def.cpp
    ${GRAPH_IR_PATH}/persistance/proto_impl/proto_graph_def.cpp
    ${GRAPH_IR_PATH}/persistance/proto_impl/proto_named_attr_def.cpp
    ${GRAPH_IR_PATH}/persistance/proto_impl/proto_op_def.cpp
    ${GRAPH_IR_PATH}/persistance/proto_impl/proto_shape_def.cpp
    ${GRAPH_IR_PATH}/persistance/proto_impl/proto_tensor_def.cpp
    ${GRAPH_IR_PATH}/persistance/proto_impl/proto_tensor_desc_def.cpp
)

set(COMPATIBLE_SRC
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/common/helper/om_file_helper.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/compatible/ir_transformer.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/compatible/ir_verify.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/compatible/array_op_transformer.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/compatible/control_flow_op_transformer.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/compatible/detection_op_transformer.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/compatible/image_op_transformer.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/compatible/internal_op_transformer.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/compatible/math_op_transformer.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/compatible/nn_op_transformer.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/compatible/transformer_utils.cpp
)

set(LOCAL_SRC_FILES
    ${COMPATIBLE_SRC}
    ${HIAI_C_SEC_SRC}
    ${GRAPH_CORE_SRC_FILES}
    ${GRAPH_PERSISTANCE_PROTO_SRC_FILES}
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    ${GRAPH_PROTOBUF_LITE_SRC_FILES}
    ${TOP_DIR}/src/infra/math/fp16_t.cpp
    ${TOP_DIR}/src/infra/log/linux_log.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/util/tensor/trans_tensor.cpp
)

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--gc-sections")

set(CMAKE_CXX_FLAGS "-std=c++11 -pthread -fPIC -frtti -Os -ffunction-sections -fdata-sections -fno-asynchronous-unwind-tables -fno-exceptions -fomit-frame-pointer -fno-unwind-tables -Wno-attributes -DHIAI_DDK -DHIAI_DDK_BUILD -DMNN_USE_NEON -D_FORTIFY_SOURCE=2 -DHAVE_PTHREAD -DGOOGLE_PROTOBUF_NO_RTTI -DPROTOBUF_INLINE_NOT_IN_HEADERS=0 -DHOST_VISIBILITY -DGRAPH_API_VISIABLE -DFMK_FEATURE_DYNAMIC_SHAPE")

set(CMAKE_C_FLAGS "-DAI_SUPPORT_UPGRADE_APK -DSUPPORT_HCL -DAIPP_ENABLE -DAI_SUPPORT_AIPP_API")

set(LIBHIAI_IR_DDK hiai_ir_ddk)
add_library(${LIBHIAI_IR_DDK} SHARED ${LOCAL_SRC_FILES})

target_compile_definitions(${LIBHIAI_IR_DDK}
    PRIVATE
    _GLIBCXX_USE_CXX11_ABI=0
)

target_include_directories(${LIBHIAI_IR_DDK}
    PRIVATE
    ${TOP_DIR}/inc
    ${TOP_DIR}/inc/framework
    ${TOP_DIR}/inc/framework/graph
    ${TOP_DIR}/inc/framework/util
    ${TOP_DIR}/inc/infra
    ${TOP_DIR}/inc/infra/om
    ${TOP_DIR}/inc/infra/mmpa
    ${TOP_DIR}/api
    ${TOP_DIR}/api/infra
    ${TOP_DIR}/api/framework
    ${TOP_DIR}/src
    ${TOP_DIR}/src/framework
    ${TOP_DIR}/src/framework/compatible
    ${TOP_DIR}/src/framework/graph/persistance/interface
    ${TOP_DIR}/src/framework/graph/persistance
    ${TOP_DIR}/src/framework/inc
    ${TOP_DIR}/src/framework/util
    ${PROTO_HDRS}
    ${THIRD_PARTY_PROTOBUF_PATH}/include
    ${THIRD_PARTY_PROTOBUF_PATH}/src
    ${THIRD_PARTY_CSEC_PATH}/include
)

set(PROTOBUF_LIB_PATH ${TOP_DIR}/third_party/protobuf)

find_library(libprotobuf libprotobuf.a ${PROTOBUF_LIB_PATH})

target_link_libraries(${LIBHIAI_IR_DDK}
    ${libprotobuf}
)
