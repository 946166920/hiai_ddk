hi_cc_library_static(
  NAME
    ai::fmk::model_static
  SRCS
    built_model/built_model_impl.cpp
  CDEFS
    HIAI_M_API_VISIABLE
)

hi_cc_function(AI_SUPPORT_AIPP_API
  NAME
    ai::fmk::model_static
  SRCS
    built_model/customdata_util.cpp
    aipp/aipp_input_converter.cpp
  COPTS
    -frtti
)