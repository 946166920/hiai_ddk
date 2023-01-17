#### define ai::infra::dl_helper_static
hi_cc_library_static(
  NAME
    ai::infra::dl_helper_static
  SRCS
    dynamic_load_helper.cpp
  CDEFS
    INFRA_API_VISIABLE
)