hi_cc_objects(
  NAME
    ai::fmk::common::helper_objs
  SRCS
    om_file_helper.cpp
)


hi_system(HI_LINUX_TEE
  NAME
    ai::fmk::common::helper_objs
  CDEFS
    USE_STATIC_LIB
    GOOGLE_PROTOBUF_NO_RTTI
  DEPS
    google::protobuf_headers
    huawei::itrustee_basic_headers
    huawei::itrustee_sdk_securec_headers
)

hi_system(HI_ANDROID_AOSP
  NAME
    ai::fmk::common::helper_objs
  SRCS
    model_serialize_wrapper.cpp
  DEPS
    huawei::c_sec
    ai::fmk::graph::proto_shared
)

hi_system(HI_ANDROID_NDK
  NAME
  ai::fmk::common::helper_objs
SRCS
  model_serialize_wrapper.cpp
DEPS
  huawei::c_sec
)


hi_generator(
  NAMES
    ai::fmk::common::helper_objs
)
