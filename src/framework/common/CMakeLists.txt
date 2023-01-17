hi_add_subdirectory(helper)

hi_cc_library_static(
  NAME
    ai::fmk::common::file_util_static
  SRCS
    file_util.cpp
)