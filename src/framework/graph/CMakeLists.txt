#cmake_minimum_required(VERSION 3.13...3.20)

hi_add_subdirectory(utils)
hi_add_subdirectory(persistance)

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


# libhiai_ir.so external
hi_cc_library_shared(
  NAME
    ai::fmk::hiai_ir_shared
  WHOLE_STATIC_LIBS
    ai::infra::log
    huawei::c_sec
    google::protobuf_lite_static
    ai::infra::math::fp16_t_static
    ai::fmk::graph::persistance::proto_impl::ge_ir_static
    ai::fmk::graph::persistance::proto_impl_static
    ai::fmk::graph::persistance::static_proxy_static
    ai::fmk::graph::core_static
    ai::fmk::graph::utils_static
    ai::fmk::compatible_static
    ai::fmk::common::helper_static
    ai::fmk::util::tensor::trans_tensor_static

    # ai::fmk::graph::persistance::dynamic_proxy_static
    # dl
)

hi_target_output_name(ai::fmk::hiai_ir_shared hiai_ir)