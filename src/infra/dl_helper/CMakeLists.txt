#### define ai::infra::dl_helper_objs

hi_cc_library_static(
  NAME
    ai::infra::dl_helper_objs
  SRCS
    dynamic_load_helper.cpp
  DEPS
    huawei::c_sec
  CDEFS
    INFRA_API_VISIABLE
)

# hi_generator(
#   NAMES
#     ai::infra::dl_helper_objs
# )