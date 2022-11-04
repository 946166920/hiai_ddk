#cmake_minimum_required(VERSION 3.13...3.20)

hi_add_subdirectory(utils)
hi_add_subdirectory(persistance)

# set(CC_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

# hi_proto_gen(
#   FULL
#   NAME
#     ai::fmk::graph::proto_full_srcs
#   PROTO_PATH
#     ${ROOT_DIR}/inc/common/proto
#   PROTO_SRCS
#     om.proto
#     task.proto
#   PROTO_OUT
#     ${CC_OUT_DIR}
# )

# hi_cc_library_static(
#   NAME
#     ai::fmk::graph::proto_full_static
#   INCS
#     ${CC_OUT_DIR}
#     # ${CC_OUT_DIR}/../
#     ${ROOT_DIR}/third_party/protobuf/src
#   SRCS
#     ai::fmk::graph::proto_full_srcs
#   # DEPS
#   #   ai::fmk::graph::protobuf_full
# )


# # hi_cc_system(HI_ANDROID_NDK
# #   NAME
# #     ai::fmk::graph::proto_full_static
# #   PUB_DEPS
# #     google::protobuf_lite_static
# #     google::protobuf_headers
# # )

# hi_proto_gen(
#   FULL
#   NAME
#     ai::fmk::graph::proto_aipp_srcs
#   PROTO_PATH
#     ${ROOT_DIR}/inc/common/proto
#   PROTO_SRCS
#     insert_op.proto
#   PROTO_OUT
#     ${CC_OUT_DIR}
# )

# hi_cc_library_static(
#   NAME
#     ai::fmk::graph::aipp_proto_static
#   INCS
#     ${CC_OUT_DIR}
#     ${ROOT_DIR}/third_party/protobuf/src
#   SRCS
#     ai::fmk::graph::proto_aipp_srcs
#   # DEPS
#   #   ai::fmk::graph::protobuf_aipp
# )

# # hi_cc_library_shared(
# #   NAME
# #     ai::fmk::graph::proto_shared
# #   COPTS
# #     -fvisibility=default
# #   PUB_DEPS
# #     ai::fmk::graph::proto_full_static
# # )

# # hi_cc_function(AI_SUPPORT_AIPP
# #   NAME
# #     ai::fmk::graph::proto_shared
# #   DEPS
# #     ai::fmk::graph::aipp_proto_static
# # )

hi_cc_library_static(
  NAME
    ai::fmk::graph::core_static
  SRCS
    core/cgraph/*.cpp
    core/edge/*.cpp
    core/node/*.cpp
    core/op/*.cpp
    ./*.cpp
  CDEFS
    GRAPH_API_VISIABLE
)

# hi_cc_system("HI_ANDROID_NDK"
#   NAME
#     ai::fmk::graph::core_static
#   DEPS
#     huawei::c_sec
# )


# libhiai_ir.so external
hi_cc_library_shared(
  NAME
    ai::fmk::hiai_ir_shared
  WHOLE_STATIC_LIBS
    # ai::infra::log
    # huawei::c_sec
    # ai::fmk::graph::persistance::proto_impl::ge_ir_static
    # ai::fmk::graph::core_static
    # ai::fmk::graph::utils_static
    # google::protobuf_lite_static
    # ai::fmk::graph::persistance::proto_impl_static
    # ai::fmk::graph::persistance::static_proxy_static

    # ai::fmk::compatible_static
    # ai::infra::math::fp16_t_static
    # ai::fmk::util::tensor::trans_tensor_static

    # ai::fmk::common::helper_static

    ai::infra::log
    huawei::c_sec
    ai::fmk::graph::persistance::proto_impl::ge_ir_static
    ai::fmk::graph::core_static
    ai::fmk::graph::utils_static
    ai::fmk::compatible_static
    ai::fmk::common::helper_static
    ai::infra::math::fp16_t_static
    ai::fmk::util::tensor::trans_tensor_static
    google::protobuf_lite_static
    ai::fmk::graph::persistance::proto_impl_static
    ai::fmk::graph::persistance::static_proxy_static
)

hi_target_output_name(ai::fmk::hiai_ir_shared hiai_ir)

# # external ddk libhiai_ir.so
# hi_cc_function(STATIC_IR_GRAPH_PROTO
#   NAME
#     ai::fmk::hiai_ir_shared
#   DEPS
#     ai::fmk::graph::persistance::proto_impl_static
#     ai::fmk::graph::persistance::static_proxy_static
# )

# # internal ddk libhiai_ir.so
# hi_function(DYNAMIC_IR_GRAPH_PROTO
#   NAME
#     ai::fmk::hiai_ir_shared
#   DEPS
#     ai::fmk::graph::persistance::dynamic_proxy_objs
# )