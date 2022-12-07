cmake_minimum_required(VERSION 3.14)

project(hiai VERSION 1.0 LANGUAGES C CXX)

### set c++ standard to c++11.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND CMAKE_MODULE_PATH ${ROOT_DIR}/build/core/himake)
include(hi_make)

### redirect output directory
set(output_prefix ${ROOT_DIR}/out/hiai/${ANDROID_ABI})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${output_prefix}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${output_prefix}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${output_prefix}/static)

### global constant
set(HI_SYSTEM_NAME "HI_ANDROID_NDK")

### feature switch
hi_cc_feature_option(AI_USE_LOG_STATIC ON)
hi_cc_feature_option(AI_USE_LOG_SHARED OFF)

hi_cc_feature_option(HIAI_DDK ON)
hi_cc_feature_option(AI_SUPPORT_STATIC_LOAD_RUNTIME ON)
hi_cc_feature_option(AI_SUPPORT_AIPP_API ON)
hi_cc_feature_option(AI_SUPPORT_MODEL_BUILDER_COMPATIBLE ON)
hi_cc_feature_option(AI_SUPPORT_AIPP_COMPATIBLE ON)
hi_cc_feature_option(AI_SUPPORT_GRAPH_API ON)
hi_cc_feature_option(AI_SUPPORT_PLUGIN_COMPATIBLE ON)
hi_cc_feature_option(AI_SUPPORT_LEGACY_APP_COMPATIBLE ON)
hi_cc_feature_option(AI_SUPPORT_LEGACY_ROM_COMPATIBLE ON)
hi_cc_feature_option(AI_SUPPORT_BUILT_MODEL_SAVE ON)
hi_cc_feature_option(AI_SUPPORT_SPECIAL_3RD_MODEL ON)
hi_cc_feature_option(AI_SUPPORT_UPGRADE_APK ON)
hi_cc_feature_option(STATIC_IR_GRAPH_PROTO ON)

hi_cc_feature_option(AI_SUPPORT_PRIORITY_EXTEND OFF)
hi_cc_feature_option(AI_SUPPORT_LOG_LEVEL_SETTING OFF)
hi_cc_feature_option(AI_SUPPORT_CL_CUSTOMIZATION OFF)

hi_cc_default(
  NAME
    hiai::default_inc
  INCS
    ${CMAKE_CURRENT_SOURCE_DIR}/api/framework
    ${CMAKE_CURRENT_SOURCE_DIR}/api/infra
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/framework
    ${CMAKE_CURRENT_SOURCE_DIR}/src/framework/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/framework
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/bounds_checking_function/include
)

## external deps
hi_add_subdirectory(build/core/cmake/external)

### src
hi_add_subdirectory(src/infra)
hi_add_subdirectory(src/framework)