cmake_minimum_required(VERSION 3.6.0)

set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../../)
set(THIRD_PARTY_PATH ${TOP_DIR}/third_party/)
set(THIRD_PARTY_CSEC_PATH ${THIRD_PARTY_PATH}/libboundscheck/)

set(TESTCASES_FILES_PATH ${CMAKE_CURRENT_LIST_DIR}/../model_manager_v2/test)
set(STUB_FILES_PATH ${CMAKE_CURRENT_LIST_DIR}/../model_manager_v2/stub)

set(LOCAL_SRC_FILES
    ${TOP_DIR}/src/framework/model_builder/ir/build/hiai_ir_build.cpp
    ${STUB_FILES_PATH}/control_c.cpp
    ${STUB_FILES_PATH}/stub_hiai_built_model.cpp
    ${STUB_FILES_PATH}/stub_hiai_built_model_aipp.c
    ${STUB_FILES_PATH}/stub_hiai_model_manager.cpp
    ${STUB_FILES_PATH}/stub_hiai_model_manager_types.c
    ${STUB_FILES_PATH}/stub_hiai_model_build_types.cpp
    ${STUB_FILES_PATH}/stub_hiai_model_builder.cpp
    ${STUB_FILES_PATH}/stub_hiai_nd_tensor_desc.cpp
    ${STUB_FILES_PATH}/stub_model_manager_listener.cpp
    ${TESTCASES_FILES_PATH}/../main.cpp
    ${TESTCASES_FILES_PATH}/common.cpp
    ${TESTCASES_FILES_PATH}/built_model_ut.cpp
    ${TESTCASES_FILES_PATH}/model_manager_ut.cpp
    ${TESTCASES_FILES_PATH}/model_manager_aipp_ut.cpp
    ${TESTCASES_FILES_PATH}/static_shape_ut.cpp
    ${TESTCASES_FILES_PATH}/dynamic_shape_ut.cpp
    ${TESTCASES_FILES_PATH}/model_device_config_ut.cpp
    ${TESTCASES_FILES_PATH}/hiai_ir_build_ut.cpp
    ${TESTCASES_FILES_PATH}/load_model_ut.cpp
)

set(GTEST_DIR "${CMAKE_CURRENT_LIST_DIR}/googletest")
set(MOCKCPP_DIR "${CMAKE_CURRENT_LIST_DIR}/mockcpp")

add_compile_options("--coverage")

set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -Wall -fprofile-arcs -ftest-coverage")


set(CMAKE_C_FLAGS "-std=c99 -fexceptions -g -fprofile-arcs -ftest-coverage -DAI_SUPPORT_AIPP_API -DAI_SUPPORT_GRAPH_API -DAI_SUPPORT_BUILT_MODEL_SAVE -DAI_SUPPORT_SPECIAL_3RD_MODEL -DAI_SUPPORT_CL_CUSTOMIZATION -DHIAI_UTIL_API_EXPORT -D_Float16=uint16_t")

set(COVERAGE_COMPILER_FLAGS "-std=c++11 -pthread -Os -fPIC -fpermissive -frtti -fexceptions -ffunction-sections -fno-asynchronous-unwind-tables -fomit-frame-pointer -fno-unwind-tables -fno-access-control -DHIAI_DDK -D_FORTIFY_SOURCE=2 -DHAVE_PTHREAD -DHOST_VISIBILITY -DAI_SUPPORT_AIPP_API -fdata-sections -DFMK_FEATURE_DYNAMIC_SHAPE -DAI_SUPPORT_GRAPH_API -DAI_SUPPORT_BUILT_MODEL_SAVE -DAI_SUPPORT_SPECIAL_3RD_MODEL -DAI_SUPPORT_CL_CUSTOMIZATION -DHIAI_UTIL_API_EXPORT -D_Float16=uint16_t")
set(CMAKE_CXX_FLAGS "${COVERAGE_COMPILER_FLAGS}")

include_directories(model_manager_v2_ut
    PRIVATE
    ${TOP_DIR}/api
    ${TOP_DIR}/api/infra
    ${TOP_DIR}/api/framework
    ${TOP_DIR}/inc
    ${TOP_DIR}/inc/framework
    ${TOP_DIR}/inc/framework/c/compatible
    ${TOP_DIR}/inc/infra
    ${TOP_DIR}/src
    ${TOP_DIR}/src/framework
    ${TOP_DIR}/src/framework/inc
    ${TOP_DIR}/src/inc
    ${TESTCASES_FILES_PATH}/src
    ${THIRD_PARTY_CSEC_PATH}/include
    ${GTEST_DIR}/googletest/include
    ${GTEST_DIR}/googlemock/include
    ${MOCKCPP_DIR}/mockcpp/include
    ${TOP_DIR}/tests/ut/model_manager/model_manager_v2/stub
    ${TOP_DIR}/tests/stubs/ai_client/src
)

add_executable(model_manager_v2_ut ${LOCAL_SRC_FILES})
target_compile_definitions(model_manager_v2_ut
    PRIVATE
    _GLIBCXX_USE_CXX11_ABI=0
    )
target_compile_options(model_manager_v2_ut
    PRIVATE
      -g --coverage -fprofile-arcs -ftest-coverage
      -Werror=format
    )

target_link_libraries(model_manager_v2_ut
    gtest
    gcov
    mockcpp
    dl
    hiai_ddk
    hiai_ir_ddk
    hiai_ir_build_aipp_ddk
    ai_client_stub_ddk
)

add_test(NAME model_manager_v2_ut COMMAND model_manager_v2_ut)


