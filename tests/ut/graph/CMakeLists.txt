cmake_minimum_required(VERSION 3.8.0)

set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../)
set(THIRD_PARTY_PATH ${TOP_DIR}/third_party/)
set(THIRD_PARTY_PROTOBUF_PATH ${THIRD_PARTY_PATH}/protobuf/)
set(THIRD_PARTY_CSEC_PATH ${THIRD_PARTY_PATH}/bounds_checking_function/)

set(GRAPH_IR_PATH ${TOP_DIR}/src/framework/graph)

set(GRAPH_C_SEC_SRC
    ${THIRD_PARTY_CSEC_PATH}/src/vsnprintf_s.c
    ${THIRD_PARTY_CSEC_PATH}/src/snprintf_s.c
    ${THIRD_PARTY_CSEC_PATH}/src/secureprintoutput_a.c
    ${THIRD_PARTY_CSEC_PATH}/src/securecutil.c
    ${THIRD_PARTY_CSEC_PATH}/src/memcpy_s.c
    ${THIRD_PARTY_CSEC_PATH}/src/memset_s.c
)

set(GRAPH_PROTOBUF_LITE_SRC_FILES
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/any.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/any.pb.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/api.pb.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/compiler/importer.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/compiler/parser.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/descriptor.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/descriptor.pb.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/descriptor_database.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/duration.pb.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/dynamic_message.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/empty.pb.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/extension_set_heavy.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/field_mask.pb.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/generated_message_reflection.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/generated_message_table_driven.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/io/gzip_stream.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/io/printer.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/io/tokenizer.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/map_field.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/message.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/reflection_ops.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/service.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/source_context.pb.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/struct.pb.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/stubs/substitute.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/text_format.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/timestamp.pb.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/type.pb.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/unknown_field_set.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/delimited_message_util.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/field_comparator.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/field_mask_util.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/internal/datapiece.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/internal/default_value_objectwriter.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/internal/error_listener.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/internal/field_mask_utility.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/internal/json_escaping.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/internal/json_objectwriter.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/internal/json_stream_parser.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/internal/object_writer.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/internal/proto_writer.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/internal/protostream_objectsource.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/internal/protostream_objectwriter.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/internal/type_info.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/internal/type_info_test_helper.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/internal/utility.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/json_util.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/message_differencer.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/time_util.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/util/type_resolver_util.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/wire_format.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/wrappers.pb.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/any_lite.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/arena.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/extension_set.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/generated_enum_util.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/generated_message_table_driven_lite.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/generated_message_util.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/implicit_weak_message.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/io/coded_stream.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/io/io_win32.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/io/strtod.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/io/zero_copy_stream.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/io/zero_copy_stream_impl.cc
    ${THIRD_PARTY_PROTOBUF_PATH}/src/google/protobuf/io/zero_copy_stream_impl_lite.cc
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

set_source_files_properties(${GRAPH_PROTOBUF_LITE_SRC_FILES} PROPERTIES GENERATED TRUE)

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
    ${GRAPH_IR_PATH}/core/node/quick_query_nodes.cpp  
    ${GRAPH_IR_PATH}/core/node/node_sub_graph.cpp 
    ${GRAPH_IR_PATH}/core/node/node_compatibler.cpp
    ${GRAPH_IR_PATH}/core/op/op_desc.cpp          
    ${GRAPH_IR_PATH}/attr_value.cpp
    ${GRAPH_IR_PATH}/buffer.cpp
    ${GRAPH_IR_PATH}/graph.cpp
    ${GRAPH_IR_PATH}/compute_graph_builder.cpp
    ${GRAPH_IR_PATH}/graph_impl.cpp
    ${GRAPH_IR_PATH}/operator.cpp
    ${GRAPH_IR_PATH}/operator_impl.cpp
    ${GRAPH_IR_PATH}/shape.cpp
    ${GRAPH_IR_PATH}/tensor.cpp
    ${GRAPH_IR_PATH}/attributes_holder.cpp
    ${GRAPH_IR_PATH}/core/cgraph/graph_serializer.cpp
    ${GRAPH_IR_PATH}/core/node/node_serializer.cpp
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

set(GRAPH_UTIL_SRC_FILES
    ${GRAPH_IR_PATH}/utils/anchor_utils.cpp
    ${GRAPH_IR_PATH}/utils/graph_utils.cpp
    ${GRAPH_IR_PATH}/utils/node_utils.cpp
    ${GRAPH_IR_PATH}/utils/op_desc_utils.cpp
    ${GRAPH_IR_PATH}/utils/tensor_utils.cpp
    ${GRAPH_IR_PATH}/utils/attr_utils.cpp
    ${GRAPH_IR_PATH}/utils/replacer/graph_replacer.cpp
    ${GRAPH_IR_PATH}/utils/checker/node_checker.cpp
    ${GRAPH_IR_PATH}/utils/checker/graph_checker.cpp
)

set(INFRA_LOG_SRC_FILES
    ${TOP_DIR}/src/infra/log/linux_log.c
)

set(PROTO_LIST
    ${GRAPH_IR_PATH}/persistance/proto_impl/proto/ge_ir.proto
)

protobuf_generate(hiai PROTO_SRCS PROTO_HDRS ${PROTO_LIST})
set_source_files_properties(${PROTO_HDRS} PROPERTIES GENERATED TRUE)

set(GRAPH_TEST_FILES
    testcase/main.cc
    testcase/ge_graph/ge_anchor_utils_unittest.cpp
    testcase/ge_graph/ge_graph_anchor_unittest.cpp
    testcase/ge_graph/ge_model_serialize_unittest.cpp
    testcase/ge_graph/ge_node_unittest.cpp
    testcase/ge_graph/ge_node_utils_unittest.cpp
    testcase/ge_graph/ge_opdesc_unittest.cpp
    testcase/ge_graph/ge_tensor_unittest.cpp
    testcase/ge_graph/ge_tensor_utils_unittest.cpp
    testcase/ge_ir/ge_graph_unittest.cpp
    testcase/ge_ir/ge_operator_unittest.cpp
    testcase/ge_ir/ge_operator_attr_unittest.cpp
    testcase/ge_ir/ge_operator_graph_builder_unittest.cpp
    testcase/ge_ir/ge_tensor_unittest.cpp
    testcase/ge_ir/ge_shape_unittest.cpp
    testcase/ge_ir/ge_attr_holder_unittest.cpp
    testcase/ge_ir/ge_buffer_unittest.cpp
    testcase/ge_ir/ge_model_unittest.cpp
)

set(GRAPH_ALL_SRC_FILES
    ${GRAPH_CORE_SRC_FILES}
    ${GRAPH_UTIL_SRC_FILES}
    ${GRAPH_PROTOBUF_LITE_SRC_FILES}
	${GRAPH_PERSISTANCE_PROTO_SRC_FILES}
    ${GRAPH_C_SEC_SRC}
	${INFRA_LOG_SRC_FILES}
    ${GRAPH_TEST_FILES}
    ${PROTO_SRCS}
    ${PROTO_HDRS}
)

set(GTEST_DIR "${CMAKE_CURRENT_LIST_DIR}/googletest")

add_compile_options("--coverage")

set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -Wall -fprofile-arcs -ftest-coverage")

set(COVERAGE_COMPILER_FLAGS " -fprofile-arcs -fPIC -O0 -ftest-coverage")
set(CMAKE_CXX_FLAGS "${COVERAGE_COMPILER_FLAGS}")

include_directories(${LIBGRAPH}
      ${TOP_DIR}/api
      ${TOP_DIR}/api/infra
      ${TOP_DIR}/api/framework
      ${TOP_DIR}/inc
      ${TOP_DIR}/inc/framework
      ${TOP_DIR}/inc/framework/graph
      ${TOP_DIR}/src
      ${TOP_DIR}/src/framework
      ${TOP_DIR}/src/framework/graph
      ${TOP_DIR}/src/inc
      ${PROTO_HDRS}
      ${THIRD_PARTY_PROTOBUF_PATH}/include
      ${THIRD_PARTY_PROTOBUF_PATH}/src
      ${THIRD_PARTY_CSEC_PATH}/include
      ${GTEST_DIR}/googletest/include
      ${GTEST_DIR}/googlemock/include
      ${TOP_DIR}/third_party/libboundscheck/include
)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_executable(ut_graph ${GRAPH_ALL_SRC_FILES})
target_compile_definitions(ut_graph
    PRIVATE
    _GLIBCXX_USE_CXX11_ABI=0
    )
target_compile_options(ut_graph
    PRIVATE
      -g --coverage -fprofile-arcs -ftest-coverage
      -Werror=format
    )

target_link_libraries(ut_graph
    gtest
    gcov
    dl
)

add_test(NAME ut_graph COMMAND ut_graph)


