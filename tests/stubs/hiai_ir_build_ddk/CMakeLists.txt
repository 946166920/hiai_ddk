set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../)
set(FRAMEWORK_BASE_DIR_FOR_INC_DIRS ${TOP_DIR}/src/framework)
set(THIRD_PARTY_PATH ${TOP_DIR}/third_party/)
set(THIRD_PARTY_CSEC_PATH ${THIRD_PARTY_PATH}/bounds_checking_function/)

set(LOCAL_SRC_FILES
    ${CMAKE_CURRENT_LIST_DIR}/../hiai_ir_build_ddk/src/stub_hiai_ir_build.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_builder/compatible/BuildOptionUtil.cpp
)

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--gc-sections")

set(CMAKE_C_FLAGS
    -DAI_SUPPORT_AIPP_API
    -DAI_SUPPORT_GRAPH_API 
    -DAI_SUPPORT_SPECIAL_3RD_MODEL 
)

set(CMAKE_CXX_FLAGS "-std=c++11 -pthread -fPIC -frtti -Os -ffunction-sections -fdata-sections -fno-asynchronous-unwind-tables -fno-exceptions -Wno-unused-parameter -fomit-frame-pointer -fno-unwind-tables -DHAVE_PTHREAD -D_FORTIFY_SOURCE=2 -DHOST_VISIBILITY -DGRAPH_API_VISIABLE")

set(LIBHIAI_IR_BUILD_DDK hiai_ir_build_ddk)
add_library(${LIBHIAI_IR_BUILD_DDK} SHARED ${LOCAL_SRC_FILES})

target_compile_definitions(${LIBHIAI_IR_BUILD_DDK}
    PRIVATE
    _GLIBCXX_USE_CXX11_ABI=0
)

target_include_directories(${LIBHIAI_IR_BUILD_DDK}
    PRIVATE
    ${TOP_DIR}/inc
    ${TOP_DIR}/inc/framework
    ${TOP_DIR}/inc/framework/util
    ${TOP_DIR}/inc/infra
    ${TOP_DIR}/inc/infra/om
    ${TOP_DIR}/inc/infra/mmpa
    ${TOP_DIR}/api
    ${TOP_DIR}/api/infra
    ${TOP_DIR}/api/framework
    ${TOP_DIR}/src
    ${TOP_DIR}/src/framework
    ${TOP_DIR}/src/framework/inc
    ${TOP_DIR}/src/framework/util
    ${TOP_DIR}/src/framework/model_manager
    ${THIRD_PARTY_CSEC_PATH}/include
)

target_link_libraries(${LIBHIAI_IR_BUILD_DDK}
    hiai_ir_ddk
)


