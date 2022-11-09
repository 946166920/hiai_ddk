cmake_minimum_required(VERSION 3.6.0)

set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
set(BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
if (EXISTS ${TOP_DIR}/third_party/protobuf)
  set(PROTOBUF_DIR ${TOP_DIR}/third_party/protobuf)
endif()

include(../cmake/external_libs/protoc.cmake)

include(../cmake/external_libs/gtest.cmake)

include(../cmake/external_libs/mockcpp.cmake)

if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)

project(googletest-distribution)
set(GOOGLETEST_VERSION 1.10.0)

include(CMakeDependentOption)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)
add_definitions(-std=c++11)
add_definitions(-fPIC -pthread -DHAVE_PTHREAD -D_FORTIFY_SOURCE=2 -ffunction-sections -fdata-sections -Os)
add_definitions(-DHIAI_IR_ONLINE_BUILD -DBUILDING_WITH_GTEST)
add_definitions(-DENABLE_MIX_COMPILE)

##################################################################
add_subdirectory(ut)
add_subdirectory(stubs)