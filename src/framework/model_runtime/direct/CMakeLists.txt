hi_add_subdirectory(model_compatible)

hi_cc_library_static(
  NAME
    ai::fmk::model_runtime::direct_static
  SRCS
    direct_model_runtime.cpp
    direct_model_compatible_proxy.cpp
    direct_model_builder.cpp
    direct_built_model.cpp
    direct_built_model_impl.cpp
    direct_common_util.cpp
    direct_model_manager_util.cpp
    direct_model_manager_container.cpp
    direct_model_manager.cpp
    direct_model_manager_impl.cpp
)

hi_cc_function(AI_SUPPORT_AIPP_API
  NAME
    ai::fmk::model_runtime::direct_static
  SRCS
    direct_built_model_aipp.cpp
)