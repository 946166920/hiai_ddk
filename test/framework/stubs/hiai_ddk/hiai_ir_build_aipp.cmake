set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../../)
set(FRAMEWORK_BASE_DIR_FOR_INC_DIRS ${TOP_DIR}/src/framework)
set(THIRD_PARTY_PATH ${TOP_DIR}/third_party/)
set(THIRD_PARTY_CSEC_PATH ${THIRD_PARTY_PATH}/bounds_checking_function/)

set(LOCAL_SRC_FILES
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_builder/ir/aipp/converter/aipp_param_info_converter.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_builder/ir/aipp/compatible/hiai_ir_aipp_compatible.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_builder/ir/aipp/infershape/aipp_infershape_util.cpp
)

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--gc-sections")

set(LIBHIAI_IR_BUILD_AIPP_DDK hiai_ir_build_aipp_ddk)
add_library(${LIBHIAI_IR_BUILD_AIPP_DDK} SHARED ${LOCAL_SRC_FILES})

target_compile_definitions(${LIBHIAI_IR_BUILD_AIPP_DDK}
    PRIVATE
    _GLIBCXX_USE_CXX11_ABI=0
)

target_include_directories(${LIBHIAI_IR_BUILD_AIPP_DDK}
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

target_link_libraries(${LIBHIAI_IR_BUILD_AIPP_DDK}
    hiai_ir_ddk
)
