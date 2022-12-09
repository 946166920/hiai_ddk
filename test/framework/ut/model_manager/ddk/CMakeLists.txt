set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../../../)
set(THIRD_PARTY_PATH ${TOP_DIR}/third_party/)
set(THIRD_PARTY_CSEC_PATH ${THIRD_PARTY_PATH}/bounds_checking_function/)

set(TESTCASES_FILES_PATH ${CMAKE_CURRENT_LIST_DIR}/../ddk)

set(TESTCASES_FILES
    ${TESTCASES_FILES_PATH}/main.cpp
    ${TESTCASES_FILES_PATH}/src/common_utils.cpp
    ${TESTCASES_FILES_PATH}/src/stub_load_models.cpp
    ${TESTCASES_FILES_PATH}/aipp_para_v2_ut.cpp
    ${TESTCASES_FILES_PATH}/base_buffer_ut.cpp
    ${TESTCASES_FILES_PATH}/check_model_compatibility_ut.cpp
    ${TESTCASES_FILES_PATH}/create_aipppara_ut.cpp
    ${TESTCASES_FILES_PATH}/create_aitensor_ut.cpp
    ${TESTCASES_FILES_PATH}/create_membuffer_ut.cpp
    ${TESTCASES_FILES_PATH}/dynamic_load_helper_ut.cpp
    ${TESTCASES_FILES_PATH}/file_util_ut.cpp
    ${TESTCASES_FILES_PATH}/image_tensor_buffer_ut.cpp
    ${TESTCASES_FILES_PATH}/local_buffer_ut.cpp
    ${TESTCASES_FILES_PATH}/nd_tensor_buffer_ut.cpp
    ${TESTCASES_FILES_PATH}/process_dynamic_aipp_ut.cpp
    ${TESTCASES_FILES_PATH}/process_ut.cpp
    ${TESTCASES_FILES_PATH}/set_model_priority_ut.cpp
)

set(GTEST_DIR "${CMAKE_CURRENT_LIST_DIR}/googletest")
set(MOCKCPP_DIR "${CMAKE_CURRENT_LIST_DIR}/mockcpp")

add_compile_options("--coverage")

set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -Wall -fprofile-arcs -ftest-coverage")

set(COVERAGE_COMPILER_FLAGS "-std=c++11 -pthread -Os -fPIC -fpermissive -frtti -fexceptions -ffunction-sections -fno-asynchronous-unwind-tables -fomit-frame-pointer -fno-unwind-tables -fno-access-control -DHIAI_DDK -D_FORTIFY_SOURCE=2 -DHAVE_PTHREAD -DHOST_VISIBILITY -DFMK_FEATURE_DYNAMIC_SHAPE -fdata-sections -g -fprofile-arcs -O0 -ftest-coverage")
set(CMAKE_CXX_FLAGS "${COVERAGE_COMPILER_FLAGS}")

include_directories(ddk_model_manager_ut
    PRIVATE
    ${TOP_DIR}/api
    ${TOP_DIR}/api/infra
    ${TOP_DIR}/api/framework
    ${TOP_DIR}/api/framework/model_builder
    ${TOP_DIR}/api/framework/model_manager
    ${TOP_DIR}/inc
    ${TOP_DIR}/inc/framework
    ${TOP_DIR}/inc/framework/graph
    ${TOP_DIR}/inc/framework/compatible
    ${TOP_DIR}/inc/framework/tensor
    ${TOP_DIR}/inc/infra
    ${TOP_DIR}/src
    ${TOP_DIR}/src/framework
    ${TOP_DIR}/src/framework/inc
    ${TOP_DIR}/src/framework/graph
    ${TOP_DIR}/src/inc
    ${TOP_DIR}/test/framework/stubs/hiai_ddk/src
    ${TESTCASES_FILES_PATH}/src
    ${THIRD_PARTY_CSEC_PATH}/include
    ${GTEST_DIR}/googletest/include
    ${GTEST_DIR}/googlemock/include
    ${MOCKCPP_DIR}/mockcpp/include
)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_executable(ddk_model_manager_ut ${TESTCASES_FILES})
target_compile_definitions(ddk_model_manager_ut
    PRIVATE
    _GLIBCXX_USE_CXX11_ABI=0
    )
target_compile_options(ddk_model_manager_ut
    PRIVATE
      -g --coverage -fprofile-arcs -ftest-coverage
      -Werror=format
    )

target_link_libraries(ddk_model_manager_ut
    gtest
    mockcpp
    dl
    hiai_ddk
    hiai_ir_ddk
    hiai_ir_build_ddk
    hiai_ir_build_aipp_ddk
    ai_client_stub_ddk
)

add_test(NAME ddk_model_manager_ut COMMAND ddk_model_manager_ut)


