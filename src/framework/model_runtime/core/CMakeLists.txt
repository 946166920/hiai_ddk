hi_cc_library_static(
  NAME
    ai::fmk::model_runtime::core_static
  SRCS
    hiai_built_model.c
    hiai_built_model_impl.c
    hiai_model_builder.c
    hiai_model_builder_impl.c
    hiai_model_builder_types.c
    hiai_model_manager.c
    hiai_model_manager_impl.c
    hiai_model_manager_types.c
    hiai_model_runtime.c
    hiai_model_runtime_repo.c
    model_runtime_initializer.cpp

    hiai_model_manager_aipp.c
    hiai_built_model_aipp.c

    model_runtime_register.cpp
)

# hi_cc_function(AI_SUPPORT_AIPP_API
#   NAME
#     ai::fmk::model_runtime::core_static
#   SRCS
#     hiai_model_manager_aipp.c
#     hiai_built_model_aipp.c
# )

# hi_cc_function(AI_SUPPORT_STATIC_LOAD_RUNTIME
#   NAME
#     ai::fmk::model_runtime::core_static
#   SRCS
#     model_runtime_register.cpp
# )

hi_cc_library_static(
  NAME
    ai::fmk::model_runtime::core_v1_static
  SRCS
    hiai_model_build_options_v1.c
    hiai_model_manager_options_v1.c
)
