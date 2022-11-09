### sub project
hi_add_subdirectory(ir)

hi_cc_library_static(
  NAME
    ai::fmk::model_builder_static
  SRCS
    om/*.cpp
  CDEFS
    HIAI_MB_API_VISIABLE
)

hi_cc_function(AI_SUPPORT_MODEL_BUILDER_COMPATIBLE
  NAME
    ai::fmk::model_builder_static
  SRCS
    compatible/BuildOptionUtil.cpp
)

hi_cc_library_static(
  NAME
    ai::fmk::model_builder::compatible_static
  SRCS
    compatible/BuildOptionUtil.cpp
)

# ### define ddk libhiai_ir_build ###

hi_cc_library_shared(
  NAME
    ai::fmk::hiai_ir_build_shared
  WHOLE_STATIC_LIBS
    ai::fmk::hiai_ir_shared
    ai::fmk::model_manager_shared
    ai::fmk::model_builder::ir_static
    ai::fmk::model_builder::compatible_static
  CDEFS
    AI_SUPPORT_AIPP_API
    HIAI_MB_API_VISIABLE
)

hi_target_output_name(ai::fmk::hiai_ir_build_shared hiai_ir_build)

### define ddk for libhiai_ir_build_aipp ###

hi_cc_library_shared(
  NAME
    ai::fmk::hiai_ir_build_aipp_shared
  WHOLE_STATIC_LIBS
    ai::fmk::hiai_ir_shared
    ai::fmk::model_builder::ir::aipp_static
)

hi_target_output_name(ai::fmk::hiai_ir_build_aipp_shared hiai_ir_build_aipp)