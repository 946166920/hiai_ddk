cmake_minimum_required(VERSION 3.8.0)

set(TOP_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../)
set(FRAMEWORK_BASE_DIR_FOR_INC_DIRS ${TOP_DIR}/src/framework)
set(THIRD_PARTY_PATH ${TOP_DIR}/third_party/)
set(THIRD_PARTY_CSEC_PATH ${THIRD_PARTY_PATH}/bounds_checking_function/)

set(HIAI_SRC_FILES
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model/built_model/built_model_impl.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_builder/om/model_build_options_util.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_builder/om/model_builder_impl.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_manager/core/model_manager_impl.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/infra/buffer/local_buffer.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/infra/buffer/base_buffer.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/infra/buffer/hiai_native_handle.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/infra/buffer/hiai_shared_buffer.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/base/nd_tensor_buffer_impl.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/base/hiai_nd_tensor_desc.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/base/hiai_nd_tensor_buffer.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/base/hiai_nd_tensor_buffer_util.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/base/hiai_nd_tensor_buffer_local.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/base/hiai_nd_tensor_buffer_legacy.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/base/hiai_nd_tensor_buffer_legacy_compatible.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/base/hiai_nd_tensor_buffer_ext.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/util/version/version_util.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/util/hiai_base_types.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/util/native_handle_creator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../hiai_ddk/src/stub_hiai_foundation_dl_helper.c

    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/hiai_built_model_impl.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/hiai_built_model_aipp.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/hiai_built_model.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/hiai_model_builder_impl.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/hiai_model_builder.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/hiai_model_build_options_v1.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/hiai_model_builder_types.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/hiai_model_manager_impl.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/hiai_model_manager_aipp.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/hiai_model_manager.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/hiai_model_manager_options_v1.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/hiai_model_manager_types.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/hiai_model_runtime_repo.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/hiai_model_runtime.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/model_runtime_initializer.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/core/model_runtime_register.cpp

    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/direct/direct_model_runtime.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/direct/direct_model_compatible_proxy.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/direct/direct_model_builder.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/direct/direct_built_model.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/direct/direct_built_model_impl.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/direct/direct_common_util.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/direct/direct_model_util.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/direct/direct_model_manager.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/direct/direct_model_manager_impl.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/om/event_manager/om_wrapper.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/om/event_manager/ai_om_shim.cpp
    ${TOP_DIR}/src/infra/dl_helper/dynamic_load_helper.cpp

    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_manager/compatible/AiContext.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_manager/compatible/AiModelBuilder.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_manager/compatible/AiModelDescription.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_builder/compatible/BuildOptionUtil.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_manager/compatible/HiAiModelManagerService.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_manager/compatible/MemBuffer.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_manager/compatible/MembufferUtil.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/compatible/AiTensor.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/compatible/HiAiAippPara.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/compatible/AippTensor.cpp

    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model/built_model/customdata_util.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model/aipp/aipp_input_converter.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/aipp/aipp_para_impl.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/aipp/hiai_tensor_aipp_para_legacy.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/aipp/hiai_tensor_aipp_para_local.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/aipp/hiai_tensor_aipp_para.c
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/image/image_tensor_buffer.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/tensor/image/image_tensor_buffer_impl.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/model_runtime/direct/direct_built_model_aipp.cpp

    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/util/file_util.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/util/model_type_util.cpp
    ${FRAMEWORK_BASE_DIR_FOR_INC_DIRS}/common/file_util.cpp
)

set(HIAI_C_SEC_SRC
    ${THIRD_PARTY_CSEC_PATH}/src/strcpy_s.c
    ${THIRD_PARTY_CSEC_PATH}/src/strcat_s.c
    ${THIRD_PARTY_CSEC_PATH}/src/memcpy_s.c
    ${THIRD_PARTY_CSEC_PATH}/src/memset_s.c
)

set(LOCAL_SRC_FILES
    ${HIAI_C_SEC_SRC}
    ${HIAI_SRC_FILES}
)

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--gc-sections")

set(CMAKE_C_FLAGS "-pthread -fPIC -Os -ffunction-sections -fdata-sections -Wno-unused-parameter -D_Float16=uint16_t -DOS_TYPE=0 -DHIAI_DDK -DHIAI_MM_API_VISIABLE -DHIAI_MB_API_VISIABLE -DHIAI_M_API_VISIABLE -DHIAI_UTIL_API_VISIABLE -DHIAI_TENSOR_API_VISIABLE -DHIAI_C_API_VISIABLE -DAI_SUPPORT_AIPP_API -DAI_SUPPORT_GRAPH_API -DAI_SUPPORT_BUILT_MODEL_SAVE -DAI_SUPPORT_CL_CUSTOMIZATION -DAI_SUPPORT_SPECIAL_3RD_MODEL -DANDROID -DAI_SUPPORT_LEGACY_APP_COMPATIBLE -DAI_SUPPORT_LEGACY_ROM_COMPATIBLE")

set(CMAKE_CXX_FLAGS "-std=c++11 -pthread -fPIC -frtti -Os -ffunction-sections -fdata-sections -Wno-unused-parameter -D_Float16=uint16_t -DOS_TYPE=0 -DHIAI_DDK -DHIAI_MM_API_VISIABLE -DHIAI_MB_API_VISIABLE -DHIAI_M_API_VISIABLE -DHIAI_UTIL_API_VISIABLE -DHIAI_TENSOR_API_VISIABLE -DHIAI_C_API_VISIABLE -DAI_SUPPORT_AIPP_API -DAI_SUPPORT_GRAPH_API -DAI_SUPPORT_BUILT_MODEL_SAVE -DAI_SUPPORT_CL_CUSTOMIZATION -DAI_SUPPORT_SPECIAL_3RD_MODEL -DANDROID -DAI_SUPPORT_LEGACY_APP_COMPATIBLE -DAI_SUPPORT_LEGACY_ROM_COMPATIBLE")


set(LIBHIAI_DDK hiai_ddk)
add_library(${LIBHIAI_DDK} SHARED ${LOCAL_SRC_FILES})

target_compile_definitions(${LIBHIAI_DDK}
    PRIVATE
    _GLIBCXX_USE_CXX11_ABI=0
)

target_include_directories(${LIBHIAI_DDK}
    PRIVATE
    ${TOP_DIR}/inc
    ${TOP_DIR}/inc/framework
    ${TOP_DIR}/inc/framework/util
    ${TOP_DIR}/inc/infra
    ${TOP_DIR}/inc/infra/base
    ${TOP_DIR}/inc/infra/om
    ${TOP_DIR}/inc/infra/mmpa
    ${TOP_DIR}/api
    ${TOP_DIR}/api/infra
    ${TOP_DIR}/api/framework
    ${TOP_DIR}/src
    ${TOP_DIR}/src/framework
    ${TOP_DIR}/src/framework/inc
    ${TOP_DIR}/src/framework/util
    ${THIRD_PARTY_PATH}/cutils
    ${THIRD_PARTY_CSEC_PATH}/include
)


