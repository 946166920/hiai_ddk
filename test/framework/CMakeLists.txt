cmake_minimum_required(VERSION 3.8.0)

set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR}/../..)
set(BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
if (EXISTS ${TOP_DIR}/third_party/protobuf)
  set(PROTOBUF_DIR ${TOP_DIR}/third_party/protobuf)
endif()

set(CMAKE_INSTALL_LIBDIR ${BASE_DIR}/build)

include(${TOP_DIR}/test/cmake/external_libs/protoc.cmake)

include(${TOP_DIR}/test/cmake/external_libs/gtest.cmake)

include(${TOP_DIR}/test/cmake/external_libs/mockcpp.cmake)

if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(googletest-distribution)
set(GOOGLETEST_VERSION 1.10.0)

include(CMakeDependentOption)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
add_definitions(-fPIC -pthread -DHAVE_PTHREAD -D_FORTIFY_SOURCE=2 -ffunction-sections -fdata-sections -Os)
add_definitions(-DHIAI_IR_ONLINE_BUILD -DBUILDING_WITH_GTEST)
add_definitions(-DENABLE_MIX_COMPILE)

set(op_h_file_list
  api/framework/graph/op/image_defs.h
  api/framework/graph/op/array_defs.h
  api/framework/graph/op/nn_defs.h
  api/framework/graph/op/random_defs.h
  api/framework/graph/op/control_flow_defs.h
  api/framework/graph/op/detection_defs.h
  api/framework/graph/op/const_defs.h
  api/framework/graph/op/math_defs.h
  inc/framework/graph/op/internal_defs.h
  inc/framework/graph/op/data_flow_defs.h
  inc/framework/graph/op/internal_nn_defs.h
)

foreach(h_file ${op_h_file_list})
  list(APPEND op_h_files ${TOP_DIR}/${h_file})
endforeach()

add_custom_command(OUTPUT ${op_h_files}
    PRE_BUILD
    COMMAND
    python3 ${TOP_DIR}/build/script/gen_op_def_h.py ${TOP_DIR} api
    COMMAND
    python3 ${TOP_DIR}/build/script/gen_op_def_h.py ${TOP_DIR} inc
)
add_custom_target(api_inc_ops ALL DEPENDS ${op_h_files})

##################################################################
add_subdirectory(ut)
add_subdirectory(stubs)