cmake_minimum_required(VERSION 3.8.0)
project(libgraph_proto_host)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(NPU_PATH ${CMAKE_CURRENT_LIST_DIR}/../../../..)

include_directories(
    ${NPU_PATH}/third_party/protobuf/include_win
    ${NPU_PATH}/${GENERATED_PROTO_DIR}/
    ${NPU_PATH}/${GENERATED_PROTO_DIR}/../
)

file(GLOB SRC_FILES  
    "${NPU_PATH}/${GENERATED_PROTO_DIR}/ge_ir.pb.cc"
    "${NPU_PATH}/${GENERATED_PROTO_DIR}/insert_op.pb.cc"
    "${NPU_PATH}/${GENERATED_PROTO_DIR}/om.pb.cc"
    "${NPU_PATH}/${GENERATED_PROTO_DIR}/task.pb.cc"
)

LINK_DIRECTORIES("${NPU_PATH}/${WIN_LIB_OUT}")

add_definitions(-DPROTOBUF_USE_DLLS)

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_NAME ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "/MP" )

target_link_libraries(${PROJECT_NAME} "libprotobuf")


