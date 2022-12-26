### define ai::fmk::model_builder::ir::aipp_static
hi_cc_library_static(
  NAME
    ai::fmk::model_builder::ir::aipp_static
  SRCS
    infershape/aipp_infershape_util.cpp
    converter/aipp_param_info_converter.cpp
    compatible/hiai_ir_aipp_compatible.cpp
  DEPS
    hiai::api::ops
    hiai::inc::ops
)