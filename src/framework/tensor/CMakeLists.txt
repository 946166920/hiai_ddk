hi_add_subdirectory(aipp)
hi_add_subdirectory(base)

hi_cc_library_static(
  NAME
    ai::fmk::tensor_static
  SRCS
    image/image_tensor_buffer_impl.cpp
    image/image_tensor_buffer.cpp
  CDEFS
    HIAI_TENSOR_API_VISIABLE
  COPTS
    -frtti
)

hi_cc_function(AI_SUPPORT_LEGACY_APP_COMPATIBLE
  NAME
    ai::fmk::tensor_static
  SRCS
    compatible/AiTensor.cpp
)

hi_cc_function(AI_SUPPORT_AIPP_COMPATIBLE
  NAME
    ai::fmk::tensor_static
  SRCS
    compatible/AippTensor.cpp
    compatible/HiAiAippPara.cpp
)

# hi_cc_library_shared(
#   NAME
#     ai::fmk::hiai_c_def_shared
#   DEPS
#     huawei::c_sec
#     ai::infra::log
#     ai::fmk::util::version_static
#     ai::fmk::util::hiai_version_static
#     ai::fmk::infra::native_handle_static
#     ai::fmk::util::dl_helper_static
#     ai::fmk::util::base_types_static
#   WHOLE_STATIC_LIBS
#     ai::fmk::tensor::nd_tensor_static
#   CDEFS
#     HIAI_C_API_VISIABLE
# )

# hi_cc_system(HI_ANDROID_NDK
#   NAME
#     ai::fmk::hiai_c_def_shared
#   # EXCLUDE_STATIC_LIBS
#   #   ai::infra::mmpa
#   #   ai::infra::log
# )

# hi_cc_function(AI_SUPPORT_AIPP_API
#   NAME
#     ai::fmk::hiai_c_def_shared
#   # EXCLUDE_STATIC_LIBS
#   #   ai::fmk::tensor::aipp_para_static
# )


# hi_cc_library_shared(
#   NAME
#     ai::fmk::hiai_c_shared
#   DEPS
#     huawei::c_sec
#     # ai::infra::mmpa
#     ai::infra::log_shared
#     ai::fmk::hiai_c_def_shared
#     ai::fmk::model_runtime::core_static
#     ai::fmk::model_runtime::core_v1_static
#   CDEFS
#     HIAI_C_API_VISIABLE
# )

# hi_cc_function(AI_SUPPORT_PREFMODE_EXTEND
#   NAME
#     ai::fmk::hiai_c_shared
#   CDEFS
#     AI_SUPPORT_PREFMODE_EXTEND
# )
