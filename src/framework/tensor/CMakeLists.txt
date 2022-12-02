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
