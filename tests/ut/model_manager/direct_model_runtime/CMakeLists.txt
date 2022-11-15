set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../../)
set(THIRD_PARTY_PATH ${TOP_DIR}/third_party/)
set(THIRD_PARTY_PROTOBUF_PATH ${THIRD_PARTY_PATH}/protobuf/)
set(THIRD_PARTY_CSEC_PATH ${THIRD_PARTY_PATH}/bounds_checking_function/)

set(TESTCASES_FILES_PATH ${CMAKE_CURRENT_LIST_DIR}/../direct_model_runtime)

set(TESTCASES_FILES
    ${TESTCASES_FILES_PATH}/main.cpp
    ${TESTCASES_FILES_PATH}/direct_built_model_aipp_ut.cpp
    ${TESTCASES_FILES_PATH}/direct_built_model_ut.cpp
    ${TESTCASES_FILES_PATH}/direct_model_builder_ut.cpp
    ${TESTCASES_FILES_PATH}/direct_model_manager_aipp_ut.cpp
    ${TESTCASES_FILES_PATH}/direct_model_manager_ut.cpp
)

set(GTEST_DIR "${CMAKE_CURRENT_LIST_DIR}/googletest")
set(MOCKCPP_DIR "${CMAKE_CURRENT_LIST_DIR}/mockcpp")

add_compile_options("--coverage")

set(CMAKE_EXE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT} -Wall -fprofile-arcs -ftest-coverage")

set(COVERAGE_COMPILER_FLAGS "-std=c++11 -pthread -Os -fPIC -fpermissive -frtti -fexceptions -ffunction-sections -fno-asynchronous-unwind-tables -fomit-frame-pointer -fno-unwind-tables -fno-access-control -DHIAI_DDK -D_FORTIFY_SOURCE=2 -DHAVE_PTHREAD -DHOST_VISIBILITY -DAI_SUPPORT_AIPP_API -fdata-sections -DFMK_FEATURE_DYNAMIC_SHAPE -DAI_SUPPORT_GRAPH_API -DAI_SUPPORT_BUILT_MODEL_SAVE -DAI_SUPPORT_PREFMODE_EXTEND -DAI_SUPPORT_PRIORITY_EXTEND -DAI_SUPPORT_SPECIAL_3RD_MODEL")
set(CMAKE_CXX_FLAGS "${COVERAGE_COMPILER_FLAGS}")

include_directories(direct_model_runtime_ut
    PRIVATE
    ${TOP_DIR}/api
    ${TOP_DIR}/api/infra
    ${TOP_DIR}/api/framework
    ${TOP_DIR}/api/framework/model_manager
    ${TOP_DIR}/inc
    ${TOP_DIR}/inc/framework
    ${TOP_DIR}/inc/framework/compatible
    ${TOP_DIR}/inc/framework/tensor
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
)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

add_executable(direct_model_runtime_ut ${TESTCASES_FILES})
target_compile_definitions(direct_model_runtime_ut
    PRIVATE
    _GLIBCXX_USE_CXX11_ABI=0
    )
target_compile_options(direct_model_runtime_ut
    PRIVATE
      -g --coverage -fprofile-arcs -ftest-coverage
      -Werror=format
    )

target_link_libraries(direct_model_runtime_ut
    gtest
    gcov
    mockcpp
    dl
    hiai_ddk
    hiai_ir_ddk
    ai_client_stub_ddk
)

add_test(NAME direct_model_runtime_ut COMMAND direct_model_runtime_ut)


