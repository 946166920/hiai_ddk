### define sub project ai::fmk::model_builder::ir::aipp
hi_add_subdirectory(aipp)

### define ai::fmk::model_builder::ir_objs ###

hi_cc_library_static(
  NAME
    ai::fmk::model_builder::ir_objs
  INCS
    vendor/hisi/ap/interface/ai/include/model_manager
  SRCS
    build/hiai_ir_build.cpp
)
