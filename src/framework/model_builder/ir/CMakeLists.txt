### define sub project ai::fmk::model_builder::ir::aipp
hi_add_subdirectory(aipp)

## define ai::fmk::model_builder::ir_static ###

hi_cc_library_static(
  NAME
    ai::fmk::model_builder::ir_static
  SRCS
    build/hiai_ir_build.cpp
)
