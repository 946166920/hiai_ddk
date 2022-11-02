hi_cc_library_static(
  NAME
    ai::fmk::graph::utils_static
  CDEFS
    GRAPH_API_VISIABLE
  SRCS
    *.cpp
    checker/*.cpp
    replacer/*.cpp
)

# hi_cc_system("HI_ANDROID_NDK"
#   NAME
#     ai::fmk::graph::utils_static
#   DEPS
#     huawei::c_sec
# )
