# This file compiles the source code in third_party/bounds_checking_function to 
# generate libhuawei_c_sec_shared.so.

set(OPEN_SOURCE_CSEC_DIR ${ROOT_DIR}/third_party/bounds_checking_function)

hi_cc_library_static(
  NAME
    huawei::c_sec_static
  SRCS
    ${OPEN_SOURCE_CSEC_DIR}/src/*.c
  CDEFS
    SECUREC_SUPPORT_STRTOLD=1
)

hi_cc_function(AI_SUPPORT_HUAWEI_C_SEC_SHARED
  NAME
    huawei::c_sec_static
  COPTS
    -fvisibility=default
)

hi_target_include_directories(
  NAME
    huawei::c_sec_static
  PUBLIC
    ${OPEN_SOURCE_CSEC_DIR}/include
)

hi_cc_library_shared(
  NAME
    huawei::c_sec_shared
  DEPS
    huawei::c_sec_static
)

hi_target_include_directories(
  NAME
    huawei::c_sec_shared
  PUBLIC
    ${OPEN_SOURCE_CSEC_DIR}/include
)

hi_target_include_directories(
  NAME
    huawei::c_sec_static
  PUBLIC
    ${OPEN_SOURCE_CSEC_DIR}/include
)

set(C_SEC_STATIC ON)
set(C_SEC_SHARED OFF)

hi_cc_library(
  NAME
    huawei::c_sec
  IMPLS
    C_SEC_SHARED huawei::c_sec_shared
    C_SEC_STATIC huawei::c_sec_static
)
