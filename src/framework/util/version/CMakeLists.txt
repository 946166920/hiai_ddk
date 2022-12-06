hi_cc_library_static(
  NAME
    ai::fmk::util::version_static
  SRCS
    version_util.cpp
  CDEFS
    HIAI_UTIL_API_VISIABLE
)

hi_cc_library_static(
  NAME
    ai::fmk::util::hiai_plugin_version_static
  SRCS
    hiai_plugin_version.c
  CDEFS
    HIAI_C_API_VISIABLE
)
