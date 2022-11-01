### sub project
hi_add_subdirectory(ir)
# hi_add_subdirectory(optimizer)

hi_cc_library_static(
  NAME
    ai::fmk::model_builder_objs
  SRCS
    om/*.cpp
  CDEFS
    HIAI_MB_API_VISIABLE
  DEPS
    huawei::c_sec
)

hi_cc_function(AI_SUPPORT_MODEL_BUILDER_COMPATIBLE
  NAME
    ai::fmk::model_builder_objs
  SRCS
    compatible/BuildOptionUtil.cpp
)

hi_cc_library_static(
  NAME
    ai::fmk::model_builder::compatible_objs
  SRCS
    compatible/BuildOptionUtil.cpp
)

# hi_generator(
#   NAMES
#     ai::fmk::model_builder_objs
#     ai::fmk::model_builder::compatible_objs
# )

### define ddk libhiai_ir_build ###

hi_cc_library_shared(
  NAME
    ai::fmk::hiai_ir_build_shared
  DEPS
    ai::infra::log
    ai::fmk::hiai_ir_shared
    ai::fmk::model_manager_shared
    ai::fmk::model_builder::ir_objs
    ai::fmk::model_builder::compatible_objs
  CDEFS
    AI_SUPPORT_AIPP_API
    HIAI_MB_API_VISIABLE
)

hi_cc_function(AI_SUPPORT_UPGRADE_APK
  NAME
    ai::fmk::hiai_ir_build_shared
  INCS
    cloud_service
  CDEFS
    AI_SUPPORT_UPGRADE_APK
)

hi_cc_function(AI_SUPPORT_GRAPH_API
  NAME
    ai::fmk::hiai_ir_build_shared
  CDEFS
    AI_SUPPORT_GRAPH_API
)

### define ddk for libhiai_ir_build_aipp ###

hi_cc_library_shared(
  NAME
    ai::fmk::hiai_ir_build_aipp_shared
  DEPS
    ai::infra::log
    ai::fmk::hiai_ir_shared
    ai::fmk::model_builder::ir::aipp_objs
)

hi_cc_function(AI_SUPPORT_UPGRADE_APK
  NAME
    ai::fmk::hiai_ir_build_aipp_shared
  INCS
    cloud_service
  CDEFS
    AI_SUPPORT_UPGRADE_APK
)
