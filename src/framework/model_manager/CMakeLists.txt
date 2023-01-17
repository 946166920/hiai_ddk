hi_cc_library_static(
  NAME
    ai::fmk::model_manager_static
  SRCS
    core/model_manager_impl.cpp
  CDEFS
    HIAI_MM_API_VISIABLE
    HIAI_HMR_API_VISIABLE
    AI_DDK_VERSION=\"${out}\"
    HIAI_DDK
  COPTS
    -frtti
)

hi_cc_function(AI_SUPPORT_LEGACY_APP_COMPATIBLE
  NAME
    ai::fmk::model_manager_static
  SRCS
    compatible/AiContext.cpp
    compatible/AiModelBuilder.cpp
    compatible/AiModelDescription.cpp
    compatible/HiAiModelManagerService.cpp
    compatible/MemBuffer.cpp
    compatible/MembufferUtil.cpp
)

hi_cc_library_static(
  NAME
    ai::fmk::model_manager_stats_static
  SRCS
    core/open_request_stats.cpp
  CDEFS
    HIAI_MM_API_VISIABLE
  COPTS
    -frtti
)

hi_cc_library_shared(
  NAME
    ai::fmk::model_manager_shared
  WHOLE_STATIC_LIBS
    ai::fmk::model_manager_static
    ai::fmk::model_builder_static
    ai::fmk::model_runtime::core_static
    ai::fmk::model_runtime::core_v1_static
    ai::fmk::model_static
    ai::fmk::tensor_static
    ai::fmk::util::model_type_util_static
    ai::fmk::tensor::nd_tensor_impl_static
    ai::fmk::tensor::nd_tensor_ext_static
    ai::fmk::util::version_static
    ai::fmk::util::hiai_plugin_version_static
    ai::fmk::util::base_types_static
    ai::fmk::infra::buffer_static
    ai::fmk::infra::base_buffer_static
    ai::fmk::infra::native_handle_static
    ai::fmk::util::native_handle_static
  
    ai::fmk::util::file_static
  
    ai::infra::base::process_util_static
    ai::fmk::model_manager_stats_static

    ai::infra::log
    huawei::c_sec
  EXCLUDE_STATIC_LIBS
    ai::fmk::common::file_util_static
    ai::infra::dl_helper_static
    ai::fmk::tensor::nd_tensor_static
    ai::fmk::util::dl_helper_static
  COPTS
    -frtti
    -std=c++11
  CDEFS
    HIAI_MM_API_VISIABLE
    HIAI_MB_API_VISIABLE
    HIAI_M_API_VISIABLE
    HIAI_UTIL_API_VISIABLE
    HIAI_TENSOR_API_VISIABLE
    OS_TYPE=0
)

hi_cc_function(AI_SUPPORT_LEGACY_ROM_COMPATIBLE
  NAME
    ai::fmk::model_manager_shared
  WHOLE_STATIC_LIBS
    ai::fmk::model_runtime::direct_static
)

hi_cc_function(AI_SUPPORT_PLUGIN_COMPATIBLE
  NAME
    ai::fmk::model_manager_shared
  WHOLE_STATIC_LIBS
    ai::fmk::om::om_wrapper_static
)

hi_cc_function(AI_SUPPORT_AIPP_API
  NAME
    ai::fmk::model_manager_shared
  WHOLE_STATIC_LIBS
    ai::fmk::tensor::aipp_para_static
  CDEFS
    AI_SUPPORT_AIPP_API
)

hi_target_output_name(ai::fmk::model_manager_shared hiai)