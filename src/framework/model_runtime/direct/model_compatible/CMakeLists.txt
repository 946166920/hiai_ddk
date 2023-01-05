### ddk for ai::fmk::hiai_model_compatible_shared ###

hi_cc_library_shared(
  NAME
    ai::fmk::hiai_model_compatible_shared
  SRCS
    hiai_model_compatible.cpp
    ir_build_transformer.cpp
  DEPS
    huawei::c_sec
    ai::fmk::hiai_ir_shared
    ai::fmk::util::hiai_version_static
    ai::fmk::util::dl_helper_static
    ai::fmk::omg::quantize_util_static
  CDEFS
    HIAI_C_API_VISIABLE
)

hi_target_output_name(ai::fmk::hiai_model_compatible_shared hiai_model_compatible)