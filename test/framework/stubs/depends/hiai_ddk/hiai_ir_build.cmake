set(LOCAL_SRC_FILES
    ${CMAKE_CURRENT_LIST_DIR}/../hiai_ddk/src/stub_hiai_ir_build.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_builder/compatible/BuildOptionUtil.cpp
)

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--gc-sections")

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


