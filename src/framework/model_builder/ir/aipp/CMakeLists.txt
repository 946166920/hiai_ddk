### define ai::fmk::model_builder::ir::aipp_objs
hi_cc_objects(
  NAME
    ai::fmk::model_builder::ir::aipp_objs
  SRCS
    infershape/aipp_infershape_util.cpp
    converter/aipp_param_info_converter.cpp
    compatible/hiai_ir_aipp_compatible.cpp
)

hi_system("HI_ANDROID_AOSP;HI_LINUX_HOST"
  NAME
    ai::fmk::model_builder::ir::aipp_objs
  SRCS
    omg/hiai_ir_aipp_compatible_adapter_api.cpp
)

hi_generator(
  NAMES
    ai::fmk::model_builder::ir::aipp_objs
)